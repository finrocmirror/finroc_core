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

#ifndef core__port__std__tPortDataManager_h__
#define core__port__std__tPortDataManager_h__

#include "rrlib/rtti/rtti.h"
#include "rrlib/finroc_core_utils/container/tReusable.h"

#include "core/portdatabase/tAbstractPortDataManager.h"
#include "core/port/tCombinedPointer.h"

namespace finroc
{
namespace core
{
class tPortDataReference;

/*!
 * \author Max Reichardt
 *
 * This class is used for managing a single port data object (or "buffer").
 *
 * It handles information on locks, data type etc.
 *
 * If it possible to derive a port data managers from another port data manager.
 * They will share the same reference counter.
 * This makes sense, when an object contained in the original port data buffer
 * shall be used in a port.
 * This way, it does not need to copied.
 */
class __attribute__((aligned(8))) tPortDataManager : public tAbstractPortDataManager<util::tReusable>
{
public:

  /*!
   * \author Max Reichardt
   *
   * This is a special atomic, thread-safe, non-blocking reference counter.
   *
   * Once, zero is reached, additional locks will fail without ruining the counter.
   *
   * In C++ this is a pseudo-class. The 'this'-pointer points to port of the refCounters
   * variable.
   */
  class tRefCounter
  {
  private:

    std::atomic<short> wrapped; // one less than actual number of references. So -1 when actually no locks.

    /*! Get pointer to manager */
    inline tPortDataManager* GetManager();

  public:

    /*!
     * Add reference/read lock
     *
     * \return Was locking successful? (zero was not already reached)
     */
    inline void AddLock()
    {
      AddLocks(static_cast<int8>(1));
    }

    /*!
     * Add reference/read locks
     *
     * \param count Number of locks to add
     * \return Was locking successful? (zero was not already reached)
     */
    inline void AddLocks(int8 count)
    {
      wrapped += count;
    }

    /*!
     * \return Number of locks
     */
    inline int GetLocks() const
    {
      return (wrapped + 1);
    }

    /*!
     * Is reference counter > 0
     *
     * \return answer
     */
    inline bool IsLocked() const
    {
      return GetLocks() > 0;
    }

    /*!
     * Release one lock
     */
    inline void ReleaseLock()
    {
      ReleaseLocks(static_cast<int8>(1));
    }

    /*!
     * Release specified number of locks
     *
     * \param count Number of locks to release
     */
    inline void ReleaseLocks(int8 count);

    /*!
     * Set reference counter to specified value
     *
     * \param count Number of references/read locks to set
     */
    inline void SetLocks(int8 count)
    {
      assert((GetManager()->unused));
      GetManager()->unused = false;

      wrapped = count - 1;
    }

    /*!
     * Set or add locks - depending on whether the buffer is already used.
     * Possibly unset unused flag.
     * Used buffer already needs to be locked.
     *
     * \return Successful?
     */
    inline void SetOrAddLock()
    {
      SetOrAddLocks(static_cast<int8>(1));
    }

    /*!
     * Set or add locks - depending on whether the buffer is already used.
     * Possibly clears unused flag.
     * Used buffer already needs to be locked.
     *
     * \param count Number of locks to set or add
     */
    inline void SetOrAddLocks(int8 count)
    {
      if (GetManager()->unused)
      {
        SetLocks(count);
      }
      else
      {
        assert((IsLocked()));
        AddLocks(count);
      }
    }

    /*!
     * Try to add reference/read lock
     *
     * \return Was locking successful? (zero was not already reached)
     */
    inline bool TryLock()
    {
      return TryLocks(static_cast<int8>(1));
    }

    tRefCounter()
    {
      wrapped = -1;
    }

    // returns number of locks
    inline short Get() const
    {
      return wrapped;
    }

    /*!
     * Try to add reference/read locks
     *
     * \param count Number of locks to add
     * \return Was locking successful? (zero was not already reached)
     */
    inline bool TryLocks(int8 count)
    {
      assert((!GetManager()->unused));

      std::atomic_fetch_add((std::atomic<int8_t>*)this, count); // mean trick... won't make negative value positive *g* - CPU should not like this, but it's reasonably fast actually
      return wrapped >= 0;

    }

  };

  friend class tPortBase;

  template<typename T>
  friend class tPort;
  friend class tPortDataReference;
  friend class rrlib::rtti::tDataTypeBase;
private:

  /*! Reference counters in this manager - 8 bytes in C++ (due to mean tricks) ... probably > 60 bytes in Java */
  tRefCounter ref_counters[4] __attribute__((aligned(8)));

  /*! Value relevant for publishing thread only - is this still a unused buffer? */
  bool unused;

  /*! PortDataManager that this manager is derived from - null if not derived */
  tPortDataManager* derived_from;

protected:

  /*! incremented every time buffer is reused */
  volatile int reuse_counter;

public:

  /*! Number of reference to port data (same as in PortDataImpl) */
  static const size_t cNUMBER_OF_REFERENCES = 4u;

  /*! Mask for selection of current reference */
  static const size_t cREF_INDEX_MASK = cNUMBER_OF_REFERENCES - 1u;

  /*! Helper variable - e.g. for blackboards */
  int lock_id;

protected:

  /*!
   * Standard constructor
   *
   * \param dt Data Type of managed data
   */
  tPortDataManager();

  /*!
   * Returns reference counter with specified index
   *
   * \param index Index
   * \return reference counter
   */
  inline const tRefCounter* GetRefCounter(size_t index) const
  {
    return &(ref_counters[index]);
  }

  inline tRefCounter* GetRefCounter(size_t index)
  {
    return &(ref_counters[index]);
  }

public:

  /*!
   * Add read lock
   */
  inline void AddLock()
  {
    GetCurrentRefCounter()->AddLock();
  }

  /*!
   * Create object of specified type managed by PortDataManager
   *
   * \param data_type Data type
   * \return Manager
   */
  inline static tPortDataManager* Create(const rrlib::rtti::tDataTypeBase& data_type)
  {
    return static_cast<tPortDataManager*>(data_type.CreateInstanceGeneric<tPortDataManager>()->GetManager());
  }

  /*!
   * Recycle manager and port data...
   * (This method is not intended to be used by framework users. Use only, if you know exactly what you're doing.)
   */
  void DangerousDirectRecycle();

  virtual bool GenericHasLock()
  {
    return IsLocked();
  }

  virtual void GenericLockRelease()
  {
    ReleaseLock();
  }

  inline tPortDataReference* GetCurReference() const
  {
    return tCombinedPointerOps::Create<tPortDataReference>(this, reuse_counter & 0x3);
  }

  /*!
   * \return Returns current reference counter
   * Method is only safe while data is locked!!
   */
  inline const tRefCounter* GetCurrentRefCounter() const
  {
    return GetRefCounter(reuse_counter & cREF_INDEX_MASK);
  }

  inline tRefCounter* GetCurrentRefCounter()
  {
    return GetRefCounter(reuse_counter & cREF_INDEX_MASK);
  }

  /*!
   * (Only meant for debugging purposes)
   *
   * \return Next element in buffer pool
   */
  inline const tPortDataManager* GetNextInBufferPool() const
  {
    return static_cast<tPortDataManager*>(this->next_in_buffer_pool);
  }

  /*!
   * \return Type of managed object
   */
  inline rrlib::rtti::tDataTypeBase GetType() const
  {
    return GetObject()->GetType();
  }

  inline void HandlePointerRelease()
  {
    assert((!IsUnused()) && ("Unused buffers retrieved from ports must be published"));
    ReleaseLock();
  }

  /*! Is port data currently locked (convenience method)? */
  inline bool IsLocked()
  {
    return GetCurrentRefCounter()->IsLocked();
  }

  /*!
   * \return Is this (still) a unused buffer?
   */
  inline bool IsUnused() const
  {
    return unused;
  }

  /*!
   * Recycle unused buffer
   */
  inline void RecycleUnused()
  {
    GetCurrentRefCounter()->SetOrAddLock();
    ReleaseLock();
  }

  /*!
   * Release read lock
   */
  inline void ReleaseLock()
  {
    GetCurrentRefCounter()->ReleaseLock();
  }

  /*!
   * \param unused Is this (still) a unused buffer?
   */
  inline void SetUnused(bool unused_)
  {
    this->unused = unused_;
  }

  virtual const util::tString ToString() const;

};

} // namespace finroc
} // namespace core

namespace finroc
{
namespace core
{

tPortDataManager* tPortDataManager::tRefCounter::GetManager()
{
  // Reference counter offset in class (offsetof-makro doesn't work here :-/ )
  const size_t cREF_COUNTERS_OFFSET = ((char*) & ((tPortDataManager*)1000)->ref_counters[0]) - ((char*)1000);
  return (tPortDataManager*)((((size_t)this) & tCombinedPointerOps::cPOINTER_MASK) - cREF_COUNTERS_OFFSET); // memory layout allows doing this - shrinks class to <=33%
}

void tPortDataManager::tRefCounter::ReleaseLocks(int8 count)
{
  assert((!GetManager()->unused));

  short new_val = wrapped.fetch_add(-count) - count;
  if (new_val < 0)
  {
    GetManager()->DangerousDirectRecycle();
    //getManager()->reuseCounter++;
  }

}

} // namespace finroc
} // namespace core

#endif // core__port__std__tPortDataManager_h__
