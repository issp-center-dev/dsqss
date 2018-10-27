#ifndef READ_KEYVALUES_H
#define READ_KEYVALUES_H

#include <cctype>
#include <exception>
#include <algorithm>
#include <string>
#include <map>
#include <boost/xpressive/xpressive_static.hpp>

void read_keyvalues(std::map<std::string, std::string> &dict, std::string const & filename);
std::map<std::string, std::string> read_keyvalues(std::string const & filename);
bool parse_kvline(std::string& key, std::string& value, std::string const& line);

struct ToLower{
  char operator()(char c) const { return std::tolower(c);}
};

void read_keyvalues(std::map<std::string, std::string> &dict, std::string const& filename){
  std::string line, key, value;
  std::ifstream ifs(filename.c_str());
  if(!ifs){
    std::string msg("read_keyvalues> ERROR: cannot open a parameter file: "); 
    msg += filename;
    throw std::runtime_error(msg);
  }

  while (ifs && std::getline(ifs, line)) {
    if(parse_kvline(key, value, line)){
      std::transform(key.begin(), key.end(), key.begin(), ToLower());
      dict[key] = value;
    }
  }
}

std::map<std::string, std::string> read_keyvalues(std::string const& filename){
  std::map<std::string, std::string> dict;
  read_keyvalues(dict, filename);
  return dict;
}

bool parse_kvline(std::string& key, std::string& value, std::string const& line) {
  using namespace boost::xpressive;

  // same as "^\s*(.*?)(#|$)" in PCRE
  sregex regex_filter_comment = bos >> *_s >> (s1 = -*_) >> ('#' | eos);

  // same as "^(.*?)\s*=\s*(.*?)\s*$" in PCRE
  sregex regex_keyvalue = bos >> (s1 = -*_) >> *_s >> '=' >> *_s >> (s2 = -*_) >> *_s >> eos;
  smatch what;

  if (regex_search(line, what, regex_filter_comment)) {
    if (static_cast<std::string>(what[1]).empty()) { return false; }
  } else {
    return false;
  }

  if (regex_match(what[1], what, regex_keyvalue)) {
    key   = what[1];
    value = what[2];
  } else {
    std::string msg("parse_kvline> ERROR: missing '=': ");
    msg += line;
    throw std::runtime_error(msg);
  }
  return true;
}

#define deprecated_parameter(dict, new, old) \
if(dict.count(old)>0){\
  std::cerr << "WARNING: parameter " << old << " is deprecated and will be removed in the next version." << std::endl; \
  std::cerr << "Use " << new << " instead." << std::endl; \
  dict[new] = dict[old];\
}


#endif  // READ_KEYVALUES_H
