/*---------------------------------------------

   Generating cf.xml for a hypercubic lattice

----------------------------------------------*/

#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#include "../util.hpp"

using namespace std;

std::vector<int> index2coord(int i, std::vector<int> L) {
  if (i < 0) {
    std::stringstream ss;
    ss << "invalid index i=";
    ss << i;
    util::ERROR(ss.str().c_str());
  }
  const int D = L.size();
  int N = 1;
  std::vector<int> r(D);
  for (int d = 0; d < D; ++d) {
    r[d] = i % L[d];
    i /= L[d];
    N *= L[d];
  }
  if (i >= N) {
    std::stringstream ss;
    ss << "ERROR: invalid index i=";
    ss << i;
    util::ERROR(ss.str().c_str());
  }
  return r;
}

void ShowUsage(std::string const& exename) {
  cout << "usage:\n";
  cout << "    " << exename
       << " [-o filename] D L_1 ... L_D Ntau Ntau_cutoff KTYPE\n";
  cout << "arguments:\n";
  cout << "    D             ... dimension of lattice\n";
  cout << "    L_d           ... the liner size of the lattice\n";
  cout << "                      (must be even)\n";
  cout << "    Ntau          ... number of discretized imaginary time\n";
  cout << "    Ntau_cutoff   ... maximum distance in imaginary time between "
          "two spacetime points\n";
  cout << "    KTYPE         ... type of wavenumbers\n";
  cout << "                      0: kx=pi n/L, n=0, 2, ..., L" << endl;
  cout << "                      1: k/pi = (0,0), (1,0), (0,1), (1,1) [example "
          "in D=2 case]"
       << endl;
  cout << "options:\n";
  cout << "    -o filename   ... output file (default: sf.xml)";
}

void WriteXML(std::vector<int> const& L, int Ntau, int CutoffOfNtau, int KTYPE,
              std::string const& filename) {
  const int D = L.size();
  ofstream fout(filename.c_str());
  fout.precision(15);
  int N = 1;  // number of sites.
  for (int i = 0; i < D; i++) {
    N *= L[i];
  }
  int KMAX;

  std::vector<int> Q(D, 2);

  if (KTYPE == 0) {
    KMAX = L[0] / 2 + 1;
  } else if (KTYPE == 1) {
    KMAX = 1;
    for (int d = 0; d < D; ++d) {
      KMAX *= 2;
    }
  } else {
    cout << " KTYPE should 0 or 1";
    exit(1);
  }

  int NumberOfElements = N * KMAX;

  fout << "<StructureFactor>" << endl << endl;
  fout << "<Comment>" << endl;
  fout << "  " << D << "-dimension hypercubic lattice" << endl;
  fout << "</Comment>" << endl << endl;

  fout << "<Ntau>                   " << Ntau << " </Ntau>" << endl;
  fout << "<NumberOfElements>       " << NumberOfElements
       << " </NumberOfElements>" << endl;
  fout << "<CutoffOfNtau>           " << CutoffOfNtau << " </CutoffOfNtau>"
       << endl;
  fout << "<NumberOfInverseLattice> " << KMAX << " </NumberOfInverseLattice>"
       << endl;

  fout << endl;
  fout << "<!-- <SF> [phase(cos)] [phase(sin)] [isite] [kindx] </SF> -->"
       << endl
       << endl;

  int NB = 0;  // 3 * N ;   // number of bonds

  for (int i = 0; i < N; i++) {
    std::vector<int> r = index2coord(i, L);
    for (int q = 0; q < KMAX; q++) {
      double phase = 0;
      if (KTYPE == 0) {
        phase = (r[0] * 2 * q * M_PI) / L[0];  // r_x * q_x
      } else if (KTYPE == 1) {
        std::vector<int> k = index2coord(q, Q);
        for (int d = 0; d < D; ++d) {
          phase += r[d] * k[d];
        }
        phase *= M_PI;
      }
      double COSrk = cos(phase);
      double SINrk = sin(phase);
      if (fabs(COSrk) < 1.0e-12) COSrk = 0.0;
      if (fabs(SINrk) < 1.0e-12) SINrk = 0.0;

      fout << "<SF> " << COSrk << " " << SINrk << " " << i << " " << q
           << " </SF>" << endl;
    }
  }

  fout << endl;
  fout << "</StructureFactor>" << endl;
}

int main(int argc, char** argv) {
  std::string exename(argv[0]);
  std::string filename("sf.xml");
  if (argc < 3) {
    ShowUsage(exename);
    exit(0);
  }
  if (std::strcmp(argv[1], "-o") == 0) {
    filename = argv[2];
    argc -= 2;
    argv += 2;
  }
  int NARG = 3;
  if (argc < NARG + 1) {
    ShowUsage(exename);
    exit(0);
  }
  int iarg = 1;
  const int D = atoi(argv[iarg]);
  iarg++;
  if (argc != D + 5) {
    ShowUsage(exename);
    exit(0);
  }

  std::vector<int> L(D);

  for (int i = 0; i < D; i++) {
    L[i] = atoi(argv[iarg]);
    iarg++;
  }
  int Ntau = atoi(argv[iarg]);
  iarg++;

  int Ntcut = atoi(argv[iarg]);
  iarg++;

  int KTYPE = atoi(argv[iarg]);

  int EvenOrOdd = 0;
  cout.precision(15);
  cout << "D     = " << D << endl;
  for (int i = 0; i < D; i++) {
    cout << "L_" << i + 1 << "    = " << L[i] << endl;
    EvenOrOdd += L[i] % 2;
  }

  if (EvenOrOdd) {
    cout << "Warnig: L should be an even number." << endl;
  }

  WriteXML(L, Ntau, Ntcut, KTYPE, filename);
  cout << "... done." << endl;
  return 0;
}
