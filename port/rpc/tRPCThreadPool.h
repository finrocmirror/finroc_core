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

#ifndef core__port__rpc__tRPCThreadPool_h__
#define core__port__rpc__tRPCThreadPool_h__

#include "rrlib/finroc_core_utils/definitions.h"

#include "core/port/rpc/tRPCThread.h"
#include "rrlib/finroc_core_utils/container/tWonderQueue.h"
#include "core/tLockOrderLevels.h"

namespace finroc
{
namespace util
{
class tTask;
} // namespace finroc
} // namespace util

namespace finroc
{
namespace core
{
/*!
 * \author Max Reichardt
 *
 * Thread pool for remote procedure calls.
 */
class tRPCThreadPool : public boost::noncopyable
{
  /*! Pool of unused threads */
  util::tWonderQueue<tRPCThread> unused_threads;

public:

  /*! Lock order: locked before thread list in C++ */
  util::tMutexLockOrder obj_mutex;

  tRPCThreadPool();

  /*!
   * Enqueue unused Thread for reuse. Threads call this automatically
   * (so normally no need to call from somewhere else)
   *
   * \param container Container to enqueue
   */
  inline void EnqueueThread(tRPCThread* container)
  {
    unused_threads.Enqueue(container);
  }

  /*!
   * Execute task by unused thread in thread pool
   *
   * \param task Task
   */
  void ExecuteTask(util::tTask* task);

  /*!
   * \return Singleton instance
   */
  static tRPCThreadPool& GetInstance();

};

} // namespace finroc
} // namespace core

#endif // core__port__rpc__tRPCThreadPool_h__
