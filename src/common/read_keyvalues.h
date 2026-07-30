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

#ifndef SRC_COMMON_READ_KEYVALUES_H_
#define SRC_COMMON_READ_KEYVALUES_H_

#include <cctype>
#include <algorithm>
#include <exception>
#include <map>
#include <string>

void read_keyvalues(std::map<std::string, std::string>& dict,
                    std::string const& filename);
std::map<std::string, std::string> read_keyvalues(std::string const& filename);
bool parse_kvline(std::string& key, std::string& value,
                  std::string const& line);

struct ToLower {
  char operator()(char c) const { return std::tolower(c); }
};

void read_keyvalues(std::map<std::string, std::string>& dict,
                    std::string const& filename) {
  std::string line, key, value;
  std::ifstream ifs(filename.c_str());
  if (!ifs) {
    std::string msg("read_keyvalues> ERROR: cannot open a parameter file: ");
    msg += filename;
    throw std::runtime_error(msg);
  }

  while (ifs && std::getline(ifs, line)) {
    if (parse_kvline(key, value, line)) {
      std::transform(key.begin(), key.end(), key.begin(), ToLower());
      dict[key] = value;
    }
  }
}

std::map<std::string, std::string> read_keyvalues(std::string const& filename) {
  std::map<std::string, std::string> dict;
  read_keyvalues(dict, filename);
  return dict;
}

bool parse_kvline(std::string& key, std::string& value,
                  std::string const& line) {
  // Equivalent to the former regex pair
  //   "^\s*(.*?)(#|$)"  (strip comment and leading whitespace)
  //   "^(.*?)\s*=\s*(.*?)\s*$"  (split at the first '=' and trim)
  static const char* const ws = " \t\v\f\r\n";

  // strip the comment and surrounding whitespace
  std::string body = line.substr(0, line.find('#'));
  std::string::size_type b = body.find_first_not_of(ws);
  if (b == std::string::npos) {
    return false;  // blank or comment-only line
  }
  std::string::size_type e = body.find_last_not_of(ws);
  body = body.substr(b, e - b + 1);

  // split at the first '='
  std::string::size_type eq = body.find('=');
  if (eq == std::string::npos) {
    std::string msg("parse_kvline> ERROR: missing '=': ");
    msg += line;
    throw std::runtime_error(msg);
  }

  key = body.substr(0, eq);
  std::string::size_type ke = key.find_last_not_of(ws);
  key = (ke == std::string::npos) ? "" : key.substr(0, ke + 1);

  value = body.substr(eq + 1);
  std::string::size_type vb = value.find_first_not_of(ws);
  value = (vb == std::string::npos) ? "" : value.substr(vb);

  return true;
}

#define deprecated_parameter(dict, new, old)                               \
  if (dict.count(old) > 0) {                                               \
    std::cerr << "WARNING: parameter " << old                              \
              << " is deprecated and will be removed in the next version." \
              << std::endl;                                                \
    std::cerr << "Use " << new << " instead." << std::endl;                \
    dict[new] = dict[old];                                                 \
  }

#endif  // SRC_COMMON_READ_KEYVALUES_H_
