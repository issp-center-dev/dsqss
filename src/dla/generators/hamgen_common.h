#ifndef DSQSS_GENERATORS_HAMGEN_COMMON_H
#define DSQSS_GENERATORS_HAMGEN_COMMON_H
#include <cstdio>
#include <cstdlib>
#include <string>
#include <vector>

#include "matrix.h"

// Writes the pair-Hamiltonian file consumed by dla_alg.
//   M           : K of the local algebra (2S for spins, NMAX for bosons)
//   Q           : one-site source (worm) operator matrix
//   H           : pair Hamiltonian matrix (dimension (M+1)^2)
//   comment     : model description for the <Comment> tag
//   localstates : measurement value of each local basis state
inline void WriteHamiltonianXML(int M, dgematrix& Q, dgematrix& H,
                                std::string const& filename,
                                std::string const& comment,
                                const std::vector<double>& localstates) {
  FILE* FOUT = fopen(filename.c_str(), "w");
  if (FOUT == NULL) {
    printf("error: cannot open output file %s\n", filename.c_str());
    exit(1);
  }
  int D = M + 1;
  int DD = D * D;
  fprintf(FOUT, "<Hamiltonian>\n");
  fprintf(FOUT, "  <General>\n");
  fprintf(FOUT, "    <Comment> %s </Comment>\n", comment.c_str());
  fprintf(FOUT, "    <NSTYPE> 1 </NSTYPE>\n");
  fprintf(FOUT, "    <NITYPE> 1 </NITYPE>\n");
  fprintf(FOUT, "    <NXMAX>  %d </NXMAX>\n", D);
  fprintf(FOUT, "  </General>\n");
  fprintf(FOUT, "\n");
  fprintf(FOUT, "  <Site>\n");
  fprintf(FOUT, "    <STYPE> 0 </STYPE>\n");
  fprintf(FOUT, "    <TTYPE> 0 </TTYPE>\n");
  fprintf(FOUT, "    <NX>   %d </NX>\n", D);
  fprintf(FOUT, "    <LocalStates>");
  for (int i = 0; i < D; i++) {
    fprintf(FOUT, " %.16g", localstates[i]);
  }
  fprintf(FOUT, " </LocalStates>\n");
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
      int j0 = j % D;
      int j1 = j / D;
      double x = -H(i, j);
      if (abs(x) > 1.0e-8) {
        fprintf(FOUT, "    <Weight> %d %d %d %d %24.16f </Weight>\n", i0, j0,
                i1, j1, x);
      }
    }
  }
  fprintf(FOUT, "  </Interaction>\n");
  fprintf(FOUT, "</Hamiltonian>\n");
  fclose(FOUT);
}

#endif  // DSQSS_GENERATORS_HAMGEN_COMMON_H
