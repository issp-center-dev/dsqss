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

//: 乱数生成
// このクラスでは乱数をＭ系列法を用いて生成している。
// 実際にt番目の乱数X(t)は次式から計算する。<BR>
// X(t) := X(t-32) xor X(t-521)
class Random {
private:
  Rint nrbit, iptr, navr;
  Rint iri[IPP];
  double runit;

private:
  void initialize(Rint irand0, Rint nrbit0);
  //: 初期設定
  //!param: irand0 - 521個の初期値を生成するのに用いる種
  //!param: nrbit0 - 乱数の精度（nrbit0ビット）
  // 初期設定をする関数

public:
  Random(Rint *seed, Rint nrbit0);
  //: 初期値と精度を指定した Class Constructor
  // 整数値配列 seedに格納されている521個を
  // 精度nrbit0ビットの初期値としてセットする。

  Random(Rint irand0 = 20000101, Rint nrbit0 = 32);
  //: 種、精度指定した Class Constructor
  // 種 irand0を使ってnrbit0ビットの精度をもつ乱数のための初期設定をする。

  void setSeed(Rint irand0, Rint nrbit0 = 32);
  //: 種、精度を指定した再初期設定
  //!param: irand0 - 521個の初期値を生成するのに用いる種
  //!param: nrbit0 - 乱数の精度（nrbit0ビット）
  // 種をirand0に精度nrbit0ビットで再初期設定をする。

  void setSeed(Rint *seed, Rint nrbit0);
  //: 初期値と精度を再設定
  //!param: seed - 521個の初期値が格納されている記憶領域へのポインタ
  //!param: nrbit0 - 乱数の精度（nrbit0ビット）
  // 整数値配列 initに格納されている521個を
  // 精度nrbit0ビットの初期値としてセットする。

  Rint getSeed(Rint *seed);
  //: 現在の乱数の初期値と精度を返す
  //!param: seed - 現在の乱数の初期値(521個)を格納する記憶領域へのポインタ
  // 現在の乱数の初期値（521個）をポインタseedの指す記憶領域に格納する。
  // さらに精度のビット数を戻り値として返す。

  double Uniform(void);
  //add ---sakakura 20110322 ---
  double Dicex(void);
  void InitRand();
  //add ---sakakura 20110322 ---

  Rint Int(Rint);
  //: 離散一様分布 (0, 1, .. ,ilimit-1)
  //!param: ilimit - 範囲の上限
  // 戻り値に 0以上、ilimit未満の乱数(整数)を返す。

  Rint Int(void);

  void Uniform(Rint nr, Rint *ir);
  //: 離散一様分布乱数(整数値配列)
  //!param: nr - 生成する乱数の個数
  //!param: ir - 生成した整数値乱数（0以上、2**nrbit0-1 未満）を格納する番地へのポインタ
  // 整数値乱数（0以上、2**nrbit0-1未満）をnr個生成し、整数ポインタ irの指す番地から順番に格納する。

  void Uniform(Rint nr, double *rx);
  //: 一様分布乱数(実数値配列)
  //!param: nr - 生成する乱数の個数
  //!param: rx - 生成した実数値乱数（0以上、1未満）を格納する番地へのポインタ
  // 実数値乱数（0以上、1未満）をnr個生成し、実数ポインタ rxの指す番地から順番に格納する。

  void Int(Rint nr, Rint *ir, Rint ilimit);
  //: 上限付き離散一様分布(整数値配列)
  //!param: nr - 生成する乱数の個数
  //!param: ir - 生成した整数値乱数（0以上、ilimit 未満）を格納する番地へのポインタ
  //!param: ilimit - 生成する整数値乱数の上限
  // 整数値乱数（0以上、ilimit未満）をnr個生成し、整数ポインタ irの指す番地から順番に格納する。

  double Gauss() {
    double theta;
    theta = 6.283185307179586477 * Uniform();
    return sqrt(-2e0 * log(1e0 - Uniform())) * sin(theta);
  }
  //: 正規分布

  double Exp() { return -log(1e0 - Uniform()); }
  //: 正規指数分布

  int Binary(double P) { return ((int)(Uniform() / P)); }
  //: ２項分布

  void Perm(Rint, int *);

  void Scramble(Rint, int *);
};

#define _RANDOM_H_
#endif
