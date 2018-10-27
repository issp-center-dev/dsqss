/*---------------------------------------------

   Generating lattice.xml for a square lattice.

----------------------------------------------*/

#include <iostream>
#include <fstream>
#include <math.h>
#include <cstdlib>
#include <cstdio>
#include <cstring>

using namespace std;

//--------------------------------------------------------------
void ShowUsage(std::string const& exename) {
  cout << "usage:\n";
  cout << "    " << exename << " [-o filename] D L B NLdiv NBdiv NFILED\n";
  cout << "    " << exename << " [-o filename] D L_1 ... L_D B NLdiv NBdiv NFILED\n";
  cout << "arguments:\n";
  cout << "    D ... dimension.                     \n";
  cout << "    L ... the liner size of the lattice. \n";
  cout << "          (must be even number. )\n";
  cout << "    B ... the inverse tempereture.\n";
  cout << "    NLdiv ... the number of decomposition of L. \n";
  cout << "    NBdiv ...  the number of decomposition of B.\n";
  cout << "    NFIELD ...  the number of external fields.\n";
  cout << "                for example, random potential etc...\n";
  cout << "                if no, here is 0.\n";
  cout << "options:\n";
  cout << "    -o filename ... output file (default: lattice.xml)\n";
}

//-------------------------------------------------------------
void WriteXML(int D, int orgL[], double orgB, double orgOB, int NLD, int NBD, int NF, std::string const& filename) {
  ofstream fout(filename.c_str());
  fout.precision(15);

  int L[3];

  for (int i = 0; i < D; i++) {
    L[i] = orgL[i] / NLD;
  }
  double B = orgB / (double)NBD;

  int N = 1;  //number of sites per domain.
  for (int i = 0; i < D; i++) {
    N *= L[i];
  }

  int NumberOfCells            = N;
  int NumberOfInteractions     = N * D;  //z/2*N
  int NumberOfLDecomposition   = NLD;
  int NumberOfBDecomposition   = NBD;
  int NumberOfSiteTypes        = 1;  //sub-lattice
  int NumberOfInteractionTypes = 1;
  int NumberOfExternalField    = NF;

  fout << "<LATTICE>" << endl << endl;
  fout << "<Comment>" << endl;
  fout << "  " << D << "-dimension square lattice" << endl;
  fout << "</Comment>" << endl << endl;

  fout << "<Dimension> " << D << " </Dimension>" << endl;
  fout << "<LinearSize> ";
  for (int i = 0; i < D; i++) {
    fout << orgL[i] << " ";
  }
  fout << "</LinearSize>" << endl;
  fout << "<NumberOfLDecomposition> " << NLD << " </NumberOfLDecomposition>" << endl;
  fout << "<LinearDomainSize> ";
  for (int i = 0; i < D; i++) {
    fout << L[i] << " ";
  }
  fout << "</LinearDomainSize>" << endl;

  fout << "<Beta> " << orgB << " </Beta>" << endl;
  fout << "<OldBeta> " << orgOB << " </OldBeta>" << endl;
  fout << "<NumberOfBDecomposition> " << NBD << " </NumberOfBDecomposition>" << endl;
  fout << "<BetaOfDomain> " << B << " </BetaOfDomain>" << endl;

  fout << "<NumberOfCells> " << NumberOfCells << " </NumberOfCells>" << endl;  //L^d units
  fout << "<NumberOfSites> " << N << " </NumberOfSites>" << endl;              //NumberOfCells*NumberOfSiteTypes
  fout << "<NumberOfInteractions> " << NumberOfInteractions << " </NumberOfInteractions>" << endl;
  fout << "<NumberOfSiteTypes> " << NumberOfSiteTypes << " </NumberOfSiteTypes>" << endl;
  fout << "<NumberOfInteractionTypes> " << NumberOfInteractionTypes << " </NumberOfInteractionTypes>" << endl;
  fout << "<NumberOfExternalField> " << NumberOfExternalField << " </NumberOfExternalField>" << endl;
  fout << endl;

  fout << "<!-- <S> [id] [stype] [mtype] </S> -->" << endl << endl;

  int stype = 0;
  int mtype = 0;

  for (int id = 0; id < N; id++) {
    int p  = id;
    int Nt = N;
    mtype  = 0;
    for (int q = D - 1; q >= 0; q--) {
      Nt /= L[q];
      mtype += p / Nt;
      p = p % Nt;
    }

    mtype = mtype % 2;
    fout << "<S> " << id << " " << stype << " " << mtype << " </S>" << endl;
  }

  fout << endl;
  fout << "<!-- <I> [id] [itype] [nbody] [edge id] [s0] [s1] ... </I> -->" << endl << endl;

  int NB    = D * N;  // number of bonds
  int *x    = new int[D];
  int itype = 0;
  int nbody = 2;
  int eid   = 0, etype;
  NB        = 0;

  for (int i = 0; i < N; i++) {
    for (int p = 0; p < D; p++) {
      int k = i;
      for (int q = 0; q < D; q++) {
        x[q] = k % L[q];
        k /= L[q];
      }

      if (x[p] == L[p] - 1) {
        etype = eid;
        eid++;
      } else
        etype = -1;

      x[p]  = (x[p] + 1) % L[p];
      int j = 0;
      for (int q = D - 1; q >= 0; q--) {
        j *= L[q];
        j += x[q];
      }

      fout << "<I> " << NB << " " << itype << " " << nbody << " " << etype << " " << i << " " << j << " </I>" << endl;

      NB++;
    }
  }

  fout << endl;
  fout << "</LATTICE>" << endl;

  delete[] x;
}

int main(int argc, char **argv) {
  std::string exename(argv[0]);
  std::string filename("lattice.xml");
  if(argc < 3){
    ShowUsage(exename);
    exit(0);
  }
  if(std::strcmp(argv[1], "-o")==0){
    filename = argv[2];
    argc -= 2;
    argv += 2;
  }
  int NARG = 6;
  if (argc < NARG + 1) {
    ShowUsage(exename);
    exit(0);
  }
  const int D = atoi(argv[1]);
  //  int        L[D] ;
  int *L   = new int[D];  //edit sakakura
  double B = 0.0;
  int NLD  = 1;
  int NBD  = 1;
  int NF   = 0;

  if (argc == NARG + 1) {
    int lx = atoi(argv[2]);
    for (int i = 0; i < D; i++) {
      L[i] = lx;
    }
    B   = (double)atof(argv[3]);
    NLD = atoi(argv[4]);
    NBD = atoi(argv[5]);
    NF  = atoi(argv[6]);
  } else if (argc == D + NARG) {
    for (int i = 0; i < D; i++) {
      L[i] = atoi(argv[2 + i]);
    }
    B   = (double)atof(argv[D + NARG - 4]);
    NLD = atoi(argv[D + NARG - 3]);
    NBD = atoi(argv[D + NARG - 2]);
    NF  = atoi(argv[D + NARG - 1]);
  } else {
    cout << "error: D != number of L[]." << endl;
    ShowUsage(exename);
    exit(0);
  }

  for (int i = 0; i < D; i++) {
    if (L[i] % NLD != 0) {
      cout << "error!: L/NLD must be the integer value." << endl;
      exit(0);
    }
  }

  int EvenOrOdd = 0;
  cout.precision(15);
  cout << "D     = " << D << endl;
  for (int i = 0; i < D; i++) {
    cout << "L" << i << "    = " << L[i] << endl;
    EvenOrOdd += L[i] % 2;
  }
  cout << "B     = " << B << endl;
  cout << "NLdiv = " << NLD << endl;
  cout << "NBdiv = " << NBD << endl;
  cout << "NFIELD = " << NF << endl;

  if (EvenOrOdd) { cout << "Warnig: L should be an even number." << endl; }

  WriteXML(D, L, B, B, NLD, NBD, NF, filename);
  cout << "... done." << endl;
  delete[] L;
  return 0;
}
