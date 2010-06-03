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
#include "core/tCoreRegister.h"
#include "core/port/tPortFlags.h"

namespace finroc
{
namespace core
{
tCCPortBase::tCCPortBase(tPortCreationInfo pci) :
    tAbstractPort(pci),
    edges_src(),
    edges_dest(),
    default_value(CreateDefaultValue(pci.data_type)),
    value(NULL),
    owned_data(NULL),
    standard_assign(!GetFlag(tPortFlags::cNON_STANDARD_ASSIGN) && (!GetFlag(tPortFlags::cHAS_QUEUE))),
    port_index(GetHandle() & tCoreRegister<>::cELEM_INDEX_MASK),
    queue(GetFlag(tPortFlags::cHAS_QUEUE) ? new tCCPortQueue<tCCPortData>(pci.max_queue_size) : NULL),
    port_listener(),
    pull_request_handler(NULL)
{
  assert((pci.data_type->IsCCType()));
  InitLists(&(edges_src), &(edges_dest));
  if (queue != NULL)
  {
    queue->Init();
  }
  PropagateStrategy(NULL, NULL);  // initialize strategy

  // set initial value to default
  tThreadLocalCache* tc = tThreadLocalCache::Get();
  tCCPortDataContainer<>* c = GetUnusedBuffer(tc);
  c->Assign(((tCCPortData*)default_value->GetDataPtr()));
  c->AddLock();
  value = c->GetCurrentRef();
  tc->last_written_to_port[port_index] = c;
}

void tCCPortBase::ApplyDefaultValue()
{
  //publish(ThreadLocalCache.get(), defaultValue.getContainer());
  tThreadLocalCache* tc = tThreadLocalCache::GetFast();
  tCCPortDataContainer<>* c = GetUnusedBuffer(tc);
  c->Assign(((tCCPortData*)default_value->GetDataPtr()));
  Publish(tc, c);
}

tCCPortBase::~tCCPortBase()
{
  util::tLock lock1(this);
  tThreadLocalCache::Get();  // Initialize ThreadLocalCache - if this has not already happened for GarbageCollector
  tThreadLocalCache::DeleteInfoForPort(port_index);
  default_value->Recycle2();
  if (owned_data != NULL)
  {
    {
      util::tLock lock3(GetThreadLocalCacheInfosLock());
      owned_data->PostThreadReleaseLock();
    }
  }
  // do not release lock on current value - this is done in one of the statements above

  if (queue != NULL)
  {
    delete queue;
  }
  ;
}

void tCCPortBase::DequeueAllRaw(tCCQueueFragment<tCCPortData>& fragment)
{
  queue->DequeueAll(fragment);
}

tCCInterThreadContainer<>* tCCPortBase::DequeueSingleUnsafeRaw()
{
  assert((queue != NULL));
  return queue->Dequeue();
}

tCCInterThreadContainer<>* tCCPortBase::GetInInterThreadContainer()
{
  tCCInterThreadContainer<>* ccitc = tThreadLocalCache::Get()->GetUnusedInterThreadBuffer(GetDataType());
  for (; ;)
  {
    tCCPortDataRef* val = value;
    ccitc->Assign(((tCCPortData*)val->GetContainer()->GetDataPtr()));
    if (val == value)    // still valid??
    {
      return ccitc;
    }
  }
}

tCCPortDataContainer<>* tCCPortBase::GetLockedUnsafeInContainer()
{
  tCCPortDataRef* val = value;
  tCCPortDataContainer<>* val_c = val->GetContainer();
  if (val_c->GetOwnerThread() == util::sThreadUtil::GetCurrentThreadId())    // if same thread: simply add read lock
  {
    val_c->AddLock();
    return val_c;
  }

  // not the same thread: create auto-locked new container
  tThreadLocalCache* tc = tThreadLocalCache::Get();
  tCCPortDataContainer<>* ccitc = tc->GetUnusedBuffer(this->data_type);
  ccitc->ref_counter = 1;
  for (; ;)
  {
    ccitc->Assign(((tCCPortData*)val_c->GetDataPtr()));
    if (val == value)    // still valid??
    {
      return ccitc;
    }
    val = value;
    val_c = val->GetContainer();
  }
}

tCCInterThreadContainer<>* tCCPortBase::GetPullInInterthreadContainerRaw(bool intermediate_assign)
{
  tCCPortDataContainer<>* tmp = PullValueRaw(intermediate_assign);
  tCCInterThreadContainer<>* ret = tThreadLocalCache::GetFast()->GetUnusedInterThreadBuffer(this->data_type);
  ret->Assign(((tCCPortData*)tmp->GetDataPtr()));
  tmp->ReleaseLock();
  return ret;
}

void tCCPortBase::GetRaw(tCCInterThreadContainer<>* buffer)
{
  for (; ;)
  {
    tCCPortDataRef* val = value;
    buffer->Assign(((tCCPortData*)val->GetContainer()->GetDataPtr()));
    if (val == value)    // still valid??
    {
      return;
    }
  }
}

void tCCPortBase::GetRaw(tCCPortDataContainer<>* buffer)
{
  for (; ;)
  {
    tCCPortDataRef* val = value;
    buffer->Assign(((tCCPortData*)val->GetContainer()->GetDataPtr()));
    if (val == value)    // still valid??
    {
      return;
    }
  }
}

void tCCPortBase::GetRaw(tCCPortData* buffer)
{
  for (; ;)
  {
    tCCPortDataRef* val = value;
    val->GetContainer()->AssignTo(buffer);
    if (val == value)    // still valid??
    {
      return;
    }
  }
}

void tCCPortBase::InitialPushTo(tAbstractPort* target, bool reverse)
{
  tThreadLocalCache* tc = tThreadLocalCache::GetFast();
  tc->data = GetLockedUnsafeInContainer();
  tc->ref = tc->data->GetCurrentRef();
  tCCPortBase* t = static_cast<tCCPortBase*>(target);

  if (reverse)
  {
    t->Receive<true, cCHANGED_INITIAL>(tc, this, true, cCHANGED_INITIAL);
  }
  else
  {
    t->Receive<false, cCHANGED_INITIAL>(tc, this, false, cCHANGED_INITIAL);
  }

  tc->data->ReleaseLock();
}

void tCCPortBase::NonStandardAssign(tThreadLocalCache* tc)
{
  if (GetFlag(tPortFlags::cUSES_QUEUE))
  {
    assert((GetFlag(tPortFlags::cHAS_QUEUE)));

    // enqueue
    tCCInterThreadContainer<tCCPortData>* itc = reinterpret_cast<tCCInterThreadContainer<tCCPortData>*>(tc->GetUnusedInterThreadBuffer(tc->data->GetType()));
    itc->Assign(((tCCPortData*)tc->data->GetDataPtr()));
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

tCCPortDataContainer<>* tCCPortBase::PullValueRaw(bool intermediate_assign)
{
  tThreadLocalCache* tc = tThreadLocalCache::GetFast();

  // pull value
  PullValueRawImpl(tc, intermediate_assign, true);

  // return locked data
  return tc->data;

  //      ThreadLocalCache tc = ThreadLocalCache.getFast();
  //      PullCall pc = tc.getUnusedPullCall();
  //      pc.ccPull = true;
  //
  //      //pullValueRaw(pc);
  //      try {
  //          pc = SynchMethodCallLogic.<PullCall>performSynchCall(pc, this, callIndex, PULL_TIMEOUT);
  //          if (pc.tc != null && pc.tc.threadId != ThreadUtil.getCurrentThreadId()) { // reset thread local cache - if it was set by another thread
  //              pc.tc = null;
  //          }
  //          if (pc.tc == null) { // init new PortDataContainer in thread local cache?
  //              pc.setupThreadLocalCache();
  //          }
  //          CCPortDataContainer<?> result = pc.tc.data;
  //          result.addLock();
  //          pc.genericRecycle();
  //          return result;
  //      } catch (MethodCallException e) {
  //          pc.genericRecycle();
  //          return getLockedUnsafeInContainer();
  //      }
}

void tCCPortBase::PullValueRawImpl(tThreadLocalCache* tc, bool intermediate_assign, bool first)
{
  util::tArrayWrapper<tCCPortBase*>* sources = edges_dest.GetIterable();
  if ((!first) && pull_request_handler != NULL)    // for network port pulling it's good if pullRequestHandler is not called on first port - and there aren't any scenarios where this would make sense
  {
    tc->data = tc->GetUnusedBuffer(this->data_type);
    pull_request_handler->PullRequest(this, tc->data->GetDataPtr());
    tc->data->SetRefCounter(1);  // one lock for caller
    tc->ref = tc->data->GetCurrentRef();
    Assign(tc);
  }
  else
  {
    // continue with next-best connected source port
    for (size_t i = 0u, n = sources->Size(); i < n; i++)
    {
      tCCPortBase* pb = sources->Get(i);
      if (pb != NULL)
      {
        pb->PullValueRawImpl(tc, intermediate_assign, false);
        if ((first || intermediate_assign) && (!value->GetContainer()->ContentEquals(((tCCPortData*)tc->data->GetDataPtr()))))
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
}

void tCCPortBase::SetMaxQueueLengthImpl(int length)
{
  assert((GetFlag(tPortFlags::cHAS_QUEUE) && queue != NULL));
  assert((!IsOutputPort()));
  assert((length >= 1));
  queue->SetMaxLength(length);
}

void tCCPortBase::SetPullRequestHandler(tCCPullRequestHandler* pull_request_handler_)
{
  if (pull_request_handler_ != NULL)
  {
    this->pull_request_handler = pull_request_handler_;
  }
}

void tCCPortBase::TransferDataOwnership(tCCPortDataContainer<>* port_data_container)
{
  tCCPortDataContainer<>* current = value->GetContainer();
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

