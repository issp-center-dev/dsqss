// DSQSS (Discrete Space Quantum Systems Solver)
// Copyright (C) 2018- The University of Tokyo
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#ifndef SERIALIZE_H
#define SERIALIZE_H

#include <cstring>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

namespace Serialize {

template <class T>
void save(std::ofstream& ofs, const T& val) {
  ofs.write(reinterpret_cast<const char*>(&val), sizeof(T));
}
template <class T>
void load(std::ifstream& ifs, T& val) {
  ifs.read(reinterpret_cast<char*>(&val), sizeof(T));
}
template <class T>
T load(std::ifstream& ifs) {
  T val;
  load(ifs, val);
  return val;
}

template <>
void save(std::ofstream& ofs, const std::string& val) {
  const size_t N = val.size();
  save(ofs, N);
  for (size_t i = 0; i < N; ++i) {
    const char c = val[i];
    save(ofs, c);
  }
}
template <>
void load(std::ifstream& ifs, std::string& val) {
  size_t N;
  load(ifs, N);
  std::stringstream ss;
  char c;
  for (size_t i = 0; i < N; ++i) {
    load(ifs, c);
    ss << c;
  }
  val = ss.str();
}

template <class T>
void save(std::ofstream& ofs, const std::vector<T>& val) {
  const size_t N = val.size();
  save(ofs, N);
  for (size_t i = 0; i < N; ++i) {
    T v = val[i];
    save(ofs, v);
  }
}

template <class T>
void load(std::ifstream& ifs, std::vector<T>& val) {
  size_t N = 0;
  load(ifs, N);
  val.clear();
  for (size_t i = 0; i < N; ++i) {
    T v;
    load(ifs, v);
    val.push_back(v);
  }
}

}  // namespace Serialize

#endif  // SERIALIZE_H
