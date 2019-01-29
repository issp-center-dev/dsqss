/*---------------------------------------------

   Generating lattice.xml for a triangular lattice.

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
  cout << "                                         \n";
  cout << "    L ... the liner size of the lattice. \n";
  cout << "          ( L, L0, L1, ... )\n";
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

  int BD = 3;  //Triangular

  int NumberOfInteractions     = N * BD;
  int NumberOfSiteTypes        = 1;
  int NumberOfInteractionTypes = 1;
  int NumberOfEdgeInteractions = 0;

  //Triangular
  int* Lext = new int[D];

  for (int di = 0; di < D; di++) {
    Lext[di] = 1;
    for (int dj = 0; dj < D; dj++) {
      if (di != dj) Lext[di] *= 2 * L[dj];
    }
  }

  for (int di = 0; di < D; di++) {
    NumberOfEdgeInteractions += Lext[di];
  }

  NumberOfEdgeInteractions--;  //double count of the corner site.

  fout << "<LATTICE>" << endl << endl;
  fout << "<Comment>" << endl;
  fout << "  " << D << "-dimension triangular lattice" << endl;
  fout << "</Comment>" << endl << endl;

  fout << "<Dimension> " << D << " </Dimension>" << endl;
  fout << "<BondDimension> " << BD << " </BondDimension>" << endl;
  fout << "<LinearSize> ";
  for (int i = 0; i < D; i++) {
    fout << L[i] << " ";
  }
  fout << "</LinearSize>" << endl;
  fout << "<Beta> " << B << " </Beta>" << endl;
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

  int stype   = 0;
  int mtype   = 0;
  double cosq = 0.0;
  double sinq = 0.0;
  double PI   = M_PI;
  double Qx   = 4.0 * PI / 3.0;
  double Qy   = 0.0;
  int ri[2];

  for (int id = 0; id < N; id++) {
    int p  = id;
    int Nt = N;
    mtype  = 0;
    for (int q = D - 1; q >= 0; q--) {
      Nt /= L[q];
      ri[q] = p / Nt;
      mtype += ri[q];
      p = p % Nt;
    }

    mtype = mtype % 2;

    double rx    = (double)ri[0] - 0.5 * (double)ri[1];
    double phase = rx * Qx;

    cosq = cos(phase);
    sinq = sin(phase);
    if (fabs(cosq) < 1.0e-12) cosq = 0.0;
    if (fabs(sinq) < 1.0e-12) sinq = 0.0;

    fout << "<S> " << id << " " << stype << " " << mtype << " " << cosq << " "
         << sinq << " </S>" << endl;
  }

  fout << endl;
  fout << "<!-- <I> [id] [itype] [nbody] [s0] [s1] ... [eid] [edim] </I> -->"
       << endl
       << endl;

  int NB    = 0;  // number of bonds
  int* x    = new int[D];
  int itype = 0;
  int nbody = 2;
  int etype;
  int eid = 0;
  int p;
  int j;

  for (int i = 0; i < N; i++) {
    int k = i;
    x[0]  = k % L[0];
    x[1]  = k / L[0];

    etype = -1;
    p     = -1;
    j     = (x[0] + 1) % L[0] + x[1] * L[0];
    if (x[0] == L[0] - 1) {
      etype = eid;
      eid++;
      p = 0;
    }
    fout << "<I> " << NB << " " << itype << " " << nbody << " " << i << " " << j
         << " " << etype << " " << p << " </I>" << endl;
    NB++;

    etype = -1;
    p     = -1;
    j     = x[0] + ((x[1] + 1) % L[1]) * L[0];
    if (x[1] == L[1] - 1) {
      etype = eid;
      eid++;
      p = 1;
    }
    fout << "<I> " << NB << " " << itype << " " << nbody << " " << i << " " << j
         << " " << etype << " " << p << " </I>" << endl;
    NB++;

    etype = -1;
    p     = -1;
    j     = (x[0] + 1) % L[0] + ((x[1] + 1) % L[1]) * L[0];
    if (x[0] == L[0] - 1 && x[1] == L[1] - 1) {
      etype = eid;
      eid++;
      p = 2;
    } else if (x[0] == L[0] - 1) {
      etype = eid;
      eid++;
      p = 0;
    } else if (x[1] == L[1] - 1) {
      etype = eid;
      eid++;
      p = 1;
    }

    fout << "<I> " << NB << " " << itype << " " << nbody << " " << i << " " << j
         << " " << etype << " " << p << " </I>" << endl;
    NB++;
  }

  fout << endl;
  fout << "<!-- <V> [bond dim] [unit vectorx]...  </V> -->" << endl << endl;

  fout << "<V> " << 0 << " " << 1 << " " << 0 << " </V>" << endl;
  fout << "<V> " << 1 << " " << 0 << " " << 1 << " </V>" << endl;
  fout << "<V> " << 2 << " " << 1 << " " << 1 << " </V>" << endl;

  fout << endl;
  fout << "</LATTICE>" << endl;

  delete[] x;
  delete[] Lext;
}
//--------------------------------------------------------------

int main(int argc, char** argv) {
  int NARG = 2;
  if (argc < NARG + 1) {
    ShowUsage(argc, argv);
    exit(0);
  }
  const int D = 2;
  int* L      = new int[D];
  double B    = 0.0;

  if (argc == NARG + 1) {
    int lx = atoi(argv[1]);
    for (int i = 0; i < D; i++) {
      L[i] = lx;
    }
    B = (double)atof(argv[2]);
  } else if (argc == D + NARG) {
    for (int i = 0; i < D; i++) {
      L[i] = atoi(argv[1 + i]);
    }
    B = (double)atof(argv[NARG + 1]);
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

  //  if( EvenOrOdd ) { cout<<"Warnig: L should be an even number."<<endl;}

  WriteXML(D, L, B);
  cout << "... done." << endl;
  delete[] L;
  return 0;
}
