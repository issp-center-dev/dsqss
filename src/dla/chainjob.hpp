#ifndef CHAINJOB_HPP
#define CHAINJOB_HPP

// BINARY FILE
#include <fstream>
#include <cstring>
#include <map>

#include <boost/lexical_cast.hpp>

#include "../common/timer.hpp"
#include "dla.hpp"

void Simulation::BinaryIO() {
  CJOBFILE = P.OUTFILE + std::string(".") + tostring(I_PROC) + std::string(".cjob");
  cjobin.open(CJOBFILE.c_str(), std::ios::in | std::ios::binary);

  if (!cjobin) {
    cjobin.close();
    isChainjob = false;
    isEnd      = true;  // isEnd becomes "false" after setNCYC fuction;
    cjobout.open(CJOBFILE.c_str(), std::ios::out | std::ios::binary);
    Serialize::save(cjobout, isEnd);
    cjobout.close();
  } else {
    isChainjob = true;
    load();
  }
};

//-----------------------------------------------------------------------------------------

void Simulation::save() {
  using Serialize::save;
  cjobout.open(CJOBFILE.c_str(), std::ios::out | std::ios::binary);

  save(cjobout, isEnd);
  save(cjobout, P.NCYC);
  save(cjobout, ISET);
  save(cjobout, IMCSD);
  save(cjobout, IMCS);

  save(cjobout, RND);

  int NSEG_ = P.NSEGMAX - TheSegmentPool.count();
  save(cjobout, NSEG_);

  int NSEG_count = 0;
  for (int site_ = 0; site_ < LAT.NSITE; site_++) {
    int nsegment_site = LAT.S(site_).count();
    save(cjobout, nsegment_site);

    Site::iterator itp(LAT.S(site_));
    while (!(++itp).atOrigin()) {
      Segment &S = *itp;
      int ID_    = S.id();
      int VID_0  = S.V(0).id();
      int VID_1  = S.V(1).id();
      int val_   = S.X();

      save(cjobout, ID_);
      save(cjobout, VID_0);
      save(cjobout, VID_1);
      save(cjobout, val_);
      ++NSEG_count;
    }
  }
  if ((NSEG_) != NSEG_count) {
    std::cout << "ERROR: We can't keep all segments." << std::endl;
    end_job();
  }

  int NVER_ = P.NVERMAX - TheVertexPool.count() - LAT.NSITE - 1;  //-1 means warm's tail
  save(cjobout, NVER_);
  int NVER_count = 0;
  for (int interaction_ = 0; interaction_ < LAT.NINT; interaction_++) {
    int nvertex_interaction = LAT.I(interaction_).count();
    save(cjobout, nvertex_interaction);
    Interaction::iterator itp(LAT.I(interaction_));
    while (!(++itp).atOrigin()) {
      Vertex &V    = *itp;
      int ID_      = V.id();
      double TIME_ = V.time();
      int VPID_    = V.type();
      int NLEG_    = V.NLEG();

      save(cjobout, ID_);
      save(cjobout, TIME_);
      save(cjobout, VPID_);
      save(cjobout, NLEG_);

      for (int leg = 0; leg < NLEG_; leg++) {
        int SID_ = V.S(leg).id();
        save(cjobout, SID_);
      }
      ++NVER_count;
    }
    //    }
  }
  if ((NVER_) != NVER_count) {
    std::cout << "ERROR: We can't keep all vertices." << std::endl;
    end_job();
  }

  int NVER_term = LAT.NSITE;
  save(cjobout, NVER_term);
  NVER_count = 0;
  for (int site_ = 0; site_ < LAT.NSITE; site_++) {
    Vertex &V    = LAT.S(site_).getVterm();
    int ID_      = V.id();
    double TIME_ = V.time();
    int VPID_    = V.type();
    int NLEG_    = V.NLEG();

    save(cjobout, ID_);
    save(cjobout, TIME_);
    save(cjobout, VPID_);
    save(cjobout, NLEG_);

    for (int leg = 0; leg < NLEG_; leg++) {
      int SID_ = V.S(leg).id();
      save(cjobout, SID_);
    }
    ++NVER_count;
  }  //terminal vertices
  if (NVER_term != NVER_count) {
    std::cout << "ERROR: We can't keep all vertices." << std::endl;
    end_job();
  }

  int NVER_used = TheVertexPool.number_of_used_elements();
  save(cjobout, NVER_used);
  int NSEG_used = TheSegmentPool.number_of_used_elements();
  save(cjobout, NSEG_used);

  MSR.save(cjobout);
  sf.save(cjobout);
  cf.save(cjobout);
  ck.save(cjobout);

  cjobout.close();
};

void Simulation::load() {
  using Serialize::load;

  std::map<int, std::pair<int, int> > newID2V;  //first:new segmentID, second: old vertexIDs
  std::map<int, Segment *> oldID_S;             //first:old segmentID, second:new segment address
  std::map<int, Vertex *> oldID_V;              //first:old vertexID, second:new vertex address

  load(cjobin, isEnd);
  if (isEnd) {
    std::cout << "This simulation has been finished or suspended before setting Ncycle" << std::endl;
    cjobin.close();
    cjobout.open(CJOBFILE.c_str(), std::ios::out | std::ios::binary);
    end_cjob();
  }

  load(cjobin, P.NCYC);
  load(cjobin, ISETstart);
  load(cjobin, IMCSDstart);
  load(cjobin, IMCSstart);

  load(cjobin, RND);

  int NSEG_      = load<int>(cjobin);
  int NSEG_count = 0;
  for (int site_ = 0; site_ < LAT.NSITE; site_++) {
    int nsegment_site = load<int>(cjobin);
    Segment &S0       = LAT.S(site_).first();
    for (int nseg_ = 1; nseg_ < nsegment_site; nseg_++) {
      Segment &S1 = TheSegmentPool.pop();
      S1.BareSegment::init();
      S1.setONSITE(&LAT.S(site_));
      S0.insert_after(S1);
    }

    Site::iterator itp(LAT.S(site_));
    while (!(++itp).atOrigin()) {
      Segment &S = *itp;
      int ID_    = load<int>(cjobin);
      oldID_S.insert(std::map<int, Segment *>::value_type(ID_, (&S)));

      int VID_0 = load<int>(cjobin);

      int VID_1 = load<int>(cjobin);

      std::pair<int, int> VID2(VID_0, VID_1);
      newID2V.insert(std::map<int, std::pair<int, int> >::value_type(S.id(), VID2));

      int val_ = load<int>(cjobin);
      S.BareSegment::setX(val_);
      ++NSEG_count;
    }
  }
  if ((NSEG_) != NSEG_count) {
    std::cout << "ERROR: We can't read all segments." << std::endl;
    end_job();
  }

  int NVER_      = load<int>(cjobin);
  int NVER_count = 0;
  for (int interaction_ = 0; interaction_ < LAT.NINT; interaction_++) {
    int nvertex_interaction = load<int>(cjobin);
    Vertex &V0              = LAT.I(interaction_).first();
    for (int nver_ = 0; nver_ < nvertex_interaction; nver_++) {
      Vertex &V1 = TheVertexPool.pop();
      V1.setONINTERACTION(&LAT.I(interaction_));
      V0.insert_after(V1);
    }
    Interaction::iterator itp(LAT.I(interaction_));
    while (!(++itp).atOrigin()) {
      Vertex &V = *itp;
      int ID_   = load<int>(cjobin);
      oldID_V.insert(std::map<int, Vertex *>::value_type(ID_, (&V)));

      double TIME_ = load<double>(cjobin);
      int VPID_    = load<int>(cjobin);

      if (VPID_ == VTYPE::UNDEF) {
        std::cout << "VPID_<------------------" << VPID_ << std::endl;
      } else {
        V.BareVertex::init(TIME_, ALG.getVertexProperty(VPID_));
      }
      int NLEG_ = load<int>(cjobin);
      if (VPID_ == VTYPE::UNDEF) { std::cout << "NLEG_" << NLEG_ << std::endl; };
      for (int leg = 0; leg < NLEG_; leg++) {
        int SID_          = load<int>(cjobin);
        Segment *findingS = (oldID_S.find(SID_))->second;
        V.setS(leg, (*findingS));
      }
      ++NVER_count;
    }
  }
  if ((NVER_) != NVER_count) {
    std::cout << "ERROR: We can't read all vertices." << std::endl;
    end_job();
  }

  int NVER_term = load<int>(cjobin);
  NVER_count    = 0;
  for (int site_ = 0; site_ < LAT.NSITE; site_++) {
    Vertex &V = LAT.S(site_).getVterm();
    int ID_   = load<int>(cjobin);
    oldID_V.insert(std::map<int, Vertex *>::value_type(ID_, (&V)));

    double TIME_ = load<double>(cjobin);
    int VPID_    = load<int>(cjobin);
    if (VPID_ != VTYPE::UNDEF) {
      std::cout << "ERROR: We don't read vertices which should be Terminal." << std::endl;
      end_job();
    }
    int NLEG_ = load<int>(cjobin);
    for (int leg = 0; leg < NLEG_; leg++) {
      int SID_          = load<int>(cjobin);
      Segment *findingS = (oldID_S.find(SID_))->second;
      V.setS(leg, (*findingS));
    }
    ++NVER_count;
  }  //terminal vertices
  if (NVER_term != NVER_count) {
    std::cout << "ERROR: We can't read all terminal vertices." << std::endl;
    end_job();
  }

  //set Segment._v
  for (int site_ = 0; site_ < LAT.NSITE; site_++) {
    Site::iterator itp(LAT.S(site_));
    while (!(++itp).atOrigin()) {
      Segment &S = *itp;
      int VID_0  = (newID2V.find(S.id())->second).first;
      int VID_1  = (newID2V.find(S.id())->second).second;
      Vertex *V0 = (oldID_V.find(VID_0))->second;
      Vertex *V1 = (oldID_V.find(VID_1))->second;
      S.BareSegment::setBottom((*V0));
      S.BareSegment::setTop((*V1));
    }
  }

  int NVER_used = load<int>(cjobin);
  TheVertexPool.set_n_of_used_elements(NVER_used);
  int NSEG_used = load<int>(cjobin);
  TheSegmentPool.set_n_of_used_elements(NSEG_used);

  MSR.load(cjobin);
  sf.load(cjobin);
  cf.load(cjobin);
  ck.load(cjobin);

  cjobin.close();
  oldID_V.clear();
  oldID_S.clear();
  newID2V.clear();
};
//-----------------------------------------------------------------------------------------
void Simulation::end_cjob() {
  cjobout.close();
  cjobout.open(CJOBFILE.c_str(), std::ios::out | std::ios::binary);  //reset
  Serialize::save(cjobout, isEnd);
  cjobout.close();
  std::cout << "Save checkpoint file and stop the simulation." << std::endl;
  end_job();
};
//-----------------------------------------------------------------------------------------
void Simulation::end_job() {
  std::cout << cjob_timer.elapsed() << " seconds have passed." << std::endl;

#ifdef MULTI
  MPI_Barrier(MPI_COMM_WORLD);
  MPI_Finalize();
#endif
  exit(0);
};

#endif  // CHAINJOB_HPP
