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
/*!\file    core/internal/tGarbageDeleter.h
 *
 * \author  Max Reichardt
 *
 * \date    2012-10-28
 *
 * \brief   Contains tGarbageDeleter
 *
 * \b tGarbageDeleter
 *
 * This class/thread deletes objects passed to it - deferred,
 *
 */
//----------------------------------------------------------------------
#ifndef __core__internal__tGarbageDeleter_h__
#define __core__internal__tGarbageDeleter_h__

//----------------------------------------------------------------------
// External includes (system with <>, local with "")
//----------------------------------------------------------------------
#include "rrlib/thread/tLoopThread.h"
#include <queue>

//----------------------------------------------------------------------
// Internal includes with ""
//----------------------------------------------------------------------

//----------------------------------------------------------------------
// Namespace declaration
//----------------------------------------------------------------------
namespace finroc
{
namespace core
{

//----------------------------------------------------------------------
// Forward declarations / typedefs / enums
//----------------------------------------------------------------------
class tFrameworkElement;

namespace internal
{

//----------------------------------------------------------------------
// Class declaration
//----------------------------------------------------------------------
//! Garbage deleter
/*!
 * This class/thread takes care of deletes objects passed to it
 * as soon as it is safe - currently after a certain safety period.
 * TODO: after all potential users (threads) signaled that it is safe.
 *
 * Passing objects to this class is blocking when calling DeleteDeferred().
 * When real-time threads need to delete objects without breaking real-time,
 * they can call DeleteRT(). Only tQueueable-derived objects are supported,
 * since we need a concurrent, real-time queue with unlimited entries for this.
 *
 * The thread won't have anything to do in "normal" operation of the system...
 * only when objects need to be deleted "safely" during concurrent operation.
 *
 * Typically, framework elements are prepared for removal and enqueued
 * in the garbage collector's deletion task list.
 * After a certain period of time (when no other thread accesses it
 * anymore), it is completely deleted by this thread.
 *
 * Thread may only be stopped by Thread::stopThreads()
 */
class tGarbageDeleter : public rrlib::thread::tLoopThread
{

//----------------------------------------------------------------------
// Public methods and typedefs
//----------------------------------------------------------------------
public:

#ifndef RRLIB_SINGLE_THREADED

  typedef void (*tRegularTask)();

  /*!
   * Adds regular task for garbage deleter
   * (will be called in every cycle of garbage deleter)
   */
  static void AddRegularTask(tRegularTask task);

  /*!
   * Creates and starts single instance of GarbageCollector thread
   */
  static void CreateAndStartInstance();

#endif

  /*!
   * Delete object deferred
   * (call blocks to this method blocks => not suitable for RT code)
   *
   * \param object_to_delete Framework element to delete (after safety period)
   */
  template <typename T>
  static void DeleteDeferred(T* object_to_delete)
  {
#ifndef RRLIB_SINGLE_THREADED
    if (object_to_delete)
    {
      DeleteDeferredImplementation(object_to_delete, &DeleterFunction<T>);
    }
#else
    delete object_to_delete;
#endif
  }

  static const char* GetLogDescription()
  {
    return "Garbage Deleter";
  }

//----------------------------------------------------------------------
// Private fields and methods
//----------------------------------------------------------------------
private:

#ifndef RRLIB_SINGLE_THREADED

  /*!
   * Garbage Collector task
   */
  class tDeferredDeleteTask
  {
    friend class tGarbageDeleter;

    /*! Object to delete */
    void* object_to_delete;

    /*! Pointer to delete function */
    void (*deleter_function)(void*);

    /*! Cycle in which to delete element */
    int64_t cycle_when;

    tDeferredDeleteTask(void* object_to_delete, void (*deleter_function)(void*), int64_t cycle_when) :
      object_to_delete(object_to_delete),
      deleter_function(deleter_function),
      cycle_when(cycle_when)
    {}

    tDeferredDeleteTask() :
      object_to_delete(),
      deleter_function(NULL),
      cycle_when(0)
    {}

    void Execute()
    {
      if (deleter_function && object_to_delete)
      {
        deleter_function(object_to_delete);
        object_to_delete = NULL;
      }
    }
  };

  /*! Current tasks of Garbage Collector */
  std::queue<tDeferredDeleteTask> tasks;

  /*! Current tasks of Garbage Collector from real-time threads */
  // rt_tasks;

  /*! Next delete task - never null */
  tDeferredDeleteTask next;

  /*! Number of cycles garbage collector has been executed */
  std::atomic<int64_t> cycle_count;

  /*! Regular tasks */
  std::vector<tRegularTask> regular_tasks;


  tGarbageDeleter();

  virtual ~tGarbageDeleter();

  static void DeleteDeferredImplementation(void* object_to_delete, void (*deleter_function)(void*));

  template <typename T>
  static void DeleterFunction(void* pointer)
  {
    delete static_cast<T*>(pointer);
  }

  virtual void MainLoopCallback() override;

  virtual void Run() override;

  virtual void StopThread() override;
#endif
};

//----------------------------------------------------------------------
// End of namespace declaration
//----------------------------------------------------------------------
}
}
}


#endif
