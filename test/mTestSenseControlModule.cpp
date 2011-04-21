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
/*!\file    mTestSenseControlModule.cpp
 *
 * \author  Tobias Foehst
 *
 * \date    2010-12-17
 *
 */
//----------------------------------------------------------------------
#include "core/test/mTestSenseControlModule.h"

//----------------------------------------------------------------------
// External includes (system with <>, local with "")
//----------------------------------------------------------------------

//----------------------------------------------------------------------
// Internal includes with ""
//----------------------------------------------------------------------

//----------------------------------------------------------------------
// Debugging
//----------------------------------------------------------------------
#include <cassert>

//----------------------------------------------------------------------
// Namespace usage
//----------------------------------------------------------------------
using namespace rrlib::logging;

//----------------------------------------------------------------------
// Forward declarations / typedefs / enums
//----------------------------------------------------------------------

//----------------------------------------------------------------------
// Const values
//----------------------------------------------------------------------
finroc::core::tStandardCreateModuleAction<mTestSenseControlModule> mTestSenseControlModule::cCREATE_ACTION("TestModule");

//----------------------------------------------------------------------
// Implementation
//----------------------------------------------------------------------

//----------------------------------------------------------------------
// mTestSenseControlModule constructors
//----------------------------------------------------------------------
mTestSenseControlModule::mTestSenseControlModule(finroc::core::tFrameworkElement *parent, const finroc::util::tString &name)
    : tSenseControlModule(parent, name),

    counter(0)
{}

//----------------------------------------------------------------------
// mTestSenseControlModule Control
//----------------------------------------------------------------------
void mTestSenseControlModule::Control()
{
  this->co_signal_2.Publish(this->counter);
  FINROC_LOG_STREAM(eLL_DEBUG) << this->counter;
  this->counter++;
}

//----------------------------------------------------------------------
// mTestSenseControlModule Sense
//----------------------------------------------------------------------
void mTestSenseControlModule::Sense()
{}
