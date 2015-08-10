/**
 * @file ExponentiationWNAF.hpp
 * @author Michael Jacobson/Reginald Lybbert
 * @brief class for left-to-right WNAF exponentiation
 */

#ifndef DEXPONENTIATION_IL_H
#define DEXPONENTIATION_IL_H

#include <vector>
#include <ANTL/DExponentiation/DExponentiation.hpp>

namespace ANTL
{

  /**
   * @brief class for left-to-right interleaving double exponentiation
   * @remarks This concrete class defines a method for left-to-right interleaving
   * double exponentiation.  The method first computes the WNAF representation of the
   * exponents using the basic right-to-left method, and then applies it to the bases
   * left-to-right. The base type is templated, and must have the following
   * functions defined:
   *   - assign(T &C, const T &A): sets C = A
   *   - mul(T &C, const T &A, const T &B): computes C = AB
   *   - sqr(T &C, const T &A): computes C = AA
   *   - inv(T &C, const T &A): computes C = inverse of A
   */
  template < class T >
  class DExponentiationIL : public DExponentiation<T> 
  {

  protected:
    vector<short> em;  /**< vector containing WNAF expansion of m */
    vector<short> en;  /**< vector containing WNAF expansion of n */
    T * precompA;      /**< array containing precomputed values, computed from A */
    T * precompAInv;   /**< array containing the inverses of the values in precompA */
    T * precompB;      /**< array containing the precomputed values, computed from B */
    T * precompBInv;   /**< array containing the inverse of the values in precompB */
    short wm;	      /**< width of WNAF representation of m */
    short wn;         /**< width of WNAF representation of n */

  public:
    DExponentiationIL() {};
    ~DExponentiationIL() {};

    /**
     * @brief Setup the class to compute A^mB^n.  Computes necessary precomputed values of A and B and allocates a digit vector
     * of size NumBits(n)+1
     * @param[in] A first base for exponentiation
     * @param[in] B second base for exponentiation
     * @param[in] m first exponent
     * @param[in] n second exponent
     * @param[in] w0 windowing width for m
     * @param[in] w1 windowing width for n
     *
     * @pre m,n >= 0
     * @pre w0,w1 >= 2
     */
    void initialize(const T &A, const T &B, const ZZ &m, const ZZ &n, const short w0, const short w1);

    /**
     * @brief Computes A^mB^n
     * @param[out] C result of computing A^m^n using left-to-right interleaving double exponentiation
     * @param[in] A first base for exponentiation
     * @param[in] B second base for exponentiation
     * @param[in] m first exponent
     * @param[in] n second exponent
     *
     * @pre n >= 0
     */
    void power (T &C, const T &A, const T &B, const ZZ &m, const ZZ &n);

  };

} // ANTL

// Unspecialized template definitions.
#include "../../../src/DExponentiation/DExponentiationIL_impl.hpp"

#endif // DEXPONENTIATION_IL_H
