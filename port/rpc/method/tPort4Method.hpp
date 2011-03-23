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
#include "core/port/rpc/tMethodCallException.h"
#include "core/port/rpc/tInterfaceServerPort.h"
#include "core/port/rpc/tRPCThreadPool.h"

namespace finroc
{
namespace core
{
template<typename HANDLER, typename R, typename P1, typename P2, typename P3, typename P4>
tPort4Method<HANDLER, R, P1, P2, P3, P4>::tPort4Method(tPortInterface& port_interface, const util::tString& name, const util::tString& p1_name, const util::tString& p2_name, const util::tString& p3_name, const util::tString& p4_name, bool handle_in_extra_thread, int default_net_timeout) :
    tAbstractNonVoidMethod(port_interface, name, p1_name, p2_name, p3_name, p4_name, handle_in_extra_thread, default_net_timeout)
{
}

template<typename HANDLER, typename R, typename P1, typename P2, typename P3, typename P4>
R tPort4Method<HANDLER, R, P1, P2, P3, P4>::Call(tInterfaceClientPort port, tP1Arg p1, tP2Arg p2, tP3Arg p3, tP4Arg p4, int net_timeout)
{
  //1
  assert((HasLock(p1)));  //2
  assert((HasLock(p2)));  //3
  assert((HasLock(p3)));  //4
  assert((HasLock(p4)));
  //n
  tInterfacePort* ip = port.GetServer();
  if (ip != NULL && ip->GetType() == tInterfacePort::eNetwork)
  {
    tMethodCall* mc = tThreadLocalCache::GetFast()->GetUnusedMethodCall();
    //1
    mc->AddParam(0, p1);  //2
    mc->AddParam(1, p2);  //3
    mc->AddParam(2, p3);  //4
    mc->AddParam(3, p4);
    //n
    mc->PrepareSyncRemoteExecution(this, port.GetDataType(), net_timeout > 0 ? net_timeout : GetDefaultNetTimeout());
    try
    {
      mc = (static_cast<tInterfaceNetPort*>(ip))->SynchCallOverTheNet(mc, mc->GetNetTimeout());
    }
    catch (const tMethodCallException& e)
    {
      // we shouldn't need to recycle anything, since call is responsible for this
      throw tMethodCallException(e.GetType(), CODE_LOCATION_MACRO);
    }
    if (mc->HasException())
    {
      int8 type = 0;

      mc->GetParam(0, type);

      mc->Recycle();
      throw tMethodCallException(type, CODE_LOCATION_MACRO);
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
      //1
      Cleanup(p1);  //2
      Cleanup(p2);  //3
      Cleanup(p3);  //4
      Cleanup(p4);
      //n
      throw tMethodCallException(tMethodCallException::eNO_CONNECTION, CODE_LOCATION_MACRO);
    }
    R ret = handler->HandleCall(this, p1, p2, p3, p4);
    assert((HasLock(ret)));
    return ret;
  }
  else
  {
    //1
    Cleanup(p1);  //2
    Cleanup(p2);  //3
    Cleanup(p3);  //4
    Cleanup(p4);
    //n
    throw tMethodCallException(tMethodCallException::eNO_CONNECTION, CODE_LOCATION_MACRO);
  }
}

template<typename HANDLER, typename R, typename P1, typename P2, typename P3, typename P4>
void tPort4Method<HANDLER, R, P1, P2, P3, P4>::CallAsync(const tInterfaceClientPort* port, tAsyncReturnHandler<R>* handler, tP1Arg p1, tP2Arg p2, tP3Arg p3, tP4Arg p4, int net_timeout, bool force_same_thread)
{
  //1
  assert((HasLock(p1)));  //2
  assert((HasLock(p2)));  //3
  assert((HasLock(p3)));  //4
  assert((HasLock(p4)));
  //n
  tInterfacePort* ip = port->GetServer();
  if (ip != NULL && ip->GetType() == tInterfacePort::eNetwork)
  {
    tMethodCall* mc = tThreadLocalCache::GetFast()->GetUnusedMethodCall();
    //1
    mc->AddParam(0, p1);  //2
    mc->AddParam(1, p2);  //3
    mc->AddParam(2, p3);  //4
    mc->AddParam(3, p4);
    //n
    mc->PrepareSyncRemoteExecution(this, port->GetDataType(), handler, static_cast<tInterfaceNetPort*>(ip), net_timeout > 0 ? net_timeout : GetDefaultNetTimeout());  // always do this in extra thread
    tRPCThreadPool::GetInstance()->ExecuteTask(mc);
  }
  else if (ip != NULL && ip->GetType() == tInterfacePort::eServer)
  {
    HANDLER mhandler = static_cast<HANDLER>((static_cast<tInterfaceServerPort*>(ip))->GetHandler());
    if (mhandler == NULL)
    {
      //1
      Cleanup(p1);  //2
      Cleanup(p2);  //3
      Cleanup(p3);  //4
      Cleanup(p4);
      //n
      handler->HandleMethodCallException(this, tMethodCallException(tMethodCallException::eNO_CONNECTION, CODE_LOCATION_MACRO));
    }
    if (force_same_thread || (!HandleInExtraThread()))
    {
      try
      {
        R ret = mhandler->HandleCall(this, p1, p2, p3, p4);
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
      //1
      mc->AddParam(0, p1);  //2
      mc->AddParam(1, p2);  //3
      mc->AddParam(2, p3);  //4
      mc->AddParam(3, p4);
      //n
      mc->PrepareExecution(this, port->GetDataType(), mhandler, handler);
      tRPCThreadPool::GetInstance()->ExecuteTask(mc);
    }
  }
  else
  {
    //1
    Cleanup(p1);  //2
    Cleanup(p2);  //3
    Cleanup(p3);  //4
    Cleanup(p4);
    //n
    handler->HandleMethodCallException(this, tMethodCallException(tMethodCallException::eNO_CONNECTION, CODE_LOCATION_MACRO));
  }
}

template<typename HANDLER, typename R, typename P1, typename P2, typename P3, typename P4>
void tPort4Method<HANDLER, R, P1, P2, P3, P4>::ExecuteAsyncNonVoidCallOverTheNet(tMethodCall* mc, tInterfaceNetPort* net_port, tAbstractAsyncReturnHandler* ret_handler, int net_timeout)
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
  if (mc->HasException())
  {
    int8 type = 0;

    mc->GetParam(0, type);

    mc->Recycle();

    r_handler->HandleMethodCallException(this, tMethodCallException(type, CODE_LOCATION_MACRO));
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

template<typename HANDLER, typename R, typename P1, typename P2, typename P3, typename P4>
void tPort4Method<HANDLER, R, P1, P2, P3, P4>::ExecuteFromMethodCallObject(tMethodCall* call, tAbstractMethodCallHandler* handler, tAbstractAsyncReturnHandler* ret_handler)
{
  HANDLER h2 = static_cast<HANDLER>(handler);
  tAsyncReturnHandler<R>* rh2 = static_cast<tAsyncReturnHandler<R>*>(ret_handler);
  ExecuteFromMethodCallObject(call, h2, rh2, false);
}

template<typename HANDLER, typename R, typename P1, typename P2, typename P3, typename P4>
void tPort4Method<HANDLER, R, P1, P2, P3, P4>::ExecuteFromMethodCallObject(tMethodCall* call, const HANDLER& handler, tAsyncReturnHandler<R>* ret_handler, bool dummy)
{
  assert((call != NULL && handler != NULL));
  HANDLER handler2 = handler;

  //1
  P1 p1;  //2
  P2 p2;  //3
  P3 p3;  //4
  P4 p4;
  //n
  //n
  //n
  //n
  //n

  //1
  call->GetParam(0, p1); //2
  call->GetParam(1, p2); //3
  call->GetParam(2, p3); //4
  call->GetParam(3, p4);
  //n

  try
  {
    R ret = handler2->HandleCall(this, p1, p2, p3, p4);
    if (ret_handler != NULL)
    {
      call->Recycle();
      ret_handler->HandleReturn(this, ret);
    }
    else
    {
      call->RecycleParameters();
      call->SetStatusReturn();
      call->AddParam(0, ret);
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

