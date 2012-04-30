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
#include "core/port/rpc/tAbstractCall.h"
#include "rrlib/serialization/serialization.h"
#include "core/datatype/tNumber.h"
#include "core/port/rpc/tMethodCallSyncher.h"

namespace finroc
{
namespace core
{

tAbstractCall::tAbstractCall() :
  syncher_id(-1),
  thread_uid(-1),
  status(tStatus::NONE),
  exception_type(tMethodCallException::tType::NONE),
  method_call_index(-1),
  local_port_handle(-1),
  remote_port_handle(-1)
{
}

void tAbstractCall::DeserializeImpl(rrlib::serialization::tInputStream& is)
{
  is >> status >> exception_type;
  syncher_id = is.ReadByte();
  thread_uid = is.ReadInt();
  method_call_index = is.ReadShort();
}

void tAbstractCall::Recycle()
{
  method_call_index = -1;
  remote_port_handle = -1;
  status = tStatus::NONE;
  exception_type = tMethodCallException::tType::NONE;
  syncher_id = -1;
  thread_uid = -1;
  tReusable::Recycle();
}

void tAbstractCall::Serialize(rrlib::serialization::tOutputStream& oos) const
{
  oos << status << exception_type;
  oos.WriteByte(syncher_id);
  oos.WriteInt(thread_uid);
  oos.WriteShort(method_call_index);
}

void tAbstractCall::SetExceptionStatus(tMethodCallException::tType type)
{
  SetStatus(tStatus::EXCEPTION);
  exception_type = type;
}

void tAbstractCall::SetupAsynchCall()
{
  status = tStatus::ASYNCH_CALL;
  thread_uid = -1;
  SetSyncherID(-1);
  //callerStack.setSize(0);
}

void tAbstractCall::SetupSynchCall(tMethodCallSyncher& mcs)
{
  status = tStatus::SYNCH_CALL;
  thread_uid = mcs.GetThreadUid();
  SetSyncherID(mcs.GetIndex());
  SetMethodCallIndex(mcs.GetAndUseNextCallIndex());
  //callerStack.setSize(0);
}

} // namespace finroc
} // namespace core

