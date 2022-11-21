/**
 * @file iq-data.cpp
 * @author Shantha Ramachandran
 */

#include "iq-data.hpp"

// constructor
iq_data::iq_data() {
  std::cout << "hello i am the pain is your ass that youve been feeling" << std::endl;
  // initialize data parameters
  int i, j, e1, e2, e3, e4, e5, e6;

  maxD = 0;
  total0 = total1 = 0;
  maxp = localmaxp = 0;
  maxh0 = maxh1 = 0;
  maxpD = localmaxpD = maxh0D = maxh1D = 0;
  maxhodd0 = maxhodd1 = 0;
  maxhodd0D = maxhodd1D = 0;

  // 1. L(1,X)
  minL0 = minL1 = minL5 = (double)1000000.0;
  maxL0 = maxL1 = maxL5 = (double)0.0;
  minLLI0 = minLLI1 = minLLI5 = (double)1000000.0;
  maxULI0 = maxULI1 = maxULI5 = (double)0.0;
  minLLI0D = minLLI1D = minLLI5D = 0;
  maxULI0D = maxULI1D = maxULI5D = 0;
  clear(sumL0);
  clear(sumL1);

  // 3a. noncyclic
  noncyc0 = noncyc1 = 0;

  // 2. divisibility
  for (i = 0; i < MAXP; ++i)
    for (j = 0; j < MAXPSMALL; ++j) {
      pdivs0[i][j] = 0;
      pdivs1[i][j] = 0;
    }

  // 3b. p-rank & 4. first occurences
  for (i = 0; i < MAXPLARGE; ++i)
    for (e1 = 0; e1 < MAXE1; ++e1)
      for (e2 = 0; e2 < MAXE2; ++e2)
        for (e3 = 0; e3 < MAXE3; ++e3)
          for (e4 = 0; e4 < MAXE4; ++e4)
            for (e5 = 0; e5 < MAXE5; ++e5)
              for (e6 = 0; e6 < MAXE6; ++e6) {
                prank0[i][e1][e2][e3][e4][e5][e6] = 0;
                prank1[i][e1][e2][e3][e4][e5][e6] = 0;
                first_prank0[i][e1][e2][e3][e4][e5][e6] = 0;
                first_prank1[i][e1][e2][e3][e4][e5][e6] = 0;
              }

  for (e1 = 0; e1 < MAXP1; ++e1)
    for (e2 = 0; e2 < MAXP2; ++e2)
      for (e3 = 0; e3 < MAXP3; ++e3)
        for (e4 = 0; e4 < MAXP4; ++e4) {
          two_noncyc0[e1][e2][e3][e4] = two_noncyc1[e1][e2][e3][e4] = 0;
          first_two_noncyc0[e1][e2][e3][e4] =
              first_two_noncyc1[e1][e2][e3][e4] = 0;
        }

  // 5. generators
  maxlograt = maxloglograt = (double)0.0;
  maxlogratD = maxloglogratD = 0;
  clear(sumlograt);
  clear(sumloglograt);

  for (i = 0; i < MAXPLARGE; i++) {
    psplit0[i] = 0;
    psplit1[i] = 0;
    kpsplit0[i] = 0;
    kpsplit1[i] = 0;
    first_psplit0[i] = 0;
    first_psplit1[i] = 0;
    first_kpsplit0[i] = 0;
    first_kpsplit1[i] = 0;
  }

  // 6. qabc
  for (i = 0; i < NUMH; i++) {
    smallh[i] = 0;
    smallhD[i] = 0;
  }

  ttime = 0;
}

// inline function to read a ZZ from a file
inline void read_zz(ZZ &a, FILE *fp) {
  unsigned char val[50];

  fread(val, sizeof(char), 10, fp);
  ZZFromBytes(a, val, 10);
}

// inline funciton to write a ZZ to a file
inline void write_zz(ZZ &a, FILE *fp) {
  unsigned char val[50];

  BytesFromZZ(val, a, 10);
  fwrite(val, sizeof(char), 10, fp);
}

// inline function to read an RR from a file
inline void read_rr(RR &a, FILE *fp) {
  char val[100];
  char *newval;

  fread(val, sizeof(char), 50, fp);
  newval = val;
  a = to_RR(val);
}

// inline function to write an RR to a file
inline void write_rr(RR &a, FILE *fp) {
  char val[100];
  const char *newval;

  ostringstream rrout(val, ostringstream::out);
  rrout << a << endl;
  newval = rrout.str().c_str();
  fwrite(newval, sizeof(char), 50, fp);
}

// read in statistics data from a given statistics file
void iq_data::read_file(char *name, char *pfix) {

  char nname[100];
  int i, j, e1, e2, e3, e4, e5, e6;
  FILE *fp;

  strcpy(nname, pfix);
  strcat(nname, name);
  if ((fp = fopen(nname, "rb")) == NULL) {
    printf("Cannot open %s.\n", nname);
    exit(1);
  }

  read_zz(maxD, fp);
  fread(&total0, sizeof(long long), 1, fp);
  fread(&total1, sizeof(long long), 1, fp);
  fread(&noncyc0, sizeof(long long), 1, fp);
  fread(&noncyc1, sizeof(long long), 1, fp);
  fread(&minL0, sizeof(double), 1, fp);
  fread(&minL1, sizeof(double), 1, fp);
  fread(&minL5, sizeof(double), 1, fp);
  fread(&maxL0, sizeof(double), 1, fp);
  fread(&maxL1, sizeof(double), 1, fp);
  fread(&maxL5, sizeof(double), 1, fp);
  fread(&minLLI0, sizeof(double), 1, fp);
  fread(&minLLI1, sizeof(double), 1, fp);
  fread(&minLLI5, sizeof(double), 1, fp);
  fread(&maxULI0, sizeof(double), 1, fp);
  fread(&maxULI1, sizeof(double), 1, fp);
  fread(&maxULI5, sizeof(double), 1, fp);
  read_zz(minLLI0D, fp);
  read_zz(minLLI1D, fp);
  read_zz(minLLI5D, fp);
  read_zz(maxULI0D, fp);
  read_zz(maxULI1D, fp);
  read_zz(maxULI5D, fp);
  fread(&maxp, sizeof(long), 1, fp);
  read_zz(maxpD, fp);
  fread(&localmaxp, sizeof(long), 1, fp);
  read_zz(localmaxpD, fp);
  fread(&maxh0, sizeof(long), 1, fp);
  fread(&maxh1, sizeof(long), 1, fp);
  read_zz(maxh0D, fp);
  read_zz(maxh1D, fp);
  fread(&maxhodd0, sizeof(long), 1, fp);
  fread(&maxhodd1, sizeof(long), 1, fp);
  read_zz(maxhodd0D, fp);
  read_zz(maxhodd1D, fp);
  read_rr(sumL0, fp);
  read_rr(sumL1, fp);
  fread(&maxlograt, sizeof(double), 1, fp);
  fread(&maxloglograt, sizeof(double), 1, fp);
  read_zz(maxlogratD, fp);
  read_zz(maxloglogratD, fp);
  read_rr(sumlograt, fp);
  read_rr(sumloglograt, fp);

  for (i = 0; i < MAXP; ++i)
    for (j = 0; j < MAXPSMALL; ++j) {
      fread(&pdivs0[i][j], sizeof(long long), 1, fp);
      fread(&pdivs1[i][j], sizeof(long long), 1, fp);
    }

  for (i = 0; i < MAXPLARGE; ++i)
    for (e1 = 0; e1 < MAXE1; ++e1)
      for (e2 = 0; e2 < MAXE2; ++e2)
        for (e3 = 0; e3 < MAXE3; ++e3)
          for (e4 = 0; e4 < MAXE4; ++e4)
            for (e5 = 0; e5 < MAXE5; ++e5)
              for (e6 = 0; e6 < MAXE6; ++e6) {
                fread(&prank0[i][e1][e2][e3][e4][e5][e6], sizeof(long), 1, fp);
                fread(&prank1[i][e1][e2][e3][e4][e5][e6], sizeof(long), 1, fp);
                read_zz(first_prank0[i][e1][e2][e3][e4][e5][e6], fp);
                read_zz(first_prank1[i][e1][e2][e3][e4][e5][e6], fp);
              }

  for (e1 = 0; e1 < MAXP1; ++e1)
    for (e2 = 0; e2 < MAXP2; ++e2)
      for (e3 = 0; e3 < MAXP3; ++e3)
        for (e4 = 0; e4 < MAXP4; ++e4) {
          fread(&two_noncyc0[e1][e2][e3][e4], sizeof(long), 1, fp);
          fread(&two_noncyc1[e1][e2][e3][e4], sizeof(long), 1, fp);
          read_zz(first_two_noncyc0[e1][e2][e3][e4], fp);
          read_zz(first_two_noncyc1[e1][e2][e3][e4], fp);
        }

  for (i = 0; i < MAXPLARGE; i++) {
    fread(&psplit0[i], sizeof(long long), 1, fp);
    fread(&psplit1[i], sizeof(long long), 1, fp);
    fread(&kpsplit0[i], sizeof(long long), 1, fp);
    fread(&kpsplit1[i], sizeof(long long), 1, fp);
    read_zz(first_psplit0[i], fp);
    read_zz(first_psplit1[i], fp);
    read_zz(first_kpsplit0[i], fp);
    read_zz(first_kpsplit1[i], fp);
  }

  for (i = 0; i < NUMH; i++) {
    fread(&smallh[i], sizeof(long), 1, fp);
    read_zz(smallhD[i], fp);
  }

  fread(&ttime, sizeof(long), 1, fp);

  fclose(fp);
}

// write out statistics data to a file
void iq_data::write_file(char *name, char *pfix) {

  char nname[100];
  int i, j, e1, e2, e3, e4, e5, e6;
  FILE *fp;

  strcpy(nname, pfix);
  strcat(nname, name);
  if ((fp = fopen(nname, "wb")) == NULL) {
    printf("Cannot open %s.\n", nname);
    exit(1);
  }

  write_zz(maxD, fp);
  fwrite(&total0, sizeof(long long), 1, fp);
  fwrite(&total1, sizeof(long long), 1, fp);
  fwrite(&noncyc0, sizeof(long long), 1, fp);
  fwrite(&noncyc1, sizeof(long long), 1, fp);
  fwrite(&minL0, sizeof(double), 1, fp);
  fwrite(&minL1, sizeof(double), 1, fp);
  fwrite(&minL5, sizeof(double), 1, fp);
  fwrite(&maxL0, sizeof(double), 1, fp);
  fwrite(&maxL1, sizeof(double), 1, fp);
  fwrite(&maxL5, sizeof(double), 1, fp);
  fwrite(&minLLI0, sizeof(double), 1, fp);
  fwrite(&minLLI1, sizeof(double), 1, fp);
  fwrite(&minLLI5, sizeof(double), 1, fp);
  fwrite(&maxULI0, sizeof(double), 1, fp);
  fwrite(&maxULI1, sizeof(double), 1, fp);
  fwrite(&maxULI5, sizeof(double), 1, fp);
  write_zz(minLLI0D, fp);
  write_zz(minLLI1D, fp);
  write_zz(minLLI5D, fp);
  write_zz(maxULI0D, fp);
  write_zz(maxULI1D, fp);
  write_zz(maxULI5D, fp);
  fwrite(&maxp, sizeof(long), 1, fp);
  write_zz(maxpD, fp);
  fwrite(&localmaxp, sizeof(long), 1, fp);
  write_zz(localmaxpD, fp);
  fwrite(&maxh0, sizeof(long), 1, fp);
  fwrite(&maxh1, sizeof(long), 1, fp);
  write_zz(maxh0D, fp);
  write_zz(maxh1D, fp);
  fwrite(&maxhodd0, sizeof(long), 1, fp);
  fwrite(&maxhodd1, sizeof(long), 1, fp);
  write_zz(maxhodd0D, fp);
  write_zz(maxhodd1D, fp);
  write_rr(sumL0, fp);
  write_rr(sumL1, fp);
  fwrite(&maxlograt, sizeof(double), 1, fp);
  fwrite(&maxloglograt, sizeof(double), 1, fp);
  write_zz(maxlogratD, fp);
  write_zz(maxloglogratD, fp);
  write_rr(sumlograt, fp);
  write_rr(sumloglograt, fp);

  for (i = 0; i < MAXP; ++i)
    for (j = 0; j < MAXPSMALL; ++j) {
      fwrite(&pdivs0[i][j], sizeof(long long), 1, fp);
      fwrite(&pdivs1[i][j], sizeof(long long), 1, fp);
    }

  for (i = 0; i < MAXPLARGE; ++i) {
    for (e1 = 0; e1 < MAXE1; ++e1)
      for (e2 = 0; e2 < MAXE2; ++e2)
        for (e3 = 0; e3 < MAXE3; ++e3)
          for (e4 = 0; e4 < MAXE4; ++e4)
            for (e5 = 0; e5 < MAXE5; ++e5)
              for (e6 = 0; e6 < MAXE6; ++e6) {
                fwrite(&prank0[i][e1][e2][e3][e4][e5][e6], sizeof(long), 1, fp);
                fwrite(&prank1[i][e1][e2][e3][e4][e5][e6], sizeof(long), 1, fp);
                write_zz(first_prank0[i][e1][e2][e3][e4][e5][e6], fp);
                write_zz(first_prank1[i][e1][e2][e3][e4][e5][e6], fp);
              }
  }

  for (e1 = 0; e1 < MAXP1; ++e1)
    for (e2 = 0; e2 < MAXP2; ++e2)
      for (e3 = 0; e3 < MAXP3; ++e3)
        for (e4 = 0; e4 < MAXP4; ++e4) {
          fwrite(&two_noncyc0[e1][e2][e3][e4], sizeof(long), 1, fp);
          fwrite(&two_noncyc1[e1][e2][e3][e4], sizeof(long), 1, fp);
          write_zz(first_two_noncyc0[e1][e2][e3][e4], fp);
          write_zz(first_two_noncyc1[e1][e2][e3][e4], fp);
        }

  for (i = 0; i < MAXPLARGE; i++) {
    fwrite(&psplit0[i], sizeof(long long), 1, fp);
    fwrite(&psplit1[i], sizeof(long long), 1, fp);
    fwrite(&kpsplit0[i], sizeof(long long), 1, fp);
    fwrite(&kpsplit1[i], sizeof(long long), 1, fp);
    write_zz(first_psplit0[i], fp);
    write_zz(first_psplit1[i], fp);
    write_zz(first_kpsplit0[i], fp);
    write_zz(first_kpsplit1[i], fp);
  }

  for (i = 0; i < NUMH; i++) {
    fwrite(&smallh[i], sizeof(long), 1, fp);
    write_zz(smallhD[i], fp);
  }

  fwrite(&ttime, sizeof(long), 1, fp);

  fclose(fp);
}

// combine data in two statistics files
void iq_data::combine(iq_data &newdata) {
  std::cout << "combine: beginning!" << std::endl;
  int i, j, e1, e2, e3, e4, e5, e6;
  long element, temp;
  ZZ elementD, tempD;

  maxD = newdata.maxD;
  total0 += newdata.total0;
  total1 += newdata.total1;
  noncyc0 += newdata.noncyc0;
  noncyc1 += newdata.noncyc1;
  if (newdata.minL0 < minL0)
    minL0 = newdata.minL0;
  if (newdata.minL1 < minL1)
    minL1 = newdata.minL1;
  if (newdata.minL5 < minL5)
    minL5 = newdata.minL5;
  if (newdata.maxL0 > maxL0)
    maxL0 = newdata.maxL0;
  if (newdata.maxL1 > maxL1)
    maxL1 = newdata.maxL1;
  if (newdata.maxL5 > maxL5)
    maxL5 = newdata.maxL5;

  std::cout << "combine: 1" << std::endl;
  minLLI0 = newdata.minLLI0;
  minLLI0D = newdata.minLLI0D;
  minLLI1 = newdata.minLLI1;
  minLLI1D = newdata.minLLI1D;
  minLLI5 = newdata.minLLI5;
  minLLI5D = newdata.minLLI5D;
  maxULI0 = newdata.maxULI0;
  maxULI0D = newdata.maxULI0D;
  maxULI1 = newdata.maxULI1;
  maxULI1D = newdata.maxULI1D;
  maxULI5 = newdata.maxULI5;
  maxULI5D = newdata.maxULI5D;
  if (newdata.maxp > maxp) {
    maxp = newdata.maxp;
    maxpD = newdata.maxpD;
  }
  localmaxp = newdata.localmaxp;
  localmaxpD = newdata.localmaxpD;

  if (newdata.maxh0 > maxh0) {
    maxh0 = newdata.maxh0;
    maxh0D = newdata.maxh0D;
  }
  if (newdata.maxh1 > maxh1) {
    maxh1 = newdata.maxh1;
    maxh1D = newdata.maxh1D;
  }
  if (newdata.maxhodd0 > maxhodd0) {
    maxhodd0 = newdata.maxhodd0;
    maxhodd0D = newdata.maxhodd0D;
  }
  if (newdata.maxhodd1 > maxhodd1) {
    maxhodd1 = newdata.maxhodd1;
    maxhodd1D = newdata.maxhodd1D;
  }

  std::cout << "combine: 2" << std::endl;
  sumL0 += newdata.sumL0;
  sumL1 += newdata.sumL1;

  if (newdata.maxlograt > maxlograt) {
    maxlograt = newdata.maxlograt;
    maxlogratD = newdata.maxlogratD;
  }
  if (newdata.maxloglograt > maxloglograt) {
    maxloglograt = newdata.maxloglograt;
    maxloglogratD = newdata.maxloglogratD;
  }

  sumlograt += newdata.sumlograt;
  sumloglograt += newdata.sumloglograt;

  std::cout << "combine: 3" << std::endl;
  for (i = 0; i < MAXP; ++i)
    for (j = 0; j < MAXPSMALL; ++j) {
      pdivs0[i][j] += newdata.pdivs0[i][j];
      pdivs1[i][j] += newdata.pdivs1[i][j];
    }

  std::cout << "combine: 4" << std::endl;
  for (i = 0; i < MAXPLARGE; ++i) {
    for (e1 = 0; e1 < MAXE1; ++e1)
      for (e2 = 0; e2 < MAXE2; ++e2)
        for (e3 = 0; e3 < MAXE3; ++e3)
          for (e4 = 0; e4 < MAXE4; ++e4)
            for (e5 = 0; e5 < MAXE5; ++e5)
              for (e6 = 0; e6 < MAXE6; ++e6) {
                prank0[i][e1][e2][e3][e4][e5][e6] +=
                    newdata.prank0[i][e1][e2][e3][e4][e5][e6];
                prank1[i][e1][e2][e3][e4][e5][e6] +=
                    newdata.prank1[i][e1][e2][e3][e4][e5][e6];
                if (first_prank0[i][e1][e2][e3][e4][e5][e6] == 0)
                  first_prank0[i][e1][e2][e3][e4][e5][e6] =
                      newdata.first_prank0[i][e1][e2][e3][e4][e5][e6];
                if (first_prank1[i][e1][e2][e3][e4][e5][e6] == 0)
                  first_prank1[i][e1][e2][e3][e4][e5][e6] =
                      newdata.first_prank1[i][e1][e2][e3][e4][e5][e6];
              }
  }

  std::cout << "combine: 5" << std::endl;
  for (e1 = 0; e1 < MAXP1; ++e1)
    for (e2 = 0; e2 < MAXP2; ++e2)
      for (e3 = 0; e3 < MAXP3; ++e3)
        for (e4 = 0; e4 < MAXP4; ++e4) {
          two_noncyc0[e1][e2][e3][e4] += newdata.two_noncyc0[e1][e2][e3][e4];
          two_noncyc1[e1][e2][e3][e4] += newdata.two_noncyc1[e1][e2][e3][e4];
          if (first_two_noncyc0[e1][e2][e3][e4] == 0)
            first_two_noncyc0[e1][e2][e3][e4] =
                newdata.first_two_noncyc0[e1][e2][e3][e4];
          if (first_two_noncyc1[e1][e2][e3][e4] == 0)
            first_two_noncyc1[e1][e2][e3][e4] =
                newdata.first_two_noncyc1[e1][e2][e3][e4];
        }

  std::cout << "combine: 6" << std::endl;
  for (i = 0; i < MAXPLARGE; i++) {
    psplit0[i] += newdata.psplit0[i];
    psplit1[i] += newdata.psplit1[i];
    kpsplit0[i] += newdata.kpsplit0[i];
    kpsplit1[i] += newdata.kpsplit1[i];
    if (first_psplit0[i] == 0)
      first_psplit0[i] = newdata.first_psplit0[i];
    if (first_psplit1[i] == 0)
      first_psplit1[i] = newdata.first_psplit1[i];
    if (first_kpsplit0[i] == 0)
      first_kpsplit0[i] = newdata.first_kpsplit0[i];
    if (first_kpsplit1[i] == 0)
      first_kpsplit1[i] = newdata.first_kpsplit1[i];
  }

  std::cout << "combine: 7" << std::endl;
  for (i = 0; i < NUMH; i++) {
  std::cout << "combine: 7.1" << std::endl;
    if (newdata.smallhD[i] != 0) {
      std::cout << "combine: 7.2" << std::endl;
      element = newdata.smallh[i];
      elementD = newdata.smallhD[i];

      std::cout << "combine: 7.3" << std::endl;
      j = 0;
      while (smallh[j] < element && smallhD[j] != 0) {
        std::cout << "combine: 7.4" << std::endl;
        j++;
      }
      while (j < NUMH - 1) {
        std::cout << "combine: 7.5.1" << std::endl;
        temp = smallh[j];
        std::cout << "combine: 7.5.2" << std::endl;
        tempD = smallhD[j];
        std::cout << "combine: 7.5.3" << std::endl;
        smallh[j] = element;
        std::cout << "combine: 7.5.4" << std::endl;
        smallhD[j] = elementD;
        std::cout << "combine: 7.5.5" << std::endl;
        element = temp;
        elementD = tempD;
        j++;
      }
    }
  }

  ttime += newdata.ttime;
  std::cout << "combine: finishing!" << std::endl;
}