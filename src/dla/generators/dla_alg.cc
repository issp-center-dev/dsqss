#include "dla_alg.h"

bool isdiagonal(int *x, int NBODY){
  for(int i=0; i<NBODY; ++i){
    if (x[2*i] != x[2*i+1]){
      return false;
    }
  }
  return true;
}

int main(int argc, char** argv) {
  HFILE = "hamiltonian.xml";
  AFILE = "algorithm.xml";
  if (argc > 1) { HFILE = argv[1]; }
  if (argc > 2) { AFILE = argv[2]; }
  printf("HFILE= %s\n", HFILE);
  printf("AFILE= %s\n", AFILE);

  XML::Block X(HFILE, "Hamiltonian");
  FALG = fopen(AFILE, "w");
  //FALG = stdout;

  XML::Block& XGEN = X["General"];
  GENERAL G(XGEN);
  Site        = new SITE[NSTYPE];
  Source      = new SOURCE[NSTYPE];
  Interaction = new INTERACTION[NITYPE];
  Vertex      = new VERTEX[NVTYPE];

  for (int i = 0; i < X.NumberOfBlocks(); i++) {
    XML::Block& B      = X[i];
    const std::string& name = B.getName();
    if (name == "Site") {
      int id = B["STYPE"].getInteger();
      Site[id].load(B);
      //      Site[id].dump();
    } else if (name == "Source") {
      int id = B["TTYPE"].getInteger();
      Source[id].load(B);
      //      Source[id].dump();
      //      Vertex[Source[id].VTYPE].dump();
    } else if (name == "Interaction") {
      int id = B["ITYPE"].getInteger();
      Interaction[id].load(B);
      //      Interaction[id].dump();
      //      Vertex[Interaction[id].VTYPE].dump();
    }
  }

  // Scattering probs

  for (int i = 0; i < NVTYPE; i++) {
    Vertex[i].Grouping();
  }
  // Worm vertex has the shared EBASE
  G.WeightDiagonal = -INF;

  for (int i = 0; i < NSTYPE; i++) {
    VERTEX& V = Site[i].V();
    double eb = V.ComputeEBASE();
    if (G.WeightDiagonal < 0.5 * eb) G.WeightDiagonal = 0.5 * eb;
  }

  for (int i = 0; i < NSTYPE; i++) {
    VERTEX& V = Site[i].V();
    V.EBASE   = 2.0 * G.WeightDiagonal;
  }

  // Interaction vertex has the own EBASE

  for (int i = 0; i < NITYPE; i++) {
    VERTEX& V = Interaction[i].V();
    V.EBASE   = V.ComputeEBASE();
  }

  for (int i = 0; i < NVTYPE; i++)
    Vertex[i].ComputeScatteringProbability();

  for (int i = 0; i < NSTYPE; i++) {
    Site[i].SetInitialHeadTypeProbability();
  }

  for (int i = 0; i < NITYPE; i++) {
    Interaction[i].SetVertexDensity();
  }


  fprintf(FALG, "<Algorithm>\n");
  G.write();
  for (int i = 0; i < NSTYPE; i++)
    Site[i].write();
  for (int i = 0; i < NITYPE; i++)
    Interaction[i].write();
  for (int i = 0; i < NVTYPE; i++)
    Vertex[i].write();
  fprintf(FALG, "\n</Algorithm>\n");

  delete[] Site;
  delete[] Source;
  delete[] Interaction;
  delete[] Vertex;
}

//######################################################################

GENERAL::GENERAL(XML::Block& X) {
  NHTYPE = 0;
  NXMAX  = 0;

  comment = X["Comment"].getJoinedString();
  NSTYPE  = X["NSTYPE"].getInteger();
  NITYPE  = X["NITYPE"].getInteger();
  NXMAX   = X["NXMAX"].getInteger();
  NVTYPE  = NSTYPE + NITYPE;
}

//======================================================================

void GENERAL::write() {
  fprintf(FALG, "\n");
  fprintf(FALG, "  <Comment>\n");
  fprintf(FALG, "    %s\n", comment.c_str());
  fprintf(FALG, "  </Comment>\n");
  fprintf(FALG, "\n");
  fprintf(FALG, "  <General>\n");
  fprintf(FALG, "    <NSTYPE> %2d </NSTYPE>\n", NSTYPE);
  fprintf(FALG, "    <NITYPE> %2d </NITYPE>\n", NITYPE);
  fprintf(FALG, "    <NVTYPE> %2d </NVTYPE>\n", NVTYPE);
  fprintf(FALG, "    <NXMAX>  %2d </NXMAX>\n", NXMAX);
  fprintf(FALG, "    <WDIAG>  %24.16lf </WDIAG>\n", WeightDiagonal);
  fprintf(FALG, "  </General>\n");
}

//######################################################################

void SITE::load(XML::Block& X) {
  ID    = X["STYPE"].getInteger();
  TTYPE = X["TTYPE"].getInteger();
  NX    = X["NX"].getInteger();
  _T    = &Source[TTYPE];
}

//======================================================================

void SITE::SetInitialHeadTypeProbability() {
  VTYPE = Source[TTYPE].VTYPE;
  // WormCreationNewState( xi, ch)
  // "xi". .. the initial local state
  // "ch" ... the scattering channel id
  WormCreationNewState.init(2, NXMAX, 2 * NXMAX);
  WormCreationNewState.set_all(STATE::UNDEF);
  WormCreationDirection.init(2, NXMAX, 2 * NXMAX);
  WormCreationDirection.set_all(DIR::UNDEF);
  WormCreationProbability.init(2, NXMAX, 2 * NXMAX);
  WormCreationProbability.set_all(0.0);
  NumberOfChannels = new int[NX];
  VERTEX& V        = Vertex[VTYPE];
  for (int i = 0; i < V.NICG; i++) {
    InitialConfigurationGroup& icg = V.ICG(i);
    for (int j = 0; j < icg.NIC; j++) {
      InitialConfiguration& ic = icg.IC[j];
      if (ic.INC == DIR::UNDEF) {
        int x0               = ic.State[0];
        int NCH              = ic.NCH;
        NumberOfChannels[x0] = NCH;
        for (int c = 0; c < NCH; c++) {
          int st                         = ic.FinalState[c];
          int out                        = ic.FinalDirection[c];
          double p                       = ic.ScatteringProbability[c];
          WormCreationNewState(x0, c)    = st;
          WormCreationDirection(x0, c)   = out;
          WormCreationProbability(x0, c) = p;
        }
      }
    }
  }
}

//======================================================================

void SITE::write() {
  fprintf(FALG, "\n");
  fprintf(FALG, "  <Site>\n");
  fprintf(FALG, "    <STYPE> %d </STYPE>\n", ID);
  fprintf(FALG, "    <NumberOfStates> %d </NumberOfStates>\n", NX);
  fprintf(FALG, "    <VertexTypeOfSource> %d </VertexTypeOfSource>\n", VTYPE);
  //printf("    <EBASE> %24.16f </EBASE>\n", V().EBASE );
  //fprintf(FALG,"    <EBASE> %24.16f </EBASE>\n", V().EBASE );
  for (int x0 = 0; x0 < NX; x0++) {
    fprintf(FALG, "    <InitialConfiguration>\n");
    fprintf(FALG, "      <State> %d </State>\n", x0);
    int NCH = NumberOfChannels[x0];
    fprintf(FALG, "      <NumberOfChannels> %d </NumberOfChannels>\n", NCH);
    for (int c = 0; c < NCH; c++) {
      int diri = WormCreationDirection(x0, c);
      int xnew = WormCreationNewState(x0, c);
      double p = WormCreationProbability(x0, c);
      if (p == 0.0) continue;
      fprintf(FALG, "      <Channel> %4d %4d %24.16lf </Channel>\n", diri, xnew,
              p);
    }
    fprintf(FALG, "    </InitialConfiguration>\n");
  }
  fprintf(FALG, "  </Site>\n");
}

//======================================================================

void SITE::dump() {
  printf("\n");
  printf("SITE[%d]> tt=%2d, NX=%2d\n", ID, TTYPE, NX);
  if (NumberOfChannels == 0) {
    printf(" ... Channels have not been defined yet.\n");
  } else {
    for (int x = 0; x < NX; x++) {
      printf("  x=%2d : ", x);
      int NCH = NumberOfChannels[x];
      printf("  NCH= %d\n", NCH);
      for (int c = 0; c < NCH; c++) {
        printf("   %2d %2d %8.3lf\n", WormCreationDirection(x, c),
               WormCreationNewState(x, c), WormCreationProbability(x, c));
      }
    }
  }
}

//######################################################################

void SOURCE::load(XML::Block& X) {
  ID         = X["TTYPE"].getInteger();
  VTYPE      = ID;
  VERTEX& V  = Vertex[VTYPE];
  _V         = &V;
  V.ID       = VTYPE;
  V.NBODY    = 1;
  V.CATEGORY = VCAT::WORM;
  V.load(X);
}

//======================================================================

void SOURCE::dump() { printf("SOURCE[%d]> vt=%2d\n", ID, VTYPE); }

//######################################################################

void INTERACTION::load(XML::Block& X) {
  ID           = X["ITYPE"].getInteger();
  VTYPE        = ID + NSTYPE;
  _V           = &(Vertex[VTYPE]);
  NBODY        = X["NBODY"].getInteger();
  V().ID       = VTYPE;
  V().NBODY    = NBODY;
  V().CATEGORY = VCAT::INT;
  V().load(X);
}

//======================================================================

void INTERACTION::SetVertexDensity() {
  VERTEX& V      = Vertex[VTYPE];
  IndexSystem& I = V.Weight.index_system();
  int* x         = new int[I.dimension()];
  VertexDensity.init(NBODY, NXMAX, ARRAY::EOL);
  IndexSystem& J = VertexDensity.index_system();
  int* y = new int[J.dimension()];  //edit sakakura
  VertexDensity.set_all(0.0);
  for (int g = 0; g < V.NICG; g++) {
    InitialConfigurationGroup& G = V.ICG(g);
    for (int c = 0; c < G.NIC; c++) {
      InitialConfiguration& C = G.IC[c];
      if (!C.isKink()) {
        int sti  = C.STI;
        double w = C.vertex_weight();
        I.coord(sti, x);
        for (int i = 0; i < NBODY; i++)
          y[i] = x[2 * i];
        VertexDensity(y) = w;
      }
    }
  }
  delete[] x;
  delete[] y;
}

//======================================================================

void INTERACTION::write() {
  fprintf(FALG, "\n");
  fprintf(FALG, "  <Interaction>\n");
  fprintf(FALG, "    <ITYPE> %d </ITYPE>\n", ID);
  fprintf(FALG, "    <VTYPE> %d </VTYPE>\n", VTYPE);
  fprintf(FALG, "    <NBODY> %d </NBODY>\n", NBODY);
  fprintf(FALG, "    <EBASE> %24.16lf </EBASE>\n", V().EBASE);
  if (VertexDensity.isDefined()) {
    IndexSystem& I = VertexDensity.index_system();
    int* x = new int[NBODY];
    for (int i = 0; i < I.size(); i++) {
      double d = VertexDensity[i];
      if (d > 0.0) {
        I.coord(i, x);
        fprintf(FALG, "    <VertexDensity> ");
        for (int j = 0; j < NBODY; j++)
          fprintf(FALG, " %2d", x[j]);
        fprintf(FALG, " %24.16lf </VertexDensity>\n", d);
      }
    }
    delete[] x;
  } else {
    //  fprintf(FALG,"  VertexDensity is not defined.\n");
  }

  int NLEG = 2*NBODY;
  Array<double> & Weight = V().Weight;
  IndexSystem& I = Weight.index_system();
  int* x = new int[2*NBODY];
  for (int i=0; i<I.size(); ++i){
    if(Weight[i] != 0.0){
      I.coord(i,x);
      if(!isdiagonal(x,NBODY)){
        double sgn = Weight[i] > 0.0 ? 1.0 : -1.0;
        fprintf(FALG, "    <Sign> ");
        for (int j = 0; j<2*NBODY; ++j){
          fprintf(FALG, " %2d", x[j]);
        }
        fprintf(FALG, " %24.16lf </Sign>\n", sgn);
      }
    }
  }
  delete[] x;

  fprintf(FALG, "  </Interaction>\n");
}

//======================================================================

void INTERACTION::dump() {
  printf("INTERACTION[%d] ", ID);
  printf(", vt=%2d", VTYPE);
  printf(", nb=%2d", NBODY);
  printf("\n");
  if (VertexDensity.isDefined()) {
    IndexSystem& I = VertexDensity.index_system();
    int* x = new int[NBODY];
    for (int i = 0; i < I.size(); i++) {
      if (VertexDensity[i] > 0.0) {
        I.coord(i, x);
        printf(" x= (");
        for (int j = 0; j < I.dimension(); j++)
          printf(" %1d", x[j]);
        printf(")");
        printf(" density= %8.3f\n", VertexDensity[i]);
        printf("\n");
      }
    }
    delete[] x;
  } else {
    printf("  VertexDensity is not defined.\n");
  }
}

//######################################################################

void VERTEX::load(XML::Block& X) {
  NLEG = 2 * NBODY;
  Weight.init("Weight", NLEG, NXMAX, ARRAY::EOL);
  setINDX(Weight.index_system());
  Weight.set_all(0.0);
  NST = Weight.size();
  SiteTypeOfLeg.init("STYPE", 1, NLEG);
  SiteTypeOfLeg.set_all(STYPE::UNDEF);
  for (int i = 0; i < NBODY; i++) {
    int st                   = X["STYPE"].getInteger(i);
    SiteTypeOfLeg[2 * i]     = st;
    SiteTypeOfLeg[2 * i + 1] = st;
  }
  int* x = new int[NLEG];
  for (int i = 0; i < X.NumberOfBlocks(); i++) {
    XML::Block& B      = X[i];
    const std::string& name = B.getName();
    if (name == "Weight") {
      for (int i = 0; i < NLEG; i++)
        x[i] = B.getInteger(i);
      double w = B.getDouble(NLEG);
      if (w < 0.0 && !isdiagonal(x,NBODY)){
        w *= -1.0;
      }
      Weight(x) = w;
    }
  }
  EBASE = 0.0;
  delete[] x;
}

//======================================================================

int VERTEX::NumberOfValidInitialConfigurations() {
  int c = 0;
  for (int i = 0; i < NICG; i++) {
    int n = ICG(i).NumberOfValidInitialConfigurations();
    c += n;
  }
  return c;
}

//======================================================================

void VERTEX::Grouping() {
  //printf("VERTEX::Grouping> Start ID=%d\n", ID);
  //printf("VERTEX::Grouping> Start.\n");
  //printf("VERTEX::Grouping> Pass 1\n");

  int NICmax = 2 * NBODY * NXMAX + 1;
  checked.init(3, NST, NLEG, NXMAX);
  checked.set_all(0);
  int* x = new int[NLEG];
  //  int x[NLEG];
  NICG = 0;
  if (isWorm()) {
    for (int sti = 0; sti < NST; sti++) {
      if (testForbidden(sti)) continue;
      if (testKink(sti)) continue;
      int inc                        = DIR::UNDEF;
      int xinc                       = STATE::UNDEF;
      InitialConfigurationGroup& icg = *(new InitialConfigurationGroup);
      icg.init(*this, NBODY, sti, inc, xinc);
      add_ICG(icg);
      //printf("VERTEX::Grouping> New initial configuration group. ID= %d\n", NICG);
      NICG++;
    }
  } else {
    for (int sti = 0; sti < NST; sti++) {
      INDX().coord(sti, x);
      if (testForbidden(sti)) continue;
      for (int inc = 0; inc < NLEG; inc++) {
        int xinc_old = x[inc];
        int st       = SiteTypeOfLeg[inc];
        SOURCE& W    = Source[Site[st].TTYPE];
        for (int xinc = 0; xinc < NXMAX; xinc++) {
          //W.V().Weight.index_system().dump();
          double ww = W.Weight(xinc_old, xinc);
          if (ww == 0.0) continue;
          if (checked(sti, inc, xinc) == 1) continue;

          //printf("VERTEX::Grouping> New initial configuration group. ID= %d\n", NICG);
          InitialConfigurationGroup& icg = *(new InitialConfigurationGroup);
          icg.init(*this, NBODY, sti, inc, xinc);
          add_ICG(icg);
          NICG++;
        }
      }
    }
    //printf(" NICG= %d, icg.size()= %d\n", NICG, icg.size());
  }
  delete[] x;
}

//======================================================================

double VERTEX::ComputeEBASE() {
  //
  // Calculate EBASE as the minimum additional term to
  // make probability of offdiagonal bounce of negative Hamiltonian (-H)
  // be zero.
  //
  // - For worm creation/annihilation, avoid direct transition
  //   between offdiagonal states.
  //   In other words, let EBASE be so large that
  //   the probability of a worm annihilation is 1.
  // - For interaction vertex, minimize diagonal weights
  //   as long as no offdiagonal bounce occurs.
  //

  for (int icg = 0; icg < NICG; icg++)
    ICG(icg).ResetWeight();

  double eb;
  if (CATEGORY == VCAT::TERM) {
    eb = 1.0;
  } else if (CATEGORY == VCAT::WORM) {
    eb = -INF;
    for (int icg = 0; icg < NICG; icg++) {
      double e = ICG(icg).sum_of_all_weights();
      if (eb < e) eb = e;
    }
  } else if (CATEGORY == VCAT::INT) {
    eb = -INF;
    for (int icg = 0; icg < NICG; icg++) {
      double e = ICG(icg).ebase();
      if (eb < e) eb = e;
    }
  } else {
    printf("VERTEX::ComputeEBASE> Error.\n");
    exit(0);
  }
  return eb;
}

//======================================================================

void VERTEX::ComputeScatteringProbability() {
  //printf("\nVERTEX::ComputeScatteringProbability> Start.\n");
  NICV = 0;
  for (int icg = 0; icg < NICG; icg++) {
    ICG(icg).ResetWeight();
    ICG(icg).numbering(NICV);
    ICG(icg).ScatteringProbability();
  }
  //printf("\nVERTEX::ComputeScatteringProbability> End.\n");
}

//======================================================================

void VERTEX::dump() {
  printf("VERTEX[%d] cat= %d", ID, CATEGORY);
  printf(" nb= %d", NBODY);
  printf(" nl= %d", NLEG);
  printf(" ncig= %d", NICG);
  printf(" eb= %8.3f", EBASE);
  printf(" st= (");
  for (int i = 0; i < NLEG; i++)
    printf(" %1d", SiteTypeOfLeg(i));
  printf(")\n");
  int* x = new int[NLEG];  //edit sakakura
                           //  int x[NLEG];
  for (int i = 0; i < NST; i++) {
    if ((!testKink(i)) || Weight[i] != 0.0) {
      if (testKink(i)) {
        printf(" kink:yes ");
      } else {
        printf(" kink:no  ");
      }
      if (Weight[i] == 0.0) {
        printf(" w=0:yes ");
      } else {
        printf(" w=0:no  ");
      }
      INDX().coord(i, x);
      printf(" (");
      for (int j = 0; j < NLEG; j++)
        printf(" %1d", x[j]);
      printf(") --> w= %8.3f\n", Weight[i]);
    }
  }
  for (int i = 0; i < NICG; i++) {
    printf("\n");
    ICG(i).dump();
  }
  delete[] x;
}

//======================================================================

void VERTEX::write() {
  fprintf(FALG, "\n");
  fprintf(FALG, "  <Vertex>\n");
  fprintf(FALG, "    <VTYPE> %d </VTYPE>\n", ID);
  fprintf(FALG, "    <VCATEGORY> %d </VCATEGORY>\n", CATEGORY);
  fprintf(FALG, "    <NBODY> %d </NBODY>\n", NBODY);
  int NICW = NumberOfValidInitialConfigurations();
  fprintf(FALG,
          "    <NumberOfInitialConfigurations> %d "
          "</NumberOfInitialConfigurations>\n",
          NICW);
  for (int i = 0; i < NICG; i++)
    ICG(i).write();
  fprintf(FALG, "\n  </Vertex>\n");
}

//======================================================================

bool VERTEX::testKink(int ist) {
  //printf("VERTEX::testKink> D = %d\n", D);
  int* x = new int[NLEG];
  INDX().coord(ist, x);
  bool ans = false;
  for (int i = 0; i < NLEG / 2; i++) {
    if (x[2 * i] != x[2 * i + 1]) ans = true;
  }
  delete[] x;
  return ans;
}

//======================================================================

bool VERTEX::testForbidden(int ist) {
  if (!testKink(ist)) return false;
  if (Weight[ist] > 0.0) return false;
  return true;
}

//######################################################################

void InitialConfigurationGroup::init(VERTEX& v, int nbody, int sti, int inc,
                                     int xinc) {
  // handling a group of initial configurations
  // that scatters into each other

  //printf("InitialConfigurationGroup::init> Start. icg=%d\n", ID);

  /*
printf("nbody= %d, nicmax= %d, sti= %d, inc= %d, xinc= %d\n",
nbody, NICmax, sti, inc, xinc);
  */

  _V    = &v;
  NBODY = nbody;
  NLEG  = 2 * nbody;

  //int x[NLEG];
  //int y[NLEG];
  int* x            = new int[NLEG];  //edit sakakura
  int* y            = new int[NLEG];
  IndexSystem& INDX = V().INDX();
  INDX.coord(sti, x);
  if (inc != DIR::UNDEF) x[inc] = xinc;
  // x[l] is the local state on the l-th leg
  // just after the entrance of the incoming
  // worm head (before its outgoing).

  /*
if (MON) {
printf("InitialConfigurationGroup::init> ");
printf("Grouping the configs related to ... \n");
printf("  ...  nb= %d, sti= %d (", nbody, sti);
for (int i=0; i<NLEG; i++) printf(" %1d", x[i]);
printf("), hti= %d, inc= %d\n", hti, inc);
}
  */

  int sitei = inc / 2;
  int diri  = inc % 2;


  NIC = 0;
  if (inc == DIR::UNDEF) NIC = 1;  // generate worm
  for (int out = 0; out < NLEG; out++) {
    for (int xout = 0; xout < NXMAX; xout++) {
      // xout is the new local state on the outgoing leg
      for (int d = 0; d < NLEG; d++)
        y[d] = x[d];
      y[out]  = xout;
      int stf = INDX(y);
      if (stf == STATE::UNDEF) continue;
      if (V().testForbidden(stf)) continue;
      NIC++;
    }
  }

  U.init("U", 1, NIC);
  U.set_all(0.0);
  //  W.init("W",2,NIC,NIC);
  //  W.set_all(0.0);
  //  P.init("P",2,NIC,NIC);
  //  P.set_all(0.0);
  IC.init(1, NIC);

  int ic = 0;
  if (inc == DIR::UNDEF) {  // generate worm
    IC[ic].init(V(), sti, inc, xinc, NIC);
    ic++;
  }
  for (int out = 0; out < NLEG; out++) {
    for (int xout = 0; xout < NXMAX; xout++) {
      // xout is the new local state on the outgoing leg
      for (int d = 0; d < NLEG; d++)
        y[d] = x[d];
      y[out] = xout;
      /*
printf(" (");
for (int i=0; i<INDX.dimension(); i++) printf(" %d", y[i]);
printf(")\n");
      */
      int stf = INDX(y);
      if (stf == STATE::UNDEF) continue;
      if (V().testForbidden(stf)) continue;
      IC[ic].init(V(), stf, out, x[out], NIC);
      V().checked(stf, out, x[out]) = 1;
      ic++;
    }
  }
  delete[] x;
  delete[] y;
  //if (MON) printf("InitialConfigurationGroup::init> End.\n");
}

//======================================================================

int InitialConfigurationGroup::NumberOfValidInitialConfigurations() {
  int c = 0;
  for (int i = 0; i < NIC; i++) {
    if (IC[i].isValid()) c++;
  }
  return c;
}

//======================================================================

void InitialConfigurationGroup::ResetWeight() {
  for (int i = 0; i < NIC; i++) {
    U[i] = IC[i].weight();
    /*
    for (int j=0; j<NIC; j++) {
      W(i,j) = 0.0;
      P(i,j) = 0.0;
    }
    */
  }
}

//======================================================================

int InitialConfigurationGroup::number_of_diagonal_states() {
  int n = 0;
  for (int i = 0; i < NIC; i++) {
    if (!IC[i].isKink()) n++;
  }
  return n;
}

//======================================================================

int InitialConfigurationGroup::number_of_offdiagonal_states() {
  return NIC - number_of_diagonal_states();
}

//======================================================================

double InitialConfigurationGroup::minimum_diagonal_weight() {
  double dmin = INF;
  for (int i = 0; i < NIC; i++) {
    if (!IC[i].isKink()) {
      if (U[i] < dmin) dmin = U[i];
    }
  }
  return dmin;
}

//======================================================================

double InitialConfigurationGroup::maximum_diagonal_weight() {
  double dmax = -INF;
  for (int i = 0; i < NIC; i++) {
    if (!IC[i].isKink()) {
      if (U[i] > dmax) dmax = U[i];
    }
  }
  return dmax;
}

//======================================================================

double InitialConfigurationGroup::maximum_offdiagonal_weight() {
  double omax = -INF;
  for (int i = 0; i < NIC; i++) {
    if (IC[i].isKink()) {
      if (U[i] > omax) omax = U[i];
    }
  }
  return omax;
}

//======================================================================

double InitialConfigurationGroup::sum_of_all_weights() {
  double sum = 0.0;
  for (int i = 0; i < NIC; i++)
    sum += U[i];
  return sum;
}

//======================================================================

double max(double& d0, double& d1, double& d2) {
  double d = d0;
  if (d1 > d) d = d1;
  if (d2 > d) d = d2;
  return d;
}

//======================================================================

double InitialConfigurationGroup::ebase() {
  //if ( MON ) printf("InitialConfigurationGroup::ebase> Start.\n");

  double eb, eb0, eb1, eb2;

  int nd = number_of_diagonal_states();
  int no = NIC - nd;
  if (nd == 0) {
    eb = -INF;
  } else {
    double dmax = maximum_diagonal_weight();
    double dmin = minimum_diagonal_weight();
    double omax = maximum_offdiagonal_weight();
    double sum  = sum_of_all_weights();
    // eb0 ... minimum EBASE to avoid negative diagonal elements
    // eb1 ... minimum EBASE to avoid offdiagonal bounce
    // eb2 ... minimum EBASE to avoid direct transition between offdiagonal states
    eb0 = -dmin;
    eb1 = (2.0 * omax - sum) / (double)nd;
    //  eb2 = -INF;
    //  eb2 = sum - 2.0 * dmax;
    if (eb0 >= eb1) {
      eb = eb0;
    } else {
      eb = eb1;
    }

    // Above we calculate EBASE from Hamilonian weights producted by worm weights
    double ww = 0.0;
    for (int i = 0; i < NIC; i++) {
      if (!IC[i].isKink()) {
        double wt = IC[i].worm_weight();
        if (fabs(ww) > EPS && fabs(wt) > EPS && wt != ww) {
          printf("InitialConfigurationGroup::ebase> Error.\n");
          printf("  The worm weights of non-kinks are not equal.\n");
          dump();
          exit(0);
        }
        ww = IC[i].worm_weight();
      }
    }
    eb /= ww;
  }

  //if ( MON ) printf("InitialConfigurationGroup::ebase> End.\n");

  return eb;
}

//======================================================================

void InitialConfigurationGroup::add_to_diagonal_element(double E) {
  for (int i = 0; i < NIC; i++) {
    if (!IC[i].isKink()) U[i] += E;
  }
}

//======================================================================

void InitialConfigurationGroup::numbering(int& ic) {
  for (int i = 0; i < NIC; i++) {
    if (U[i] > -EPS && U[i] < EPS) {
      IC[i].id() = ICONF::UNDEF;
    } else {
      IC[i].id() = ic;
      ic++;
    }
  }
}

//======================================================================

void InitialConfigurationGroup::ScatteringProbability() {
  //printf("\nInitialConfigurationGroup::ScatteringProbability> Start. ICG= %d\n", ID);
  //if (MON) dump();

  // NIC : # of initial state
  // U   : weights of initial states
  // UP  : weights of initial states (sorted)
  Array<int> PERM("PERM");
  PERM.init(1, NIC);
  bubble_sort(NIC, U, PERM);

  Array<double> UP("UP");
  UP.init(1, NIC);
  for (int i = 0; i < NIC; i++) {
    UP[i] = U[PERM[i]];
  }

  Array<double> P("P");
  Array<double> W("W");
  Array<double> WP("WP");
  P.init(2, NIC, NIC);
  W.init(2, NIC, NIC);
  WP.init(2, NIC, NIC);

  SolveWeightEquation(NIC, UP, WP);

  /*
  for (int i=0; i<NIC; i++) { 
    for (int j=0; j<NIC; j++) W(PERM[i],PERM[j]) = WP(i,j); 
  }
*/

  IndexSystem& INDX = V().INDX();
  for (int i = 0; i < NIC; i++) {
    int I = PERM[i];
    if (-EPS < U[I] && U[I] < EPS) continue;
    int nc = 0;
    for (int j = 0; j < NIC; j++) {
      int J = PERM[j];
      if (-EPS < U[J] && U[J] < EPS) continue;
      W(I, J) = WP(i, j);
      P(I, J) = W(I, J) / U[I];
      if (P(I, J) > EPS) {
        double p = P(I, J);
        int stf  = IC[J].STI;
        int out  = IC[J].INC;
        int xout = STATE::UNDEF;
        if (out != DIR::UNDEF) { xout = INDX.coord(stf, out); }
        IC[I].FinalDirection[nc]        = out;
        IC[I].FinalState[nc]            = xout;
        IC[I].ScatteringProbability[nc] = p;
        nc++;
      }
    }
    IC[I].NCH = nc;
  }

  //if (MON) printf("  ==>\n");
}

//======================================================================

void InitialConfigurationGroup::dump() {
  for (int i = 0; i < NIC; i++) {
    IC[i].dump();
  }
  for (int i = 0; i < NIC; i++) {
    printf("(%d)", i);
    printf(" ID= %2d", IC[i].id());
    printf(" : U= %4.1f", U[i]);
    /*
    printf(" , W= ");
    for (int j=0; j<NIC; j++) {
      printf(" %4.1f", W(i,j));
    }
    printf(" , P= ");
    for (int j=0; j<NIC; j++) {
      printf(" %6.3f", P(i,j));
    }
    printf("\n");
    */
  }
}

//======================================================================

void InitialConfigurationGroup::write() {
  for (int i = 0; i < NIC; i++) {
    IC[i].write();
  }
}

//######################################################################

void InitialConfiguration::init(VERTEX& v, int sti, int inc, int xinc,
                                int NICmax) {
  // define an initial configuration for which
  // the initial vertex state is "sti"
  // the incoming worm head is coming on the "inc"-th leg
  // the new state on the leg after the passage of the worm head is "xinc"

  setV(v);
  STI                   = sti;
  INC                   = inc;
  XINC                  = xinc;
  NLEG                  = V().NLEG;
  NBODY                 = 2 * NLEG;
  State                 = new int[NLEG];
  ScatteringProbability = new double[NICmax];
  FinalState            = new int[NICmax];
  FinalDirection        = new int[NICmax];
  V().INDX().coord(STI, State);
  NCH = 0;
}

//======================================================================

bool InitialConfiguration::isValid() {
  if (INC == DIR::UNDEF) return false;
  if (fabs(weight()) < EPS) return false;
  return true;
}

//======================================================================

bool InitialConfiguration::isKink() { return V().testKink(STI); }

//======================================================================

double InitialConfiguration::weight() {
  double w = vertex_weight() * worm_weight();
  return w;
}

//======================================================================

double InitialConfiguration::vertex_weight() {
  double ws = V().Weight[STI];
  if (!isKink()) ws += V().EBASE;
  return ws;
}

//======================================================================

double InitialConfiguration::worm_weight() {
  double ww;
  if (INC == DIR::UNDEF) {
    ww = 1.0;
  } else {
    int stype = V().SiteTypeOfLeg[INC];
    int ttype = Site[stype].TTYPE;
    SOURCE& T = Source[ttype];
    ww        = T.Weight(XINC, State[INC]);
  }
  return ww;
}

//======================================================================

void InitialConfiguration::dump() {
  double wv, ww;
  if (INC == DIR::UNDEF) {
    wv = 1.0;
    ww = 1.0;
  } else {
    wv = vertex_weight();
    ww = worm_weight();
  }
  printf(" IC(%3d) : nleg= %2d, sti= %4d (", id(), NLEG, STI);
  for (int i = 0; i < NLEG; i++) {
    printf(" %1d", State[i]);
  }
  printf("), inc= %2d, xinc= %2d ", INC, XINC);
  printf("--> wv= %8.3f, ww= %8.3f, w= %8.3f\n", wv, ww, wv * ww);
  if (NCH != 0) {
    for (int i = 0; i < NCH; i++) {
      printf("   Channel[%d] : out= %d, xout= %d, prob= %8.3f\n", i,
             FinalDirection[i], FinalState[i], ScatteringProbability[i]);
    }
  }
}

//======================================================================

void InitialConfiguration::write() {
  if (!isValid()) return;
  fprintf(FALG, "\n");
  fprintf(FALG, "    <InitialConfiguration>\n");
  fprintf(FALG, "      <State> ");
  for (int i = 0; i < NLEG; i++)
    fprintf(FALG, " %d", State[i]);
  fprintf(FALG, " </State>\n");
  fprintf(FALG, "      <IncomingDirection> %d </IncomingDirection>\n", INC);
  fprintf(FALG, "      <NewState> %d </NewState>\n", XINC);
  fprintf(FALG, "      <NumberOfChannels> %d </NumberOfChannels>\n", NCH);
  for (int ch = 0; ch < NCH; ch++) {
    int out  = FinalDirection[ch];
    int xout = FinalState[ch];
    double p = ScatteringProbability[ch];
    fprintf(FALG, "      <Channel> %4d %4d %24.16lf </Channel>\n", out, xout, p);
  }
  fprintf(FALG, "    </InitialConfiguration>\n");
}

//######################################################################

void QUANTITY::load(XML::Block& X) {
  ID   = X["QTYPE"].getInteger();
  NAME = X["Name"].getString();
  Value.init(2, NSTYPE, NXMAX);
  Value.set_all(0.0);
  isDefined.init(2, NSTYPE, NXMAX);
  isDefined.set_all(false);
  for (int i = 0; i < X.NumberOfBlocks(); i++) {
    XML::Block& B = X[i];
    if (B.getName() == "Value") {
      int st           = B.getInteger(0);
      int x            = B.getInteger(1);
      double v         = B.getDouble(2);
      isDefined(st, x) = true;
      Value(st, x)     = v;
    }
  }
}

//======================================================================

void QUANTITY::write() {
  fprintf(FALG, "\n");
  fprintf(FALG, "  <Quantity>\n");
  fprintf(FALG, "    <QTYPE> %d </QTYPE>\n", getID());
  fprintf(FALG, "    <Name> %s </Name>\n", getName().c_str());
  for (int st = 0; st < NSTYPE; st++) {
    for (int x = 0; x < NXMAX; x++) {
      if (isDefined(st, x)) {
        fprintf(FALG, "    <Value> %d %d %24.16lf </Value>\n", st, x,
                Value(st, x));
      }
    }
  }
  fprintf(FALG, "  </Quantity>\n");
}

//######################################################################

//######################################################################

void SolveWeightEquation(int N, Array<double>& V, Array<double>& W) {
  /*
  for (int i=0; i<N; i++) {
    printf("V[%d] = %8.3f\n", i, V[i]);
  }
*/

  W.set_all(0.0);

  int p, q;
  double V_first;
  double V_second;
  double V_third;
  double V_first_new;
  double V_second_new;
  int N_first;   // the number of the largest elements
  int N_second;  // the number of the second largest

  // First three (unique) weights and indices
  while (V[1] > EPS) {
    V_first = V[0];
    for (p = 0; p < N; p++)
      if (V[p] != V_first) break;
    N_first = p;
    if (p == N) {
      V_second = 0.0;
      V_third  = 0.0;
      N_second = 0;
    } else {
      V_second = V[p];
      for (q = p; q < N; q++)
        if (V[q] != V_second) break;
      if (q == N) {
        V_third = 0.0;
      } else {
        V_third = V[q];
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
      V[0] = V_first_new;
      for (int i = 1; i < 1 + N_second; i++) {
        W(0, i) += dw2;
        W(i, 0) += dw2;
        V[i] = V_second_new;
      }
    } else {
      // When the maximum weight state is degenerated
      // introduce a transition between these states
      // and reduce weights of these states to the weight of the second largest.
      dw1 = (V_first - V_second) / (double)(N_first - 1);
      for (int i = 0; i < N_first; i++) {
        V[i] = V_second;
        for (int j = 0; j < N_first; j++) {
          if (i == j) continue;
          W(i, j) += dw1;
        }
      }
    }
  }
  if (V[0] > 0.0) {
    W(0, 0) += V[0];
    V[0] = 0.0;
  }
}

//======================================================================

void bubble_sort(int N, Array<double>& V, Array<int>& I) {
  for (int i = 0; i < N; i++)
    I[i] = i;
  for (int i = 0; i < N - 1; i++) {
    for (int j = i + 1; j < N; j++) {
      if (V[I[i]] < V[I[j]]) {
        int ii = I[i];
        I[i]   = I[j];
        I[j]   = ii;
      }
    }
  }
}
