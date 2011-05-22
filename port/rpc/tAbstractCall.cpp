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
#include "core/port/rpc/tAbstractCall.h"
#include "rrlib/serialization/tInputStream.h"
#include "rrlib/serialization/tOutputStream.h"
#include "core/datatype/tNumber.h"
#include "core/port/rpc/tMethodCallSyncher.h"

namespace finroc
{
namespace core
{
const int8 tAbstractCall::cNONE, tAbstractCall::cSYNCH_CALL, tAbstractCall::cASYNCH_CALL, tAbstractCall::cSYNCH_RETURN, tAbstractCall::cASYNCH_RETURN, tAbstractCall::cCONNECTION_EXCEPTION;

const util::tString const_array_tAbstractCall_0[6] = {"NONE", "SYNCH_CALL", "ASYNCH_CALL", "SYNCH_RETURN", "ASYNCH_RETURN"
    , "CONNECTION_EXCEPTION"
                                                     };
::finroc::util::tArrayWrapper<util::tString> tAbstractCall::cSTATUS_STRINGS(const_array_tAbstractCall_0, 6);
const size_t tAbstractCall::cMAX_PARAMS;

tAbstractCall::tAbstractCall() :
    syncher_iD(-1),
    thread_uid(0),
    method_call_index(0),
    local_port_handle(0),
    remote_port_handle(0),
    status(cNONE)
{
  //callerStack = new CallStack(maxCallDepth);
}

void tAbstractCall::DeserializeImpl(rrlib::serialization::tInputStream& is, bool skip_parameters)
{
  status = is.ReadByte();
  syncher_iD = is.ReadByte();
  thread_uid = is.ReadInt();
  method_call_index = is.ReadShort();

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

tPortDataPtr<rrlib::serialization::tGenericObject> tAbstractCall::GetParamGeneric(int index)
{
  tCallParameter& p = params[index];
  if (p.type == tCallParameter::cNULLPARAM || p.value == NULL)
  {
    return tPortDataPtr<rrlib::serialization::tGenericObject>();
  }
  else
  {
    return std::move(p.value);
  }
}

void tAbstractCall::Recycle()
{
  RecycleParameters();
  method_call_index = -1;
  remote_port_handle = -1;
  status = cNONE;
  syncher_iD = -1;
  thread_uid = -1;
  ::finroc::util::tReusable::Recycle();
}

void tAbstractCall::RecycleParameters()
{
  for (size_t i = 0u; i < cMAX_PARAMS; i++)
  {
    params[i].Recycle();
  }
}

void tAbstractCall::Serialize(rrlib::serialization::tOutputStream& oos) const
{
  oos.WriteByte(status);
  oos.WriteByte(syncher_iD);
  oos.WriteInt(thread_uid);
  oos.WriteShort(method_call_index);

  // Serialize parameters
  for (size_t i = 0u; i < cMAX_PARAMS; i++)
  {
    params[i].Serialize(oos);
  }
}

void tAbstractCall::SetExceptionStatus(int8 type_id)
{
  RecycleParameters();
  SetStatus(cCONNECTION_EXCEPTION);
  params[0].type = tCallParameter::cNUMBER;
  params[0].value.reset();
  params[0].number.SetValue(type_id);
}

void tAbstractCall::SetupAsynchCall()
{
  status = cASYNCH_CALL;
  thread_uid = -1;
  SetSyncherID(-1);
  //callerStack.setSize(0);
}

void tAbstractCall::SetupSynchCall(tMethodCallSyncher* mcs)
{
  status = cSYNCH_CALL;
  thread_uid = mcs->GetThreadUid();
  SetSyncherID(mcs->GetIndex());
  SetMethodCallIndex(mcs->GetAndUseNextCallIndex());
  //callerStack.setSize(0);
}

} // namespace finroc
} // namespace core

