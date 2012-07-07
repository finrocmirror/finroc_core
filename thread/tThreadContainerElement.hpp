/**
 * You received this file as part of an advanced experimental
 * robotics framework prototype ('finroc')
 *
 * Copyright (C) 2010 Max Reichardt,
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
#include "core/thread/tThreadContainerElement.h"
#include "core/parameter/tStaticParameterList.h"
#include "core/tAnnotatable.h"
#include "core/thread/tExecutionControl.h"
#include "core/tFrameworkElement.h"

namespace finroc
{
namespace core
{

template <typename BASE>
template <typename ... ARGS>
tThreadContainerElement<BASE>::tThreadContainerElement(const ARGS&... args) :
  BASE(args...),
  rt_thread("Realtime Thread", this, false),
  cycle_time("Cycle Time", this, std::chrono::milliseconds(40), tBounds<rrlib::time::tDuration>(rrlib::time::tDuration::zero(), std::chrono::seconds(60), true)),
  warn_on_cycle_time_exceed("Warn on cycle time exceed", this, true),
  thread(),
  last_cycle_execution_time("Last Cycle execution time", this, tPortFlags::cOUTPUT_PORT)
{
  this->AddAnnotation(new tExecutionControl(*this));
}

template <typename BASE>
tThreadContainerElement<BASE>::~tThreadContainerElement()
{
  if (thread.get())
  {
    StopThread();
    JoinThread();
  }
}

template <typename BASE>
bool tThreadContainerElement<BASE>::IsExecuting()
{
  return thread.get();
}

template <typename BASE>
void tThreadContainerElement<BASE>::JoinThread()
{
  rrlib::thread::tLock l(*this);
  if (thread.get() != NULL)
  {
    thread->Join();
    thread.reset();
  }
}

template <typename BASE>
void tThreadContainerElement<BASE>::StartExecution()
{
  rrlib::thread::tLock l(*this);
  if (thread)
  {
    FINROC_LOG_PRINT(rrlib::logging::eLL_WARNING, "Thread is already executing.");
    return;
  }
  tThreadContainerThread* thread_tmp = new tThreadContainerThread(*this, cycle_time.Get(), warn_on_cycle_time_exceed.Get(), last_cycle_execution_time);
  thread_tmp->SetAutoDelete();
  thread = std::static_pointer_cast<tThreadContainerThread>(thread_tmp->GetSharedPtr());
  if (rt_thread.Get())
  {
    thread_tmp->SetRealtime();
  }
  l.Unlock();
  thread->Start();
}

template <typename BASE>
void tThreadContainerElement<BASE>::StopThread()
{
  rrlib::thread::tLock l(*this);
  if (thread)
  {
    thread->StopThread();
  }
}

} // namespace finroc
} // namespace core

