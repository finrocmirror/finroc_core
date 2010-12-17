//
// You received this file as part of Finroc
// A framework for integrated robot control
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
/*!\file    tSenseControlModule.cpp
 *
 * \author  Tobias Foehst
 * \author  Bernd-Helge Schaefer
 *
 * \date    2010-12-09
 *
 */
//----------------------------------------------------------------------
#include "core/structure/tSenseControlModule.h"

//----------------------------------------------------------------------
// External includes (system with <>, local with "")
//----------------------------------------------------------------------

//----------------------------------------------------------------------
// Internal includes with ""
//----------------------------------------------------------------------
#include "core/thread/tPeriodicFrameworkElementTask.h"

//----------------------------------------------------------------------
// Debugging
//----------------------------------------------------------------------
#include <cassert>

//----------------------------------------------------------------------
// Namespace usage
//----------------------------------------------------------------------
using namespace finroc::core::structure;

//----------------------------------------------------------------------
// Forward declarations / typedefs / enums
//----------------------------------------------------------------------

//----------------------------------------------------------------------
// Const values
//----------------------------------------------------------------------

//----------------------------------------------------------------------
// Implementation
//----------------------------------------------------------------------

//----------------------------------------------------------------------
// tSenseControlModule constructors
//----------------------------------------------------------------------
tSenseControlModule::tSenseControlModule(tFrameworkElement *parent, const util::tString &name)
    : tFrameworkElement(parent, name),

    controller_input(new tEdgeAggregator(this, "Controller Input", tEdgeAggregator::cIS_INTERFACE | tEdgeAggregator::cCONTROLLER_DATA)),
    controller_output(new tEdgeAggregator(this, "Controller Output", tEdgeAggregator::cIS_INTERFACE | tEdgeAggregator::cCONTROLLER_DATA)),
    control_task(this),

    sensor_input(new tEdgeAggregator(this, "Sensor Input", tEdgeAggregator::cIS_INTERFACE | tEdgeAggregator::cSENSOR_DATA)),
    sensor_output(new tEdgeAggregator(this, "Sensor Output", tEdgeAggregator::cIS_INTERFACE | tEdgeAggregator::cSENSOR_DATA)),
    sense_task(this)
{
  this->AddAnnotation(new tPeriodicFrameworkElementTask(this->controller_input, this->controller_output, &this->control_task));
  this->AddAnnotation(new tPeriodicFrameworkElementTask(this->sensor_input, this->sensor_output, &this->sense_task));
}

//----------------------------------------------------------------------
// tSenseControlModule Control
//----------------------------------------------------------------------
void tSenseControlModule::Control()
{}

//----------------------------------------------------------------------
// tSenseControlModule Sense
//----------------------------------------------------------------------
void tSenseControlModule::Sense()
{}

//----------------------------------------------------------------------
// tSenseControlModule::ControlTask constructors
//----------------------------------------------------------------------
tSenseControlModule::ControlTask::ControlTask(tSenseControlModule *module)
    : module(module)
{}

//----------------------------------------------------------------------
// tSenseControlModule::ControlTask ExecuteTask
//----------------------------------------------------------------------
void tSenseControlModule::ControlTask::ExecuteTask()
{
  this->module->Control();
}

//----------------------------------------------------------------------
// tSenseControlModule::SenseTask constructors
//----------------------------------------------------------------------
tSenseControlModule::SenseTask::SenseTask(tSenseControlModule *module)
    : module(module)
{}

//----------------------------------------------------------------------
// tSenseControlModule::ControlTask ExecuteTask
//----------------------------------------------------------------------
void tSenseControlModule::SenseTask::ExecuteTask()
{
  this->module->Sense();
}
