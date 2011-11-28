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
#include "rrlib/finroc_core_utils/thread/sThreadUtil.h"

namespace finroc
{
namespace core
{

template <typename BASE>
tThreadContainerElement<BASE>::tThreadContainerElement(tFrameworkElement* parent, const util::tString& description, uint flags) :
    BASE(parent, description, flags),
    rt_thread("Realtime Thread", this, false),
    cycle_time("Cycle Time", this, 40, tBounds<int>(1, 60000, true)),
    warn_on_cycle_time_exceed("Warn on cycle time exceed", this, true),
    thread()
{
  this->AddAnnotation(new tExecutionControl(*this));
}

template <typename BASE>
tThreadContainerElement<BASE>::~tThreadContainerElement()
{
  if (thread.get() != NULL)
  {
    StopThread();
    JoinThread();
  }
}

template <typename BASE>
bool tThreadContainerElement<BASE>::IsExecuting()
{
  std::shared_ptr<tThreadContainerThread> t = thread;
  if (t.get() != NULL)
  {
    return t->IsRunning();
  }
  return false;
}

template <typename BASE>
void tThreadContainerElement<BASE>::JoinThread()
{
  if (thread.get() != NULL)
  {
    try
    {
      thread->Join();
    }
    catch (const util::tInterruptedException& e)
    {
      FINROC_LOG_PRINT(rrlib::logging::eLL_WARNING, log_domain, "Interrupted ?!!");
    }
    thread.reset();
  }
}

template <typename BASE>
void tThreadContainerElement<BASE>::StartExecution()
{
  assert((thread.get() == NULL));
  thread = util::sThreadUtil::GetThreadSharedPtr(new tThreadContainerThread(this, cycle_time.Get(), warn_on_cycle_time_exceed.Get()));
  if (rt_thread.Get())
  {
    util::sThreadUtil::MakeThreadRealtime(thread);
  }
  thread->Start();
}

template <typename BASE>
void tThreadContainerElement<BASE>::StopThread()
{
  if (thread.get() != NULL)
  {
    thread->StopThread();
  }
}

} // namespace finroc
} // namespace core

