/**
 * You received this file as part of an advanced experimental
 * robotics framework prototype ('finroc')
 *
 * Copyright (C) 2011 Max Reichardt,
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

#include "core/thread/tThreadContainerThread.h"
#include "core/thread/tPeriodicFrameworkElementTask.h"
#include "core/tRuntimeEnvironment.h"
#include "core/port/tAbstractPort.h"
#include "core/port/tAggregatedEdge.h"
#include "core/tFinrocAnnotation.h"
#include "core/thread/tExecutionControl.h"

namespace finroc
{
namespace core
{
tThreadContainerThread::tThreadContainerThread(tFrameworkElement& thread_container, rrlib::time::tDuration default_cycle_time, bool warn_on_cycle_time_exceed, tPort<rrlib::time::tDuration> last_cycle_execution_time) :
  tCoreLoopThreadBase(default_cycle_time, true, warn_on_cycle_time_exceed),
#ifdef NDEBUG  // only activate monitoring in debug mode
  tWatchDogTask(false),
#else
  tWatchDogTask(true),
#endif
  thread_container(thread_container),
  reschedule(true),
  schedule(),
  tasks(),
  non_sensor_tasks(),
  trace(),
  trace_back(),
  last_cycle_execution_time(last_cycle_execution_time),
  current_task(NULL)
{
  this->SetName(std::string("ThreadContainer ") + thread_container.GetCName());
}

void tThreadContainerThread::HandleWatchdogAlert()
{
  tPeriodicFrameworkElementTask* task = current_task;
  if (!task)
  {
    FINROC_LOG_PRINT(ERROR, "Got stuck without executing any task!? This should not happen.");
  }
  else
  {
    FINROC_LOG_PRINT(ERROR, "Got stuck executing task associated with '", task->incoming[0]->GetQualifiedName(), "'. Please check your code for infinite loops etc.!");
  }
  tWatchDogTask::Deactivate();

}

void tThreadContainerThread::MainLoopCallback()
{
  if (reschedule)
  {
    reschedule = false;
    {
      tLock lock3(this->thread_container.GetRegistryLock());

      // find tasks
      tasks.clear();
      non_sensor_tasks.clear();
      schedule.clear();

      for (auto it = thread_container.SubElementsBegin(true); it != thread_container.SubElementsEnd(); ++it)
      {
        if ((!it->IsReady()) || tExecutionControl::Find(*it)->GetAnnotated() != &thread_container)    // don't handle elements in nested thread containers
        {
          continue;
        }
        tFinrocAnnotation* ann = it->GetAnnotation(tPeriodicFrameworkElementTask::cTYPE);
        if (ann)
        {
          tPeriodicFrameworkElementTask* task = static_cast<tPeriodicFrameworkElementTask*>(ann);
          task->previous_tasks.clear();
          task->next_tasks.clear();
          if (task->IsSenseTask())
          {
            tasks.push_back(task);
          }
          else
          {
            non_sensor_tasks.push_back(task);
          }
        }
      }

      tasks.insert(tasks.end(), non_sensor_tasks.begin(), non_sensor_tasks.end());

      // create task graph
      for (auto task = tasks.begin(); task != tasks.end(); ++task)
      {
        // trace outgoing connections
        for (auto it = (*task)->outgoing.begin(); it < (*task)->outgoing.end(); ++it)
        {
          TraceOutgoing(**task, **it);
        }
      }

      // now create schedule
      while (tasks.size() > 0)
      {
        // do we have task without previous tasks?
        bool found = false;
        for (auto it = tasks.begin(); it != tasks.end(); ++it)
        {
          tPeriodicFrameworkElementTask* task = *it;
          if (task->previous_tasks.size() == 0)
          {
            schedule.push_back(task);
            tasks.erase(std::remove(tasks.begin(), tasks.end(), task), tasks.end());
            found = true;

            // delete from next tasks' previous task list
            for (auto next = task->next_tasks.begin(); next != task->next_tasks.end(); ++next)
            {
              (*next)->previous_tasks.erase(std::remove((*next)->previous_tasks.begin(), (*next)->previous_tasks.end(), task), (*next)->previous_tasks.end());
            }
            break;
          }
        }
        if (found)
        {
          continue;
        }

        // ok, we didn't find module to continue with... (loop)
        FINROC_LOG_PRINT_TO(thread_containers, WARNING, "Detected loop: doing traceback");
        trace_back.clear();
        tPeriodicFrameworkElementTask* current = tasks[0];
        trace_back.push_back(current);
        while (true)
        {
          bool end = true;
          for (size_t i = 0u; i < current->previous_tasks.size(); i++)
          {
            tPeriodicFrameworkElementTask* prev = current->previous_tasks[i];
            if (std::find(trace_back.begin(), trace_back.end(), prev) == trace_back.end())
            {
              end = false;
              current = prev;
              trace_back.push_back(current);
              break;
            }
          }
          if (end)
          {
            FINROC_LOG_PRINT_TO(thread_containers, WARNING, "Choosing ", current->incoming[0]->GetQualifiedName(), " as next element");
            schedule.push_back(current);
            tasks.erase(std::remove(tasks.begin(), tasks.end(), current), tasks.end());

            // delete from next tasks' previous task list
            for (auto next = current->next_tasks.begin(); next != current->next_tasks.end(); ++next)
            {
              (*next)->previous_tasks.erase(std::remove((*next)->previous_tasks.begin(), (*next)->previous_tasks.end(), current), (*next)->previous_tasks.end());
            }
            break;
          }
        }
      }
    }
  }

  // execute tasks
  last_cycle_execution_time.Publish(GetLastCycleTime());

#ifndef NDEBUG
  SetDeadLine(rrlib::time::Now() + GetCycleTime() * 4 + std::chrono::seconds(1));
#endif

  for (size_t i = 0u; i < schedule.size(); i++)
  {
    current_task = schedule[i];
    current_task->task.ExecuteTask();
  }

#ifndef NDEBUG
  tWatchDogTask::Deactivate();
#endif
}

void tThreadContainerThread::Run()
{
  this->thread_container.GetRuntime().AddListener(*this);
  core::tCoreLoopThreadBase::Run();
}

void tThreadContainerThread::RuntimeChange(int8 change_type, tFrameworkElement& element)
{
  if (element.IsChildOf(this->thread_container, true))
  {
    reschedule = true;
  }
}

void tThreadContainerThread::RuntimeEdgeChange(int8 change_type, tAbstractPort& source, tAbstractPort& target)
{
  if (source.IsChildOf(this->thread_container) && target.IsChildOf(this->thread_container))
  {
    reschedule = true;
  }
}

void tThreadContainerThread::StopThread()
{
  tLock lock2(this->thread_container.GetRegistryLock());
  this->thread_container.GetRuntime().RemoveListener(*this);
  tLoopThread::StopThread();
}

void tThreadContainerThread::TraceOutgoing(tPeriodicFrameworkElementTask& task, tEdgeAggregator& outgoing)
{
  // add to trace stack
  trace.push_back(&outgoing);

  util::tArrayWrapper<tAggregatedEdge*>* out_edges = outgoing.GetEmergingEdges();
  assert(out_edges != NULL);
  for (size_t i = 0u; i < out_edges->Size(); i++)
  {
    tAggregatedEdge* temp = out_edges->Get(i);
    if (!temp)
    {
      FINROC_LOG_PRINT_TO(thread_containers, WARNING, "Out Edge ", i , " is NULL ... skipping.\n");
      continue;
    }
    assert(temp);

    tEdgeAggregator* dest = &temp->destination;

    assert(dest);
    if (std::find(trace.begin(), trace.end(), dest) == trace.end())
    {
      // ok, have we reached another task?
      tFinrocAnnotation* ann = dest->GetAnnotation(tPeriodicFrameworkElementTask::cTYPE);
      if (ann == NULL && IsInterface(*dest))
      {
        ann = dest->GetParent()->GetAnnotation(tPeriodicFrameworkElementTask::cTYPE);
      }
      if (ann)
      {
        tPeriodicFrameworkElementTask* task2 = static_cast<tPeriodicFrameworkElementTask*>(ann);
        if (std::find(task.next_tasks.begin(), task.next_tasks.end(), task2) == task.next_tasks.end())
        {
          task.next_tasks.push_back(task2);
          task2->previous_tasks.push_back(&task);
        }
        continue;
      }

      // continue from this edge aggregator
      if (dest->GetEmergingEdges()->Size() > 0)
      {
        TraceOutgoing(task, *dest);
      }
      else if (IsInterface(*dest)) // TODO: check whether this breaks sense-control-groups
      {
        tFrameworkElement* parent = dest->GetParent();
        if (parent->GetFlag(tCoreFlags::cEDGE_AGGREGATOR))
        {
          tEdgeAggregator* ea = static_cast<tEdgeAggregator*>(parent);
          if (std::find(trace.begin(), trace.end(), ea) == trace.end())
          {
            TraceOutgoing(task, *ea);
          }
        }
        for (auto it = parent->ChildrenBegin(); it != parent->ChildrenEnd(); ++it)
        {
          if (it->GetFlag(tCoreFlags::cREADY) && it->GetFlag(tCoreFlags::cEDGE_AGGREGATOR) && it->GetFlag(tEdgeAggregator::cIS_INTERFACE))
          {
            tEdgeAggregator& ea = static_cast<tEdgeAggregator&>(*it);
            if (std::find(trace.begin(), trace.end(), &ea) == trace.end())
            {
              TraceOutgoing(task, ea);
            }
          }
        }
      }
    }
  }

  // remove from trace stack
  assert(trace[trace.size() - 1] == &outgoing);
  trace.pop_back();
}

} // namespace finroc
} // namespace core

