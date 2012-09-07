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
#include "core/port/rpc/tThreadLocalRPCData.h"
#include "core/port/rpc/tInterfaceNetPort.h"
#include "core/port/rpc/tInterfaceServerPort.h"
#include "core/port/rpc/tMethodCallException.h"
#include "core/port/rpc/tRPCThreadPool.h"

namespace finroc
{
namespace core
{

namespace internal
{

template<typename HANDLER, int ARGNO, typename ... TArgs>
struct tVoidCallHandler; // make the compiler happy - see http://stackoverflow.com/questions/1989552/gcc-error-with-variadic-templates-sorry-unimplemented-cannot-expand-identif

template<typename HANDLER, int ARGNO, typename ARG1, typename ... TArgs>
struct tVoidCallHandler<HANDLER, ARGNO, ARG1, TArgs...>
{
  template <typename ... Args>
  inline static void Handle(tAbstractMethod& method, tMethodCall::tPtr& call, HANDLER& handler, Args&... args)
  {
    typename std::remove_reference<ARG1>::type arg;
    call->GetParam(ARGNO, arg);
    tVoidCallHandler < HANDLER, ARGNO + 1, TArgs... >::Handle(method, call, handler, args..., arg);
  }
};

template<typename HANDLER, int ARGNO>
struct tVoidCallHandler<HANDLER, ARGNO>
{
  template <typename ... Args>
  inline static void Handle(tAbstractMethod& method, tMethodCall::tPtr& call, HANDLER& handler, Args&... args)
  {
    handler.HandleVoidCall(method, args...);
  }
};

}

template<typename HANDLER, typename ... TArgs>
void tVoidMethod<HANDLER, TArgs...>::Call(tInterfaceClientPort port, bool force_same_thread, TArgs... args)
{
  tInterfacePort* ip = port.GetServer();
  if (ip && ip->GetType() == tInterfacePort::tType::NETWORK)
  {
    tMethodCall::tPtr mc = tThreadLocalRPCData::Get().GetUnusedMethodCall();
    mc->SetParameters(args...);
    mc->SetMethod(this, port.GetDataType());
    (static_cast<tInterfaceNetPort*>(ip))->SendAsyncCall(mc);
  }
  else if (ip && ip->GetType() == tInterfacePort::tType::SERVER)
  {
    HANDLER* handler = static_cast<HANDLER*>((static_cast<tInterfaceServerPort*>(ip))->GetHandler());
    if (!handler)
    {
      throw tMethodCallException(tMethodCallException::tType::NO_CONNECTION, CODE_LOCATION_MACRO);
    }
    if (force_same_thread || (!HandleInExtraThread()))
    {
      handler->HandleVoidCall(*this, args...);
    }
    else
    {
      tMethodCall::tPtr mc = tThreadLocalRPCData::Get().GetUnusedMethodCall();
      mc->SetParameters(args...);
      mc->PrepareExecution(this, port.GetDataType(), handler, NULL);
      tRPCThreadPool::GetInstance().ExecuteTask(std::move(mc));
    }
  }
  else
  {
    throw tMethodCallException(tMethodCallException::tType::NO_CONNECTION, CODE_LOCATION_MACRO);
  }
}

template<typename HANDLER, typename ... TArgs>
void tVoidMethod<HANDLER, TArgs...>::ExecuteFromMethodCallObject(tMethodCall::tPtr& call, tAbstractMethodCallHandler& handler, tAbstractAsyncReturnHandler* ret_handler)
{
  HANDLER& h2 = static_cast<HANDLER&>(handler);
  ExecuteFromMethodCallObject(call, h2);
}

template<typename HANDLER, typename ... TArgs>
void tVoidMethod<HANDLER, TArgs...>::ExecuteFromMethodCallObject(tMethodCall::tPtr& call, HANDLER& handler)
{
  try
  {
    internal::tVoidCallHandler<HANDLER, 0, TArgs...>::Handle(*this, call, handler);
  }
  catch (const tMethodCallException& e)
  {
    // don't send anything back
    FINROC_LOG_PRINT(ERROR, e);
  }
  call.reset();
}

} // namespace finroc
} // namespace core

