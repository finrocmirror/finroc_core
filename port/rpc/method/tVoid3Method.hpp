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
template<typename HANDLER, typename P1, typename P2, typename P3>
tVoid3Method<HANDLER, P1, P2, P3>::tVoid3Method(tPortInterface& port_interface, const util::tString& name, const util::tString& p1_name, const util::tString& p2_name, const util::tString& p3_name, bool handle_in_extra_thread) :
    tAbstractVoidMethod(port_interface, name, p1_name, p2_name, p3_name, cNO_PARAM, handle_in_extra_thread)
{
}

template<typename HANDLER, typename P1, typename P2, typename P3>
void tVoid3Method<HANDLER, P1, P2, P3>::Call(tInterfaceClientPort port, P1 p1, P2 p2, P3 p3, bool force_same_thread)
{
  //1
  assert((HasLock(p1)));  //2
  assert((HasLock(p2)));  //3
  assert((HasLock(p3)));
  tInterfacePort* ip = port.GetServer();
  if (ip != NULL && ip->GetType() == tInterfacePort::eNetwork)
  {
    tMethodCall* mc = tThreadLocalCache::GetFast()->GetUnusedMethodCall();
    //1
    mc->AddParamForSending(p1);  //2
    mc->AddParamForSending(p2);  //3
    mc->AddParamForSending(p3);
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
      Cleanup(p1);  //2
      Cleanup(p2);  //3
      Cleanup(p3);
      throw tMethodCallException(tMethodCallException::eNO_CONNECTION, CODE_LOCATION_MACRO);
    }
    if (force_same_thread || (!HandleInExtraThread()))
    {
      handler->HandleVoidCall(this, p1, p2, p3);
    }
    else
    {
      tMethodCall* mc = tThreadLocalCache::GetFast()->GetUnusedMethodCall();
      //1
      mc->AddParamForLocalCall(0, p1);  //2
      mc->AddParamForLocalCall(1, p2);  //3
      mc->AddParamForLocalCall(2, p3);
      mc->PrepareExecution(this, port.GetDataType(), handler, NULL);
      tRPCThreadPool::GetInstance()->ExecuteTask(mc);
    }
  }
  else
  {
    //1
    Cleanup(p1);  //2
    Cleanup(p2);  //3
    Cleanup(p3);
    throw tMethodCallException(tMethodCallException::eNO_CONNECTION, CODE_LOCATION_MACRO);
  }
}

template<typename HANDLER, typename P1, typename P2, typename P3>
void tVoid3Method<HANDLER, P1, P2, P3>::ExecuteFromMethodCallObject(tMethodCall* call, tAbstractMethodCallHandler* handler, tAbstractAsyncReturnHandler* ret_handler)
{
  assert((ret_handler == NULL));
  HANDLER h2 = static_cast<HANDLER>(handler);
  ExecuteFromMethodCallObject(call, h2);
}

template<typename HANDLER, typename P1, typename P2, typename P3>
void tVoid3Method<HANDLER, P1, P2, P3>::ExecuteFromMethodCallObject(tMethodCall* call, const HANDLER& handler)
{
  assert((call != NULL && handler != NULL));
  HANDLER handler2 = handler;

  //1
  P1 p1;  //2
  P2 p2;  //3
  P3 p3;

  //1
  call->GetParam(0, p1); //2
  call->GetParam(1, p2); //3
  call->GetParam(2, p3);

  try
  {
    handler2->HandleVoidCall(this, p1, p2, p3);
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

