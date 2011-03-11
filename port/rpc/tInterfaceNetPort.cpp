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
#include "core/port/rpc/tInterfaceNetPort.h"
#include "core/port/rpc/method/tAbstractMethod.h"
#include "core/port/rpc/tMethodCall.h"
#include "core/port/rpc/tMethodCallException.h"
#include "core/port/rpc/tRPCThreadPool.h"
#include "core/port/rpc/method/tAbstractMethodCallHandler.h"
#include "core/port/rpc/tInterfaceServerPort.h"

namespace finroc
{
namespace core
{
tInterfaceNetPort::tInterfaceNetPort(tPortCreationInfo pci) :
    tInterfacePort(pci, ::finroc::core::tInterfacePort::eNetwork, -1)
{
}

void tInterfaceNetPort::ExecuteCallFromNetwork(tMethodCall* mc, tAbstractMethodCallHandler* mhandler)
{
  mc->GetMethod()->ExecuteFromMethodCallObject(mc, mhandler, NULL);
  if (!mc->GetMethod()->IsVoidMethod())
  {
    SendSyncCallReturn(mc);
  }
}

void tInterfaceNetPort::ExecuteNetworkForward(tMethodCall* mc, tInterfaceNetPort* net_port)
{
  try
  {
    mc = net_port->SynchCallOverTheNet(mc, mc->GetNetTimeout());
  }
  catch (const tMethodCallException& e)
  {
    mc->SetExceptionStatus(e.GetTypeId());
  }
  SendSyncCallReturn(mc);
}

void tInterfaceNetPort::ProcessCallFromNet(tMethodCall* mc)
{
  ::finroc::core::tInterfacePort* ip = GetServer();
  tAbstractMethod* m = mc->GetMethod();
  if (ip != NULL && ip->GetType() == ::finroc::core::tInterfacePort::eNetwork)
  {
    tInterfaceNetPort* inp = static_cast<tInterfaceNetPort*>(ip);
    if (m->IsVoidMethod())
    {
      inp->SendAsyncCall(mc);
    }
    else
    {
      mc->PrepareForwardSyncRemoteExecution(this, inp);  // always do this in extra thread
      tRPCThreadPool::GetInstance()->ExecuteTask(mc);
      SendSyncCallReturn(mc);
    }
  }
  else if (ip != NULL && ip->GetType() == ::finroc::core::tInterfacePort::eServer)
  {
    tAbstractMethodCallHandler* mhandler = static_cast<tAbstractMethodCallHandler*>((static_cast<tInterfaceServerPort*>(ip))->GetHandler());
    if (mhandler == NULL)
    {
      if (m->IsVoidMethod())
      {
        mc->Recycle();
      }
      else
      {
        mc->SetExceptionStatus(tMethodCallException::eNO_CONNECTION);
        SendSyncCallReturn(mc);
      }
    }
    else
    {
      if (mc->GetMethod()->HandleInExtraThread())
      {
        mc->PrepareExecutionForCallFromNetwork(this, mhandler);
        tRPCThreadPool::GetInstance()->ExecuteTask(mc);
        return;
      }
      else
      {
        mc->GetMethod()->ExecuteFromMethodCallObject(mc, mhandler, NULL);
        if (!m->IsVoidMethod())
        {
          SendSyncCallReturn(mc);
        }
      }
    }
  }
  else
  {
    if (m->IsVoidMethod())
    {
      mc->Recycle();
    }
    else
    {
      mc->SetExceptionStatus(tMethodCallException::eNO_CONNECTION);
      SendSyncCallReturn(mc);
    }
  }
}

} // namespace finroc
} // namespace core

