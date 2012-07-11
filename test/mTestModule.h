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
/*!\file    mTestModule.h
 *
 * \author  Tobias Foehst
 *
 * \date    2010-12-09
 *
 * \brief Contains mTestModule
 *
 * \b mTestModule
 *
 */
//----------------------------------------------------------------------
#ifndef _core__mTestModule_h_
#define _core__mTestModule_h_

#include "core/structure/tModule.h"

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
class mTestModule : public finroc::core::structure::tModule
{
  static finroc::core::tStandardCreateModuleAction<mTestModule> cCREATE_ACTION;

//----------------------------------------------------------------------
// Public ports
//----------------------------------------------------------------------
public:

  tInput<> signal_1;
  tOutput<> signal_2;

//----------------------------------------------------------------------
// Public methods
//----------------------------------------------------------------------
public:

  mTestModule(finroc::core::tFrameworkElement *parent, const finroc::util::tString &name = "TestModule");


//----------------------------------------------------------------------
// Private fields and methods
//----------------------------------------------------------------------
private:

  int counter;

  virtual void Update();

};

//----------------------------------------------------------------------
// End of namespace declaration
//----------------------------------------------------------------------

#endif
