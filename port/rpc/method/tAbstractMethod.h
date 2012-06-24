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

#ifndef core__port__rpc__method__tAbstractMethod_h__
#define core__port__rpc__method__tAbstractMethod_h__

#include "rrlib/finroc_core_utils/definitions.h"
#include "rrlib/finroc_core_utils/log/tLogUser.h"
#include <array>

#include "core/tRuntimeEnvironment.h"

#include "core/port/rpc/tParameterUtil.h"
#include "core/port/rpc/tMethodCall.h"

namespace finroc
{
namespace core
{
class tPortInterface;
class tInterfaceNetPort;
class tAbstractAsyncReturnHandler;
class tAbstractMethodCallHandler;

/*!
 * \author Max Reichardt
 *
 * This is the base class for all static method objects used in
 * remote procedure calls (RPCs).
 *
 * Subclasses are statically instantiated in InterfaceServerPort
 * for every method that server supports.
 *
 * These static instances contain all infos and provide all methods
 * for methods that server supports.
 */
class tAbstractMethod : public util::tLogUser
{
  friend class tPortInterface;
private:

  /*! Method name */
  util::tString name;

  /*! Parameter names */
  std::array<util::tString, 4> parameter_names;

  /*! Number of paramaters */
  int parameter_count;

  /*! Handle call in extra thread by default (should be true, if call can block or needs significant time to complete) */
  bool handle_in_extra_thread;

protected:

  /*! Name for unused parameters */
  static const char* cNO_PARAM;

  /*! Id of method - set by PortInterface class */
  int8 method_id;

  /*! PortInterface to which this method belongs */
  tPortInterface* type;

public:

  tAbstractMethod(tPortInterface& port_interface, const util::tString& name_, const util::tString& p1_name, const util::tString& p2_name, const util::tString& p3_name, const util::tString& p4_name, bool handle_in_extra_thread_);

  virtual ~tAbstractMethod()
  {
    tRuntimeEnvironment::Shutdown();
  }

  /*!
   * (only for async non-void calls)
   * If we have a method call object (prepared for sending) - actually send it over the net
   *
   * \param call Method call
   * \param net_port Network port to send it over
   * \param ret_handler Return handler
   * \param net_timeout Timeout for call
   */
  virtual void ExecuteAsyncNonVoidCallOverTheNet(tMethodCall::tPtr& mc, tInterfaceNetPort& net_port, tAbstractAsyncReturnHandler& ret_handler, const rrlib::time::tDuration& net_timeout) = 0;

  /*!
   * If we have a method call object (either from network or from another thread):
   * Execute the call
   *
   * \param call Method call object (must not be null and will be recycled - exception: method call is from net; return value will be stored in it)
   * \param handler Handler to handle object (must not be null)
   * \param ret_handler Return handler (optional)
   */
  virtual void ExecuteFromMethodCallObject(tMethodCall::tPtr& call, tAbstractMethodCallHandler& handler, tAbstractAsyncReturnHandler* ret_handler) = 0;

  inline int GetMethodId()
  {
    return method_id;
  }

  /*!
   * \return Name of method
   */
  inline util::tString GetName()
  {
    return name;
  }

  /*!
   * \return Port interface that method belongs to
   */
  inline tPortInterface* GetPortInterface()
  {
    return type;
  }

  inline bool HandleInExtraThread()
  {
    return handle_in_extra_thread;
  }

  /*!
   * \return Is this a void method?
   */
  virtual bool IsVoidMethod() = 0;

  bool operator==(const tAbstractMethod& other) const
  {
    return this == &other;
  }
};

/*!
 * Method that returns something.
 * Can be called synchronous and asynchronous.
 *
 * Synchronous call will block current thread until return value is available.
 * Asynchronous call will call the provided ReturnHandler when return value is available (this
 * often starts method call in a separate thread, so be careful, not to send dozens at once over the network...)
 */
class tAbstractNonVoidMethod : public tAbstractMethod
{
private:

  /*! Default timeout for calls over the net */
  rrlib::time::tDuration default_net_timeout;

protected:

  /*! Default network timeout */
#if (__GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 6))
  static constexpr rrlib::time::tDuration cDEFAULT_NET_TIMEOUT = std::chrono::seconds(2);
#else
  static rrlib::time::tDuration cDEFAULT_NET_TIMEOUT;
#endif

public:

  tAbstractNonVoidMethod(tPortInterface& port_interface, const util::tString& name, const util::tString& p1_name, const util::tString& p2_name, const util::tString& p3_name, const util::tString& p4_name, bool handle_in_extra_thread, rrlib::time::tDuration default_net_timeout);

  /*!
   * \return Default timeout for calls over the net
   */
  inline rrlib::time::tDuration GetDefaultNetTimeout()
  {
    return default_net_timeout;
  }

  virtual bool IsVoidMethod()
  {
    return false;
  }

};

namespace internal
{

/**
 * Helper to count number of template arguments
 */
template<typename... TArgs>
struct tCountArgs;

template <>
struct tCountArgs<>
{
  enum { value = 0 };
};

template<typename T, typename... TArgs>
struct tCountArgs<T, TArgs...>
{
  enum { value = 1 + tCountArgs<TArgs...>::value };
};

}

} // namespace finroc
} // namespace core

#endif // core__port__rpc__method__tAbstractMethod_h__
