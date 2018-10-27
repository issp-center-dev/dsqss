#include <cstring>
#include <cstdio>
#include <iostream>
#include <string>
using namespace std;
#include "matrix.h"
#include "boson_B.h"
#include "canonical.h"

//----------------------------------------------------------------------

// Bose-Hubbard Model
//H= - J\sum_{<i,j>} b_i b_j + V\sum_{<i,j>} n_i n_j
//    + u/2z \sum_{<i,j>}( n_i (n_i + 1) + n_j (n_j + 1) )
//     - mu/z \sum_{<i,j>}( n_i + n_j ).
//F=mu/z,
//U=u/z

class BoseHubbardModel {
public:
  int M;  // Number of bosons ( [M]-representaion )
  int NSITE;
  double J;  // the bilinear coupling for XY
  double V;  // the bilinear coupling for Z
  double U;  // the on-site coupling for Z
  double F;  // the field
  int DIM;   // dimension of the Hilbert space
  dgematrix H;
  dgematrix MXU;
  dgematrix MZU;
  dgematrix MXS;
  dgematrix MZS;
  dgematrix I;

  BoseHubbardModel(int M0, int NSITE0, double J0, double V0, double U0,
                   double F0) {
    M     = M0;
    NSITE = NSITE0;
    J     = J0;
    V     = V0;
    U     = U0;
    F     = F0;
    //double Vh=V*0.5;

    BosonOperatorSet S(M, NSITE);

    DIM = S.DIM;
    cmatrix h(DIM);
    h.zero();
    printf("  defining the hamiltonian...\n");
    for (int k = 0; k < NSITE; k++) {
      int l = (k + 1) % NSITE;
      printf("    k= %d\n", k);
      //h += (-J) * ( S.X[k] * S.X[l] + S.Y[k] * S.Y[l] );
      h += (-J) * (S.UP[k] * S.DN[l]);
      h += (+V) * (S.Z[k] * S.Z[l]);
    }
    for (int k = 0; k < NSITE; k++) {
      h += (+U) * (S.Z[k] * S.Z[k]);
      h += (-F - U) * (S.Z[k]);
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

void WriteXML(int M, dgematrix& Q, dgematrix& H, std::string const & filename) {
  FILE* FOUT = fopen(filename.c_str(), "w");
  int D      = M + 1;
  int DD     = D * D;
  fprintf(FOUT, "<Hamiltonian>\n");
  fprintf(FOUT, "  <General>\n");
  fprintf(FOUT,
          "    <Comment> Extended Bose-Hubbard model with NMAX=%d </Comment>\n",
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
        if (i != j) x = abs(x);
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
  printf("    %s [-o filename] M J V U F\n", exename.c_str());
  printf("arguments:\n");
  printf("    M ... the maxmum number of bosons on each site \n");
  printf("    J ... the hopping constant (positive)\n");
  printf("    V ... the nearest neighbor interaction (positive for " "replusive)\n");
  printf("    U ... the on-site interaction (positive for replusive)\n");
  printf("          ( = u/z if the field u is the field per site.)\n");
  printf("    F ... the chemical potential in the pair Hamiltonian\n");
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

  int NARG = 5;
  if (argc != NARG + 1) {
    ShowUsage(exename);
    exit(0);
  }
  int M    = atoi(argv[1]);
  double J = (double)atof(argv[2]);
  double V = (double)atof(argv[3]);
  double U = (double)atof(argv[4]);
  double F = (double)atof(argv[5]);
  printf(" M     = %4d\n", M);
  printf(" J     = %8.3f\n", J);
  printf(" V     = %8.3f\n", V);
  printf(" U     = %8.3f\n", U);
  printf(" F     = %8.3f\n", F);
  BosonOperator S(M);
  BoseHubbardModel MDL(M, 2, J, 0.5 * V, 0.5 * U, F);
  WriteXML(M, S.X.re, MDL.H, filename);

  return 0;
}
