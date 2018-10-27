#ifndef SYSTEMPARAM_H
#define SYSTEMPARAM_H

#define VAL 10
#define INF 1.0e+14
#define EPS 1.0e-14

struct Size {
  int x;        //the number of lattice for x.
  int y;        //the number of lattice for y.
  int z;        //the number of lattice for z.
  int V;        //the total number of lattice.
  int d;        //dimension.
  double B;     //the inverse of temperature.
  double oldB;  //for annealing.
};

struct System {
  int nmax;    // the maximum number of bosons on a same site.
  double Ubb;  //on-site interaction.
  double Vb1;  // nearest-neighbor interaction.
  double tb;   // hopping
  double mu;   // chemical potential.
  int w_num;   //
  double Htr;  // the strenght of the transverse field for introducing worm.
  double Eu;   // arbitrary energy shift for Ubb.
  double Et;   // arbitrary energy shift for t-Vb1.
};

struct MC_p {
  int seed;
  int Nstep;
  int Nthermal;
  int Nsample;
  int Nbin;
  int Ntest;
  int nc;
  int runtype; //{0: normal, 1:restart, 2:annealing}
};

struct Parallel {
  int p_num;                       // the total number of processors.
  int my_rank;                     // the rank of each proseccor.
  int Ntdiv;                       // the number of the temporal decomposition.
  int Nsdiv, Nxdiv, Nydiv, Nzdiv;  // the number of the spatial decomposition. (total, x, y, z)
  int NtNs;                        // the number of domain decomposition.
  int Npara;                       // the number of trivial parallelization.
  double B;                        // beta of "a domain".
  double oldB;                     // for annealing.
  bool FlgAnneal;                  // for annealing.
  bool FlgRestart;                 // for restart.

  int x, y, z, V;  // the coordinate number.

  // process 番号
  int np; // 乱数並列の番号
  int Rpara; // the number of replica (random system)
  int nq; // ランダム系の番号（乱数並列）
  int nr; // ランダム系の番号（レプリカ）
  /*
  // a random potential number (one of trivial parallelization)
    PR.nr = PR.np % PR.Rpara; 
  // a seed number of the trivial parallelization for the random potential (one of trivial parallelization)
    PR.nq = PR.np / PR.Rpara;  
        */

  // 自分が受け持つドメインのインデックス
  int nst; // ドメインのインデックス
  int nt; // 虚時間方向インデックス
  int ns; // 実空間方向インデックス
  int nx; // x 座標
  int ny; // y
  int nz; // z

  // 時空間の原点を受け持つプロセス番号
  int nst0; // 同じnpを持つプロセスのうち、nst=0 のもの
  int nt0;  // 同じnp, ns を持つプロセスのうち、 nt=0 のもの
  int ns0;
  int nx0;

  // 隣のドメインを受け持つプロセス番号
  int upper; // tau 方向上
  int lower; // tau 方向下
  int right[3]; // 空間方向右 (x,y,z)
  int left[3];  // 空間方向下 (x,y,z)
};

#endif
