#ifndef PARAMETER_H
#define PARAMETER_H

#include <cmath>
#include <string>
#include <cstring>
#include <algorithm>
#include <fstream>
#include <iostream>
#include <cctype>
#include <cstdio>
#include <map>
#include <exception>

#include <boost/lexical_cast.hpp>
#include <boost/math/special_functions/fpclassify.hpp>

#include "debug.hpp"
#include "util.hpp"
#include "../common/read_keyvalues.h"

#define PNUM 22

class Parameter {
private:

public:
  Parameter(int, char**);
  void dump(FILE*);
  void dump();
  void openfile() {
    FOUT = fopen(OUTFILE.c_str(), "w");

    if (DISPFILE.length() > 0) {
      FOUT4CF = fopen(CFOUTFILE.c_str(), "w");
      if (!FOUT4CF) {
        std::string msg("cannot open cfoutfile: ");
        msg += CFOUTFILE;
        util::ERROR(msg.c_str());
      }
    } else {
      FOUT4CF = NULL;
    }
    if (WVFILE.length() > 0) {
      FOUT4SF = fopen(SFOUTFILE.c_str(), "w");
      if (!FOUT4SF) {
        std::string msg("cannot open sfoutfile: ");
        msg += SFOUTFILE;
        util::ERROR(msg.c_str());
      }
    } else {
      FOUT4SF = NULL;
    }
    if (WVFILE.length() > 0) {
      FOUT4CK = fopen(CKOUTFILE.c_str(), "w");
      if (!FOUT4CK) {
        std::string msg("cannot open ckoutfile: ");
        msg += CKOUTFILE;
        util::ERROR(msg.c_str());
      }
    } else {
      FOUT4CK = NULL;
    }
  }
  void closefile() {
    if (FOUT) { fclose(FOUT); }
    if (FOUT4CF) { fclose(FOUT4CF); }
    if (FOUT4SF) { fclose(FOUT4SF); }
    if (FOUT4CK) { fclose(FOUT4CK); }
  }

  void readfile(std::string const &filename);

  void init(std::map<std::string, std::string> &dict);

  double BETA;  // inversed temperature

  int RUNTYPE;  // runtype  number
  int NSET;     // number of sets
  int NPRE;    // number of sweeps for determining NCYC
  int NTHERM;    // number of sweeps for initial thermalization
  int NDECOR;   // number of sweeps for decorrelating two subsequent sets
  int NMCS;     // number of sweeps for measurement in a set
  int SEED;     // seed for the random number generator
  int NSEGMAX;  // maximum allowed number of segments for dla or worms for pmwa
  int NVERMAX;  // maximum allowed number of vertices

  int NTAU;

  double SIMTIME;  // time [sec] to save a snapshot and stop simulation
  // if <= 0, never stop until finish

  std::string ALGFILE;  // algorithm file name
  std::string LATFILE;  // lattice file name
  std::string OUTFILE;  // output file name
  std::string WVFILE; // wavevector file name
  std::string DISPFILE; // displacement file name
  int NCYC;            // number of cycles in a sweep (not provided from the file)
  FILE* FOUT;  // file handler for the output file

  std::string CFOUTFILE;  // output file name
  FILE* FOUT4CF;         // file handler for the output file

  std::string SFOUTFILE;  // output file name
  FILE* FOUT4SF;         // file handler for the output file

  std::string CKOUTFILE;  // output file name
  FILE* FOUT4CK;         // file handler for the output file

  int save_set_obs;   // if true, save observables of each set
};

void Parameter::readfile(std::string const& filename) {
  using std::string;
  using boost::lexical_cast;
  double val;
  std::map<string, string> dict;
  init(dict);

  read_keyvalues(dict, filename);

  deprecated_parameter(dict, "npre", "nmcse");
  deprecated_parameter(dict, "ntherm", "nmcsd");

  BETA = lexical_cast<double>(dict["beta"]);
  if(boost::math::isinf(BETA) || BETA <= 0.0){
    util::ERROR("\"beta\" is not specified or invalid.");
  }

  NMCS = lexical_cast<int>(dict["nmcs"]);
  NTHERM = lexical_cast<int>(dict["ntherm"]);
  NPRE = lexical_cast<int>(dict["npre"]);
  if(dict.find("ndecor") != dict.end()){
    NDECOR = lexical_cast<int>(dict["ndecor"]);
  }else{
    NDECOR = NTHERM;
  }
  NSET = lexical_cast<int>(dict["nset"]);
  SIMTIME = lexical_cast<double>(dict["simulationtime"]);
  SEED = lexical_cast<int>(dict["seed"]);
  NSEGMAX = lexical_cast<int>(dict["nsegmax"]);
  NVERMAX = lexical_cast<int>(dict["nvermax"]);

  NTAU = lexical_cast<int>(dict["ntau"]);

  ALGFILE = dict["algfile"];
  LATFILE = dict["latfile"];
  WVFILE = dict["wvfile"];
  DISPFILE = dict["dispfile"];
  OUTFILE = dict["outfile"];
  SFOUTFILE = dict["sfoutfile"];
  CFOUTFILE = dict["cfoutfile"];
  CKOUTFILE = dict["ckoutfile"];
  save_set_obs = lexical_cast<int>(dict["savesetobs"]);

  RUNTYPE = lexical_cast<int>(dict["runtype"]);
};

inline Parameter::Parameter(int NP, char** PLIST) {
  AutoDebugDump("Parameter::Parameter");

  if (NP < 2) {
    std::string msg("no parameter file");
    util::ERROR(msg.c_str());
  }

  readfile(PLIST[1]);

  if (RUNTYPE == 1 || RUNTYPE == 2) {
    std::string msg("replica exchange method is disabled in this version.");
    util::ERROR(msg.c_str());
  }

#ifdef MULTI
  if (RUNTYPE >= 3) {  // runtype == 3 --> each processor must read its own parameter file
    char FILENAME[128];
    sprintf(FILENAME, "%s.%03d", PLIST[1], I_PROC);
    printf("[%2d] input file name = %s\n", I_PROC, FILENAME);
    std::ifstream ifs2(FILENAME);
    if (ifs2.fail()) {
      printf("no such File: %s\n", FILENAME);
      exit(1);
    }
    readfile(FILENAME);
  }
#endif

  if(I_PROC==0){
    dump();
  }

  if (RUNTYPE == 0) SEED += I_PROC;
  NCYC = 0;
}

void Parameter::init(std::map<std::string, std::string>& dict) {
  dict.clear();
  dict["beta"]           = "inf";
  dict["nmcs"]           = "1000";
  dict["nset"]           = "10";
  dict["npre"]           = "1000";
  dict["ntherm"]         = "1000";
  dict["simulationtime"] = "0.0";
  dict["seed"]           = "198212240";
  dict["nvermax"]        = "10000";
  dict["nsegmax"]        = "10000";
  dict["ntau"]           = "10";
  dict["algfile"]        = "algorithm.xml";
  dict["latfile"]        = "lattice.xml";
  dict["wvfile"]         = "wavevector.xml";
  dict["dispfile"]       = "displacement.xml";
  dict["outfile"]        = "sample.log";
  dict["sfoutfile"]      = "sf.dat";
  dict["cfoutfile"]      = "cf.dat";
  dict["ckoutfile"]      = "ck.dat";
  dict["savesetobs"]     = "0";
  dict["runtype"]        = "0";
}

inline void Parameter::dump() {
  using std::cout;
  using std::endl;

  cout << endl << "+++++++++ input data +++++++++" << endl;
  cout << "RUNTYPE = " << RUNTYPE << endl << endl;
  cout << "NSET    = " << NSET << endl;
  cout << "NTHERM  = " << NTHERM << endl;
  cout << "NMCS    = " << NMCS << endl;
  cout << "SEED    = " << SEED << endl;
  cout << "NVERMAX = " << NVERMAX << endl;
  cout << "NSEGMAX = " << NSEGMAX << endl << endl;
  cout << "BETA    = " << BETA << endl;
  cout << "NTAU    = " << NTAU << endl;
  cout << "ALGFILE = " << ALGFILE << endl;
  cout << "LATFILE = " << LATFILE << endl;
  cout << "WVFILE = " << WVFILE << endl;
  cout << "DISPFILE = " << DISPFILE << endl;
  cout << "OUTFILE = " << OUTFILE << endl;
  cout << "CFOUTFILE  = " << CFOUTFILE << endl;
  cout << "SFOUTFILE  = " << SFOUTFILE << endl;
  cout << "CKOUTFILE  = " << CKOUTFILE << endl;
  cout << "SAVESETOBS = " << save_set_obs << endl;
  cout << "SIMULATIONTIME   = " << SIMTIME << endl;

  cout << "+++++++++ input data +++++++++" << endl;
}
inline void Parameter::dump(FILE* F) {
  fprintf(F, "P NSET    = %12d\n", NSET);
  fprintf(F, "P NPRE    = %12d\n", NPRE);
  fprintf(F, "P NTHERM  = %12d\n", NTHERM);
  fprintf(F, "P NDECOR  = %12d\n", NDECOR);
  fprintf(F, "P NMCS    = %12d\n", NMCS);
  fprintf(F, "P SEED    = %12d\n", SEED);
  fprintf(F, "P NSEGMAX = %12d\n", NSEGMAX);
  fprintf(F, "P NVERMAX = %12d\n", NVERMAX);
  fprintf(F, "P BETA    = %.12lf\n", BETA);
  fprintf(F, "P NTAU    = %12d\n", NTAU);
  fprintf(F, "P NCYC    = %12d\n", NCYC);
  fprintf(F, "P ALGFILE = %s\n", ALGFILE.c_str());
  fprintf(F, "P LATFILE = %s\n", LATFILE.c_str());
  fprintf(F, "P WVFILE = %s\n", WVFILE.c_str());
  fprintf(F, "P DISPFILE  = %s\n", DISPFILE.c_str());

  fprintf(F, "P OUTFILE    = %s\n", OUTFILE.c_str());
  fprintf(F, "P CFOUTFILE  = %s\n", CFOUTFILE.c_str());
  fprintf(F, "P SFOUTFILE  = %s\n", SFOUTFILE.c_str());
  fprintf(F, "P CKOUTFILE  = %s\n", CKOUTFILE.c_str());
  fprintf(F, "P SAVESETOBS = %d\n", save_set_obs);
  fprintf(F, "P SIMULATIONTIME   = %12lf\n", SIMTIME);
}
#endif
