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
#include "core/port/rpc/tMethodCallSyncher.h"
#include "core/port/tThreadLocalCache.h"
#include "core/port/rpc/tAbstractCall.h"

namespace finroc
{
namespace core
{
util::tMutex tMethodCallSyncher::static_class_mutex;
const size_t tMethodCallSyncher::cMAX_THREADS;
::finroc::util::tArrayWrapper<tMethodCallSyncher> tMethodCallSyncher::slots(tMethodCallSyncher::cMAX_THREADS);

int16 tMethodCallSyncher::GetAndUseNextCallIndex()
{
  current_method_call_index++;
  if (current_method_call_index == util::tShort::cMAX_VALUE)
  {
    current_method_call_index = 0;
  }
  return current_method_call_index;
}

tMethodCallSyncher* tMethodCallSyncher::GetFreeInstance(tThreadLocalCache* tc)
{
  util::tLock lock1(static_class_mutex);
  for (size_t i = 0u; i < slots.length; i++)
  {
    if (slots[i].thread_uid == 0)
    {
      tMethodCallSyncher* mcs = &(slots[i]);
      mcs->Reset();
      mcs->thread = util::tThread::CurrentThread().get();
      mcs->thread_uid = tc->GetThreadUid();
      return mcs;
    }
  }
  throw util::tRuntimeException("Number of threads maxed out", CODE_LOCATION_MACRO);
}

void tMethodCallSyncher::Reset()
{
  //      beforeQuickReturnCheck = false;
  thread_uid = 0;
  thread = NULL;
  method_return = NULL;
  current_method_call_index = 0;
}

void tMethodCallSyncher::ReturnValue(tAbstractCall* mc)
{
  util::tLock lock1(this);

  if (GetThreadUid() != mc->GetThreadUid())
  {
    mc->GenericRecycle();
    return;  // waiting thread has already ended
  }
  if (current_method_call_index != mc->GetMethodCallIndex())
  {
    mc->GenericRecycle();
    return;  // outdated method result - timeout could have elapsed
  }

  method_return = mc;
  assert(((thread != NULL)) && "No thread to notify ??");
  monitor.NotifyAll(lock1);
}

void tMethodCallSyncher::StaticInit()
{
  for (size_t i = 0u; i < slots.length; i++)
  {
    slots[i].index = i;
  }
}

} // namespace finroc
} // namespace core

