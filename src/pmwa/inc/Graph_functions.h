#include "Configuration.h"
#include "Probability.h"

void GraphSpace::initial_functions() {
  fexp[0]       = &GraphSpace::mch;
  fexp[1]       = &GraphSpace::msh;
  fmath[0]      = &GraphSpace::mcosh;
  fmath[1]      = &GraphSpace::msinh;
  Transition[0] = &GraphSpace::bounce;
  Transition[1] = &GraphSpace::pass;
  Transition[2] = &GraphSpace::hop;
  Transition[3] = &GraphSpace::turn;

  for (int i = 0; i < 6; i++)
    Form[i][i] = &GraphSpace::P_Stay;
  Form[0][3] = Form[3][0] = Form[1][2] = Form[2][1] = Form[4][5] = Form[5][4] = &GraphSpace::P_All;

  Form[0][1] = Form[1][0] = &GraphSpace::P_Right;
  Form[2][3] = Form[3][2] = &GraphSpace::P_Right;

  Form[0][2] = Form[2][0] = &GraphSpace::P_Left;
  Form[1][3] = Form[3][1] = &GraphSpace::P_Left;

  Form[1][4] = Form[4][1] = &GraphSpace::P_LdRd;
  Form[2][5] = Form[5][2] = &GraphSpace::P_LdRd;

  Form[1][5] = Form[5][1] = &GraphSpace::P_LuRu;
  Form[2][4] = Form[4][2] = &GraphSpace::P_LuRu;

  Form[0][5] = Form[5][0] = &GraphSpace::P_LuRd;
  Form[3][4] = Form[4][3] = &GraphSpace::P_LuRd;

  Form[0][4] = Form[4][0] = &GraphSpace::P_LdRu;
  Form[3][5] = Form[5][3] = &GraphSpace::P_LdRu;

  Parity_Update[0][3] = Parity_Update[3][0] = Parity_Update[1][2] = Parity_Update[2][1] = Parity_Update[4][5] =
      Parity_Update[5][4]                                                               = &GraphSpace::All;

  Parity_Update[0][1] = Parity_Update[1][0] = &GraphSpace::Right;
  Parity_Update[2][3] = Parity_Update[3][2] = &GraphSpace::Right;

  Parity_Update[0][2] = Parity_Update[2][0] = &GraphSpace::Left;
  Parity_Update[1][3] = Parity_Update[3][1] = &GraphSpace::Left;

  Parity_Update[1][4] = Parity_Update[4][1] = &GraphSpace::LdRd;
  Parity_Update[2][5] = Parity_Update[5][2] = &GraphSpace::LdRd;

  Parity_Update[1][5] = Parity_Update[5][1] = &GraphSpace::LuRu;
  Parity_Update[2][4] = Parity_Update[4][2] = &GraphSpace::LuRu;

  Parity_Update[0][5] = Parity_Update[5][0] = &GraphSpace::LuRd;
  Parity_Update[3][4] = Parity_Update[4][3] = &GraphSpace::LuRd;

  Parity_Update[0][4] = Parity_Update[4][0] = &GraphSpace::LdRu;
  Parity_Update[3][5] = Parity_Update[5][3] = &GraphSpace::LdRu;

  Event_Update[0] = &GraphSpace::a00;
  Event_Update[1] = &GraphSpace::a01;
  Event_Update[2] = &GraphSpace::a10;
  Event_Update[3] = &GraphSpace::a11;
  Event_Update[4] = &GraphSpace::t01;
  Event_Update[5] = &GraphSpace::t10;
}

double GraphSpace::msh(double expD) { return (expD - 1.0 / expD) / 2.0; }

double GraphSpace::mch(double expD) { return (expD + 1.0 / expD) / 2.0; }

double GraphSpace::msinh(double D) { return sinh(D); }

double GraphSpace::mcosh(double D) { return cosh(D); }

//#######################################################################################################

double GraphSpace::P_Stay(double I1, double I2, double I3, double I4, bool py1, bool py2, bool py3, bool py4) {
  return 1.0;
}

double GraphSpace::P_All(double I1, double I2, double I3, double I4, bool py1, bool py2, bool py3, bool py4) {
  double fth1 = I1;  //tanh(I1*P->rh_even);
  double fth2 = I2;  //tanh(I2*P->rh_even);
  double fth3 = I3;  //tanh(I3*P->rh_even);
  double fth4 = I4;  //tanh(I4*P->rh_even);

  if (py1) fth1 = 1.0 / fth1;
  if (py2) fth2 = 1.0 / fth2;
  if (py3) fth3 = 1.0 / fth3;
  if (py4) fth4 = 1.0 / fth4;

  return fth1 * fth2 * fth3 * fth4;
}

double GraphSpace::P_Right(double I1, double I2, double I3, double I4, bool py1, bool py2, bool py3, bool py4) {
  double fth3 = I3;  //tanh(I3*P->rh_even);
  double fth4 = I4;  //tanh(I4*P->rh_even);

  if (py3) fth3 = 1.0 / fth3;
  if (py4) fth4 = 1.0 / fth4;

  return fth3 * fth4;
}

double GraphSpace::P_Left(double I1, double I2, double I3, double I4, bool py1, bool py2, bool py3, bool py4) {
  double fth1 = I1;  //tanh(I1*P->rh_even);
  double fth2 = I2;  //tanh(I2*P->rh_even);

  if (py1) fth1 = 1.0 / fth1;
  if (py2) fth2 = 1.0 / fth2;

  return fth1 * fth2;
}

double GraphSpace::P_LuRd(double I1, double I2, double I3, double I4, bool py1, bool py2, bool py3, bool py4) {
  double fth2 = I2;  //tanh(I2*P->rh_even);
  double fth4 = I4;  //tanh(I4*P->rh_even);

  if (py2) fth2 = 1.0 / fth2;
  if (py4) fth4 = 1.0 / fth4;

  return fth2 * fth4;
}

double GraphSpace::P_LdRu(double I1, double I2, double I3, double I4, bool py1, bool py2, bool py3, bool py4) {
  double fth1 = I1;  //tanh(I1*P->rh_even);
  double fth3 = I3;  //tanh(I3*P->rh_even);

  if (py1) fth1 = 1.0 / fth1;
  if (py3) fth3 = 1.0 / fth3;

  return fth1 * fth3;
}

double GraphSpace::P_LuRu(double I1, double I2, double I3, double I4, bool py1, bool py2, bool py3, bool py4) {
  double fth2 = I2;  //tanh(I2*P->rh_even);
  double fth3 = I3;  //tanh(I3*P->rh_even);

  if (py2) fth2 = 1.0 / fth2;
  if (py3) fth3 = 1.0 / fth3;

  return fth2 * fth3;
}

double GraphSpace::P_LdRd(double I1, double I2, double I3, double I4, bool py1, bool py2, bool py3, bool py4) {
  double fth1 = I1;  //tanh(I1*P->rh_even);
  double fth4 = I4;  //tanh(I4*P->rh_even);

  if (py1) fth1 = 1.0 / fth1;
  if (py4) fth4 = 1.0 / fth4;

  return fth1 * fth4;
}

//#######################################################################################################

void GraphSpace::All(Vertex *wl, int d, int i, int rnum) {
  wl->dir                            = !wl->dir;
  wl->next[1]->dir                   = !wl->next[1]->dir;
  BoxSpace_py_th1[f(d, i, rnum + 1)] = !((bool)BoxSpace_py_th1[f(d, i, rnum + 1)]);
  BoxSpace_py_th1[f(d, i, rnum)]     = !((bool)BoxSpace_py_th1[f(d, i, rnum)]);
}

void GraphSpace::Right(Vertex *wl, int d, int i, int rnum) {
  BoxSpace_py_th1[f(d, i, rnum + 1)] = !((bool)BoxSpace_py_th1[f(d, i, rnum + 1)]);
  BoxSpace_py_th1[f(d, i, rnum)]     = !((bool)BoxSpace_py_th1[f(d, i, rnum)]);
}

void GraphSpace::Left(Vertex *wl, int d, int i, int rnum) {
  wl->dir          = !wl->dir;
  wl->next[1]->dir = !wl->next[1]->dir;
}

void GraphSpace::LdRu(Vertex *wl, int d, int i, int rnum) {
  wl->dir                            = !wl->dir;
  BoxSpace_py_th1[f(d, i, rnum + 1)] = !((bool)BoxSpace_py_th1[f(d, i, rnum + 1)]);
}

void GraphSpace::LuRd(Vertex *wl, int d, int i, int rnum) {
  wl->next[1]->dir               = !wl->next[1]->dir;
  BoxSpace_py_th1[f(d, i, rnum)] = !((bool)BoxSpace_py_th1[f(d, i, rnum)]);
}

void GraphSpace::LuRu(Vertex *wl, int d, int i, int rnum) {
  wl->next[1]->dir                   = !wl->next[1]->dir;
  BoxSpace_py_th1[f(d, i, rnum + 1)] = !((bool)BoxSpace_py_th1[f(d, i, rnum + 1)]);
}

void GraphSpace::LdRd(Vertex *wl, int d, int i, int rnum) {
  wl->dir                        = !wl->dir;
  BoxSpace_py_th1[f(d, i, rnum)] = !((bool)BoxSpace_py_th1[f(d, i, rnum)]);
}

//#######################################################################################################
void GraphSpace::a00(Vertex *wl, int d, int i, int rnum) {
  wl->type                         = -2;
  BoxSpace_type_th1[f(d, i, rnum)] = -4;
  wl->p                            = 0;
  BoxSpace_p_th1[d][f(d, i, rnum)] = 0;
}

void GraphSpace::a01(Vertex *wl, int d, int i, int rnum) {
  wl->type                         = -2;
  BoxSpace_type_th1[f(d, i, rnum)] = -4;
  wl->p                            = 0;
  BoxSpace_p_th1[d][f(d, i, rnum)] = 1;
}

void GraphSpace::a10(Vertex *wl, int d, int i, int rnum) {
  wl->type                         = -2;
  BoxSpace_type_th1[f(d, i, rnum)] = -4;
  wl->p                            = 1;
  BoxSpace_p_th1[d][f(d, i, rnum)] = 0;
}

void GraphSpace::a11(Vertex *wl, int d, int i, int rnum) {
  wl->type                         = -2;
  BoxSpace_type_th1[f(d, i, rnum)] = -4;
  wl->p                            = 1;
  BoxSpace_p_th1[d][f(d, i, rnum)] = 1;
}

void GraphSpace::t01(Vertex *wl, int d, int i, int rnum) {
  wl->type                         = -1;
  BoxSpace_type_th1[f(d, i, rnum)] = -3;
  wl->p                            = 0;
  BoxSpace_p_th1[d][f(d, i, rnum)] = 1;
}

void GraphSpace::t10(Vertex *wl, int d, int i, int rnum) {
  wl->type                         = -1;
  BoxSpace_type_th1[f(d, i, rnum)] = -3;
  wl->p                            = 1;
  BoxSpace_p_th1[d][f(d, i, rnum)] = 0;
}
