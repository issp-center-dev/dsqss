#include <My_rdm.hrd.h>

My_rdm::My_rdm(long seed) { RND.setSeed(seed, 32); }

My_rdm::~My_rdm() {}

void My_rdm::outgen(std::string const& fname) {
  ofstream fout(fname.c_str(), ios::out | ios::binary);

  fout.write((char*)this, sizeof(My_rdm));
}

void My_rdm::ingen(std::string const& fname) {
  ifstream fin(fname.c_str(), ios::in | ios::binary);

  if (fin)
    fin.read((char*)this, sizeof(My_rdm));
  else {
    cout << "no file!" << endl;
  };
}
