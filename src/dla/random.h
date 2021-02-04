/*
 *=================================================================
 *     Header of Class Random for random number generation
 *=================================================================
 *     $Log: Random.h,v $
 *     Revision 1.1  2000/02/10 16:14:16  kenji
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
//! id="$Id: Random.h,v 1.1 2000/02/10 16:14:16 kenji Exp $"
//! author="Kenji Harada"

#ifndef _RANDOM_H_
#include <cmath>
#define IPP 521
#define SIZE_SEED IPP
#define IQQ 32
#define IPQ (IPP - IQQ)
typedef unsigned int Rint;

// Random Number Generator
// using M series method
// X(t) := X(t-32) xor X(t-521)
class Random {
 private:
  Rint nrbit, iptr, navr;
  Rint iri[IPP];
  double runit;

 private:
  void initialize(Rint irand0, Rint nrbit0);
  //: Initialization
  //! param: irand0 - seed for 521 initial random numbers
  //! param: nrbit0 - precision (number of bit)

 public:
  Random(Rint *seed, Rint nrbit0);
  //: Constructor
  //! param: seed - 521 initial random numbers
  //! param: nrbit0 - precision (number of bit)

  Random(Rint irand0 = 20000101, Rint nrbit0 = 32);
  //: Constructor
  //! param: irand0 - seed for 521 initial random numbers
  //! param: nrbit0 - precision (number of bit)

  void setSeed(Rint irand0, Rint nrbit0 = 32);
  //: Reset
  //! param: irand0 - seed for 521 initial random numbers
  //! param: nrbit0 - precision (number of bit)

  void setSeed(Rint *seed, Rint nrbit0);
  //: Reset
  //! param: seed - 521 initial random numbers
  //! param: nrbit0 - precision (number of bit)

  Rint getSeed(Rint *seed);
  //: Return seed and nrbit0

  double Uniform(void);
  double Dicex(void);
  void InitRand();

  Rint Int(Rint ilimit);
  //: Uniform integer (0, 1, .. ,ilimit-1)

  Rint Int(void);

  void Uniform(Rint nr, Rint *ir);
  //: Uniform integers
  //! param: nr - the number of random numbers to be generated
  //! param: ir - pointer to store outputs

  void Uniform(Rint nr, double *rx);
  //: Uniform reals
  //! param: nr - the number of random numbers to be generated
  //! param: rx - pointer to store outputs

  void Int(Rint nr, Rint *ir, Rint ilimit);
  //: Uniform integers
  //! param: nr - the number of random numbers to be generated
  //! param: ir - pointer to store outputs
  //! param: ilimit - maximum

  double Gauss() {
    double theta;
    theta = 6.283185307179586477 * Uniform();
    return sqrt(-2e0 * log(1e0 - Uniform())) * sin(theta);
  }

  double Exp() { return -log(1e0 - Uniform()); }

  int Binary(double P) { return ((int)(Uniform() / P)); }

  void Perm(Rint, int *);

  void Scramble(Rint, int *);
};

#define _RANDOM_H_
#endif
