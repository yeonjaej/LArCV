/**
 * \file YJFiller.h
 *
 * \ingroup Package_Name
 * 
 * \brief Class def header for a class YJFiller
 *
 * @author yz2600
 */

/** \addtogroup Package_Name

    @{*/
#ifndef __YJFILLER_H__
#define __YJFILLER_H__

#include "Processor/ProcessBase.h"
#include "Processor/ProcessFactory.h"
namespace larcv {

  /**
     \class ProcessBase
     User defined class YJFiller ... these comments are used to generate
     doxygen documentation!
  */
  class YJFiller : public ProcessBase {

  public:
    
    /// Default constructor
    YJFiller(const std::string name="YJFiller");
    
    /// Default destructor
    ~YJFiller(){}

    void configure(const PSet&);

    void initialize();

    bool process(IOManager& mgr);

    void finalize();

  };

  /**
     \class larcv::YJFillerFactory
     \brief A concrete factory class for larcv::YJFiller
  */
  class YJFillerProcessFactory : public ProcessFactoryBase {
  public:
    /// ctor
    YJFillerProcessFactory() { ProcessFactory::get().add_factory("YJFiller",this); }
    /// dtor
    ~YJFillerProcessFactory() {}
    /// creation method
    ProcessBase* create(const std::string instance_name) { return new YJFiller(instance_name); }
  };

}

#endif
/** @} */ // end of doxygen group 

