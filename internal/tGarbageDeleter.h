//
// You received this file as part of Finroc
// A Framework for intelligent robot control
//
// Copyright (C) Finroc GbR (finroc.org)
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
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

  /*!
   * Creates and starts single instance of GarbageCollector thread
   */
  static void CreateAndStartInstance();

  /*!
   * Delete framework element deferred
   * (call blocks => not RT-capable)
   *
   * \param element_to_delete Framework element to delete (after safety period)
   */
  static void DeleteDeferred(tFrameworkElement& element_to_delete);

  static const char* GetLogDescription()
  {
    return "Garbage Deleter";
  }

//----------------------------------------------------------------------
// Private fields and methods
//----------------------------------------------------------------------
private:

  /*!
   * Garbage Collector task
   */
  class tDeferredDeleteTask
  {
    friend class tGarbageDeleter;

    /*! Element to delete */
    tFrameworkElement* element_to_delete;

    /*! Cycle in which to delete element */
    int64_t cycle_when;

    tDeferredDeleteTask(tFrameworkElement* element_to_delete, int64_t cycle_when) :
      element_to_delete(element_to_delete),
      cycle_when(cycle_when)
    {}

    tDeferredDeleteTask() :
      element_to_delete(),
      cycle_when(0)
    {}
  };

  /*! Current tasks of Garbage Collector */
  std::queue<tDeferredDeleteTask> tasks;

  /*! Current tasks of Garbage Collector from real-time threads */
  // rt_tasks;

  /*! Next delete task - never null */
  tDeferredDeleteTask next;

  /*! Number of cycles garbage collector has been executed */
  std::atomic<int64_t> cycle_count;


  tGarbageDeleter();

  virtual ~tGarbageDeleter();

  virtual void MainLoopCallback();

  virtual void Run();

  virtual void StopThread();
};

//----------------------------------------------------------------------
// End of namespace declaration
//----------------------------------------------------------------------
}
}
}


#endif
