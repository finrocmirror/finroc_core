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

#ifndef core__port__rpc__tInterfaceNetPort_h__
#define core__port__rpc__tInterfaceNetPort_h__

#include "rrlib/finroc_core_utils/definitions.h"

#include "core/port/tPortCreationInfo.h"
#include "core/port/rpc/tInterfacePort.h"

namespace finroc
{
namespace core
{
class tMethodCall;
class tAbstractMethodCallHandler;

/*!
 * \author Max Reichardt
 *
 * Interface ports that will forward calls over the net
 */
class tInterfaceNetPort : public tInterfacePort
{
public:

  tInterfaceNetPort(tPortCreationInfo pci);

  /*!
   * Called in extread thread for call received from network
   *
   * \param mc Method call
   * \param mhandler Server/Handler handling call
   */
  void ExecuteCallFromNetwork(tMethodCall* mc, tAbstractMethodCallHandler* mhandler);

  /*!
   * Called in extra thread for network forwarding of a call
   *
   * \param mc Call to forward
   * \param net_port Port to forward call over
   */
  void ExecuteNetworkForward(tMethodCall* mc, tInterfaceNetPort* net_port);

  /*!
   * Process method call that was received from a network connection
   *
   * \param mc Received method call (takes responsibility of recycling it)
   * \return MethodCall containing result
   */
  void ProcessCallFromNet(tMethodCall* mc);

  /*!
   * Send asynchronous call over network connection
   *
   * \param mc Call to send (recycles call including parameters afterwards)
   */
  virtual void SendAsyncCall(tMethodCall* mc) = 0;

  /*!
   * Send synchronous call return over network connection
   * (usually pretty similar to above method)
   *
   * \param mc Call to send (recycles call including parameters afterwards)
   */
  virtual void SendSyncCallReturn(tMethodCall* mc) = 0;

  /*!
   * Send synchronous call over network connection
   * Thread blocks until result is available or call timed out
   *
   * \param mc Method Call to process
   * \param timeout Network timeout for call (should be larger than any timeout in call due to network delays)
   * \return Method Call object containing results (may be a different one than mc - transfers responsibility for it to caller)
   */
  virtual tMethodCall* SynchCallOverTheNet(tMethodCall* mc, int timeout) = 0;

};

} // namespace finroc
} // namespace core

#endif // core__port__rpc__tInterfaceNetPort_h__
