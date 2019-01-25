#include <boost/lexical_cast.hpp>

#include "../common/version.h"

#include <PMWA.h>

#include "../common/read_keyvalues.h"
#include "../common/tostring.h"

int main(int argc, char **argv) {
  Dla Sim(argc, argv);

  ////////////////////////////////////////////////////////////
  Sim.PMWA();
  ////////////////////////////////////////////////////////////

  return 0;
}

Dla::Dla(int NP, char **PLIST) {
  int p_num, my_rank;

  MPI_Init(&NP, &PLIST);
  MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
  MPI_Comm_size(MPI_COMM_WORLD, &p_num);

  MPI_Barrier(MPI_COMM_WORLD);
  allstart = MPI_Wtime();

  pstart = pend = tstart = tend = ostart = oend = 0.0;

  ReadParameterfile(p_num, my_rank, NP, PLIST);
}

Dla::~Dla() {
  MPI_Barrier(MPI_COMM_WORLD);
  allend = MPI_Wtime();

  if (PR.nst == 0) {
    show_TIME();
    ftest.close();
#ifdef SF
    fclose(FOUT4SF);
#endif
  }

  MPI_Finalize();
}

//*************************************************************************************************

double Dla::PMWA() {
  Lattice LT(latfile);

  if(!std::isinf(BETA)){
    LT.set_beta(BETA);
  }
  PR.FlgRestart = (MC.runtype == Restart);
  if(PR.FlgRestart) {
      if (!std::isinf(oldBETA)) {
          LT.set_oldbeta(oldBETA);
          PR.FlgAnneal = true;
          PR.FlgRestart = false;
      } else PR.FlgAnneal = false;
  }

  LT.make_Size(&N);
  LT.make_Parallel(&PR);

  NameOutfiles();

  My_rdm MR(MC.seed);

  Probability P(&N, &sp, &PR);
  P.LookUpTable(&N, &sp);

  Configuration CS(&MC, &N, sp.nmax, &LT, &P, &PR, IMAX, WMAX, Eventfile_old, &MR, (bool)cb, outfile);
  Quantities QNT(&N, &MC, &sp, &LT, &PR, sfinpfile);

  //##########################################################################
  ////////////////// Determination of Ncyc ////////////////////////
  if (MC.nc <= 1) {
    MPI_Barrier(MPI_COMM_WORLD);
    pstart = MPI_Wtime();

    CS.DeterminationNworm(MC.Ntest, &MR, &QNT); // NCyc

    MPI_Barrier(MPI_COMM_WORLD);
    pend = MPI_Wtime();
  }
  if (PR.nst == 0) {
    ftest << "C This is PMWA ver.0" << endl << endl;
    LT.show_param(ftest);
    CS.dump(ftest);
#ifdef SF
    fprintf(FOUT4SF, "C This is PMWA ver.0\n\n");
    QNT.dump(FOUT4SF);
    fclose(FOUT4SF);
    FOUT4SF = fopen(sfoutfile, "a");
#endif
  }

  if (MC.nc == 1) {
    CS.Output(Eventfile.c_str(), &MR);
    return 1;
  }

  if(PR.my_rank==0){
    cout << "start thermalization" << endl;
  }
  ////////////////// Thermalization ////////////////////////
  if (MC.nc < 3) {
    MPI_Barrier(MPI_COMM_WORLD);
    tstart = MPI_Wtime();

    CS.sweep(MC.Nthermal, &MR, &QNT);

    MPI_Barrier(MPI_COMM_WORLD);
    tend = MPI_Wtime();
  }

  if (MC.nc == 2) {
    CS.Output(Eventfile.c_str(), &MR);
    return 2;
  }

  /////////////////////////////////////////////////////////
  ////////////////// Observation ////////////////////////
  /////////////////////////////////////////////////////////
  MPI_Barrier(MPI_COMM_WORLD);
  ostart = MPI_Wtime();

  // this function includes all (e.g., sweep)
  CS.measurement(&QNT, &MR);

  if (PR.nst == 0) QNT.show(ftest, FOUT4SF);

  MPI_Barrier(MPI_COMM_WORLD);
  oend = MPI_Wtime();

  ////////////////// Output the Configuration ////////////////////////
  CS.Output(Eventfile.c_str(), &MR);

  return 0;
}

void Dla::NameOutfiles() {
  outfile += std::string(".") + tostring(PR.np);
  sfoutfile += std::string(".") + tostring(PR.np);

  if (PR.nst == 0) {
    ftest.open(outfile.c_str(), std::ios::app);
    ftest << setprecision(16);
#ifdef SF
    FOUT4SF = fopen(sfoutfile.c_str(), "a");
    //   cout<<"sf start"<<endl;
#endif
  }

#ifdef DEBUG
  show_MPIP();
  show_SP();
#endif

  Eventfile = std::string("evout_");
  Eventfile += outfile;
  Eventfile += std::string("_rank") + tostring(PR.my_rank) + std::string(".dat");
  if (MC.runtype != Normal) {
    Eventfile_old = Eventfile;
  }
}

void Dla::ReadParameterfile(int m_pnum, int m_myrank, int NP, char **PLIST) {
  using boost::lexical_cast;

  PR.p_num   = m_pnum;
  PR.my_rank = m_myrank;

  if (NP < 2) {
    std::string msg("Dla::RealBuffer> ERROR: no input file ");
    throw std::runtime_error(msg);
  }

  std::map<std::string, std::string> dict;
  init_paramdict(dict);
  read_keyvalues(dict, PLIST[1]);

  deprecated_parameter(dict, "npre", "ntest");
  deprecated_parameter(dict, "ntherm", "nmcse");
  deprecated_parameter(dict, "ndecor", "nmcsd");

  MC.runtype  = lexical_cast<int>(dict["runtype"]);
  MC.Nbin     = lexical_cast<int>(dict["nset"]);
  MC.Nsample  = lexical_cast<int>(dict["nmcs"]);
  MC.Nthermal = lexical_cast<int>(dict["ntherm"]);
  MC.Nstep    = lexical_cast<int>(dict["ndecor"]);
  MC.Ntest    = lexical_cast<int>(dict["npre"]);
  MC.seed     = lexical_cast<int>(dict["seed"]);

  BETA = lexical_cast<double>(dict["beta"]);
  oldBETA = lexical_cast<double>(dict["oldbeta"]);

  deprecated_parameter(dict, "t", "tb");
  deprecated_parameter(dict, "u", "ub");
  deprecated_parameter(dict, "v", "vbb");

  sp.tb  = lexical_cast<double>(dict["t"]);
  sp.Ubb = lexical_cast<double>(dict["u"]);
  sp.Vb1 = lexical_cast<double>(dict["v"]);
  sp.mu  = lexical_cast<double>(dict["mu"]);
  sp.Htr = lexical_cast<double>(dict["g"]);
  sp.nmax = lexical_cast<int>(dict["nmax"]);

  cb   = lexical_cast<int>(dict["cb"]);
  IMAX = lexical_cast<int>(dict["nvermax"]);
  WMAX = lexical_cast<int>(dict["nwormax"]);

  algfile   = dict["algfile"];
  latfile   = dict["latfile"];
  sfinpfile = dict["sfinpfile"];

  outfile   = dict["outfile"];
  sfoutfile = dict["sfoutfile"];

  N.d = (N.y == 1) ? 1 : ((N.z == 1) ? 2 : 3);

  MC.seed += PR.my_rank;

  sp.Eu = PR.V * sp.Ubb / 2.0 * sp.nmax * (sp.nmax - 1.0);

  N.V = N.x * N.y * N.z;
}

void Dla::init_paramdict(std::map<std::string, std::string> &dict) {
  dict["cb"]     = "0";
  dict["nset"]   = "10";
  dict["nmcs"]   = "1000";
  dict["npre"]   = "1000";
  dict["ntherm"] = "1000";
  dict["ndecor"] = "1000";
  dict["seed"]   = "13";

  dict["nvermax"] = "100000000";
  dict["nwormax"] = "1000";

  dict["beta"] = "Inf";
  dict["oldbeta"] = "Inf";

  dict["g"]    = "0.2";
  dict["t"]   = "1.0";
  dict["u"]  = "0.0";
  dict["v"]  = "3.0";
  dict["mu"]   = "1.0";
  dict["nmax"] = "1";

  dict["algfile"]   = "algorithm.xml";
  dict["latfile"]   = "lattice.xml";
  dict["sfinpfile"] = "sf.xml";
  dict["outfile"]   = "res.dat";
  dict["sfoutfile"] = "sf.out";
  dict["cfoutfile"] = "cf.out";
}

void Dla::set_Parallel() {
  PR.B    = N.B / (double)PR.Ntdiv;     // beta for a domain.
  PR.oldB = N.oldB / (double)PR.Ntdiv;  // for annealing.

  PR.Nsdiv = PR.Nxdiv * PR.Nydiv * PR.Nzdiv;  // the number of spatial decompositions.

  PR.x = N.x / PR.Nxdiv;
  PR.y = N.y / PR.Nydiv;
  PR.z = N.z / PR.Nzdiv;

  PR.V = PR.x * PR.y * PR.z;

  PR.NtNs  = PR.Ntdiv * PR.Nsdiv;  //the number of decompositions.
  PR.Npara = PR.p_num / PR.NtNs;   //the number of seeds for trivial parallelization.

  PR.nt = PR.my_rank % PR.Ntdiv;                    // the temporal domain number for the processor.
  PR.ns = (int)(PR.my_rank / PR.Ntdiv) % PR.Nsdiv;  // the spatial domain number for the processor.

  PR.nx = PR.ns % PR.Nxdiv;               // the x-directional domain number for the processor.
  PR.ny = (PR.ns / PR.Nxdiv) % PR.Nydiv;  // the y-directional domain number for the processor.
  PR.nz = PR.ns / (PR.Nxdiv * PR.Nydiv);  // the z-directional domain number for the processor.

  PR.nst = PR.my_rank % PR.NtNs;    // the domain number for the processor.
  PR.np  = PR.my_rank / (PR.NtNs);  // the seed number of the trivial parallelization for the processor.

  if (PR.Rpara > 0) {
    PR.nr = PR.np % PR.Rpara;  // a random potential number (one of trivial parallelization)
    PR.nq =
        PR.np
        / PR.Rpara;  // a seed number of the trivial parallelization for the random potential (one of trivial parallelization)
  } else {
    PR.nr = 0;
    PR.nq = PR.np;
  }

  //the coordinate is (nt,nx,ny,nz,np)

  PR.nst0 = PR.np * PR.NtNs;                     // nst=0 process number for the processor.
  PR.nt0  = PR.ns * PR.Ntdiv + PR.np * PR.NtNs;  //nt=0 process number for the processor.
  PR.ns0  = PR.nt + PR.np * PR.NtNs;             //ns=0 process number for the processor.
  PR.nx0  = PR.nt + (PR.ny * PR.Nxdiv + PR.nz * PR.Nxdiv * PR.Nydiv) * PR.Ntdiv
           + PR.np * PR.NtNs;  //nx=0 process number for the processor.

  PR.upper = (PR.nt + 1) % PR.Ntdiv + PR.ns * PR.Ntdiv
             + PR.np * PR.Ntdiv * PR.Nsdiv;  //the upper process number for the temporal direction.
  PR.lower = (PR.nt - 1 + PR.Ntdiv) % PR.Ntdiv + PR.ns * PR.Ntdiv
             + PR.np * PR.Ntdiv * PR.Nsdiv;  //the lower process number the temporal direction.

  PR.right[0] = PR.nt + ((PR.nx + 1) % PR.Nxdiv + PR.ny * PR.Nxdiv + PR.nz * PR.Nxdiv * PR.Nydiv) * PR.Ntdiv
                + PR.np * PR.Ntdiv * PR.Nsdiv;  //the right side process number for the x direction.
  PR.left[0] = PR.nt + ((PR.nx - 1 + PR.Nxdiv) % PR.Nxdiv + PR.ny * PR.Nxdiv + PR.nz * PR.Nxdiv * PR.Nydiv) * PR.Ntdiv
               + PR.np * PR.Ntdiv * PR.Nsdiv;  //the left side process number for the x direction.

  PR.right[1] = PR.nt + (PR.nx + ((PR.ny + 1) % PR.Nydiv) * PR.Nxdiv + PR.nz * PR.Nxdiv * PR.Nydiv) * PR.Ntdiv
                + PR.np * PR.Ntdiv * PR.Nsdiv;  //the right side process number for the y direction.
  PR.left[1] = PR.nt + (PR.nx + ((PR.ny - 1 + PR.Nydiv) % PR.Nydiv) * PR.Nxdiv + PR.nz * PR.Nxdiv * PR.Nydiv) * PR.Ntdiv
               + PR.np * PR.Ntdiv * PR.Nsdiv;  //the left side process number for the y direction.

  PR.right[2] = PR.nt + (PR.nx + PR.ny * PR.Nxdiv + ((PR.nz + 1) % PR.Nzdiv) * PR.Nxdiv * PR.Nydiv) * PR.Ntdiv
                + PR.np * PR.Ntdiv * PR.Nsdiv;  //the right side process number for the z direction.
  PR.left[2] = PR.nt + (PR.nx + PR.ny * PR.Nxdiv + ((PR.nz - 1 + PR.Nzdiv) % PR.Nzdiv) * PR.Nxdiv * PR.Nydiv) * PR.Ntdiv
               + PR.np * PR.Ntdiv * PR.Nsdiv;  //the left side process number for the z direction.
}
