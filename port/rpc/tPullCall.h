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

#ifndef core__port__rpc__tPullCall_h__
#define core__port__rpc__tPullCall_h__

#include "rrlib/finroc_core_utils/definitions.h"

#include "core/port/rpc/tAbstractCall.h"
#include "rrlib/finroc_core_utils/thread/tTask.h"

namespace rrlib
{
namespace serialization
{
class tInputStream;
} // namespace rrlib
} // namespace serialization

namespace finroc
{
namespace core
{
class tNetPort;

/*!
 * \author Max Reichardt
 *
 * This class is used for port-pull-requests/calls - locally and over the net.
 *
 * (Caller stack will contain every port in chain - pulled value will be assigned to each of them)
 */
class tPullCall : public tAbstractCall, public util::tTask
{
public:

  /*! Assign pulled value to ports in between? */
  bool intermediate_assign;

  /*! Is this a pull call for a cc port? */
  bool cc_pull;

  /*! when received through network and executed in separate thread: Port to call pull on and port to send result back over */
  tNetPort* port;

private:

  /*!
   * Reset all variable in order to reuse object
   */
  inline void Reset()
  {
    RecycleParameters();
  }

public:

  tPullCall();

  virtual void Deserialize(rrlib::serialization::tInputStream& is);

  virtual void ExecuteTask();

  /*!
   * Prepare Execution of call received over network in extra thread
   *
   * \param port Port to execute pull on and to return value over later
   */
  inline void PrepareForExecution(tNetPort* port_)
  {
    this->port = port_;
  }

  virtual void Serialize(rrlib::serialization::tOutputStream& oos) const;

  virtual const util::tString ToString() const
  {
    return util::tStringBuilder("PullCall (") + GetStatusString() + ", callid: " + ::finroc::core::tAbstractCall::GetMethodCallIndex() + ", threaduid: " + ::finroc::core::tAbstractCall::GetThreadUid() + ")";
  }

  virtual void CustomDelete(bool b)
  {
    tReusable::CustomDelete(b);
  }

};

} // namespace finroc
} // namespace core

#endif // core__port__rpc__tPullCall_h__
