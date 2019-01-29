/*---------------------------------------------

   Generating lattice.xml for a triangular lattice.

----------------------------------------------*/

#include <cstdlib>
#include <cmath>
#include <cstring>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

using namespace std;

void ShowUsage(std::string const& exename) {
  cout << "usage:\n";
  cout << "    " << exename << " [-o output] Lx [Ly] B\n";
  cout << "arguments:\n";
  cout << "    Lx ... the liner size of the lattice in x direction. \n";
  cout << "    Ly ... the liner size of the lattice in y direction. \n";
  cout << "           if omitted, Ly will be the same as Lx\n";
  cout << "    B ... the inverse tempereture.       \n";
  cout << "option:\n";
  cout << "    -o output ... output file (default: lattice.xml)\n";
}
void WriteXML(std::vector<int> const& L, double B, std::string const& filename) {
  const int D = L.size();
  ofstream fout(filename.c_str());
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

  std::vector<int> Lext(D);

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
  fout << "<NumberOfInteractions> " << NumberOfInteractions << " </NumberOfInteractions>" << endl;
  fout << "<NumberOfSiteTypes> " << NumberOfSiteTypes << " </NumberOfSiteTypes>" << endl;
  fout << "<NumberOfInteractionTypes> " << NumberOfInteractionTypes << " </NumberOfInteractionTypes>" << endl;
  fout << "<NumberOfEdgeInteractions> " << NumberOfEdgeInteractions << " </NumberOfEdgeInteractions>" << endl;

  fout << endl;

  fout << "<!-- <S> [id] [stype] [mtype] </S> -->" << endl;

  std::vector<int> x(D);
  for (int id = 0; id < N; id++) {
    x[0]  = id % L[0];
    x[1]  = id / L[0];
    int stype = 0;
    int mtype = (x[0] + x[1]) % 3;

    fout << "<S> " << id << " " << stype << " " << mtype << " </S>" << endl;
  }

  fout << endl;
  fout << "<!-- <I> [id] [itype] [nbody] [s0] [s1] ... [eid] [edim] </I> -->" << endl << endl;

  int NB = 0;  // number of bonds
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
    fout << "<I> " << NB << " " << itype << " " << nbody << " " << i << " " << j << " " << etype << " " << p << " </I>"
         << endl;
    NB++;

    etype = -1;
    p     = -1;
    j     = x[0] + ((x[1] + 1) % L[1]) * L[0];
    if (x[1] == L[1] - 1) {
      etype = eid;
      eid++;
      p = 1;
    }
    fout << "<I> " << NB << " " << itype << " " << nbody << " " << i << " " << j << " " << etype << " " << p << " </I>"
         << endl;
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

    fout << "<I> " << NB << " " << itype << " " << nbody << " " << i << " " << j << " " << etype << " " << p << " </I>"
         << endl;
    NB++;
  }

  fout << endl;
  fout << "<!-- <V> [bond dim] [unit vectorx]...  </V> -->" << endl << endl;

  fout << "<V> " << 0 << " " << 1 << " " << 0 << " </V>" << endl;
  fout << "<V> " << 1 << " " << 0 << " " << 1 << " </V>" << endl;
  fout << "<V> " << 2 << " " << 1 << " " << 1 << " </V>" << endl;

  fout << endl;
  fout << "</LATTICE>" << endl;
}
//--------------------------------------------------------------

int main(int argc, char** argv) {
  std::string exename(argv[0]);
  std::string filename("lattice.xml");
  if (argc < 3) {
    ShowUsage(exename);
    exit(0);
  }
  if (std::strcmp(argv[1], "-o") == 0) {
    filename = argv[2];
    argc -= 2;
    argv = argv + 2;
  }
  int NARG = 2;
  if (argc < NARG + 1) {
    ShowUsage(exename);
    exit(0);
  }
  const int D = 2;
  std::vector<int> L(D);
  double B = 0.0;

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
  cout << "B     = " << B << endl;

  //  if( EvenOrOdd ) { cout<<"Warnig: L should be an even number."<<endl;}

  WriteXML(L, B, filename);
  cout << "... done." << endl;
  return 0;
}
