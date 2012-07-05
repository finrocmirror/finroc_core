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

#ifndef core__port__cc__tCCPortDataBufferPool_h__
#define core__port__cc__tCCPortDataBufferPool_h__

#include "rrlib/finroc_core_utils/definitions.h"

#include "core/port/cc/tCCPortDataManager.h"
#include "rrlib/rtti/tDataTypeBase.h"
#include "rrlib/finroc_core_utils/container/tWonderQueue.h"
#include "rrlib/finroc_core_utils/container/tReusablesPool.h"
#include "rrlib/finroc_core_utils/container/tReusablesPoolTL.h"

namespace finroc
{
namespace core
{
class tCCPortQueueElement;
class tCCPortDataManagerTL;

/*!
 * \author Max Reichardt
 *
 * Buffer pool for specific ("cheap-copy") port data type and thread.
 * In order to be real-time-capable, enough buffers need to be initially allocated.
 * Otherwise the application becomes real-time-capable later - after enough buffers
 * have been allocated.
 */
class tCCPortDataBufferPool : public util::tReusablesPoolTL<tCCPortDataManagerTL>
{
private:

  /*! List/Queue with buffers returned by other threads */
  util::tWonderQueue<tCCPortQueueElement> returned_buffers;

  /*! Pool with "inter-thread" buffers */
  util::tReusablesPool<tCCPortDataManager>* inter_threads;

public:

  /*! Data Type of buffers in pool */
  const rrlib::rtti::tDataTypeBase data_type;

private:

  /*!
   * \return Create new buffer/instance of port data and add to pool
   */
  tCCPortDataManagerTL* CreateBuffer();

  /*!
   * \return Create new buffer/instance of port data and add to pool
   */
  tCCPortDataManager* CreateInterThreadBuffer();

  /*!
   * Helper method for getting lock for above method
   *
   * \return Lock
   */
  rrlib::thread::tRecursiveMutex& GetThreadLocalCacheInfosLock();

protected:

  virtual ~tCCPortDataBufferPool();

public:

  /*!
   * \param data_type Type of buffers in pool
   */
  tCCPortDataBufferPool(const rrlib::rtti::tDataTypeBase& data_type_, int initial_size);

  virtual void CustomDelete(bool called_from_gc)
  {
    if (called_from_gc)
    {
      tSafeDestructible::CustomDelete(called_from_gc);
    }
    else
    {
      ControlledDelete();
    }
  }

  /* (non-Javadoc)
   * @see jc.container.ReusablesPoolTL#controlledDelete()
   */
  virtual void ControlledDelete();

  /*!
   * (Is final so it is not used polymorphically.
   * This has merely efficiency reasons (~factor 4)
   * which is critical here.)
   *
   * \return Returns unused buffer. If there are no buffers that can be reused, a new buffer is allocated.
   */
  inline tCCPortDataManagerTL* GetUnusedBuffer()
  {
    tCCPortDataManagerTL* pc = GetUnused();
    if (pc != NULL)
    {
      return pc;
    }
    return CreateBuffer();
  }

  /*!
   * \return Returns unused "inter-thread" buffer. If there are no buffers that can be reused, a new buffer is allocated.
   */
  inline tCCPortDataManager* GetUnusedInterThreadBuffer()
  {
    tCCPortDataManager* pc = inter_threads->GetUnused();
    if (pc != NULL)
    {
      return pc;
    }
    return CreateInterThreadBuffer();
  }

  /*!
   * Lock release for non-owner threads - appended to returnedBuffers
   *
   * \param pd Port data to release lock of
   */
  void ReleaseLock(tCCPortDataManagerTL* pd);

};

} // namespace finroc
} // namespace core

#endif // core__port__cc__tCCPortDataBufferPool_h__
