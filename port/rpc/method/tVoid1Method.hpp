//Generated from Void4Method.java
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
#include "core/port/rpc/tInterfacePort.h"
#include "core/port/rpc/tMethodCall.h"
#include "core/port/tThreadLocalCache.h"
#include "core/port/rpc/tInterfaceNetPort.h"
#include "core/port/rpc/tInterfaceServerPort.h"
#include "core/port/rpc/tMethodCallException.h"
#include "core/port/rpc/tRPCThreadPool.h"
#include "rrlib/finroc_core_utils/log/tLogUser.h"

namespace finroc
{
namespace core
{
template<typename HANDLER, typename P1>
tVoid1Method<HANDLER, P1>::tVoid1Method(tPortInterface& port_interface, const util::tString& name, const util::tString& p1_name, bool handle_in_extra_thread) :
    tAbstractVoidMethod(port_interface, name, p1_name, cNO_PARAM, cNO_PARAM, cNO_PARAM, handle_in_extra_thread)
{
}

template<typename HANDLER, typename P1>
void tVoid1Method<HANDLER, P1>::Call(tInterfaceClientPort port, P1 p1, bool force_same_thread)
{
  //1
  assert((HasLock(p1)));
  tInterfacePort* ip = port.GetServer();
  if (ip != NULL && ip->GetType() == tInterfacePort::eNetwork)
  {
    tMethodCall* mc = tThreadLocalCache::GetFast()->GetUnusedMethodCall();
    //1
    mc->AddParamForSending(p1);
    mc->SendParametersComplete();
    mc->SetMethod(this, port.GetDataType());
    (static_cast<tInterfaceNetPort*>(ip))->SendAsyncCall(mc);
  }
  else if (ip != NULL && ip->GetType() == tInterfacePort::eServer)
  {
    HANDLER handler = static_cast<HANDLER>((static_cast<tInterfaceServerPort*>(ip))->GetHandler());
    if (handler == NULL)
    {
      //1
      Cleanup(p1);
      throw tMethodCallException(tMethodCallException::eNO_CONNECTION, CODE_LOCATION_MACRO);
    }
    if (force_same_thread || (!HandleInExtraThread()))
    {
      handler->HandleVoidCall(this, p1);
    }
    else
    {
      tMethodCall* mc = tThreadLocalCache::GetFast()->GetUnusedMethodCall();
      //1
      mc->AddParamForLocalCall(0, p1);
      mc->PrepareExecution(this, port.GetDataType(), handler, NULL);
      tRPCThreadPool::GetInstance()->ExecuteTask(mc);
    }
  }
  else
  {
    //1
    Cleanup(p1);
    throw tMethodCallException(tMethodCallException::eNO_CONNECTION, CODE_LOCATION_MACRO);
  }
}

template<typename HANDLER, typename P1>
void tVoid1Method<HANDLER, P1>::ExecuteFromMethodCallObject(tMethodCall* call, tAbstractMethodCallHandler* handler, tAbstractAsyncReturnHandler* ret_handler)
{
  assert((ret_handler == NULL));
  HANDLER h2 = static_cast<HANDLER>(handler);
  ExecuteFromMethodCallObject(call, h2);
}

template<typename HANDLER, typename P1>
void tVoid1Method<HANDLER, P1>::ExecuteFromMethodCallObject(tMethodCall* call, const HANDLER& handler)
{
  assert((call != NULL && handler != NULL));
  HANDLER handler2 = handler;

  //1
  P1 p1;

  //1
  call->GetParam(0, p1);

  try
  {
    handler2->HandleVoidCall(this, p1);
  }
  catch (const tMethodCallException& e)
  {
    // don't send anything back
    FINROC_LOG_STREAM(rrlib::logging::eLL_ERROR, log_domain, e);
  }
  call->Recycle();
}

} // namespace finroc
} // namespace core

