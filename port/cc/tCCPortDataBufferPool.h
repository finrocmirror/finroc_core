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
#include "finroc_core_utils/tJCBase.h"

#ifndef CORE__PORT__CC__TCCPORTDATABUFFERPOOL_H
#define CORE__PORT__CC__TCCPORTDATABUFFERPOOL_H

#include "finroc_core_utils/container/tWonderQueue.h"
#include "finroc_core_utils/container/tReusablesPool.h"
#include "core/port/cc/tCCInterThreadContainer.h"
#include "core/port/cc/tCCPortDataContainer.h"
#include "finroc_core_utils/container/tReusablesPoolTL.h"

namespace finroc
{
namespace core
{
class tDataType;
class tCCPortQueueElement;

/*!
 * \author Max Reichardt
 *
 * Buffer pool for specific ("cheap-copy") port data type and thread.
 * In order to be real-time-capable, enough buffers need to be initially allocated.
 * Otherwise the application becomes real-time-capable later - after enough buffers
 * have been allocated.
 */
class tCCPortDataBufferPool : public util::tReusablesPoolTL<tCCPortDataContainer<> >
{
private:

  ::std::tr1::shared_ptr<util::tObject> thread_local_cache_infos;

  /*! List/Queue with buffers returned by other threads */
  util::tWonderQueue<tCCPortQueueElement> returned_buffers;

  /*! Pool with "inter-thread" buffers */
  util::tReusablesPool<tCCInterThreadContainer<> >* inter_threads;

public:

  /*! Data Type of buffers in pool */
  tDataType* data_type;

private:

  /*!
   * \return Create new buffer/instance of port data and add to pool
   */
  tCCPortDataContainer<>* CreateBuffer();

  /*!
   * \return Create new buffer/instance of port data and add to pool
   */
  tCCInterThreadContainer<>* CreateInterThreadBuffer();

  /*!
   * Helper method for getting lock for above method
   *
   * \return Lock
   */
  util::tMutexLockOrder& GetThreadLocalCacheInfosLock();

protected:

  virtual ~tCCPortDataBufferPool();

public:

  /*!
   * \param data_type Type of buffers in pool
   */
  tCCPortDataBufferPool(tDataType* data_type_, int initial_size);

  virtual void AutoDelete()
  {
    ControlledDelete();
  }

  /* (non-Javadoc)
   * @see jc.container.ReusablesPoolTL#controlledDelete()
   */
  virtual void ControlledDelete()
  {
    inter_threads->ControlledDelete();
    ::finroc::util::tReusablesPoolTL<tCCPortDataContainer<> >::ControlledDelete();
  }

  /*!
   * (Is final so it is not used polymorphically.
   * This has merely efficiency reasons (~factor 4)
   * which is critical here.)
   *
   * \return Returns unused buffer. If there are no buffers that can be reused, a new buffer is allocated.
   */
  inline tCCPortDataContainer<>* GetUnusedBuffer()
  {
    tCCPortDataContainer<>* pc = GetUnused();
    if (pc != NULL)
    {
      return pc;
    }
    return CreateBuffer();
  }

  /*!
   * \return Returns unused "inter-thread" buffer. If there are no buffers that can be reused, a new buffer is allocated.
   */
  inline tCCInterThreadContainer<>* GetUnusedInterThreadBuffer()
  {
    tCCInterThreadContainer<>* pc = inter_threads->GetUnused();
    if (pc != NULL)
    {
      return pc;
    }
    return CreateInterThreadBuffer();
  }

  //  /**
  //   * Reclaim any returned PortDataContainers
  //   */
  //  private boolean reclaimReturnedBuffers(ThreadLocalCache tc) {
  //      ArrayWrapper<CCPortDataContainer<?>> buffer = tc.reclaimBuffer;
  //      //typically reclaim maximum of 10 buffers to make things more deterministic/predictable
  //      int n = returnedBuffers.dequeue(buffer, buffer.getCapacity());
  //      for (int i = 0; i < n; i++) {
  //          buffer.get(i).releaseLock();
  //      }
  //      return n;
  //  }

  //  @Override @NonVirtual
  //  public void enqueue(@Ptr CCPortDataContainer<? extends CCPortData> pd) {
  //      assert pd.refCounter == 0;
  //      super.enqueue(pd);
  //  }

  /*!
   * Lock release for non-owner threads - appended to returnedBuffers
   *
   * \param pd Port data to release lock of
   */
  void ReleaseLock(tCCPortDataContainer<>* pd);

};

} // namespace finroc
} // namespace core

#endif // CORE__PORT__CC__TCCPORTDATABUFFERPOOL_H
