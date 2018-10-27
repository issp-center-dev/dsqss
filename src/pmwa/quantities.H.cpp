//#include<stdma.h>
#include <Configuration.h>
//¤³¤³¤Ç¤Îpnum¤ÏNpara¤Î¤³¤È
//#define REWEIGHT
//#define KSTEPALL
//#define XSTEPALL
//#include <cmath>

Quantities::Quantities(Size *m_N, MC_p *m_MC, System *m_sp, Lattice *m_LT, Parallel *m_PR, std::string const & sfinfile) {
#ifdef REWEIGHT
  cout << "REWEIGHTING ON" << endl;
#else
  cout << "REWEIGHTING OFF" << endl;
#endif

  Npara   = m_PR->Npara;
  my_rank = m_PR->my_rank;

  MC = m_MC;
  N  = m_N;
  LT = m_LT;
  PR = m_PR;

  sp = m_sp;
  V  = PR->V;
  Nx = PR->x;

  NL[0] = N->x;
  NL[1] = N->y;
  NL[2] = N->z;

  SFD_Norm = (double)N->V * N->B * sp->tb * 2.0 * N->d;

  NVMAX = 0;
  NWMAX = 0;

  //  for(int i=0; i<V; i++)  cout<<"LT->lx="<<i<<" "<<LT->lx[i]<<endl;

#ifdef KSTEPALL
  Nkstep = 1;
#else
  Nkstep = Nx / 2;
#endif

#ifdef XSTEPALL
  Nxstep = 1;
#else
  Nxstep = Nx / 2;
#endif

  Nkxmax = N->x / Nkstep;
  Nxmax  = N->x / Nxstep;
  Nkmax  = Nkxmax * 2;  //i.e. kx=ky=kz, kx!=ky=kz=0
  Nkkmax = Nkxmax;

  Nend = Nq - 3;  //Mx's number

  newcall(Lmax, Nc);
  newcall(Lsum, Nc);

  Lsum[mz] = Lmax[mz] = V;
  Lmax[cr2]           = Nxmax;
  //  Lmax[sk]=Nkmax*2;//i.e. real and imaginary part
  Lmax[ck2] = Nkmax * 2;
  //#ifdef GF22Gk2
  //  Lmax[cr4]=N->x*N->x;
  //#else
  // Lmax[cr4]=Nxmax*PR->Nxdiv;
  //#endif
  Lmax[ck4] = Nkxmax * Nkkmax * 2;
  Lmax[dkk] = Nkxmax * Nkkmax;
  Lmax[nw]  = V;
  Lmax[nw2] = V;
  Lmax[lw]  = V;

  Lsize = 0;  //sin,cos,Nk,Sk

  for (int i = 1; i < Nc; i++)
    Lsum[i] = Lsum[i - 1] + Lmax[i];
  for (int i = 0; i < Nc; i++)
    Lsize += Lmax[i];

  newcall(file, Nc + Nq, 128);
  newcall(Qname, Nc + Nq, 128);

  //  int NVMAX, NWMAX;

  //  sprintf(parainfo,"B%.1lf_Nx%d_Ny%d_Vbb%.1lf",N->B,N->x,N->y,sp->Vb1);
  sprintf(Qname[wndx], "wndx");  //Winding Number for x-axis
  sprintf(Qname[wndy], "wndy");  //Winding Number for y-axis
  sprintf(Qname[wndz], "wndz");  //Winding Number for z-axis
  sprintf(Qname[wnd2], "wnd2");  //square of Winding Number
  sprintf(Qname[amzu], "amzu");  //Density or z-Magnetization
  sprintf(Qname[bmzu], "bmzu");  //Density or z-Magnetization
  sprintf(Qname[ang], "ang ");   //Phase
  sprintf(Qname[ene], "ene ");   //Energy
  sprintf(Qname[nver], "nver");  //Number of vertices
  sprintf(Qname[spe], "spe ");   //Specific heat
  sprintf(Qname[nwor], "nwor");  //Number of worms
  sprintf(Qname[xmx], "xmx ");   //Susceptibility
  sprintf(Qname[nkin], "nkin");  //Number of kinks
  sprintf(Qname[comp], "comp");  //Compressibility
  sprintf(Qname[lxmx], "lxmx");  //Compressibility
  sprintf(Qname[magx], "bmxu");  //BEC order parameter or xmag
  sprintf(Qname[magp], "bmpu");  //BEC order parameter or +mag
  sprintf(Qname[magm], "bmmu");  //BEC order parameter or -mag
  sprintf(Qname[smzu], "smzu");  //structure factor (uniform)
  sprintf(Qname[smzs], "smzs");  //structure factor (staggerd)
  sprintf(Qname[xmzu], "xmzu");  //structure factor (uniform)
  sprintf(Qname[xmzs], "xmzs");  //structure factor (staggerd)
  // sprintf(Qname[d00] ,"d00");  //noise correlation at k=0
  sprintf(Qname[len], "len");  //noise correlation at k=0

  sprintf(Qname[Nq + mz], "mz");    //Local density or local mz
  sprintf(Qname[Nq + cr2], "cr2");  //2-points correlation (mx-mx)
  //  sprintf(Qname[Nq+sk] ,"sk");  //structure factor
  sprintf(Qname[Nq + ck2], "ck2");  //k-space of cr2
  // sprintf(Qname[Nq+cr4],"cr4"); //4-points correlation
  sprintf(Qname[Nq + ck4], "ck4");  //k-space of cr4
  sprintf(Qname[Nq + dkk], "dkk");  //Noise correlation
  sprintf(Qname[Nq + nw], "nw");    //local number of worms
  sprintf(Qname[Nq + nw2], "nw2");  //square of nw
  sprintf(Qname[Nq + lw], "lw");    //square of nw

  //  int S=Nc+Nq;

  newcall(values_S, Nq);
  newcall(MCmean_S, Nq * 2);
  newcall(BINmean_S, Nq * 2 * MC->Nbin);
  //newcall(RNDmean_S,Nq*2*PR->Npara);

  newcall(values_L, Lsize);
  newcall(MCmean_L, Lsize * 2);
  newcall(BINmean_L, Lsize * MC->Nbin);
  //  newcall(RNDmean_L,Lsize*PR->Npara);

  newcall(m_val, max(Lsize, (int)Nq));

  newcalls(EXPrk, 4 * Nkmax * V);

  Cknum  = 16;
  Nk_set = 5 + 2 * Nkmax * Cknum;
  Sk_set = 2;

  newcalls(Ck, Nk_set + Sk_set);
  newcalls(Ck_m, Nk_set + Sk_set);

  double PI = M_PI;
  for (int i = 0; i < V; i++) {
    int x = (i % Nx) + PR->nx * Nx;
    int y = (int)(i / Nx) % PR->y + PR->ny * PR->y;
    int z = (int)(i / (Nx * PR->y)) + PR->nz * PR->z;

    for (int k = -Nkmax + 1; k < Nkmax; k++) {  //max(kx+kk)=Nkkmax-1 + Nkxmax-1

      double phase = 2.0 * PI * x * k / (double)Nkxmax;  //kx!=0,ky=kz=0
      complex<double> phase_c(0.0, phase);
      EXPrk[theta(i, k, 0)] = exp(phase_c);

      phase                 = 2.0 * PI * (x + y + z) * k / (double)Nkxmax;  //kx=ky
      phase_c               = complex<double>(0.0, phase);
      EXPrk[theta(i, k, 1)] = exp(phase_c);
    }
  }

  newcall(an, V);
  newcall(cr, V);
  newcall(ca, V);
  newcall(ac, V);
  newcall(Q, V * 3);

#ifdef SF
  // printf("Measurement::Measurement> Reading %s\n",sfinfile );
  X.initialize(sfinfile);
  read_sf();
#endif
}

Quantities::~Quantities() {
  delcall(Lmax);
  delcall(Lsum);

  delcall(file, Nc + Nq);
  delcall(Qname, Nc + Nq);

  delcall(values_S);
  delcall(MCmean_S);
  delcall(BINmean_S);
  // delcall(RNDmean_S);

  delcall(values_L);
  delcall(MCmean_L);
  delcall(BINmean_L);
  //delcall(RNDmean_L);

  delcall(m_val);

  delcall(EXPrk);

  delcall(Ck);
  delcall(Ck_m);

  delcall(an);
  delcall(cr);
  delcall(ca);
  delcall(ac);
  delcall(Q);

#ifdef SF

  delcall(MCmean_SF, NSF);
  delcall(BINmean_SF, NSF);
  delcall(counter4SFC, NKMAX);
  delcall(counter4SFS, NKMAX);
  delcall(sfsamp, NKMAX);
  delcall(COSrk, V);
  delcall(SINrk, V);

#endif
}

void Quantities::Init() {
  for (int i = 0; i < Nq1; i++)
    values_S[i] = 0;
  for (int i = 0; i < Lsize; i++)
    values_L[i] = 0;
  for (int i = 0; i < Nq * 2; i++)
    MCmean_S[i] = 0;
  for (int i = 0; i < Lsize * 2; i++)
    MCmean_L[i] = 0;
#ifdef SF
  for (int isf = 0; isf < NSF; isf++)
    for (int it = 0; it < Ntau; it++)
      MCmean_SF[isf][it] = 0;
      //  for( int i=0; i<Nq*2*PR->Npara; i++ ) RNDmean_S[i]=0;
      //  for( int i=0; i<Lsize*PR->Npara; i++ ) RNDmean_L[i]=0;
#endif
}

#ifdef SF
void Quantities::read_sf() {
  Ntau1 = X["Ntau"].getInteger();

  int Nline = X["NumberOfElements"].getInteger();

  dtau = PR->B / (double)Ntau1;  //in preparation for temporal pararellization

  Ntau  = X["CutoffOfNtau"].getInteger();
  NKMAX = X["NumberOfInverseLattice"].getInteger();
  NSF   = NKMAX * 2;

  newcall(MCmean_SF, NSF, Ntau);
  newcall(BINmean_SF, NSF, Ntau);

  newcall(counter4SFC, NKMAX, Ntau1);
  newcall(counter4SFS, NKMAX, Ntau1);
  newcall(sfsamp, NKMAX, Ntau);

  newcall(COSrk, V, NKMAX);
  newcall(SINrk, V, NKMAX);

  int count = 0;

  for (int i = 0; i < X.NumberOfBlocks(); i++) {
    XML::Block &BLCK = X[i];
    if (BLCK.getName() == "SF") {
      double COSrk_       = BLCK.getDouble(0);
      double SINrk_       = BLCK.getDouble(1);
      int isite           = BLCK.getInteger(2);
      int ksite           = BLCK.getInteger(3);
      COSrk[isite][ksite] = COSrk_;
      SINrk[isite][ksite] = SINrk_;
      count++;
    }
  }

  if (count != Nline) {
    cout << "ERROR Nline( " << Nline << " ) != count( " << count << " )" << endl;
    exit(0);
  }
};
#endif

void Quantities::Measure(int Nv, int Nk, vector<GraphSpace::Vertex> &ev, vector<GraphSpace::Vertex *> WORM,
                         GraphSpace::Vertex *world, GraphSpace::Vertex *worldB, double length, int m_Wnum, int mcs) {
  NVMAX = max(NVMAX, m_Wnum + Nv);
  NWMAX = max(NWMAX, m_Wnum);

#ifdef CFOUT
  WindingNumber(ev, mcs);  //no MPI
#endif
  NumberOfVertices(m_Wnum + Nv, mcs);  //no MPI
  NumberOfWorms(ev, m_Wnum);           //noMPI
  NumberOfKinks(Nk, mcs);              //no MPI
  CondensateFraction(mcs, world);      //MPI in correlation function1,2
  Density(world, worldB);              //no MPI
  CorrelationLength(length);

  SUM_OVER_T();   // for local nw and mz
  SUM_OVER_S();   // for amzu
  SUM_OVER_ST();  //for values_S
  // cout<<"Sk"<<endl;

  ////////////////////////
  MCsum_S();
#ifdef SF
  MCsum_SF();
#endif
#ifdef CFOUT
  MCsum_L();
#endif
  ////////////////////////
}

void Quantities::Measure() {

  Average();

  if (PR->nst == 0) {
    Energy();
    SpecificHeat();
    Susceptibility();

#ifdef CFOUT
    Compressibility();
    WindingNumber2();
    NoiseCorrelation();
#endif
  }
}

void Quantities::BINsum(int bin) {
  if (PR->nst == 0) {
    BINsum(MCmean_S, BINmean_S, Nq * 2, bin);
#ifdef CFOUT
    BINsum(MCmean_L, BINmean_L, Lsize, bin);
#endif
#ifdef SF
    for (int i = 0; i < NKMAX; i++)
      for (int it = 0; it < Ntau; it++) {
        BINmean_SF[2 * i][it] += MCmean_SF[2 * i][it];
        BINmean_SF[2 * i + 1][it] += MCmean_SF[2 * i][it] * MCmean_SF[2 * i][it];
      }
#endif
  }
}

void Quantities::BINaverage() {
  if (PR->nst == 0) {
    Average(BINmean_S, MC->Nbin, Nq * 2, MCmean_S, 1);
#ifdef CFOUT
    Average(BINmean_L, MC->Nbin, Lsize * 2, MCmean_L, 2);
#endif
#ifdef SF
    for (int i = 0; i < NKMAX; i++)
      for (int it = 0; it < Ntau; it++) {
        double mean               = BINmean_SF[2 * i][it] / (double)MC->Nbin;
        double smean              = BINmean_SF[2 * i + 1][it] / (double)MC->Nbin;
        double num                = (MC->Nbin == 1) ? 1.0 : MC->Nbin - 1.0;
        BINmean_SF[2 * i][it]     = mean;
        BINmean_SF[2 * i + 1][it] = sqrt((smean - mean * mean) / num);
      }
#endif
  }
}

void Quantities::BINsum(double *MCmean, double *BINmean, int Lmax, int bin) {
  for (int i = 0; i < Lmax; i++)
    BINmean[i + bin * Lmax] = MCmean[i];
}

void Quantities::Average(double *g, int Nval, int S, double *MCmean, int kstep) {
  double num = (Nval == 1) ? 1.0 : Nval - 1.0;

  for (int k = 0; k < S; k += 2) {
    double mean = 0.0, error = 0.0;

    for (int i = 0; i < Nval; i++) {
      mean += g[(k + i * S) / kstep];
      error += g[(k + i * S) / kstep] * g[(k + i * S) / kstep];
    }

    mean /= (double)Nval;
    error /= (double)Nval;

    MCmean[k]     = mean;
    MCmean[k + 1] = (Nval == 1) ? 0 : sqrt((error - mean * mean) / num);
  }
}

void Quantities::show_S(ofstream &F) {  //ÄÌ¾ï

  int S = Nq * 2;

  for (int k = 0; k < S; k += 2)
    F << "R " << Qname[k / 2] << " = " << MCmean_S[k] << " " << MCmean_S[k + 1] << endl;
}

#ifdef SF
void Quantities::dump(FILE *F) {
  fprintf(F, "C This is pmwa ver.%d\n\n", 4);
  fprintf(F, "P Ntau = %d\n", Ntau1);
  fprintf(F, "P Ntau1= %d\n", Ntau);
  fprintf(F, "P dtau = %lf\n", dtau);
  fprintf(F, "P NKMAX= %d\n\n", NKMAX);
}

inline void Quantities::show4SF(FILE *F) {
  for (int i = 0; i < NKMAX; i++) {
    for (int it = 0; it < Ntau; it++) {
      fprintf(F, "R C%dt%d = %16.10e %16.10e\n", i, it, MCmean_SF[2 * i][it], MCmean_SF[2 * i + 1][it]);
    }
    fprintf(F, "\n");
  }
}
#endif

void Quantities::show_L() {  //ÄÌ¾ï

  int k;
  long R, R_, Ry;
  string ll, Cname;

  std::ofstream F;

  //Local density
  F.open(file[Nq + mz], std::ios::app);
  F << "# Local density." << endl;
  F << "# R x-SITE  <VAL>  <ERROR>" << endl;
  F << setprecision(16);
  for (int l = 0; l < Nx; l++) {
    R     = l;
    k     = f_ld(l) * 2;
    Cname = "real_" + tostr(R);
    F << "R " << Cname << " = " << MCmean_L[k] << " " << MCmean_L[k + 1] << endl;
  }
  F.close();

  //Local worm density
  F.open(file[Nq + nw], std::ios::app);
  F << "# Local worm density." << endl;
  F << "# R x-SITE  <VAL>  <ERROR>" << endl;
  F << setprecision(16);
  for (int l = 0; l < Nx; l++) {
    R     = l;
    k     = f_nw(l) * 2;
    Cname = "real_" + tostr(R);
    F << "R " << Cname << " = " << MCmean_L[k] << " " << MCmean_L[k + 1] << endl;
  }
  F.close();

  //worm length
  F.open(file[Nq + lw], std::ios::app);
  F << "# worm length." << endl;
  F << "# R x-SITE  <VAL>  <ERROR>" << endl;
  F << setprecision(16);
  for (int l = 0; l < V; l++) {
    R     = l;
    k     = f_lw(l) * 2;
    Cname = "real_" + tostr(R);
    F << "R " << Cname << " = " << MCmean_L[k] << " " << MCmean_L[k + 1] << endl;
  }
  F.close();

  //Local susceptibility
  F.open(file[Nq + nw2], std::ios::app);
  F << "# Local susceptibility." << endl;
  F << "# R x-SITE  <VAL>  <ERROR>" << endl;
  F << setprecision(16);
  for (int l = 0; l < Nx; l++) {
    R     = l;
    k     = f_nw2(l) * 2;
    Cname = "real_" + tostr(R);
    F << "R " << Cname << " = " << MCmean_L[k] << " " << MCmean_L[k + 1] << endl;
  }
  F.close();

  //GF
  F.open(file[Nq + cr2], std::ios::app);
  F << "#2-point Correlation with sites." << endl;
  F << "# R x-distance  <VAL>  <ERROR>" << endl;
  F << setprecision(16);
  for (int l = 0; l < Lmax[cr2]; l++) {
    R     = l * Nxstep;
    k     = f_gf(l) * 2;
    Cname = "real_" + tostr(R);
    F << "R " << Cname << " = " << MCmean_L[k] << " " << MCmean_L[k + 1] << endl;
  }
  F.close();

  //nk
  F.open(file[Nq + ck2], std::ios::app);
  F << "# 2-point Correlation with wave numbers." << endl;
  F << "# real/imag  kx  ky  <VAL>  <ERROR>" << endl;
  F << setprecision(16);
  for (int l = 0; l < Lmax[ck2]; l++) {
    R     = l % Nkxmax;
    Ry    = ((int)(l / Nkxmax) % 2) * R;
    ll    = ((bool)(l / Nkmax)) ? "imag" : "real";
    k     = f_nk(l) * 2;
    Cname = ll + "_" + tostr(R * Nkstep) + "_" + tostr(Ry * Nkstep);
    F << "R " << Cname << " = " << MCmean_L[k] << " " << MCmean_L[k + 1] << endl;
  }
  F.close();

  //4-point correlation
  F.open(file[Nq + ck4], std::ios::app);
  F << "# 4-point Correlation with wave numbers." << endl;
  F << "# kx  kx'  <VAL>  <ERROR>" << endl;
  F << setprecision(16);
  for (int l = 0; l < Lmax[ck4]; l++) {
    k       = f_gk2(l) * 2;
    R       = l % Nkxmax;
    R_      = (int)(l / Nkxmax) % Nkkmax;
    bool kl = l / (Nkkmax * Nkxmax);
    ll      = (kl) ? "imag" : "real";  // real or imag
    Cname   = ll + "_" + tostr(R * Nkstep) + "_" + tostr(R_ * Nkstep);
    F << "R " << Cname << " = " << MCmean_L[k] << " " << MCmean_L[k + 1] << endl;

    if (R == Nkxmax - 1) F << endl;
  }
  F.close();

  //noise correlation
  F.open(file[Nq + dkk], std::ios::app);
  F << "# Noise Correlation with wave numbers." << endl;
  F << "# kx  kx'  <VAL>  <ERROR>" << endl;
  F << setprecision(16);
  for (int l = 0; l < Lmax[dkk]; l++) {
    k     = f_noise(l) * 2;
    R     = l % Nkxmax;
    R_    = (int)(l / Nkxmax);
    ll    = "real";
    Cname = ll + "_" + tostr(R * Nkstep) + "_" + tostr(R_ * Nkstep);
    F << "R " << Cname << " = " << MCmean_L[k] << " " << MCmean_L[k + 1] << endl;

    if (R == Nkxmax - 1) F << endl;
  }
  F.close();
}

///////

void Quantities::show(ofstream &F, FILE *SFF) {
  if (PR->nst == 0) {
    F << "I [the maximum number of vertices]          =" << NVMAX << endl;
    F << "I [the maximum number of worms]             =" << NWMAX << endl;
    show_S(F);
#ifdef SF
    show4SF(SFF);
#endif
#ifdef CFOUT
    show_L();
#endif
  }
}

//////
void Quantities::Output(std::string const & fname, double g) {
  std::ofstream file(fname.c_str(), std::ios::app);
  file << sp->Htr << " " << sp->mu << " " << sp->Vb1 << " " << sp->tb << " " << N->B << " " << g << endl;
}

//##########################################################################################

//*******************TYPEA************************

void Quantities::WindingNumber(vector<GraphSpace::Vertex> &ev, int mcs) {
  int Wi[3] = {0, 0, 0};

  vector<GraphSpace::Vertex>::iterator it = ev.begin();
  while (it != ev.end()) {
    int l = it->i / V;

    if ((it->type == 2 && /*LT->bd[it->i%V][l*2] == it->nleg->i%V &&*/ it->i % V < it->nleg->i % V) || it->type == -1) {
      int crr = (it->p < it->next[0]->p) ? 1 : -1;
      for (int d = 0; d < N->d; d++)
        if (LT->bond_vec[l][d] != 0.0) Wi[d] += crr;
    }

    ++it;
  }

  for (int d = 0; d < N->d; d++) {
    values_S[wndx + d] = (double)Wi[d];
  }  //(double)NL[d]; }

}

void Quantities::WindingNumber2() {
  double ww = 0.0;

  for (int d = 0; d < N->d; d++) {
    ww += MCmean_S[(wndx + d) * 2 + 1];
  }

  MCmean_S[wnd2 * 2]     = ww;
  MCmean_S[wnd2 * 2 + 1] = ww * ww;
}

//*******************Ì©ÅÙ************************

void Quantities::Density(GraphSpace::Vertex *world, GraphSpace::Vertex *worldB) {
  double ph[2] = {1.0, -1.0};
  double atot = 0.0, btot = 0.0, stot = 0.0, xtot = 0.0;
#ifdef SF
  reset4SF();
#endif

  for (int i = 0; i < V; i++) {
    int it     = 0;
    double tau = 0.0;
    double n0  = 0.0;

    for (GraphSpace::Vertex *wl = &(world[i]); wl != &(worldB[i]); wl = wl->next[1]) {
      double tTri = wl->next[1]->t;
      double bTri = wl->t;
      double mz   = wl->p - 0.5;

      n0 += (tTri - bTri) * mz;

#ifdef SF
      while (tau < tTri) {
        for (int k = 0; k < NKMAX; k++) {
          counter4SFC[k][it] += mz * COSrk[i][k];
          counter4SFS[k][it] += mz * SINrk[i][k];
        }

        it++;
        tau = it * dtau;
      }
#endif
    }

    values_L[f_ld(i)] = n0 / N->B;

    btot += values_L[f_ld(i)];

    double a0 = world[i].p - 0.5;
    atot += a0;
    stot += a0 * ph[LT->lx[i]];
    xtot += values_L[f_ld(i)] * ph[LT->lx[i]];
  }

  values_S[bmzu] = btot;
  values_S[amzu] = atot;

  values_S[xmzs] = xtot;
  values_S[smzs] = stot;

#ifdef SF

  for (int k = 0; k < NKMAX; k++) {
    for (int it = 0; it < Ntau; it++) {
      double SZKT = 0.0;
      for (int tt = 0; tt < Ntau1; tt++) {
        SZKT += counter4SFC[k][tt] * counter4SFC[k][(tt + it) % Ntau1]
                + counter4SFS[k][tt] * counter4SFS[k][(tt + it) % Ntau1];
      }
      sfsamp[k][it] = SZKT / (double)Ntau1;
    }
  }

#endif

  //*******
}

void Quantities::Compressibility() {
  MCmean_S[comp * 2] =
      N->B * N->V * (MCmean_S[smzu * 2] / (MCmean_S[amzu * 2] * MCmean_S[amzu * 2] * (double)N->V) - 1.0);
  MCmean_S[comp * 2 + 1] = MCmean_S[comp * 2] * MCmean_S[comp * 2];
}

//*******************¥¨¥Í¥ë¥®¡¼************************

void Quantities::Energy() {
  MCmean_S[ene * 2]     = (sp->Eu + sp->Et - MCmean_S[nver * 2] / N->B) / (double)N->V;
  MCmean_S[ene * 2 + 1] = MCmean_S[ene * 2] * MCmean_S[ene * 2];
}

//*******************´¶¼õÎ¨************************

void Quantities::NumberOfVertices(int countv, int mcs) {
  values_S[nver] = countv;
}

void Quantities::SpecificHeat() {
  MCmean_S[spe * 2] =
      (MCmean_S[nver * 2 + 1] - MCmean_S[nver * 2] * MCmean_S[nver * 2] - MCmean_S[nver * 2]) / (double)N->V;
  MCmean_S[spe * 2 + 1] = MCmean_S[spe * 2] * MCmean_S[spe * 2];
}

//*******************¥ï¡¼¥à¿ô************************

void Quantities::NumberOfWorms(vector<GraphSpace::Vertex> &ev, int m_Nw) {
#ifdef CFOUT
  for (int i = 0; i < V; i++)
    values_L[f_nw(i)] = 0;
  int Nw = 0;

  vector<GraphSpace::Vertex>::iterator it = ev.begin();
  while (it != ev.end()) {
    if (it->type == 4 || it->type == 5) {
      int site = it->i % V;
      values_L[f_nw(site)]++;
      Nw++;
    }

    ++it;
  }
#endif

  values_S[nwor] = m_Nw;
}
//*******************¥ï¡¼¥à¿ô************************

void Quantities::NumberOfKinks(int Nk, int mcs) {
  values_S[nkin] = Nk;
  values_S[ang]  = 2 * (Nk % 2) - 1;
}

//*******************´¶¼õÎ¨************************

void Quantities::Susceptibility() {
#ifdef CFOUT
  double lx = 0.0;

  for (int i = 0; i < V; i++) {
    MCmean_L[f_nw2(i)] =
        (MCmean_L[f_nw2(i)] - MCmean_L[f_nw(i)] * MCmean_L[f_nw(i)]) / (4.0 * N->B * sp->Htr * sp->Htr);
    lx += MCmean_L[f_nw2(i)];
  }

  MCmean_S[lxmx * 2]     = lx / (double)V;
  MCmean_S[lxmx * 2 + 1] = MCmean_S[lxmx * 2] * MCmean_S[lxmx * 2];
#endif

  MCmean_S[xmx * 2] =
      (MCmean_S[nwor * 2 + 1] - MCmean_S[nwor * 2] * MCmean_S[nwor * 2]) / (4.0 * N->V * N->B * sp->Htr * sp->Htr);

  MCmean_S[xmx * 2 + 1] = MCmean_S[xmx * 2] * MCmean_S[xmx * 2];
}

//*******************Ä¶Î®Æ°OP¡¢¥°¥ê¡¼¥ó´Ø¿ô************************

void Quantities::CondensateFraction(int mcs, GraphSpace::Vertex *world) {
  double atot = 0.0, ctot = 0.0;

  for (int i = 0; i < V; i++) {
    atot += an[i];
    ctot += cr[i];
  }

  atot /= N->V;
  ctot /= N->V;

  values_S[magx] = (atot + ctot) / 2.0 / PR->Ntdiv;
  values_S[magp] = atot / PR->Ntdiv;
  values_S[magm] = ctot / PR->Ntdiv;
#ifdef CFOUT
  CorrelationFunction1();
  //CorrelationFunction2(world);
#endif
}

void Quantities::CorrelationLength(double length) { values_S[len] = length; }

////////////////////////////////////
//***MC average

void Quantities::Average(){


#ifdef REWEIGHT
  double ZW=MCmean_S[2*ang];
#else
  double ZW=MC->Nsample;
#endif

  for(int i=0; i<Nq; i++){
#ifdef REWEIGHT
    if(i!=ang){
#endif
      MCmean_S[2*i] /= ZW;
      MCmean_S[2*i+1] /= ZW;
    }
#ifdef REWEIGHT
  }
  
  MCmean_S[ang*2] /= (double)MC->Nsample;
  MCmean_S[ang*2+1] /= (double)MC->Nsample;
#endif
  
#ifdef CFOUT
  for(int i=0; i<Lsize*2; i++){
    MCmean_L[i] /= ZW;
  }
#endif  
#ifdef SF
  for (int isf=0; isf<NKMAX; isf++) 
    for(int it=0;it<Ntau;it++)
      MCmean_SF[2*isf][it] /= ZW;
#endif

  

}

//****MC sum
void Quantities::MCsum_S() {
  if (PR->nst == 0) {
    for (int i = 0; i < Nq1; i++) {
#ifdef REWEIGHT
      if (i != ang) {
        MCmean_S[2 * i] += values_S[i] * values_S[ang];
        MCmean_S[2 * i + 1] += values_S[i] * values_S[i] * values_S[ang];
      } else {
        MCmean_S[2 * i] += values_S[i];
        MCmean_S[2 * i + 1] += values_S[i] * values_S[i];
      }
#else
      MCmean_S[2 * i] += values_S[i];
      MCmean_S[2 * i + 1] += values_S[i] * values_S[i];
#endif
    }
  }
}

#ifdef SF
void Quantities::MCsum_SF() {
  for (int isf = 0; isf < NKMAX; isf++)
    for (int it = 0; it < Ntau; it++)
#ifdef REWEIGHT
      MCmean_SF[2 * isf][it] += sfsamp[isf][it] * values_S[ang] / (double)N->V;
#else
      MCmean_SF[2 * isf][it] += sfsamp[isf][it] / (double)N->V;
#endif
}
#endif

void Quantities::MCsum_L() {
  int ltoc = Lsum[Nc1 - 1];

  if (PR->nt == 0) {
    for (int i = 0; i < ltoc; i++) {
#ifdef REWEIGHT
      MCmean_L[i] += values_L[i] * values_S[ang];
#else
      MCmean_L[i] += values_L[i];
#endif
    }
  }

  if (PR->nst == 0) {
    for (int i = ltoc; i < Lsize; i++) {
#ifdef REWEIGHT
      MCmean_L[i] += values_L[i] * values_S[ang];
#else
      MCmean_L[i] += values_L[i];
#endif
    }
  }
}

////////////////////////////////////
void Quantities::SUM_OVER_T() {
  if (PR->nt == 0) {  //Sum over t

    for (int tag = 1; tag < PR->Ntdiv; tag++) {
      MPI_Recv(m_val, V * 2, MPI_DOUBLE, tag + PR->nt0, 0, MPI_COMM_WORLD, &status);
      for (int i = 0; i < V * 2; i++)
        values_L[f_ld(i)] += m_val[i];
    }
#ifdef CFOUT
    for (int i = 0; i < V; i++) {
      values_L[f_nw2(i)] = values_L[f_nw(i)] * values_L[f_nw(i)];
    }
#endif

  } else {
    MPI_Send(&(values_L[f_ld(0)]), V * 2, MPI_DOUBLE, PR->nt0, 0, MPI_COMM_WORLD);
  }
}

/////////

void Quantities::SUM_OVER_S() {
  if (PR->ns == 0) {  //Sum over s at same tau

    double Norm = PR->Ntdiv;

    for (int tag = 1; tag < PR->Nsdiv; tag++) {
      MPI_Recv(m_val, 2, MPI_DOUBLE, tag * PR->Ntdiv + PR->ns0, 0, MPI_COMM_WORLD, &status);

      values_S[amzu] += m_val[0];
      values_S[smzs] += m_val[1];
    }

    values_S[smzu] = values_S[amzu] * values_S[amzu] / Norm;
    values_S[smzs] = values_S[smzs] * values_S[smzs] / Norm;
    values_S[amzu] /= Norm;

  } else {
    MPI_Send(&(values_S[amzu]), 2, MPI_DOUBLE, PR->ns0, 0, MPI_COMM_WORLD);

    values_S[amzu] = 0;
    values_S[smzs] = 0;
    values_S[smzu] = 0;
  }
}

/////////

void Quantities::SUM_OVER_ST() {
  if (PR->nst == 0) {  //Sum over t and s

    for (int tag = 1; tag < PR->NtNs; tag++) {
      MPI_Recv(m_val, Nq1, MPI_DOUBLE, tag + PR->nst0, 0, MPI_COMM_WORLD, &status);
      for (int i = 0; i < Nq1; i++) {
        values_S[i] += m_val[i];
      }
    }

  } else {
    MPI_Send(values_S, Nq1, MPI_DOUBLE, PR->nst0, 0, MPI_COMM_WORLD);
  }

  values_S[xmzu] = values_S[bmzu] * values_S[bmzu] / (double)N->V;
  values_S[xmzs] = values_S[xmzs] * values_S[xmzs] / (double)N->V;
  values_S[smzu] /= (double)N->V;
  values_S[smzs] /= (double)N->V;
  values_S[amzu] /= (double)N->V;
  values_S[bmzu] /= (double)N->V;
}

void Quantities::CorrelationFunction1() {  //real space

  //***MPI sum for G
  double ntot = 0.0;
  int Lcr2    = Lmax[cr2];
  int ixmax   = Lcr2 / PR->Nxdiv;
  double Norm;

  for (int i = 0; i < Lcr2; i++)
    values_L[f_gf(i)] = 0.0;

  if (PR->nx == 0) {  //Sum over x at same tau

    for (int i = 0; i < ixmax; i++) {
      int ixx               = i * Nxstep;
      this->Q[i]            = an[ixx];
      this->Q[i + Lcr2]     = cr[ixx];
      this->Q[i + 2 * Lcr2] = ca[ixx];
    }

    for (int tag = 1; tag < PR->Nxdiv; tag++) {
      MPI_Recv(m_val, ixmax * 3, MPI_DOUBLE, tag * PR->Ntdiv + PR->nx0, 0, MPI_COMM_WORLD, &status);
      for (int i = 0; i < ixmax; i++)
        this->Q[i + tag * ixmax] = m_val[i];
      for (int i = 0; i < ixmax; i++)
        this->Q[i + tag * ixmax + Lcr2] = m_val[i + ixmax];
      for (int i = 0; i < ixmax; i++)
        this->Q[i + tag * ixmax + 2 * Lcr2] = m_val[i + 2 * ixmax];
    }

    for (int ix = 0; ix < Lcr2; ix++)
      for (int ixx = 0; ixx < Lcr2; ixx++) {
        int rx = (ix - ixx + Lcr2) % Lcr2;
        if (ix == ixx)
          values_L[f_gf(0)] += this->Q[ix + 2 * Lcr2] * 2.0;
        else
          values_L[f_gf(rx)] += this->Q[ix] * this->Q[ixx + Lcr2] + this->Q[ix + Lcr2] * this->Q[ixx];
      }

    Norm = 2.0 * Lcr2 * PR->Ntdiv * PR->Nydiv * PR->Nzdiv;
    for (int i = 0; i < Lcr2; i++)
      values_L[f_gf(i)] /= Norm;

  } else {
    for (int i = 0; i < ixmax; i++) {
      int ixx                = i * Nxstep;
      this->Q[i]             = an[ixx];
      this->Q[i + ixmax]     = cr[ixx];
      this->Q[i + 2 * ixmax] = cr[ixx];
    }
    MPI_Send(this->Q, ixmax * 3, MPI_DOUBLE, PR->nx0, 0, MPI_COMM_WORLD);
  }

  if (PR->nst == 0) {  //Sum over t at x=0

    for (int tag = 1; tag < PR->NtNs; tag++) {
      if ((tag / PR->Ntdiv) % PR->Nxdiv != 0) continue;

      MPI_Recv(m_val, Lcr2, MPI_DOUBLE, tag + PR->nst0, 0, MPI_COMM_WORLD, &status);
      for (int i = 0; i < Lcr2; i++)
        values_L[f_gf(i)] += m_val[i];
    }

  } else if (PR->nx == 0) {
    MPI_Send(&(values_L[f_gf(0)]), Lcr2, MPI_DOUBLE, PR->nst0, 0, MPI_COMM_WORLD);
  }
}

void Quantities::CorrelationFunction2(GraphSpace::Vertex *world) {  //momentum space

  MPI_Status status;
  complex<double> Nk2;
  ////G2///

  for (int i = 0; i < Nk_set; i++)
    Ck[i] = complex<double>(0.0, 0.0);

  double rootV  = sqrt((double)N->V);
  double rootV2 = N->V;
  double rootV3 = rootV2 * rootV;
  double rootV4 = rootV2 * rootV2;

  for (int r = 0; r < V; r++) {
    double ancr = an[r] * cr[r];
    double n    = world[r].p;
    double ck0  = ancr - n;
    double nn   = 1.0 - n;

    Ck[0] += ck0 / rootV2;
    Ck[1] += ancr / rootV2;
    Ck[2] += (ancr * ancr - n) / rootV4;
    Ck[3] += n / rootV2;
    Ck[4] += (5.0 * ancr * ancr - 6.0 * n * ancr - 2.0 * nn * ancr + 2.0 * ancr + n * n + n * nn) / rootV4;

    for (int a = 0; a < 2; a++) {
      for (int k = 0; k < Nkmax; k++) {
        Ck[f_ck(k, 0, a)] += an[r] * EXPrk[theta(r, -k, a)] / rootV;
        Ck[f_ck(k, 1, a)] += cr[r] * EXPrk[theta(r, k, a)] / rootV;

        Ck[f_ck(k, 2, a)] += an[r] * an[r] * EXPrk[theta(r, -k, a)] / rootV2;
        Ck[f_ck(k, 3, a)] += cr[r] * cr[r] * EXPrk[theta(r, k, a)] / rootV2;

        Ck[f_ck(k, 5, a)] += ancr * EXPrk[theta(r, k, a)] / rootV2;
        Ck[f_ck(k, 7, a)] += n * EXPrk[theta(r, k, a)] / rootV2;
        Ck[f_ck(k, 9, a)] += nn * EXPrk[theta(r, k, a)] / rootV2;

        Ck[f_ck(k, 10, a)] += ck0 * an[r] * EXPrk[theta(r, -k, a)] / rootV3;
        Ck[f_ck(k, 11, a)] += ck0 * cr[r] * EXPrk[theta(r, k, a)] / rootV3;

        Ck[f_ck(k, 12, a)] += (ancr - nn) * an[r] * EXPrk[theta(r, -k, a)] / rootV3;
        Ck[f_ck(k, 13, a)] += (ancr - nn) * cr[r] * EXPrk[theta(r, k, a)] / rootV3;

        Ck[f_ck(k, 14, a)] += ancr * an[r] * EXPrk[theta(r, -k, a)] / rootV3;
        Ck[f_ck(k, 15, a)] += ancr * cr[r] * EXPrk[theta(r, k, a)] / rootV3;
      }
      for (int k = 1; k < Nkmax; k++) {
        Ck[f_ck(-k, 5, a)] += ancr * EXPrk[theta(r, -k, a)] / rootV2;  //ck(4)<=ck(-k)<=ck(5)
        Ck[f_ck(-k, 7, a)] += n * EXPrk[theta(r, -k, a)] / rootV2;
        Ck[f_ck(-k, 9, a)] += nn * EXPrk[theta(r, -k, a)] / rootV2;
      }
    }
  }

  if (PR->ns == 0) {  //Sum over site at same tau

    for (int tag = 1; tag < PR->Nsdiv; tag++) {
      MPI_Recv(Ck_m, Nk_set + Sk_set, MPI_DOUBLE_COMPLEX, tag * PR->Ntdiv + PR->ns0, 0, MPI_COMM_WORLD, &status);
      for (int i = 0; i < Nk_set + Sk_set; i++)
        Ck[i] += Ck_m[i];
    }

    ///////////////// Gk //////////////////////
    for (int a = 0; a < 2; a++) {
      for (int k = 0; k < Nkxmax; k++) {
        Nk2                             = (Ck[f_ck(k, 0, a)] * Ck[f_ck(k, 1, a)] - Ck[0]) / (double)N->V;
        values_L[f_nkr(k + a * Nkxmax)] = real(Nk2);
        values_L[f_nki(k + a * Nkxmax)] = imag(Nk2);
      }
    }
    //////////////////////////////////////////
    ///////////////// Sk //////////////////////
    //for(int k=0;k<Sk_set;k++){
    //  double Sk=real(Ck[Nk_set+k]);
    //  values_S[sku+k] = Sk*Sk/(double)PR->Ntdiv; //Structure factor
    //}
    //////////////////////////////////////////
  } else {
    MPI_Send(Ck, Nk_set + Sk_set, MPI_DOUBLE_COMPLEX, PR->ns0, 0, MPI_COMM_WORLD);
  }

  //
  complex<double> Gk2, Dkkk;
  if (PR->ns == 0) {
    for (int kx = 0; kx < Nkxmax; kx++) {  //(kx,ky,kz)=(0~Nkmax,0,0); (0~Nkmax,kx,kx)

      for (int kk = 0; kk < Nkkmax; kk++) {  //(k'x,k'y,k'z)=(0,0,0);...(depends on CKK)...; (kx,ky,kz)

        Gk2 = Ck[f_ck(kx, 0)] * Ck[f_ck(kx, 1)] * Ck[f_ck(kk, 0)] * Ck[f_ck(kk, 1)]

              - (Ck[0] * (Ck[f_ck(kk, 1)] * Ck[f_ck(kk, 0)] - Ck[1]) - Ck[f_ck(kk, 11)] * Ck[f_ck(kk, 0)]
                 - Ck[f_ck(kk, 10)] * Ck[f_ck(kk, 1)])  //i=j
              - (Ck[f_ck(kk + kx, 3)] * (Ck[f_ck(kx, 0)] * Ck[f_ck(kk, 0)] - Ck[f_ck(kk + kx, 2)])
                 - Ck[f_ck(kk, 15)] * Ck[f_ck(kk, 0)] - Ck[f_ck(kx, 15)] * Ck[f_ck(kx, 0)])  //i=l
              - ((Ck[f_ck(kx - kk, 5)] - Ck[f_ck(kx - kk, 7)])
                     * (Ck[f_ck(kx, 0)] * Ck[f_ck(kk, 1)] - Ck[f_ck(kk - kx, 5)])
                 - Ck[f_ck(kk, 10)] * Ck[f_ck(kk, 1)] - Ck[f_ck(kx, 11)] * Ck[f_ck(kx, 0)])  //i=m
              - (Ck[f_ck(kx + kk, 2)] * (Ck[f_ck(kx, 1)] * Ck[f_ck(kk, 1)] - Ck[f_ck(kk + kx, 3)])
                 - Ck[f_ck(kx, 14)] * Ck[f_ck(kx, 1)] - Ck[f_ck(kk, 14)] * Ck[f_ck(kk, 1)])  //j=m
              - ((Ck[f_ck(kk - kx, 5)] - Ck[f_ck(kk - kx, 9)])
                     * (Ck[f_ck(kx, 1)] * Ck[f_ck(kk, 0)] - Ck[f_ck(kx - kk, 5)])
                 - Ck[f_ck(kx, 12)] * Ck[f_ck(kx, 1)] - Ck[f_ck(kk, 13)] * Ck[f_ck(kk, 0)])  //j=l
              - (Ck[0] * (Ck[f_ck(kx, 1)] * Ck[f_ck(kx, 0)] - Ck[1]) - Ck[f_ck(kx, 11)] * Ck[f_ck(kx, 0)]
                 - Ck[f_ck(kx, 10)] * Ck[f_ck(kx, 1)])  //l=m

              - (Ck[f_ck(kk, 15)] - Ck[f_ck(kk, 1)] / rootV2) * Ck[f_ck(kk, 0)]  //i=j=l
              - Ck[f_ck(kk, 14)] * Ck[f_ck(kk, 1)]                               //i=j=m
              - Ck[f_ck(kx, 15)] * Ck[f_ck(kx, 0)]                               //i=m=l
              - (Ck[f_ck(kx, 14)] - Ck[f_ck(kx, 0)] / rootV2) * Ck[f_ck(kx, 1)]  //j=l=m

              - (Ck[1] * Ck[1] - Ck[3] * Ck[3])                                                              //i=j,l=m
              - (Ck[f_ck(kx - kk, 5)] * Ck[f_ck(kk - kx, 5)] - Ck[f_ck(kx - kk, 7)] * Ck[f_ck(kk - kx, 9)])  //i=m,j=l
              - Ck[f_ck(kk + kx, 3)] * Ck[f_ck(kk + kx, 2)]                                                  //i=l,j=m
              - Ck[2]                                                                                        //i=j=l=m
              - Ck[4];

        Dkkk = Gk2;  //vol2;

        //	  for(int kky=0; kky<1; kky++){ //ky=kky= 0 or kx(kkx)
        values_L[f_gk2r(kx, kk)] = real(Dkkk);
        values_L[f_gk2i(kx, kk)] = imag(Dkkk);
        // }
      }
    }
  }

  //*******
  int L3 = Lmax[ck2] + Lmax[ck4];  //for nk, gk2

  if (PR->nst == 0) {  //Sum over t=0

    for (int tag = 1; tag < PR->Ntdiv; tag++) {
      MPI_Recv(m_val, L3, MPI_DOUBLE, tag + PR->nst0, 0, MPI_COMM_WORLD, &status);
      for (int i = 0; i < L3; i++)
        values_L[f_nk(i)] += m_val[i];
    }

    for (int i = 0; i < L3; i++)
      values_L[f_nk(i)] /= (double)PR->Ntdiv;

  } else if (PR->ns == 0) {  //Ntdiv prosessors for nk, gf2, gk2
    MPI_Send(&(values_L[f_nk(0)]), L3, MPI_DOUBLE, PR->nst0, 0, MPI_COMM_WORLD);
  }
}
