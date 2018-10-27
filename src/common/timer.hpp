#ifndef TIMER_HPP
#define TIMER_HPP

#include <boost/config.hpp>

#ifdef BOOST_NO_CXX11_HDR_CHRONO
#define BOOST_CHRONO_HEADER_ONLY
#include <boost/chrono.hpp>
namespace CHRONO = boost::chrono;
#else
#include <chrono>
namespace CHRONO = std::chrono;
#endif

class Timer {
  CHRONO::high_resolution_clock::time_point start;

public:
  Timer():start(CHRONO::high_resolution_clock::now()) {}
  void reset() {start = CHRONO::high_resolution_clock::now(); }
  double elapsed() const {
    CHRONO::high_resolution_clock::time_point end = CHRONO::high_resolution_clock::now();
    return CHRONO::duration_cast<CHRONO::microseconds>(end-start).count()*1.0e-6;
  }
};
#endif  // TIMER_HPP
