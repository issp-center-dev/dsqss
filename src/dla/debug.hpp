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

#ifndef DEBUG_H
#define DEBUG_H

#ifdef DEB
#include <iostream>
#include <string>

class AutoDebugDump_impl {
 private:
  std::string message;

 public:
  explicit AutoDebugDump_impl(std::string const& msg) : message(msg) {
    std::cout << message << "> Start.\n" << std::flush;
  }
  ~AutoDebugDump_impl() {
    { std::cout << message << "> End.\n" << std::flush; }
  }
};

// we should expand macro twice to expand __LINE__ properly.
// (concatenation(##) is stronger than substitution (__LINE__ -> line number))
#define AutoDebugDump_macroimpl2(msg, l) AutoDebugDump_impl ADD_obj_##l(msg)
#define AutoDebugDump_macroimpl(msg, l) AutoDebugDump_macroimpl2(msg, l)
#define AutoDebugDump(msg) AutoDebugDump_macroimpl(msg, __LINE__)

#define DebugDumpNV(name)                                      \
  do {                                                         \
    std::cout << #name << " = " << name << "\n" << std::flush; \
  } while (false)

#else  // DEB

#define AutoDebugDump(msg) (0)
#define DebugDumpNV(name) (0)

#endif  // DEB

#endif  // DEBUG_H
