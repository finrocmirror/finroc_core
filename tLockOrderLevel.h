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
/*!\file    core/tLockOrderLevel.h
 *
 * \author  Max Reichardt
 *
 * \date    2012-10-28
 *
 * \brief   Contains tLockOrderLevel
 *
 * \b tLockOrderLevel
 *
 * Lock order level constants for different types of classes
 *
 */
//----------------------------------------------------------------------
#ifndef __core__tLockOrderLevel_h__
#define __core__tLockOrderLevel_h__

//----------------------------------------------------------------------
// External includes (system with <>, local with "")
//----------------------------------------------------------------------
#include <limits>

//----------------------------------------------------------------------
// Internal includes with ""
//----------------------------------------------------------------------

//----------------------------------------------------------------------
// Namespace declaration
//----------------------------------------------------------------------
namespace finroc
{
namespace core
{

//----------------------------------------------------------------------
// Forward declarations / typedefs / enums
//----------------------------------------------------------------------

//----------------------------------------------------------------------
// Class declaration
//----------------------------------------------------------------------
//! Lock order level constants
/*!
 * Lock order level constants for different types of classes
 */
enum class tLockOrderLevel : int
{
  /*! Group that won't contain any other (unknown) groups anymore */
  RUNTIME_ROOT = 100000,

  /*! Group that won't contain any other (unknown) groups anymore */
  LEAF_GROUP = 200000,

  /*! Port Group that won't contain any other framework elements except of ports */
  LEAF_PORT_GROUP = 300000,

  /*! Ports */
  PORT = 400000,

  /*! Runtime Register */
  RUNTIME_REGISTER = 800000,

  /*! Stuff in remote runtime environment */
  REMOTE = 500000,

  /*! Stuff in remote runtime environment */
  REMOTE_PORT = 600000,

  /*! Links to stuff in remote runtime environment */
  REMOTE_LINKING = 500000,

  /*! Stuff to lock before everything else */
  FIRST = 0,

  /*! Innermost locks */
  INNER_MOST = std::numeric_limits<int>::max() - 10
};

//----------------------------------------------------------------------
// End of namespace declaration
//----------------------------------------------------------------------
}
}


#endif
