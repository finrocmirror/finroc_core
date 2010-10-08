//Generated from Port4Method.java
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
#include "finroc_core_utils/tJCBase.h"

#ifndef CORE__PORT__RPC__METHOD__TPORT1METHOD_H
#define CORE__PORT__RPC__METHOD__TPORT1METHOD_H

#include "core/port/rpc/method/tAsyncReturnHandler.h"
#include "core/port/rpc/method/tAbstractMethod.h"

namespace finroc
{
namespace core
{
class tPortInterface;
class tInterfaceClientPort;
class tMethodCall;
class tInterfaceNetPort;
class tAbstractAsyncReturnHandler;
class tAbstractMethodCallHandler;

/*!
 * \author Max Reichardt
 *
 * Non-void method with 1 parameters.
 */
template<typename HANDLER, typename R, typename P1>
class tPort1Method : public tAbstractNonVoidMethod
{
public:

  /*!
   * \param port_interface PortInterface that method belongs to
   * \param name Name of method        //1
   * \param p1_name Name of parameter 1
   * \param handle_in_extra_thread Handle call in extra thread by default (only relevant for async calls; should be true if call (including return handler) can block or can consume a significant amount of time)
   * \param default_net_timeout Default timeout for calls over the net (should be higher than any timeout for call to avoid that returning calls get lost)
   */
  tPort1Method(tPortInterface& port_interface, const util::tString& name, const util::tString& p1_name, bool handle_in_extra_thread, int default_net_timeout = cDEFAULT_NET_TIMEOUT);

  /*!
   * Call method and wait for return value.
   * (is performed in same thread and blocks)
   *
   * \param port Port that call is performed from (typically 'this')
   * \param p1 Parameter 1 (with one lock for call - typically server will release it)
   * \param p2 Parameter 2 (with one lock for call - typically server will release it)
   * \param p3 Parameter 3 (with one lock for call - typically server will release it)
   * \param p4 Parameter 4 (with one lock for call - typically server will release it)//
   * \param net_timout Network timeout in ms (value <= 0 means method default)
   * \return return value of method
   */
  R Call(tInterfaceClientPort* port, P1 p1, int net_timeout = -1);

  /*!
   * Asynchronously call method.
   * Return value is handled by AsyncReturnHandler passed to this method.
   * (is performed in separate thread, if method object suggests so and server is on local machine)
   *
   * \param port Port that call is performed from (typically 'this')
   * \param handler AsyncReturnHandler that will handle return value.                  //1
   * \param p1 Parameter 1 (with one lock for call - typically server will release it)
   * \param net_timout Network timeout in ms (value <= 0 means method default)
   * \param force_same_thread Force that method call is performed by this thread on local machine (even if method call default is something else)
   */
  void CallAsync(const tInterfaceClientPort* port, tAsyncReturnHandler<R>* handler, P1 p1, int net_timeout = -1, bool force_same_thread = false);

  virtual void ExecuteAsyncNonVoidCallOverTheNet(tMethodCall* mc, tInterfaceNetPort* net_port, tAbstractAsyncReturnHandler* ret_handler, int net_timeout);

  virtual void ExecuteFromMethodCallObject(tMethodCall* call, tAbstractMethodCallHandler* handler, tAbstractAsyncReturnHandler* ret_handler);

  /*!
   * Called when current thread is not receiver thread of return
   * 2 cases for this:
   * - method call has been received from the net
   * - method call is executed asynchronously in local runtime
   *
   * \param call Call to process
   * \param handler Handler (server port) that will handle call
   * \param ret_handler Return Handler (not null - when receiver is in local runtime)
   * \param dummy Dummy parameter - to ensure that generic executeFromMethodCallObject-method will always call this
   */
  void ExecuteFromMethodCallObject(tMethodCall* call, const HANDLER& handler, tAsyncReturnHandler<R>* ret_handler, bool dummy);

};

} // namespace finroc
} // namespace core

#include "core/port/rpc/method/tPort1Method.hpp"

#endif // CORE__PORT__RPC__METHOD__TPORT1METHOD_H
