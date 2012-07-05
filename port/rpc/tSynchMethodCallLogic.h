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

#ifndef core__port__rpc__tSynchMethodCallLogic_h__
#define core__port__rpc__tSynchMethodCallLogic_h__

#include "rrlib/finroc_core_utils/definitions.h"

#include "core/port/rpc/tCallable.h"
#include "core/port/rpc/tAbstractCall.h"
#include "core/port/rpc/tThreadLocalRPCData.h"
#include "core/port/rpc/tMethodCallSyncher.h"

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
private:

  tSynchMethodCallLogic() {}

  /*!
   * \return Description for logging
   */
  inline static const char* GetLogDescription()
  {
    return "SynchMethodCallLogic";
  }

public:

  /*!
   * Deliver/pass return value to calling/waiting thread.
   *
   * \param call Call object that (possibly) contains some return value
   */
  static void HandleMethodReturn(tAbstractCall::tPtr& call);

  /*!
   * Perform synchronous call. Thread will wait for return value (until timeout has passed).
   * If something goes wrong, a tMethodCallException is thrown.
   * Therefore, this should _always_ be called in a try/catch-block.
   *
   * \param <T> Call type
   * \param call Actual Call object (typically, swapped with another if call is transferred via network; just use this to obtain result)
   * \param call_me "Thing" that will be invoked/called with Call object
   * \param timeout Timeout for call
   */
  template <typename T>
  inline static void PerformSynchCall(std::unique_ptr<T, tSerializableReusable::tRecycler>& call, tCallable<T>& call_me, const rrlib::time::tDuration& timeout)
  {
    tMethodCallSyncher& mcs = tThreadLocalRPCData::Get().GetMethodSyncher();

    rrlib::thread::tLock lock2(mcs);
    call->SetupSynchCall(mcs);
    mcs.current_method_call_index = call->GetMethodCallIndex();
    assert(mcs.method_return.get() == NULL);
    call_me.InvokeCall(call);
    mcs.monitor.Wait(lock2, timeout, false);

    // reset stuff for next call
    mcs.GetAndUseNextCallIndex();  // Invalidate results of any incoming outdated returns
    call.reset(static_cast<T*>(mcs.method_return.release()));  // more or less equivalent to std::move()

    if (!call)
    {
      throw tMethodCallException(tMethodCallException::tType::TIMEOUT, CODE_LOCATION_MACRO);
    }
    else if (call->HasException())
    {
      throw tMethodCallException(call->GetExceptionType(), CODE_LOCATION_MACRO);
    }
  }
};

} // namespace finroc
} // namespace core

#endif // core__port__rpc__tSynchMethodCallLogic_h__
