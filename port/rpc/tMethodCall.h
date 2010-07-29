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

#ifndef CORE__PORT__RPC__TMETHODCALL_H
#define CORE__PORT__RPC__TMETHODCALL_H

#include "core/portdatabase/tDataType.h"
#include "core/port/rpc/method/tAbstractMethodCallHandler.h"
#include "core/port/rpc/method/tAbstractAsyncReturnHandler.h"
#include "core/buffers/tCoreInput.h"
#include "core/buffers/tCoreOutput.h"
#include "core/port/rpc/tAbstractCall.h"
#include "finroc_core_utils/thread/tTask.h"

namespace finroc
{
namespace core
{
class tInterfaceNetPort;
class tAbstractMethod;

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

  //  /** Maximum size of caller stack */
  //  private final static int MAX_CALL_DEPTH = 4;
  //
  //  //private final TypedObject[] ccBufferRepository;
  //
  //  /** Parameters */
  //  private byte tCount, iCount, dCount; // number of parameters of each type
  //  @InCpp("TypedObject* tParams[2];")
  //  private final TypedObject[] tParams = new TypedObject[2];
  //  @InCpp("int64 iParams[3];")
  //  private final long[] iParams = new long[3];
  //  @InCpp("double dParams[3];")
  //  private final double[] dParams = new double[3];
  //
  //  /** Type of return value */
  //  public static final byte RETURN_OBJ = 1, RETURN_INT = 2, RETURN_DOUBLE = 3;
  //  protected byte rType;
  //
  //  /** Return value - depending on type */
  //  protected TypedObject rt; // either PortData or CCInterthreadContainer
  //  protected long ri;
  //  protected double rd;

  //  /** method ID of call */
  //  private byte methodID;

  //  /** Data type of method */
  //  public static DataType METHOD_TYPE;

  // Temporary data for processing and checking requests - no need to transfer over the network
  /*! Server port that processes current request */
  //InterfacePort curServerPort;

  //  /** Has return value been set by server port? */
  //  boolean returnValueSet;
  //
  //  /** Automatically recycle these objects after call/return? */
  //  boolean autoRecycleTParam1, autoRecycleTParam2, autoRecycleRetVal;
  //
  //  /** Defer method call? */
  //  boolean defer;
  //
  //  /** Has method call been deferred at least once? */
  //  boolean alreadyDeferred;
  //
  //  /** Time when method call arrived - optional */
  //  private long arrivalTime;

  //  public static void staticInit() {
  //      // JavaOnlyBlock
  //      METHOD_TYPE = DataTypeRegister.getInstance().addDataType(MethodCall.class);
  //
  //       cMETHOD_TYPE = tDataTypeRegister::GetInstance()->AddDataType<tMethodCall>("MethodCall");
  //  }

  /*! Method that is called */
  tAbstractMethod* method;

  /*!
   * Data type of interface that method belong, too
   * (method may belong to multiple - so this is the one
   *  we wan't to actually use)
   */
  tDataType* port_interface_type;

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
  void DeserializeCall(tCoreInput* is, tDataType* dt, bool skip_parameters);

  virtual void ExecuteTask();

  //  /**
  //   * Read object from strem
  //   * (helper method for convenience - ensures that object has a lock)
  //   *
  //   * \param ci Input stream
  //   */
  //  public TypedObject readObject(CoreInput ci) {
  //      TypedObject result = ci.readObjectInInterThreadContainer();
  //      if (result != null && result.getType().isStdType()) {
  //          ((PortData)result).getManager().getCurrentRefCounter().setLocks((byte)1);
  //      }
  //      return result;
  //  }

  //  /**
  //   * (for call-handler) don't unlock/recycle parameter 1
  //   */
  //  public void dontRecycleParam1() {
  //      autoRecycleTParam1 = false;
  //  }
  //
  //  /**
  //   * (for call-handler) don't unlock/recycle parameter 2
  //   */
  //  public void dontRecycleParam2() {
  //      autoRecycleTParam2 = false;
  //  }
  //
  //  /**
  //   * (for return handler) don't recycle return value
  //   */
  //  public void dontRecycleReturnValue() {
  //      autoRecycleRetVal = false;
  //  }
  //
  //  @Inline void deferCall(boolean logArrivalTime) {
  //      if (logArrivalTime && (!alreadyDeferred)) {
  //          setArrivalTime();
  //      }
  //      defer = true;
  //      alreadyDeferred = true;
  //  }
  //
  //  /**
  //   * Recycle/unlock parameters (provided they are not set not to be recycled)
  //   */
  //  void recycleParams() {
  //      if (autoRecycleTParam1) {
  //          recycleParam(tParams[0]);
  //      }
  //      tParams[0] = null;
  //      if (autoRecycleTParam2) {
  //          recycleParam(tParams[1]);
  //      }
  //      tParams[1] = null;
  //  }
  //
  //  private void recycleParam(@Ptr TypedObject p) {
  //      if (p == null) {
  //          return;
  //      }
  //      if (p.getType().isCCType()) {
  //          ((CCInterThreadContainer<?>)p).recycle2();
  //      } else {
  //          ((PortData)p).getManager().getCurrentRefCounter().releaseLock();
  //      }
  //  }
  //
  //  /**
  //   * Recycle everything... regardless over whether it has been set not to be recycled
  //   * (should only be called by network ports)
  //   */
  //  @Override
  //  public void genericRecycle() {
  //      if (isResponsible()) {
  //          recycleParam(tParams[0]);
  //          recycleParam(tParams[1]);
  //          recycleParam(rt);
  //          tParams[0] = null;
  //          tParams[1] = null;
  //          rt = null;
  //          super.recycle();
  //      }
  //  }
  //
  //  /**
  //   * Recycle method call and recycle/unlock any objects (provided they are not set not to be recycled)
  //   */
  //  void recycleComplete() {
  //      recycleParams();
  //      if (autoRecycleRetVal) {
  //          recycleParam(rt);
  //      }
  //      rt = null;
  //      super.recycle();
  //  }

  //  @Inline
  //  protected void setupCall(byte retValueType, byte methodId2, long int1, long int2, long int3, double dbl1, double dbl2, double dbl3, TypedObject obj1, boolean lockedOrCopied1, TypedObject obj2, boolean lockedOrCopied2) {
  //      // parameters
  //      methodID = methodId2;
  //      iCount = (byte)(int1 == 0 ? 0 : (int2 == 0 ? 1 : (int3 == 0 ? 2 : 3)));
  //      dCount = (byte)(dbl1 == 0 ? 0 : (dbl2 == 0 ? 1 : (dbl3 == 0 ? 2 : 3)));
  //      tCount = (byte)(obj1 == null ? 0 : (obj2 == null ? 1 : 2));
  //      iParams[0] = int1;
  //      iParams[1] = int2;
  //      iParams[2] = int3;
  //      dParams[0] = dbl1;
  //      dParams[1] = dbl2;
  //      dParams[2] = dbl3;
  //      setTParam(0, obj1, lockedOrCopied1);
  //      setTParam(1, obj2, lockedOrCopied2);
  //
  //      // return values
  //      ri = 0;
  //      rd = 0;
  //      rt = null;
  //
  //      // other stuff
  //      rType = retValueType;
  //      arrivalTime = 0;
  //  }
  //
  //  private void setTParam(@SizeT int i, @Ptr TypedObject obj, boolean lockedOrCopied) {
  //      if (obj != null) {
  //          if (!obj.getType().isCCType()) {
  //              PortData tmp = (PortData)obj;
  //              if (!lockedOrCopied) {
  //                  tmp.getCurReference().getRefCounter().setOrAddLock();
  //              } else {
  //                  assert(tmp.getCurReference().getRefCounter().isLocked());
  //              }
  //          } else { // cc type
  //              @Ptr CCContainerBase cb = (CCContainerBase)obj;
  //              if (cb.isInterThreadContainer()) {
  //                  @Ptr CCInterThreadContainer<?> ic = (CCInterThreadContainer<?>)cb;
  //                  if (!lockedOrCopied) {
  //                      @Ptr CCInterThreadContainer<?> citc = ThreadLocalCache.get().getUnusedInterThreadBuffer(obj.getType());
  //                      citc.assign(ic.getDataPtr());
  //                      obj = citc;
  //                  }
  //              } else {
  //                  @Ptr CCPortDataContainer<?> ic = (CCPortDataContainer<?>)cb;
  //                  @Ptr CCInterThreadContainer<?> citc = ThreadLocalCache.get().getUnusedInterThreadBuffer(obj.getType());
  //                  citc.assign(ic.getDataPtr());
  //                  obj = citc;
  //              }
  //          }
  //      }
  //      tParams[i] = obj;
  //  }
  //
  //  /**
  //   * \return Integer return value
  //   */
  //  protected long getReturnInt() {
  //      assert(rType == RETURN_INT);
  //      return ri;
  //  }
  //
  //  /**
  //   * \return Double return value
  //   */
  //  protected double getReturnDouble() {
  //      assert(rType == RETURN_DOUBLE);
  //      return rd;
  //  }
  //
  //  /**
  //   * \return Object return value (locked)
  //   */
  //  protected TypedObject getReturnObject() {
  //      assert(rType == RETURN_OBJ);
  //      return rt;
  //  }
  //
  //  /**
  //   * \param r Integer return value
  //   */
  //  public void setReturn(int r) {
  //      assert(rType == RETURN_INT);
  //      returnValueSet = true;
  //      ri = r;
  //  }
  //
  //  /**
  //   * \param r Double return value
  //   */
  //  public void setReturn(double r) {
  //      assert(rType == RETURN_DOUBLE);
  //      returnValueSet = true;
  //      rd = r;
  //  }
  //
  //  /**
  //   * Return null value
  //   */
  //  public void setReturnNull() {
  //      assert(rType == RETURN_OBJ);
  //      returnValueSet = true;
  //      rt = null;
  //  }
  //
  //  /**
  //   * \param r Object return value (locked)
  //   * \param locked Has return value already been locked? (so that it can be automatically unlocked by this class)
  //   */
  //  public void setReturn(PortData obj, boolean locked) {
  //      assert(rType == RETURN_OBJ);
  //      returnValueSet = true;
  //      rt = obj;
  //      if (obj == null) {
  //          return;
  //      }
  //      if (!locked) {
  //          obj.getCurReference().getRefCounter().setOrAddLock();
  //      } else {
  //          assert(obj.getCurReference().getRefCounter().isLocked());
  //      }
  //  }
  //
  //  /**
  //   * \param obj Object return value (will be copied)
  //   */
  //  public void setReturn(CCPortDataContainer<?> obj) {
  //      assert(rType == RETURN_OBJ);
  //      if (obj == null) {
  //          setReturnNull();
  //          return;
  //      }
  //      CCInterThreadContainer<?> citc = ThreadLocalCache.get().getUnusedInterThreadBuffer(obj.getType());
  //      citc.assign(obj.getDataPtr());
  //      setReturn(citc, true);
  //  }
  //
  //  /**
  //   * \param obj Object return value
  //   * \param extraCopy Is this already an extra copy that can be recycled automatically?
  //   */
  //  public void setReturn(CCInterThreadContainer<?> obj, boolean extraCopy) {
  //      assert(rType == RETURN_OBJ);
  //      returnValueSet = true;
  //      if (obj == null) {
  //          rt = null;
  //          return;
  //      }
  //      if (!extraCopy) {
  //          CCInterThreadContainer<?> citc = ThreadLocalCache.get().getUnusedInterThreadBuffer(obj.getType());
  //          citc.assign(obj.getDataPtr());
  //          rt = citc;
  //      } else {
  //          rt = obj;
  //      }
  //  }
  //
  //  /**
  //   * Perform method call on specified call handler
  //   *
  //   * \param callHandler Call Handler
  //   * \param deferred Is this a deferred call?
  //   */
  //  @Inline public void call(CallHandler callHandler, boolean deferred) {
  //      callHandler.handleMethodCall(this, methodID, deferred, iParams[0], iParams[1], iParams[2], dParams[0], dParams[1], dParams[2], tParams[0], tParams[1]);
  //  }
  //
  //  /**
  //   * \return the arrivalTime
  //   */
  //  public long getArrivalTime() {
  //      return arrivalTime;
  //  }
  //
  //  /**
  //   * \param arrivalTime the arrivalTime to set
  //   */
  //  public void setArrivalTime() {
  //      if (arrivalTime == 0) {
  //          arrivalTime = Time.getCoarse();
  //      }
  //  }
  //
  //  /**
  //   * \param arrivalTime the arrivalTime to set
  //   */
  //  public void setArrivalTime(long time) {
  //      arrivalTime = time;
  //  }

  virtual void GenericRecycle()
  {
    Recycle();
  }

  //  @Override @JavaOnly
  //  public DataType getType() {
  //      return METHOD_TYPE;
  //  }

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
  inline tDataType* GetPortInterfaceType()
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
  void PrepareExecution(tAbstractMethod* method_, tDataType* port_interface, tAbstractMethodCallHandler* handler_, tAbstractAsyncReturnHandler* ret_handler_);

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
  void PrepareSyncRemoteExecution(tAbstractMethod* method_, tDataType* port_interface, tAbstractAsyncReturnHandler* ret_handler_, tInterfaceNetPort* net_port_, int net_timeout_);

  /*!
   * Prepare method call object for blocking remote execution in same thread (as a task)
   *
   * \param method Method that is to be called
   * \param port_interface Data type of interface that method belongs to
   * \param net_timeout Network timeout in ms for call
   */
  void PrepareSyncRemoteExecution(tAbstractMethod* method_, tDataType* port_interface, int net_timeout_);

  void Recycle();

  virtual void Serialize(tCoreOutput& oos) const;

  /*!
   * \param m The Method that will be called (may not be changed - to avoid ugly programming errors)
   * \param port_interface Data type of interface that method belongs to
   */
  void SetMethod(tAbstractMethod* m, tDataType* port_interface);

};

} // namespace finroc
} // namespace core

#endif // CORE__PORT__RPC__TMETHODCALL_H
