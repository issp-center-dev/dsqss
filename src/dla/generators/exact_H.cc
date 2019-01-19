#include <cstring>
#include <cstdio>
#include <iostream>
#include <string>
using namespace std;
#include "matrix.h"
#include "canonical.h"
#include "spin_H.h"

//----------------------------------------------------------------------

// Heisenberg Model

class HeisenbergModel {
public:
  int M;  // Number of bosons ( [M]-representaion )
  int NSITE;
  double J;  // the bilinear coupling
  double F;  // the field
  int DIM;   // dimension of the Hilbert space
  dgematrix H;
  dgematrix MXU;
  dgematrix MZU;
  dgematrix MXS;
  dgematrix MZS;
  dgematrix I;

  HeisenbergModel(int M0, int NSITE0, double J0, double F0) {
    M     = M0;
    NSITE = NSITE0;
    J     = J0;
    F     = F0;

    HeisenbergSpinSet S(M, NSITE);

    DIM = S.DIM;
    cmatrix h(DIM);
    h.zero();
    printf("  defining the hamiltonian...\n");
    for (int k = 0; k < NSITE; k++) {
      int l = (k + 1) % NSITE;
      printf("    k= %d\n", k);
      h += (-J) * (S.X[k] * S.X[l] + S.Y[k] * S.Y[l] + S.Z[k] * S.Z[l]);
    }
    for (int k = 0; k < NSITE; k++) {
      h += (-F) * (S.Z[k]);
    }
    printf("  ... done.\n");

    printf("  defining the magnetizations...\n");
    cmatrix mxu(DIM);
    cmatrix mzu(DIM);
    cmatrix mxs(DIM);
    cmatrix mzs(DIM);
    mxu.zero();
    mzu.zero();
    mxs.zero();
    mzs.zero();
    for (int k = 0; k < NSITE; k++) {
      printf("   k= %d\n", k);
      double sgn = 1.0;
      if (k % 2 == 1) sgn = -1.0;
      mxu += (1.0 * S.X[k]);
      mzu += (1.0 * S.Z[k]);
      mxs += (sgn * S.X[k]);
      mzs += (sgn * S.Z[k]);
    }
    printf("  ... done.\n");

    H   = h.re;
    MXU = mxu.re;
    MZU = mzu.re;
    MXS = mxs.re;
    MZS = mzs.re;
    I   = S.I.re;
  };
};

//============================================================================

void Average(int DIM, dgematrix& R, dgematrix& Q, double& Ave, double& Var) {
  dgematrix W(DIM, DIM);
  W   = Q * Q;
  Ave = CanonicalAverage(R, Q);
  Var = CanonicalAverage(R, W);
  Var = Var - Ave * Ave;
}

//============================================================================

void WriteXML(int M, dgematrix& Q, dgematrix& H, std::string const& filename) {
  FILE* FOUT = fopen(filename.c_str(), "w");
  int D      = M + 1;
  int DD     = D * D;
  fprintf(FOUT, "<Hamiltonian>\n");
  fprintf(FOUT, "  <General>\n");
  fprintf(FOUT, "    <Comment> SU(2) Heisenberg model with S=%d/2 </Comment>\n",
          M);
  fprintf(FOUT, "    <NSTYPE> 1 </NSTYPE>\n");
  fprintf(FOUT, "    <NITYPE> 1 </NITYPE>\n");
  fprintf(FOUT, "    <NXMAX>  %d </NXMAX>\n", D);
  fprintf(FOUT, "  </General>\n");
  fprintf(FOUT, "\n");
  fprintf(FOUT, "  <Site>\n");
  fprintf(FOUT, "    <STYPE> 0 </STYPE>\n");
  fprintf(FOUT, "    <TTYPE> 0 </TTYPE>\n");
  fprintf(FOUT, "    <NX>   %d </NX>\n", D);
  fprintf(FOUT, "  </Site>\n");
  fprintf(FOUT, "\n");
  fprintf(FOUT, "  <Source>\n");
  fprintf(FOUT, "    <TTYPE> 0 </TTYPE>\n");
  fprintf(FOUT, "    <STYPE> 0 </STYPE>\n");
  for (int i = 0; i < D; i++) {
    for (int j = 0; j < D; j++) {
      double x = Q(i, j);
      if (abs(x) > 1.0e-8) {
        fprintf(FOUT, "    <Weight> %d %d %24.16f </Weight>\n", i, j, x);
      }
    }
  }
  fprintf(FOUT, "  </Source>\n");
  fprintf(FOUT, "\n");
  fprintf(FOUT, "  <Interaction>\n");
  fprintf(FOUT, "    <ITYPE> 0 </ITYPE>\n");
  fprintf(FOUT, "    <NBODY> 2 </NBODY>\n");
  fprintf(FOUT, "    <STYPE> 0 0 </STYPE>\n");
  for (int i = 0; i < DD; i++) {
    int i0 = i % D;
    int i1 = i / D;
    for (int j = 0; j < DD; j++) {
      int j0   = j % D;
      int j1   = j / D;
      double x = -H(i, j);
      if (abs(x) > 1.0e-8) {
        // if (i != j) x = abs(x);
        fprintf(FOUT, "    <Weight> %d %d %d %d %24.16f </Weight>\n", i0, j0,
                i1, j1, x);
      }
    }
  }
  fprintf(FOUT, "  </Interaction>\n");
  fprintf(FOUT, "</Hamiltonian>\n");
}

void ShowUsage(std::string const& exename){
  printf("usage:\n");
  printf("    %s [-o filename] M J F\n", exename.c_str());
  printf("arguments:\n");
  printf("    M ... the number of bosons on each site \n");
  printf("            ( M= 1, 2, 3, ... for S= 1/2, 1, 3/2, ... ) \n");
  printf("    J ... the coupling constant (positive for ferromagnets)\n");
  printf("    F ... the magnetic field in the pair Hamiltonian\n");
  printf("          ( = H/z if the field H is the field per site\n");
  printf("            and H is shared equally by all pairs,\n");
  printf("            e.g., F = H/4 for a square lattice. )\n");
  printf("option:\n");
  printf("    -o filename ... output file (default: hamiltonian.xml)\n");
}

//============================================================================
//    Main
//============================================================================

int main(int argc, char** argv) {

  string exename = argv[0];
  string filename("hamiltonian.xml");

  if(argc < 3){
    ShowUsage(exename);
    exit(0);
  }
  if(std::strcmp(argv[1], "-o")==0){
    filename = argv[2];
    argc -= 2;
    argv += 2;
  }

  int NARG = 3;
  if (argc != NARG + 1) {
    ShowUsage(exename);
    exit(0);
  }
  int M    = atoi(argv[1]);
  double J = (double)atof(argv[2]);
  double F = (double)atof(argv[3]);
  printf(" M     = %4d\n", M);
  printf(" J     = %8.3f\n", J);
  printf(" F     = %8.3f\n", F);
  HeisenbergSpin S(M);
  HeisenbergModel MDL(M, 2, 0.5 * J, F);
  WriteXML(M, S.X.re, MDL.H, filename);
  return 0;
}
