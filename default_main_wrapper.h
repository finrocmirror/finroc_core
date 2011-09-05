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
/*!\file    default_main_wrapper.h
 *
 * \author  Tobias Foehst
 * \author  Bernd-Helge Schaefer
 *
 * \date    2010-12-09
 *
 * \brief
 *
 * \b
 *
 */
//----------------------------------------------------------------------
#ifndef _core__default_main_wrapper_h_
#define _core__default_main_wrapper_h_

//----------------------------------------------------------------------
// External includes (system with <>, local with "")
//----------------------------------------------------------------------

//----------------------------------------------------------------------
// Internal includes with ""
//----------------------------------------------------------------------
#include "rrlib/getopt/parser.h"

#include "core/thread/tThreadContainer.h"

//----------------------------------------------------------------------
// Debugging
//----------------------------------------------------------------------

//----------------------------------------------------------------------
// Namespace declaration
//----------------------------------------------------------------------

//----------------------------------------------------------------------
// Forward declarations / typedefs / enums / const values
//----------------------------------------------------------------------
extern const char * const cPROGRAM_VERSION;
extern const char * const cPROGRAM_DESCRIPTION;

//----------------------------------------------------------------------
// Function declaration
//----------------------------------------------------------------------

void StartUp();
void InitMainGroup(finroc::core::tThreadContainer *main_thread, std::vector<char*> remaining_args);



//----------------------------------------------------------------------
// End of namespace declaration
//----------------------------------------------------------------------

#endif
