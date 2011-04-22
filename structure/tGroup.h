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
/*!\file    tGroup.h
 *
 * \author  Tobias Foehst
 * \author  Bernd-Helge Schaefer
 * \author  Max Reichardt
 *
 * \date    2010-12-09
 *
 * \brief Contains tGroup
 *
 * \b tGroup
 *
 */
//----------------------------------------------------------------------
#ifndef _core__tGroup_h_
#define _core__tGroup_h_

#include "core/finstructable/tFinstructableGroup.h"

//----------------------------------------------------------------------
// External includes (system with <>, local with "")
//----------------------------------------------------------------------

//----------------------------------------------------------------------
// Internal includes with ""
//----------------------------------------------------------------------
#include "core/port/tEdgeAggregator.h"
#include "core/port/tPort.h"
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
 *
 */
class tGroup : public tFinstructableGroup
{
  template <typename T>
  friend class tConveniencePort;

  finroc::core::tEdgeAggregator* controller_input;
  finroc::core::tEdgeAggregator* controller_output;

  finroc::core::tEdgeAggregator* sensor_input;
  finroc::core::tEdgeAggregator* sensor_output;

  /*! Number of ports already created that have auto-generated names */
  int auto_name_port_count;

  /*! Counter should be reset for every module class in type hierarchy. This helper variable is used to detect this. */
  const char* count_for_type;

//----------------------------------------------------------------------
// Public methods
//----------------------------------------------------------------------
public:

  template < typename T = double >
  struct tControllerInput : public tPort<T>, tConveniencePort<tGroup>
  {
    tControllerInput()
        : tPort<T>(this->GetPortName(), this->FindParent()->controller_input, tPortFlags::cINPUT_PROXY)
    {}

    tControllerInput(const finroc::util::tString &name)
        : tPort<T>(tPortCreationInfo(name, this->FindParent()->controller_input, tPortFlags::cINPUT_PROXY))
    {
      this->UpdateCurrentPortNameIndex();
    }
  };
  template < typename T = double >
  struct tControllerOutput : public tPort<T>, tConveniencePort<tGroup>
  {
    tControllerOutput()
        : tPort<T>(this->GetPortName(), this->FindParent()->controller_output, tPortFlags::cOUTPUT_PROXY)
    {}

    tControllerOutput(const finroc::util::tString &name)
        : tPort<T>(tPortCreationInfo(name, this->FindParent()->controller_output, tPortFlags::cOUTPUT_PROXY))
    {
      this->UpdateCurrentPortNameIndex();
    }
  };
  template < typename T = double >
  struct tSensorInput : public tPort<T>, tConveniencePort<tGroup>
  {
    tSensorInput()
        : tPort<T>(this->GetPortName(), this->FindParent()->sensor_input, tPortFlags::cINPUT_PROXY)
    {}

    tSensorInput(const finroc::util::tString &name)
        : tPort<T>(tPortCreationInfo(name, this->FindParent()->sensor_input, tPortFlags::cINPUT_PROXY))
    {
      this->UpdateCurrentPortNameIndex();
    }
  };
  template < typename T = double >
  struct tSensorOutput : public tPort<T>, tConveniencePort<tGroup>
  {
    tSensorOutput()
        : tPort<T>(this->GetPortName(), this->FindParent()->sensor_output, tPortFlags::cOUTPUT_PROXY)
    {}

    tSensorOutput(const finroc::util::tString &name)
        : tPort<T>(tPortCreationInfo(name, this->FindParent()->sensor_output, tPortFlags::cOUTPUT_PROXY))
    {
      this->UpdateCurrentPortNameIndex();
    }
  };

  tGroup(finroc::core::tFrameworkElement *parent, const finroc::util::tString &name, const finroc::util::tString &structure_config_file);

//  void SetParameter(size_t index, const finroc::util::tString &new_value);
//  void SetParameter(const finroc::util::tString &name, const finroc::util::tString &new_value);

};

//----------------------------------------------------------------------
// End of namespace declaration
//----------------------------------------------------------------------
}
}
}

#endif
