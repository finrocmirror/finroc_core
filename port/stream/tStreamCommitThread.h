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
#include "rrlib/finroc_core_utils/tJCBase.h"

#ifndef CORE__PORT__STREAM__TSTREAMCOMMITTHREAD_H
#define CORE__PORT__STREAM__TSTREAMCOMMITTHREAD_H

#include "rrlib/finroc_core_utils/thread/sThreadUtil.h"
#include "core/thread/tCoreLoopThreadBase.h"

namespace finroc
{
namespace core
{
/*!
 * \author Max Reichardt
 *
 * This thread performs update tasks for streams
 */
class tStreamCommitThread : public tCoreLoopThreadBase
{
public:
  class tCallback; // inner class forward declaration
private:

  /*! Singleton instance */
  static ::std::tr1::shared_ptr<tStreamCommitThread> instance;

  /*! All stream threads that need to be processed */
  util::tSafeConcurrentlyIterableList<tCallback*> callbacks;

  tStreamCommitThread();

public:

  /*!
   * \return Singleton instance
   */
  inline static tStreamCommitThread* GetInstance()
  {
    return instance.get();
  }

  virtual void MainLoopCallback();

  /*!
   * (Called by OutputStreamPorts)
   * They register so that thread will regularly check whether things need to be commited
   */
  inline void Register(tCallback* c)
  {
    callbacks.Add(c, false);
  }

  inline static void StaticInit()
  {
    instance = util::sThreadUtil::GetThreadSharedPtr(new tStreamCommitThread());
  }

  //  public static void staticStop() {
  //      if (instance == null) {
  //          return;
  //      }
  //      instance.stopThread();
  //      try {
  //          instance.join();
  //      } catch (InterruptedException e) {
  //          //e.printStackTrace();
  //          System.out.println("Exception in StreamCommitThread::staticStop");
  //      }
  //      instance = null;
  //  }

  virtual void StopThread();

  /*!
   * (Called by OutputStreamPorts when they're deleted)
   */
  inline void Unregister(tCallback* c)
  {
    callbacks.Remove(c);
  }

public:

  /*!
   * Classes that want to be called by StreamThread need to implement this interface
   */
  class tCallback : public util::tInterface
  {
  public:

    virtual void StreamThreadCallback(int64 cur_time) = 0;

  };

};

} // namespace finroc
} // namespace core

#endif // CORE__PORT__STREAM__TSTREAMCOMMITTHREAD_H
