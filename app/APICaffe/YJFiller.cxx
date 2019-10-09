#ifndef __YJFILLER_CXX__
#define __YJFILLER_CXX__

#include "YJFiller.h"

namespace larcv {

  static YJFillerProcessFactory __global_YJFillerProcessFactory__;

  YJFiller::YJFiller(const std::string name)
    : ProcessBase(name)
  {}
    
  void YJFiller::configure(const PSet& cfg)
  {}

  void YJFiller::initialize()
  {}

  bool YJFiller::process(IOManager& mgr)
  {}

  void YJFiller::finalize()
  {}

}
#endif
