//
// You received this file as part of Finroc
// A framework for intelligent robot control
//
// Copyright (C) Finroc GbR (finroc.org)
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along
// with this program; if not, write to the Free Software Foundation, Inc.,
// 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
//
//----------------------------------------------------------------------
/*!\file    core/definitions.h
 *
 * \author  Max Reichardt
 *
 * \date    2012-10-26
 *
 * \brief
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
#include "rrlib/time/time.h"
#include "rrlib/uri/tURI.h"

//----------------------------------------------------------------------
// Internal includes with ""
//----------------------------------------------------------------------
#include "core/version.h"

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

/*!
 * Definitions for framework element handles:
 * A handle is assigned to each framework element that is created.
 * It is guaranteed that such handles are unique for the amount of time
 * specified below (after deleting an element with the same handle).
 * This way, e.g. client requests with outdated handles simply
 * fail and do not operate on wrong ports.
 *
 * In order to save memory, the lookup handle=>element is stored in a
 * nested array structure.
 *
 * Handles have the following format (bit widths are defined below):
 * [primary array index][secondary array index][stamp]
 *
 * The maximum number of framework elements is
 * 2^(cHANDLE_PRIMARY_ARRAY_INDEX_BIT_WIDTH + cHANDLE_SECONDARY_ARRAY_INDEX_BIT_WIDTH)
 * , which is currently ~1 million.
 *
 * This is divided into two:
 * Handles >= 0x80000000 are ports.
 * Handles < 0x80000000 are non-ports.
 *
 * Handle 0 is runtime environment.
 *
 * This means, we currently have max. 512K ports and max. 512K non-ports.
 */
enum { cHANDLE_PRIMARY_ARRAY_INDEX_BIT_WIDTH = 10 };
enum { cHANDLE_SECONDARY_ARRAY_INDEX_BIT_WIDTH = 10 };
constexpr rrlib::time::tDuration cHANDLE_UNIQUENESS_GUARANTEE_DURATION(std::chrono::minutes(1));

}

namespace core
{
typedef rrlib::uri::tURI tURI;
typedef rrlib::uri::tPath tPath;
}

//----------------------------------------------------------------------
// Function declarations
//----------------------------------------------------------------------

//----------------------------------------------------------------------
// End of namespace declaration
//----------------------------------------------------------------------
}


#endif
