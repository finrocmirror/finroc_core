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
#include "core/port/rpc/method/tPortInterface.h"

#include "core/port/rpc/tMethodCall.h"

namespace finroc
{
namespace core
{
tMethodCall::tMethodCall() :
    tAbstractCall(),
    method(NULL),
    port_interface_type(NULL),
    handler(NULL),
    ret_handler(NULL),
    net_port(NULL),
    net_timeout(0),
    source_net_port(NULL)
{
  //System.out.println("New method call");

  // type = cMETHOD_TYPE;

  //tParams[0] = null;
  //tParams[1] = null;
}

void tMethodCall::DeserializeCall(tCoreInput* is, tDataType* dt, bool skip_parameters)
{
  //assert(skipParameters || (dt != null && dt.isMethodType())) : "Method type required here";
  port_interface_type = dt;
  int8 b = is->ReadByte();
  method = (dt == NULL) ? NULL : dt->GetPortInterface()->GetMethod(b);
  net_timeout = is->ReadInt();
  ::finroc::core::tAbstractCall::DeserializeImpl(is, skip_parameters);
  //      methodID = is.readByte();
  //      autoRecycleRetVal = is.readBoolean();
  //      byte mask = 0;
  //      switch(status) {
  //      case SYNCH_CALL:
  //      case ASYNCH_CALL:
  //          mask = is.readByte();
  //          tCount = (byte)((mask >> 6) & 3);
  //          iCount = (byte)((mask >> 4) & 3);
  //          dCount = (byte)((mask >> 2) & 3);
  //          rType = (byte)(mask & 3);
  //          for (int i = 0; i < tCount; i++) {
  //              tParams[i] = readObject(is);
  //          }
  //          for (int i = 0; i < iCount; i++) {
  //              iParams[i] = is.readLong();
  //          }
  //          for (int i = 0; i < dCount; i++) {
  //              dParams[i] = is.readLong();
  //          }
  //          break;
  //      case ASYNCH_RETURN:
  //      case SYNCH_RETURN:
  //          rType = is.readByte();
  //          assert(rType != NONE);
  //          ri = is.readLong();
  //          rd = is.readDouble();
  //          rt = readObject(is);
  //          break;
  //      case CONNECTION_EXCEPTION:
  //          break;
  //      }
  //
  //      // reset some values
  //      arrivalTime = 0;
  //      alreadyDeferred = false;
}

void tMethodCall::ExecuteTask()
{
  assert((method != NULL));
  if (source_net_port != NULL)
  {
    if (net_port != NULL)    // sync network forward in another thread
    {
      source_net_port->ExecuteNetworkForward(this, net_port);
    }
    else    // sync network call in another thread
    {
      source_net_port->ExecuteCallFromNetwork(this, handler);
    }
  }
  else if (net_port == NULL)    // async call in another thread
  {
    assert((handler != NULL));
    method->ExecuteFromMethodCallObject(this, handler, ret_handler);
  }
  else    // sync network call in another thread
  {
    method->ExecuteAsyncNonVoidCallOverTheNet(this, net_port, ret_handler, net_timeout);
  }
}

void tMethodCall::PrepareExecution(tAbstractMethod* method_, tDataType* port_interface, tAbstractMethodCallHandler* handler_, tAbstractAsyncReturnHandler* ret_handler_)
{
  assert((this->method == NULL && this->handler == NULL && method_ != NULL));
  this->method = method_;
  this->port_interface_type = port_interface;
  assert((TypeCheck()));
  this->handler = handler_;
  this->ret_handler = ret_handler_;
}

void tMethodCall::PrepareExecutionForCallFromNetwork(tInterfaceNetPort* source, tAbstractMethodCallHandler* mhandler)
{
  assert((method != NULL));
  this->source_net_port = source;
  this->handler = mhandler;
  this->net_port = NULL;
}

void tMethodCall::PrepareForwardSyncRemoteExecution(tInterfaceNetPort* source, tInterfaceNetPort* dest)
{
  assert((ret_handler == NULL && method != NULL));
  this->source_net_port = source;
  this->net_port = dest;
}

void tMethodCall::PrepareSyncRemoteExecution(tAbstractMethod* method_, tDataType* port_interface, tAbstractAsyncReturnHandler* ret_handler_, tInterfaceNetPort* net_port_, int net_timeout_)
{
  assert((this->method == NULL && this->handler == NULL && method_ != NULL));
  this->method = method_;
  this->port_interface_type = port_interface;
  assert((TypeCheck()));
  this->ret_handler = ret_handler_;
  this->net_port = net_port_;
  this->net_timeout = net_timeout_;
}

void tMethodCall::PrepareSyncRemoteExecution(tAbstractMethod* method_, tDataType* port_interface, int net_timeout_)
{
  assert((this->method == NULL && this->handler == NULL && method_ != NULL));
  this->method = method_;
  this->port_interface_type = port_interface;
  assert((TypeCheck()));
  this->net_timeout = net_timeout_;
}

void tMethodCall::Recycle()
{
  method = NULL;
  handler = NULL;
  ret_handler = NULL;
  net_port = NULL;
  net_timeout = -1;
  source_net_port = NULL;
  ::finroc::core::tAbstractCall::Recycle();
}

void tMethodCall::Serialize(tCoreOutput& oos) const
{
  oos.WriteByte(method == NULL ? -1 : method->GetMethodId());
  assert(((GetStatus() != cSYNCH_CALL || net_timeout > 0)) && "Network timeout needs to be >0 with a synch call");
  oos.WriteInt(net_timeout);
  ::finroc::core::tAbstractCall::Serialize(oos);

  //      oos.writeBoolean(autoRecycleRetVal);
  //      byte mask = 0;
  //      switch(status) {
  //      case SYNCH_CALL:
  //      case ASYNCH_CALL:
  //          mask = (byte)((tCount << 6) | (iCount << 4) | (dCount << 2) | rType);
  //          oos.writeByte(mask);
  //          for (int i = 0; i < tCount; i++) {
  //              oos.writeObject(tParams[i]);
  //          }
  //          for (int i = 0; i < iCount; i++) {
  //              oos.writeLong(iParams[i]);
  //          }
  //          for (int i = 0; i < dCount; i++) {
  //              oos.writeDouble(dParams[i]);
  //          }
  //          break;
  //      case ASYNCH_RETURN:
  //      case SYNCH_RETURN:
  //          oos.writeByte(rType);
  //          assert(rType != NONE);
  //          // TODO optimize
  //          oos.writeLong(ri);
  //          oos.writeDouble(rd);
  //          oos.writeObject(rt);
  //          break;
  //      case CONNECTION_EXCEPTION:
  //          break;
  //      }
}

void tMethodCall::SetMethod(tAbstractMethod* m, tDataType* port_interface)
{
  method = m;
  port_interface_type = port_interface;
  assert((TypeCheck()));
}

bool tMethodCall::TypeCheck()
{
  return method != NULL && port_interface_type != NULL && port_interface_type->GetPortInterface() != NULL && port_interface_type->GetPortInterface()->ContainsMethod(method);
}

} // namespace finroc
} // namespace core

