//Generated from Port4Method.java
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
#include "core/port/tThreadLocalCache.h"
#include "core/port/rpc/tMethodCallException.h"
#include "core/port/rpc/tInterfaceServerPort.h"
#include "core/port/rpc/tRPCThreadPool.h"

namespace finroc
{
namespace core
{
template<typename HANDLER, typename R>
tPort0Method<HANDLER, R>::tPort0Method(tPortInterface& port_interface, const util::tString& name, bool handle_in_extra_thread, int default_net_timeout) :
    tAbstractNonVoidMethod(port_interface, name, cNO_PARAM, cNO_PARAM, cNO_PARAM, cNO_PARAM, handle_in_extra_thread, default_net_timeout)
{
}

template<typename HANDLER, typename R>
R tPort0Method<HANDLER, R>::Call(tInterfaceClientPort* port, int net_timeout)
{
  tInterfacePort* ip = port->GetServer();
  if (ip != NULL && ip->GetType() == tInterfacePort::eNetwork)
  {
    tMethodCall* mc = tThreadLocalCache::GetFast()->GetUnusedMethodCall();

    mc->SendParametersComplete();
    mc->PrepareSyncRemoteExecution(this, port->GetDataType(), net_timeout > 0 ? net_timeout : GetDefaultNetTimeout());
    try
    {
      mc = (static_cast<tInterfaceNetPort*>(ip))->SynchCallOverTheNet(mc, mc->GetNetTimeout());
    }
    catch (const tMethodCallException& e)
    {
      // we shouldn't need to recycle anything, since call is responsible for this
      throw tMethodCallException(e.GetType());
    }
    mc->DeserializeParamaters();
    if (mc->HasException())
    {
      int8 type = 0;

      mc->GetParam(0, type);

      mc->Recycle();
      throw tMethodCallException(type);
    }
    else
    {
      R ret;

      mc->GetParam(0, ret);

      mc->Recycle();
      assert((HasLock(ret)));
      return ret;
    }

  }
  else if (ip != NULL && ip->GetType() == tInterfacePort::eServer)
  {
    HANDLER handler = static_cast<HANDLER>((static_cast<tInterfaceServerPort*>(ip))->GetHandler());
    if (handler == NULL)
    {
      throw tMethodCallException(tMethodCallException::eNO_CONNECTION);
    }
    R ret = handler->HandleCall(this);
    assert((HasLock(ret)));
    return ret;
  }
  else
  {
    throw tMethodCallException(tMethodCallException::eNO_CONNECTION);
  }
}

template<typename HANDLER, typename R>
void tPort0Method<HANDLER, R>::CallAsync(const tInterfaceClientPort* port, tAsyncReturnHandler<R>* handler, int net_timeout, bool force_same_thread)
{
  tInterfacePort* ip = port->GetServer();
  if (ip != NULL && ip->GetType() == tInterfacePort::eNetwork)
  {
    tMethodCall* mc = tThreadLocalCache::GetFast()->GetUnusedMethodCall();

    mc->SendParametersComplete();
    mc->PrepareSyncRemoteExecution(this, port->GetDataType(), handler, static_cast<tInterfaceNetPort*>(ip), net_timeout > 0 ? net_timeout : GetDefaultNetTimeout());  // always do this in extra thread
    tRPCThreadPool::GetInstance()->ExecuteTask(mc);
  }
  else if (ip != NULL && ip->GetType() == tInterfacePort::eServer)
  {
    HANDLER mhandler = static_cast<HANDLER>((static_cast<tInterfaceServerPort*>(ip))->GetHandler());
    if (mhandler == NULL)
    {
      handler->HandleMethodCallException(this, tMethodCallException(tMethodCallException::eNO_CONNECTION));
    }
    if (force_same_thread || (!HandleInExtraThread()))
    {
      try
      {
        R ret = mhandler->HandleCall(this);
        assert((HasLock(ret)));
        handler->HandleReturn(this, ret);
      }
      catch (const tMethodCallException& e)
      {
        handler->HandleMethodCallException(this, e);
      }
    }
    else
    {
      tMethodCall* mc = tThreadLocalCache::GetFast()->GetUnusedMethodCall();

      mc->PrepareExecution(this, port->GetDataType(), mhandler, handler);
      tRPCThreadPool::GetInstance()->ExecuteTask(mc);
    }
  }
  else
  {
    handler->HandleMethodCallException(this, tMethodCallException(tMethodCallException::eNO_CONNECTION));
  }
}

template<typename HANDLER, typename R>
void tPort0Method<HANDLER, R>::ExecuteAsyncNonVoidCallOverTheNet(tMethodCall* mc, tInterfaceNetPort* net_port, tAbstractAsyncReturnHandler* ret_handler, int net_timeout)
{
  tAsyncReturnHandler<R>* r_handler = static_cast<tAsyncReturnHandler<R>*>(ret_handler);
  assert((mc->GetMethod() == this));
  //mc.setMethod(this);
  try
  {
    mc = net_port->SynchCallOverTheNet(mc, net_timeout);
  }
  catch (const tMethodCallException& e)
  {
    // we shouldn't need to recycle anything, since call is responsible for this
    r_handler->HandleMethodCallException(this, e);
    return;
  }
  mc->DeserializeParamaters();
  if (mc->HasException())
  {
    int8 type = 0;

    mc->GetParam(0, type);

    mc->Recycle();

    r_handler->HandleMethodCallException(this, tMethodCallException(type));
  }
  else
  {
    R ret;

    mc->GetParam(0, ret);

    mc->Recycle();
    assert((HasLock(ret)));
    r_handler->HandleReturn(this, ret);
  }
}

template<typename HANDLER, typename R>
void tPort0Method<HANDLER, R>::ExecuteFromMethodCallObject(tMethodCall* call, tAbstractMethodCallHandler* handler, tAbstractAsyncReturnHandler* ret_handler)
{
  HANDLER h2 = static_cast<HANDLER>(handler);
  tAsyncReturnHandler<R>* rh2 = static_cast<tAsyncReturnHandler<R>*>(ret_handler);
  ExecuteFromMethodCallObject(call, h2, rh2, false);
}

template<typename HANDLER, typename R>
void tPort0Method<HANDLER, R>::ExecuteFromMethodCallObject(tMethodCall* call, const HANDLER& handler, tAsyncReturnHandler<R>* ret_handler, bool dummy)
{
  assert((call != NULL && handler != NULL));
  HANDLER handler2 = handler;

  //JavaOnlyBlock

  try
  {
    R ret = handler2->HandleCall(this);
    if (ret_handler != NULL)
    {
      call->Recycle();
      ret_handler->HandleReturn(this, ret);
    }
    else
    {
      call->RecycleParameters();
      call->SetStatusReturn();
      call->AddParamForSending(ret);
      call->SendParametersComplete();
    }
  }
  catch (const tMethodCallException& e)
  {
    if (ret_handler != NULL)
    {
      call->Recycle();
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

