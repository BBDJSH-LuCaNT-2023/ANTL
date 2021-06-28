#ifndef QUADRATICORDER_ZZ_TEST
#define QUADRATICORDER_ZZ_TEST

#include "../catch.hpp"
#include <ANTL/Quadratic/QuadraticOrder.hpp>

using namespace NTL;
using namespace ANTL;

TEST_CASE("Miscellaneous tests for QuadraticOrder<ZZ>", "[capturing]") {

    ZZ D1 = ZZ(13);
    ZZ D2 = ZZ(17);

    QuadraticOrder<ZZ> quad_order1 = QuadraticOrder<ZZ>(D1);
    QuadraticOrder<ZZ> quad_order2 = QuadraticOrder<ZZ>(D1);

    QuadraticOrder<ZZ> quad_order3 = QuadraticOrder<ZZ>(D2);

    ZZ expected_discriminant = ZZ(13);

    REQUIRE(quad_order1 == quad_order2);

    REQUIRE(quad_order1 != quad_order3);

    REQUIRE(quad_order1.getDiscriminant() == expected_discriminant);

    REQUIRE(quad_order1.IsReal());

    REQUIRE_FALSE(quad_order1.IsImaginary());
}


#endif
