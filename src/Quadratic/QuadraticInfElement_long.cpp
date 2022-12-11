#include <ANTL/Quadratic/QuadraticInfElement.hpp>

using namespace ANTL;

template <class S> class QuadraticInfElement<long, S> {
private:
  QuadraticIdealBase<long> qib;
  S Distance;

  long Delta;
  long FloorRootDelta;

public:
  QuadraticInfElement();
  QuadraticInfElement(QuadraticOrder<long> &quad_o) : qib{quad_o} {
  qib.assign_one();
  Delta = quad_o.get_discriminant();
  FloorRootDelta = to_long(FloorToZZ(sqrt(to_RR(Delta))));
  Distance = to<S>(0);
}

  ~QuadraticInfElement();

  QuadraticIdealBase<long> get_qib() const;

  RR get_distance() const;

  // Friend functions for arithmetic
  friend void mul<ZZ, S>(QuadraticInfElement<long, S> qie_a,
                         QuadraticInfElement<long, S> qie_b);
  friend void mul<ZZ, S>(QuadraticInfElement<long, S> qie_a,
                         QuadraticInfElement<long, S> qie_b,
                         QuadraticInfElement<long, S> qie_c);

  friend void sqr<ZZ, S>(QuadraticInfElement<long, S> qie_a,
                         QuadraticInfElement<long, S> qie_b);
  friend void sqr<ZZ, S>(QuadraticInfElement<long, S> qie_a,
                         QuadraticInfElement<long, S> qie_b,
                         QuadraticInfElement<long, S> qie_c);

  // Infrastructure methods
  void baby_step() {
    long a, b, c, q, r, R, Q, P;

    a = qib.get_a();
    b = qib.get_b();
    c = qib.get_c();

    DivRem(q, r, b + FloorRootDelta, 2 * a);

    R = -a;
    P = FloorRootDelta - r;
    Q = q * ((b - P) / 2) - c;

    qib.assign(Q, P, R);
    qib.normalize();
    Distance +=
        to<S>((inv(abs((to_RR(P) - sqrt(to_RR(Delta))) / to_RR(2 * Q)))));
  }

  void giant_step(QuadraticInfElement<long, S> &quad_ib) {

    if (quad_ib == *this) {
      sqr(qib, qib);
    }

    else {
      mul(qib, qib, quad_ib.get_qib());
    }

    Distance += quad_ib.get_distance();

    // the correcting factor below needs to go in each case above
/*
    Distance +=
        log(qib.get_QO()->get_mul_nucomp()->get_RelativeGenerator()->conv_RR());
*/
  }

  void adjust(const ZZ &a) {

    S bound = to<S>(a);

    if (Distance > max) {
      while (Distance > max) {
        inverse_rho();
      }
    }

    else {
      while (Distance <= max) {
        baby_step();
      }
      inverse_rho();
    }
    return;
  }

  void assign(const HashEntryReal<long, S>);

  void assign_one();

  QuadraticInfElement<long, S> conjugate() const;

  ZZ eval();

  HashEntryReal<long, S> hash_real() const;

  bool is_one();

  void inverse_rho() {
    long a, b, c, q, a2, r, temp, nb, oa;

    a = qib.get_a();
    b = qib.get_b();
    c = qib.get_c();

    oa = a;
    a = -c;
    a2 = a << 1;

    // q = floor((rootD + b) / 2a)
    temp = FloorRootDelta + b;
    DivRem(q, r, temp, a2);
    if (temp < 0 && !IsZero(r)) {
      --q;
    }

    // d -= ln( (b + rd) / 2a )
    // distance.multiply_inverse_rho(q, b, oa << 1);

    Distance -= to<S>(log(inv(abs((to_RR(b) - sqrt(to_RR(Delta))) / to_RR(2 * oa)))));

    // b = 2aq - b
    nb = FloorRootDelta - r;

    // c =  q*((nb - b)/2) - c
    c = q * ((nb - b) >> 1) - oa;

    b = nb;
    if (a < 0) {
      a = -a;
      c = -c;
    }

    qib.assign(a, b, c);
  }

  RR get_baby_steps(IndexedHashTable<HashEntryReal<long, S>> &prin_list, const ZZ &B,
                    const QuadraticInfElement<long, S> &A);

  RR get_baby_steps(IndexedHashTable<HashEntryReal<long, S>> &prin_list, const ZZ &B,
                    const QuadraticInfElement<long, S> &A, long l, long &M);

};
