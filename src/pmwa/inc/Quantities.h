#ifndef SRC_PMWA_INC_QUANTITIES_H_
#define SRC_PMWA_INC_QUANTITIES_H_

#include <complex>
#include <xml.hpp>

using namespace XML;

class Quantities {
  //  public:
 private:
#ifdef SF
  XML::Block X;
#endif

  MPI_Status status;

  void WindingNumber(vector<GraphSpace::Vertex> &ev, int mcs);

  void WindingNumber(vector<GraphSpace::Vertex *> WORM, int mcs);

  void WindingNumber2();

  void Density(GraphSpace::Vertex *world, GraphSpace::Vertex *worldB);

  void Energy();

  void NumberOfVertices(int countv, int mcs);

  void NumberOfKinks(int Nv, int mcs);

  void NumberOfWorms(vector<GraphSpace::Vertex> &ev, int mcs);

  void CondensateFraction(int mcs, GraphSpace::Vertex *world);

  void CorrelationFunction1();

  void CorrelationFunction2(GraphSpace::Vertex *world);

  void SpecificHeat();

  void Susceptibility();

  void NoiseCorrelation();

  void Compressibility();

  void CorrelationLength(double);

  void SUM_OVER_T();

  void SUM_OVER_S();

  void SUM_OVER_ST();

  int Nend, Lsize;
  int Cknum;
  int NVMAX, NWMAX;

  enum {
    nver1,
    nwor1,
    nkin1,
    wndx1,
    wndy1,
    wndz1,
    amzu1,
    smzs1,
    smzu1,
    bmz1,
    xmzs1,
    magx1,
    magp1,
    magm1,
    ang1,
    Nq1
  };
  enum {
    nver,
    nwor,
    nkin,
    wndx,
    wndy,
    wndz,
    amzu,
    smzs,
    smzu,
    bmzu,
    xmzs,
    magx,
    magp,
    magm,
    ang,
    ene,
    spe,
    wnd2,
    comp,
    lxmx,
    xmx,
    xmzu,
    d00,
    len,
    Nq
  };
  enum { mzl, nwl, nw2l, Nc1 };
  enum { cr2r, ck2k, ck4k, dkkk, Nc2 };
  enum { mz, nw, nw2, cr2, ck2, ck4, dkk, lw, Nc };

  //  int Nq, Nc;

  inline int f_ld(int i) { return i; }

  inline int f_nw(int i) { return i + Lsum[nw - 1]; }

  inline int f_nw2(int i) { return i + Lsum[nw2 - 1]; }

  inline int f_gf(int i) { return i + Lsum[cr2 - 1]; }
  //  inline int f_gf2( int i ){ return i+Lsum[cr4-1]; }

  //  inline int f_sk( int i ) { return i+Lsum[sk -1]; }
  inline int f_nk(int i) { return i + Lsum[ck2 - 1]; }

  inline int f_gk2(int i) { return i + Lsum[ck4 - 1]; }

  inline int f_noise(int i) { return i + Lsum[dkk - 1]; }

  inline int f_lw(int i) { return i + Lsum[lw - 1]; }

  inline int f_nkr(int k) { return k + Lsum[ck2 - 1]; }

  inline int f_nki(int k) { return k + Lsum[ck2 - 1] + Nkmax; }

  /*  inline int f_gf2( int r1, int r2 ){
      return r1 + r2*V + Lsum[cr4-1];
      }*/
  inline int f_gk2r(int k, int k_) { return k + k_ * Nkxmax + Lsum[ck4 - 1]; }

  inline int f_gk2i(int k, int k_) {
    return k + k_ * Nkxmax + Lsum[ck4 - 1] + Nkkmax * Nkxmax;
  }

  inline int f_noise(int k, int k_) { return k + k_ * Nkxmax + Lsum[dkk - 1]; }

  inline int f_ck(int k, int num, int a) {
    return 5 + k + num * Nkmax + a * Nkmax * Cknum;
  };

  inline int f_ck(int k, int num) { return 5 + k + num * Nkmax; };

  //  inline int theta(int i, int k){ return i+(k+Nkxmax)*V; };
  inline int theta(int i, int k, int a) {
    return i + (k + (2 * a + 1) * Nkmax) * V;
  }

  int *Lmax, *Lsum;

  void MCsum_S();

  double *values_S;
  double *MCmean_S;
  double *BINmean_S;
  //  double *RNDmean_S;

  void MCsum_L();

  double *values_k;
  double *values_L;
  double *MCmean_L;
  double *BINmean_L;
  //  double *RNDmean_L;

  double *m_val;
  double *AC;
  double *MCmeanACT;
  double *RNDmeanACT;

  char **Qname;
  char **file;
  char **acfile;

  void BINsum(double *MCmean, double *BINmean, int Lmax, int bin);

  void Average(double *g, int Nval, int S, double *MCmean, int kstep);

  void show_S(ofstream &F);

  void show_L();

  void RNDsum(double *local, double *global, int Ndiv, int my_rank, int rmax2,
              int Npara, int);

  void TreeSum(int num, double *child, double Normalization, int cnum, int mnum,
               int Nsum);

  void TreeSum(int num, double *child, double *mother, double Normalization,
               int cnum, int mnum, int Nsum);

  int V, Nx, NUM;
  System *sp;
  Size *N;
  Parallel *PR;
  Lattice *LT;
  MC_p *MC;
  int NL[3];

  void autocorrelation(int mcs, double q) { AC[mcs] = q; };

  void AutoCorrelationAverage(char *fname);

  //  private:

  int Npara;
  int my_rank;
  int Kcut, Kcut2;
  int Nxmax, Nkxmax, Nkmax, Nkkmax, Nkstep, Nxstep, Nk_set, Sk_set;

  complex<double> *EXPrk, *Ck, *Ck_m;

  double SFD_Norm;
  double cr0, an0;

 public:
#ifdef SF
  int Ntau;
  int Ntau1;
  int NKMAX, NSF;
  double dtau;
  double **COSrk, **SINrk;

  double **BINmean_SF;
  double **MCmean_SF;
  double **counter4SFC;
  double **counter4SFS;
  double **sfsamp;

  void show4SF(FILE *);

  void MCsum_SF();

  void read_sf();

  void reset4SF() {
    for (int isf = 0; isf < NKMAX; isf++) {
      for (int it = 0; it < Ntau1; it++) {
        counter4SFC[isf][it] = 0.0;
        counter4SFS[isf][it] = 0.0;
      }
    }
  };

  void dump(FILE *);
#endif
  double *an, *cr, *Q, *ca, *ac;

  char parainfo[64];

  Quantities(Size *m_N, MC_p *m_MC, System *m_sp, Lattice *m_LT, Parallel *m_PR,
             std::string const &sfinfile);

  ~Quantities();

  void Init();

  void Output(std::string const &fname, double g);

  void Measure(int Nv, int Nk, vector<GraphSpace::Vertex> &ev,
               vector<GraphSpace::Vertex *> WORM, GraphSpace::Vertex *world,
               GraphSpace::Vertex *worldB, double length, int m_Wnum, int mcs);

  void Measure();

  void Average();

  void BINsum(int);

  void BINaverage();

  void show(ofstream &F, FILE *SFF);
};

#endif // SRC_PMWA_INC_QUANTITIES_H_
