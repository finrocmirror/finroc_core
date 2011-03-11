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
/*!\file    tSenseControlModule.h
 *
 * \author  Tobias Foehst
 * \author  Bernd-Helge Schaefer
 *
 * \date    2010-12-09
 *
 * \brief Contains tSenseControlModule
 *
 * \b tSenseControlModule
 *
 */
//----------------------------------------------------------------------
#ifndef _core__structure__tSenseControlModule_h_
#define _core__structure__tSenseControlModule_h_

#include "core/tFrameworkElement.h"

//----------------------------------------------------------------------
// External includes (system with <>, local with "")
//----------------------------------------------------------------------

//----------------------------------------------------------------------
// Internal includes with ""
//----------------------------------------------------------------------
#include "core/port/tEdgeAggregator.h"
#include "core/port/tPort.h"
#include "core/plugin/tStandardCreateModuleAction.h"

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
class tSenseControlModule : public finroc::core::tFrameworkElement
{
  class ControlTask : public finroc::util::tTask
  {
    tSenseControlModule *const module;
  public:
    ControlTask(tSenseControlModule *module);
    virtual void ExecuteTask();
  };

  class SenseTask : public finroc::util::tTask
  {
    tSenseControlModule *const module;
  public:
    SenseTask(tSenseControlModule *module);
    virtual void ExecuteTask();
  };

  finroc::core::tEdgeAggregator *controller_input;
  finroc::core::tEdgeAggregator *controller_output;
  ControlTask control_task;

  finroc::core::tEdgeAggregator *sensor_input;
  finroc::core::tEdgeAggregator *sensor_output;
  SenseTask sense_task;

//----------------------------------------------------------------------
// Protected methods
//----------------------------------------------------------------------
protected:

  virtual void Control();

  virtual void Sense();

//----------------------------------------------------------------------
// Public methods
//----------------------------------------------------------------------
public:

  template < typename T = double >
  struct tCI : public tPort<T>
  {
    tCI(tSenseControlModule *parent, const finroc::util::tString &name)
        : tPort<T>(name, parent->controller_input, false)
    {}
  };
  template < typename T = double >
  struct tCO : public tPort<T>
  {
    tCO(tSenseControlModule *parent, const finroc::util::tString &name)
        : tPort<T>(name, parent->controller_output, true)
    {}
  };
  template < typename T = double >
  struct tSI : public tPort<T>
  {
    tSI(tSenseControlModule *parent, const finroc::util::tString &name)
        : tPort<T>(name, parent->sensor_input, false)
    {}
  };
  template < typename T = double >
  struct tSO : public tPort<T>
  {
    tSO(tSenseControlModule *parent, const finroc::util::tString &name)
        : tPort<T>(name, parent->sensor_output, true)
    {}
  };

  tSenseControlModule(finroc::core::tFrameworkElement *parent, const finroc::util::tString &name);

};

//----------------------------------------------------------------------
// End of namespace declaration
//----------------------------------------------------------------------
}
}
}

#endif
