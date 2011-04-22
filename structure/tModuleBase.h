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
#include "core/plugin/tStandardCreateModuleAction.h"
#include "core/structure/tConveniencePort.h"
#include "core/structure/tStructureElementRegister.h"

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
class tModuleBase : public finroc::core::tFrameworkElement, finroc::core::tPortListener<>
{
  template <typename T>
  friend class tConveniencePort;

  /*! Element aggregating parameters */
  finroc::core::tFrameworkElement* parameters;

  /*! Changed flag that is set whenever a parameter change is detected */
  volatile bool parameters_changed;

  /*! Number of ports already created that have auto-generated names */
  int auto_name_port_count;

  /*! Counter should be reset for every module class in type hierarchy. This helper variable is used to detect this. */
  const char* count_for_type;

protected:

  /*! Called whenever parameters have changed */
  virtual void ParametersChanged() {}

public:

  tModuleBase(finroc::core::tFrameworkElement *parent, const finroc::util::tString &name);

  /*!
   * (Should only be called by abstract module classes succh as tModule and tSenseControlModule)
   *
   * Calls ParametersChanged() if a parameter change was detected and resets change flag
   */
  void CheckParameters();

  /*! Implementation of tPortListenerRaw */
  virtual void PortChanged(tAbstractPort* origin, const void* const& value);

  template < typename T = double >
  class tParameter : public finroc::core::tParameter<T>, tConveniencePort<tModuleBase>
  {
  public:

    tParameter(const finroc::util::tString& description, const util::tString& config_entry = "")
        : finroc::core::tParameter<T>(description, this->FindParent()->parameters, config_entry)
    {
      this->AddPortListener(this->FindParent());
      this->UpdateCurrentPortNameIndex();
    }

    tParameter(const util::tString& description, const T& default_value, tUnit* unit = &(tUnit::cNO_UNIT), const util::tString& config_entry = "")
        : finroc::core::tParameter<T>(description, this->FindParent()->parameters, default_value, unit, config_entry)
    {
      this->AddPortListener(this->FindParent());
      this->UpdateCurrentPortNameIndex();
    }

    template < typename Q = T >
    tParameter(const util::tString& description, const T& default_value, typename boost::enable_if_c<tPortTypeMap<Q>::boundable, tBounds<T> >::type b, tUnit* unit = &(tUnit::cNO_UNIT), const util::tString& config_entry = "")
        : finroc::core::tParameter<T>(description, this->FindParent()->parameters, default_value, b, unit, config_entry)
    {
      this->AddPortListener(this->FindParent());
      this->UpdateCurrentPortNameIndex();
    }

    tParameter(const char* description, const util::tString& config_entry = "")
        : finroc::core::tParameter<T>(description, this->FindParent()->parameters, config_entry)
    {
      this->AddPortListener(this->FindParent());
      this->UpdateCurrentPortNameIndex();
    }

    tParameter(const char* description, const T& default_value, tUnit* unit = &(tUnit::cNO_UNIT), const util::tString& config_entry = "")
        : finroc::core::tParameter<T>(description, this->FindParent()->parameters, default_value, unit, config_entry)
    {
      this->AddPortListener(this->FindParent());
      this->UpdateCurrentPortNameIndex();
    }

    template < typename Q = T >
    tParameter(const char* description, const T& default_value, typename boost::enable_if_c<tPortTypeMap<Q>::boundable, tBounds<T> >::type b, tUnit* unit = &(tUnit::cNO_UNIT), const util::tString& config_entry = "")
        : finroc::core::tParameter<T>(description, this->FindParent()->parameters, default_value, b, unit, config_entry)
    {
      this->AddPortListener(this->FindParent());
      this->UpdateCurrentPortNameIndex();
    }

    tParameter(const util::tString& config_entry = "")
        : finroc::core::tParameter<T>(this->GetPortName(), this->FindParent()->parameters, config_entry)
    {
      this->AddPortListener(this->FindParent());
    }

    tParameter(const T& default_value, tUnit* unit = &(tUnit::cNO_UNIT), const util::tString& config_entry = "")
        : finroc::core::tParameter<T>(this->GetPortName(), this->FindParent()->parameters, default_value, unit, config_entry)
    {
      this->AddPortListener(this->FindParent());
    }

    template < typename Q = T >
    tParameter(const T& default_value, typename boost::enable_if_c<tPortTypeMap<Q>::boundable, tBounds<T> >::type b, tUnit* unit = &(tUnit::cNO_UNIT), const util::tString& config_entry = "")
        : finroc::core::tParameter<T>(this->GetPortName(), this->FindParent()->parameters, default_value, b, unit, config_entry)
    {
      this->AddPortListener(this->FindParent());
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
