#include <float.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include <fstream>
#include <iostream>
using namespace std;

#include <stdma.h>

double factorial(int num) {
  double f;
  if (num == 0) return 1.0;

  f = num;
  for (int n = num - 1; n >= 1; n--) {
    f = (double)n * f;
  }

  return f;
}

void count_time(long tms, double x) {
  FILE *file;
  file = fopen("time.dat", "a+");

  fprintf(file, "%lf %lf\n", x, (double)tms / CLOCKS_PER_SEC);

  fclose(file);
}

void count_time(double tms, double x, char fname[128]) {
  FILE *file;
  file = fopen(fname, "a+");

  fprintf(file, "%lf %le\n", x, tms);

  fclose(file);
}
