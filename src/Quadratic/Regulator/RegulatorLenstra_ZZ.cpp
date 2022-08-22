#include <ANTL/Quadratic/Regulator/RegulatorLenstra_ZZ.hpp>

template <> void RegulatorLenstraData<ZZ, double>::regulator_lenstra() {

//   std::cout<< "hello there! you are using <ZZ, double> !" << std::endl;
  //
  // initialize hash table
  //
  ZZ K, N, B, entry_size;
  double u, s, s2;
  long l = 1, M = 1;
  RR mu;
  QuadraticInfElement<ZZ, double> AA{*quadratic_order}, G{*quadratic_order};

  double S;
  // qo_distance<ZZ> S;
  clear(S);
  clear(regulator);

  double E = approximate_hR();

  K = estimate_hR_error() >> 1;

  if (DBG_LENSTR) {
    std::cout << "LENSTR: K is " << K << std::endl;
  }

  // l is baby-step increment for which are stored in the table
  l = bsgs_getl(K, N, entry_size, mu, false);

  init_prinlist(N, l, s, M, G);

  if (DBG_LENSTR) {
    std::cout << "LENSTR: s is " << s << std::endl;
    std::cout << "LENSTR: N is " << N << std::endl;
    std::cout << "LENSTR: l is " << l << std::endl;
  }

  B = N * l;
  if (DBG_LENSTR) {
    std::cout << "LENSTR: B is " << B << std::endl;
  }

  if (IsZero(B)) {
    regulator_bsgs(B);
    S = regulator;
    if (!IsZero(regulator)) {
      set_case_type(" -- Initial regulator_bsfs -- ");
    }
  }

  if (DBG_LENSTR) {
    std::cout << "LENSTR: regulator is " << regulator << std::endl;
    std::cout << "LENSTR: S is " << S << std::endl;
    std::cout << "LENSTR: E is " << E << std::endl;
  }

  if (IsZero(S)) {
    //
    // compute approximation of hR
    //

    nuclose(AA, FloorToZZ(E));
    AA.adjust(E);

    if (AA.is_one()) {
      S = AA.get_distance();
      if (DBG_LENSTR) {
        std::cout << "LENSTR: AA.is_one() - setting S!" << std::endl;
        std::cout << "LENSTR: S is " << S << std::endl;
      }
    }
  }

  //
  // compute list of baby steps (distance < B)
  //
  QuadraticInfElement<ZZ, double> A{*quadratic_order}, C{*quadratic_order},
      C_gap_check{*quadratic_order}, D{*quadratic_order},
      D_gap_check{*quadratic_order}, GG{*quadratic_order}, C1{*quadratic_order},
      D1{*quadratic_order};
  HashEntryReal<ZZ, double> *F;

  if (IsZero(S)) {

    A.assign_one();
    if (l == 1) {
      regulator = G.get_baby_steps(prin_list, B, A);
      if (DBG_LENSTR) {
        std::cout << "LENSTR: G.get_baby_steps(prin_list, B, A) " << std::endl;
        std::cout << "LENSTR: B is " << B << std::endl;
      }
    } else {
      regulator = G.get_baby_steps(prin_list, B, A, l, M);
      if (DBG_LENSTR) {
        std::cout << "LENSTR: G.get_baby_steps(prin_list, B, A, l, M) "
                  << std::endl;
      }
    }

    if (DBG_LENSTR) {
        std::cout << "LENSTR: prin_list is " << prin_list << std::endl;
    }

    if (!IsZero(regulator)) {
      Rbsgs = true;
    }

    prinlist_M = M;

    if (IsZero(regulator)) {
      G.adjust(s);
      if (DBG_LENSTR) {
        std::cout << "LENSTR: G.adjust(s) " << std::endl;
        std::cout << "LENSTR: s is " << s << std::endl;
      }
      u = 2 * s;
      if (DBG_LENSTR) {
        std::cout << "LENSTR: u is " << u << std::endl;
      }
      G.giant_step(G);
      if (DBG_LENSTR) {
        std::cout << "LENSTR: G.giant_step(G) " << std::endl;
      }
      // sqr(G, G); makeshift square above
      G.adjust(u);
      if (DBG_LENSTR) {
        std::cout << "LENSTR: G.adjust(s) " << std::endl;
      }
      if (G.is_one()) {
        G.baby_step();
        if (DBG_LENSTR) {
          std::cout << "LENSTR: G.baby_step() " << std::endl;
        }
      }

      conjugate(GG, G);
      if (DBG_LENSTR) {
        std::cout << "LENSTR: conjugate(GG, G) " << std::endl;
      }
      // return;
      while (abs(GG.eval()) <= u) {
        GG.inverse_rho();
        if (DBG_LENSTR) {
          std::cout << "LENSTR: GG.inverse_rho() " << std::endl;
        }
      }
      while (abs(GG.eval()) > u) {
        if (DBG_LENSTR) {
          std::cout << "LENSTR: GG.eval()) > u" << std::endl;
        }

        if (DBG_LENSTR) {
          std::cout << "LENSTR: GG is (" << GG.get_qib().get_a() << ", "
                    << GG.get_qib().get_b() << " , " << GG.get_qib().get_c()
                    << ") with distance " << GG.get_distance() << std::endl;
          std::cout << "LENSTR: u is " << u << std::endl;
        }

        GG.baby_step();
        if (DBG_LENSTR) {
          std::cout << "LENSTR: GG.baby_step() " << std::endl;
        }
      }

      if (DBG_LENSTR) {
        std::cout << "AA is " << AA.get_qib() << " with distance "
                  << AA.get_distance() << std::endl;
      }
      s2 = s = AA.get_distance();
      C = AA;
      D = AA.conjugate();
    }

    else {
      set_case_type(" -- baby_step list construction -- ");
    }
  }

  if (DBG_LENSTR) {
    std::cout << "LENSTR: regulator is " << regulator << std::endl;
    std::cout << "LENSTR: S is " << S << std::endl;
  }

  //
  // compute giant steps until R is found or the bound is exceeded
  //

  // Saving initial ideals
  C1 = C;
  D1 = D;

  // Perform giant-steps on both C and D until one is found in the
  // baby-step list
  while (IsZero(regulator) && IsZero(S)) {

    if (DBG_LENSTR) {
      std::cout << "C distance is " << C.get_distance() << std::endl;
      std::cout << "D distance is " << D.get_distance() << std::endl;
    }

    C_gap_check = C;
    D_gap_check = D;

    // If there are gaps of M baby-steps in our baby-step list, we must perform
    // M baby-steps on C and D and check for each of these in the list
    for (long i = 0; i < M && IsZero(S); ++i) {

      F = prin_list.search(C_gap_check.hash_real());
      if (F) {

        // found C in the hash table!
        set_case_type(" -- C found -- ");
        if (DBG_LENSTR) {
          std::cout << "found C in the hash table!" << std::endl;
        }

        combine_BSGS(S, C_gap_check, F);
        break;
      }

      F = prin_list.search(D_gap_check.hash_real());
      if (F) {

        // found D in the hash table!
        set_case_type(" -- D found -- ");
        if (DBG_LENSTR) {
          std::cout << "found D in the hash table!" << std::endl;
        }

        // combine_conj_BSGS(S, D_gap_check, F);
        S = AA.get_distance() -
            ((D_gap_check.get_distance() - D1.get_distance()) - F->get_d()) -
            log(AA.get_qib().get_a());
        break;
      }

      C_gap_check.baby_step();
      D_gap_check.baby_step();
    }

    if(abs(S) < 1) {
      S = 0;
    }

    // If nothing has been found, S will still be zero; Continue with giant
    // steps
    if (IsZero(S)) {
      s += u;
      s2 -= u;

      // mul(C, C, G); makeshift multiplication below
      C.giant_step(G);
      C.adjust(s);

      // mul(D, D, GG); makeshift multiplication below
      D.giant_step(G);
      // D.adjust(s2);
    }
  }


  if (DBG_LENSTR) {
    std::cout << "LENSTR: regulator is " << regulator << std::endl;
    std::cout << "LENSTR: S is " << S << std::endl;
  }

  //
  // factor out h*
  //
  if (S < 0) {
    S *= -1;
  }
  if (IsZero(regulator)) {

    if (DBG_LENSTR) {
      std::cout << "LENSTR: Factoring out h* " << std::endl;
      std::cout << "LENSTR: regulator is " << regulator << std::endl;
      std::cout << "LENSTR: S is " << S << std::endl;
    }

    // verify that Regulator > S^2/3 (= B) using BS-GS
    ZZ B, N, entry_size;
    RR mu;
    long l;
    if (DBG_LENSTR) {
      std::cout << "LENSTR: Before log(S), S is " << S << std::endl;
    }
    RR temp = log(to_RR(S)) * to_RR(2) / to_RR(3);

    if (DBG_LENSTR) {
      std::cout << "LENSTR: After log(S)" << std::endl;
      std::cout << "LENSTR: Before conv(B, ceil(exp(temp)))" << std::endl;
    }

    conv(B, ceil(exp(temp)));
    if (DBG_LENSTR) {
      std::cout << "LENSTR: After conv(B, ceil(exp(temp)))" << std::endl;
    }

    l = bsgs_getl(B, N, entry_size, mu, true);

    if (DBG_LENSTR) {
      std::cout << "LENSTR: B is " << B << std::endl;
      std::cout << "LENSTR: FloorToZZ(S) is " << FloorToZZ(S) << std::endl;
      std::cout << "LENSTR: N is " << N << std::endl;
      std::cout << "LENSTR: l is " << l << std::endl;
    }

    // THE BELOW LINE SHOULD EVENTUALLY BE UNCOMMENTED
    //     optimize_K(B, FloorToZZ(S), N, l);

    if (DBG_LENSTR) {
      std::cout << "LENSTR: After optimize_K(B, FloorToZZ(S), N, l)"
                << std::endl;
      std::cout << "LENSTR: B is " << B << std::endl;
    }

    // THE BELOW LINE SHOULD EVENTUALLY BE UNCOMMENTED
    B = CeilToZZ(E / SqrRoot(to_RR(K)));
    regulator_bsgs(B);
    if(!IsZero(regulator)){
      set_case_type(" -- second regulator_bsgs -- ");
    }

    if (DBG_LENSTR) {
      std::cout << "LENSTR: After regulator_bsgs(B) regulator is " << regulator
                << std::endl;
      std::cout << "LENSTR: S is " << S << std::endl;
    }

    ZZ Pmax;
    if (!IsZero(regulator)) {
      hstar = (FloorToZZ(log(S) / log(RR(2)))) /
              (FloorToZZ(log(regulator) / log(RR(2))));
      if (DBG_LENSTR) {
        std::cout << "Found R = " << regulator << std::endl;
        std::cout << "h* = " << hstar << std::endl;
      }
    } else {
      Pmax = 1 + CeilToZZ(to_RR(S) / to_RR(B));
      //       Pmax = FloorToZZ(sqrt(S));
      if (DBG_LENSTR) {
        std::cout << "LENSTR: Pmax is " << Pmax << std::endl;
      }
      find_hstar(hstar, S, Pmax);
    }

    if (DBG_LENSTR) {
      std::cout << "LENSTR: Factoring out h* - S is " << S << std::endl;
      std::cout << "LENSTR: Factoring out h* - hstar is " << hstar << std::endl;
      std::cout << "LENSTR: Factoring out h* - regulator is " << regulator
                << std::endl;
      std::cout << "LENSTR: Factoring out h* - nuclose start" << std::endl;
    }

    nuclose(C, FloorToZZ(regulator));
    C.adjust(regulator);
//     C.adjust_to_one();

    if (DBG_LENSTR) {
      std::cout << "LENSTR: Factoring out h* - nuclose finish" << std::endl;
      std::cout << "LENSTR: Factoring out h* - assign regulator" << std::endl;
    }

    regulator = C.get_distance();
  }

  if (DBG_LENSTR) {
    std::cout << "LENSTR: Factoring out h* - END S is" << S << std::endl;
    std::cout << "LENSTR: Factoring out h* - END regulator is" << regulator
              << std::endl;
  }
}