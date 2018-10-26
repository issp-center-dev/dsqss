#ifndef DSQSS_TOSTRING_H
#define DSQSS_TOSTRING_H

#include <string>
#include <sstream>

template <class T>
std::string tostring(T const& x){
  std::stringstream ss;
  ss << x;
  return ss.str();
}

#endif
