#include <My_rdm.hrd.h>

#include <cstdlib>
#include <fstream>
#include <iostream>

// The generator state is stored as the engine's standard textual
// representation instead of a raw byte dump of the object, which had
// unspecified layout and was not portable between platforms.

void My_rdm::outgen(std::string const& fname) {
  std::ofstream fout(fname.c_str());
  fout << mt;
  fout.flush();
  if (!fout) {
    std::cerr << "ERROR: failed to write the RNG state file " << fname
              << std::endl;
  }
}

void My_rdm::ingen(std::string const& fname) {
  std::ifstream fin(fname.c_str());
  if (!fin) {
    std::cerr << "ERROR: RNG state file " << fname
              << " not found; the run cannot be resumed reproducibly."
              << std::endl;
    std::exit(1);
  }
  fin >> mt;
  if (!fin) {
    std::cerr << "ERROR: RNG state file " << fname << " is corrupted."
              << std::endl;
    std::exit(1);
  }
}
