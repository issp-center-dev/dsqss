#include <lattice.hpp>

namespace ARRAY {
int EOL = -1;
}
using namespace XML;
using namespace ARRAY;

inline void FileReader::getWordList(int &NW, std::string *&W) {
  NW = 0;
  rewind();

  while (read())
    NW += split();
  W      = new std::string[NW + 1];
  int iw = 0;
  rewind();
  while (read()) {
    int nw = split();
    for (int i = 0; i < nw; i++) {
      W[iw] = word(i);
      iw++;
    }
  }
  W[NW] = EOL;
}

void IndexSystem::init(const int d, const int *l, const std::string &LBL0) {
  if (initialized()) delete[] L;
  if (d == 0) {
    printf("IndexSystem::init> Error. d = 0.\n");
    exit(0);
  }
  INI = true;
  LBL = LBL0;
  D   = d;
  L   = new int[d];
  N   = 1;
  for (int i = 0; i < d; i++) {
    N *= l[i];
    L[i] = l[i];
  }
  if (N == 0) {
    printf("IndexSystem::init> Error. N = 0.\n");
    for (int i = 0; i < d; i++)
      printf("  L[%d] = %d\n", i, L[i]);
    exit(0);
  };
}

template <class C>
void Array<C>::reset() {
  if (val != 0) {
    delete[] val;
    val = 0;
  }
}

template <class C>
void Array<C>::init(va_list &ap) {
  reset();
  int *L = new int[D];
  int N  = 1;
  int l;
  int i = 0;
  for (i = 0; i < D; i++) {
    l = va_arg(ap, int);

    if (l == ARRAY::EOL) break;
    L[i] = l;
    N *= l;
  }
  if (N == 0) {
    printf("Array::init> Error.\n");
    printf("   ... tried to create an array with size 0.\n");
    exit(0);
  }
  if (l == ARRAY::EOL) {
    for (int j = i; j < D; j++) {
      L[j] = L[i - 1];
      N *= L[j];
    }
  }
  val = new C[N];
  ID.init(D, L, LBL);
  delete[] L;
}

template <class C>
void Array<C>::init(const std::string &s, int d, ...) {
  LBL = s;
  D   = d;
  va_list ap;
  va_start(ap, d);
  init(ap);
  va_end(ap);
}

template <class C>
Array<C>::~Array() {
  //  printf("*** Destroying Array (%s)\n", LBL.c_str());
  reset();
}

inline Block &Block::getElement(const std::string &name) {
  int ib;
  for (ib = 0; ib < NB; ib++) {
    if (name == SubBlock[ib].getName()) break;
  }
  if (ib < NB) {
    return SubBlock[ib];
  } else {
    printf("Block::getElement> Error.\n");
    printf("  Tried to find a subblock '%s',\n", name.c_str());
    printf("  but no such block exists in a block '%s'.\n", getName().c_str());
    exit(0);
  }
}

Block &Block::operator[](const int &i) { return SubBlock[i]; };

Block &Block::operator[](const std::string &name) {
  Block &B = getElement(name);
  return B;
}

inline void Block::initialize(std::string *word, const std::string &name) {
  //printf("Block::initialize> Pass 1\n");
  Name = name;
  Word = word;
  if (name == "") {
    Open  = "";
    Close = "";
  } else {
    Open  = "<" + name + ">";
    Close = "</" + name + ">";
  }
  read();
}

void Block::initialize(const std::string &FNAME, const std::string &name) {
  //    printf("Block::initialize> Pass 2\n"); // koko

  FileReader F;
  // cout<<"FNAME="<<FNAME<<endl;
  F.open(FNAME.c_str());
  int NW;
  std::string *W;
  F.getWordList(NW, W);  // # of words, the first name
  initialize(W, name);
  delete[] W;
}

inline bool isCommentOpening(const std::string &key) {
  std::string sopen = "<!--";
  int n        = key.length();
  if (n < 4) return false;
  if (key.substr(0, 4) == sopen) return true;
  return false;
}


inline bool isOpening(const std::string &key) {
  int n = key.length();
  if (key[0] != '<') return false;
  if (key[1] == '/') return false;
  if (key[n - 1] != '>') return false;
  return true;
}


inline bool isClosing(const std::string &key) {
  int n = key.length();
  if (key[0] != '<') return false;
  if (key[1] != '/') return false;
  if (key[n - 1] != '>') return false;
  return true;
}


inline const std::string getOpeningName(const std::string &key) {
  if (!isOpening(key)) exit(0);
  int n      = key.length();
  std::string ans = key.substr(1, n - 2);
  return ans;
}


inline const std::string getClosingName(const std::string &key) {
  if (!isClosing(key)) exit(0);
  int n      = key.length();
  std::string ans = key.substr(2, n - 3);
  return ans;
}

inline bool Block::syntax_error() {
  NB            = 0;
  NV            = 0;
  int depth     = 0;
  bool open     = false;
  std::string SkipTo = "";
  int i         = 0;
  while (Word[i] != EOL) {
    std::string &w = Word[i++];
    if (Name == "") {
      if (isOpening(w)) {
        const std::string name = getOpeningName(w);
        Name              = name;
        Open              = "<" + name + ">";
        Close             = "</" + name + ">";
      }
    }
    if (w == Open) {
      depth++;
      open = true;
      continue;
    }
    if (!open) continue;
    if (w == Close) {
      depth--;
      break;
    }
    if (SkipTo != "" && w != SkipTo) continue;
    if (SkipTo != "" && w == SkipTo) {
      depth--;
      SkipTo = "";
      continue;
    }
    if (isCommentOpening(w)) {
      //cout << "  ... beginning of a comment" << endl;
      depth++;
      SkipTo = "-->";
      continue;
    }
    if (isOpening(w)) {
      //cout << "  ... beginning of a subelement [" << w << "]" << endl;
      depth++;
      const std::string name = getOpeningName(w);
      //cout << "opening name : " << name << endl;
      SkipTo = "</" + name + ">";
      NB++;
      continue;
    }
    if (isClosing(w)) {
      printf("Block::read> Error.\n");
      printf("  An unexpected closing tag %s\n", w.c_str());
      printf("  is detected in reading an element of name [%s].\n", getName().c_str());
      return true;
    }
    //cout << "  ... a value" << endl;
    NV++;
  }

  if (depth != 0) {
    printf("Block::read> Error.\n");
    std::string expected = SkipTo;
    if (expected == "") { expected = Close; }
    printf("  A missing closing tag %s\n", expected.c_str());
    printf("  is detected in reading an element of name [%s].\n", getName().c_str());
    return true;
  }

  return false;
}


void Block::read() {
  //  printf("Block::read> Pass 1\n");
  if (syntax_error()) exit(0);

  if (NV > 0) Value.init("Value", 1, NV);
  if (NB > 0) SubBlock.init("SubBlock", 1, NB);
  // cout<<"Block.NV = "<<NV<<",  NB = "<<NB<<endl;
  bool open     = false;
  std::string SkipTo = "";
  int ib        = 0;
  int iv        = 0;
  int i         = 0;
  while (true) {
    std::string &w = Word[i++];
    //  cout << "### " << w << endl;//koko
    if (w == Open) {
      //	printf("Opened. %s\n", w.c_str());//koko
      open = true;
      continue;
    }
    if (!open) continue;
    if (w == Close) break;
    if (SkipTo != "" && w != SkipTo) continue;
    if (SkipTo != "" && w == SkipTo) {
      SkipTo = "";
      continue;
    }
    if (isCommentOpening(w)) {
      SkipTo = "-->";
      continue;
    }
    if (isOpening(w)) {
      const std::string name = getOpeningName(w);
      SubBlock[ib].initialize(&w, name);
      ib++;
      SkipTo = "</" + name + ">";
      continue;
    }
    Value[iv] = w;
    iv++;
  }
}


Lattice::Lattice(const std::string &FNAME) {
#ifdef DEBUG
  printf("\nLattice::Lattice> Start.\n");  // koko
  cout << "latfile = " << FNAME << endl;
#endif
  X.initialize(FNAME.c_str(), "LATTICE");
  //  cout<<"XML is read successfully."<<endl;
  read();
#ifdef DEBUG
  printf("Lattice::Lattice> End.\n");  // koko
#endif
}


void Lattice::read() {
  D = X["Dimension"].getInteger();
  // cout<<"D="<<D<<endl;
  for (int i = 0; i < 4; i++)
    L[i] = 1;
  for (int i = 0; i < D; i++)
    L[i] = X["LinearSize"].getInteger(i);

  //  cout<<"L="<<L[0]<<" "<<L[1]<<" "<<L[2]<<endl;
  NLdiv = X["NumberOfLDecomposition"].getInteger();
  // cout<<"NLdiv="<<NLdiv<<endl;

  Nx = X["LinearDomainSize"].getInteger(0);
  Ny = (D > 1) ? X["LinearDomainSize"].getInteger(1) : 1;
  Nz = (D == 3) ? X["LinearDomainSize"].getInteger(2) : 1;
  //cout<<"Ldom="<<Nx<<" "<<Ny<<" "<<Nz<<endl;

  BETA = X["Beta"].getDouble();
  //cout<<"BETA="<<BETA<<endl;
  //oldBETA = X["OldBeta"].getDouble();
  NBdiv   = X["NumberOfBDecomposition"].getInteger();
  // B       = X["BetaOfDomain"].getDouble();
  NCELL   = X["NumberOfCells"].getInteger();
  V       = X["NumberOfSites"].getInteger();
  TB      = X["NumberOfInteractions"].getInteger();
  //cout<<"TB="<<TB<<endl;
  //cout<<"V="<<V<<endl;
  lc   = TB / V;
  bnum = lc;
  //cout<<"lc="<<lc<<endl;
  NSTYPE = X["NumberOfSiteTypes"].getInteger();
  NITYPE = X["NumberOfInteractionTypes"].getInteger();
  NFIELD = X["NumberOfExternalField"].getInteger();

  newcall(bond_vec, lc, D);
  newcalls(bd, V, bnum);
  newcall(frame, lc, V);  //sites on boundary
  newcall(frame_rsite, lc, V);
  newcall(frame_rnum, lc, V);
  newcall(frame_lsite, lc, V);
  newcall(frame_lnum, lc, V);

  newcall(Fx, lc);
  newcall(lx, V);

  for (int l = 0; l < lc; l++) {
    if (l == 0)
      bond_vec[l][0] = 1.0;
    else if (l == 1)
      bond_vec[l][1] = 1.0;
    else
      bond_vec[l][2] = 1.0;
  }

  Pd = lc;

  for (int i = 0; i < X.NumberOfBlocks(); i++) {
    XML::Block &B = X[i];

    if (B.getName() == "S") {
      int id = B.getInteger(0);
      int st = B.getInteger(1);
      int mt = B.getInteger(2);
      lx[id] = mt;
    }
    int k;

    if (B.getName() == "I") {
      int id = B.getInteger(0);
      int it = B.getInteger(1);
      int nb = B.getInteger(2);
      int ei = B.getInteger(3);

      int s0 = B.getInteger(4);
      int s1 = B.getInteger(5);

      int l = id % lc;

      bd[s0][l] = s1;

      if (ei >= 0) {
        k                 = Fx[l];
        frame[l][s0]      = true;
        frame_lsite[l][k] = s0;
        frame_lnum[l][s0] = k;
        frame_rsite[l][k] = s1;
        frame_rnum[l][s1] = k;
        Fx[l]++;
      } else {
        frame_lnum[l][s0] = -1;
        frame_rnum[l][s1] = -1;
      }
    }
  }
}

#include <set>

void Lattice::make_Size(Size *_Nsize) {
  N = _Nsize;

  _Nsize->d    = D;
  _Nsize->x    = L[0];     //14
  _Nsize->y    = L[1];     //15
  _Nsize->z    = L[2];     //16
  _Nsize->B    = BETA;     //17
  _Nsize->oldB = oldBETA;  //18
  _Nsize->V    = _Nsize->x * _Nsize->y * _Nsize->z;
}

void Lattice::make_Parallel(Parallel *_PR) {
  PR = _PR;

  PR->Nxdiv = NLdiv;
  PR->Nydiv = (D >= 2) ? NLdiv : 1;
  PR->Nzdiv = (D == 3) ? NLdiv : 1;
  PR->Ntdiv = NBdiv;
  PR->Rpara = NFIELD;

  PR->B    = BETA/NBdiv;                        // beta for a domain.
  PR->oldB = oldBETA / (double)NBdiv;  // for annealing.

  PR->Nsdiv = PR->Nxdiv * PR->Nydiv * PR->Nzdiv;  // the number of spatial decompositions.

  PR->x = Nx;
  PR->y = Ny;
  PR->z = Nz;

  PR->V = V;

  PR->NtNs  = PR->Ntdiv * PR->Nsdiv;  //the number of decompositions (non-trivial parallelization).
  PR->Npara = PR->p_num / PR->NtNs;   //the number of trivial parallelization.

  PR->nt = PR->my_rank % PR->Ntdiv;                     // the temporal domain number for the processor.
  PR->ns = (int)(PR->my_rank / PR->Ntdiv) % PR->Nsdiv;  // the spatial domain number for the processor.

  PR->nx = PR->ns % PR->Nxdiv;                // the x-directional domain number for the processor.
  PR->ny = (PR->ns / PR->Nxdiv) % PR->Nydiv;  // the y-directional domain number for the processor.
  PR->nz = PR->ns / (PR->Nxdiv * PR->Nydiv);  // the z-directional domain number for the processor.

  PR->nst = PR->my_rank % PR->NtNs;    // the domain number for the processor.
  PR->np  = PR->my_rank / (PR->NtNs);  // the seed number of the trivial parallelization for the processor.

  if (PR->Rpara > 0) {
    PR->nr = PR->np % PR->Rpara;  // a random potential number (one of trivial parallelization)
    PR->nq =
        PR->np
        / PR->Rpara;  // a seed number of the trivial parallelization for the random potential (one of trivial parallelization)
  } else {
    PR->nr = 0;
    PR->nq = PR->np;
  }

  //the coordinate is (nt,nx,ny,nz,np)

  PR->nst0 = PR->np * PR->NtNs;                       // nst=0 process number for the processor.
  PR->nt0  = PR->ns * PR->Ntdiv + PR->np * PR->NtNs;  //nt=0 process number for the processor.
  PR->ns0  = PR->nt + PR->np * PR->NtNs;              //ns=0 process number for the processor.
  PR->nx0  = PR->nt + (PR->ny * PR->Nxdiv + PR->nz * PR->Nxdiv * PR->Nydiv) * PR->Ntdiv
            + PR->np * PR->NtNs;  //nx=0 process number for the processor.

  PR->upper = (PR->nt + 1) % PR->Ntdiv + PR->ns * PR->Ntdiv
              + PR->np * PR->Ntdiv * PR->Nsdiv;  //the upper process number for the temporal direction.
  PR->lower = (PR->nt - 1 + PR->Ntdiv) % PR->Ntdiv + PR->ns * PR->Ntdiv
              + PR->np * PR->Ntdiv * PR->Nsdiv;  //the lower process number the temporal direction.

  PR->right[0] = PR->nt + ((PR->nx + 1) % PR->Nxdiv + PR->ny * PR->Nxdiv + PR->nz * PR->Nxdiv * PR->Nydiv) * PR->Ntdiv
                 + PR->np * PR->Ntdiv * PR->Nsdiv;  //the right side process number for the x direction.
  PR->left[0] =
      PR->nt + ((PR->nx - 1 + PR->Nxdiv) % PR->Nxdiv + PR->ny * PR->Nxdiv + PR->nz * PR->Nxdiv * PR->Nydiv) * PR->Ntdiv
      + PR->np * PR->Ntdiv * PR->Nsdiv;  //the left side process number for the x direction.

  PR->right[1] = PR->nt + (PR->nx + ((PR->ny + 1) % PR->Nydiv) * PR->Nxdiv + PR->nz * PR->Nxdiv * PR->Nydiv) * PR->Ntdiv
                 + PR->np * PR->Ntdiv * PR->Nsdiv;  //the right side process number for the y direction.
  PR->left[1] =
      PR->nt
      + (PR->nx + ((PR->ny - 1 + PR->Nydiv) % PR->Nydiv) * PR->Nxdiv + PR->nz * PR->Nxdiv * PR->Nydiv) * PR->Ntdiv
      + PR->np * PR->Ntdiv * PR->Nsdiv;  //the left side process number for the y direction.

  PR->right[2] = PR->nt + (PR->nx + PR->ny * PR->Nxdiv + ((PR->nz + 1) % PR->Nzdiv) * PR->Nxdiv * PR->Nydiv) * PR->Ntdiv
                 + PR->np * PR->Ntdiv * PR->Nsdiv;  //the right side process number for the z direction.
  PR->left[2] =
      PR->nt
      + (PR->nx + PR->ny * PR->Nxdiv + ((PR->nz - 1 + PR->Nzdiv) % PR->Nzdiv) * PR->Nxdiv * PR->Nydiv) * PR->Ntdiv
      + PR->np * PR->Ntdiv * PR->Nsdiv;  //the left side process number for the z direction.
}


Lattice::~Lattice() {
  delcall(bond_vec, lc);
  delcall(bd, V);
  delcall(frame, lc);
  delcall(Fx);
  delcall(frame_rsite, lc);
  delcall(frame_rnum, lc);
  delcall(frame_lsite, lc);
  delcall(frame_lnum, lc);
  delcall(lx);
}

//======================================================================
/*
inline int Lattice::countVertices() {
  int NV = 0;
  for (int b=0; b<NINT; b++) {
    NV += I(b).count();
  }
  return NV;
}
*/
//======================================================================

inline void Lattice::dump() {
  printf("\n");
  printf("Lattice Information:\n");
  printf("  D      = %d\n", D);
  printf("  L(global)=");
  for (int i = 0; i < D; i++) {
    printf(" %d", L[i]);
  }
  printf("\n");
  printf("  BETA   = %24.16f\n", BETA);
  printf("  NCELL  = %d\n", NCELL);
  printf("  NSITE  = %d\n", NSITE);
  printf("  NINT   = %d\n", NINT);
  printf("  NSTYPE = %d\n", NSTYPE);
  printf("  NITYPE = %d\n", NITYPE);
  printf("\n");
  //  for (int i=0; i<NSITE; i++) { S(i).dump(); }
  //  printf("\n");
  //  for (int i=0; i<NINT; i++) { I(i).dump(); }
}

void Lattice::show_param(std::ofstream &F) {
  using namespace std;
  F << "P D       = " << N->d << endl;
  F << "P L       = " << N->x << " " << N->y << " " << N->z << endl;
  F << "P BETA    = " << BETA << endl;
  F << "P DOML    = " << PR->x << " " << PR->y << " " << PR->z << endl;
  F << "P DOMBETA = " << PR->B << endl;
  F << "P NDIVL   = " << PR->Nxdiv << " " << PR->Nydiv << " " << PR->Nzdiv << endl;
  F << "P NDIVBETA= " << PR->Ntdiv << endl;
}

void Lattice::set_beta(double beta){
  BETA = beta;
  B = BETA / NBdiv;
}

void Lattice::set_oldbeta(double oldbeta){
  oldBETA = oldbeta;
}
