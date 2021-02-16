
//============================================================================
//    Computation of Canonical Averages
//============================================================================

dgematrix DensityMatrix(double B, vector<double>& V, dgematrix& U) {
  int DIM = V.size();
  dgematrix UT(DIM, DIM);
  dgematrix W(DIM, DIM);
  UT = t(U);
  W.zero();
  double emin = V[0];
  for (int i = 0; i < DIM; i++)
    if (emin > V[i]) emin = V[i];
  for (int i = 0; i < DIM; i++) W(i, i) = exp(-B * (V[i] - emin));
  return U * W * UT;
}

//----------------------------------------------------------------------------

double chi(double B, double E1, double E0) {
  if (fabs(E1 - E0) < 1.0e-12) {
    return B * exp(-B * E0);
  } else {
    return -(exp(-B * E1) - exp(-B * E0)) / (E1 - E0);
  }
}

//----------------------------------------------------------------------------

//
// ans = (\beta)^{-1} \int_0^{\beta} dt ( <Q(t)Q(0)> - <Q(t)Q(0)> )
//     --> <Q^2>-<Q>^2 (in the classical or the high-T limit)
//
double Susceptibility(double B, vector<double>& V, dgematrix& U, dgematrix& Q) {
  int DIM = V.size();
  double* E = new double[DIM];
  double emin = V[0];
  for (int i = 0; i < DIM; i++)
    if (V[i] < emin) emin = V[i];
  for (int i = 0; i < DIM; i++) E[i] = V[i] - emin;

  dgematrix UT(DIM, DIM);
  dgematrix W(DIM, DIM);
  UT = t(U);
  W = UT * Q * U;

  double Z0 = 0.0;
  double Z1 = 0.0;
  double ZX = 0.0;
  for (int i = 0; i < DIM; i++) {
    Z0 += exp(-B * E[i]);
    Z1 += exp(-B * E[i]) * W(i, i);
    for (int j = 0; j < DIM; j++) {
      ZX += chi(B, E[j], E[i]) * W(j, i) * W(i, j);
    }
  }

  double ave = Z1 / Z0;
  double ans = ZX / Z0 / B - ave * ave;
  delete[] E;
  return ans;
}

//----------------------------------------------------------------------------

double CanonicalAverage(dgematrix& R, dgematrix& Q) {
  double z0 = 0.0;
  double z1 = 0.0;
  int n = R.n;
  dgematrix W(n, n);
  W = R * Q;
  for (int i = 0; i < n; i++) {
    z0 += R(i, i);
    z1 += W(i, i);
  }
  return z1 / z0;
}
