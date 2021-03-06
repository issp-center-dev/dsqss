/*---------------------------------------------

   Generating lattice.xml for a square lattice.

----------------------------------------------*/

#include <cmath>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>
#include <string>

using namespace std;

//--------------------------------------------------------------
void ShowUsage(std::string const& exename) {
  cout << "usage:\n";
  cout << "    " << exename << " [-o output] D L\n";
  cout << "    " << exename << " [-o output] D L_1 ... L_D\n";
  cout << "arguments:\n";
  cout << "    D ... dimension.\n";
  cout << "    L ... the liner size of the lattice. \n";
  cout << "          ( L, L1, ... must be even number. )\n";
  cout << "option:\n";
  cout << "    -o output ... output file (default: lattice.xml)\n";
}
//-------------------------------------------------------------
void WriteXML(int D, int L[], std::string const& filename) {
  ofstream fout(filename.c_str());
  fout.precision(15);
  int N = 1;  // number of sites.
  for (int i = 0; i < D; i++) {
    N *= L[i];
  }

  int NumberOfInteractions = N * D;
  int NumberOfSiteTypes = 1;
  int NumberOfInteractionTypes = 1;
  int NumberOfEdgeInteractions = 0;

  // Hypercubic
  int* Lext = new int[D];

  for (int di = 0; di < D; di++) {
    Lext[di] = 1;
    for (int dj = 0; dj < D; dj++) {
      if (di != dj) Lext[di] *= L[dj];
    }
  }

  for (int di = 0; di < D; di++) NumberOfEdgeInteractions += Lext[di];

  int BD = D;  // Hypercubic

  fout << "<LATTICE>" << endl << endl;
  fout << "<Comment>" << endl;
  fout << "  " << D << "-dimension square lattice" << endl;
  fout << "</Comment>" << endl << endl;

  fout << "<Dimension> " << D << " </Dimension>" << endl;
  fout << "<BondDimension> " << BD << " </BondDimension>" << endl;
  fout << "<LinearSize> ";
  for (int i = 0; i < D; i++) {
    fout << L[i] << " ";
  }
  fout << "</LinearSize>" << endl;
  fout << "<NumberOfSites> " << N << " </NumberOfSites>" << endl;
  fout << "<NumberOfInteractions> " << NumberOfInteractions
       << " </NumberOfInteractions>" << endl;
  fout << "<NumberOfSiteTypes> " << NumberOfSiteTypes << " </NumberOfSiteTypes>"
       << endl;
  fout << "<NumberOfInteractionTypes> " << NumberOfInteractionTypes
       << " </NumberOfInteractionTypes>" << endl;
  fout << "<NumberOfEdgeInteractions> " << NumberOfEdgeInteractions
       << " </NumberOfEdgeInteractions>" << endl;

  fout << endl;

  fout << "<!-- <S> [id] [stype] [mtype] </S> -->" << endl;

  int stype = 0;
  int mtype = 0;

  for (int id = 0; id < N; id++) {
    int p = id;
    int Nt = N;
    mtype = 0;
    for (int q = D - 1; q >= 0; q--) {
      Nt /= L[q];
      mtype += p / Nt;
      p = p % Nt;
    }

    mtype = mtype % 2;
    fout << "<S> " << id << " " << stype << " " << mtype << " </S>" << endl;
  }

  fout << endl;
  fout << "<!-- <I> [id] [itype] [nbody] [s0] [s1] ... [eid] [edim] </I> -->"
       << endl
       << endl;

  int NB = D * N;  // number of bonds
  int* x = new int[D];
  int itype = 0;
  int nbody = 2;
  NB = 0;
  int etype;
  int eid = 0;

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

      x[p] = (x[p] + 1) % L[p];
      int j = 0;
      for (int q = D - 1; q >= 0; q--) {
        j *= L[q];
        j += x[q];
      }

      fout << "<I> " << NB << " " << itype << " " << nbody << " " << i << " "
           << j << " " << etype << " " << p << " </I>" << endl;

      NB++;
    }
  }

  fout << endl;
  fout << "<!-- <V> [bond dim] [unit vectorx]...  </V> -->" << endl << endl;
  for (int p = 0; p < BD; p++) {
    fout << "<V> " << p << " ";
    for (int q = 0; q < D; q++) {
      if (p == q)
        fout << 1 << " ";
      else
        fout << 0 << " ";
    }
    fout << "</V>" << endl;
  }
  fout << "</LATTICE>" << endl;

  delete[] x;
  delete[] Lext;
}
//--------------------------------------------------------------

int main(int argc, char** argv) {
  std::string exename(argv[0]);
  if (argc < 3) {
    ShowUsage(exename);
    exit(0);
  }
  std::string filename("lattice.xml");
  if (std::strcmp(argv[1], "-o") == 0) {
    filename = argv[2];
    argc -= 2;
    argv = argv + 2;
  }
  int NARG = 3;
  if (argc < NARG) {
    ShowUsage(exename);
    exit(0);
  }
  const int D = atoi(argv[1]);
  //  int        L[D] ;
  int* L = new int[D];  // edit sakakura

  if (argc == NARG) {
    int lx = atoi(argv[2]);
    for (int i = 0; i < D; i++) {
      L[i] = lx;
    }
  } else if (argc == D + NARG - 1) {
    for (int i = 0; i < D; i++) {
      L[i] = atoi(argv[2 + i]);
    }
  } else {
    cout << "error: D != number of L[]." << endl;
    ShowUsage(exename);
    exit(0);
  }

  int EvenOrOdd = 0;
  cout.precision(15);
  cout << "D     = " << D << endl;
  for (int i = 0; i < D; i++) {
    cout << "L" << i << "    = " << L[i] << endl;
    EvenOrOdd += L[i] % 2;
  }

  if (EvenOrOdd) {
    cout << "Warnig: L should be an even number." << endl;
  }

  WriteXML(D, L, filename);
  cout << "... done." << endl;
  delete[] L;
  return 0;
}
