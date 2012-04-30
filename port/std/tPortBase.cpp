/**
 * You received this file as part of an advanced experimental
 * robotics framework prototype ('finroc')
 *
 * Copyright (C) 2007-2011 Max Reichardt,
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
#include "core/port/std/tPortQueueFragmentRaw.h"

#include "core/port/std/tPortBase.h"
#include "core/port/cc/tCCPortBase.h"
#include "core/portdatabase/tFinrocTypeInfo.h"
#include "core/portdatabase/tPortFactory.h"
#include "core/port/std/tPullRequestHandlerRaw.h"

namespace finroc
{
namespace core
{

namespace internal
{
class tDataPortFactory : public tPortFactory
{
public:
  tDataPortFactory()
  {
    tFinrocTypeInfo::SetDefaultPortFactory(*this);
  }

  virtual tAbstractPort& CreatePort(const std::string& port_name, tFrameworkElement& parent, const rrlib::rtti::tDataTypeBase& dt, uint flags)
  {
    tAbstractPort* ap = NULL;
    assert(tFinrocTypeInfo::IsStdType(dt) || tFinrocTypeInfo::IsCCType(dt));
    if (tFinrocTypeInfo::IsStdType(dt))
    {
      ap = new tPortBase(tPortCreationInfoBase(port_name, &parent, dt, flags));
    }
    else
    {
      ap = new tCCPortBase(tPortCreationInfoBase(port_name, &parent, dt, flags));
    }
    return *ap;
  }
};

tDataPortFactory default_data_port_factory;

}


tPortBase::tPortBase(tPortCreationInfoBase pci) :
  tAbstractPort(ProcessPci(pci)),
  edges_src(),
  edges_dest(),
  default_value(CreateDefaultValue(pci.data_type)),
  value(),
  cur_data_type(this->data_type),
  buffer_pool(HasSpecialReuseQueue() ? NULL : new tPortDataBufferPool(this->data_type, IsOutputPort() ? 2 : 0)),
  multi_buffer_pool(HasSpecialReuseQueue() ? new tMultiTypePortDataBufferPool() : NULL),
  standard_assign(!GetFlag(tPortFlags::cNON_STANDARD_ASSIGN) && (!GetFlag(tPortFlags::cHAS_QUEUE))),
  queue(GetFlag(tPortFlags::cHAS_QUEUE) ? new tPortQueue(pci.max_queue_size) : NULL),
  pull_request_handler(NULL),
  port_listener()
{
  assert((tFinrocTypeInfo::IsStdType(pci.data_type)));
  InitLists(&(edges_src), &(edges_dest));
  value.Set(default_value->GetCurReference());
  if (queue)
  {
    queue->Init();
  }

  PropagateStrategy(NULL, NULL);  // initialize strategy
}

void tPortBase::AddLock(tPublishCache& pc)
{
  pc.set_locks++;
  if (pc.set_locks >= pc.lock_estimate)    // make lockEstimate bigger than setLocks to make notifyListeners() safe
  {
    pc.lock_estimate++;
    pc.cur_ref_counter->AddLock();
  }
}

void tPortBase::BrowserPublish(const tPortDataManager* data)
{
  PublishImpl<false, cCHANGED, true>(data);
}

tPortDataManager* tPortBase::CreateDefaultValue(const rrlib::rtti::tDataTypeBase& dt)
{
  tPortDataManager* pdm = tPortDataManager::Create(dt);  //new PortDataManager(dt, null);
  pdm->GetCurrentRefCounter()->SetLocks(static_cast<int8>(2));
  return pdm;
}

tPortBase::~tPortBase()
{
  util::tLock lock1(this);
  default_value->GetCurrentRefCounter()->ReleaseLock();  // thread safe, since called deferred - when no one else should access this port anymore
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
  ;
}

void tPortBase::DequeueAllRaw(tPortQueueFragmentRaw& fragment)
{
  queue->DequeueAll(fragment);
}

tPortDataManager* tPortBase::DequeueSingleAutoLockedRaw()
{
  tPortDataManager* result = DequeueSingleUnsafeRaw();
  if (result != NULL)
  {
    tThreadLocalCache::Get()->AddAutoLock(result);
  }
  return result;
}

tPortDataManager* tPortBase::DequeueSingleUnsafeRaw()
{
  assert((queue != NULL));
  tPortDataReference* pd = queue->Dequeue();
  return pd != NULL ? pd->GetManager() : NULL;
}

void tPortBase::ForwardData(tAbstractPort* other)
{
  assert((tFinrocTypeInfo::IsStdType(other->GetDataType())));
  (static_cast<tPortBase*>(other))->Publish(GetAutoLockedRaw());
  ReleaseAutoLocks();
}

void tPortBase::InitialPushTo(tAbstractPort* target, bool reverse)
{
  tPortDataManager* pd = GetLockedUnsafeRaw();

  assert((pd->GetType() != NULL) && "Port data type not initialized");
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

void tPortBase::PrintStructure(int indent, std::stringstream& output)
{
  ::finroc::core::tFrameworkElement::PrintStructure(indent, output);
  if (buffer_pool != NULL)
  {
    buffer_pool->PrintStructure(indent + 2, output);
  }
  else if (multi_buffer_pool != NULL)
  {
    multi_buffer_pool->PrintStructure(indent + 2, output);
  }
}

tPortCreationInfoBase& tPortBase::ProcessPci(tPortCreationInfoBase& pci)
{
  if ((pci.flags & tPortFlags::cIS_OUTPUT_PORT) == 0)    // no output port
  {
    pci.flags |= tPortFlags::cSPECIAL_REUSE_QUEUE;
  }
  return pci;
}

tPortDataManager* tPortBase::PullValueRaw(bool intermediate_assign, bool ignore_pull_request_handler_on_this_port)
{
  // prepare publish cache
  tPublishCache pc;

  pc.lock_estimate = 1;  // 1 for return (the one for this port is added in pullValueRawImpl)
  pc.set_locks = 0;

  // pull value
  PullValueRawImpl(pc, intermediate_assign, ignore_pull_request_handler_on_this_port);

  // lock value and return
  pc.ReleaseObsoleteLocks();
  return pc.cur_ref->GetManager();
}

const void tPortBase::PullValueRawImpl(tPublishCache& pc, bool intermediate_assign, bool first)
{
  util::tArrayWrapper<tPortBase*>* sources = edges_dest.GetIterable();
  if ((!first) && pull_request_handler != NULL)
  {
    pc.lock_estimate++;  // for local assign
    const tPortDataManager* mgr = pull_request_handler->PullRequest(this, static_cast<int8>(pc.lock_estimate), intermediate_assign);
    if (mgr != NULL)
    {
      tPortDataReference* pdr = mgr->GetCurReference();
      pc.cur_ref = pdr;
      pc.cur_ref_counter = pdr->GetRefCounter();
      assert((pdr->GetRefCounter()->GetLocks() >= pc.lock_estimate));
      if (pc.cur_ref != value.Get())
      {
        Assign(pc);
      }
      pc.set_locks++;  // lock for return
      return;
    }
    else
    {
      pc.lock_estimate--; // ok... pull request was not handled, revert and continue normally
    }
  }

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

void tPortBase::SetMaxQueueLengthImpl(int length)
{
  assert((GetFlag(tPortFlags::cHAS_QUEUE) && queue != NULL));
  assert((!IsOutputPort()));
  assert((length >= 1));
  queue->SetMaxLength(length);
}

void tPortBase::SetPullRequestHandler(tPullRequestHandlerRaw* pull_request_handler_)
{
  if (pull_request_handler_ != NULL)
  {
    this->pull_request_handler = pull_request_handler_;
  }
}

} // namespace finroc
} // namespace core

