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

#ifndef CORE__PORT__CC__TCCPORTDATACONTAINER_H
#define CORE__PORT__CC__TCCPORTDATACONTAINER_H

#include "core/portdatabase/tTypedObjectContainer.h"
#include "finroc_core_utils/thread/sThreadUtil.h"
#include "core/port/cc/tCCPortData.h"
#include "core/buffers/tCoreInput.h"
#include "core/buffers/tCoreOutput.h"
#include "finroc_core_utils/container/tReusableTL.h"
#include "core/port/cc/tCCContainerBase.h"

#include "core/port/tCombinedPointer.h"

namespace finroc
{
namespace core
{
class tDataType;
class tCCPortDataRef;
class tCCPortDataBufferPool;

/*!
 * \author Max Reichardt
 *
 * Container for "cheap-copy" port data.
 * Contains diverse management information such as data type an reuse
 * queue pointers - possibly time stamp
 */
template < typename T = tCCPortData >
class __attribute__((aligned(8))) tCCPortDataContainer : public util::tReusableTL, public tCCContainerBase
{
  friend class tCCPortDataBufferPool;
  friend class tCCPortBase;
private:

  /*! Reuse counter */
  size_t reuse_counter;

  /*! ID of thread that owns this PortDataContainer */
  const int64 owner_thread;

protected:

  /*! Reference counter for owner - typically equal to number of ports data is assigned to */
  int ref_counter;

public:

  // for synchronization on an object of this class
  mutable util::tMutex obj_synch;

  /*! Number of reference to port data */
  const static size_t cNUMBER_OF_REFERENCES = 8;

  /*! Mask for selection of current reference */
  static const size_t cREF_INDEX_MASK = cNUMBER_OF_REFERENCES - 1u;

  /*!
   * Actual data - important: last field in this class - so offset in
   * C++ is fixed and known - regardless of template parameter
   */
  tTypedObjectContainer<T> port_data;

  // object parameter only used in Java
  tCCPortDataContainer(tDataType* type, util::tObject* object = NULL);

  /*!
   * (May only be called by owner thread)
   * Add read lock
   */
  inline void AddLock()
  {
    AddLocks(1);
  }

  /*!
   * (May only be called by owner thread)
   * Add the specified number of read locks
   *
   * \param count Number of read locks to add
   */
  inline void AddLocks(int count)
  {
    assert((owner_thread == util::sThreadUtil::GetCurrentThreadId()) && "may only be called by owner thread");
    ref_counter += count;
  }

  virtual ~tCCPortDataContainer()
  {
    //finroc::util::System::out.println(finroc::util::StringBuilder("Deleting container ") + this);
  }

  /*! Assign other data to this container */
  inline void Assign(const tCCPortData* other)
  {
    port_data.Assign(other);
  }

  /*! Copy current data to other object */
  inline void AssignTo(tCCPortData* buffer) const
  {
    port_data.AssignTo(buffer);
  }

  /*! @return Is data in this container equal to data in other container? */
  inline bool ContentEquals(const tCCPortData* other) const
  {
    return port_data.ContentEquals(other);
  }

  virtual void Deserialize(tCoreInput& is)
  {
    port_data.Deserialize(is);
  }

  /*!
   * \return Current reference to use
   */
  inline tCCPortDataRef* GetCurrentRef()
  {
    return tCombinedPointerOps::Create<tCCPortDataRef>(this, reuse_counter & cREF_INDEX_MASK);
  }

  /*!
   * \return Actual data
   */
  inline const T* GetData() const
  {
    return port_data.GetData();
  }

  inline T* GetData()
  {
    return port_data.GetData();
  }

  /*!
   * \return Pointer to actual data (beginning of data - important for multiple inheritance)
   */
  inline const void* GetDataPtr() const
  {
    return port_data.GetData();
  }

  inline void* GetDataPtr()
  {
    return port_data.GetData();
  }

  /*!
   * \return the ownerThread Thread ID of owner
   */
  inline int64 GetOwnerThread() const
  {
    return owner_thread;
  }

  /*!
   * \return Reference counter
   */
  inline int GetRefCounter()
  {
    return ref_counter;
  }

  virtual bool IsInterThreadContainer()
  {
    return false;
  }

  /*!
   * \return Does current thread own this data container?
   */
  inline bool IsOwnerThread()
  {
    return util::sThreadUtil::GetCurrentThreadId() == owner_thread;
  }

  /*!
   * (may be called by any thread)
   * Removes read lock
   */
  void NonOwnerLockRelease(tCCPortDataBufferPool* owner);

  /*!
   * (Should only be called by port related classes)
   * Release method to call after owner thread has terminated
   */
  void PostThreadReleaseLock();

  /*!
   * (May only be called by owner thread)
   * Remove read lock
   */
  inline void ReleaseLock()
  {
    ReleaseLocks(1);
  }

  /*!
   * (May only be called by owner thread)
   * Remove the specified number of read locks
   *
   * \param count Number of read locks to release
   */
  inline void ReleaseLocks(int count)
  {
    assert((ref_counter >= count) && "More locks released than acquired");
    assert((owner_thread == util::sThreadUtil::GetCurrentThreadId()) && "may only be called by owner thread");
    ref_counter -= count;
    if (ref_counter == 0)
    {
      reuse_counter++;
      Recycle();
    }
  }

  virtual void Serialize(tCoreOutput& os) const
  {
    port_data.Serialize(os);
  }

  /*!
   * Set reference counter directly
   * (should only be called by port classes)
   *
   * \param ref_counter
   */
  inline void SetRefCounter(int ref_counter_)
  {
    this->ref_counter = ref_counter_;
  }

  virtual const util::tString ToString() const
  {
    return util::tStringBuilder("CCPortDataContainer: ") + port_data.ToString();
  }

};

} // namespace finroc
} // namespace core

#include "core/port/cc/tCCPortDataContainer.hpp"

#endif // CORE__PORT__CC__TCCPORTDATACONTAINER_H
