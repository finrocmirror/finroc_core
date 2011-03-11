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
#include "core/port/rpc/tMethodCall.h"
#include "core/buffers/tCoreInput.h"
#include "core/port/rpc/method/tPortInterface.h"
#include "core/portdatabase/tFinrocTypeInfo.h"
#include "core/port/rpc/tInterfaceNetPort.h"
#include "core/port/rpc/method/tAbstractMethod.h"
#include "core/buffers/tCoreOutput.h"

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
}

void tMethodCall::DeserializeCall(tCoreInput* is, const rrlib::serialization::tDataTypeBase& dt, bool skip_parameters)
{
  //assert(skipParameters || (dt != null && dt.isMethodType())) : "Method type required here";
  port_interface_type = dt;
  int8 b = is->ReadByte();
  method = (dt == NULL) ? NULL : tFinrocTypeInfo::Get(dt).GetPortInterface()->GetMethod(b);
  net_timeout = is->ReadInt();
  ::finroc::core::tAbstractCall::DeserializeImpl(is, skip_parameters);
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

void tMethodCall::PrepareExecution(tAbstractMethod* method_, const rrlib::serialization::tDataTypeBase& port_interface, tAbstractMethodCallHandler* handler_, tAbstractAsyncReturnHandler* ret_handler_)
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

void tMethodCall::PrepareSyncRemoteExecution(tAbstractMethod* method_, const rrlib::serialization::tDataTypeBase& port_interface, tAbstractAsyncReturnHandler* ret_handler_, tInterfaceNetPort* net_port_, int net_timeout_)
{
  assert((this->method == NULL && this->handler == NULL && method_ != NULL));
  this->method = method_;
  this->port_interface_type = port_interface;
  assert((TypeCheck()));
  this->ret_handler = ret_handler_;
  this->net_port = net_port_;
  this->net_timeout = net_timeout_;
}

void tMethodCall::PrepareSyncRemoteExecution(tAbstractMethod* method_, const rrlib::serialization::tDataTypeBase& port_interface, int net_timeout_)
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
}

void tMethodCall::SetMethod(tAbstractMethod* m, const rrlib::serialization::tDataTypeBase& port_interface)
{
  method = m;
  port_interface_type = port_interface;
  assert((TypeCheck()));
}

bool tMethodCall::TypeCheck()
{
  return method != NULL && port_interface_type != NULL && tFinrocTypeInfo::Get(port_interface_type).GetPortInterface() != NULL && tFinrocTypeInfo::Get(port_interface_type).GetPortInterface()->ContainsMethod(method);
}

} // namespace finroc
} // namespace core

