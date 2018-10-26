/*---------------------------------------------

   Generating lattice.xml for a square lattice.

----------------------------------------------*/

#include <cmath>
#include <fstream>
#include <iostream>

#include <cstdlib>

using namespace std;

//--------------------------------------------------------------
void ShowUsage(int argc, char** argv) {
  cout << "usage: $ " << argv[0] << " [ D , L , B ]     \n";
  cout << "    or $ " << argv[0] << " [ D , L0 , L1 , ... , B ] \n";
  cout << "    D ... dimension.                     \n";
  cout << "                                         \n";
  cout << "    L ... the liner size of the lattice. \n";
  cout << "          ( L, L0, L1, ..., must be even number. )\n";
  cout << "    B ... the inverse tempereture.       \n";
  cout << "                                         \n";
}
//-------------------------------------------------------------
void WriteXML(int D, int L[], double B) {
  ofstream fout("lattice.xml");
  fout.precision(15);
  int N = 1;  //number of sites.
  for (int i = 0; i < D; i++) {
    N *= L[i];
  }

  int NumberOfCells            = N;
  int NumberOfSiteTypes        = 1;
  int NumberOfInteractionTypes = 1;
  int NumberOfEdgeInteractions = 0;

  //Opencubic
  int* Lext = new int[D];

  for (int di = 0; di < D; di++) {
    Lext[di] = 1;
    for (int dj = 0; dj < D; dj++) {
      if (di != dj) Lext[di] *= L[dj];
    }
  }

  for (int di = 0; di < D; di++)
    NumberOfEdgeInteractions += Lext[di];

  int NumberOfInteractions = N * D - NumberOfEdgeInteractions;

  int BD = D;  //Opencubic

  fout << "<LATTICE>" << endl << endl;
  fout << "<Comment>" << endl;
  fout << "  " << D << "-dimension open-square lattice" << endl;
  fout << "</Comment>" << endl << endl;

  fout << "<Dimension> " << D << " </Dimension>" << endl;
  fout << "<BondDimension> " << BD << " </BondDimension>" << endl;
  fout << "<LinearSize> ";
  for (int i = 0; i < D; i++) {
    fout << L[i] << " ";
  }
  fout << "</LinearSize>" << endl;
  fout << "<Beta> " << B << " </Beta>" << endl;
  fout << "<NumberOfCells> " << NumberOfCells << " </NumberOfCells>" << endl;
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
  fout << "<!-- <I> [id] [itype] [nbody] [s0] [s1] ... [eid] [edim] </I> -->"
       << endl
       << endl;

  int NB    = D * N;  // number of bonds
  int* x    = new int[D];
  int itype = 0;
  int nbody = 2;
  NB        = 0;
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
        continue;
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
  int NARG = 3;
  if (argc < NARG + 1) {
    ShowUsage(argc, argv);
    exit(0);
  }
  const int D = atoi(argv[1]);
  //  int        L[D] ;
  int* L   = new int[D];  //edit sakakura
  double B = 0.0;

  if (argc == NARG + 1) {
    int lx = atoi(argv[2]);
    for (int i = 0; i < D; i++) {
      L[i] = lx;
    }
    B = (double)atof(argv[3]);
  } else if (argc == D + NARG) {
    for (int i = 0; i < D; i++) {
      L[i] = atoi(argv[2 + i]);
    }
    B = (double)atof(argv[D + NARG - 1]);
  } else {
    cout << "error: D != number of L[]." << endl;
    ShowUsage(argc, argv);
    exit(0);
  }

  int EvenOrOdd = 0;
  cout.precision(15);
  cout << "D     = " << D << endl;
  for (int i = 0; i < D; i++) {
    cout << "L" << i << "    = " << L[i] << endl;
    EvenOrOdd += L[i] % 2;
  }
  cout << "B     = " << B << endl;

  if (EvenOrOdd) { cout << "Warnig: L should be an even number." << endl; }

  WriteXML(D, L, B);
  cout << "... done." << endl;
  delete[] L;
  return 0;
}
