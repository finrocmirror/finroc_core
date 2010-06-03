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

#ifndef CORE__PORT__RPC__TMETHODCALLSYNCHER_H
#define CORE__PORT__RPC__TMETHODCALLSYNCHER_H

#include "core/tLockOrderLevels.h"
#include "core/port/rpc/tAbstractCall.h"
#include "core/port/tThreadLocalCache.h"

namespace finroc
{
namespace core
{
/*!
 * \author Max Reichardt
 *
 * Thread local class for forwarding method return values
 * back to calling thread.
 */
class tMethodCallSyncher : public util::tUncopyableObject
{
private:

  /*! Maximum number of active/alive threads that perform synchronous method calls */
  static const size_t cMAX_THREADS = 127u;

  /*! PreAllocated array of (initially empty) MethodCallSyncher classes */
  static ::finroc::util::tArrayWrapper<tMethodCallSyncher> slots;

  /*! Index in array */
  size_t index;

  /*! Thread currently associated with this Syncher object - null if none */
  util::tThread* thread;

  /*! Uid of associated thread; 0 if none */
  int thread_uid;

public:

  // for static synchronization in this class' methods
  static util::tMutex static_class_mutex;

  // for monitor functionality
  mutable util::tMonitor monitor;

  /*! Network writer threads need to be notified afterwards */
  util::tMutexLockOrder obj_mutex;

  //  /** Optimization for method calls handled directly by the same thread - may only be accessed and written by one thread */
  //  boolean beforeQuickReturnCheck = false;

  /*! Return values for synchronous method calls are placed here */
  tAbstractCall* volatile method_return;

  //  /** Is thread currently waiting for return value? (only used in synchronized context - so may not be volatile) */
  //  boolean threadWaitingForReturn = false;

  /*! Incremented with each synchronized method call (after return value is received - or method call failed) */
  int16 current_method_call_index;

private:

  /*!
   * Reset object - for use with another thread
   */
  void Reset();

public:

  tMethodCallSyncher() :
      index(0),
      thread(NULL),
      thread_uid(0),
      obj_mutex(tLockOrderLevels::cINNER_MOST - 300),
      method_return(NULL),
      current_method_call_index(0)
  {}

  /*!
   * \param syncher_iD Index of syncher objects
   * \return Syncher object
   */
  inline static tMethodCallSyncher* Get(int syncher_iD)
  {
    return &(slots[syncher_iD]);
  }

  /*!
   * Get and use the next method call index.
   * (only call in synchronized context)
   *
   * \return The new current method call index
   */
  int16 GetAndUseNextCallIndex();

  /*!
   * (Only called by ThreadLocalCache and thread to use object with)
   *
   * \return currently unused instance of Method Call synchronization object
   */
  static tMethodCallSyncher* GetFreeInstance(tThreadLocalCache* tc);

  /*!
   * \return Index in array
   */
  inline size_t GetIndex()
  {
    return index;
  }

  /*!
   * \return Uid of associated thread; 0 if none
   */
  inline int GetThreadUid()
  {
    return thread_uid;
  }

  /*!
   * (Only called by ThreadLocalCache when thread has terminated)
   *
   * Return method syncher to pool of unused ones
   */
  inline void Release()
  {
    util::tLock lock2(this);
    Reset();
  }

  //  /**
  //   * Pause thread until return value arrives
  //   *
  //   * \param timeout time to wait for return value
  //   * @throws InterruptedException thrown as soon as return value arrived
  //   */
  //  public void waitForReturnValue(long timeout) throws InterruptedException {
  //      //System.out.println("Thread " + Thread.currentThread().toString() + " waiting for return value");
  //      Thread.sleep(timeout);
  //      synchronized(this) {
  //          threadWaitingForReturn = false;
  //      }
  //  }

  /*!
   * Return value arrives
   *
   * \param mc MethodCall buffer containing return value
   */
  void ReturnValue(tAbstractCall* mc);

  static void StaticInit();

};

} // namespace finroc
} // namespace core

#endif // CORE__PORT__RPC__TMETHODCALLSYNCHER_H
