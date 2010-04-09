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

#ifndef CORE__PORT__STD__TPORTDATAMANAGER_H
#define CORE__PORT__STD__TPORTDATAMANAGER_H

#include "core/portdatabase/tDataType.h"
#include "finroc_core_utils/container/tReusable.h"

#include "core/port/tCombinedPointer.h"

namespace finroc
{
namespace core
{
class tPortData;

/*!
 * \author Max Reichardt
 *
 * This class is used for allocating/managing/deleting a single port data object that
 * is used in ports.
 * It handles information on locks etc.
 * It may do this for multiple port data objects if port data object owns other
 * port data object.
 */
class tPortDataManager : public util::tReusable
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

    ::tbb::atomic<short> wrapped; // one less than actual number of references. So -1 when actually no locks.

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
     * Possibly unset unused flag.
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

      __TBB_machine_fetchadd1(this, count); // mean trick... won't make negative value positive *g* - CPU should not like this, but it's reasonably fast actually
      return wrapped >= 0;

    }

  };

  friend class tPortBase;

  template<typename T>
  friend class tPort;
  friend class tPortData;
  friend class tPortDataReference;
private:

  /*! Reference counters in this manager - 8 bytes in C++ (due to mean tricks) ... probably > 60 bytes in Java */
  tRefCounter ref_counters[4] __attribute__((aligned(8)));

  /*! Value relevant for publishing thread only - is this still a unused buffer? */
  bool unused;

  // Reference counter offset in class (offsetof-makro doesn't work here :-/ )
  static size_t cREF_COUNTERS_OFFSET;

  // PortDataManager prototype to obtain above offset
  static tPortDataManager cPROTOTYPE;

  tPortDataManager() {} // dummy constructor for prototype

  /*! Pointer to actual PortData (outermost buffer) */
  tPortData* data;

protected:

  //  /**
  //   * Reference/Lock counter - it is divided in four to avoid collisions with multiple iterations:
  //   * [7bit counter[0]][7bit counter[1]][7bit counter[2]][7bit counter[3]][4 bit reuse counter]
  //   * The reuse counter is incremented (and wrapped around) every time the buffer is recycled.
  //   * Reuse counter % 4 is the index of the counter used (fast operation & 0x03)
  //   * Warnings should be thrown if the reuse counter increases by two or more during a lock operation.
  //   * If it increases by four or more, the counters may become corrupted. The program should be aborted.
  //   * Increasing the initial number of buffers in the publish pool should solve the problem.
  //   *
  //   * There is a maximum of 127 locks per object (7 bit).
  //   **/
  //  protected final AtomicInt refCounter = new AtomicInt(); // 4byte => 16byte

  //  /**
  //   * for optimizations: Thread that owns this buffer... could be moved to owner in order to save memory... however, owner would need to final => not null
  //   */
  //  private final long ownerThread;

  //  /** for optimizations: Reference/Lock counter for owner thread - when zero, the refCounter is decremented */
  //  int ownerRefCounter = 0;

  /*! incremented every time buffer is reused */
  volatile int reuse_counter;

public:

  // static helper variables

  /*! Bit masks for different counters in refCounter */
  //protected final static int[] refCounterMasks = new int[]{0xFE000000, 0x1FC0000, 0x3F800, 0x7F0};

  /*! Increment constant for different counters in refCounter */
  //protected final static int[] refCounterIncrement = new int[]{0x2000000, 0x40000, 0x800, 0x10};

  /*! Maximum number of locks */
  //public final static int MAX_LOCKS = 63;/*refCounterMasks[3] / 2; // due to sign*/

  /*!
   * Masks for retrieving additional lock info from value pointer
   */
  //public static final @SizeT int LOCK_INFO_MASK = 0x7;

  /*! Number of reference to port data (same as in PortDataImpl) */
  static const size_t cNUMBER_OF_REFERENCES = 4u;

  /*! Mask for selection of current reference */
  static const size_t cREF_INDEX_MASK = cNUMBER_OF_REFERENCES - 1u;

protected:

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

  virtual ~tPortDataManager();

  tPortDataManager(tDataType* dt, const tPortData* port_data);

  /*!
   * Add read lock
   */
  inline void AddLock()
  {
    GetCurrentRefCounter()->AddLock();
  }

  /*!
   * Recycle manager and port data...
   * (This method is not intended to be used by framework users. Use only, if you know exactly what you're doing.)
   */
  void DangerousDirectRecycle();

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

  //  /**
  //   * \return Thread that owns this buffer
  //   */
  //  @Inline public long getOwnerThread() {
  //    return ownerThread;
  //  }

  //  /**
  //   * Release lock from buffer... may only be calld by owner thread
  //   *
  //   * (Should only be called by port directly)
  //   */
  //  @Inline public void releaseOwnerLock() {
  //    ownerRefCounter--;
  //    //System.out.println("Release Lock: " + data + " " + ownerRefCounter);
  //    if (ownerRefCounter == 0) {
  //      releaseNonOwnerLock();
  //    }
  //    assert ownerRefCounter >= 0 : "More locks released than acquired. Application in undefined state.";
  //  }
  //
  //  /**
  //   * Release lock from buffer
  //   */
  //  @Inline public void releaseLock() {
  //    if (ThreadUtil.getCurrentThreadId() == ownerThread) {
  //      releaseOwnerLock();
  //    } else {
  //      releaseNonOwnerLock();
  //    }
  //  }
  //
  //  /**
  //   * Release lock from buffer - atomic add operation
  //   */
  //  @Inline private void releaseNonOwnerLock() {
  //    int counterIndex = reuseCounter & 0x3;
  //    int count = refCounter.addAndGet(-refCounterIncrement[counterIndex]);
  //    assert ((count & refCounterMasks[counterIndex]) != refCounterMasks[counterIndex]) : "More locks released than acquired. Application in undefined state.";
  //    if ((count & refCounterMasks[counterIndex]) == 0) {
  //      // reuse object
  //      //System.out.println("Recycling: " + data + " " + reuseCounter);
  //      reuseCounter++;
  //      recycle();
  //    }
  //  }

  inline const tPortData* GetData() const
  {
    return data;
  }

  inline tPortData* GetData()
  {
    return data;
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
   * TODO
   *
   * \return Manager time stamp
   */
  inline int64 GetTimestamp()
  {
    return 0;
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
   * Release read lock
   */
  inline void ReleaseLock()
  {
    GetCurrentRefCounter()->ReleaseLock();
  }

  //  /**
  //   * (only called by port class)
  //   * Set locks to specified amount
  //   *
  //   * \param count number of locks
  //   */
  //  @Inline protected void setLocksAsOwner(int count) {
  //    int counterIndex = reuseCounter & LOCK_INFO_MASK;
  //    int counterIndex2 = counterIndex & 0x3;
  //    refCounter.set((refCounterIncrement[counterIndex2]) | counterIndex);
  //    ownerRefCounter = count;
  //  }
  //
  //  // see above
  //  @Inline protected void setLocksAsNonOwner(int count) {
  //    int counterIndex = reuseCounter & LOCK_INFO_MASK;
  //    int counterIndex2 = counterIndex & 0x3;
  //    refCounter.set((refCounterIncrement[counterIndex2] * count) | counterIndex);
  //  }

  //  /**
  //   * (only called by port classes)
  //   * Set locks to specified amount
  //   *
  //   * \param count number of locks
  //   */
  //  @Inline public void setLocks(int count) {
  //    /*int counterIndex = reuseCounter & LOCK_INFO_MASK;
  //    int counterIndex2 = counterIndex & 0x3;*/
  //    int counterIndex2 = reuseCounter & 0x3;
  //    refCounter.set((refCounterIncrement[counterIndex2] * count) | counterIndex2);
  //  }

  //  // see above
  //  @Inline protected @Ptr PortDataManager setLocks(int count) {
  //    if (ThreadUtil.getCurrentThreadId() == ownerThread) {
  //      setLocksAsOwner(count);
  //    } else {
  //      setLocksAsNonOwner(count);
  //    }
  //    return this;
  //  }

  //  /**
  //   * \return Is Buffer currently locked?
  //   */
  //  @Inline public boolean isLocked() {
  //    int counterIndex = reuseCounter & 0x3;
  //    int count = refCounter.get();
  //    return (count & refCounterMasks[counterIndex]) > 0;
  //  }
  //
  //  /**
  //   * Acquire read lock.
  //   * Prerequisite: Buffer needs to be already read-locked...
  //   * (usually the case after getting buffer from port)
  //   */
  //  @Inline public void addReadLock() {
  //    if (ThreadUtil.getCurrentThreadId() == ownerThread) {
  //      if (ownerRefCounter > 0) {
  //        ownerRefCounter++;
  //        return;
  //      } else {
  //        ownerRefCounter = 1;
  //      }
  //    }
  //    int counterIndex = reuseCounter & 0x3;
  //    int old = refCounter.getAndAdd(refCounterIncrement[counterIndex]);
  //    assert ((old & refCounterMasks[counterIndex]) != 0) : "Element already reused. Application in undefined state. Element has to be locked prior to calling this method.";
  //    assert (counterIndex == ((old >> 28) & 0x3)) : "Counter index changed. Application in undefined state. Element has to be locked prior to calling this method.";
  //    assert (counterIndex != refCounterMasks[counterIndex]) : "Reference counter overflow. Maximum of 127 locks allowed. Consider making a copy somewhere.";
  //  }
  //
  //  /**
  //   * Add owner lock.
  //   * Precondition: there's already a owner lock
  //   */
  //  void addOwnerLock() {
  //    assert ownerRefCounter > 0 : "Element already reused. Application in undefined state. Element has to be locked prior to calling this method.";
  //    ownerRefCounter++;
  //  }

  /*!
   * Releases all lock from object
   * (should only be called prior to deletion - by ports)
   */
  /*void releaseAllLocks() {
    assert refCounter.get() > 0 : "Element already recycled";
    refCounter.set(0);
    ownerRefCounter = 0;

    // reuse object
    PortDataBufferPool owner = this.owner;
    reuseCounter++;
    if (owner != null) {
      owner.enqueue(this);
    } else {
       delete this;
    }
  }*/

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
  return (tPortDataManager*)((((size_t)this) & tCombinedPointerOps::cPOINTER_MASK) - cREF_COUNTERS_OFFSET); // memory layout allows doing this - shrinks class to <=33%
}

void tPortDataManager::tRefCounter::ReleaseLocks(int8 count)
{
  assert((!GetManager()->unused));

  short new_val = wrapped.fetch_and_add(-count) - count;
  if (new_val < 0)
  {
    GetManager()->DangerousDirectRecycle();
    //getManager()->reuseCounter++;
  }

}

} // namespace finroc
} // namespace core

#endif // CORE__PORT__STD__TPORTDATAMANAGER_H
