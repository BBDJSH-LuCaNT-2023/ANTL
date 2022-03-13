#ifndef REGULATOR_LENSTRA_COMPUTE_H
#define REGULATOR_LENSTRA_COMPUTE_H

#include <ANTL/Quadratic/QuadraticInfElement.hpp>
#include <ANTL/Quadratic/QuadraticOrder.hpp>
#include <ANTL/Quadratic/Regulator/RegulatorLenstraData.hpp>
#include <ANTL/common.hpp>

NTL_CLIENT
using namespace ANTL;

namespace ANTL {

template <class T> class RegulatorLenstraCompute {
public:
  RR regulator_lenstra(RegulatorLenstraData<T> &rl_data);

private:
  ZZ estimate_hR_error(RegulatorLenstraData<T> &rl_data);

  long get_optimal_Q_cnum(RegulatorLenstraData<T> &rl_data);

  long bsgs_getl(RegulatorLenstraData<T> &rl_data, const ZZ &K, ZZ &N,
                 ZZ &entry_size, RR &mu, bool nodist);

  RR get_mu(const T &delta);

  void bsgs_getentrysize(RegulatorLenstraData<T> &rl_data, ZZ &entry_size,
                         bool nodist);
};

// START: Forward Declaring template specializations

template <>
long RegulatorLenstraCompute<ZZ>::get_optimal_Q_cnum(
    RegulatorLenstraData<ZZ> &rl_data);

template <> RR RegulatorLenstraCompute<ZZ>::get_mu(const ZZ &delta);

template <>
void RegulatorLenstraCompute<ZZ>::bsgs_getentrysize(
    RegulatorLenstraData<ZZ> &rl_data, ZZ &entry_size, bool nodist);

// FINISH:Forward Declaring template specializations

// Method definitions - Everything below will eventually go into a
// RegulatorLenstraCompute_impl.hpp file.

// RegulatorLenstraCompute<T>::regulator_lenstra
// Task: Computes the regulator using an O(D^1/5) baby-step giant-step algorithm
// of Shanks and improvements of Lenstra.

template <class T>
RR RegulatorLenstraCompute<T>::regulator_lenstra(
    RegulatorLenstraData<T> &rl_data) {

  //
  // initialize hash table
  //

  RR regulator;
  ZZ K, N, B, entry_size, u, s, s2;
  long l = 1, M = 1;
  RR mu;
  QuadraticInfElement<T> AA, G;

  qo_distance<T> S;
  clear(S);

  K = estimate_hR_error(rl_data.get_quadratic_order()) >> 1;
  l = bsgs_getl(rl_data, K, N, entry_size, mu, false);
  init_prinlist(N, l, s, M, G);
  B = N * l;
  if (IsZero(B)) {
    regulator_bsgs(B);
    S = regulator;
  }

  if (IsZero(S)) {
    //
    // compute approximation of hR
    //

    ZZ E = approximate_hR();
    nuclose(AA, E);

    if (AA.is_one())
      S = AA.get_distance();
  }

  //
  // compute list of baby steps (distance < B)
  //

  QuadraticInfElement<T> A, C, CC, D, DD, GG;
  qo_hash_entry_real<T> *F;

  if (IsZero(S)) {

    A.assign_one();
    if (l == 1)
      regulator = G.get_baby_steps(prin_list, B, A);
    else
      regulator = G.get_baby_steps(prin_list, B, A, l, M);

    if (!IsZero(R)) {
      nuclose(C, R.eval());
      regulator = C.get_distance();
      Rbsgs = true;
    }

    prinlist_M = M;

    if (IsZero(regulator)) {
      G.adjust(s);

      u = (s << 1);
      nudupl(G, G);
      G.adjust(u);
      if (G.is_one())
        G.rho();

      GG = conjugate(G);
      while (abs(GG.get_distance().eval()) <= u)
        GG.inverse_rho();
      while (abs(GG.get_distance().eval()) > u)
        GG.rho();

      s2 = s = AA.get_distance().eval();
      C = AA;
      D = AA;
    }
  }

  //
  // compute giant steps until R is found or the bound is exceeded
  //

  long i;

  while (IsZero(regulator) && IsZero(S)) {

    // search for C, rho_1(C), ..., rho_l(C) in the hash table
    CC = C;
    DD = D;
    for (i = 0; i < M && IsZero(S); ++i) {
      F = prin_list.search(CC.hash_real());
      if (F) {
        // found CC in the hash table!

        combine_BSGS(S, CC, F);
      } else {
        F = prin_list.search((-CC).hash_real());
        if (F) {
          // found CC^-1 in the hash table!

          combine_conj_BSGS(S, CC, F);
        } else {
          F = prin_list.search(DD.hash_real());
          if (F) {
            // found DD in the hash table!

            combine_BSGS(S, DD, F);
          } else {
            F = prin_list.search((-DD).hash_real());
            if (F) {
              // found DD^-1 in the hash table!

              combine_conj_BSGS(S, DD, F);
            } else if (i < M) {
              CC.rho();
              DD.rho();
            }
          }
        }
      }
    }

    if (IsZero(S)) {
      s += u;
      nucomp(C, C, G);
      C.adjust(s);

      s2 -= u;
      nucomp(D, D, GG);
      D.adjust(s2);
    }
  }

  //
  // factor out h*
  //

  if (IsZero(regulator)) {

    // verify that Regulator > S^2/3 (= B) using BS-GS
    ZZ B, N, entry_size;
    RR mu;
    long l;
    RR temp = log(to_RR(S.eval())) * to_RR(2) / to_RR(3);

    conv(B, ceil(exp(temp)));

    l = bsgs_getl(B, N, entry_size, mu, true);
    optimize_K(B, S.eval(), N, l);

    regulator_bsgs(B);

    ZZ hstar, Pmax;

    if (info > 1 && !IsZero(regulator)) {
      hstar = S.eval() / R.eval();

      cout << "Found R = " << regulator << endl;
      cout << "h* = " << hstar << endl;
    } else {
      Pmax = 1 + S.eval() / B;
      find_hstar(hstar, S.eval(), Pmax, t1);
    }

    nuclose(C, R.eval());
    regulator = C.get_distance();
  }
}

// RegulatorLenstraCompute<ZZ>::estimate_hR_error(RegulatorLenstraData<ZZ>
// &rl_data)
// Task: returns L such that |hR - hR'| < exp(L)^2

template <>
ZZ RegulatorLenstraCompute<ZZ>::estimate_hR_error(
    RegulatorLenstraData<ZZ> &rl_data) {
  ZZ err, delta = rl_data.get_delta();
  RR Aval, Fval, temp;

  if (rl_data.get_l_function().terms_used(1) == 0)
    return ZZ::zero();

  long n = get_optimal_Q_cnum(rl_data);
  RR FI = rl_data.get_l_function().approximateL1(n);

  Aval = rl_data.get_l_function().calculate_L1_error(
      delta, rl_data.get_l_function().terms_used(1));
  Fval = exp(Aval) - 1;
  temp = 1 - exp(-Aval);
  if (temp > Fval)
    Fval = temp;

  if (rl_data.get_quadratic_order().is_imaginary()) {
    // h = sqrt(delta) L / Pi
    Fval *= FI * SqrRoot(to_RR(-delta)) / ComputePi_RR();
    if (delta == -4)
      temp *= 2;
    if (delta == -3)
      temp *= 3;
  } else {
    // hR = sqrt(delta) L / 2
    Fval *= FI * SqrRoot(to_RR(delta)) / 2;
  }

  err = CeilToZZ(Fval * log(to_RR(2))) >> 1;
  return err;
}

template <>
long RegulatorLenstraCompute<ZZ>::get_optimal_Q_cnum(
    RegulatorLenstraData<ZZ> &rl_data) {
  long dlog;
  ZZ temp;

  temp = FloorToZZ(
      log10(to_RR(abs(rl_data.get_quadratic_order().get_discriminant()))));
  conv(dlog, temp);

  return rl_data.get_OQvals_cnum_entry((dlog / 5));
}

template <class T>
long RegulatorLenstraCompute<T>::bsgs_getl(RegulatorLenstraData<T> &rl_data,
                                           const ZZ &K, ZZ &N, ZZ &entry_size,
                                           RR &mu, bool nodist) {

  T delta = rl_data.get_delta();
  ZZ max_memory = rl_data.get_max_memory();
  bool parallel = rl_data.get_parallel;

  ZZ maxN, rootK;
  RR n, temp;
  long l;

  // get mu (# baby steps per giant step)
  mu = get_mu(delta);

  // n = number of slave processes
  if (parallel)
    n = parallel;
  else
    set(n);

  // compute max number of baby steps to store
  bsgs_getentrysize(entry_size, nodist);
  maxN = 1 + (max_memory / (to_ZZ(n) * entry_size));

  // compute number of baby steps assuming l=1, N=sqrt(KG/2n)
  temp = floor(SqrRoot(to_RR(K) * mu / (to_RR(1) * n)));
  conv(N, temp);
  l = 1;

  if (N > maxN) {
    // compute l = sqrt(KG/2n) / N;
    //    N = maxN;
    entry_size *= 3;
    entry_size >>= 1;
    N = 1 + (max_memory / (to_ZZ(n) * entry_size));
    temp = floor((SqrRoot(to_RR(K) * mu / (to_RR(2) * n)) / to_RR(N)));
    conv(l, temp);
    if (l < 1)
      l = 1;
  }

  return l;
}

// quadratic_order<ZZ>::get_mu()
// Task: determines the number of baby-steps which can be done in the time of
// one giant step.  These values are read in from a table (file) which is
// computed at compile-time.

template <> RR RegulatorLenstraCompute<ZZ>::get_mu(const ZZ &delta) {
  //  return to_RR(2)*((to_RR(NumBits(delta) - 5) / to_RR(10)) + to_RR(6));
  //  return (to_RR(NumBits(delta) - 5) / to_RR(10)) + to_RR(6);
  return to_RR(6.85) + to_RR(10.62) * to_RR(NumBits(delta)) / to_RR(135);
}

template <>
void RegulatorLenstraCompute<ZZ>::bsgs_getentrysize(
    RegulatorLenstraData<ZZ> &rl_data, ZZ &entry_size, bool nodist) {
  ZZ delta = rl_data.get_delta();
  ZZ temp;

  entry_size = 3 * NTL_BITS_PER_LONG; // space for pointers
  if (nodist) {
    entry_size += SqrRoot(delta).size() * NTL_ZZ_NBITS; // a coeff
    entry_size += 8;                                    // b coeff
  } else {
    // temp = to_ZZ(1) << qo_distance<ZZ>::get_p();
    temp = to_ZZ(1) << 64;
    entry_size +=
        temp.size() * NTL_ZZ_NBITS + 3 * SqrRoot(delta).size() * NTL_ZZ_NBITS;
  }

  // convert bits to bytes
  entry_size >>= 2;
}

// quadratic_order<T>::init_prinlist
// Task: initializes the list of reduced principal ideals for BS-GS
// computations. Uses current contents if appropriate.

template <class T>
void RegulatorLenstraCompute<T>::init_prinlist(const ZZ &N, long l, ZZ &s, long &M,
                                       qi_pair<T> &G) {
  long lsize, P;

  // compute B and s
  P = 3 + qi_pair<T>::size_rd();
  s = N * l - P;

  // initialize hash table
  if (prin_list.no_of_elements() > 0) {
    G.assign(prin_list.last_entry());
    l = prinlist_l;
    M = prinlist_M;
    if (prinlist_s > s)
      s = prinlist_s;
    else
      prinlist_s = s;
  } else {
    conv(lsize, N + P);
    lsize += 100;
    prin_list.initialize(lsize);
    G.assign_one();
    prinlist_l = l;
    prinlist_s = s;
  }
}

} // namespace ANTL
#endif
