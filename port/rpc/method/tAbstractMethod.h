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

#ifndef CORE__PORT__RPC__METHOD__TABSTRACTMETHOD_H
#define CORE__PORT__RPC__METHOD__TABSTRACTMETHOD_H

#include "core/port/rpc/tMethodCall.h"
#include "core/port/rpc/method/tAbstractAsyncReturnHandler.h"
#include "core/port/rpc/method/tAbstractMethodCallHandler.h"
#include "finroc_core_utils/log/tLogUser.h"

#include "core/port/rpc/tParameterUtil.h"

namespace finroc
{
namespace core
{
class tPortInterface;
class tInterfaceNetPort;

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
  ::finroc::util::tArrayWrapper<util::tString> parameter_names;

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

  /*! Log domain for this class */
  RRLIB_LOG_CREATE_NAMED_DOMAIN(log_domain, "rpc");

  tAbstractMethod(tPortInterface& port_interface, const util::tString& name_, const util::tString& p1_name, const util::tString& p2_name, const util::tString& p3_name, const util::tString& p4_name, bool handle_in_extra_thread_);

  /*!
   * (only for async non-void calls)
   * If we have a method call object (prepared for sending) - actually send it over the net
   *
   * \param call Method call
   * \param net_port Network port to send it over
   * \param ret_handler Return handler
   * \param net_timeout Timeout for call
   */
  virtual void ExecuteAsyncNonVoidCallOverTheNet(tMethodCall* call, tInterfaceNetPort* net_port, tAbstractAsyncReturnHandler* ret_handler, int net_timeout) = 0;

  /*!
   * If we have a method call object (either from network or from another thread):
   * Execute the call
   *
   * \param call Method call object (must not be null and will be recycled - exception: method call is from net; return value will be stored in it)
   * \param handler Handler to handle object (must not be null)
   * \param ret_handler Return handler (optional)
   */
  virtual void ExecuteFromMethodCallObject(tMethodCall* call, tAbstractMethodCallHandler* handler, tAbstractAsyncReturnHandler* ret_handler) = 0;

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

  template <typename T>
  bool HasLock(T t)
  {
    return tParameterUtil<T>::HasLock(t);
  }

  template <typename T>
  void Cleanup(T t)
  {
    tParameterUtil<T>::Cleanup(t);
  }

  //
  //  bool hasLock(const PortData* pd) {
  //      if (pd == NULL) {
  //          return true;
  //      }
  //      return pd->getManager()->isLocked();
  //  }
  //
  //  bool hasLock(const CCPortData* cpd) {
  //      if (cpd == NULL) {
  //          return true;
  //      }
  //      return true;
  //  }
  //
  //  inline bool hasLock(const int64 cpd) {
  //      return true;
  //  }
  //
  //  inline bool hasLock(const double cpd) {
  //      return true;
  //  }
  //
  //  inline bool hasLock(const int cpd) {
  //      return true;
  //  }
  //
  //  inline bool hasLock(const int16 cpd) {
  //      return true;
  //  }
  //
  //  inline bool hasLock(const int8 cpd) {
  //      return true;
  //  }
  //
  //  inline bool hasLock(const float cpd) {
  //      return true;
  //  }
  //
  //  template <typename T>
  //  bool hasLock(const CCInterThreadContainer<T>* c) {
  //      if (c == NULL) {
  //          return true;
  //      }
  //      return true;
  //  }
  //
  //  template <typename T>
  //  inline bool hasLock(T t) {
  //      return hasLock(static_cast<const T>(t));
  //  }
  //
  //
  //  void cleanup(PortData* pd) {
  //      if (pd == NULL) {
  //          return;
  //      }
  //      pd->getManager()->releaseLock();
  //  }
  //
  //  void cleanup(const CCPortData* pd) {}
  //  inline void cleanup(const int num) {}
  //  inline void cleanup(const double num) {}
  //  inline void cleanup(const int64 num) {}
  //  inline void cleanup(const int16 num) {}
  //  inline void cleanup(const int8 num) {}
  //  inline void cleanup(const float num) {}
  //
  //  template <typename T>
  //  void cleanup(const CCInterThreadContainer<T>* c) {
  //      if (c == NULL) {
  //          return;
  //      }
  //      c->recycle2();
  //  }
  //
  //  template <typename T>
  //  inline void cleanup(T t) {
  //      cleanup(static_cast<const T>(t));
  //  }
  //

  inline bool HandleInExtraThread()
  {
    return handle_in_extra_thread;
  }

  /*!
   * \return Is this a void method?
   */
  virtual bool IsVoidMethod() = 0;

};

/*!
 * Method that does not return anything.
 * Such methods typically don't block.
 */
class tAbstractVoidMethod : public tAbstractMethod
{
public:

  tAbstractVoidMethod(tPortInterface& port_interface, const util::tString& name, const util::tString& p1_name, const util::tString& p2_name, const util::tString& p3_name, const util::tString& p4_name, bool handle_in_extra_thread);

  virtual bool IsVoidMethod()
  {
    return true;
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
  int default_net_timeout;

protected:

  /*! Default network timeout */
  static const int cDEFAULT_NET_TIMEOUT = 2000;

public:

  tAbstractNonVoidMethod(tPortInterface& port_interface, const util::tString& name, const util::tString& p1_name, const util::tString& p2_name, const util::tString& p3_name, const util::tString& p4_name, bool handle_in_extra_thread, int default_net_timeout_);

  /*!
   * \return Default timeout for calls over the net
   */
  inline int GetDefaultNetTimeout()
  {
    return default_net_timeout;
  }

  virtual bool IsVoidMethod()
  {
    return false;
  }

};

} // namespace finroc
} // namespace core

#endif // CORE__PORT__RPC__METHOD__TABSTRACTMETHOD_H
