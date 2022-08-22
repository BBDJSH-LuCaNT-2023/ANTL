#ifndef CLASSGROUP_BSGS_TEST
#define CLASSGROUP_BSGS_TEST

#include "../../catch.hpp"
#include "../../Quadratic/TestData/TestData.hpp"

#include <ANTL/Quadratic/ClassGroup/ClassGroupBSGSReal.hpp>
// #include "../../../src/Quadratic/ClassGroup/ClassGroupBSGSReal.cpp"
#include <ANTL/Quadratic/Regulator/RegulatorLenstra_ZZ.hpp>

using namespace NTL;
using namespace ANTL;

bool DBG_CGBSGSR_TEST = true;

TEST_CASE("ClassGroupReal<ZZ>: Does it work?", "[ClassGroupReal]") {

  extern const std::vector<long> discriminants;
  extern const std::vector<double> correct_regulators;
  extern const std::vector<std::vector<long>> correct_class_groups;

  int correct_count = 0;
  int test_start = 11;
//   int test_bound = correct_class_groups.size();
  int test_bound = 12;

  vector<vector<long>> computed_class_groups;

  std::vector<bool> computed_correctly;

  for (int i = test_start; i < test_bound; i++) {
    std::cout << "computing test " << i << std::endl;
    // Setting up neccessary objects
    QuadraticOrder<ZZ> quad_order{ZZ(discriminants.at(i))};
    QuadraticNumber<ZZ> quad_number1{quad_order};
    QuadraticNumber<ZZ> quad_number2{quad_order};

    MultiplyComp<ZZ> mul_comp_object{};
    mul_comp_object.set_RelativeGenerator(quad_number1);
    quad_order.set_mul_comp(mul_comp_object);

    ReducePlainReal<ZZ> red_plain_real_object{};
    red_plain_real_object.set_RelativeGenerator(quad_number2);
    quad_order.set_red_best(red_plain_real_object);

    L_function<ZZ> l_function;
    l_function.init(ZZ(discriminants.at(i)), 2);

    RegulatorLenstraData<ZZ, double> regulator_lenstra_data{&quad_order, &l_function};

    // Computing h*
    double regulator = correct_regulators.at(i);
    RR h_star_close = to_RR(regulator_lenstra_data.lower_bound_hR()) / to_RR(regulator);
    ZZ h_star = CeilToZZ(h_star_close);

    // Setting up the ClassGroupBSGSReal object
    ClassGroupBSGSReal<ZZ> class_group_bsgs_real1{&quad_order};
    class_group_bsgs_real1.set_regulator(regulator);

    // Computing the class group
    class_group_bsgs_real1.cg_bsgs_real(h_star);

    // Adding computed class group to reslults vector
    vector<ZZ> class_group_ZZ = class_group_bsgs_real1.get_class_group();
    vector<long> class_group_long = {};

    for(auto num : class_group_ZZ) {
      class_group_long.push_back(to<long>(num));
    }

    std::sort(class_group_long.begin(), class_group_long.end());
    std:cout << class_group_long << std::endl;
    computed_class_groups.push_back(class_group_long);

    // Checking for corect output
    if (correct_class_groups.at(i) == computed_class_groups.at(i-test_start)) {
      computed_correctly.push_back(true);
    } else {
      computed_correctly.push_back(false);
    }
  }

  bool test_bool = true;

  if (DBG_CGBSGSR_TEST) {
    std::cout << "CASE" << std::setw(8) << "RESULT" << std::setw(10) << "CORRECT"
              << std::setw(21) << "COMPUTED" << std::setw(21) << "DELTA"
              << std::endl;
    std::cout << std::setw(65) << std::setfill('=') << "" << std::endl;
  }

  for (int i = test_start; i < test_bound; i++) {
    if (DBG_CGBSGSR_TEST) {
      std::cout << std::setfill('0') << std::setw(4) << i + 1
                << std::setfill(' ') << std::setw(6) << computed_correctly.at(i)
                << std::setw(21) << correct_class_groups.at(i)
                << std::setw(21) << computed_class_groups.at(i)
                << std::setw(14) << discriminants.at(i)
                << std::endl;
      if(computed_correctly[i]) {
        correct_count++;
      }
    }

    test_bool = test_bool && computed_correctly.at(i);
  }

  if (DBG_CGBSGSR_TEST) {
    std::cout << correct_count << "/" << test_bound - test_start << " tests passed!" << std::endl;
    for(int i = test_start; i < test_bound; i++) {
      if(!computed_correctly.at(i)){
        std::cout << "case " << i << " was wrong!" << std::endl;
      }
    }
  }


  REQUIRE(test_bool == true);
}

#endif
