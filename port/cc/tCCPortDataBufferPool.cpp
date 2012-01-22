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
#include "core/port/tThreadLocalCache.h"

#include "core/port/cc/tCCPortDataBufferPool.h"
#include "core/port/cc/tCCPortDataManagerTL.h"
#include "core/port/cc/tCCPortQueueElement.h"
#include "core/port/tThreadLocalCache.h"
#include "rrlib/finroc_core_utils/container/tAbstractReusable.h"

namespace finroc
{
namespace core
{
tCCPortDataBufferPool::tCCPortDataBufferPool(const rrlib::serialization::tDataTypeBase& data_type_, int initial_size) :
  thread_local_cache_infos(tThreadLocalCache::Get()->GetInfosLock()),
  returned_buffers(),
  inter_threads(new util::tReusablesPool<tCCPortDataManager>()),
  data_type(data_type_)
{
  for (int i = 0; i < initial_size; i++)
  {
    //enqueue(createBuffer());
    Attach(tCCPortDataManagerTL::Create(data_type_), true);
  }
  assert(thread_local_cache_infos.get() != NULL);
}

void tCCPortDataBufferPool::ControlledDelete()
{
  inter_threads->ControlledDelete();
  ::finroc::util::tReusablesPoolTL<tCCPortDataManagerTL>::ControlledDelete();
}

tCCPortDataManagerTL* tCCPortDataBufferPool::CreateBuffer()
{
  // try to reclaim any returned buffers, before new one is created - not so deterministic, but usually doesn't occur and memory allocation is anyway
  tCCPortDataManagerTL* pc;
  bool found = false;
  while (true)
  {
    tCCPortQueueElement* pqe = returned_buffers.Dequeue();
    if (pqe == NULL)
    {
      break;
    }
    pc = static_cast<tCCPortDataManagerTL*>(pqe->GetElement());
    pqe->Recycle(false);
    found = true;
    pc->ReleaseLock();
  }
  if (found)
  {
    pc = GetUnused();
    if (pc != NULL)
    {
      return pc;
    }
  }

  // okay... create new buffer
  tCCPortDataManagerTL* pdm = tCCPortDataManagerTL::Create(data_type);
  Attach(pdm, false);
  return pdm;
}

tCCPortDataManager* tCCPortDataBufferPool::CreateInterThreadBuffer()
{
  tCCPortDataManager* pc = tCCPortDataManager::Create(data_type);
  inter_threads->Attach(pc, false);
  return pc;
}

tCCPortDataBufferPool::~tCCPortDataBufferPool()
{
  // delete any returned buffers
  tCCPortQueueElement* pqe = returned_buffers.Dequeue();
  tCCPortDataManagerTL* pc = NULL;
  {
    util::tLock lock2(GetThreadLocalCacheInfosLock());  // for postThreadReleaseLock()
    while (pqe != NULL)
    {
      //pc = static_cast<CCPortDataContainer<>*>(pqe->getElement());
      pc = static_cast<tCCPortDataManagerTL*>(pqe->GetElement());
      pqe->Recycle(false);
      pc->PostThreadReleaseLock();
      pqe = returned_buffers.Dequeue();
    }
  }
  ;
}

util::tMutexLockOrder& tCCPortDataBufferPool::GetThreadLocalCacheInfosLock()
{
  return static_cast<util::tSimpleListWithMutex<tThreadLocalCache*>*>(thread_local_cache_infos.get())->obj_mutex;
}

void tCCPortDataBufferPool::ReleaseLock(tCCPortDataManagerTL* pd)
{
  tCCPortQueueElement* pqe = tThreadLocalCache::GetFast()->GetUnusedCCPortQueueFragment();
  pqe->SetElement(pd);
  assert((pqe->StateChange(static_cast<int8>((util::tAbstractReusable::cUNKNOWN | util::tAbstractReusable::cUSED | util::tAbstractReusable::cPOST_QUEUED)), util::tAbstractReusable::cENQUEUED, this)));
  returned_buffers.Enqueue(pqe);
}

} // namespace finroc
} // namespace core

