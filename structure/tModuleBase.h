//
// You received this file as part of Finroc
// A framework for innovative robot control
//
// Copyright (C) AG Robotersysteme TU Kaiserslautern
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
//
//----------------------------------------------------------------------
/*!\file    tModuleBase.h
 *
 * \author  Max Reichardt
 *
 * \date    2011-04-12
 *
 * \brief Contains tModuleBase
 *
 * \b tModuleBase
 *
 */
//----------------------------------------------------------------------
#ifndef _core__structure__tModuleBase_h_
#define _core__structure__tModuleBase_h_

#include "core/tFrameworkElement.h"

//----------------------------------------------------------------------
// External includes (system with <>, local with "")
//----------------------------------------------------------------------

//----------------------------------------------------------------------
// Internal includes with ""
//----------------------------------------------------------------------
#include "core/parameter/tParameter.h"
#include "core/parameter/tStaticParameter.h"
#include "core/plugin/tStandardCreateModuleAction.h"
#include "core/structure/tConveniencePort.h"
#include "core/structure/tStructureElementRegister.h"
#include "core/port/tPortGroup.h"

//----------------------------------------------------------------------
// Debugging
//----------------------------------------------------------------------

//----------------------------------------------------------------------
// Namespace declaration
//----------------------------------------------------------------------
namespace finroc
{
namespace core
{
namespace structure
{

//----------------------------------------------------------------------
// Forward declarations / typedefs / enums
//----------------------------------------------------------------------

//----------------------------------------------------------------------
// Class declaration
//----------------------------------------------------------------------
//!
/*!
 * Base class for the different module types.
 * It contains common functionality.
 */
class tModuleBase : public finroc::core::tFrameworkElement
{
  template <typename T>
  friend class tConveniencePortBase;

  /*! Introduced this helper class to remove ambiguities when derived classes add listeners to ports */
  class tParameterChangeDetector : public finroc::core::tPortListener<>
  {
    friend class tModuleBase;

    /*! Changed flag that is set whenever a parameter change is detected */
    volatile bool parameters_changed;

    /*! Implementation of tPortListenerRaw */
    virtual void PortChanged(tAbstractPort* origin, const void* const& value);

    tParameterChangeDetector() : parameters_changed(true) {}
  };

  /*! Element aggregating parameters */
  finroc::core::tFrameworkElement* parameters;

  /*! Changed flag that is set whenever a parameter change is detected */
  tParameterChangeDetector parameters_changed;

  /*! Number of ports already created that have auto-generated names */
  int auto_name_port_count;

  /*! Counter should be reset for every module class in type hierarchy. This helper variable is used to detect this. */
  const char* count_for_type;

protected:

  /*! Called whenever parameters have changed */
  virtual void ParametersChanged() {}

  /*!
   * Has any port in edge aggregator changed?
   * (should only be called by module classes such as tModule and tSenseControlModule)
   *
   * \param ea Edge aggregator to check
   */
  bool HasAnyPortChanged(tEdgeAggregator* ea);

  /*!
   * Reset changed flags of all ports
   * (should only be called by module classes such as tModule and tSenseControlModule)
   *
   * \param ea Edge aggregator containing ports to be reset
   */
  void ResetChangedFlags(tEdgeAggregator* ea);

public:

  tModuleBase(finroc::core::tFrameworkElement *parent, const finroc::util::tString &name);

  virtual ~tModuleBase();

  void* operator new(size_t size)
  {
    void* result = ::operator new(size);
    tStructureElementRegister::AddMemoryBlock(result, size);
    return result;
  }

  void* operator new[](size_t size)
  {
    assert(false && "Allocating (non-pointer) array of framework elements is not allowed.");
    throw std::bad_alloc();
  }

  /*!
   * (Should only be called by abstract module classes succh as tModule and tSenseControlModule)
   *
   * Calls ParametersChanged() if a parameter change was detected and resets change flag
   */
  void CheckParameters();

  template < typename T = double >
  class tParameter : public tConveniencePort < T, tModuleBase, finroc::core::tParameter<T> >
  {
  public:
    template<typename ... ARGS>
    explicit tParameter(const ARGS&... args)
        : tConveniencePort < T, tModuleBase, finroc::core::tParameter<T>>(0u, GetContainer, args...)
    {
      assert(this->GetWrapped()->GetParent()->DescriptionEquals("Parameters"));
      this->AddPortListener(&static_cast<tModuleBase*>(this->GetWrapped()->GetParent()->GetParent())->parameters_changed);
    }

  private:
    static tFrameworkElement* GetContainer(tModuleBase* module)
    {
      return module->parameters;
    }
  };


  template < typename T = double >
  class tStaticParameter : public tConveniencePort < T, tModuleBase, finroc::core::tStaticParameter<T> >
  {
  public:
    template<typename ... ARGS>
    explicit tStaticParameter(const ARGS&... args)
        : tConveniencePort < T, tModuleBase, finroc::core::tStaticParameter<T> >(0u, GetContainer, args...)
    {
    }

  private:
    static tFrameworkElement* GetContainer(tModuleBase* module)
    {
      return module;
    }
  };

};

//----------------------------------------------------------------------
// End of namespace declaration
//----------------------------------------------------------------------
}
}
}

#endif
