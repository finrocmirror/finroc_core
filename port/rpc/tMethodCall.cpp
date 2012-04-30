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
#include "core/port/rpc/tMethodCall.h"
#include "core/port/rpc/method/tPortInterface.h"
#include "core/portdatabase/tFinrocTypeInfo.h"
#include "core/port/rpc/tInterfaceNetPort.h"
#include "core/port/rpc/method/tAbstractMethod.h"

namespace finroc
{
namespace core
{
const size_t tMethodCall::cMAX_PARAMS;

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

void tMethodCall::DeserializeCall(rrlib::serialization::tInputStream& is, const rrlib::rtti::tDataTypeBase& dt, bool skip_parameters)
{
  //assert(skipParameters || (dt != null && dt.isMethodType())) : "Method type required here";
  port_interface_type = dt;
  int8 b = is.ReadByte();
  method = (dt == NULL) ? NULL : tFinrocTypeInfo::Get(dt).GetPortInterface()->GetMethod(b);
  net_timeout = is.ReadInt();
  tAbstractCall::DeserializeImpl(is);

  // deserialize parameters
  if (skip_parameters)
  {
    return;
  }
  for (size_t i = 0u; i < cMAX_PARAMS; i++)
  {
    params[i].Deserialize(is);
  }
}

void tMethodCall::ExecuteTask(tSerializableReusableTask::tPtr& self)
{
  tMethodCall::tPtr& self2 = reinterpret_cast<tMethodCall::tPtr&>(self);
  assert(method);
  if (source_net_port)
  {
    if (net_port)    // sync network forward in another thread
    {
      source_net_port->ExecuteNetworkForward(self2, *net_port);
    }
    else    // sync network call in another thread
    {
      source_net_port->ExecuteCallFromNetwork(self2, *handler);
    }
  }
  else if (!net_port)    // async call in another thread
  {
    assert(handler);
    method->ExecuteFromMethodCallObject(self2, *handler, ret_handler);
  }
  else    // sync network call in another thread
  {
    method->ExecuteAsyncNonVoidCallOverTheNet(self2, *net_port, *ret_handler, net_timeout);
  }
}

tPortDataPtr<rrlib::rtti::tGenericObject> tMethodCall::GetParamGeneric(int index)
{
  tCallParameter& p = params[index];
  if (p.type == tCallParameter::cNULLPARAM || p.value == NULL)
  {
    return tPortDataPtr<rrlib::rtti::tGenericObject>();
  }
  else
  {
    return std::move(p.value);
  }
}

void tMethodCall::PrepareExecution(tAbstractMethod* method_, const rrlib::rtti::tDataTypeBase& port_interface, tAbstractMethodCallHandler* handler_, tAbstractAsyncReturnHandler* ret_handler_)
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

void tMethodCall::PrepareSyncRemoteExecution(tAbstractMethod* method_, const rrlib::rtti::tDataTypeBase& port_interface, tAbstractAsyncReturnHandler* ret_handler_, tInterfaceNetPort* net_port_, int net_timeout_)
{
  assert((this->method == NULL && this->handler == NULL && method_ != NULL));
  this->method = method_;
  this->port_interface_type = port_interface;
  assert((TypeCheck()));
  this->ret_handler = ret_handler_;
  this->net_port = net_port_;
  this->net_timeout = net_timeout_;
}

void tMethodCall::PrepareSyncRemoteExecution(tAbstractMethod* method_, const rrlib::rtti::tDataTypeBase& port_interface, int net_timeout_)
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
  RecycleParameters();
  tAbstractCall::Recycle();
}

void tMethodCall::RecycleParameters()
{
  for (size_t i = 0u; i < cMAX_PARAMS; i++)
  {
    params[i].Recycle();
  }
}

void tMethodCall::Serialize(rrlib::serialization::tOutputStream& oos) const
{
  oos.WriteByte(method == NULL ? -1 : method->GetMethodId());
  assert(((GetStatus() != tStatus::SYNCH_CALL || net_timeout > 0)) && "Network timeout needs to be >0 with a synch call");
  oos.WriteInt(net_timeout);
  tAbstractCall::Serialize(oos);

  // Serialize parameters
  for (size_t i = 0u; i < cMAX_PARAMS; i++)
  {
    params[i].Serialize(oos);
  }
}

void tMethodCall::SetMethod(tAbstractMethod* m, const rrlib::rtti::tDataTypeBase& port_interface)
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

