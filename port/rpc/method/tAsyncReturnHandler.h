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

#ifndef core__port__rpc__method__tAsyncReturnHandler_h__
#define core__port__rpc__method__tAsyncReturnHandler_h__

#include "rrlib/finroc_core_utils/definitions.h"

#include "core/port/rpc/method/tAbstractAsyncReturnHandler.h"

namespace finroc
{
namespace core
{
class tAbstractMethod;
class tMethodCallException;

/*!
 * \author Max Reichardt
 *
 * Handles return value from method call
 */
template<typename R>
class tAsyncReturnHandler : public tAbstractAsyncReturnHandler
{
public:

  /*!
   * Called on client when an asynchronous method call fails with an Exception
   *
   * \param method Method that was called and failed
   * \param mce Exception that was thrown
   */
  virtual void HandleMethodCallException(const tAbstractMethod* method, const tMethodCallException& mce) = 0;

  /*!
   * Called on client when an asynchronous method call returns
   *
   * \param method Method that was called
   * \param r Return value from method
   */
  virtual void HandleReturn(const tAbstractMethod* method, const R& r) = 0;

};

} // namespace finroc
} // namespace core

#endif // core__port__rpc__method__tAsyncReturnHandler_h__
