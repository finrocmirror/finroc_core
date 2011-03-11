/**
 * You received this file as part of an advanced experimental
 * robotics framework prototype ('finroc')
 *
 * Copyright (C) 2007-2010 Max Reichardt,
 *   Robotics Research Lab, University of Kaiserslautern
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

#ifndef core__tLockOrderLevels_h__
#define core__tLockOrderLevels_h__

#include "rrlib/finroc_core_utils/definitions.h"

namespace finroc
{
namespace core
{
/*!
 * \author Max Reichardt
 *
 * Lock order level constants for different types of classes
 */
class tLockOrderLevels : public util::tUncopyableObject
{
public:

  /*! Group that won't contain any other (unknown) groups anymore */
  static const int cRUNTIME_ROOT = 100000;

  /*! Group that won't contain any other (unknown) groups anymore */
  static const int cLEAF_GROUP = 200000;

  /*! Port Group that won't contain any other framework elements except of ports */
  static const int cLEAF_PORT_GROUP = 300000;

  /*! Ports */
  static const int cPORT = 400000;

  /*! Runtime Register */
  static const int cRUNTIME_REGISTER = 800000;

  /*! Stuff in remote runtime environment */
  static const int cREMOTE = 500000;

  /*! Stuff in remote runtime environment */
  static const int cREMOTE_PORT = 600000;

  /*! Links to stuff in remote runtime environment */
  static const int cREMOTE_LINKING = 500000;

  /*! Stuff to lock before everything else */
  static const int cFIRST = 0;

  /*! Innermost locks */
  static const int cINNER_MOST = util::tInteger::cMAX_VALUE - 10;

  tLockOrderLevels() {}
};

} // namespace finroc
} // namespace core

#endif // core__tLockOrderLevels_h__
