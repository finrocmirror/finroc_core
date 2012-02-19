/**
 * You received this file as part of an advanced experimental
 * robotics framework prototype ('finroc')
 *
 * Copyright (C) 2007-2012 Max Reichardt,
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
#include "core/port/rpc/tThreadLocalRPCData.h"
#include "core/port/rpc/tInterfaceNetPort.h"
#include "core/port/rpc/tMethodCallException.h"
#include "core/port/rpc/tInterfaceServerPort.h"
#include "core/port/rpc/tRPCThreadPool.h"

namespace finroc
{
namespace core
{
namespace internal
{

template<typename HANDLER, typename R, int ARGNO, typename ... TArgs>
struct tCallHandler; // make the compiler happy - see http://stackoverflow.com/questions/1989552/gcc-error-with-variadic-templates-sorry-unimplemented-cannot-expand-identif

template<typename HANDLER, typename R, int ARGNO, typename ARG1, typename ... TArgs>
struct tCallHandler<HANDLER, R, ARGNO, ARG1, TArgs...>
{
  template <typename ... Args>
  inline static R Handle(tAbstractMethod& method, tMethodCall::tPtr& call, HANDLER& handler, Args&... args)
  {
    typename std::remove_reference<ARG1>::type arg;
    call->GetParam(ARGNO, arg);
    return tCallHandler < HANDLER, R, ARGNO + 1, TArgs... >::Handle(method, call, handler, args..., arg);
  }
};

template<typename HANDLER, typename R, int ARGNO>
struct tCallHandler<HANDLER, R, ARGNO>
{
  template <typename ... Args>
  inline static R Handle(tAbstractMethod& method, tMethodCall::tPtr& call, HANDLER& handler, Args&... args)
  {
    return handler.HandleCall(method, args...);
  }
};

}

template<typename HANDLER, typename R, typename ... TArgs>
R tMethod<HANDLER, R, TArgs...>::Call(tInterfaceClientPort port, TArgs... args, int net_timeout)
{
  tInterfacePort* ip = port.GetServer();
  if (ip && ip->GetType() == tInterfacePort::eNetwork)
  {
    tMethodCall::tPtr mc = tThreadLocalRPCData::Get().GetUnusedMethodCall();
    mc->SetParameters(args...);
    mc->PrepareSyncRemoteExecution(this, port.GetDataType(), net_timeout > 0 ? net_timeout : GetDefaultNetTimeout());
    mc = static_cast<tInterfaceNetPort*>(ip)->SynchCallOverTheNet(mc, mc->GetNetTimeout());
    assert(mc);
    R ret;
    mc->GetParam(0, ret);
    return ret;
  }
  else if (ip && ip->GetType() == tInterfacePort::eServer)
  {
    HANDLER* handler = static_cast<HANDLER*>((static_cast<tInterfaceServerPort*>(ip))->GetHandler());
    if (!handler)
    {
      throw tMethodCallException(tMethodCallException::eNO_CONNECTION, CODE_LOCATION_MACRO);
    }
    return handler->HandleCall(*this, args...);
  }
  else
  {
    throw tMethodCallException(tMethodCallException::eNO_CONNECTION, CODE_LOCATION_MACRO);
  }
}

template<typename HANDLER, typename R, typename ... TArgs>
void tMethod<HANDLER, R, TArgs...>::CallAsync(tInterfaceClientPort port, tAsyncReturnHandler<R>* handler, TArgs... args, int net_timeout, bool force_same_thread)
{
  tInterfacePort* ip = port.GetServer();
  if (ip && ip->GetType() == tInterfacePort::eNetwork)
  {
    tMethodCall::tPtr mc = tThreadLocalRPCData::Get().GetUnusedMethodCall();
    mc->SetParameters(args...);
    mc->PrepareSyncRemoteExecution(this, port.GetDataType(), handler, static_cast<tInterfaceNetPort*>(ip), net_timeout > 0 ? net_timeout : GetDefaultNetTimeout());  // always do this in extra thread
    tRPCThreadPool::GetInstance().ExecuteTask(std::move(mc));
  }
  else if (ip && ip->GetType() == tInterfacePort::eServer)
  {
    HANDLER* mhandler = static_cast<HANDLER*>((static_cast<tInterfaceServerPort*>(ip))->GetHandler());
    if (!mhandler)
    {
      handler->HandleMethodCallException(this, tMethodCallException(tMethodCallException::eNO_CONNECTION, CODE_LOCATION_MACRO));
    }
    if (force_same_thread || (!HandleInExtraThread()))
    {
      try
      {
        R ret = mhandler->HandleCall(this, args...);
        handler->HandleReturn(this, ret);
      }
      catch (const tMethodCallException& e)
      {
        handler->HandleMethodCallException(this, e);
      }
    }
    else
    {
      tMethodCall::tPtr mc = tThreadLocalRPCData::Get().GetUnusedMethodCall();
      mc->SetParameters(args...);
      mc->PrepareExecution(this, port.GetDataType(), mhandler, handler);
      tRPCThreadPool::GetInstance().ExecuteTask(mc);
    }
  }
  else
  {
    handler->HandleMethodCallException(this, tMethodCallException(tMethodCallException::eNO_CONNECTION, CODE_LOCATION_MACRO));
  }
}

template<typename HANDLER, typename R, typename ... TArgs>
void tMethod<HANDLER, R, TArgs...>::ExecuteAsyncNonVoidCallOverTheNet(tMethodCall::tPtr& mc, tInterfaceNetPort& net_port, tAbstractAsyncReturnHandler& ret_handler, int net_timeout)
{
  tAsyncReturnHandler<R>& r_handler = static_cast<tAsyncReturnHandler<R>&>(ret_handler);
  assert(mc->GetMethod() == this);
  try
  {
    mc = net_port.SynchCallOverTheNet(mc, net_timeout);
  }
  catch (const tMethodCallException& e)
  {
    // we shouldn't need to recycle anything, since call is responsible for this
    r_handler.HandleMethodCallException(this, e);
    return;
  }
  assert(!mc->HasException() && "should have been thrown");
  R ret;
  mc->GetParam(0, ret);
  mc.reset();
  r_handler.HandleReturn(this, ret);
}

template<typename HANDLER, typename R, typename ... TArgs>
void tMethod<HANDLER, R, TArgs...>::ExecuteFromMethodCallObject(tMethodCall::tPtr& call, tAbstractMethodCallHandler& handler, tAbstractAsyncReturnHandler* ret_handler)
{
  HANDLER& h2 = static_cast<HANDLER&>(handler);
  tAsyncReturnHandler<R>* rh2 = static_cast<tAsyncReturnHandler<R>*>(ret_handler);
  ExecuteFromMethodCallObject(call, h2, rh2, false);
}

template<typename HANDLER, typename R, typename ... TArgs>
void tMethod<HANDLER, R, TArgs...>::ExecuteFromMethodCallObject(tMethodCall::tPtr& call, HANDLER& handler, tAsyncReturnHandler<R>* ret_handler, bool dummy)
{
  try
  {
    R ret = internal::tCallHandler<HANDLER, R, 0, TArgs...>::Handle(*this, call, handler);
    if (ret_handler)
    {
      call.reset();
      ret_handler->HandleReturn(this, ret);
    }
    else
    {
      call->RecycleParameters();
      call->SetStatusReturn();
      call->SetParameters(ret);
    }
  }
  catch (const tMethodCallException& e)
  {
    if (ret_handler)
    {
      call.reset();
      ret_handler->HandleMethodCallException(this, e);
    }
    else
    {
      call->SetExceptionStatus(e.GetTypeId());
    }
  }
}

} // namespace finroc
} // namespace core

