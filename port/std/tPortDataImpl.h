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

#ifndef CORE__PORT__STD__TPORTDATAIMPL_H
#define CORE__PORT__STD__TPORTDATAIMPL_H

#include "core/portdatabase/tTypedObjectImpl.h"

namespace finroc
{
namespace core
{
class tPortDataManager;
class tPortDataReference;
class tDataType;

/*!
 * \author Max Reichardt
 *
 * This is the abstract base class for all data that is used in ports.
 *
 * There are diverse management tasks (these task are handled by the data's manager).
 *  - Keeping track of "users" (reference counting - read locks would be more precise)
 *  - Managing Timestamps
 *
 * By convention, port data is immutable while published/read-locked/referenced.
 */
class __attribute__((aligned(8))) tPortData : public tTypedObject
{
private:
// 4byte => 4byte

  /*! Manager of data */
  tPortDataManager* manager;

public:

  /*! Port to whose PortDataContainerPool this buffer belongs - automatically counts as initial user */
  //protected final Port<?> origin;

  /*! Last iteration of PortTracker when he found this buffer assigned to a port */
  //protected volatile int lastTracked = -5;
  //public static final int FILLING = Integer.MAX_VALUE;

  /*! Value to add for user lock */
  //private final static int USER_LOCK = 0x10000;

  /*! Constant to AND refCounter with to determine whether there's a user lock */
  //private final static int USER_LOCK_MASK = 0xFFFF0000;

  /*! Constant to AND refCounter with to determine whether there's a system lock */
  //private final static int SYSTEM_LOCK_MASK = 0xFFFF;

  /*! Log domain for serialization */
  RRLIB_LOG_CREATE_NAMED_DOMAIN(log_domain, "serialization");

protected:

  /*!
   * \return lookup object's data type - may be overriden by subclass
   */
  virtual tDataType* LookupDataType();

public:

  /*!
   * Constructor as base class
   */
  tPortData();

  inline tPortDataReference* GetCurReference() const;

  /*!
   * \return Manager of data (null for PortData not used in ports)
   */
  inline tPortDataManager* GetManager() const
  {
    return manager;
  }

  /* (non-Javadoc)
   * @see core.port7.std.PortData#handleRecycle()
   */
  virtual void HandleRecycle()
  {
    // default: do nothing
  }

  //  /**
  //   * Add read lock to buffer.
  //   * Prerequisite: Buffer needs to be already read-locked...
  //   * (usually the case after getting buffer from port)
  //   */
  //  // no extreme optimization necessary, since not called that often...
  //  public void addReadLock() {
  //      //int counterIndex = (refCounter.get() >> 28) & 0x3;
  //      int counterIndex = reuseCounter & 0x3;
  //      int old = refCounter.getAndAdd(refCounterIncrement[counterIndex]);
  //      assert ((old & refCounterMasks[counterIndex]) != 0) : "Element already reused. Application in undefined state. Element has to be locked prior to calling this method.";
  //      assert (counterIndex == ((old >> 28) & 0x3)) : "Counter index changed. Application in undefined state. Element has to be locked prior to calling this method.";
  //      assert (counterIndex != refCounterMasks[counterIndex]) : "Reference counter overflow. Maximum of 127 locks allowed. Consider making a copy somewhere.";
  //  }
  //
  //  /**
  //   * Release lock from buffer
  //   */
  //  public void releaseLock() {
  //      int counterIndex = reuseCounter & 0x3;
  //      int count = refCounter.addAndGet(-refCounterIncrement[counterIndex]);
  //      assert ((count & refCounterMasks[counterIndex]) != refCounterMasks[counterIndex]) : "More locks released than acquired. Application in undefined state.";
  //      if ((count & refCounterMasks[counterIndex]) == 0) {
  //          // reuse object
  //          PortDataBufferPool owner = this.owner;
  //          reuseCounter++;
  //          if (owner != null) {
  //              owner.enqueue(this);
  //          } else {
  //               delete this;
  //          }
  //      }
  //  }

  /*!
   * initialize data type
   * (typically called by PortDataCreationInfo)
   */
  void InitDataType();

  /*!
   * For Port.get().
   *
   * A user lock is added to object, if there's still a system lock.
   * Otherwise it is outdated and the next one in port should be used.
   * User locks may still appear to be there although object has been reused,
   * if this method is called concurrently.
   *
   * \return Did lock succeed?
   */
  /*boolean addUserLockIfSystemLock() {
      int old = refCounter.getAndAdd(USER_LOCK);
      if ((old & SYSTEM_LOCK_MASK) <= 0) {
          refCounter.getAndAdd(-USER_LOCK); // remove interference
          return false;
      }
      return true;
  }*/

  /*!
   * Add read lock to buffer (thread safe)
   *
   * \return Did lock succeed (or is element already reused) ?
   */
  /*void addSystemReadLock() {
      int old = refCounter.getAndIncrement();
      if (old <= 0) {
          throw new RuntimeException("Element already reused");
      }
  }*/

  /*!
   * Release read lock (thread safe)
   */
  virtual const util::tString ToString() const
  {
    return "some port data";
  }

};

} // namespace finroc
} // namespace core

#include "core/port/std/tPortDataReference.h"

namespace finroc
{
namespace core
{
tPortDataReference* tPortData::GetCurReference() const
{
  tPortDataManager* mgr = GetManager();
  return tCombinedPointerOps::Create<tPortDataReference>(this, mgr->reuse_counter & 0x3);
}

} // namespace finroc
} // namespace core

#endif // CORE__PORT__STD__TPORTDATAIMPL_H
