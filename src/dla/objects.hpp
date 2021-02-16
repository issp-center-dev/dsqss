#ifndef SRC_DLA_OBJECTS_HPP_
#define SRC_DLA_OBJECTS_HPP_

//######################################################################

#include <cstdio>

#include "algorithm.hpp"
#include "name.h"
#include "xml.h"

class BareSegment;
class BareVertex;
class Segment;
class Vertex;
class Worm;
class Site;
class Interaction;
class UniformInterval;
class RegisteredVertexInfo;

//######################################################################
//####  Object Declarations
//######################################################################

class BareSegment {
 private:
  static int lastID;
  int IDX;
  Vertex* _v[2];
  int val;  // the local state (0, 1, 2, ... , NX-1)
  Site* ONSITE;

 public:
  void init() {
    val = 0;
    _v[0] = 0;
    _v[1] = 0;
    ONSITE = 0;
  }

  void set(int x, Vertex& v0, Vertex& v1) {
    val = x;
    _v[0] = &v0;
    _v[1] = &v1;
  }

  BareSegment() : ONSITE(0) {
    lastID++;
    IDX = lastID;
    init();
  }

  ~BareSegment() {
    //    printf("*** Destroying BareSegment\n");
  }

  int id() const { return IDX; }
  Vertex& V(int i) { return *_v[i]; }
  Vertex& bottom() { return *_v[0]; }
  Vertex& top() { return *_v[1]; }
  void setV(int i, Vertex& v) { _v[i] = &v; }
  void setBottom(Vertex& v) { _v[0] = &v; }
  void setTop(Vertex& v) { _v[1] = &v; }
  int X() { return val; }
  void setX(int x) { val = x; }
  double topTime();
  double bottomTime();
  double length();
  void dump();
  bool check();

  void dump_id() { std::cout << IDX << std::endl; }

  Site* getONSITE() { return ONSITE; }
  void setONSITE(Site* on_site) { ONSITE = on_site; }
};

int BareSegment::lastID = 0;

//######################################################################

class Segment : public Linked<BareSegment> {
 public:
  Segment& cut(Vertex& V, int side);
  void erase();
  void absorbNext();
  Segment& prev() { return (Segment&)Linked<BareSegment>::prev(); }
  Segment& next() { return (Segment&)Linked<BareSegment>::next(); }
  bool operator==(Segment& s) { return this == &s; }
};

//######################################################################

class BareVertex {
 private:
  static int lastID;
  int ID;
  VertexProperty* _VP;
  double TIME;
  Array<Segment*> _s;
  Interaction* ONINTERACTION;

 public:
  void init() {
    _VP = 0;
    TIME = -1.0;
    _s.init();
  }

  void init_WORM();

  void init_TERM(Segment& S);

  // Used when PlaceWorm
  void init(double t, VertexProperty& VP) {
    TIME = t;
    _s.init(1, VP.NLEG);  // koko !!!
    _s.set_all(0);
    _VP = &VP;
  }

  void init(Interaction* O_I, double t, VertexProperty& VP) {
    TIME = t;
    _s.init(1, VP.NLEG);  // koko !!!
    _s.set_all(0);
    _VP = &VP;
    ONINTERACTION = O_I;
  }
  BareVertex() {
    _s.setLabel("BareVertex::_s");
    lastID++;
    ID = lastID;
    ONINTERACTION = 0;
    init();
  }

  ~BareVertex() {
    //    printf("*** Destroying BareVertex (%d)\n", this);
  }

  int NBODY() const {
    if (isTerminal()) return 1;
    return (*_VP).NBODY;
  }

  int NLEG() const { return 2 * NBODY(); }

  bool isTerminal() const;
  bool isTail() const;

  int id() const { return ID; }

  int size() const { return 2 * NBODY(); }

  bool isKink();

  Segment& S(int i) {
#ifdef DEB
    if (_s[i] == 0) {
      printf("BareVertex::S> Error with i= %d\n", i);
      printf("  The segment pointer is not set ( _s[i]=0 ).\n");
    }
#endif
    return *_s[i];
  }

  void setS(int dir, Segment& s) { _s[dir] = &s; }

  int X(int i);

  double time() const { return TIME; }

  void setTime(double t) { TIME = t; }

  int which(Segment& s);

  void replace(Segment& Sold, Segment& Snew);

  bool check();

  void setProperty(VertexProperty& P) { _VP = &P; }

  int type() {
    if (_VP == 0) {
      return VTYPE::UNDEF;
    } else {
      return property().id();
    }
  }

  int category() {
    if (_VP == 0) {
      return VCAT::TERM;
    } else {
      return property().VCAT;
    }
  }

  VertexProperty& property() {
#ifdef DEB
    if (_VP == 0) {
      printf("BareVertex::property> Error.\n");
      printf("  Vertex property has not been set.\n");
      exit(0);
    }
#endif
    return *_VP;
  }

  VertexInitialConfiguration& getInitialConfiguration(int inc, int xinc);

  void dump();
  Interaction* getONINTERACTION() { return ONINTERACTION; }
  void setONINTERACTION(Interaction* on_interaction) {
    ONINTERACTION = on_interaction;
  }
};

int BareVertex::lastID = 0;

//######################################################################

class Vertex : public Linked<BareVertex> {
 public:
  void reconnect();  // reconnect the segments attached to the vertex
  void erase();      // remove from the linked list and push back to the pool
  Vertex& prev() { return (Vertex&)Linked<BareVertex>::prev(); }
  Vertex& next() { return (Vertex&)Linked<BareVertex>::next(); }
  bool operator==(Vertex& v) { return this == &v; }
};

//######################################################################

class Worm {
 private:
  int x_behind;    // the local state just after a passage of the worm head
  Vertex* _vorg;   // the vertex sitting at the creation point
  Vertex* _vcur;   // the current vertex
  Segment* _scur;  // the current segment

 public:
  Worm();

  ~Worm();

  Vertex& getNextVertex() {
    Vertex* _v = &(_scur->bottom());
    if (_v == _vcur) _v = &(_scur->top());
    return *_v;
  }

  Vertex& origin() { return *_vorg; }

  Vertex& getCurrentVertex() { return *_vcur; }

  void setCurrentVertex(Vertex& v) { _vcur = &v; }

  Segment& getCurrentSegment() { return *_scur; }

  void setCurrentSegment(Segment& s) { _scur = &s; }

  void setXBEHIND() { x_behind = STATE::UNDEF; }

  void setXBEHIND(int x) { x_behind = x; }

  int getXBEHIND() {
#ifdef DEB
    if (x_behind == STATE::UNDEF) {
      printf("Worm::getXBEHIND> Error.\n  x_behind = %d\n", x_behind);
      exit(0);
    }
#endif
    return x_behind;
  }

  bool atOrigin() { return (_vcur == _vorg); }

  bool getUORD() {
    return (_vcur == &((*_scur).top()));
  }  // up -> false 0, down -> true 1
  void getV() {
    printf("top,bottom = %d, %d ", (*_scur).top().id(), (*_scur).bottom().id());
  }

  void remove();

  void dump();
};

//######################################################################

class Site : public Ring<Segment> {
 private:
  static int lastID;
  int ID;
  SiteProperty* _SP;
  Vertex* _vterm;

  int NCI;  // the number of connected interactions
  Interaction**
      ConnectedInteraction;  // Interaction* ConnectedInteraction [ NCI ];

 public:
  void setNCI(int ni) {
    NCI = ni;
    ConnectedInteraction = new Interaction*[ni];
  }
  int getNCI() { return NCI; }
  void setCI(int nth, Interaction* CI) { ConnectedInteraction[nth] = CI; }
  Interaction** getCI() { return ConnectedInteraction; }

  void init(SiteProperty& SP) { _SP = &SP; }

  Site();

  ~Site();

  int id() { return ID; }

  int type() {
    if (_SP == 0) return STYPE::UNDEF;
    return (*_SP).STYPE;
  }

  void setProperty(SiteProperty& sp) { _SP = &sp; }

  SiteProperty& Property() { return *_SP; }

  double getBeta() const { return _vterm->time(); }
  void setBeta(double b) { _vterm->setTime(b); }

  Segment& findS(double t);

  void merge(Segment& S1);

  void dump() {
    //    printf("Site::dump> Start.\n");
    printf("Site(%2d) type= %d | ", id(), type());
    Ring<Segment>::dump();
    printf("\n");
  }

  void idclear() { lastID = 0; }
  // void idclear() { cout << "dddd " << endl; };

  Vertex& getVterm() { return (*_vterm); }
};

int Site::lastID = 0;

//######################################################################

class Interaction : public Ring<Vertex> {
 private:
  static int lastID;
  int ID;
  InteractionProperty* _IP;
  Site** _s;

 public:
  void init() {
    _IP = 0;
    if (_s != 0) {
      delete[] _s;
    }
  }

  void init(InteractionProperty& IP) {
    init();
    _IP = &IP;
    _s = new Site*[(*_IP).NBODY];
  }

  Interaction();

  ~Interaction();

  int id() { return ID; }

  int NBODY() { return (*_IP).NBODY; }

  int type() { return (*_IP).ITYPE; }

  void setProperty(InteractionProperty& ip) { _IP = &ip; }

  InteractionProperty& property() {
#ifdef DEB
    if (_IP == 0) {
      printf("InteractinProperty::property> Error.\n");
      printf("  The property not defined.\n");
      exit(0);
    }
#endif
    return *_IP;
  }

  Site& site(int i) { return *(_s[i]); }

  void setSite(int i, Site& s) {
#ifdef DEB
    if (NBODY() == 0) {
      printf("Interaction::setSite> Error.\n");
      printf("  Initialize before using!\n");
      exit(0);
    }
#endif
    _s[i] = &s;
  }

  void dump() {
    int* sid = new int[NBODY()];
    for (int i = 0; i < NBODY(); i++) {
      sid[i] = STYPE::UNDEF;
      if (_s[i] != 0) sid[i] = site(i).id();
    }
    printf("Interaction(%2d) = (", id());
    for (int i = 0; i < NBODY(); i++) printf(" %2d", sid[i]);
    printf(" ), type= %2d | ", type());
    Ring<Vertex>::dump();
    delete[] sid;
  }

  void idclear() { lastID = 0; }
};

int Interaction::lastID = 0;

//######################################################################

Pool<Vertex> TheVertexPool;
Pool<Segment> TheSegmentPool;

Interaction TerminalOfWorldline;
Interaction TailOfWorm;

//######################################################################
//###########  Member Functions  #######################################
//######################################################################

//######################################################################

inline double BareSegment::topTime() { return top().time(); }

//======================================================================

inline double BareSegment::bottomTime() {
  double t = bottom().time();
  if (bottom().isTerminal()) t = 0.0;
  return t;
}

//======================================================================

inline double BareSegment::length() { return topTime() - bottomTime(); }

//======================================================================

inline void BareSegment::dump() {
  //  printf("\nBareSegment::dump> Start.\n");
  fprintf(FERR, " Segment [%3d] : ", id());
  double bt = 0.0;
  int bid = -1;
  if (_v[0] != 0) {
    bt = bottomTime();
    bid = bottom().id();
  }
  double tt = 0.0;
  int tid = -1;
  if (_v[1] != 0) {
    tt = topTime();
    tid = top().id();
  }
  if (bt >= tt) bt = 0.0;
  fprintf(FERR, " <%3d>(%8.3f)---<%3d>(%8.3f)", bid, bt, tid, tt);
  fprintf(FERR, " X=%d", X());
  fprintf(FERR, "\n");
}

//======================================================================

inline bool BareSegment::check() {
  bool ERROR = false;
  return ERROR;
}

//######################################################################

inline Segment& Segment::cut(Vertex& V, int side) {
  //
  // before:   --(V0)---------[S0]----------(V1)---> tau
  //
  // after:    --<V0>---[S0]---<V>---[S1]---<V1>---> tau
  //
  int x = X();  // int X() { return val; };

  Vertex& V0 = bottom();  // Vertex& bottom() { return *_v[0]; }; Vertex* _v[2];
  Vertex& V1 = top();     // Vertex& top()    { return *_v[1]; };
  Segment& S0 = *this;
  Segment& S1 = TheSegmentPool.pop();
  insert_after(S1);
  S0.set(x, V0, V);
  S1.set(x, V, V1);
  S1.setONSITE(S0.getONSITE());
  int leg_below = 2 * side;
  int leg_above = leg_below + 1;
  V.setS(leg_below, S0);
  V.setS(leg_above, S1);

  // Since S1 is always below we need investigate only even V1
  for (int l = 0; l < V1.NLEG(); l += 2) {
    if (V1.S(l) == S0) {
      // printf(" gotcha!\n");
      V1.setS(l, S1);
    }
  }
  return S1;
}

//======================================================================

inline void Segment::erase() {
  Linked<BareSegment>::remove();
  TheSegmentPool.push(*this);
}

//======================================================================

inline void Segment::absorbNext() {
  Segment& S0 = *this;
  Segment& S1 = S0.next();
  Vertex& V1 = S1.top();
  S0.setTop(V1);
  V1.replace(S1, S0);
  S1.erase();
}

//######################################################################

inline int BareVertex::X(int i) { return S(i).X(); }

//======================================================================
inline void BareVertex::init_WORM() {
  init();
  _s.init(1, 2);
  _s.set_all(0);
  ONINTERACTION = &TailOfWorm;
}
//======================================================================
inline void BareVertex::init_TERM(
    Segment& S) {  // initializer for a terminal vertex//katou
  init();
  _s.init(1, 2);
  _s.set_all(0);
  _s[0] = &S;
  _s[1] = &S;
  ONINTERACTION = &TerminalOfWorldline;
}
//======================================================================
inline bool BareVertex::isTerminal() const {
  return (ONINTERACTION == &TerminalOfWorldline);
}
//======================================================================
inline bool BareVertex::isTail() const {
  return (ONINTERACTION == &TailOfWorm);
}
//======================================================================

inline int BareVertex::which(Segment& s) {
  int i;
  for (i = 0; i < 4; i++) {
    if (_s[i] == &s) return i;
  }
  printf("BareVertex::which> ERROR. No such segment.\n");
  printf("tried to find the segment\n");
  s.dump();
  printf("in the vertex\n");
  dump();
  exit(0);
}

//======================================================================

void BareVertex::replace(Segment& Sold, Segment& Snew) {
  setS(which(Sold), Snew);
}

//======================================================================

inline bool BareVertex::isKink() {
  for (int i = 0; i < NLEG(); i += 2) {
    if (X(i) != X(i + 1)) return true;
  }
  return false;
}

//======================================================================

inline bool BareVertex::check() {
  bool ERROR = false;
  for (int i = 0; i < size(); i += 2) {
    Segment& S0 = S(i);
    Segment& S1 = S(i + 1);
    if (&S0.next() != &S1) ERROR = true;
    if (&S1.prev() != &S0) ERROR = true;
  }
  if (ERROR) {
    printf("Vertex::check> ERROR.\n");
    dump();
  }
  return ERROR;
}

//======================================================================

inline VertexInitialConfiguration& BareVertex::getInitialConfiguration(
    int inc, int xinc) {
  VertexProperty& P = property();
  //  int NLEG = 2 * P.NBODY;
  int NDIM = P.NLEG + 2;
  int* x = new int[NDIM];  // edit sakakura
                           //  int x[NDIM];
  for (int i = 0; i < P.NLEG; i++) x[i] = X(i);
  int st = P.StateCode(x);

#ifdef DEB
  if (st == STATE::UNDEF) {
    printf("BareVertex::getInitialConfiguration> Error.\n");
    printf("  An forbidden state has been encountered.\n");
    printf(" x= (");
    for (int i = 0; i < P.NLEG; i++) printf(" %d", x[i]);
    printf(") \n");
    dump();
    P.dump();
    exit(0);
  }
#endif
  delete[] x;
  return P.getIC(st, inc, xinc);
}

//======================================================================

void BareVertex::dump() {
  int vt = type();
  int vc = category();
  //  int n = size();
  //  double t = time();

  fprintf(FERR, "    <%2d> ... vt=(%1d,%2d), nl=%1d, tau=%11.6f ", id(), vc, vt,
          size(), time());
  if (_s.size() != 0) {
    for (int i = 0; i < size(); i++) {
      int id = -1;
      if (_s[i] != 0) id = _s[i]->id();
      fprintf(FERR, "[%2d]", id);
    }
    fprintf(FERR, " X= (");
    for (int i = 0; i < size(); i++) {
      int x = -1;
      if (_s[i] != 0) x = _s[i]->X();
      fprintf(FERR, " %d", x);
    }
    fprintf(FERR, ")");
  } else {
    fprintf(FERR, " segments not defined.");
  }
  fprintf(FERR, "\n");
}

//######################################################################

inline void Vertex::reconnect() {
  for (int l = 0; l < size(); l += 2) S(l).absorbNext();
}

//======================================================================

inline void Vertex::erase() {
  reconnect();
  // if (ALERT) {
  //  printf("\nVertex::erase> ### Erasing\n");
  //  dump();
  //}
  Linked<BareVertex>::remove();
  // if (ALERT) {
  //  printf("\nVertex::erase> ### Done\n");
  //  dump();
  //}
  TheVertexPool.push(*this);
}

//######################################################################

inline Worm::Worm() {
  Vertex& V = TheVertexPool.pop();
  V.init_WORM();  // initialization is the same as TERM, since Worm is a two leg
                  // vertex
  _vorg = &V;
  _vcur = 0;
  _scur = 0;
}

//======================================================================

inline Worm::~Worm() {
  TheVertexPool.push(*_vorg);
  _vorg = 0;
}

//======================================================================

inline void Worm::remove() {
  Vertex& v = getCurrentVertex();
  v.reconnect();
}

//======================================================================

inline void Worm::dump() {
  fprintf(FERR, "\n");
  fprintf(FERR, "Worm:\n");
  fprintf(FERR, "  x_behind = %d\n", getXBEHIND());
  if (_vorg == 0) {
    fprintf(FERR, "  _vorg (the worm tail) is not defined.\n");
  } else {
    fprintf(FERR, "  Tail:\n");
    _vorg->dump();  // barevertex.dump()
  }
  if (_vcur == 0) {
    fprintf(FERR, "  _vcur (the current vertex) is not defined.\n");
  } else {
    fprintf(FERR, "  The last visited vertex:\n");
    _vcur->dump();
  }
  if (_scur == 0) {
    fprintf(FERR, "  _scur (the current segment) is not defined.\n");
  } else {
    fprintf(FERR, "  The segment ahead of the head:\n");
    _scur->dump();
  }
}

//######################################################################

inline Site::Site() {
  // printf("Site::Site> Start.\n");
  lastID++;
  ID = lastID;
  _SP = 0;
  Vertex& v = TheVertexPool.pop();
  Segment& s = TheSegmentPool.pop();
  v.BareVertex::init_TERM(s);
  s.set(0, v, v);
  s.setONSITE(this);
  _vterm = &v;
  add_head(s);
}

//======================================================================

inline Site::~Site() {
  /*
  for(int iCI = 0; iCI<NCI; ++iCI){
    (*ConnectedInteraction[iCI]).dump();
  }
  */
  delete[] ConnectedInteraction;
  while (!empty()) {
    Segment& S = first();
    S.remove();
    TheSegmentPool.push(S);
  }
  TheVertexPool.push(*_vterm);
}

//======================================================================

inline Segment& Site::findS(double t) {
  iterator p(*this);
  while ((++p)->topTime() < t) {
  }

  return *p;
}

//######################################################################

inline Interaction::Interaction() {
  lastID++;
  ID = lastID;
  _IP = 0;
  _s = 0;
}

//======================================================================

inline Interaction::~Interaction() {
  //  printf("*** Destroying Interaction\n");
  while (!empty()) {
    Vertex& V = first();
    V.remove();
    TheVertexPool.push(V);
  }
  if (_s != 0) delete[] _s;
}

// ##########################katou#######################################
class UniformInterval {
 public:
  Interaction* I_n;
  VertexInitialConfiguration* VIC;  //
  VertexProperty* VP;

  // Segment* n_S[nbody]
  Segment** n_S;
  // int x[nbody]
  int* x;
  int inc;  //
  int xinc;
  int nbody;
  bool DefinedVIC;  // Defined StateCode yes no
  void init(Interaction* Ia, int xbehind) {
    I_n = Ia;
    nbody = (*I_n).NBODY();
    VP = &((*I_n).property().getVertexProperty());

    n_S = new Segment*[nbody];
    x = new int[nbody];
    xinc = xbehind;
    DefinedVIC = false;
  }
  void setVIC() {
    int st = (*VP).SCNK(x);
    if (st < 0) {
      VIC = 0;
      DefinedVIC = false;
    } else {
      VIC = &((*VP).getIC(st, inc, xinc));
      DefinedVIC = true;
    }
  }
  void setx() {
    for (int i_body = 0; i_body < nbody; ++i_body) {
      x[i_body] = (*n_S[i_body]).X();
    }
  }
  void setx(int i_body) { x[i_body] = (*n_S[i_body]).X(); }
  UniformInterval() : I_n(0), VIC(0), VP(0), n_S(0), x(0) {}
  ~UniformInterval() {
    delete[] n_S;
    delete[] x;
  }
};
//######################################################################
class RegisteredVertexInfo {
 public:
  Vertex* V_x;
  int i_UI;
  int i_body;
  double V_time;
  void setRVI(Vertex* Vx, int iUI, int ibody, double Vtime) {
    V_x = Vx;
    i_UI = iUI;
    i_body = ibody;
    V_time = Vtime;
  }
  RegisteredVertexInfo() : V_x(0) {}
  void init(){}
};
inline bool operator<(const RegisteredVertexInfo& obj0,
                      const RegisteredVertexInfo& obj) {
  return obj0.V_time < obj.V_time;
}
inline bool operator>(const RegisteredVertexInfo& obj0,
                      const RegisteredVertexInfo& obj) {
  return obj0.V_time > obj.V_time;
}

//######################################################################

class RegVInfo : public Linked<RegisteredVertexInfo> {
 public:
  void erase();
  RegVInfo& prev() { return (RegVInfo&)Linked<RegisteredVertexInfo>::prev(); }
  RegVInfo& next() { return (RegVInfo&)Linked<RegisteredVertexInfo>::next(); }
  bool operator==(RegVInfo& s) { return this == &s; }
};

Pool<RegVInfo> TheRVIPool;

inline void RegVInfo::erase() {
  Linked<RegisteredVertexInfo>::remove();
  TheRVIPool.push(*this);
}

#endif  // SRC_DLA_OBJECTS_HPP_
