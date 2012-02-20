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

#ifndef core__port__rpc__method__tVoidMethod_h__
#define core__port__rpc__method__tVoidMethod_h__

#include "rrlib/finroc_core_utils/definitions.h"

#include "core/port/rpc/tInterfaceClientPort.h"
#include "core/port/rpc/method/tAbstractMethod.h"

namespace finroc
{
namespace core
{
class tPortInterface;
class tMethodCall;
class tInterfaceNetPort;
class tAbstractAsyncReturnHandler;
class tAbstractMethodCallHandler;

/*!
 * Method that does not return anything - with an arbitrary number of parameters.
 *
 * Handler class needs to implement:
 *   void HandleVoidCall(tAbstractMethod& method, <args>);
 */
template<typename HANDLER, typename ... TArgs>
class tVoidMethod : public tAbstractMethod
{
  static const int cARG_COUNT = internal::tCountArgs<TArgs...>::value;
public:

  /*!
   * \param port_interface PortInterface that method belongs to
   * \param name Name of method
   * \param p1_name Name of parameter 1
   * \param p2_name Name of parameter 2
   * \param p3_name Name of parameter 3
   * \param p4_name Name of parameter 4
   * \param handle_in_extra_thread Handle call in extra thread by default (should be true if call can block or can consume a significant amount of time)
   */
  template <int ARGCOUNT = cARG_COUNT>
  tVoidMethod(tPortInterface& port_interface, const util::tString& name, const util::tString& p1_name, const util::tString& p2_name, const util::tString& p3_name, const util::tString& p4_name, typename std::enable_if<ARGCOUNT == 4, bool>::type handle_in_extra_thread) :
    tAbstractMethod(port_interface, name, p1_name, p2_name, p3_name, p4_name, handle_in_extra_thread) {}
  template <int ARGCOUNT = cARG_COUNT>
  tVoidMethod(tPortInterface& port_interface, const util::tString& name, const util::tString& p1_name, const util::tString& p2_name, const util::tString& p3_name, typename std::enable_if<ARGCOUNT == 3, bool>::type handle_in_extra_thread) :
    tAbstractMethod(port_interface, name, p1_name, p2_name, p3_name, cNO_PARAM, handle_in_extra_thread) {}
  template <int ARGCOUNT = cARG_COUNT>
  tVoidMethod(tPortInterface& port_interface, const util::tString& name, const util::tString& p1_name, const util::tString& p2_name, typename std::enable_if<ARGCOUNT == 2, bool>::type handle_in_extra_thread) :
    tAbstractMethod(port_interface, name, p1_name, p2_name, cNO_PARAM, cNO_PARAM, handle_in_extra_thread) {}
  template <int ARGCOUNT = cARG_COUNT>
  tVoidMethod(tPortInterface& port_interface, const util::tString& name, const util::tString& p1_name, typename std::enable_if<ARGCOUNT == 1, bool>::type handle_in_extra_thread) :
    tAbstractMethod(port_interface, name, p1_name, cNO_PARAM, cNO_PARAM, cNO_PARAM, handle_in_extra_thread) {}
  template <int ARGCOUNT = cARG_COUNT>
  tVoidMethod(tPortInterface& port_interface, const util::tString& name, typename std::enable_if<ARGCOUNT == 0, bool>::type handle_in_extra_thread) :
    tAbstractMethod(port_interface, name, cNO_PARAM, cNO_PARAM, cNO_PARAM, cNO_PARAM, handle_in_extra_thread) {}

  /*!
   * Call method.
   * (is performed in separate thread, if method object suggests so and server is on local machine)
   *
   * \param port Port that call is performed from (typically 'this')                   //1
   * \param force_same_thread Force that method call is performed by this thread on local machine (even if method call default is something else)
   * \param args Arguments for method call
   */
  void Call(tInterfaceClientPort port, bool force_same_thread, TArgs... args);
  inline void Call(tInterfaceClientPort port, TArgs... args)
  {
    Call(port, false, args...);
  }

  virtual void ExecuteAsyncNonVoidCallOverTheNet(tMethodCall::tPtr& mc, tInterfaceNetPort& net_port, tAbstractAsyncReturnHandler& ret_handler, int net_timeout)
  {
    throw util::tRuntimeException("Only supported by non-void methods", CODE_LOCATION_MACRO);
  }

  virtual void ExecuteFromMethodCallObject(tMethodCall::tPtr& call, tAbstractMethodCallHandler& handler, tAbstractAsyncReturnHandler* ret_handler);

  void ExecuteFromMethodCallObject(tMethodCall::tPtr& call, HANDLER& handler);

  virtual bool IsVoidMethod()
  {
    return true;
  }
};

} // namespace finroc
} // namespace core

#include "core/port/rpc/method/tVoidMethod.hpp"

#endif // core__port__rpc__method__tVoidMethod_h__
