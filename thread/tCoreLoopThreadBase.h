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

#ifndef CORE__THREAD__TCORELOOPTHREADBASE_H
#define CORE__THREAD__TCORELOOPTHREADBASE_H

#include "core/port/tThreadLocalCache.h"
#include "finroc_core_utils/thread/tLoopThread.h"

namespace finroc
{
namespace core
{
/*!
 * \author Max Reichardt
 *
 * Base class for worker threads managed by the runtime environment
 */
class tCoreLoopThreadBase : public util::tLoopThread
{
protected:

  /*! Thread local info */
  tThreadLocalCache* tc;

  /*!
   * Initialize reference to ThreadLocalCache
   * (Needs to be called at start of thread)
   */
  void InitThreadLocalCache();

public:

  tCoreLoopThreadBase(int64 default_cycle_time, bool warn_on_cycle_time_exceed, bool pause_on_startup);

  tCoreLoopThreadBase(int64 default_cycle_time, bool warn_on_cycle_time_exceed);

  tCoreLoopThreadBase(int64 default_cycle_time);

  virtual void Run()
  {
    InitThreadLocalCache();
    ::finroc::util::tLoopThread::Run();
  }

//  /** currently acquired locks - should be thread safe since only modified by this thread */
//  private final FastSet<Lockable> currentLocks = new FastSet<Lockable>();
//
//  /** Index of Thread */
//  private final int index;
//
//  /** Does thread update critical iteration itself? */
//  protected final boolean updatesIterationItself;
//
//  /** increases monotonically */
//  protected volatile int hasNoValuesBeforeIteration;
//
//  public CoreThread(int index, boolean updatesIterationItself, boolean startThread) {
//      super(RuntimeSettings.DEFAULT_CYCLE_TIME, RuntimeSettings.WARN_ON_CYCLE_TIME_EXCEED, true);
//      this.index = index;
//      this.updatesIterationItself = updatesIterationItself;
//      setName(getClass().getSimpleName() + " " + index);
//      hasNoValuesBeforeIteration = BufferTracker.getInstance().getCurIteration();
//      BufferTracker.getInstance().registerThread(this);
//
//      if (startThread) {
//          start();
//      }
//  }
//
//  @Override
//  public void run() {
//      super.run();
//      cleanShutdown();
//  }
//
//  public void cleanShutdown() {
//      BufferTracker.getInstance().unregisterThread(this);
//  }
//
//
//  @Override
//  public int locksNoValuesBeforeIteration(int curIteration) {
//      if (isWaiting()) {
//          hasNoValuesBeforeIteration = curIteration;
//          return curIteration;
//      }
//      if (updatesIterationItself) {
//          return hasNoValuesBeforeIteration;
//      } else {
//          synchronized(this) {
//              return curIteration;
//          }
//      }
//  }
//
//  /** release all locks */
//  public void releaseLocks() {
//      if (updatesIterationItself) {
//          int iteration = BufferTracker.getInstance().getCurIteration();
//          if (iteration > hasNoValuesBeforeIteration) {
//              hasNoValuesBeforeIteration = iteration;
//          }
//      }
//      if (currentLocks.size() == 0) {
//          return;
//      }
//      for (FastSet.Record r = currentLocks.head(), end = currentLocks.tail(); (r = r.getNext()) != end;) {
//          currentLocks.valueOf(r).release(this);
//      }
//      currentLocks.clear();
//  }
//
//  /**
//   * Add lock
//   *
//   * \param l Object that is locked by this thread
//   */
//  public void addLock(Lockable l) {
//      currentLocks.add(l);
//  }
//
//  /**
//   * \return Thread Index
//   */
//  public int getIndex() {
//      return index;
//  }
//
//  public boolean updatesIterationItself() {
//      return updatesIterationItself;
//  }

};

} // namespace finroc
} // namespace core

#endif // CORE__THREAD__TCORELOOPTHREADBASE_H
