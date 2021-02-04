/*---------------------------------------------

   Generating cf.xml for a hypercubic lattice

----------------------------------------------*/

#include <cmath>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>
#include <string>

using namespace std;

void ShowUsage(std::string const& exename) {
  cout << "usage:\n";
  cout << "    " << exename << " [-o filename] D L_1 ... L_D Ntau\n";
  cout << "arguments:\n";
  cout << "    D           ... dimension of lattice\n";
  cout << "    L_1 ... L_D ... the liner size of the lattice. \n";
  cout << "                    (must be even number. )\n";
  cout << "    Ntau        ... the number of discretized imaginary time\n";
  cout << "options:\n";
  cout << "    -o filename ... output file (default: cf.xml)";
}

void WriteXML(int D, int L[], int Ntau, std::string const& filename) {
  ofstream fout(filename.c_str());
  fout.precision(15);
  int N = 1;  // number of sites.
  for (int i = 0; i < D; i++) {
    N *= L[i];
  }

  int NumberOfElements = N * N;
  int NumberOfKinds = N;

  fout << "<CorrelationFunction>" << endl << endl;
  fout << "<Comment>" << endl;
  fout << "  " << D << "-dimension hypercubic lattice" << endl;
  fout << "</Comment>" << endl << endl;

  fout << "<Ntau>             " << Ntau << " </Ntau>" << endl;
  fout << "<NumberOfElements> " << NumberOfElements << " </NumberOfElements>"
       << endl;
  fout << "<NumberOfKinds>    " << NumberOfKinds << " </NumberOfKinds>" << endl;
  fout << endl;

  fout << "<!-- <CF> [kind] [isite] [jsite] </CF> -->" << endl << endl;

  int NB = 0;  // 3 * N ;   // number of bonds
  int* x = new int[D];
  int* dx = new int[D];

  int kind = 0;

  for (int di = 0; di < N; di++) {
    int dk = di;
    for (int q = 0; q < D; q++) {
      dx[q] = dk % L[q];
      dk /= L[q];
    }

    for (int i = 0; i < N; i++) {
      int k = i;
      for (int q = 0; q < D; q++) {
        x[q] = k % L[q];
        k /= L[q];
      }
      for (int q = 0; q < D; q++) {
        x[q] = (x[q] + dx[q]) % L[q];
      }
      int j = 0;
      for (int q = D - 1; q >= 0; q--) {
        j *= L[q];
        j += x[q];
      }

      fout << "<CF> " << kind << " " << i << " " << j << " </CF>" << endl;
    }
    kind++;
  }
  cout << "NumberOfKinds = " << kind << endl;
  fout << endl;
  fout << "</CorrelationFunction>" << endl;

  delete[] x;
}

int main(int argc, char** argv) {
  std::string exename(argv[0]);
  std::string filename("cf.xml");
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
  if (argc != D + 3) {
    ShowUsage(exename);
    exit(0);
  }

  int* L = new int[D];

  for (int i = 0; i < D; i++) {
    L[i] = atoi(argv[iarg]);
    iarg++;
  }
  int Ntau = atoi(argv[iarg]);

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

  WriteXML(D, L, Ntau, filename);
  cout << "... done." << endl;
  delete[] L;
  return 0;
}
