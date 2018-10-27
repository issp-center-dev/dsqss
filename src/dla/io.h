
#ifndef IO_H
#define IO_H

//######################################################################

#include <cstdio>
#include <cstdlib>
#include <string>
#include <cstring>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>

#include <boost/lexical_cast.hpp>

#include "array.h"
#include "../common/tostring.h"

#define BLEN 256

//######################################################################

std::string EOL = "_E_O_L_";

//======================================================================

inline void reform_end_of_line(std::string& line) {
  int n         = line.size();
  const char* a = line.c_str();
  if (a[n - 1] == 13) {  // 13 stands for ^M
    line.replace(n - 1, 1, 1, '\n');
  }
}

//======================================================================

inline int line_split(char* line, std::string* w) {
  std::string s(line);
  std::istringstream ist(s);
  int nw = 0;
  while (ist >> w[nw++])
    ;
  nw--;
  return nw;
}

//======================================================================

inline void get_line(FILE* F, char* line) {
  char buff[256];
  strcpy(buff, "");
  strcpy(line, "");
  fgets(buff, 100, F);
  if (buff[0] == 0 || buff[0] == 13 || buff[0] == 10) return;
  char* pch = strchr(buff, '#');
  if (pch != NULL) {
    int len = pch - buff;
    if (len > 0) {
      strncat(line, buff, len);
      strcat(line, "\n");
    }
  } else {
    strcat(line, buff);
  }
}

//======================================================================

inline void get_nbl(FILE* F, char* line) {  // get the next non-blank line
  strcpy(line, "");
  while (!strcmp(line, "")) {
    if (feof(F)) break;
    get_line(F, line);
    //  printf("%s\n",line);
  }
}

//======================================================================

inline int break_into_words(char* line, char* delim, char** word) {
  char* last = line + strlen(line) - 1;
  //printf( "line = '%s'\n", line);
  //printf( "delimiter = '%s'\n", delim);
  char* w = line;
  int n   = 0;
  while (w != last) {
    while (w == strpbrk(w, delim))
      w++;
    char* p = strpbrk(w, delim);
    if (p == 0) p = last;
    //printf("\nw= %s", w);
    //printf("p= %d, (*p) = '%c' (%d)\n", p, *p, *p);
    strncpy(word[n], w, p - w);
    strcat(word[n], "\0");
    //printf("word[%d] = '%s'\n", n, word[n]);
    n++;
    w = p;
  }
  return n;
}

//######################################################################

class FileReader {
private:
  char NAME[BLEN];
  char LINE[BLEN];
  int IL;
  int NW;
  std::string WORD[BLEN];
  std::ifstream INS;
  std::streampos top;
  std::streampos mark;

public:
  void open(const char* name) {
    strcpy(NAME, name);
    INS.open(NAME);
    if (!INS) {
      std::string msg("FileReader::open> Error. Unable to open ");
      msg += NAME;
      throw std::runtime_error(msg);
    }
    top = INS.tellg();
    IL = 0;
  };

  FileReader(){};

  FileReader(const char* name) { open(name); };

  void rewind() {
    INS.clear();
    INS.seekg(top);
  };

  void set_mark() { mark = INS.tellg(); };

  void goto_mark() {
    INS.clear();
    INS.seekg(mark);
  };

  bool read() {
    bool b = static_cast<bool>(INS.getline(LINE, BLEN));
    if (b) { IL++; }
    return b;
  };

  char* line() { return LINE; };

  int split() {
    NW = line_split(LINE, WORD);
    return NW;
  };

  std::string& word(int i) {
    if (i < 0 && i >= NW) {
      std::string msg("FileReader::word> Invalid index ");
      msg += tostring(i);
      throw std::out_of_range(msg);
    }
    return WORD[i];
  };

  int as_int(int i) { return atoi(WORD[i].c_str()); };

  double as_double(int i) { return (double)atof(WORD[i].c_str()); };

  void show() { std::cout << LINE << std::endl; };

  void dump() {
    std::cout << NAME << "[" << IL << "]> ";
    for (int i = 0; i < NW; i++) {
      std::cout << " " << WORD[i];
    }
    std::cout << std::endl;
  };

  std::string get(char* key);

  int makeIndex(const char* scope, const char* field, const char* k0,
                Array<int>& index);

  void getWordList(int& NW, std::string*& W);
};

//======================================================================

inline void FileReader::getWordList(int& NW, std::string*& W) {
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

//======================================================================

inline std::string FileReader::get(char* key) {
  std::string ans;
  char key_open[BLEN];
  char key_close[BLEN];
  strcpy(key_open, "<");
  strcat(key_open, key);
  strcat(key_open, ">");
  strcpy(key_close, "</");
  strcat(key_close, key);
  strcat(key_close, ">");
  printf(" %s %s\n", key_open, key_close);
  rewind();
  bool active = false;
  while (read()) {
    char* l = line();
    //    int nw = split();
    std::string keyt = word(0);
    if (keyt == key_close) active = false;
    if (active) {
      std::string s = l;
      reform_end_of_line(s);
      ans += s;
    }
    if (keyt == key_open) active = true;
  }
  return ans;
}

//======================================================================

inline int FileReader::makeIndex(const char* scope, const char* field,
                                 const char* k0, Array<int>& index) {
  //  printf("FileReader::makeIndex> start.\n");

  std::string key(k0);
  std::string close = "</";
  close += scope;
  close += ">";
  std::string activate = "<";
  activate += field;
  activate += ">";
  std::string inactivate = "</";
  inactivate += field;
  inactivate += ">";

  //  printf(" close= %s\n", close.c_str());
  //  printf(" activate= %s\n", activate.c_str());
  //  printf(" inactivate= %s\n", inactivate.c_str());

  int n = 0;
  set_mark();
  bool active = false;
  while (read()) {
    //show();
    int nw = split();
    if (nw == 0) continue;
    std::string k = word(0);
    if (k == close) break;
    if (k == inactivate) active = false;
    if (active) {
      if (k == key) {
        //printf(" %s:%s:%s> %s\n", scope, field, k0, LINE);
        n++;
      }
    }
    if (k == activate) active = true;
  }

  index.init(1, n);

  int i = 0;
  goto_mark();
  active = false;
  while (read()) {
    int nw = split();
    if (nw == 0) continue;
    std::string k = word(0);
    if (k == close) break;
    if (k == inactivate) active = false;
    if (active) {
      if (k == key) {
        index[i] = as_int(1);
        i++;
      }
    }
    if (k == activate) active = true;
  }

  goto_mark();

  //  printf("FileReader::makeIndex> end.\n");

  return n;
}

#endif
