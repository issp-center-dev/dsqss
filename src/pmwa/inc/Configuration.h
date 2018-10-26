#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include <stdio.h>

#include <float.h>
#include <math.h>
#include <stdlib.h>
#include <fstream>
#include <iostream>
#include <iomanip>

#include <stack>
#include <vector>
#include <list>
#include <algorithm>
#include <systemparameter.h>
#include <mpi.h>
#include <string.h>

#include <simtype.h>

#define G_NMIN 1.0e-8
#define NSAMPLE 100000

#include <xml.hpp>

using namespace XML;

#include <lattice.hpp>

//************************************GraphSpaceクラスここから***********************************************************

class Quantities;

class Probability;

class GraphSpace {
public:
  double NMIN;

  double msh(double);

  double mch(double);

  double msinh(double);

  double mcosh(double);

  double (GraphSpace::*fexp[2])(double);

  double (GraphSpace::*fmath[2])(double);

  int my_rank;

  //ワームはonsiteバーテックスとして扱い、双方向リストをつなぎ変えながら動く
  class Vertex {
  public:
    double t;  //imaginary time
    short p;   //the number of bosons
    int i;     //bond index = site + d*V
    short type;
    //  -5: temporal edge, -2(left),-4(right),:inter-domain vertex, -1(l),-3(r),:inter-domain kink,
    //    0:on-site, 1: 2-site vertex, 2:kink, 3:2site-vertex(second nearest), 4:active "annihilation"-worm, 5:active "creation"-worm,  6:nonactive worm or other, 7:marker
    bool dir;  //for worm, moving direction. for vertex, parity.

    Vertex *next[2];  //doubly-linked list
    Vertex *nleg;     //the opposite side of the 2-site vertex.
  };

  Parallel *PR;

  Vertex *world, *worldB;  //t=0での各サイトの粒子数
  std::vector<Vertex> ev;
  std::stack<Vertex *> VertexPool;
  std::vector<Vertex *> WORM;

  int *W;  //ワームのイベント番号を保管 //ただし0番目はワームの個数

  int **After, **Switch;
  int **op;
  short sgn[2];

  double rtb_i, rt_i, rtb_tot, rt_tot, rt_frame, rtot;
  double Pv1,  PBv1;
  double Wlen;
  long Ncyc;
  int Nv, Nk, Nu;
  double OPcount;

  Lattice *LT;
  Probability *P;
  int EVmax;
  bool For_Nworm;  //ワームの数を決めるために長さを測る

  double B, Bh;
  double BetaOrg;
  double BetaOldOrg;
  int V;
  int D;
  int VD;
  int ES;

  int nmax;
  double tb;
  double Vbb;
  long long IMAX, WMAX;
  long long Vnum;
  std::string outfile;
  std::string LATFILE;
  std::string ALGFILE;

public:
  GraphSpace(Size *N, int m_nmax, Lattice *m_LT, Probability *m_P, Parallel *PR, long long m_IMAX, long long m_WMAX,
             std::string const& m_Eventfile, My_rdm *MR, int cb, std::string const& m_outfile);

  ~GraphSpace();

  void Output(std::string const &fname, My_rdm *MR);

protected:
  void Worm_Update(My_rdm *MR);

  bool Diagonal_Update(My_rdm *MR, Quantities *QNT);

  void DLA_Update(My_rdm *MR);

private:
  double *dtn;
  Vertex **w;

  Vertex **vertex;
  Vertex *worm;

  int *NewVertex_th0;
  int *NewVertex_th1;

  double *i1_box, *i0_box;
  short *p1_box, *p0_box;

  int **wf, **newt, **newt_th1;

  int boxsize;
  double **BoxSpace_t_th0;
  short **BoxSpace_p_th0;
  short *BoxSpace_py_th0;
  short *BoxSpace_type_th0;

  double **BoxSpace_t_th1;
  short **BoxSpace_p_th1;
  short *BoxSpace_py_th1;
  short *BoxSpace_type_th1;

  double *ver_time;
  int *INmax;  //サイトiでの区間の数

  double *I, *t, tA;  //  t[1]|    |t[2]
  //      |----|
  //   t[0]|    |t[3]

  inline int f(int d, int i, int j) { return i + j * LT->Fx[d]; }
  //world->(x座標,y座標)、Box->(1次元的座標,バーテックス)

  //worm
  bool Next_event(My_rdm *MR);

  void transition(My_rdm *MR);

  void t_update(My_rdm *MR, int b, int a, bool oh, bool dir);

  inline double LengthofWalk(double next_t, double curr_t, bool dir);

  void k_neibor();

  void k_neibor_dir();

  void v_neibor();

  void v_neibor_dir();

  void Newtime(int a, double dla);

  void relink(bool b_dir, bool a_dir, bool aside);

  bool update_selection(My_rdm *MR, bool dir, short type);

  bool update_selection(My_rdm *MR, bool dir);

  void bounce(bool oh, bool dir, int b, int a, double dla);

  void pass(bool oh, bool dir, int b, int a, double dla);

  void hop(bool oh, bool dir, int b, int a, double dla);

  void turn(bool oh, bool dir, int b, int a, double dla);

  void (GraphSpace::*Transition[4])(bool oh, bool dir, int b, int a, double dla);

  //vertex
  void initialev(std::string const& m_Eventfile, My_rdm *MR, int cb, double oldB);

  void Remove_Vertex();

  void Assign_TwoSiteVertex(My_rdm *MR);

  void worm_release(My_rdm *MR);

  void Assign_Worm(My_rdm *MR);

  void Assign_OnsiteVertex(My_rdm *MR);

  void connect_before(Vertex *wl, Vertex *new_event);

  void connect_after(Vertex **wl, Vertex *el, int x);

  void parity_check(My_rdm *MR, int py, double *x, int &j, double topt, double bottomt);

  int NumberOfVertex(My_rdm *MR, double m, int py);

  void CondensateFraction(int site, Quantities *QNT, bool py0, bool py1, double exp0, double exp1);

  void OldBox();

  void NewBox();

  void release(Vertex *wx);

  void insert(Vertex *vl, Vertex *vr, short type, double targ_time, int xl, int xr, int pl, int pr, int d);

  void insert(Vertex *v, short type, double targ_time, int xl, int pl, int d);

  void insert_NewEvent(Vertex *v, int new_type, double new_time, int xx, int px, int d);

  void Renew_Vertex(Vertex *v, int new_type, double new_time, int xx, int px, int d);

  void relink(Vertex *v1, Vertex *v2, Vertex *v3);

  void initial_functions();

  //###################################################

  void (GraphSpace::*Parity_Update[6][6])(Vertex *wl, int d, int i, int rnum);

  void All(Vertex *wl, int d, int i, int rnum);

  void Right(Vertex *wl, int d, int i, int rnum);

  void Left(Vertex *wl, int d, int i, int rnum);

  void LuRu(Vertex *wl, int d, int i, int rnum);

  void LdRd(Vertex *wl, int d, int i, int rnum);

  void LdRu(Vertex *wl, int d, int i, int rnum);

  void LuRd(Vertex *wl, int d, int i, int rnum);

  void (GraphSpace::*Event_Update[6])(Vertex *wl, int d, int i, int rnum);

  void a00(Vertex *wl, int d, int i, int rnum);

  void a01(Vertex *wl, int d, int i, int rnum);

  void a10(Vertex *wl, int d, int i, int rnum);

  void a11(Vertex *wl, int d, int i, int rnum);

  void t01(Vertex *wl, int d, int i, int rnum);

  void t10(Vertex *wl, int d, int i, int rnum);

  double (GraphSpace::*Form[6][6])(double I1, double I2, double I3, double I4, bool py1, bool py2, bool py3, bool py4);

  double P_All(double I1, double I2, double I3, double I4, bool py1, bool py2, bool py3, bool py4);

  double P_Right(double I1, double I2, double I3, double I4, bool py1, bool py2, bool py3, bool py4);

  double P_Left(double I1, double I2, double I3, double I4, bool py1, bool py2, bool py3, bool py4);

  double P_LuRu(double I1, double I2, double I3, double I4, bool py1, bool py2, bool py3, bool py4);

  double P_LdRd(double I1, double I2, double I3, double I4, bool py1, bool py2, bool py3, bool py4);

  double P_LdRu(double I1, double I2, double I3, double I4, bool py1, bool py2, bool py3, bool py4);

  double P_LuRd(double I1, double I2, double I3, double I4, bool py1, bool py2, bool py3, bool py4);

  double P_Stay(double I1, double I2, double I3, double I4, bool py1, bool py2, bool py3, bool py4);

  void SpatialDomainBoundary(My_rdm *MR);

  void TemporalDomainBoundary(My_rdm *MR, Quantities *QNT);

  void SpatialDomainBoundary();
};

//************************************GraphSpaceクラスおわり***********************************************************

//************************************Configurationクラスここから***********************************************************

#include <Quantities.h>

class Configuration : public GraphSpace {
  Lattice *LT;
  Size *N;

  void initialization();

  double BV;
  int p_num;
  MC_p *MC;

public:
  void dump(ofstream &F);

  Configuration(MC_p *m_MC, Size *N, int m_nmax, Lattice *m_LT, Probability *m_P, Parallel *PR, long long m_IMAX,
                long long m_WMAX, std::string const& m_Eventfile, My_rdm *MR, bool cb, std::string const& m_outfile);

  ~Configuration();

  void updateAnner(int MCS, My_rdm *MR, Quantities *QNT);

  void updateAnner(My_rdm *MR, Quantities *QNT);

  void sweep(int MCS, My_rdm *MR, Quantities *QNT);

  void measurement(Quantities *QNT, My_rdm *MR);

  void DeterminationNworm(int MCS, My_rdm *MR, Quantities *QNT);
  //  void update( int MCS, My_rdm *MR, int &Wnum );
};

//************************************Configurationクラスおわり***********************************************************

#endif
