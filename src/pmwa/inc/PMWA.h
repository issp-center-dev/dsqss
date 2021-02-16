#ifndef SRC_PMWA_INC_PMWA_H_
#define SRC_PMWA_INC_PMWA_H_

#include <Configuration.h>
#include <Probability.h>

#include <map>
#include <string>

namespace pmwa_para {
enum sim {
  RUN_TYPE,
  N_SET,
  N_MCSE,
  N_MCSD,
  N_MCS,
  N_TEST,
  N_SEED,
  N_VERMAX,
  N_WORMAX,
  N_PARA_SIM
};
enum file {
  ALG_FILE = N_PARA_SIM,
  LAT_FILE,
  SF_INPFILE,
  OUT_FILE,
  SF_OUT_FILE,
  N_PARA_FILE
};
enum model {
  G = N_PARA_FILE,
  UBB,
  VB1,
  TB,
  MU,
  CB,
  BETA,
  OLD_BETA,
  N_PARA_MODEL
};
enum other { N_MAX = N_PARA_MODEL, NUMPARA };
};  // namespace pmwa_para

//////////////////////////////////////////////////////////////////////////////////////////

class Dla {
 private:
  Size N;
  System sp;
  MC_p MC;

  Parallel PR;

  double BETA, oldBETA;

  long long IMAX, WMAX;
  double allstart, allend;
  double pstart, pend, tstart, tend, ostart, oend;
  int cb;

  std::string confdir;
  std::string outfile;
  std::string sfinpfile;
  std::string sfoutfile;
  std::string algfile;
  std::string latfile;
  std::string Eventfile;
  std::string Eventfile_old;

  std::ofstream ftest;
  FILE *FOUT4SF;  // file handler for the output file

  void init_paramdict(std::map<std::string, std::string> &dict);

  void set_Parallel();

  void ReadParameterfile(int p_num, int my_rank, int NP, char **PLIST);

  void NameOutfiles();

  enum { Normal, Restart };  // for runtype

 public:
  double PMWA();

  Dla(int NP, char **PLIST);

  ~Dla();

 private:
  inline void show_MPIP() {
    cout << "MPI check ---> rank= " << PR.my_rank << ", V= " << PR.V
         << " B=  " << PR.B << ", Nx= " << PR.x << ", Ny= " << PR.y
         << ", Nz= " << PR.z << ",, nt= " << PR.nt << ", nx= " << PR.nx
         << ", ny= " << PR.ny << ", nz= " << PR.nz << ", up= " << PR.upper
         << ", lower= " << PR.lower << ", right(x)= " << PR.right[0]
         << ", left(x)= " << PR.left[0] << ", right(y)= " << PR.right[1]
         << ", left(y)= " << PR.left[1] << endl;
  }

  inline void show_SP() {
    std::cout << "step =" << MC.Nstep << " thermal=" << MC.Nthermal
              << "  bin=" << MC.Nbin << "  B=" << N.B << ",  Nx=" << N.x
              << ",  Ny=" << N.y << "  Nz=" << N.z << "  IMAX=" << IMAX
              << "  WMAX=" << WMAX << "  U=" << sp.Ubb << "  V=" << sp.Vb1
              << " Ntest =" << MC.Ntest << "  Nsample=" << MC.Nsample
              << "  nc=" << MC.nc << "  t=" << sp.tb << "  mu=" << sp.mu
              << " seed =" << MC.seed << "  nmax=" << sp.nmax << "  Nd=" << N.d
              << "  Htr=" << sp.Htr << "  Ntvid=" << PR.Ntdiv
              << "  Npara=" << PR.Npara << "  Nsdiv=" << PR.Nsdiv
              << " outfile=" << outfile << std::endl;
  }

  inline void show_TIME() {
    ftest << "Testing Time for Ncyc = " << pend - pstart << endl;
    ftest << "Thermalization Time   = " << tend - tstart << endl;
    ftest << "Measuring Time        = " << oend - ostart << endl;
    ftest << "Total Simulation Time = " << allend - allstart << endl;
  }
};

#endif  // SRC_PMWA_INC_PMWA_H_
