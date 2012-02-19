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
#include "core/port/net/tNetPort.h"
#include "rrlib/rtti/rtti.h"
#include "core/port/std/tPortDataManager.h"
#include "core/port/cc/tCCPortDataManagerTL.h"
#include "core/port/std/tPortQueueFragmentRaw.h"
#include "core/port/tThreadLocalCache.h"
#include "core/port/std/tPortDataReference.h"
#include "core/port/cc/tCCPortDataManager.h"
#include "core/port/cc/tCCQueueFragmentRaw.h"
#include "core/port/rpc/tMethodCallException.h"
#include "core/port/rpc/tThreadLocalRPCData.h"

namespace finroc
{
namespace core
{
const int tNetPort::cPULL_TIMEOUT;

tNetPort::tNetPort(tPortCreationInfoBase pci, util::tObject* belongs_to_) :
  wrapped(NULL),
  belongs_to(belongs_to_),
  remote_handle(0),
  ftype(tFinrocTypeInfo::Get(pci.data_type).GetType()),
  last_update(util::tLong::cMIN_VALUE)
{
  // keep most these flags
  uint f = pci.flags & (tPortFlags::cACCEPTS_DATA | tPortFlags::cEMITS_DATA | tPortFlags::cMAY_ACCEPT_REVERSE_DATA | tPortFlags::cIS_OUTPUT_PORT | tPortFlags::cIS_BULK_PORT | tPortFlags::cIS_EXPRESS_PORT | tPortFlags::cNON_STANDARD_ASSIGN | tCoreFlags::cALTERNATE_LINK_ROOT | tCoreFlags::cGLOBALLY_UNIQUE_LINK | tCoreFlags::cFINSTRUCTED);

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
  if (IsStdType() || IsMethodType())
  {
    f |= tPortFlags::cSPECIAL_REUSE_QUEUE;  // different data types may be incoming - cc types are thread local
  }
  pci.flags = f;

  wrapped = (IsMethodType() ? static_cast<tAbstractPort*>(new tInterfaceNetPortImpl(this, pci)) : (IsCCType() ? static_cast<tAbstractPort*>(new tCCNetPort(this, pci)) : static_cast<tAbstractPort*>(new tStdNetPort(this, pci))));
  tTypedObject::type = rrlib::rtti::tDataType<tNetPort>();
  wrapped->AddAnnotation(this);
}

rrlib::rtti::tGenericObject* tNetPort::CreateGenericObject(const rrlib::rtti::tDataTypeBase& dt, void* factory_parameter)
{
  if (tFinrocTypeInfo::IsStdType(dt))
  {
    return GetPort()->GetUnusedBufferRaw(dt)->GetObject();
  }
  else if (tFinrocTypeInfo::IsCCType(dt))
  {
    if (factory_parameter == NULL)
    {
      // get thread local buffer
      return tThreadLocalCache::Get()->GetUnusedBuffer(dt)->GetObject();
    }
    else
    {
      // get interthread buffer
      return tThreadLocalCache::Get()->GetUnusedInterThreadBuffer(dt)->GetObject();
    }
  }
  FINROC_LOG_PRINT(rrlib::logging::eLL_ERROR, "Cannot create buffer of type ", dt.GetName());
  return NULL;
}

tNetPort* tNetPort::FindNetPort(tAbstractPort& port, util::tObject* belongs_to)
{
  if (!belongs_to)
  {
    return NULL;
  }
  util::tSimpleList<tAbstractPort*> result;
  port.GetConnectionPartners(result, port.IsOutputPort(), !port.IsOutputPort());
  for (int i = 0, n = result.Size(); i < n; i++)
  {
    tAbstractPort* port2 = result.Get(i);
    if (port2 && port2->GetFlag(tCoreFlags::cNETWORK_ELEMENT))
    {
      tNetPort* np = port2->AsNetPort();
      if (np && np->GetBelongsTo() == belongs_to)
      {
        return np;
      }
    }
  }
  return NULL;
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

void tNetPort::ReceiveDataFromStream(rrlib::serialization::tInputStream& ci, int64 timestamp, int8 changed_flag)
{
  assert((GetPort()->IsReady()));
  if (IsStdType() || IsTransactionType())
  {
    tStdNetPort* pb = static_cast<tStdNetPort*>(wrapped);
    ci.SetFactory(this);
    do
    {
      pb->PublishFromNet(static_cast<tPortDataManager*>(rrlib::rtti::ReadObject(ci, wrapped->GetDataType(), NULL)->GetManager()), changed_flag);
    }
    while (ci.ReadBoolean());
    ci.SetFactory(NULL);
  }
  else if (IsCCType())
  {
    tCCNetPort* pb = static_cast<tCCNetPort*>(wrapped);
    do
    {
      pb->PublishFromNet(static_cast<tCCPortDataManagerTL*>(rrlib::rtti::ReadObject(ci, wrapped->GetDataType(), NULL)->GetManager()), changed_flag);
    }
    while (ci.ReadBoolean());
  }
  else    // interface port
  {
    throw util::tRuntimeException("Method calls are not handled using this mechanism", CODE_LOCATION_MACRO);
  }
}

void tNetPort::UpdateFlags(uint flags)
{
  // process flags... keep DELETE and READY flags
  uint keep_flags = tCoreFlags::cSTATUS_FLAGS;
  uint cur_flags = GetPort()->GetAllFlags() & keep_flags;
  flags &= ~(keep_flags);
  flags |= cur_flags;

  if (IsStdType() || IsTransactionType())
  {
    (static_cast<tStdNetPort*>(wrapped))->UpdateFlags(flags);
  }
  else if (IsCCType())
  {
    (static_cast<tCCNetPort*>(wrapped))->UpdateFlags(flags);
  }
  else
  {
    (static_cast<tInterfaceNetPortImpl*>(wrapped))->UpdateFlags(flags);
  }
}

void tNetPort::WriteDataToNetwork(rrlib::serialization::tOutputStream& co, int64 start_time)
{
  bool use_q = wrapped->GetFlag(tPortFlags::cUSES_QUEUE);
  bool first = true;
  if (IsStdType() || IsTransactionType())
  {
    tStdNetPort* pb = static_cast<tStdNetPort*>(wrapped);
    if (!use_q)
    {
      tPortDataManager* pd = pb->GetLockedUnsafeRaw(true);
      rrlib::rtti::WriteObject(co, pd->GetObject());
      pd->ReleaseLock();
    }
    else
    {
      tPortQueueFragmentRaw fragment;
      pb->DequeueAllRaw(fragment);
      const tPortDataReference* pd = NULL;
      while ((pd = static_cast<tPortDataReference*>(fragment.Dequeue())) != NULL)
      {
        if (!first)
        {
          co.WriteBoolean(true);
        }
        first = false;
        rrlib::rtti::WriteObject(co, pd->GetManager()->GetObject());
        pd->GetManager()->ReleaseLock();
      }
    }
  }
  else if (IsCCType())
  {
    tCCNetPort* pb = static_cast<tCCNetPort*>(wrapped);
    if (!use_q)
    {
      tCCPortDataManager* ccitc = pb->GetInInterThreadContainer(true);
      rrlib::rtti::WriteObject(co, ccitc->GetObject());
      ccitc->Recycle2();
    }
    else
    {
      tCCQueueFragmentRaw fragment;
      pb->DequeueAllRaw(fragment);
      tCCPortDataManager* pd = NULL;
      while ((pd = fragment.DequeueUnsafe()) != NULL)
      {
        if (!first)
        {
          co.WriteBoolean(true);
        }
        first = false;
        rrlib::rtti::WriteObject(co, pd->GetObject());
        pd->Recycle2();
      }
    }
  }
  else    // interface port
  {
    throw util::tRuntimeException("Method calls are not handled using this mechanism", CODE_LOCATION_MACRO);
  }
  co.WriteBoolean(false);
}

tNetPort::tCCNetPort::tCCNetPort(tNetPort* const outer_class_ptr_, tPortCreationInfoBase pci) :
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

void tNetPort::tCCNetPort::PublishFromNet(tCCPortDataManagerTL* read_object, int8 changed_flag)
{
  // Publish all pushed data from the network. This avoids problems in finstruct.
  // Compared to tranferring the stuff over the network,
  // effort for publishing the stuff locally, is negligible.
  /*if (changedFlag != AbstractPort.CHANGED_INITIAL) {
    check ...
  }*/

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

bool tNetPort::tCCNetPort::PullRequest(tCCPortBase* origin, tCCPortDataManagerTL* result_buffer)
{
  tPullCall::tPtr pc(tThreadLocalRPCData::Get().GetUnusedPullCall());
  pc->SetRemotePortHandle(outer_class_ptr->remote_handle);
  //          pc.setLocalPortHandle(getHandle());
  try
  {
    tSynchMethodCallLogic::PerformSynchCall(pc, *this, cPULL_TIMEOUT);
    assert(!pc->HasException());
    tPortDataPtr<rrlib::rtti::tGenericObject> o = pc->GetParamGeneric(0);
    tCCPortDataManager* c = static_cast<tCCPortDataManager*>(o->GetManager());
    result_buffer->GetObject()->DeepCopyFrom(c->GetObject(), NULL);
  }
  catch (const tMethodCallException& e)
  {
    GetRaw(result_buffer->GetObject(), true);
  }
  return true;
}

tNetPort::tStdNetPort::tStdNetPort(tNetPort* const outer_class_ptr_, tPortCreationInfoBase pci) :
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

void tNetPort::tStdNetPort::PublishFromNet(tPortDataManager* read_object, int8 changed_flag)
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

const tPortDataManager* tNetPort::tStdNetPort::PullRequest(tPortBase* origin, int8 add_locks)
{
  assert((add_locks > 0));
  tPullCall::tPtr pc = tThreadLocalRPCData::Get().GetUnusedPullCall();
  pc->SetRemotePortHandle(outer_class_ptr->remote_handle);
  //          pc.setLocalPortHandle(getHandle());
  try
  {
    tSynchMethodCallLogic::PerformSynchCall(pc, *this, cPULL_TIMEOUT);
    assert(!pc->HasException());
    tPortDataPtr<rrlib::rtti::tGenericObject> o = pc->GetParamGeneric(0);
    tPortDataManager* pd = static_cast<tPortDataManager*>(o->GetManager());
    int locks = 0;  // Java: we already have one lock
    pd->GetCurrentRefCounter()->AddLocks(static_cast<int8>((add_locks - locks)));
    return pd;
  }
  catch (const tMethodCallException& e)
  {
    // return local port data
    tPortDataManager* pd = LockCurrentValueForRead();
    pd->GetCurrentRefCounter()->AddLocks(static_cast<int8>((add_locks - 1)));  // we already have one lock
    return pd;
  }
}

tNetPort::tInterfaceNetPortImpl::tInterfaceNetPortImpl(tNetPort* const outer_class_ptr_, tPortCreationInfoBase pci) :
  tInterfaceNetPort(pci),
  outer_class_ptr(outer_class_ptr_)
{
  //setCallHandler(this);
}

tMethodCall::tPtr tNetPort::tInterfaceNetPortImpl::SynchCallOverTheNet(tMethodCall::tPtr& mc, int timeout)
{
  assert((mc->GetMethod() != NULL));
  tSynchMethodCallLogic::PerformSynchCall(mc, *this, timeout);
  return std::move(mc);
}


} // namespace finroc
} // namespace core

