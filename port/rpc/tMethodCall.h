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

#ifndef core__port__rpc__tMethodCall_h__
#define core__port__rpc__tMethodCall_h__

#include "rrlib/finroc_core_utils/definitions.h"

#include "rrlib/serialization/tDataTypeBase.h"
#include "core/port/rpc/tAbstractCall.h"
#include "rrlib/finroc_core_utils/thread/tTask.h"

namespace finroc
{
namespace core
{
class tAbstractMethod;
class tAbstractMethodCallHandler;
class tAbstractAsyncReturnHandler;
class tInterfaceNetPort;
class tCoreInput;
class tCoreOutput;

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
class tMethodCall : public tAbstractCall, public util::tTask
{
private:

  /*! Method that is called */
  tAbstractMethod* method;

  /*!
   * Data type of interface that method belong, too
   * (method may belong to multiple - so this is the one
   *  we wan't to actually use)
   */
  rrlib::serialization::tDataTypeBase port_interface_type;

  /*! Needed when executed as a task: Handler that will handle this call */
  tAbstractMethodCallHandler* handler;

  /*! Needed when executed as a task and method has return value: Handler that will handle return of this call */
  tAbstractAsyncReturnHandler* ret_handler;

  /*! Needed when executed as a task with synch call over the net - Port over which call is sent */
  tInterfaceNetPort* net_port;

  /*! Needed when executed as a task with synch call over the net - Network timeout in ms */
  int net_timeout;

  /*! Needed when executed as a task with synch forward over the net - Port from which call originates */
  tInterfaceNetPort* source_net_port;

  /*!
   * Sanity check for method and portInterfaceType.
   *
   * \return Is everything all right?
   */
  bool TypeCheck();

public:

  /*! (Typically not instantiated directly - possible though) */
  tMethodCall();

  virtual void Deserialize(tCoreInput& is)
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
  void DeserializeCall(tCoreInput* is, const rrlib::serialization::tDataTypeBase& dt, bool skip_parameters);

  virtual void ExecuteTask();

  virtual void GenericRecycle()
  {
    Recycle();
  }

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
  inline int GetNetTimeout()
  {
    return net_timeout;
  }

  /*!
   * \return Data type of interface that method belongs to
   */
  inline const rrlib::serialization::tDataTypeBase GetPortInterfaceType()
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
  void PrepareExecution(tAbstractMethod* method_, const rrlib::serialization::tDataTypeBase& port_interface, tAbstractMethodCallHandler* handler_, tAbstractAsyncReturnHandler* ret_handler_);

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
  void PrepareSyncRemoteExecution(tAbstractMethod* method_, const rrlib::serialization::tDataTypeBase& port_interface, tAbstractAsyncReturnHandler* ret_handler_, tInterfaceNetPort* net_port_, int net_timeout_);

  /*!
   * Prepare method call object for blocking remote execution in same thread (as a task)
   *
   * \param method Method that is to be called
   * \param port_interface Data type of interface that method belongs to
   * \param net_timeout Network timeout in ms for call
   */
  void PrepareSyncRemoteExecution(tAbstractMethod* method_, const rrlib::serialization::tDataTypeBase& port_interface, int net_timeout_);

  void Recycle();

  virtual void Serialize(tCoreOutput& oos) const;

  /*!
   * \param m The Method that will be called (may not be changed - to avoid ugly programming errors)
   * \param port_interface Data type of interface that method belongs to
   */
  void SetMethod(tAbstractMethod* m, const rrlib::serialization::tDataTypeBase& port_interface);

  virtual void CustomDelete(bool b)
  {
    tReusable::CustomDelete(b);
  }

};

} // namespace finroc
} // namespace core

#endif // core__port__rpc__tMethodCall_h__
