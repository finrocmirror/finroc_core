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
#include "rrlib/finroc_core_utils/thread/sThreadUtil.h"
#include "rrlib/util/patterns/singleton.h"
#include "core/port/rpc/tRPCThreadPool.h"

namespace finroc
{
namespace core
{
typedef rrlib::util::tSingletonHolder<tRPCThreadPool, rrlib::util::singleton::Longevity> tRPCThreadPoolInstance;
static inline unsigned int GetLongevity(tRPCThreadPool*)
{
  return 10; // delete after runtime environment
}

tRPCThreadPool::tRPCThreadPool() :
  unused_threads(),
  obj_mutex(tLockOrderLevels::cINNER_MOST - 100)
{
}

tRPCThread& tRPCThreadPool::GetUnusedThread()
{
  util::tLock lock1(obj_mutex);
  tRPCThread* r = NULL;
  r = unused_threads.Dequeue();

  if (!r)
  {
    r = new tRPCThread();
    util::sThreadUtil::SetAutoDelete(*r);
    r->Start();
#if (__GNUC__ < 4 || (__GNUC__ == 4 && __GNUC_MINOR__ <= 5))
    util::tThread::Sleep(std::chrono::milliseconds(250), false);
#endif
  }
  return *r;
}

tRPCThreadPool& tRPCThreadPool::GetInstance()
{
  return tRPCThreadPoolInstance::Instance();
}


} // namespace finroc
} // namespace core

