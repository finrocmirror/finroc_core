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

#ifndef core__port__cc__tCCPortDataManagerTL_h__
#define core__port__cc__tCCPortDataManagerTL_h__

#include "rrlib/finroc_core_utils/definitions.h"

#include "rrlib/finroc_core_utils/thread/sThreadUtil.h"
#include "rrlib/finroc_core_utils/container/tReusableTL.h"
#include "core/portdatabase/tReusableGenericObjectManagerTL.h"

#include "core/port/tCombinedPointer.h"

namespace rrlib
{
namespace serialization
{
class tDataTypeBase;
} // namespace rrlib
} // namespace serialization

namespace finroc
{
namespace core
{
class tCCPortDataRef;
class tCCPortDataBufferPool;

/*!
 * \author Max Reichardt
 *
 * Manager for thread-local "cheap-copy" port data.
 * Contains diverse management information such as reuse
 * queue pointers - possibly time stamp.
 */
class __attribute__((aligned(8))) tCCPortDataManagerTL : public tReusableGenericObjectManagerTL
{
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

  /*! Number of reference to port data */
  const static size_t cNUMBER_OF_REFERENCES = 8;

  /*! Mask for selection of current reference */
  static const size_t cREF_INDEX_MASK = cNUMBER_OF_REFERENCES - 1u;

  tCCPortDataManagerTL();

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

  //    // object parameter only used in Java
  //    public CCPortDataManagerTL(DataTypeBase type, @Ptr @CppDefault("NULL") Object object) {
  //        portData = new GenericObjectInstance<T>(type, object);
  //        ownerThread = ThreadUtil.getCurrentThreadId();
  //    }

  virtual ~tCCPortDataManagerTL()
  {
    //finroc::util::System::out.println(finroc::util::StringBuilder("Deleting container ") + this);
  }

  //    /** Assign other data to this container */
  //    public void assign(@Const CCPortData other) {
  //        portData.assign(other);
  //    }
  //
  //    /** Copy current data to other object */
  //    @ConstMethod public void assignTo(CCPortData buffer) {
  //        portData.assignTo(buffer);
  //    }
  //

  /*!
   * \param other Other object
   * \return Is data in this container equal to data provided as parameter?
   * (In C++ this is a heuristic for efficiency reasons. Objects are compared via memcmp.
   *  If classes contain pointers, this only guarantees that classes identified as equal really are.
   *  This is typically sufficient for "cheap copy" types though.)
   */
  inline bool ContentEquals(const void* other) const
  {
    return memcmp(GetObject()->GetRawDataPtr(), other, GetObject()->GetType().GetSize()) == 0;
  }

  inline void HandlePointerRelease()
  {
    ReleaseLock();
  }

  /*!
   * Create object of specified type managed by CCPortDataManagerTL
   *
   * \param data_type Data type
   * \return Manager
   */
  inline static tCCPortDataManagerTL* Create(const rrlib::serialization::tDataTypeBase& data_type)
  {
    return static_cast<tCCPortDataManagerTL*>(data_type.CreateInstanceGeneric<tCCPortDataManagerTL>()->GetManager());
  }

  /*!
   * \return Current reference to use
   */
  inline tCCPortDataRef* GetCurrentRef()
  {
    return tCombinedPointerOps::Create<tCCPortDataRef>(this, reuse_counter & cREF_INDEX_MASK);
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

  /*!
   * \return Does current thread own this data container?
   */
  inline bool IsOwnerThread()
  {
    return util::sThreadUtil::GetCurrentThreadId() == owner_thread;
  }

  /*!
   * (may be called by any thread)
   * (Needs to be called with lock on ThreadLocalCache::infos)
   * Removes read lock
   */
  void NonOwnerLockRelease(tCCPortDataBufferPool* owner);

  /*!
   * (Should only be called by port related classes)
   * (Needs to be called with lock on ThreadLocalCache::infos)
   * Release-method to call after owner thread has terminated
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
    return util::tStringBuilder("CCPortDataContainer: ") + GetContentString();
  }

//
//    void setData(const T& data) {
//        setData(&data);
//    }
//
//
//    /**
//     * Assign new value to container
//     *
//     * \param data new value
//     */
//    public void setData(@Const @Ptr T data) {
//        portData.assign((CCPortData)data);
//    }

};

} // namespace finroc
} // namespace core

#endif // core__port__cc__tCCPortDataManagerTL_h__
