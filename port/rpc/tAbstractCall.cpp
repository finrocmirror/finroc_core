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
#include "core/port/std/tPortDataImpl.h"
#include "core/port/std/tPortDataManager.h"
#include "rrlib/finroc_core_utils/log/tLogUser.h"
#include "core/port/tThreadLocalCache.h"
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
    param_storage(150u, 1),
    os(&(param_storage)),
    is(&(param_storage)),
    responsibilities(),
    deserializable_parameters(false),
    status(cNONE)
{
  //callerStack = new CallStack(maxCallDepth);
}

void tAbstractCall::DeserializeImpl(tCoreInput* is_, bool skip_parameters)
{
  status = is_->ReadByte();
  syncher_iD = is_->ReadByte();
  thread_uid = is_->ReadInt();
  method_call_index = is_->ReadShort();

  // deserialize parameters
  if (skip_parameters)
  {
    return;
  }
  param_storage.Deserialize(*is_);
  int resp_size = is_->ReadByte();
  assert((responsibilities.Size() == 0));
  for (int i = 0; i < resp_size; i++)
  {
    tPortData* p = static_cast<tPortData*>(is_->ReadObject());
    p->GetManager()->GetCurrentRefCounter()->SetLocks(static_cast<int8>(1));  // one lock for us
    responsibilities.Add(p);
  }

  deserializable_parameters = true;
}

void tAbstractCall::DeserializeParamaters()
{
  if (!deserializable_parameters)
  {
    FINROC_LOG_STREAM(rrlib::logging::eLL_DEBUG_WARNING, log_domain, "warning: double deserialization of parameters");
    return;
  }
  deserializable_parameters = false;
  is.Reset();
  int cur_param = 0;
  int cur_resp = 0;
  tCCInterThreadContainer<>* container = NULL;
  while (is.MoreDataAvailable())
  {
    tCallParameter* p = &(params[cur_param]);
    p->type = is.ReadByte();
    switch (p->type)
    {
    case tCallParameter::cNULLPARAM:
      p->value = NULL;
      break;
    case tCallParameter::cINT:

      p->ival = is.ReadInt();
      break;
    case tCallParameter::cLONG:

      p->lval = is.ReadLong();
      break;
    case tCallParameter::cFLOAT:

      p->fval = is.ReadFloat();
      break;
    case tCallParameter::cDOUBLE:

      p->dval = is.ReadDouble();
      break;
    case tCallParameter::cBYTE:

      p->bval = is.ReadByte();
      break;
    case tCallParameter::cSHORT:

      p->sval = is.ReadShort();
      break;
    case tCallParameter::cCCDATA:
    case tCallParameter::cCCCONTAINER:
      container = static_cast<tCCInterThreadContainer<>*>(is.ReadObjectInInterThreadContainer());

      p->ccval = container;
      break;
    case tCallParameter::cPORTDATA:
      p->value = responsibilities.Get(cur_resp);

      p->value->GetManager()->AddLock();

      cur_resp++;
      break;
    }

    cur_param++;
  }
}

tCCInterThreadContainer<>* tAbstractCall::GetInterThreadBuffer(tDataType* dt)
{
  return tThreadLocalCache::GetFast()->GetUnusedInterThreadBuffer(dt);
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
  param_storage.Clear();
  os.Reset(&(param_storage));
  for (size_t i = 0u; i < responsibilities.Size(); i++)
  {
    responsibilities.Get(i)->GetManager()->ReleaseLock();
  }
  responsibilities.Clear();
  for (size_t i = 0u; i < cMAX_PARAMS; i++)
  {
    params[i].Recycle();
  }
  deserializable_parameters = false;
}

void tAbstractCall::Serialize(tCoreOutput& oos) const
{
  oos.WriteByte(status);
  oos.WriteByte(syncher_iD);
  oos.WriteInt(thread_uid);
  oos.WriteShort(method_call_index);

  // Serialize parameters
  param_storage.Serialize(oos);
  oos.WriteByte(responsibilities.Size());
  for (size_t i = 0u; i < responsibilities.Size(); i++)
  {
    oos.WriteObject(responsibilities.Get(i));
  }
}

void tAbstractCall::SetExceptionStatus(int8 type_id)
{
  RecycleParameters();
  SetStatus(cCONNECTION_EXCEPTION);
  AddParamForSending(type_id);
  params[0].type = tCallParameter::cBYTE;

  params[0].bval = type_id;
  SendParametersComplete();
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

