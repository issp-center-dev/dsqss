#include <My_rdm.boost.h>

My_rdm::My_rdm(long seed) : gen_d(seed), dst_d(0, 1), rdm_d(gen_d, dst_d) {}

My_rdm::~My_rdm() {}

void My_rdm::outgen(char *fname) {
  ofstream fout;
  fout.open(fname, ios::out | ios::binary);

  fout.write((char *)this, sizeof(My_rdm));
  fout.close();
}

void My_rdm::ingen(char *fname) {
  ifstream fin;
  fin.open(fname, ios::in | ios::binary);

  if (fin)
    fin.read((char *)this, sizeof(My_rdm));
  else {
    cout << "no file!" << endl;
  };
  fin.close();
}

My_rdmi::My_rdmi(long seed, long iMax)
    : gen_i(seed), dst_i(0, iMax - 1), rdm_i(gen_i, dst_i) {}

My_rdmi::~My_rdmi() {}

void My_rdmi::outgen(char *fname) {
  ofstream fout;
  fout.open(fname, ios::out | ios::binary);

  fout.write((char *)this, sizeof(My_rdmi));
  fout.close();
}

void My_rdmi::ingen(char *fname) {
  ifstream fin;
  fin.open(fname, ios::in | ios::binary);

  if (fin)
    fin.read((char *)this, sizeof(My_rdmi));
  else {
    cout << "no file!" << endl;
  };
  fin.close();
}

My_rdms::My_rdms(long seed, short iMax)
    : gen_s(seed), dst_s(0, iMax - 1), rdm_s(gen_s, dst_s) {}

My_rdms::~My_rdms() {}

void My_rdms::outgen(char *fname) {
  ofstream fout;
  fout.open(fname, ios::out | ios::binary);

  fout.write((char *)this, sizeof(My_rdms));
  fout.close();
}

void My_rdms::ingen(char *fname) {
  ifstream fin;
  fin.open(fname, ios::in | ios::binary);

  if (fin)
    fin.read((char *)this, sizeof(My_rdms));
  else {
    cout << "no file!" << endl;
  };
  fin.close();
}
