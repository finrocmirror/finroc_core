/**
 * You received this file as part of an advanced experimental
 * robotics framework prototype ('finroc')
 *
 * Copyright (C) 2007-2012 Max Reichardt,
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

#ifndef core__port__rpc__tPullCall_h__
#define core__port__rpc__tPullCall_h__

#include "rrlib/finroc_core_utils/definitions.h"
#include "rrlib/finroc_core_utils/thread/tTask.h"

#include "core/port/tAbstractPort.h"
#include "core/port/rpc/tAbstractCall.h"
#include "core/port/rpc/tCallable.h"


namespace finroc
{
namespace core
{

/*!
 * \author Max Reichardt
 *
 * This class is used for port-pull-requests/calls - locally and over the net.
 *
 * (Caller stack will contain every port in chain - pulled value will be assigned to each of them)
 */
class tPullCall : public tAbstractCall
{
  /*! Assign pulled value to ports in between? */
  bool intermediate_assign;

  /*! When received through network and executed in separate thread: Port to call pull on */
  tAbstractPort* port;

  /*! When received through network and executed in separate thread: Object to return result to */
  tCallable<tPullCall>* return_to;

  /*! Desired data encoding for port data */
  rrlib::serialization::tDataEncoding desired_encoding;

  /*! If pull call is returning: Pulled buffer */
  tPortDataPtr<rrlib::rtti::tGenericObject> pulled_buffer;

public:

  typedef std::unique_ptr<tPullCall, tRecycler> tPtr;

  tPullCall();

  virtual void CustomDelete(bool b)
  {
    tReusable::CustomDelete(b);
  }

  virtual void Deserialize(rrlib::serialization::tInputStream& is);

  virtual void ExecuteTask(tSerializableReusableTask::tPtr& self);

  virtual void GenericRecycle()
  {
    Recycle();
  }

  /*!
   * \return If pull call is returning: Pulled buffer
   */
  rrlib::rtti::tGenericObject* GetPulledBuffer()
  {
    return pulled_buffer.get();
  }

  /*!
   * Prepare Execution of call received over network in extra thread
   *
   * \param port Port to execute pull on and to return value over later
   * \param return_to When received through network and executed in separate thread: Object to return result to
   */
  inline void PrepareForExecution(tAbstractPort& port, tCallable<tPullCall>& return_to)
  {
    this->port = &port;
    this->return_to = &return_to;
  }

  virtual void Recycle();

  virtual void Serialize(rrlib::serialization::tOutputStream& oos) const;

  /*!
   * Setup pull call for remote execution
   *
   * \param remote_handle Destination port handle - only used while call is enqueued in network queue
   * \param intermediate_assign Assign pulled value to ports in between?
   * \param encoding Data encoding to use for serialization of pulled buffer
   */
  void SetupPullCall(int remote_handle, bool intermediate_assign, rrlib::serialization::tDataEncoding enc)
  {
    SetRemotePortHandle(remote_handle);
    this->intermediate_assign = intermediate_assign;
    this->desired_encoding = enc;
  }

  virtual const util::tString ToString() const;

};

} // namespace finroc
} // namespace core

#endif // core__port__rpc__tPullCall_h__
