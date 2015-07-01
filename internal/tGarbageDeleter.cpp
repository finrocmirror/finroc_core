//
// You received this file as part of Finroc
// A framework for intelligent robot control
//
// Copyright (C) Finroc GbR (finroc.org)
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along
// with this program; if not, write to the Free Software Foundation, Inc.,
// 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
//
//----------------------------------------------------------------------
/*!\file    core/internal/tGarbageDeleter.cpp
 *
 * \author  Max Reichardt
 *
 * \date    2012-10-28
 *
 */
//----------------------------------------------------------------------
#include "core/internal/tGarbageDeleter.h"

//----------------------------------------------------------------------
// External includes (system with <>, local with "")
//----------------------------------------------------------------------
#include "rrlib/buffer_pools/tGarbageFromDeletedBufferPools.h"

//----------------------------------------------------------------------
// Internal includes with ""
//----------------------------------------------------------------------
#include "core/tFrameworkElement.h"

//----------------------------------------------------------------------
// Debugging
//----------------------------------------------------------------------
#include <cassert>

//----------------------------------------------------------------------
// Namespace usage
//----------------------------------------------------------------------

//----------------------------------------------------------------------
// Namespace declaration
//----------------------------------------------------------------------
namespace finroc
{
namespace core
{
namespace internal
{

//----------------------------------------------------------------------
// Forward declarations / typedefs / enums
//----------------------------------------------------------------------

#ifndef RRLIB_SINGLE_THREADED

//----------------------------------------------------------------------
// Const values
//----------------------------------------------------------------------
/*! Garbage collector cycle time */
static const rrlib::time::tDuration cCYCLE_TIME(std::chrono::milliseconds(1000));

/*! Interval after which all threads should have executed enough code to not access deleted objects anymore */
static const rrlib::time::tDuration cSAFE_DELETE_INTERVAL(std::chrono::milliseconds(5000));

/*! Number of garbage collector cycles after which deleting is safe */
static const int cSAFE_DELETE_CYCLES = (cSAFE_DELETE_INTERVAL.count() / cCYCLE_TIME.count()) + (cSAFE_DELETE_INTERVAL < cCYCLE_TIME ? 2 : 1);

/*! Constants for below - weird numbers to detect any memory corruption (shouldn't happen I think) */
static const int cYES = 0x37347377, cNO = 0x1946357;

//----------------------------------------------------------------------
// Implementation
//----------------------------------------------------------------------

/*! Singleton instance - non-null while thread is running */
static tGarbageDeleter* instance = NULL;

/*! True after garbage collector has been started */
static std::atomic<int> started(cNO);

/*!
 * Thread that is deleting garbage collector at program shutdown using Thread::StopThreads()
 * We use the pointer to the thread instead of thread id, because it is still valid after thread has been deleted
 */
static std::atomic<rrlib::thread::tThread*> deleter_thread(0);

tGarbageDeleter::tGarbageDeleter() :
  tLoopThread(cCYCLE_TIME, false),
  tasks(),
  next(),
  cycle_count(0),
  regular_tasks()
{
  assert((started == cNO) && "May only create single instance");
  instance = this;
  SetName("Garbage Deleter");
  SetLongevity(0xF0000000);
  SetAutoDelete();
}

tGarbageDeleter::~tGarbageDeleter()
{
  assert(tasks.empty());
  instance = NULL;
  //assert(rt_tasks.Dequeue() == NULL);
}

void tGarbageDeleter::AddRegularTask(tRegularTask task)
{
  tGarbageDeleter* garbage_deleter = instance;
  assert(garbage_deleter);
  rrlib::thread::tLock lock(*garbage_deleter);
  garbage_deleter->regular_tasks.push_back(task);
}

void tGarbageDeleter::CreateAndStartInstance()
{
  if (tThread::StoppingThreads())
  {
    FINROC_LOG_PRINT_STATIC(WARNING, "Starting gc in this phase is not allowed");
    return;
  }

  if (started == cNO)
  {
    tGarbageDeleter* tmp = new tGarbageDeleter();
    tmp->Start();
    instance = tmp;
    started = cYES;
  }
  else
  {
    FINROC_LOG_PRINT_STATIC(WARNING, "Cannot start gc thread twice. This attempt is possibly dangerous, since this method is not thread-safe");
  }
}

void tGarbageDeleter::DeleteDeferredImplementation(void* object_to_delete, void (*deleter_function)(void*))
{
  //FINROC_LOG_PRINT(DEBUG_VERBOSE_1, "Delete requested for: ", object_to_delete->GetQualifiedName());
  tGarbageDeleter* gc = instance;
  if (gc == NULL)
  {
    assert(started == cNO || &rrlib::thread::tThread::CurrentThread() == deleter_thread);
    // safe to delete object now
    deleter_function(object_to_delete);
    return;
  }

  tDeferredDeleteTask t(object_to_delete, deleter_function, gc->cycle_count + cSAFE_DELETE_CYCLES);
  tLock lock(*gc);
  gc->tasks.push(t);
}

/*void tGarbageDeleter::DeleteRT(tQueueable* element_to_delete)
{
  FINROC_LOG_PRINT_TO(garbage_collector, DEBUG_VERBOSE_1, "RT Delete requested for: ", element_to_delete);
  tGarbageDeleter* gc = instance;
  if (gc == NULL)
  {
    assert((started == cNO || rrlib::thread::tThread::CurrentThreadId() == deleter_thread_id));
    // safe to delete object now
    element_to_delete->CustomDelete(true);
    return;
  }

  gc->rt_tasks.Enqueue(element_to_delete);
}*/

void tGarbageDeleter::MainLoopCallback()
{
  int64_t current_cycle = cycle_count;

  // check waiting deletion tasks
  while (true)
  {
    if (!next.object_to_delete)
    {
      tLock lock(*this);
      if (tasks.empty())
      {
        break;
      }
      next = tasks.front();
      tasks.pop();
    }

    if (current_cycle < next.cycle_when)
    {
      break;
    }
    next.Execute();
  }

  rrlib::buffer_pools::tGarbageFromDeletedBufferPools::DeleteGarbage();

  for (auto it = regular_tasks.begin(); it != regular_tasks.end(); ++it)
  {
    (**it)();
  }

  // process waiting deletion tasks from RT thread
  //rt_tasks.DeleteEnqueued();

  cycle_count++;
}

void tGarbageDeleter::Run()
{
  SetPriority(16); // Garbage deleter should only run if other threads do not need CPU

  tLoopThread::Run();

  assert(tThread::StoppingThreads());

  // delete everything - other threads should have been stopped before
  next.Execute();

  while (!tasks.empty())
  {
    tasks.front().Execute();
    tasks.pop();
  }

  for (auto it = regular_tasks.begin(); it != regular_tasks.end(); ++it)
  {
    (**it)();
  }
}

void tGarbageDeleter::StopThread()
{
  assert(tThread::StoppingThreads() && "may only be called by Thread::stopThreads()");
  deleter_thread = &rrlib::thread::tThread::CurrentThread();

  tLoopThread::StopThread();
  try
  {
    Join();
  }
  catch (const std::exception& e)
  {
    FINROC_LOG_PRINT(DEBUG_WARNING, e);
  }

  // possibly some thread-local objects of Garbage Collector thread
  while (!tasks.empty())
  {
    tasks.front().Execute();
    tasks.pop();
  }
  //rt_tasks.DeleteEnqueued();

  instance = NULL;
  rrlib::buffer_pools::tGarbageFromDeletedBufferPools::DeleteGarbage();
}

#endif

//----------------------------------------------------------------------
// End of namespace declaration
//----------------------------------------------------------------------
}
}
}
