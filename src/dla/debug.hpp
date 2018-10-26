#ifndef DEBUG_H
#define DEBUG_H

#ifdef DEB
#include <string>
#include <iostream>

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
