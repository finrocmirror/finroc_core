
#include "core/thread/tThreadContainerThread.h"
#include "core/thread/tThreadContainer.h"
#include "core/thread/tPeriodicFrameworkElementTask.h"
#include "rrlib/finroc_core_utils/thread/tLoopThread.h"
#include "rrlib/finroc_core_utils/thread/tTask.h"
#include "core/tRuntimeEnvironment.h"
#include "core/port/tAbstractPort.h"
#include "core/port/tAggregatedEdge.h"
#include "core/tFinrocAnnotation.h"
#include "core/thread/tExecutionControl.h"

namespace finroc
{
namespace core
{
tThreadContainerThread::tThreadContainerThread(tFrameworkElement* thread_container_, int64 default_cycle_time, bool warn_on_cycle_time_exceed) :
  tCoreLoopThreadBase(default_cycle_time, warn_on_cycle_time_exceed),
  thread_container(thread_container_),
  reschedule(true),
  schedule(),
  tasks(),
  non_sensor_tasks(),
  trace(),
  trace_back(),
  filter(),
  tmp()
{
  this->SetName(util::tStringBuilder("ThreadContainer ") + thread_container_->GetDescription());
}

void tThreadContainerThread::MainLoopCallback()
{
  if (reschedule)
  {
    reschedule = false;
    {
      util::tLock lock3(this->thread_container->GetRegistryLock());

      // find tasks
      tasks.Clear();
      non_sensor_tasks.Clear();
      schedule.Clear();

      filter.TraverseElementTree(this->thread_container, this, false, tmp);
      tasks.AddAll(non_sensor_tasks);

      // create task graph
      for (size_t i = 0u; i < tasks.Size(); i++)
      {
        tPeriodicFrameworkElementTask* task = tasks.Get(i);

        // trace outgoing connections
        TraceOutgoing(task, task->outgoing);
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
            FINROC_LOG_PRINT_TO(thread_containers, rrlib::logging::eLL_WARNING, "Choosing ", current->incoming->GetQualifiedName(), " as next element");
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
  for (size_t i = 0u; i < schedule.Size(); i++)
  {
    schedule.Get(i)->task->ExecuteTask();
  }
}

void tThreadContainerThread::Run()
{
  this->thread_container->GetRuntime()->AddListener(this);
  ::finroc::core::tCoreLoopThreadBase::Run();
}

void tThreadContainerThread::RuntimeChange(int8 change_type, tFrameworkElement* element)
{
  if (element->IsChildOf(this->thread_container, true))
  {
    reschedule = true;
  }
}

void tThreadContainerThread::RuntimeEdgeChange(int8 change_type, tAbstractPort* source, tAbstractPort* target)
{
  if (source->IsChildOf(this->thread_container) && target->IsChildOf(this->thread_container))
  {
    reschedule = true;
  }
}

void tThreadContainerThread::StopThread()
{
  {
    util::tLock lock2(this->thread_container->GetRegistryLock());
    this->thread_container->GetRuntime()->RemoveListener(this);
    ::finroc::util::tLoopThread::StopThread();
  }
}

void tThreadContainerThread::TraceOutgoing(tPeriodicFrameworkElementTask* task, tEdgeAggregator* outgoing)
{
  // add to trace stack
  trace.Add(outgoing);

  util::tArrayWrapper<tAggregatedEdge*>* out_edges = outgoing->GetEmergingEdges();
  for (size_t i = 0u; i < out_edges->Size(); i++)
  {
    tEdgeAggregator* dest = out_edges->Get(i)->destination;
    if (!trace.Contains(dest))
    {
      // ok, have we reached another task?
      tFinrocAnnotation* ann = dest->GetAnnotation(tPeriodicFrameworkElementTask::cTYPE);
      if (ann == NULL && IsInterface(dest))
      {
        ann = dest->GetParent()->GetAnnotation(tPeriodicFrameworkElementTask::cTYPE);
      }
      if (ann != NULL)
      {
        tPeriodicFrameworkElementTask* task2 = static_cast<tPeriodicFrameworkElementTask*>(ann);
        if (!task->next_tasks.Contains(task2))
        {
          task->next_tasks.Add(task2);
          task2->previous_tasks.Add(task);
        }
        continue;
      }

      // continue from this edge aggregator
      if (dest->GetEmergingEdges()->Size() > 0)
      {
        TraceOutgoing(task, dest);
      }
      else if (IsInterface(dest))
      {
        tFrameworkElement* parent = dest->GetParent();
        if (parent->GetFlag(tCoreFlags::cEDGE_AGGREGATOR))
        {
          tEdgeAggregator* ea = static_cast<tEdgeAggregator*>(parent);
          if (!trace.Contains(ea))
          {
            TraceOutgoing(task, ea);
          }
        }
        tFrameworkElement::tChildIterator ci(parent, tCoreFlags::cREADY | tCoreFlags::cEDGE_AGGREGATOR | tEdgeAggregator::cIS_INTERFACE);
        tFrameworkElement* other_if = NULL;
        while ((other_if = ci.Next()) != NULL)
        {
          tEdgeAggregator* ea = static_cast<tEdgeAggregator*>(other_if);
          if (!trace.Contains(ea))
          {
            TraceOutgoing(task, ea);
          }
        }
      }
    }
  }

  // remove from trace stack
  assert((trace.Get(trace.Size() - 1) == outgoing));
  trace.Remove(trace.Size() - 1);
}

void tThreadContainerThread::TreeFilterCallback(tFrameworkElement* fe, bool unused)
{
  if (tExecutionControl::Find(fe)->GetAnnotated() != thread_container)    // don't handle elements in nested thread containers
  {
    return;
  }
  tFinrocAnnotation* ann = fe->GetAnnotation(tPeriodicFrameworkElementTask::cTYPE);
  if (ann != NULL)
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
}

} // namespace finroc
} // namespace core

