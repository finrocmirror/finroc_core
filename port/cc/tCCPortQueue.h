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
#include "rrlib/finroc_core_utils/tJCBase.h"

#ifndef CORE__PORT__CC__TCCPORTQUEUE_H
#define CORE__PORT__CC__TCCPORTQUEUE_H

#include "core/port/tThreadLocalCache.h"
#include "rrlib/finroc_core_utils/container/tWonderQueueBounded.h"
#include "core/port/cc/tCCInterThreadContainer.h"

namespace finroc
{
namespace core
{
class tCCPortQueueElement;

/*!
 * \author Max Reichardt
 *
 * FIFO Queue that is used in ports.
 *
 * Thread-safe, non-blocking and very efficient for writing.
 * Anything can be enqueued - typically PortData.
 *
 * Use concurrentDequeue, with threads reading from this queue concurrently.
 */
template < typename T = tCCPortData >
class tCCPortQueue : public util::tWonderQueueBounded<tCCInterThreadContainer<T>, tCCPortQueueElement>
{
  inline tCCPortQueueElement* GetEmptyContainer2()
  {
    return tThreadLocalCache::GetFast()->GetUnusedCCPortQueueFragment();
  }

protected:

  virtual tCCPortQueueElement* GetEmptyContainer()
  {
    return GetEmptyContainer2();
  }

public:

  tCCPortQueue(int max_length) :
      util::tWonderQueueBounded<tCCInterThreadContainer<T>, tCCPortQueueElement>(max_length)
  {
  }

  virtual ~tCCPortQueue()
  {
    ::finroc::util::tWonderQueueBounded<tCCInterThreadContainer<T>, tCCPortQueueElement>::Clear(true);
    ;
  }

//  public void enqueue(PortDataReference pdr) {
//      CCPortQueueElement pqe = getEmptyContainer2();
//      assert(pqe.getElement() == null);
//      assert(!pqe.isDummy());
//      pqe.setElement(pdr);
//      super.enqueueDirect(pqe);
//  }

//  /**
//   * How many chunks should be allocated in advance? (one is typically far more than enough).
//   * However, in special cases it might be necessary to preallocate more (if one thread may
//   * write masses of data to this port in a very short time - this will end in an null pointer exception).
//   */
//  private final int preAllocate;
//
//  /** Fragments or chunks that are currently used */
//  private final PortQueueFragment[] chunks;
//
//  /** Counts number of writes - element is found in chunks[writeIndex & chunkMask]->array[writeIndex & PQF.MASK]*/
//  private final AtomicInteger writeIndex = new AtomicInteger(0);
//
//  /** Counts number of reads */
//  private int readIndex;
//
//  /** Mask to and index with to get chunk (see below) */
//  private final int chunkMask;
//
//  /** above, but unshifted */
//  private final int rawChunkMask;
//
//  /**
//   * \param maxElements Maximum number of elements in queue.
//   * \param preAllocate How many chunks should be allocated in advance? (one is typically far more that enough).
//   * However, in special cases it might be necessary to preallocate more (if one thread may
//   * write masses of elements to this port at once - this will end in an null pointer exception).
//   */
//  public PortQueue(int maxElements, int preAllocate) {
//      int chunkCount = (maxElements - 1 / PortQueueFragment.SIZE) + 1 + 3 + preAllocate; // (3 is for safety/reservation overhead)
//      int bits = 32 - Integer.numberOfLeadingZeros(chunkCount - 1);
//      chunkCount = 1 << bits;
//      chunks = new PortQueueFragment[chunkCount];
//      this.preAllocate = Math.min(1, preAllocate);
//
//      // calculate mask
//      rawChunkMask = ((1 << bits) - 1);
//      chunkMask = rawChunkMask << PortQueueFragment.BITS;
//
//      // fill first <preallocate> chunks
//      for (int i = 0; i < preAllocate; i++) {
//          chunks[i] = ThreadLocalCache.get().getUnusedPortQueueFragment();
//      }
//  }
//
//  /**
//   * Enqueue element in queue
//   * (Thread-safe)
//   *
//   * \param element Element to add
//   */
//  public void enqueue(PortData element) {
//
//
//
//      int index = writeIndex.getAndIncrement(); // the only atomic operation necessary
//      int chunkIdx = index & chunkMask;
//      int elemIdx = index & PortQueueFragment.MASK;
//      if (elemIdx == 0) { // time for allocation?
//          int idx = (chunkIdx + preAllocate) & rawChunkMask;
//          assert(chunks[idx] == null);
//          chunks[idx] = ThreadLocalCache.get().getUnusedPortQueueFragment();
//      }
//      assert(chunks[chunkIdx].array[elemIdx] == null);
//      chunks[chunkIdx].array[elemIdx] = element;
//  }
//
//  /**
//   * Dequeue element from queue.
//   * (May only be called by one thread concurrently.)
//   *
//   * \return Element returned element
//   */
//  @SuppressWarnings("unchecked")
//  public T dequeue() {
//      int index = readIndex;
//      int chunkIdx = index & chunkMask;
//      int elemIdx = index & PortQueueFragment.MASK;
//      T result = (T)chunks[chunkIdx].array[elemIdx];
//      if (result != null) {
//          chunks[chunkIdx].array[elemIdx] = null;
//          index++;
//          if (elemIdx == PortQueueFragment.MASK) { // recycle current chunk?
//              chunks[chunkIdx].recycle();
//              chunks[chunkIdx] = null;
//          }
//      }
//      return result;
//  }

};

} // namespace finroc
} // namespace core

#endif // CORE__PORT__CC__TCCPORTQUEUE_H
