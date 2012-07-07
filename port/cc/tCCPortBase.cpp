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
#include "core/port/cc/tCCPortBase.h"
#include "core/port/cc/tCCQueueFragmentRaw.h"
#include "core/portdatabase/tFinrocTypeInfo.h"
#include "core/tCoreRegister.h"
#include "core/port/tPortFlags.h"
#include "core/port/cc/tCCPullRequestHandlerRaw.h"

namespace finroc
{
namespace core
{
tCCPortBase::tCCPortBase(tPortCreationInfoBase pci) :
  tAbstractPort(pci),
  edges_src(),
  edges_dest(),
  cc_type_index(tFinrocTypeInfo::Get(GetDataType()).GetCCIndex()),
  default_value(CreateDefaultValue(pci.data_type)),
  value(NULL),
  owned_data(NULL),
  standard_assign(!GetFlag(tPortFlags::cNON_STANDARD_ASSIGN) && (!GetFlag(tPortFlags::cHAS_QUEUE))),
  port_index(GetHandle() & tCoreRegister<tAbstractPort*>::GetElementIndexMask()),
  queue(GetFlag(tPortFlags::cHAS_QUEUE) ? new tCCPortQueue(pci.max_queue_size) : NULL),
  port_listener(),
  pull_request_handler(NULL),
  unit(pci.unit)
{
  assert((tFinrocTypeInfo::IsCCType(pci.data_type)));
  InitLists(&(edges_src), &(edges_dest));
  if (queue != NULL)
  {
    queue->Init();
  }
  PropagateStrategy(NULL, NULL);  // initialize strategy

  // set initial value to default
  tThreadLocalCache* tc = tThreadLocalCache::Get();
  tCCPortDataManagerTL* c = GetUnusedBuffer(tc);
  c->GetObject()->DeepCopyFrom(default_value->GetObject(), NULL);
  c->AddLock();
  value = c->GetCurrentRef();
  tc->last_written_to_port[port_index] = c;
}

tCCPortBase::~tCCPortBase()
{
  tLock lock1(*this);
  tThreadLocalCache::Get();  // Initialize ThreadLocalCache - if this has not already happened for GarbageCollector
  tThreadLocalCache::DeleteInfoForPort(port_index);
  default_value->Recycle2();
  if (owned_data != NULL)
  {
    tLock lock3(*tThreadLocalCache::infos_mutex);
    owned_data->PostThreadReleaseLock();
  }
  // do not release lock on current value - this is done in one of the statements above

  if (queue != NULL)
  {
    delete queue;
  }
}

void tCCPortBase::ApplyDefaultValue()
{
  //publish(ThreadLocalCache.get(), defaultValue.getContainer());
  tThreadLocalCache* tc = tThreadLocalCache::GetFast();
  tCCPortDataManagerTL* c = GetUnusedBuffer(tc);
  c->GetObject()->DeepCopyFrom(default_value->GetObject(), NULL);
  Publish(tc, c);
}

void tCCPortBase::BrowserPublishRaw(tCCPortDataManagerTL* buffer)
{
  assert(buffer->GetOwnerThread() == rrlib::thread::tThread::CurrentThreadId());
  tThreadLocalCache* tc = tThreadLocalCache::Get();

  PublishImpl<false, cCHANGED, true>(tc, buffer);
}

bool tCCPortBase::ContainsDefaultValue()
{
  tCCPortDataManager* c = GetInInterThreadContainer();
  bool result = c->GetObject()->GetType() == default_value->GetObject()->GetType() && c->GetObject()->Equals(*default_value->GetObject());
  c->Recycle2();
  return result;
}

void tCCPortBase::DequeueAllRaw(tCCQueueFragmentRaw& fragment)
{
  queue->DequeueAll(fragment);
}

rrlib::rtti::tGenericObject* tCCPortBase::DequeueSingleAutoLockedRaw()
{
  tCCPortDataManager* result = DequeueSingleUnsafeRaw();
  if (result == NULL)
  {
    return NULL;
  }
  tThreadLocalCache::Get()->AddAutoLock(result);
  return result->GetObject();
}

tCCPortDataManager* tCCPortBase::DequeueSingleUnsafeRaw()
{
  assert((queue != NULL));
  return queue->Dequeue();
}

void tCCPortBase::ForwardData(tAbstractPort& other)
{
  assert((tFinrocTypeInfo::IsCCType(other.GetDataType())));
  tCCPortDataManagerTL* c = GetLockedUnsafeInContainer();
  (static_cast<tCCPortBase&>(other)).Publish(c);
  c->ReleaseLock();
}

const rrlib::rtti::tGenericObject* tCCPortBase::GetAutoLockedRaw()
{
  tThreadLocalCache* tc = tThreadLocalCache::Get();

  if (PushStrategy())
  {
    tCCPortDataManagerTL* mgr = GetLockedUnsafeInContainer();
    tc->AddAutoLock(mgr);
    return mgr->GetObject();
  }
  else
  {
    tCCPortDataManager* mgr = GetInInterThreadContainer();
    tc->AddAutoLock(mgr);
    return mgr->GetObject();
  }
}

tCCPortDataManager* tCCPortBase::GetInInterThreadContainer(bool dont_pull)
{
  tCCPortDataManager* ccitc = tThreadLocalCache::Get()->GetUnusedInterThreadBuffer(GetDataType());
  rrlib::time::tTimestamp timestamp;
  GetRaw(*ccitc->GetObject(), timestamp, dont_pull);
  ccitc->SetTimestamp(timestamp);
  return ccitc;
}

tCCPortDataManagerTL* tCCPortBase::GetLockedUnsafeInContainer()
{
  tCCPortDataRef* val = value;
  tCCPortDataManagerTL* val_c = val->GetContainer();
  if (val_c->GetOwnerThread() == rrlib::thread::tThread::CurrentThreadId())    // if same thread: simply add read lock
  {
    val_c->AddLock();
    return val_c;
  }

  // not the same thread: create auto-locked new container
  tThreadLocalCache* tc = tThreadLocalCache::Get();
  tCCPortDataManagerTL* ccitc = tc->GetUnusedBuffer(this->data_type);
  ccitc->ref_counter = 1;
  for (; ;)
  {
    ccitc->GetObject()->DeepCopyFrom(val_c->GetObject(), NULL);
    ccitc->SetTimestamp(val_c->GetTimestamp());
    if (val == value)    // still valid??
    {
      return ccitc;
    }
    val = value;
    val_c = val->GetContainer();
  }
}

tCCPortDataManager* tCCPortBase::GetPullInInterthreadContainerRaw(bool intermediate_assign, bool ignore_pull_request_handler_on_this_port)
{
  tCCPortDataManagerTL* tmp = PullValueRaw(intermediate_assign, ignore_pull_request_handler_on_this_port);
  tCCPortDataManager* ret = tThreadLocalCache::GetFast()->GetUnusedInterThreadBuffer(this->data_type);
  ret->GetObject()->DeepCopyFrom(tmp->GetObject(), NULL);
  ret->SetTimestamp(tmp->GetTimestamp());
  tmp->ReleaseLock();
  return ret;
}

void tCCPortBase::GetRaw(rrlib::rtti::tGenericObject& buffer, rrlib::time::tTimestamp& timestamp, bool dont_pull)
{
  if (PushStrategy() || dont_pull)
  {
    for (; ;)
    {
      tCCPortDataRef* val = value;
      buffer.DeepCopyFrom(val->GetData(), NULL);
      timestamp = val->GetContainer()->GetTimestamp();
      if (val == value)    // still valid??
      {
        return;
      }
    }
  }
  else
  {
    tCCPortDataManagerTL* dc = PullValueRaw();
    buffer.DeepCopyFrom(dc->GetObject(), NULL);
    timestamp = dc->GetTimestamp();
    dc->ReleaseLock();
  }
}

void tCCPortBase::InitialPushTo(tAbstractPort& target, bool reverse)
{
  tThreadLocalCache* tc = tThreadLocalCache::GetFast();
  tc->data = GetLockedUnsafeInContainer();
  tc->ref = tc->data->GetCurrentRef();
  tCCPortBase& t = static_cast<tCCPortBase&>(target);

  if (reverse)
  {
    t.Receive<true, cCHANGED_INITIAL>(tc, *this, true, cCHANGED_INITIAL);
  }
  else
  {
    t.Receive<false, cCHANGED_INITIAL>(tc, *this, false, cCHANGED_INITIAL);
  }

  tc->data->ReleaseLock();
}

void tCCPortBase::NonStandardAssign(tThreadLocalCache* tc)
{
  if (GetFlag(tPortFlags::cUSES_QUEUE))
  {
    assert((GetFlag(tPortFlags::cHAS_QUEUE)));

    // enqueue
    tCCPortDataManager* itc = tc->GetUnusedInterThreadBuffer(tc->data->GetObject()->GetType());
    itc->GetObject()->DeepCopyFrom(tc->data->GetObject(), NULL);
    itc->SetTimestamp(tc->data->GetTimestamp());
    queue->EnqueueWrapped(itc);
  }
}

void tCCPortBase::NotifyDisconnect()
{
  if (GetFlag(tPortFlags::cDEFAULT_ON_DISCONNECT))
  {
    ApplyDefaultValue();
  }
}

tCCPortDataManagerTL* tCCPortBase::PullValueRaw(bool intermediate_assign, bool ignore_pull_request_handler_on_this_port)
{
  tThreadLocalCache* tc = tThreadLocalCache::GetFast();

  // pull value
  PullValueRawImpl(tc, intermediate_assign, ignore_pull_request_handler_on_this_port);

  // return locked data
  return tc->data;
}

void tCCPortBase::PullValueRawImpl(tThreadLocalCache* tc, bool intermediate_assign, bool first)
{
  util::tArrayWrapper<tCCPortBase*>* sources = edges_dest.GetIterable();
  if ((!first) && pull_request_handler != NULL)    // for network port pulling it's good if pullRequestHandler is not called on first port - and there aren't any scenarios where this would make sense
  {
    tCCPortDataManagerTL* res_buf = tc->GetUnusedBuffer(this->data_type);
    if (pull_request_handler->PullRequest(*this, *res_buf, intermediate_assign))
    {
      tc->data = res_buf;
      tc->data->SetRefCounter(1);  // one lock for caller
      tc->ref = tc->data->GetCurrentRef();
      Assign(tc);
      return;
    }
    else
    {
      res_buf->RecycleUnused();
    }
  }

  // continue with next-best connected source port
  for (size_t i = 0u, n = sources->Size(); i < n; i++)
  {
    tCCPortBase* pb = sources->Get(i);
    if (pb != NULL)
    {
      pb->PullValueRawImpl(tc, intermediate_assign, false);
      if ((first || intermediate_assign) && (!value->GetContainer()->ContentEquals(tc->data->GetObject()->GetRawDataPtr())))
      {
        Assign(tc);
      }
      return;
    }
  }

  // no connected source port... pull/return current value
  tc->data = GetLockedUnsafeInContainer();  // one lock for caller
  tc->ref = tc->data->GetCurrentRef();
}

void tCCPortBase::SetMaxQueueLengthImpl(int length)
{
  assert((GetFlag(tPortFlags::cHAS_QUEUE) && queue != NULL));
  assert((!IsOutputPort()));
  assert((length >= 1));
  queue->SetMaxLength(length);
}

void tCCPortBase::SetPullRequestHandler(tCCPullRequestHandlerRaw* pull_request_handler_)
{
  if (pull_request_handler_ != NULL)
  {
    this->pull_request_handler = pull_request_handler_;
  }
}

void tCCPortBase::TransferDataOwnership(tCCPortDataManagerTL* port_data_container)
{
  tCCPortDataManagerTL* current = value->GetContainer();
  if (current == port_data_container)    // ownedData is outdated and can be deleted
  {
    if (owned_data != NULL)
    {
      owned_data->PostThreadReleaseLock();
    }
    owned_data = port_data_container;
  }
  else if (current == owned_data)    // ownedData is outdated and can be deleted
  {
    port_data_container->PostThreadReleaseLock();
  }
  else    // both are outdated and can be deleted
  {
    if (owned_data != NULL)
    {
      owned_data->PostThreadReleaseLock();
      owned_data = NULL;
    }
    port_data_container->PostThreadReleaseLock();
  }
}

} // namespace finroc
} // namespace core

