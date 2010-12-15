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
#include "core/thread/tThreadContainer.h"
#include "core/parameter/tStructureParameterList.h"
#include "core/tAnnotatable.h"
#include "core/thread/tExecutionControl.h"
#include "core/tFrameworkElement.h"
#include "rrlib/finroc_core_utils/thread/sThreadUtil.h"

namespace finroc
{
namespace core
{
tStandardCreateModuleAction<tThreadContainer> tThreadContainer::cCREATE_ACTION("ThreadContainer", util::tTypedClass<tThreadContainer>());

tThreadContainer::tThreadContainer(tFrameworkElement* parent, const util::tString& description) :
    tGroup(parent, description),
    rt_thread(new tStructureParameterBool("Realtime Thread", false)),
    cycle_time(new tStructureParameterNumeric<int>("Cycle Time", 40, tBounds(1, 60000, true))),
    warn_on_cycle_time_exceed(new tStructureParameterBool("Warn on cycle time exceed", true)),
    thread()
{
  tStructureParameterList::GetOrCreate(this)->Add(rt_thread);
  AddAnnotation(new tExecutionControl(*this));
}

tThreadContainer::~tThreadContainer()
{
  if (thread != NULL)
  {
    StopThread();
    JoinThread();
  }
  ;
}

bool tThreadContainer::IsExecuting()
{
  ::std::tr1::shared_ptr<tThreadContainerThread> t = thread;
  if (t != NULL)
  {
    return t->IsRunning();
  }
  return false;
}

void tThreadContainer::JoinThread()
{
  if (thread != NULL)
  {
    try
    {
      thread->Join();
    }
    catch (const util::tInterruptedException& e)
    {
      FINROC_LOG_STREAM(rrlib::logging::eLL_WARNING, log_domain, "Interrupted ?!!");
    }
    thread.reset();
  }
}

void tThreadContainer::StartExecution()
{
  assert((thread == NULL));
  thread = util::sThreadUtil::GetThreadSharedPtr(new tThreadContainerThread(this, cycle_time->Get(), warn_on_cycle_time_exceed->Get()));
  if (rt_thread->Get())
  {
    util::sThreadUtil::MakeThreadRealtime(thread);
  }
  thread->Start();
}

void tThreadContainer::StopThread()
{
  if (thread != NULL)
  {
    thread->StopThread();
  }
}

} // namespace finroc
} // namespace core

