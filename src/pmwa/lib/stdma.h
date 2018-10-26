#ifndef STDMA_H
#define STDMA_H

//#include<systemparameter.h>
#include <string>
#include <sstream>  // std::ostringstream

double factorial(int num);
void count_time(long tms, double x);
void count_time(double tms, double x, char* fname);

inline int EO2(int i) { return i % 2; }

inline int EO4(int i) { return i % 4; }

inline double sh(double expD) { return (expD - 1.0 / expD) * 0.5; }
inline double ch(double expD) { return (expD + 1.0 / expD) * 0.5; }

template <typename T>
std::string tostr(const T& t) {
  std::ostringstream os;
  os << t;
  return os.str();
}

template <class T>
void newcalls(T*& Array, int max_val) {
  Array = new T[max_val];
}
template <class T>
void newcalls(T**& Array, int max_val1, int max_val2) {
  Array = new T*[max_val1];
  for (int x = 0; x < max_val1; x++)
    Array[x] = new T[max_val2];
}

template <class T>
void newcall(T*& Array, int max_val) {
  Array = new T[max_val];

  for (int x = 0; x < max_val; x++)
    Array[x] = 0;
}

template <class T>
void newcall(T**& Array, int max_val1, int max_val2) {
  Array = new T*[max_val1];
  for (int x = 0; x < max_val1; x++)
    Array[x] = new T[max_val2];

  for (int x = 0; x < max_val1; x++)
    for (int y = 0; y < max_val2; y++)
      Array[x][y] = 0;
}

template <class T>
void newcall(T***& Array, int max_val1, int max_val2, int max_val3) {
  Array = new T**[max_val1];
  for (int x = 0; x < max_val1; x++)
    Array[x] = new T*[max_val2];
  for (int x = 0; x < max_val1; x++)
    for (int y = 0; y < max_val2; y++)
      Array[x][y] = new T[max_val3];

  for (int x = 0; x < max_val1; x++)
    for (int y = 0; y < max_val2; y++)
      for (int z = 0; z < max_val3; z++)
        Array[x][y][z] = 0;
}

template <class T>
void newcall(T****& Array, int max_val1, int max_val2, int max_val3, int max_val4) {
  Array = new T***[max_val1];
  for (int x = 0; x < max_val1; x++)
    Array[x] = new T**[max_val2];
  for (int x = 0; x < max_val1; x++)
    for (int y = 0; y < max_val2; y++)
      Array[x][y] = new T*[max_val3];
  for (int x = 0; x < max_val1; x++)
    for (int y = 0; y < max_val2; y++)
      for (int z = 0; z < max_val3; z++)
        Array[x][y][z] = new T[max_val4];

  for (int x = 0; x < max_val1; x++)
    for (int y = 0; y < max_val2; y++)
      for (int z = 0; z < max_val3; z++)
        for (int a = 0; a < max_val4; a++)
          Array[x][y][z][a] = 0;
}

template <class T>
void newcall(T*****& Array, int max_val1, int max_val2, int max_val3, int max_val4, int max_val5) {
  Array = new T****[max_val1];
  for (int x = 0; x < max_val1; x++)
    Array[x] = new T***[max_val2];
  for (int x = 0; x < max_val1; x++)
    for (int y = 0; y < max_val2; y++)
      Array[x][y] = new T**[max_val3];
  for (int x = 0; x < max_val1; x++)
    for (int y = 0; y < max_val2; y++)
      for (int z = 0; z < max_val3; z++)
        Array[x][y][z] = new T*[max_val4];
  for (int x = 0; x < max_val1; x++)
    for (int y = 0; y < max_val2; y++)
      for (int z = 0; z < max_val3; z++)
        for (int a = 0; a < max_val4; a++)
          Array[x][y][z][a] = new T[max_val5];

  for (int x = 0; x < max_val1; x++)
    for (int y = 0; y < max_val2; y++)
      for (int z = 0; z < max_val3; z++)
        for (int a = 0; a < max_val4; a++)
          for (int b = 0; b < max_val5; b++)
            Array[x][y][z][a][b] = 0;
}

template <class T>
void newcall(T******& Array, int max_val1, int max_val2, int max_val3, int max_val4, int max_val5, int max_val6) {
  Array = new T*****[max_val1];
  for (int x = 0; x < max_val1; x++)
    Array[x] = new T****[max_val2];
  for (int x = 0; x < max_val1; x++)
    for (int y = 0; y < max_val2; y++)
      Array[x][y] = new T***[max_val3];
  for (int x = 0; x < max_val1; x++)
    for (int y = 0; y < max_val2; y++)
      for (int z = 0; z < max_val3; z++)
        Array[x][y][z] = new T**[max_val4];
  for (int x = 0; x < max_val1; x++)
    for (int y = 0; y < max_val2; y++)
      for (int z = 0; z < max_val3; z++)
        for (int a = 0; a < max_val4; a++)
          Array[x][y][z][a] = new T*[max_val5];
  for (int x = 0; x < max_val1; x++)
    for (int y = 0; y < max_val2; y++)
      for (int z = 0; z < max_val3; z++)
        for (int a = 0; a < max_val4; a++)
          for (int b = 0; b < max_val5; b++)
            Array[x][y][z][a][b] = new T[max_val6];

  for (int x = 0; x < max_val1; x++)
    for (int y = 0; y < max_val2; y++)
      for (int z = 0; z < max_val3; z++)
        for (int a = 0; a < max_val4; a++)
          for (int b = 0; b < max_val5; b++)
            for (int c = 0; c < max_val6; c++)
              Array[x][y][z][a][b][c] = 0;
}

template <class T>
void delcall(T*& Array) {
  delete[] Array;
}

template <class T>
void delcall(T**& Array, int max_val1) {
  for (int x = 0; x < max_val1; x++)
    delete[] Array[x];
  delete[] Array;
}

template <class T>
void delcall(T***& Array, int max_val1, int max_val2) {
  for (int x = 0; x < max_val1; x++)
    for (int y = 0; y < max_val2; y++)
      delete[] Array[x][y];
  for (int x = 0; x < max_val1; x++)
    delete[] Array[x];
  delete[] Array;
}

template <class T>
void delcall(T****& Array, int max_val1, int max_val2, int max_val3) {
  for (int x = 0; x < max_val1; x++)
    for (int y = 0; y < max_val2; y++)
      for (int z = 0; z < max_val3; z++)
        delete[] Array[x][y][z];
  for (int x = 0; x < max_val1; x++)
    for (int y = 0; y < max_val2; y++)
      delete[] Array[x][y];
  for (int x = 0; x < max_val1; x++)
    delete[] Array[x];
  delete[] Array;
}

template <class T>
void delcall(T*****& Array, int max_val1, int max_val2, int max_val3, int max_val4) {
  for (int x = 0; x < max_val1; x++)
    for (int y = 0; y < max_val2; y++)
      for (int z = 0; z < max_val3; z++)
        for (int a = 0; a < max_val4; a++)
          delete[] Array[x][y][z][a];
  for (int x = 0; x < max_val1; x++)
    for (int y = 0; y < max_val2; y++)
      for (int z = 0; z < max_val3; z++)
        delete[] Array[x][y][z];
  for (int x = 0; x < max_val1; x++)
    for (int y = 0; y < max_val2; y++)
      delete[] Array[x][y];
  for (int x = 0; x < max_val1; x++)
    delete[] Array[x];
  delete[] Array;
}

template <class T>
void delcall(T*****& Array, int max_val1, int max_val2, int max_val3, int max_val4, int max_val5) {
  for (int x = 0; x < max_val1; x++)
    for (int y = 0; y < max_val2; y++)
      for (int z = 0; z < max_val3; z++)
        for (int a = 0; a < max_val4; a++)
          for (int b = 0; b < max_val5; b++)
            delete[] Array[x][y][z][a][b];
  for (int x = 0; x < max_val1; x++)
    for (int y = 0; y < max_val2; y++)
      for (int z = 0; z < max_val3; z++)
        for (int a = 0; a < max_val4; a++)
          delete[] Array[x][y][z][a];
  for (int x = 0; x < max_val1; x++)
    for (int y = 0; y < max_val2; y++)
      for (int z = 0; z < max_val3; z++)
        delete[] Array[x][y][z];
  for (int x = 0; x < max_val1; x++)
    for (int y = 0; y < max_val2; y++)
      delete[] Array[x][y];
  for (int x = 0; x < max_val1; x++)
    delete[] Array[x];
  delete[] Array;
}

#endif
