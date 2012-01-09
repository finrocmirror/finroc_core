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
/*!\file    tModule.cpp
 *
 * \author  Tobias Foehst
 * \author  Bernd-Helge Schaefer
 *
 * \date    2010-12-17
 *
 */
//----------------------------------------------------------------------
#include "core/structure/tModule.h"

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
// tModule constructors
//----------------------------------------------------------------------
tModule::tModule(tFrameworkElement *parent, const util::tString &name)
    : tModuleBase(parent, name),

    input(new tPortGroup(this, "Input", tEdgeAggregator::cIS_INTERFACE, tPortFlags::cINPUT_PORT)),
    output(new tPortGroup(this, "Output", tEdgeAggregator::cIS_INTERFACE, tPortFlags::cOUTPUT_PORT)),
    update_task(this),
    input_changed(true)
{
  this->AddAnnotation(new tPeriodicFrameworkElementTask(this->input, this->output, &this->update_task));
}

//----------------------------------------------------------------------
// tModule Update
//----------------------------------------------------------------------
void tModule::Update()
{}

//----------------------------------------------------------------------
// tModule::UpdateTask constructors
//----------------------------------------------------------------------
tModule::UpdateTask::UpdateTask(tModule *module)
    : module(module)
{}

//----------------------------------------------------------------------
// tModule::UpdateTask ExecuteTask
//----------------------------------------------------------------------
void tModule::UpdateTask::ExecuteTask()
{
  this->module->CheckParameters();
  this->module->input_changed = this->module->ProcessChangedFlags(this->module->GetInputs());
  this->module->Update();
}
