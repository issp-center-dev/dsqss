#include <math.h>
#include <stdlib.h>
#include <iostream>

#include <Probability.h>
#include "mpi.h"


int Pcomp(const void *p, const void *q) {
  double x, y;
  x = ((Probability::Omega *)p)->val;
  y = ((Probability::Omega *)q)->val;

  if (x > y)
    return 1;
  else if (x < y)
    return -1;
  else
    return 0;
}

void Probability::look(Size *N, System *sp) {
  int oh, type;
  double sql;
  int h;

  local_Et    = 0.0;  //local energy shift
  double rmin = 0.0, Ri;

  // search the minimum value of the vertex density.
  for (int i = 0; i <= nmax; i++) {
    for (int j = 0; j <= nmax; j++) {
      for (int x = 0; x < XMAX; x++) {
        Ri = at_make(i, j, x);
        if (Ri <= rmin) { rmin = Ri; }
      }
    }
  }

  local_Et = tb - rmin;
  sp->Et   = z / 2.0 * N->V * local_Et;

  ////////

  for (int x = 0; x < PR->V; x++) {
    double rumin = 0.0;
    for (int i = 0; i <= nmax; i++) {
      Ri = au_make(i, x);
      if (Ri <= rumin) rumin = Ri;
    }

    //    cout << "rumin=" << rumin <<endl;
    local_Eu[x] = -rumin;
  }

  sp->Eu = 0.0;
  for (int x = 0; x < PR->V; x++)
    sp->Eu += local_Eu[x];

  MPI_Status status;
  MPI_Comm comm_nst0;
  int nst0_rank;

  cout << "rank=" << PR->my_rank << ":: local domain Eu = " << sp->Eu << endl;

  MPI_Comm_split(MPI_COMM_WORLD, PR->np, 0, &comm_nst0);
  MPI_Comm_rank(comm_nst0, &nst0_rank);

  double *peu;
  newcall_zero(peu, PR->NtNs);

  MPI_Allgather(&sp->Eu, 1, MPI_DOUBLE, peu, 1, MPI_DOUBLE, comm_nst0);

  sp->Eu = 0;
  for (int tag = 0; tag < PR->NtNs; tag++) {
    sp->Eu += peu[tag];
  }
  sp->Eu /= (double)PR->Ntdiv;  //double count for Ntdiv
  delcall(peu);

  MPI_Comm_free(&comm_nst0);

  cout << "rank=" << PR->my_rank << ":: global Eu = " << sp->Eu << endl;
  ////////

  rh_odd  = sp->Htr;
  rh_even = sp->Htr;

  //************ at **************
  for (int i = 0; i <= nmax; i++)
    for (int j = 0; j <= nmax; j++)
      for (int x = 0; x < XMAX; x++)
        at[i][j][x] = at_make(i, j, x);
  for (int i = 0; i <= nmax; i++)
    for (int x = 0; x < PR->V; x++)
      ru[i][x] = au_make(i, x);
  //*************romax******************
  rtmax = rmin;
  for (int i = 0; i <= nmax; i++)
    for (int j = 0; j <= nmax; j++)
      for (int x = 0; x < XMAX; x++)
        if (rtmax < at[i][j][x]) rtmax = at[i][j][x];

  for (int i = 0; i <= nmax; i++)
    for (int x = 0; x < PR->V; x++)
      if (rumax[x] < ru[i][x]) rumax[x] = ru[i][x];
  //******************* scattering prob against u ******************
  for (int i = 0; i <= nmax; i++)
    for (int x = 0; x < PR->V; x++)
      ru[i][x] /= rumax[x];

  for (int x = 0; x < PR->V; x++)
    for (int b = 0; b < 2; b++)          //b=0 corresconds "oh=dir" : b =(oh==dir)? 0: 1;
      for (int i = 0; i <= nmax; i++) {  //state before scattering
        int j = (b) ? i - 1 : i + 1;     //state after scattering
        if (j < 0 || j > nmax)
          u[b][i][x] = 0.0;
        else if (ru[i][x] <= ru[j][x])
          u[b][i][x] = 1.0;
        else
          u[b][i][x] = Tuab(i, j, x);  //when i is larger(L), if L->S then P = S/L, if L->L then 1.0 - S/L.

        //	if(PR->my_rank==0)cout <<"x="<<x<<",  b="<<b<<", ni="<<i<<", nj="<<j<<", u="<<u[b][i][x]<<endl;
      }

  //****************** scattering prob against t  *********************************

  int flaver = 4;
  for (int x = 0; x < XMAX; x++) {
    for (h = 0; h < 2; h++) {  // head operator

      oh = h * 2 - 1;

      for (int i = 0; i <= nmax; i++) {  // # of particles on the left site

        sql = sqrt(i - h + 1.0);

        for (int j = 0; j <= nmax; j++) {  // # of particles on the right site

          if (h == i)
            type = 5;
          else {
            type      = 0;
            Om[0].val = at[i][j][x];
            Om[1].val = at[i + oh][j][x];
            Om[2].val = (j != h) ? tb : 0.0;
            Om[3].val = (j == h) ? tb : 0.0;
          }

          for (int k = 0; k < 4; k++)
            Om[k].num = k;
          qsort(Om, 4, sizeof(Omega), Pcomp);
          for (int k = 0; k < 4; k++)
            Tr[Om[k].num] = k;  // sorted 

          if (type != 5) SolveWeightEquation(flaver);  //
          //if(type!=5) Color(flaver);

          for (int b = 0; b < 4; b++)      // before update
            for (int a = 0; a < 4; a++) {  // after update

              //******************* scattering against t ******************
              if (type == 5)
                t[h][a][b][i][j][x] = 0.0;
              else if (Om[Tr[b]].val != 0.0)
                t[h][a][b][i][j][x] = Wall[Tr[b]][Tr[a]] / Om[Tr[b]].val;
                //*****************************************************
#ifdef DEBUG
              if (x == 0)
                std::cout << "h=" << h << " a=" << a << " b=" << b << " i=" << i << " j=" << j << "  type=" << type
                          << "  Om[0]=" << Om[0].val << "  Om[1]=" << Om[1].val << "  Om[2]=" << Om[2].val
                          << "  Om[3]=" << Om[3].val << "  sql=" << sql << "  a_t=" << at[i][j][x]
                          << "   t =" << t[h][a][b][i][j][x] << std::endl;
#endif
            }
        }
      }
    }
  }
}

void Probability::Color(int cmax) {
  for (int i = 0; i < cmax; i++) {
    ex_Wall[i] = ex_Penki[i] = Om[i].val;
  }
  for (int i = 0; i < cmax; i++)
    for (int p = 0; p < cmax; p++)
      Wall[i][p] = 0.0;

  double total_Penki, paint;

  for (int penki = 0; penki < cmax - 1; penki++) {
    if (ex_Penki[penki] == 0.0) continue;
    total_Penki = 0.0;
    for (int kabe = penki + 1; kabe < cmax; kabe++)
      total_Penki += Om[kabe].val;
    for (int kabe = penki + 1; kabe < cmax; kabe++) {
      paint             = ex_Wall[penki] * (Om[kabe].val / total_Penki);
      // paint `kabe` wall by `penki` penki.
      Wall[kabe][penki] = paint;
      // paint `penki` wall by `kabe` penki.
      Wall[penki][kabe] = paint;
      ex_Wall[kabe] -= paint;
    }
  }

  Wall[cmax - 1][cmax - 1] = ex_Wall[cmax - 1];

#ifdef DEBUG
  for (int i = 0; i < 4; i++)
    for (int p = 0; p < 4; p++)
      cout << "wall=" << i << "  penki=" << p << "  wij=" << Wall[i][p] << endl;
#endif
}

void Probability::SolveWeightEquation(int cmax) {
  for (int i = 0; i < cmax; i++) {
    ex_Wall[cmax - 1 - i] = ex_Penki[cmax - 1 - i] = Om[i].val;
  }
  for (int i = 0; i < cmax; i++)
    for (int p = 0; p < cmax; p++)
      Wall[i][p] = 0.0;

  int p, q;
  double V_first;
  double V_second;
  double V_third;
  double V_first_new;
  double V_second_new;
  int N_first;   // the number of the largest elements
  int N_second;  // the number of the second largest

  // First three (unique) weights and indices
  while (ex_Wall[1] > EPS) {
    V_first = ex_Wall[0];
    for (p = 0; p < cmax; p++)
      if (ex_Wall[p] != V_first) break;
    N_first = p;
    if (p == cmax) {  //all are same
      V_second = 0.0;
      V_third  = 0.0;
      N_second = 0;
    } else {
      V_second = ex_Wall[p];
      for (q = p; q < cmax; q++)
        if (ex_Wall[q] != V_second) break;
      if (q == cmax) {
        V_third = 0.0;
      } else {
        V_third = ex_Wall[q];
      }
      N_second = q - p;
    }

    // Calculation w_{ij} from V_i

    double dw1;  // decrement of the first largest element
    double dw2;  // decrement of the second largest element
    if (N_first == 1) {
      // When the maximum weight state is unique
      // introduce a transition between the max state and the second
      // and reduce weights of these states
      double x = V_first - V_second;
      double y = (double)(N_second - 1) * (V_second - V_third);
      if (x < y) {
        dw1          = (V_first - V_second) / (1.0 - 1.0 / (double)(N_second));
        dw2          = dw1 / (double)N_second;
        V_second_new = V_second - dw2;
        V_first_new  = V_second_new;
      } else {
        dw2          = V_second - V_third;
        dw1          = dw2 * (double)N_second;
        V_second_new = V_third;
        V_first_new  = V_first - dw1;
      }
      ex_Wall[0] = V_first_new;
      for (int i = 1; i < 1 + N_second; i++) {
        Wall[cmax - 1][cmax - 1 - i] += dw2;
        Wall[cmax - 1 - i][cmax - 1] += dw2;
        ex_Wall[i] = V_second_new;
      }
    } else {
      // When the maximum weight state is degenerated
      // introduce a transition between these states
      // and reduce weights of these states to the weight of the second largest.
      dw1 = (V_first - V_second) / (double)(N_first - 1);
      for (int i = 0; i < N_first; i++) {
        ex_Wall[i] = V_second;
        for (int j = 0; j < N_first; j++) {
          if (i == j) continue;
          Wall[cmax - 1 - i][cmax - 1 - j] += dw1;
        }
      }
    }
  }
  if (ex_Wall[0] > 0.0) {
    Wall[cmax - 1][cmax - 1] += ex_Wall[0];
    ex_Wall[0] = 0.0;
  }
}

double Probability::Tuab(int p, int q, int x) {  // p(L)->q(S)

  return au_make(q, x) / au_make(p, x);
}

// return t vertex density
double Probability::at_make(int p, int q, int x) {

  //double Ht = dim*(double)( p + q ) - V1*(double)(p*q);
  //  double Ht = (double)( mu1[x]*p + mu2[x]*q )/z - V1*(double)(p*q);
  double Ht = V1 * ((double)p - 0.5) * ((double)q - 0.5);
  // if(Ht + local_Et < 0.0){cout<<"negative sign! (Hv)"<<endl; exit(1);}

  return (Ht + local_Et);
}

// return u vertex density
double Probability::au_make(int p, int x) {

  double Hu = -ep[x] * (p - 0.5);

  //  if(Hu + local_Eu[x] < 0.0){cout<<"negative sign! (Hu)"<<endl; exit(1);}

  return Hu + local_Eu[x];
}

Probability::Probability(Size *N, System *sp, Parallel *m_PR) {
  PR   = m_PR;
  Ubb  = sp->Ubb;
  V1   = sp->Vb1;
  nmax = sp->nmax;

  tb   = 0.5 * sp->tb;
  z    = 2.0 * N->d;
  XMAX = 2;

  newcall_zero(ep, PR->V);
  newcall_zero(local_Eu, PR->V);
  newcall_zero(rumax, PR->V);
  newcall_zero(ru, nmax + 1, PR->V);

  newcall_zero(t, 2, 4, 4, nmax + 1, nmax + 1, XMAX);
  newcall_zero(u, 2, nmax + 1, PR->V);
  newcall_zero(w, 5, nmax + 1);
  newcall_zero(at, nmax + 1, nmax + 1, XMAX);

  newcalls(Om, 4);
  newcall_zero(Tr, 4);
  newcall_zero(ex_Wall, 4);
  newcall_zero(ex_Penki, 4);

  newcall_zero(Wall, 4, 4);

  newcall_zero(FracW, 6, 6, XMAX);
  newcall_zero(weight, 6);
}

void Probability::LocalPotential(double mu) {
  mu1[0] = -mu ;
  mu1[1] = -mu ;

  double A0 = 1.0;  //staggerd lattice
  for (int z = 0; z < PR->z; z++)
    for (int y = 0; y < PR->y; y++)
      for (int x = 0; x < PR->x; x++) {
        int i = x + y * PR->x + z * PR->x * PR->y;
        ep[i] = -mu;
      }
#ifdef DEBUG
  char fname[256];
  sprintf(fname, "potential_S%02dT%02dR%03d", PR->ns, PR->nt, PR->nr);
  std::ofstream fout(fname);
  for (int i = 0; i < PR->V; i++)
    fout << i << " " << ep[i] << endl;
  fout.close();
#endif
}

void Probability::LookUpTable(Size *N, System *sp) {
  dim     = sp->mu / z;
  Nx      = N->x;
  LocalPotential(sp->mu);
  look(N, sp);

  for (int x = 0; x < XMAX; x++) {
    weight[0] = at[0][0][x];
    weight[1] = at[0][1][x];
    weight[2] = at[1][0][x];
    weight[3] = at[1][1][x];
    weight[4] = weight[5] = tb;

    for (int i = 0; i < 6; i++)
      for (int j = 0; j < 6; j++)
        FracW[j][i][x] = weight[i] / weight[j];
  }
}

Probability::~Probability() {
  delcall(ep);

  delcall(t, 2, 4, 4, nmax + 1, nmax + 1);
  delcall(u, 2, nmax + 1);

  delcall(w, 5);

  delcall(at, nmax + 1, nmax + 1);

  delcall(ru, nmax + 1);
  delcall(rumax);
  delcall(local_Eu);

  delcall(Om);
  delcall(Tr);
  delcall(ex_Wall);
  delcall(ex_Penki);

  delcall(Wall, 4);

  delcall(FracW, 6, 6);
  delcall(weight);
}
