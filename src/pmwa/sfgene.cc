/*---------------------------------------------

   Generating cf.xml for a hypercubic lattice

----------------------------------------------*/

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include <fstream>
#include <iostream>

using namespace std;

//--------------------------------------------------------------
void ShowUsage(int argc, char **argv) {
  cout << "usage: $ " << argv[0]
       << " [ D,  L0 , L1 , Ntau, Ntau_cutoff, KTYPE ]     \n";
  cout << "    Hypercubic lattice L0 x L1 x L2...          \n";
  cout << "                                         \n";
  cout << "    L0, L1 ... the liner size of the lattice. \n";
  cout << "          ( L0, L1, ..., must be even number. )\n";
  cout << " available  KTYPE=0: kx=pi n/L, n=0,2, 4,,,, L" << endl;
  cout << " available  KTYPE=1: k=(pi,0),(pi,pi),(0,pi),(pi/2,pi/2)" << endl;
  cout << " available  KTYPE=2: k=(pi 7/8,pi 1/8),(pi 3/4,pi/4),(pi 5/8,pi 3/8)"
       << endl;
  cout << " available  KTYPE=3: k=(pi 7/8,pi 7/8)" << endl;
}

//-------------------------------------------------------------
void WriteXML(int D, int L[], int Ntau, int CutoffOfNtau, int KTYPE) {
  ofstream fout("sf.xml");
  fout.precision(8);
  int N = 1;  // number of sites.
  for (int i = 0; i < D; i++) {
    N *= L[i];
  }
  int KMAX;

  if (KTYPE == 0) {
    KMAX = L[0] / 2 + 1;
  } else if (KTYPE == 1) {
    KMAX = 4;
  } else if (KTYPE == 2) {
    KMAX = 3;
  } else if (KTYPE == 3) {
    KMAX = 3;
  } else {
    cout << " available  KTYPE=0: kx/pi = n/L, n=0,2, 4,,,, L";
    cout << " available  KTYPE=1: k/pi = (1,0),(1,1),(0,1),(1/2,1/2)";
    cout << " available  KTYPE=2: AFMBZ";
    cout << " available  KTYPE=3: k=(7/8,7/8),(1.0),(1/2,1/2)";
    exit(1);
  }

  int NumberOfElements = N * KMAX;

  fout << "<StructureFactor>" << endl << endl;
  fout << "<Comment>" << endl;
  fout << "  " << D << "-dimension hypercubic lattice" << endl;
  fout << "  KTYPE = " << KTYPE << endl;
  for (int i = 0; i < D; i++) {
    fout << "L" << i << "    = " << L[i] << endl;
  }
  fout << " ((  KTYPE=0: kx/pi = n/L, n=0,2, 4,,,, L         ))" << endl;
  fout << " ((  KTYPE=1: k/pi = (1,0),(1,1),(0,1),(1/2,1/2)  ))" << endl;
  fout << " ((  KTYPE=2: k=(7/8,1/8),(3/4,1/4),(5/8,3/8)     ))" << endl;
  fout << " ((  KTYPE=3: k=(7/8,7/8),(1.0),(1/2,1/2)         ))" << endl;
  fout << "</Comment>" << endl << endl;

  fout << "<Ntau>                   " << Ntau << " </Ntau>" << endl;
  fout << "<NumberOfElements>       " << NumberOfElements
       << " </NumberOfElements>" << endl;
  fout << "<CutoffOfNtau>           " << CutoffOfNtau << " </CutoffOfNtau>"
       << endl;
  fout << "<NumberOfInverseLattice> " << KMAX << " </NumberOfInverseLattice>"
       << endl;
  fout << endl;

  fout << "<!-- <Kind> [k-index] [ksite] </Kind> -->" << endl << endl;

  int *ksite = new int[KMAX];

  for (int q = 0; q < KMAX; q++) {
    if (KTYPE == 0)
      ksite[q] = 2 * q;
    else
      ksite[q] = q;

    fout << "<Kind> "
         << " " << q << " " << ksite[q] << " </Kind>" << endl;
  }

  fout << endl;
  fout << "<!-- <SF> [phase(cos)] [phase(sin)] [isite] [kindx] </SF> -->"
       << endl
       << endl;

  int NB = 0;  // 3 * N ;   // number of bonds
  int *x = new int[D];
  int *dx = new int[D];

  double PI = M_PI;

  for (int i = 0; i < N; i++) {
    for (int q = 0; q < KMAX; q++) {
      int rx = i % L[0];
      double phase;
      if (KTYPE == 0) {
        phase = rx * ksite[q] * PI / L[0];  // r_x * q_x
      } else if (KTYPE == 1) {
        int ry = (i / L[0]) % L[1];
        double qx;
        double qy;
        if (q == 0) {
          qx = 1.0;
          qy = 0.0;
        } else if (q == 1) {
          qx = 1.0;
          qy = 1.0;
        } else if (q == 2) {
          qx = 0.0;
          qy = 1.0;
        } else if (q == 3) {
          qx = 0.5;
          qy = 0.5;
        }

        phase = rx * qx * PI + ry * qy * PI;  // r_x * q_x
      } else if (KTYPE == 2) {
        int ry = (i / L[0]) % L[1];
        double qx;
        double qy;
        if (q == 0) {
          qx = 7.0 / 8.0;
          qy = 1.0 / 8.0;
        } else if (q == 1) {
          qx = 3.0 / 4.0;
          qy = 1.0 / 4.0;
        } else if (q == 2) {
          qx = 5.0 / 8.0;
          qy = 3.0 / 8.0;
        }

        phase = rx * qx * PI + ry * qy * PI;  // r_x * q_x
      } else if (KTYPE == 3) {
        int ry = (i / L[0]) % L[1];
        double qx;
        double qy;
        if (q == 0) {
          qx = 7.0 / 8.0;
          qy = 7.0 / 8.0;
        } else if (q == 1) {
          qx = 1.0;
          qy = 0.0;
        } else if (q == 2) {
          qx = 0.5;
          qy = 0.5;
        }

        phase = rx * qx * PI + ry * qy * PI;  // r_x * q_x
      }
      //+++++++++++++++++++++++
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

  delete[] x;
  delete[] ksite;
}

int main(int argc, char **argv) {
  int NARG = 3;
  if (argc < NARG + 1) {
    ShowUsage(argc, argv);
    exit(0);
  }
  int iarg = 1;
  const int D = atoi(argv[iarg]);
  iarg++;
  if (argc != D + 5) {
    ShowUsage(argc, argv);
    exit(0);
  }

  int *L = new int[D];

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
    cout << "L" << i << "    = " << L[i] << endl;
    EvenOrOdd += L[i] % 2;
  }

  if (EvenOrOdd) {
    cout << "Warnig: L should be an even number." << endl;
  }

  WriteXML(D, L, Ntau, Ntcut, KTYPE);
  cout << "... done." << endl;
  delete[] L;
  return 0;
}
