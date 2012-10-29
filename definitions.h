//
// You received this file as part of Finroc
// A Framework for intelligent robot control
//
// Copyright (C) Finroc GbR (finroc.org)
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
/*!\file    core/definitions.h
 *
 * \author  Max Reichardt
 *
 * \date    2012-10-26
 *
 * Global finroc definitions.
 * Therefore, placed in finroc namespace.
 *
 */
//----------------------------------------------------------------------
#ifndef __core__definitions_h__
#define __core__definitions_h__

//----------------------------------------------------------------------
// External includes (system with <>, local with "")
//----------------------------------------------------------------------
#include <string>

//----------------------------------------------------------------------
// Internal includes with ""
//----------------------------------------------------------------------

//----------------------------------------------------------------------
// Namespace declaration
//----------------------------------------------------------------------
namespace finroc
{

//----------------------------------------------------------------------
// Forward declarations / typedefs / enums
//----------------------------------------------------------------------
typedef std::string tString; // could be replaced with RT-String type later

namespace definitions
{

#ifdef RRLIB_SINGLE_THREADED
enum { cSINGLE_THREADED = 1 };  //!< Compile Finroc in single-threaded mode
#else
enum { cSINGLE_THREADED = 0 };  //!< Compile Finroc in multi-threaded mode
#endif

/*! Collect edge statistics (for profiling) ? */
enum { cCOLLECT_EDGE_STATISTICS = 0 };

}

//----------------------------------------------------------------------
// Function declarations
//----------------------------------------------------------------------

//----------------------------------------------------------------------
// End of namespace declaration
//----------------------------------------------------------------------
}


#endif
