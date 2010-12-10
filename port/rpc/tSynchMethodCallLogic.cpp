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
#include "core/port/rpc/tSynchMethodCallLogic.h"
#include "core/port/rpc/tMethodCallSyncher.h"
#include "core/port/rpc/tAbstractCall.h"
#include "core/port/tThreadLocalCache.h"
#include "core/port/rpc/tMethodCallException.h"

namespace finroc
{
namespace core
{
void tSynchMethodCallLogic::HandleMethodReturn(tAbstractCall* call)
{
  // return value
  tMethodCallSyncher* mcs = tMethodCallSyncher::Get(call->GetSyncherID());
  mcs->ReturnValue(call);
}

tAbstractCall* tSynchMethodCallLogic::PerformSynchCallImpl(tAbstractCall* call, tCallable<tAbstractCall>* call_me, int64 timeout)
{
  tMethodCallSyncher* mcs = tThreadLocalCache::Get()->GetMethodSyncher();
  tAbstractCall* ret = NULL;
  {
    util::tLock lock2(mcs);
    call->SetupSynchCall(mcs);
    mcs->current_method_call_index = call->GetMethodCallIndex();
    assert((mcs->method_return == NULL));
    call_me->InvokeCall(call);
    try
    {
      mcs->monitor.Wait(lock2, timeout);
    }
    catch (const util::tInterruptedException& e)
    {
      FINROC_LOG_STREAM(rrlib::logging::eLL_ERROR, log_domain, "Synch method call interrupted... this shouldn't happen... usually");
    }

    // reset stuff for next call
    mcs->GetAndUseNextCallIndex();  // Invalidate results of any incoming outdated returns
    ret = mcs->method_return;
    mcs->method_return = NULL;

    if (ret == NULL)
    {
      // (recycling is job of receiver)
      throw tMethodCallException(tMethodCallException::eTIMEOUT, CODE_LOCATION_MACRO);
    }
  }
  return ret;
}

} // namespace finroc
} // namespace core

