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
#include "rrlib/design_patterns/singleton.h"
#include "core/port/rpc/tRPCThreadPool.h"

namespace finroc
{
namespace core
{
typedef rrlib::design_patterns::tSingletonHolder<tRPCThreadPool, rrlib::design_patterns::singleton::Longevity> tRPCThreadPoolInstance;
static inline unsigned int GetLongevity(tRPCThreadPool*)
{
  return 10; // delete after runtime environment
}

tRPCThreadPool::tRPCThreadPool() :
  unused_threads(),
  obj_mutex("RPC Thread Pool", tLockOrderLevels::cINNER_MOST - 100)
{
}

tRPCThread& tRPCThreadPool::GetUnusedThread()
{
  rrlib::thread::tLock lock1(obj_mutex);
  tRPCThread* r = NULL;
  r = unused_threads.Dequeue();

  if (!r)
  {
    r = new tRPCThread();
    r->SetAutoDelete();
    r->Start();
#if (__GNUC__ < 4 || (__GNUC__ == 4 && __GNUC_MINOR__ <= 5)) // TODO: Is this still required?
    rrlib::thread::tThread::Sleep(std::chrono::milliseconds(70), false);
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

