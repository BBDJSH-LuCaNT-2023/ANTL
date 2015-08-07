/**
 * @file zz_p_expo_test.cpp
 * @author Michael Jacobson
 * @brief Test program for exponentiation routines using zz_p as base type.
 */

#include <NTL/lzz_p.h>
#include <ANTL/Exponentiation/ExponentiationBinary.hpp>
#include <ANTL/Exponentiation/ExponentiationNAF.hpp>

NTL_CLIENT
using namespace ANTL;
	
int main (int argc, char **argv)
{
  zz_p a,b_bin,b_naf;
  ZZ n;

  // use GF(1073741827) for these tests
  zz_p::init(1073741827);

  // set base to be a random value in GF(5)
  do {
    random(a);
  } while (IsOne(a));


  // generate random exponent of size 128 bits
  RandomLen (n, 512);

  cout << "Using:" << endl;
  cout << " p = " << zz_p::modulus() << endl;
  cout << " a = " << a << endl;
  cout << " n = " << n << endl;

  // initialize exponentiation classes
  ExponentiationBinary<zz_p> ebin;
  ExponentiationNAF<zz_p> enaf;

  // compute a^n with available methods
  ebin.power(b_bin,a,n);

  enaf.initialize(a,n);
  enaf.power(b_naf,a,n);

  // check and output results
  cout << "a^n (binary) = " << b_bin << endl;
  cout << "a^n (naf)    = " << b_naf << endl;

  if (b_bin == b_naf)
    cout << "RESULTS MATCH!" << endl;
  else
    cout << "ERROR:  RESULTS DO NOT MATCH!" << endl;
}
