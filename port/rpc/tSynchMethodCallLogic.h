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
#include "finroc_core_utils/tJCBase.h"

#ifndef CORE__PORT__RPC__TSYNCHMETHODCALLLOGIC_H
#define CORE__PORT__RPC__TSYNCHMETHODCALLLOGIC_H

#include "core/port/rpc/tAbstractCall.h"
#include "core/port/rpc/tCallable.h"

namespace finroc
{
namespace core
{
/*!
 * \author Max Reichardt
 *
 * This class contains the logic for triggering synchronous (method) calls
 * (possibly over the net & without blocking further threads etc.)
 */
class tSynchMethodCallLogic : public util::tUncopyableObject
{
  /*!
   * (Private Helper method - because we'd need a .hpp otherwise)
   * Perform synchronous call. Thread will wait for return value (until timeout has passed).
   *
   * \param <T> Call type
   * \param call Actual Call object
   * \param call_me "Thing" that will be invoked/called with Call object
   * \param timeout Timeout for call
   * \return Returns call object - might be the same as in call parameter (likely - if call wasn't transferred via network)
   */
  static tAbstractCall* PerformSynchCallImpl(tAbstractCall* call, tCallable<tAbstractCall>* call_me, int64 timeout);

public:

  tSynchMethodCallLogic() {}

  /*!
   * Deliver/pass return value to calling/waiting thread.
   *
   * \param call Call object that (possibly) contains some return value
   */
  static void HandleMethodReturn(tAbstractCall* call);

  /*!
   * Perform synchronous call. Thread will wait for return value (until timeout has passed).
   *
   * \param <T> Call type
   * \param call Actual Call object
   * \param call_me "Thing" that will be invoked/called with Call object
   * \param timeout Timeout for call
   * \return Returns call object - might be the same as in call parameter (likely - if call wasn't transferred via network)
   */
  template <typename T>
  inline static T* PerformSynchCall(T* call, tCallable<T>* call_me, int64 timeout)
  {
    assert(((void*)(static_cast<tAbstractCall*>(call))) == ((void*)call)); // ensure safety for Callable cast
    tCallable<tAbstractCall>* tmp = reinterpret_cast<tCallable<tAbstractCall>*>(call_me);
    return static_cast<T*>(PerformSynchCallImpl(call, tmp, timeout));
  }

//  /**
//   * Mark this call and signal that it won't
//   *
//   * \param call Call
//   */
//  public static void noQuickReturn(AbstractCall call) {
//
//  }

};

} // namespace finroc
} // namespace core

#endif // CORE__PORT__RPC__TSYNCHMETHODCALLLOGIC_H
