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
/*!\file    mTestSenseControlModule.h
 *
 * \author  Tobias Foehst
 *
 * \date    2010-12-17
 *
 * \brief Contains mTestModule
 *
 * \b mTestModule
 *
 */
//----------------------------------------------------------------------
#ifndef _core__mTestSenseControlModule_h_
#define _core__mTestSenseControlModule_h_

#include "core/structure/tSenseControlModule.h"

//----------------------------------------------------------------------
// External includes (system with <>, local with "")
//----------------------------------------------------------------------

//----------------------------------------------------------------------
// Internal includes with ""
//----------------------------------------------------------------------

//----------------------------------------------------------------------
// Debugging
//----------------------------------------------------------------------

//----------------------------------------------------------------------
// Namespace declaration
//----------------------------------------------------------------------

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
class mTestSenseControlModule : public finroc::core::structure::tSenseControlModule
{
  static finroc::core::tStandardCreateModuleAction<mTestSenseControlModule> cCREATE_ACTION;

  int counter;

//----------------------------------------------------------------------
// Protected methods
//----------------------------------------------------------------------

  virtual void Control();

  virtual void Sense();

//----------------------------------------------------------------------
// Public methods
//----------------------------------------------------------------------
public:

  mTestSenseControlModule(finroc::core::tFrameworkElement *parent, const finroc::util::tString &name = "TestModule");

  tCI<finroc::core::tPortNumeric> ci_signal_1;
  tCO<finroc::core::tPortNumeric> co_signal_2;
  tSI<finroc::core::tPortNumeric> si_signal_3;
  tSO<finroc::core::tPortNumeric> so_signal_4;

};

//----------------------------------------------------------------------
// End of namespace declaration
//----------------------------------------------------------------------

#endif