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

#ifndef CORE__PORT__RPC__METHOD__TVOID4METHOD_H
#define CORE__PORT__RPC__METHOD__TVOID4METHOD_H

#include "core/port/rpc/method/tPortInterface.h"
#include "core/port/rpc/tInterfaceClientPort.h"
#include "core/port/rpc/tMethodCall.h"
#include "core/port/rpc/tInterfaceNetPort.h"
#include "core/port/rpc/method/tAbstractAsyncReturnHandler.h"
#include "core/port/rpc/method/tAbstractMethodCallHandler.h"
#include "core/port/rpc/method/tAbstractMethod.h"

namespace finroc
{
namespace core
{
template<typename HANDLER, typename P1, typename P2, typename P3, typename P4>
class tVoid4Method : public tAbstractVoidMethod
{
public:

  /*!
   * \param port_interface PortInterface that method belongs to
   * \param name Name of method        //1
   * \param p1_name Name of parameter 1 //2
   * \param p2_name Name of parameter 2 //3
   * \param p3_name Name of parameter 3 //4
   * \param p4_name Name of parameter 4 //n
   * \param handle_in_extra_thread Handle call in extra thread by default (should be true if call can block or can consume a significant amount of time)
   */
  tVoid4Method(tPortInterface& port_interface, const util::tString& name, const util::tString& p1_name, const util::tString& p2_name, const util::tString& p3_name, const util::tString& p4_name, bool handle_in_extra_thread);

  /*!
   * Call method.
   * (is performed in separate thread, if method object suggests so and server is on local machine)
   *
   * \param port Port that call is performed from (typically 'this')                   //1
   * \param p1 Parameter 1 (with one lock for call - typically server will release it) //2
   * \param p2 Parameter 2 (with one lock for call - typically server will release it) //3
   * \param p3 Parameter 3 (with one lock for call - typically server will release it) //4
   * \param p4 Parameter 4 (with one lock for call - typically server will release it) //n
   * \param force_same_thread Force that method call is performed by this thread on local machine (even if method call default is something else)
   */
  void Call(tInterfaceClientPort* port, P1 p1, P2 p2, P3 p3, P4 p4, bool force_same_thread = false);

  virtual void ExecuteAsyncNonVoidCallOverTheNet(tMethodCall* call, tInterfaceNetPort* net_port, tAbstractAsyncReturnHandler* ret_handler, int net_timeout)
  {
    throw util::tRuntimeException("Only supported by non-void methods", __CODE_LOCATION__);
  }

  virtual void ExecuteFromMethodCallObject(tMethodCall* call, tAbstractMethodCallHandler* handler, tAbstractAsyncReturnHandler* ret_handler);

  void ExecuteFromMethodCallObject(tMethodCall* call, const HANDLER& handler);

};

} // namespace finroc
} // namespace core

#include "core/port/rpc/method/tVoid4Method.hpp"

#endif // CORE__PORT__RPC__METHOD__TVOID4METHOD_H
