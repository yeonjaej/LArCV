/**
 * \file Binarize.h
 *
 * \ingroup Package_Name
 * 
 * \brief Class def header for a class Binarize
 *
 * @author taritree
 */

/** \addtogroup Package_Name

    @{*/
#ifndef __BINARIZE_H__
#define __BINARIZE_H__

#include "Processor/ProcessBase.h"
#include "Processor/ProcessFactory.h"

#include <vector>
#include <string>

namespace larcv {

  /**
     \class ProcessBase
     User defined class Binarize ... these comments are used to generate
     doxygen documentation!
  */
  class Binarize : public ProcessBase {

  public:
    
    /// Default constructor
    Binarize(const std::string name="Binarize");
    
    /// Default destructor
    ~Binarize(){}

    void configure(const PSet&);

    void initialize();

    bool process(IOManager& mgr);

    void finalize(TFile* ana_file);


    std::vector<float> fChannelThresholds;
    std::string fInputImageProducer;
    std::string fOutputImageProducer;

  };

  /**
     \class larcv::BinarizeFactory
     \brief A concrete factory class for larcv::Binarize
  */
  class BinarizeProcessFactory : public ProcessFactoryBase {
  public:
    /// ctor
    BinarizeProcessFactory() { ProcessFactory::get().add_factory("Binarize",this); }
    /// dtor
    ~BinarizeProcessFactory() {}
    /// creation method
    ProcessBase* create(const std::string instance_name) { return new Binarize(instance_name); }
  };

}

#endif
/** @} */ // end of doxygen group 
