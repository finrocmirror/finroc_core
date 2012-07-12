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

#ifndef core__port__rpc__tMethodCall_h__
#define core__port__rpc__tMethodCall_h__

#include "rrlib/finroc_core_utils/definitions.h"

#include "rrlib/serialization/serialization.h"
#include "core/port/rpc/tAbstractCall.h"

namespace finroc
{
namespace core
{
class tAbstractMethod;
class tAbstractMethodCallHandler;
class tAbstractAsyncReturnHandler;
class tInterfaceNetPort;

/*!
 * \author Max Reichardt
 *
 * This is the class for a complete method call.
 *
 * Such calls can be sent over the network via ports.
 * They can furthermore be asynchronous.
 *
 * Currently a method call may have max. 3 double parameters,
 * 3 long parameters and 2 object parameters. This should be
 * sufficient - since anything can be put into custom objects.
 */
class tMethodCall : public tAbstractCall
{
private:

  /*! Method that is called */
  tAbstractMethod* method;

  /*!
   * Data type of interface that method belong, too
   * (method may belong to multiple - so this is the one
   *  we wan't to actually use)
   */
  rrlib::rtti::tDataTypeBase port_interface_type;

  /*! Needed when executed as a task: Handler that will handle this call */
  tAbstractMethodCallHandler* handler;

  /*! Needed when executed as a task and method has return value: Handler that will handle return of this call */
  tAbstractAsyncReturnHandler* ret_handler;

  /*! Needed when executed as a task with synch call over the net - Port over which call is sent */
  tInterfaceNetPort* net_port;

  /*! Needed when executed as a task with synch call over the net - Network timeout in ms */
  rrlib::time::tDuration net_timeout;

  /*! Needed when executed as a task with synch forward over the net - Port from which call originates */
  tInterfaceNetPort* source_net_port;

  /*! Maximum number of parameters */
  static const size_t cMAX_PARAMS = 4u;

  /*! Storage for parameters that are used in call - for usage in local runtime (fixed size, since this is smaller & less hassle than dynamic array) */
  tCallParameter params[cMAX_PARAMS];

  /*!
   * Sanity check for method and portInterfaceType.
   *
   * \return Is everything all right?
   */
  bool TypeCheck();

protected:

  virtual void GenericRecycle()
  {
    Recycle();
  }

  void Recycle();

  void SetParametersHelper(int arg_no) {}

  template <typename Arg, typename ... TArgs>
  void SetParametersHelper(int arg_no, Arg& arg, TArgs&... args)
  {
    SetParameter(arg_no, arg);
    SetParametersHelper(arg_no + 1, args...);
  }

public:

  typedef std::unique_ptr<tMethodCall, tRecycler> tPtr;

  /*! (Typically not instantiated directly - possible though) */
  tMethodCall();

  virtual void CustomDelete(bool b)
  {
    tReusable::CustomDelete(b);
  }

  virtual void Deserialize(rrlib::serialization::tInputStream& is)
  {
    throw util::tRuntimeException("Call deserializeCall instead, please!", CODE_LOCATION_MACRO);
  }

  /*!
   * (Buffer source for CoreInput should have been set before calling with parameter deserialization enabled)
   *
   * \param is Input Stream
   * \param dt Method Data Type
   * \param skip_parameters Skip deserialization of parameter stuff? (for cases when port has been deleted;
   * in this case we need to jump to skip target afterwards)
   */
  void DeserializeCall(rrlib::serialization::tInputStream& is, const rrlib::rtti::tDataTypeBase& dt, bool skip_parameters);

  virtual void ExecuteTask(tSerializableReusableTask::tPtr& self);

  /*!
   * \return the methodID
   */
  inline tAbstractMethod* GetMethod()
  {
    return method;
  }

  /*!
   * \return Needed when executed as a task with synch call over the net - Network timeout in ms
   */
  inline rrlib::time::tDuration GetNetTimeout()
  {
    return net_timeout;
  }

  template <typename T>
  void GetParam(int index, T& pd)
  {
    tParameterUtil<T>::GetParam(&(params[index]), pd);
  }

  tPortDataPtr<rrlib::rtti::tGenericObject> GetParamGeneric(int index);

  /*!
   * \return Data type of interface that method belongs to
   */
  inline const rrlib::rtti::tDataTypeBase GetPortInterfaceType()
  {
    return port_interface_type;
  }

  /*!
   * Prepare method call object for execution in another thread (as a task)
   *
   * \param method Method that is to be called
   * \param port_interface Data type of interface that method belongs to
   * \param handler Handler (server port) that will handle method
   * \param ret_handler asynchronous return handler (required for method calls with return value)
   */
  void PrepareExecution(tAbstractMethod* method_, const rrlib::rtti::tDataTypeBase& port_interface, tAbstractMethodCallHandler* handler_, tAbstractAsyncReturnHandler* ret_handler_);

  /*!
   * Prepare method call object for blocking remote execution in another thread (as a task)
   * Difference to above: Method call was received from the net
   *
   * \param interface_net_port
   * \param mhandler
   */
  void PrepareExecutionForCallFromNetwork(tInterfaceNetPort* source, tAbstractMethodCallHandler* mhandler);

  /*!
   * Prepare method call object for blocking remote execution in another thread (as a task)
   * Difference to above: Method call was received from the net and is simply forwarded
   *
   * \param source Port that method call was received from
   * \param dest Port that method call will be forwarded to
   */
  void PrepareForwardSyncRemoteExecution(tInterfaceNetPort* source, tInterfaceNetPort* dest);

  /*!
   * Prepare method call object for blocking remote execution in another thread (as a task)
   *
   * \param method Method that is to be called
   * \param port_interface Data type of interface that method belongs to
   * \param ret_handler asynchronous return handler (required for method calls with return value)
   * \param net_port Port over which call is sent
   * \param net_timeout Network timeout in ms for call
   */
  void PrepareSyncRemoteExecution(tAbstractMethod* method_, const rrlib::rtti::tDataTypeBase& port_interface, tAbstractAsyncReturnHandler* ret_handler_, tInterfaceNetPort* net_port_, const rrlib::time::tDuration& net_timeout_);

  /*!
   * Prepare method call object for blocking remote execution in same thread (as a task)
   *
   * \param method Method that is to be called
   * \param port_interface Data type of interface that method belongs to
   * \param net_timeout Network timeout in ms for call
   */
  void PrepareSyncRemoteExecution(tAbstractMethod* method_, const rrlib::rtti::tDataTypeBase& port_interface, const rrlib::time::tDuration& net_timeout_);

  /*!
   * Recycle all parameters, but keep empty method call
   */
  void RecycleParameters();

  virtual void Serialize(rrlib::serialization::tOutputStream& oos) const;

  virtual void SetExceptionStatus(tMethodCallException::tType type)
  {
    RecycleParameters();
    tAbstractCall::SetExceptionStatus(type);
  }

  /*!
   * \param m The Method that will be called (may not be changed - to avoid ugly programming errors)
   * \param port_interface Data type of interface that method belongs to
   */
  void SetMethod(tAbstractMethod* m, const rrlib::rtti::tDataTypeBase& port_interface);

  /*!
   * Set Parameter with specified in index to specified value
   */
  template <typename T>
  void SetParameter(int index, T& pd)
  {
    tParameterUtil<T>::AddParam(&(params[index]), pd);
  }

  /*!
   * Set Parameters to specified values
   */
  template <typename ... TArgs>
  void SetParameters(TArgs&... args)
  {
    SetParametersHelper(0, args...);
  }

  virtual const util::tString ToString() const;
};

} // namespace finroc
} // namespace core

#endif // core__port__rpc__tMethodCall_h__
