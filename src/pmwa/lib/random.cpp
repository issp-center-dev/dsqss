/*
 *=================================================================
 *     Class Random for random number generation
 *=================================================================
 *     $Id: Random.cc,v 1.1 2000/02/10 16:13:02 kenji Exp $
 *     $Log: Random.cc,v $
 *     Revision 1.1  2000/02/10 16:13:02  kenji
 *     Initial revision
 *
 *=================================================================
 *     The copyright holder of the following codes is
 *
 *     Kenji HARADA
 *     Graduate School of Infomatics, Kyoto University,
 *     Kyoto 606-8501, Japan
 *     e-mail: harada@acs.i.kyoto-u.ac.jp
 *     home-page: http://www-fcs.acs.i.kyoto-u.ac.jp/~harada/
 *=================================================================
 */

#include "random.h"

// Uniform [0,1)
double Random::Uniform(void) {
  if (navr == 0) {
    Rint i;
    for (i = 0; i < IQQ; i++) iri[i] ^= iri[i + IPQ];
    for (i = IQQ; i < IPP; i++) iri[i] ^= iri[i - IQQ];
    iptr = 0;
    navr = IPP;
  }
  navr--;
  return runit * iri[iptr++];
}

Rint Random::Int(Rint ilimit) {
  if (navr == 0) {
    Rint i;
    for (i = 0; i < IQQ; i++) iri[i] ^= iri[i + IPQ];
    for (i = IQQ; i < IPP; i++) iri[i] ^= iri[i - IQQ];
    iptr = 0;
    navr = IPP;
  }
  navr--;
  return (Rint)(ilimit * runit * iri[iptr++]);
}

// Uniform Integer (0, 1, ..., 2**nrbit0-1)
Rint Random::Int(void) {
  if (navr == 0) {
    Rint i;
    for (i = 0; i < IQQ; i++) iri[i] ^= iri[i + IPQ];
    for (i = IQQ; i < IPP; i++) iri[i] ^= iri[i - IQQ];
    iptr = 0;
    navr = IPP;
  }
  navr--;
  return (Rint)iri[iptr++];
}

// Random Permutation
void Random::Perm(Rint N, int *Index) {
  for (Rint i = 0; i < N; i++) Index[i] = i;
  for (Rint i = 0; i < N - 1; i++) {
    Rint x = Index[i];
    Rint ix = i + Int(N - i);
    Index[i] = Index[ix];
    Index[ix] = x;
  }
  /*
    Rint i,n,ind;
    double rx=runit*N;
    for (i=0; i<N; i++) Index[i] = -1;
    ind = 0;
    while(ind < N) {
      n = (Rint) (Int()*rx);
      if (Index[n] == -1) {
        Index[n] = ind;
        ind++;
      }
    }
    */
}

void Random::Scramble(Rint N, int *Index) {
  for (Rint i = 0; (i + 1) < N; i++) {
    Rint x = Index[i];
    Rint ix = i + Int(N - i);
    Index[i] = Index[ix];
    Index[ix] = x;
  }
}
//: Scramble

void Random::initialize(Rint irand0, Rint nrbit0) {
  /*
   *-----------------------------------------------------------------
   *
   *     This subroutine calculates the random seed and the effective bit.
   *     The irand0 is initial seed.
   *     The nrbit0 is effective bit as 0 <= random number <= 2**nrbit0 - 1.
   *     (Note. 0<= nrbit0 <= 32)
   *-----------------------------------------------------------------
   */
  Rint imask;
  Rint ix, ih, mj, ii, ij;
  Rint i, j;
  Rint ia[IPP], ib[IPP];

  nrbit = nrbit0;
  imask = 1;
  for (i = 0; i < nrbit - 1; i++) imask = (imask << 1) + 1;
  runit = 1e0 / (imask + 1e0);
  ix = irand0;
  for (i = 0; i < IPP; i++) {
    ix = (ix * 1812433253 + 1) & 0xffffffff;
    if (ix & 0x80000000)
      ia[i] = 1;
    else
      ia[i] = 0;
  }
  for (j = 0; j < IPP; j++) {
    ih = ((j * 32) % IPP);
    mj = 0;
    for (i = 0; i < 32; i++) {
      ii = (ih + i) % IPP;
      mj = 2 * mj + ia[ii];
      ij = (ii + IPQ) % IPP;
      ia[ii] ^= ia[ij];
    }
    ib[j] = mj;
  }
  for (j = 0; j < IPP; j++) {
    ih = ((j * 16) % IPP);
    iri[j] = imask & ib[ih];
    for (i = 0; i < 16; i++) {
      ii = (ih + i) % IPP;
      ij = (ii + IPQ) % IPP;
      ib[ii] ^= ib[ij];
    }
  }
  navr = IPP;
  iptr = 0;
  return;
}

Random::Random(Rint *seed, Rint nrbit0) { setSeed(seed, nrbit0); }
Random::Random(Rint irand0, Rint nrbit0) { initialize(irand0, nrbit0); }

void Random::setSeed(Rint irand0, Rint nrbit0) { initialize(irand0, nrbit0); }
void Random::setSeed(Rint *seed, Rint nrbit0) {
  Rint imask;
  Rint i;
  nrbit = nrbit0;
  imask = 1;
  for (i = 0; i < nrbit - 1; i++) imask = (imask << 1) + 1;
  runit = 1e0 / (imask + 1e0);
  for (i = 0; i < IPP; i++) iri[i] = imask & seed[i];
  navr = IPP;
  iptr = 0;
  return;
}

Rint Random::getSeed(Rint *seed) {
  Uniform(IPP, seed);
  return nrbit;
}

void Random::Uniform(Rint nr, Rint *ir) {
  /*
   *----------------------------------------------------------------
   *
   *     This subroutine calculates random number table.
   *     The nr is length of the table.
   *     The ir is Rint array for returned table.
   *
   *     The returned value is as follows:0 <= returned value <= 2**nrbit-1.
   *
   *----------------------------------------------------------------
   */
  Rint i, nrec, n1, iptd;

  nrec = nr;
  n1 = (navr > nrec ? nrec : navr);
  for (i = 0; i < n1; i++) ir[i] = iri[iptr + i];
  iptd = n1;
  iptr += n1;
  navr -= n1;
  nrec -= n1;
  while (nrec > 0) {
    for (i = 0; i < IQQ; i++) iri[i] ^= iri[i + IPQ];
    for (i = IQQ; i < IPP; i++) iri[i] ^= iri[i - IQQ];
    n1 = (IPP > nrec ? nrec : IPP);
    for (i = 0; i < n1; i++) ir[iptd + i] = iri[i];
    iptd += n1;
    iptr = n1;
    navr = IPP - n1;
    nrec -= n1;
  }
  return;
}

void Random::Uniform(Rint nr, double *rx) {
  /*
   *----------------------------------------------------------------
   *
   *     This subroutine calculates random number table.
   *     The nr is length of the table.
   *     The rx is double array for returned table.
   *
   *     The returned value is as follows:0 <= returned value < 1.
   *
   *----------------------------------------------------------------
   */
  Rint i, nrec, n1, iptd;

  nrec = nr;
  n1 = (navr > nrec ? nrec : navr);
  for (i = 0; i < n1; i++) rx[i] = runit * iri[iptr + i];
  iptd = n1;
  iptr += n1;
  navr -= n1;
  nrec -= n1;
  while (nrec > 0) {
    for (i = 0; i < IQQ; i++) iri[i] ^= iri[i + IPQ];
    for (i = IQQ; i < IPP; i++) iri[i] ^= iri[i - IQQ];
    n1 = (IPP > nrec ? nrec : IPP);
    for (i = 0; i < n1; i++) rx[iptd + i] = runit * iri[i];
    iptd += n1;
    iptr = n1;
    navr = IPP - n1;
    nrec -= n1;
  }
  return;
}

void Random::Int(Rint nr, Rint *ir, Rint ilimit) {
  /*
   *----------------------------------------------------------------
   *
   *     This subroutine calculates random number table.
   *     The nr is length of the table.
   *     The ir is Rint array for returned table.
   *     The ilimit is upper bound of returned table.
   *
   *     The returned value is as follows:0 <= returned value < ilimit.
   *
   *----------------------------------------------------------------
   */
  Rint i, nrec, n1, iptd;
  double runitx;

  runitx = runit * ilimit;
  nrec = nr;
  n1 = (navr > nrec ? nrec : navr);
  for (i = 0; i < n1; i++) ir[i] = (Rint)(runitx * iri[iptr + i]);
  iptd = n1;
  iptr += n1;
  navr -= n1;
  nrec -= n1;
  while (nrec > 0) {
    for (i = 0; i < IQQ; i++) iri[i] ^= iri[i + IPQ];
    for (i = IQQ; i < IPP; i++) iri[i] ^= iri[i - IQQ];
    n1 = (IPP > nrec ? nrec : IPP);
    for (i = 0; i < n1; i++) ir[iptd + i] = (Rint)(runitx * iri[i]);
    iptd += n1;
    iptr = n1;
    navr = IPP - n1;
    nrec -= n1;
  }
  return;
}
