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
  filter(),
  tmp(),
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
    FINROC_LOG_PRINT(rrlib::logging::eLL_ERROR, "Got stuck without executing any task!? This should not happen.");
  }
  else
  {
    FINROC_LOG_PRINT(rrlib::logging::eLL_ERROR, "Got stuck executing task associated with '", task->incoming[0]->GetQualifiedName(), "'. Please check your code for infinite loops etc.!");
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
      tasks.Clear();
      non_sensor_tasks.Clear();
      schedule.Clear();

      filter.TraverseElementTree(this->thread_container, tmp, [&](tFrameworkElement & fe)
      {
        if (tExecutionControl::Find(fe)->GetAnnotated() != &thread_container)    // don't handle elements in nested thread containers
        {
          return;
        }
        tFinrocAnnotation* ann = fe.GetAnnotation(tPeriodicFrameworkElementTask::cTYPE);
        if (ann)
        {
          tPeriodicFrameworkElementTask* task = static_cast<tPeriodicFrameworkElementTask*>(ann);
          task->previous_tasks.Clear();
          task->next_tasks.Clear();
          if (task->IsSenseTask())
          {
            tasks.Add(task);
          }
          else
          {
            non_sensor_tasks.Add(task);
          }
        }
      });

      tasks.AddAll(non_sensor_tasks);

      // create task graph
      for (size_t i = 0u; i < tasks.Size(); i++)
      {
        tPeriodicFrameworkElementTask* task = tasks.Get(i);

        // trace outgoing connections
        for (auto it = task->outgoing.begin(); it < task->outgoing.end(); it++)
        {
          TraceOutgoing(*task, **it);
        }
      }

      // now create schedule
      while (tasks.Size() > 0)
      {
        // do we have task without previous tasks?
        bool found = false;
        for (size_t i = 0u; i < tasks.Size(); i++)
        {
          tPeriodicFrameworkElementTask* task = tasks.Get(i);
          if (task->previous_tasks.Size() == 0)
          {
            schedule.Add(task);
            tasks.RemoveElem(task);
            found = true;

            // delete from next tasks' previous task list
            for (size_t j = 0u; j < task->next_tasks.Size(); j++)
            {
              tPeriodicFrameworkElementTask* next = task->next_tasks.Get(j);
              next->previous_tasks.RemoveElem(task);
            }
            break;
          }
        }
        if (found)
        {
          continue;
        }

        // ok, we didn't find module to continue with... (loop)
        FINROC_LOG_PRINT_TO(thread_containers, rrlib::logging::eLL_WARNING, "Detected loop: doing traceback");
        trace_back.Clear();
        tPeriodicFrameworkElementTask* current = tasks.Get(0);
        trace_back.Add(current);
        while (true)
        {
          bool end = true;
          for (size_t i = 0u; i < current->previous_tasks.Size(); i++)
          {
            tPeriodicFrameworkElementTask* prev = current->previous_tasks.Get(i);
            if (!trace_back.Contains(prev))
            {
              end = false;
              current = prev;
              trace_back.Add(current);
              break;
            }
          }
          if (end)
          {
            FINROC_LOG_PRINT_TO(thread_containers, rrlib::logging::eLL_WARNING, "Choosing ", current->incoming[0]->GetQualifiedName(), " as next element");
            schedule.Add(current);
            tasks.RemoveElem(current);

            // delete from next tasks' previous task list
            for (size_t j = 0u; j < current->next_tasks.Size(); j++)
            {
              tPeriodicFrameworkElementTask* next = current->next_tasks.Get(j);
              next->previous_tasks.RemoveElem(current);
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

  for (size_t i = 0u; i < schedule.Size(); i++)
  {
    current_task = schedule.Get(i);
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
  trace.Add(&outgoing);

  util::tArrayWrapper<tAggregatedEdge*>* out_edges = outgoing.GetEmergingEdges();
  assert(out_edges != NULL);
  for (size_t i = 0u; i < out_edges->Size(); i++)
  {
    tAggregatedEdge* temp = out_edges->Get(i);
    if (!temp)
    {
      FINROC_LOG_PRINT_TO(thread_containers, rrlib::logging::eLL_WARNING, "Out Edge ", i , " is NULL ... skipping.\n");
      continue;
    }
    assert(temp);

    tEdgeAggregator* dest = &temp->destination;

    assert(dest);
    if (!trace.Contains(dest))
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
        if (!task.next_tasks.Contains(task2))
        {
          task.next_tasks.Add(task2);
          task2->previous_tasks.Add(&task);
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
          if (!trace.Contains(ea))
          {
            TraceOutgoing(task, *ea);
          }
        }
        tFrameworkElement::tChildIterator ci(*parent, tCoreFlags::cREADY | tCoreFlags::cEDGE_AGGREGATOR | tEdgeAggregator::cIS_INTERFACE);
        tFrameworkElement* other_if = NULL;
        while ((other_if = ci.Next()) != NULL)
        {
          tEdgeAggregator* ea = static_cast<tEdgeAggregator*>(other_if);
          if (!trace.Contains(ea))
          {
            TraceOutgoing(task, *ea);
          }
        }
      }
    }
  }

  // remove from trace stack
  assert(trace.Get(trace.Size() - 1) == &outgoing);
  trace.Remove(trace.Size() - 1);
}

} // namespace finroc
} // namespace core

