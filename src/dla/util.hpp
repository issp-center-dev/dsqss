#ifndef UTIL_HPP
#define UTIL_HPP

#include <cstdlib>
#include <iostream>
#include <vector>

namespace util {
template <class T>
int min_index(const std::vector<T>& xs) {
  const int n = xs.size();
  int i       = 0;
  T xmin      = xs[0];
  for (int j = 1; j < n; ++j) {
    if (xs[j] < xmin) {
      i    = j;
      xmin = xs[j];
    }
  }
  return i;
}
template <class T>
int max_index(const std::vector<T>& xs) {
  const int n = xs.size();
  int i       = 0;
  T xmax      = xs[0];
  for (int j = 1; j < n; ++j) {
    if (xs[j] > xmax) {
      i    = j;
      xmax = xs[j];
    }
  }
  return i;
}


void ERROR(const char* msg){
  std::cerr << "ERROR: " << msg << std::endl;
  std::exit(1);
}

}  // namespace util

#endif // UTIL_HPP
