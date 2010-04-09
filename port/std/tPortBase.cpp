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
#include "core/port/std/tPortBase.h"
#include "core/port/std/tPortDataManager.h"

namespace finroc
{
namespace core
{
tPortBase::tPortBase(tPortCreationInfo pci) :
    tAbstractPort(ProcessPci(pci)),
    edges_src(),
    edges_dest(),
    default_value(CreateDefaultValue(pci.data_type)),
    value(),
    cur_data_type(this->data_type),
    buffer_pool(HasSpecialReuseQueue() ? NULL : new tPortDataBufferPool(this->data_type, IsOutputPort() ? 2 : 0)),
    multi_buffer_pool(HasSpecialReuseQueue() ? new tMultiTypePortDataBufferPool() : NULL),
    standard_assign(!GetFlag(tPortFlags::cNON_STANDARD_ASSIGN) && (!GetFlag(tPortFlags::cHAS_QUEUE))),
    queue(GetFlag(tPortFlags::cHAS_QUEUE) ? new tPortQueue<tPortData>(pci.max_queue_size) : NULL),
    pull_request_handler(NULL),
    port_listener()
{
  assert((pci.data_type->IsStdType()));
  InitLists(&(edges_src), &(edges_dest));
  //pDefaultValue = pdm.getData();
  //PortDataCreationInfo.get().reset();
  //pdm.setLocks(2); // one... so it will stay read locked... and another one for pValue
  value.Set(default_value->GetCurReference());
  if (queue != NULL)
  {
    queue->Init();
  }

  PropagateStrategy(NULL, NULL);  // initialize strategy
}

void tPortBase::AddLock(tPublishCache& pc)
{
  pc.set_locks++;
  if (pc.set_locks > pc.lock_estimate)
  {
    pc.lock_estimate = pc.set_locks;
    pc.cur_ref_counter->AddLock();
  }
}

tPortData* tPortBase::CreateDefaultValue(tDataType* dt)
{
  tPortDataManager* pdm = new tPortDataManager(dt, NULL);
  pdm->GetCurrentRefCounter()->SetLocks(static_cast<int8>(2));
  return pdm->GetData();
}

tPortBase::~tPortBase()
{
  util::tLock lock1(obj_synch);
  default_value->GetManager()->GetCurrentRefCounter()->ReleaseLock();  // thread safe, since called deferred - when no one else should access this port anymore
  value.Get()->GetRefCounter()->ReleaseLock();  // thread safe, since nobody should publish to port anymore

  if (buffer_pool != NULL)
  {
    buffer_pool->ControlledDelete();
  }
  else
  {
    delete multi_buffer_pool;
  }

  if (queue != NULL)
  {
    delete queue;
  }
}

void tPortBase::DequeueAllRaw(tPortQueueFragment<tPortData>& fragment)
{
  queue->DequeueAll(fragment);
}

tPortData* tPortBase::DequeueSingleAutoLockedRaw()
{
  tPortData* result = DequeueSingleUnsafeRaw();
  if (result != NULL)
  {
    tThreadLocalCache::Get()->AddAutoLock(result);
  }
  return result;
}

tPortData* tPortBase::DequeueSingleUnsafeRaw()
{
  assert((queue != NULL));
  tPortDataReference* pd = queue->Dequeue();
  return pd != NULL ? pd->GetData() : NULL;
}

tPortData* tPortBase::GetUnusedBufferRaw()
{
  return buffer_pool == NULL ? multi_buffer_pool->GetUnusedBuffer(cur_data_type) : buffer_pool->GetUnusedBuffer();

  //    @Ptr ThreadLocalCache tli = ThreadLocalCache.get();
  //    @Ptr PortDataBufferPool pdbp = tli.getBufferPool(handle);
  //    boolean hasSQ = hasSpecialReuseQueue();
  //    if ((!hasSQ) && pdbp != null) {  // common case
  //      return pdbp.getUnusedBuffer();
  //    }
  //
  //    return getUnusedBuffer2(pdbp, tli, hasSQ);
}

void tPortBase::InitialPushTo(tAbstractPort* target, bool reverse)
{
  const tPortData* pd = GetLockedUnsafeRaw();

  assert((pd->GetType() != NULL) && "Port data type not initialized");
  assert((pd->GetManager() != NULL) && "Only port data obtained from a port can be sent");
  assert(IsInitialized());
  assert(target != NULL);

  tPublishCache pc;

  pc.lock_estimate = 1;
  pc.set_locks = 0;  // this port
  pc.cur_ref = pd->GetCurReference();
  pc.cur_ref_counter = pc.cur_ref->GetRefCounter();
  //pc.curRefCounter.setOrAddLocks((byte)pc.lockEstimate); - we already have this one lock
  assert((pc.cur_ref->IsLocked()));

  tPortBase* t = static_cast<tPortBase*>(target);

  if (reverse)
  {
    t->Receive<true, cCHANGED_INITIAL>(pc, this, true, cCHANGED_INITIAL);
  }
  else
  {
    t->Receive<false, cCHANGED_INITIAL>(pc, this, false, cCHANGED_INITIAL);
  }

  // release any locks that were acquired too much
  pc.ReleaseObsoleteLocks();
}

void tPortBase::NonStandardAssign(tPublishCache& pc)
{
  if (GetFlag(tPortFlags::cUSES_QUEUE))
  {
    assert((GetFlag(tPortFlags::cHAS_QUEUE)));

    // enqueue
    AddLock(pc);
    queue->EnqueueWrapped(pc.cur_ref);
  }
}

void tPortBase::NotifyDisconnect()
{
  if (GetFlag(tPortFlags::cDEFAULT_ON_DISCONNECT))
  {
    ApplyDefaultValue();
  }
}

void tPortBase::PrintStructure(int indent)
{
  ::finroc::core::tFrameworkElement::PrintStructure(indent);
  if (buffer_pool != NULL)
  {
    buffer_pool->PrintStructure(indent + 2);
  }
  else if (multi_buffer_pool != NULL)
  {
    multi_buffer_pool->PrintStructure(indent + 2);
  }
}

tPortCreationInfo& tPortBase::ProcessPci(tPortCreationInfo& pci)
{
  if ((pci.flags & tPortFlags::cIS_OUTPUT_PORT) == 0)    // no output port
  {
    pci.flags |= tPortFlags::cSPECIAL_REUSE_QUEUE;
  }
  return pci;
}

const tPortData* tPortBase::PullValueRaw(bool intermediate_assign)
{
  // prepare publish cache
  tPublishCache pc;

  pc.lock_estimate = 1;  // 1 for return (the one for this port is added in pullValueRawImpl)
  pc.set_locks = 0;

  // pull value
  PullValueRawImpl(pc, intermediate_assign, true);

  // lock value and return
  //pc.curRef.getManager().addLock(); we already have extra lock from pullValueRawImpl
  //pc.setLocks++; we already have extra lock from pullValueRawImpl
  pc.ReleaseObsoleteLocks();
  return pc.cur_ref->GetData();
}

const void tPortBase::PullValueRawImpl(tPublishCache& pc, bool intermediate_assign, bool first)
{
  util::tArrayWrapper<tPortBase*>* sources = edges_dest.GetIterable();
  if ((!first) && pull_request_handler != NULL)    // for network port pulling it's good if pullRequestHandler is not called on first port - and there aren't any scenarios where this would make sense
  {
    pc.lock_estimate++;  // for local assign
    tPortDataReference* pdr = pull_request_handler->PullRequest(this, static_cast<int8>(pc.lock_estimate))->GetCurReference();
    pc.cur_ref = pdr;
    pc.cur_ref_counter = pdr->GetRefCounter();
    assert((pdr->GetRefCounter()->GetLocks() >= pc.lock_estimate));
    if (pc.cur_ref != value.Get())
    {
      Assign(pc);
    }
    pc.set_locks++;  // lock for return
  }
  else
  {
    // continue with next-best connected source port
    for (size_t i = 0u, n = sources->Size(); i < n; i++)
    {
      tPortBase* pb = sources->Get(i);
      if (pb != NULL)
      {
        if (intermediate_assign)
        {
          pc.lock_estimate++;
        }
        pb->PullValueRawImpl(pc, intermediate_assign, false);
        if ((first || intermediate_assign) && (pc.cur_ref != value.Get()))
        {
          Assign(pc);
        }
        return;
      }
    }

    // no connected source port... pull/return current value
    pc.cur_ref = LockCurrentValueForRead(static_cast<int8>(pc.lock_estimate));
    pc.cur_ref_counter = pc.cur_ref->GetRefCounter();
    pc.set_locks++;  // lock for return
  }

  //    PullCall pc = ThreadLocalCache.getFast().getUnusedPullCall();
  //
  //    pc.ccPull = false;
  //    pc.info.lockEstimate = intermediateAssign ? 2 : 1; // 3: 1 for call, 1 for this port, 1 for return
  //    pc.info.setLocks = 0;
  //    pc.intermediateAssign = intermediateAssign;
  //    //pullValueRaw(pc);
  //    try {
  //      addLock(pc.info); // lock for the pull call
  //      pc = SynchMethodCallLogic.<PullCall>performSynchCall(pc, this, callIndex, PULL_TIMEOUT);
  //      addLock(pc.info); // lock for the outside
  //
  //      // assign if this wasn't done yet
  //      if (!intermediateAssign) {
  //        assign(pc.info);
  //      }
  //
  //      assert(pc.info.curRef.isLocked());
  //      PortData result = pc.info.curRef.getData();
  //      pc.genericRecycle();
  //      assert(result.getCurReference().isLocked());
  //
  //      return result;
  //    } catch (MethodCallException e) {
  //
  //      // possibly timeout
  //      pc.genericRecycle();
  //      return lockCurrentValueForRead();
  //    }
}

void tPortBase::SetMaxQueueLengthImpl(int length)
{
  assert((GetFlag(tPortFlags::cHAS_QUEUE) && queue != NULL));
  assert((!IsOutputPort()));
  assert((length >= 1));
  queue->SetMaxLength(length);
}

void tPortBase::SetPullRequestHandler(tPullRequestHandler* pull_request_handler_)
{
  if (pull_request_handler_ != NULL)
  {
    this->pull_request_handler = pull_request_handler_;
  }
}

} // namespace finroc
} // namespace core

