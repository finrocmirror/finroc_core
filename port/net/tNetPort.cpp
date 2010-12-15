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
#include "core/portdatabase/tDataType.h"
#include "core/port/net/tNetPort.h"
#include "core/buffers/tCoreInput.h"
#include "core/port/std/tPortData.h"
#include "core/buffers/tCoreOutput.h"
#include "core/port/std/tPortDataManager.h"
#include "core/port/std/tPortQueueFragment.h"
#include "core/port/tThreadLocalCache.h"
#include "core/port/std/tPortDataReference.h"
#include "core/port/cc/tCCInterThreadContainer.h"
#include "core/port/cc/tCCQueueFragment.h"
#include "core/port/cc/tCCPortData.h"
#include "core/port/rpc/tMethodCallException.h"

namespace finroc
{
namespace core
{
const int tNetPort::cPULL_TIMEOUT;

tNetPort::tNetPort(tPortCreationInfo pci, util::tObject* belongs_to_) :
    wrapped(NULL),
    belongs_to(belongs_to_),
    remote_handle(0),
    last_update(util::tLong::cMIN_VALUE)
{
  // keep most these flags
  int f = pci.flags & (tPortFlags::cACCEPTS_DATA | tPortFlags::cEMITS_DATA | tPortFlags::cMAY_ACCEPT_REVERSE_DATA | tPortFlags::cIS_OUTPUT_PORT | tPortFlags::cIS_BULK_PORT | tPortFlags::cIS_EXPRESS_PORT | tPortFlags::cNON_STANDARD_ASSIGN | tCoreFlags::cALTERNATE_LINK_ROOT | tCoreFlags::cGLOBALLY_UNIQUE_LINK | tCoreFlags::cFINSTRUCTED);

  // set either emit or accept data
  f |= ((f & tPortFlags::cIS_OUTPUT_PORT) > 0) ? tPortFlags::cEMITS_DATA : tPortFlags::cACCEPTS_DATA;
  f |= tCoreFlags::cNETWORK_ELEMENT | tPortFlags::cIS_VOLATILE;
  if ((f & tPortFlags::cIS_OUTPUT_PORT) == 0)    // we always have a queue with (remote) input ports - to be able to switch
  {
    f |= tPortFlags::cHAS_QUEUE;
    if (pci.max_queue_size > 1)
    {
      f |= tPortFlags::cUSES_QUEUE;
    }
  }
  if (pci.data_type->IsStdType() || pci.data_type->IsMethodType())
  {
    f |= tPortFlags::cSPECIAL_REUSE_QUEUE;  // different data types may be incoming - cc types are thread local
  }
  pci.flags = f;
  wrapped = (pci.data_type->IsMethodType() ? static_cast<tAbstractPort*>(new tInterfaceNetPortImpl(this, pci)) : (pci.data_type->IsCCType() ? static_cast<tAbstractPort*>(new tCCNetPort(this, pci)) : static_cast<tAbstractPort*>(new tStdNetPort(this, pci))));
}

void tNetPort::PropagateStrategyFromTheNet(int16 strategy)
{
  if (!GetPort()->IsOutputPort())    // only input ports are relevant for strategy changes
  {
    tAbstractPort* ap = GetPort();
    if (typeid(*ap) == typeid(tStdNetPort))
    {
      (static_cast<tStdNetPort*>(ap))->PropagateStrategy(strategy);
    }
    else if (typeid(*ap) == typeid(tCCNetPort))
    {
      (static_cast<tCCNetPort*>(ap))->PropagateStrategy(strategy);
    }
    else
    {
      // ignore
    }
  }
}

void tNetPort::ReceiveDataFromStream(tCoreInput* ci, int64 timestamp, int8 changed_flag)
{
  assert((GetPort()->IsReady()));
  if (wrapped->GetDataType()->IsStdType() || wrapped->GetDataType()->IsTransactionType())
  {
    tStdNetPort* pb = static_cast<tStdNetPort*>(wrapped);
    ci->SetBufferSource(pb);
    do
    {
      pb->PublishFromNet(static_cast<tPortData*>(ci->ReadObject()), changed_flag);
    }
    while (ci->ReadBoolean());
    ci->SetBufferSource(NULL);
  }
  else if (wrapped->GetDataType()->IsCCType())
  {
    tCCNetPort* pb = static_cast<tCCNetPort*>(wrapped);
    do
    {
      pb->PublishFromNet(static_cast<tCCPortDataContainer<>*>(ci->ReadObject()), changed_flag);
    }
    while (ci->ReadBoolean());
  }
  else    // interface port
  {
    throw util::tRuntimeException("Method calls are not handled using this mechanism", CODE_LOCATION_MACRO);
  }
}

void tNetPort::UpdateFlags(int flags)
{
  // process flags... keep DELETE and READY flags
  int keep_flags = tCoreFlags::cSTATUS_FLAGS;
  int cur_flags = GetPort()->GetAllFlags() & keep_flags;
  flags &= ~(keep_flags);
  flags |= cur_flags;

  if (wrapped->GetDataType()->IsStdType() || wrapped->GetDataType()->IsTransactionType())
  {
    (static_cast<tStdNetPort*>(wrapped))->UpdateFlags(flags);
  }
  else if (wrapped->GetDataType()->IsCCType())
  {
    (static_cast<tCCNetPort*>(wrapped))->UpdateFlags(flags);
  }
  else
  {
    (static_cast<tInterfaceNetPortImpl*>(wrapped))->UpdateFlags(flags);
  }
}

void tNetPort::WriteDataToNetwork(tCoreOutput* co, int64 start_time)
{
  bool use_q = wrapped->GetFlag(tPortFlags::cUSES_QUEUE);
  bool first = true;
  if (wrapped->GetDataType()->IsStdType() || wrapped->GetDataType()->IsTransactionType())
  {
    tStdNetPort* pb = static_cast<tStdNetPort*>(wrapped);
    if (!use_q)
    {
      const tPortData* pd = pb->GetLockedUnsafeRaw();
      co->WriteObject(pd);
      pd->GetManager()->ReleaseLock();
    }
    else
    {
      tPortQueueFragment<tPortData> fragment;
      pb->DequeueAllRaw(fragment);
      const tPortDataReference* pd = NULL;
      while ((pd = static_cast<tPortDataReference*>(fragment.Dequeue())) != NULL)
      {
        if (!first)
        {
          co->WriteBoolean(true);
        }
        first = false;
        co->WriteObject(pd->GetData());
        pd->GetManager()->ReleaseLock();
      }
    }
  }
  else if (wrapped->GetDataType()->IsCCType())
  {
    tCCNetPort* pb = static_cast<tCCNetPort*>(wrapped);
    if (!use_q)
    {
      tCCInterThreadContainer<>* ccitc = pb->GetInInterThreadContainer();
      co->WriteObject(ccitc);
      ccitc->Recycle2();
    }
    else
    {
      tCCQueueFragment<tCCPortData> fragment;
      pb->DequeueAllRaw(fragment);
      tCCInterThreadContainer<>* pd = NULL;
      while ((pd = reinterpret_cast<tCCInterThreadContainer<>*>(fragment.Dequeue())) != NULL)
      {
        if (!first)
        {
          co->WriteBoolean(true);
        }
        first = false;
        co->WriteObject(pd);
        pd->Recycle2();
      }
    }
  }
  else    // interface port
  {
    throw util::tRuntimeException("Method calls are not handled using this mechanism", CODE_LOCATION_MACRO);
  }
  co->WriteBoolean(false);
}

tNetPort::tCCNetPort::tCCNetPort(tNetPort* const outer_class_ptr_, tPortCreationInfo pci) :
    tCCPortBase(pci),
    outer_class_ptr(outer_class_ptr_)
{
  ::finroc::core::tCCPortBase::AddPortListenerRaw(outer_class_ptr);
  ::finroc::core::tCCPortBase::SetPullRequestHandler(this);
}

void tNetPort::tCCNetPort::InitialPushTo(tAbstractPort* target, bool reverse)
{
  if (reverse)
  {
    // do we have other reverse push listeners? - in this case, there won't be coming anything new from the network => immediately push
    util::tArrayWrapper< ::finroc::core::tCCPortBase*>* it = this->edges_dest.GetIterable();
    for (int i = 0, n = it->Size(); i < n; i++)
    {
      ::finroc::core::tAbstractPort* port = it->Get(i);
      if (port != NULL && port != target && port->IsReady() && port->GetFlag(tPortFlags::cPUSH_STRATEGY_REVERSE))
      {
        ::finroc::core::tCCPortBase::InitialPushTo(target, reverse);
        return;
      }
    }

  }
  else
  {
    // do nothing ... since remote port will do this
  }
}

void tNetPort::tCCNetPort::PrepareDelete()
{
  util::tLock lock1(this);
  ::finroc::core::tCCPortBase::RemovePortListenerRaw(outer_class_ptr);
  ::finroc::core::tAbstractPort::PrepareDelete();
  outer_class_ptr->PrepareDelete();
}

bool tNetPort::tCCNetPort::PropagateStrategy(tAbstractPort* push_wanter, tAbstractPort* new_connection_partner)
{
  if (IsOutputPort() && IsInitialized())
  {
    if (::finroc::core::tAbstractPort::PropagateStrategy(NULL, NULL))    // we don't want to push ourselves directly - unless there's no change
    {
      outer_class_ptr->PropagateStrategyOverTheNet();
      return true;
    }
    else
    {
      if (push_wanter != NULL)
      {
        ::finroc::core::tCCPortBase::InitialPushTo(push_wanter, false);
      }
    }
    return false;
  }
  else
  {
    return ::finroc::core::tAbstractPort::PropagateStrategy(push_wanter, new_connection_partner);
  }
}

void tNetPort::tCCNetPort::PropagateStrategy(int16 strategy)
{
  SetFlag(tPortFlags::cPUSH_STRATEGY, strategy > 0);
  //this.strategy = strategy;
  ::finroc::core::tAbstractPort::SetMaxQueueLength(strategy);
}

void tNetPort::tCCNetPort::PublishFromNet(tCCPortDataContainer<>* read_object, int8 changed_flag)
{
  if (changed_flag != ::finroc::core::tAbstractPort::cCHANGED_INITIAL)    // always publish initial pushes
  {

    // only publish if value has actually changed...
    tCCPortDataContainer<>* cur_data = this->GetLockedUnsafeInContainer();
    bool equal = cur_data->ContentEquals(((tCCPortData*)read_object->GetDataPtr()));
    cur_data->ReleaseLock();   // unlock value that we just locked for comparison
    if (equal)
    {
      // laborious version of recycle
      read_object->SetRefCounter(1);
      read_object->ReleaseLock();
      return;
    }
  }

  tThreadLocalCache* tc = tThreadLocalCache::GetFast();
  if (IsOutputPort())
  {
    ::finroc::core::tCCPortBase::Publish(tc, read_object, false, changed_flag);
  }
  else
  {
    // reverse push: reset changed flag - since this change comes from the net and needs not to be propagated
    outer_class_ptr->GetPort()->ResetChanged();
    // not entirely thread-safe: if changed flag is set now - value from the net will be published back - usually not a problem
    // dual-way ports are somewhat ugly anyway

    ::finroc::core::tCCPortBase::Publish(tc, read_object, true, changed_flag);
  }
}

void tNetPort::tCCNetPort::PullRequest(tCCPortBase* origin, void* result_buffer)
{
  tPullCall* pc = tThreadLocalCache::GetFast()->GetUnusedPullCall();
  pc->SetRemotePortHandle(outer_class_ptr->remote_handle);
  //          pc.setLocalPortHandle(getHandle());
  try
  {
    pc = tSynchMethodCallLogic::PerformSynchCall(pc, this, cPULL_TIMEOUT);
    if (pc->HasException())
    {
      GetRaw(((tCCPortData*)result_buffer));
    }
    else
    {
      tCCInterThreadContainer<>* c;

      pc->GetParam(0, c);

      c->AssignTo(((tCCPortData*)result_buffer));
      c->Recycle2();
    }
    pc->Recycle();

  }
  catch (const tMethodCallException& e)
  {
    GetRaw(((tCCPortData*)result_buffer));
  }
}

tNetPort::tStdNetPort::tStdNetPort(tNetPort* const outer_class_ptr_, tPortCreationInfo pci) :
    tPortBase(pci),
    outer_class_ptr(outer_class_ptr_)
{
  ::finroc::core::tPortBase::AddPortListenerRaw(outer_class_ptr);
  ::finroc::core::tPortBase::SetPullRequestHandler(this);
}

void tNetPort::tStdNetPort::InitialPushTo(tAbstractPort* target, bool reverse)
{
  if (reverse)
  {
    // do we have other reverse push listeners? - in this case, there won't be coming anything new from the network => immediately push
    util::tArrayWrapper< ::finroc::core::tPortBase*>* it = this->edges_dest.GetIterable();
    for (int i = 0, n = it->Size(); i < n; i++)
    {
      ::finroc::core::tAbstractPort* port = it->Get(i);
      if (port != NULL && port != target && port->IsReady() && port->GetFlag(tPortFlags::cPUSH_STRATEGY_REVERSE))
      {
        ::finroc::core::tPortBase::InitialPushTo(target, reverse);
        return;
      }
    }

  }
  else
  {
    // do nothing ... since remote port will do this
  }
}

void tNetPort::tStdNetPort::PrepareDelete()
{
  util::tLock lock1(this);
  ::finroc::core::tPortBase::RemovePortListenerRaw(outer_class_ptr);
  ::finroc::core::tAbstractPort::PrepareDelete();
  outer_class_ptr->PrepareDelete();
}

bool tNetPort::tStdNetPort::PropagateStrategy(tAbstractPort* push_wanter, tAbstractPort* new_connection_partner)
{
  if (IsOutputPort() && IsInitialized())
  {
    if (::finroc::core::tAbstractPort::PropagateStrategy(NULL, NULL))    // we don't want to push ourselves directly - unless there's no change
    {
      outer_class_ptr->PropagateStrategyOverTheNet();
      return true;
    }
    else
    {
      if (push_wanter != NULL)
      {
        ::finroc::core::tPortBase::InitialPushTo(push_wanter, false);
      }
    }
    return false;
  }
  else
  {
    return ::finroc::core::tAbstractPort::PropagateStrategy(push_wanter, new_connection_partner);
  }
}

void tNetPort::tStdNetPort::PublishFromNet(tPortData* read_object, int8 changed_flag)
{
  if (!IsOutputPort())
  {
    // reverse push: reset changed flag - since this change comes from the net and needs not to be propagated
    outer_class_ptr->GetPort()->ResetChanged();
    // not entirely thread-safe: if changed flag is set now - value from the net will be published back - usually not a problem
    // dual-way ports are somewhat ugly anyway
  }
  ::finroc::core::tPortBase::Publish(read_object, !IsOutputPort(), changed_flag);
}

const tPortData* tNetPort::tStdNetPort::PullRequest(tPortBase* origin, int8 add_locks)
{
  assert((add_locks > 0));
  tPullCall* pc = tThreadLocalCache::GetFast()->GetUnusedPullCall();
  pc->SetRemotePortHandle(outer_class_ptr->remote_handle);
  //          pc.setLocalPortHandle(getHandle());
  try
  {
    pc = tSynchMethodCallLogic::PerformSynchCall(pc, this, cPULL_TIMEOUT);
    if (pc->HasException())
    {
      // return local port data
      const tPortData* pd = GetLockedUnsafeRaw();
      pd->GetManager()->GetCurrentRefCounter()->AddLocks(static_cast<int8>((add_locks - 1)));  // we already have one lock
      pc->Recycle();
      return pd;
    }
    else
    {
      tPortData* pd;

      pc->GetParam(0, pd);

      pd->GetManager()->GetCurrentRefCounter()->AddLocks(static_cast<int8>((add_locks - 1)));  // we already have one lock
      pc->Recycle();
      return pd;
    }

  }
  catch (const tMethodCallException& e)
  {
    // return local port data
    const tPortData* pd = GetLockedUnsafeRaw();
    pd->GetManager()->GetCurrentRefCounter()->AddLocks(static_cast<int8>((add_locks - 1)));  // we already have one lock
    pc->Recycle();
    return pd;
  }
}

tNetPort::tInterfaceNetPortImpl::tInterfaceNetPortImpl(tNetPort* const outer_class_ptr_, tPortCreationInfo pci) :
    tInterfaceNetPort(pci),
    outer_class_ptr(outer_class_ptr_)
{
  //setCallHandler(this);
}

} // namespace finroc
} // namespace core

