/**
 * @file qo_reduce_plain_long.cpp
 * @author Michael Jacobson
 * @remark Basic ideal reduction specialization (long base type).
 */

#include <ANTL/Quadratic/Reduce/ReducePlainReal.hpp>

// reduce
//
// Task: reduces the ideal
template <> void ReducePlainReal<long>::reduce(QuadraticIdealBase<long> & A) {
  static long a, b, c;

  // normalize ideal
  if (!A.is_normal()) {
    A.normalize();
  }

  // reduce
  while (!A.is_reduced()) {
    a = A.get_a();
    b = A.get_b();
    c = A.get_c();

    A.assign(c, -1*b, a);
    A.normalize();
  }

  //account for special case
  if ((a == c) && (b < 0)) {
    b = -b;
    A.set_b(b);
  }
}