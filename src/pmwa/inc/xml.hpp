#ifndef XML_H
#define XML_H

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

// io.h (here)##################
#define BLEN 256

inline int line_split(char *line, std::string *w) {
  std::string s(line);
  std::istringstream ist(s);
  int nw = 0;
  while (ist >> w[nw++])
    ;
  nw--;
  return nw;
}

class FileReader {
 private:
  std::string EOL;
  char NAME[BLEN];
  char LINE[BLEN];
  std::string WORD[BLEN];
  int IL;
  int NW;
  std::ifstream INS;
  std::streampos top;

 public:
  void open(const char *name) {
    strcpy(NAME, name);
    INS.open(NAME);
    top = INS.tellg();
    if (!INS) {
      printf("FILEIO::FILEIO> Error.\n");
      exit(0);
    };
    IL = 0;
  };

  FileReader() { EOL = "_E_O_L_"; };

  void rewind() {
    INS.clear();
    INS.seekg(top);
  };

  int split() {
    NW = line_split(LINE, WORD);
    return NW;
  };

  bool read() {
    bool b = (bool)INS.getline(LINE, BLEN);
    if (b) {
      IL++;
    }
    return b;
  };

  std::string &word(int i) {
    if (i < 0 && i >= NW) {
      printf("FileReader::word> Error.\n");
      exit(0);
    }
    return WORD[i];
  };

  void getWordList(int &NW, std::string *&W);
};

// io.h (end)################
// array.h (here)##################
//---------------------------------
class IndexSystem {
 private:
  bool INI;
  std::string LBL;  // label
  int D;
  int *L;
  int N;

 public:
  void init(const int d, const int *l, const std::string &LBL0 = "");

  IndexSystem() { INI = false; };

  ~IndexSystem() {
    if (initialized()) delete[] L;
  };

  bool initialized() const { return INI; };
};

//------------------------------
template <class C>
class Array {
 private:
  std::string LBL;  // label
  int D;
  C *val;
  IndexSystem ID;

  void init(va_list &ap);

 public:
  Array() {
    LBL = "";
    val = 0;
  };

  ~Array();

  void reset();

  void init(const std::string &s, int d, ...);

  C &operator[](const int i);
};

template <class C>
C &Array<C>::operator[](const int i) {
  return val[i];
}

//======================================================================

// array.h (end)##################

namespace XML {

class Block;

class Block {
 private:
  std::string EOL;
  std::string Name;   // the name of the element
  std::string *Word;  // the whole list of words to be processed
  std::string Open;   // the key at which to start the process
  std::string Close;  // the key at which to terminate the process
  Array<Block> SubBlock;
  int NB;  // the number of subelements
  int NV;  // the number of values
  Array<std::string> Value;

 public:
  void initialize(std::string *word, const std::string &name = "");

  void initialize(const std::string &FNAME, const std::string &name = "");

  Block() {
    NB = 0;
    NV = 0;
    EOL = "_E_O_L_";
  };

  ~Block(){
      //    printf("*** Destroying XML::Block (%s)\n", Name.c_str());
  };

  void read();

  ////
  bool syntax_error();
  ///

  Block &operator[](const int &i);

  Block &operator[](const std::string &name);

  const std::string &getName() const;

  int getInteger(const int i = 0);

  double getDouble(const int i = 0);

  std::string &getString(const int i = 0);

  Block &getElement(const std::string &name);

  const int &NumberOfBlocks() const { return NB; };

  const int &NumberOfValues() const { return NV; };
};

inline int Block::getInteger(const int i) {
#ifdef DEBUG
  if (i < 0 || i >= NV) {
    printf("Block::getInteger> Error.\n");
    printf("  The argument (= %d) is out of the bounds [0,%d).\n", i, NV);
    exit(0);
  }
  cout << "getInteger NV=" << NV << endl;
#endif
  std::string &s = getString(i);
  return atoi(s.c_str());
}

inline double Block::getDouble(const int i) {
#ifdef DEBUG
  if (i < 0 || i >= NV) {
    printf("Block::getDouble> Error.\n");
    printf("  The argument (= %d) is out of the bounds [0,%d).\n", i, NV);
    exit(0);
  }
#endif
  std::string &s = getString(i);
  return (double)atof(s.c_str());
}

inline const std::string &Block::getName() const { return Name; }

//======================================================================

inline std::string &Block::getString(const int i) {
#ifdef DEBUG
  if (i < 0 || i >= NV) {
    printf("Block::getString> Error.\n");
    printf("  The argument (= %d) is out of the bounds [0,%d).\n", i, NV);
    exit(0);
  }
#endif
  return Value[i];
}

//======================================================================

//##### for "read()"
//======================================================================

}  // namespace XML

#endif
