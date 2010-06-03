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

#ifndef CORE__PORT__RPC__TINTERFACECLIENTPORT_H
#define CORE__PORT__RPC__TINTERFACECLIENTPORT_H

#include "core/portdatabase/tDataType.h"
#include "core/tFrameworkElement.h"
#include "core/port/rpc/tInterfacePort.h"

namespace finroc
{
namespace core
{
/*! Base class for client interface ports */
class tInterfaceClientPort : public tInterfacePort
{
public:

  /* implements ReturnHandler*/

  ///** Return handler */
  //private ReturnHandler handler;

  tInterfaceClientPort(const util::tString& description, tFrameworkElement* parent, tDataType* type) :
      tInterfacePort(description, parent, type, ::finroc::core::tInterfacePort::eClient)
  {
    //setReturnHandler(this);
  }

  /*!
   * \return Is server for port in remote runtime environment?
   */
  inline bool HasRemoteServer()
  {
    ::finroc::core::tInterfacePort* server = GetServer();
    return (server != NULL) && (server->GetType() == ::finroc::core::tInterfacePort::eNetwork);
  }

//  public InterfaceClientPort(String description, FrameworkElement parent, DataType type, ReturnHandler rh) {
//      super(description, parent, type, Type.Client);
//      setReturnHandler(rh);
//  }

//  /**
//   * \param rh ReturnHandler
//   */
//  public void setReturnHandler(ReturnHandler rh) {
//      handler = rh;
//  }
//
//  @Override
//  public void handleMethodReturn(MethodCall mc, byte methodId, long intRet, double dblRet, TypedObject objRet) {
//      System.out.println("Unhandled asynchronous method reply: " + mc.toString());
//  }

//  @Override
//  public void handleCallReturn(AbstractCall pc) {
//      returnValue((MethodCall)pc);
//  }
//
//  /**
//   * (low-level method - only use when you know what you're doing)
//   *
//   * Perform synchronous method call with parameters in
//   * specified method call buffer.
//   * Thread will be stuck inside this method until it
//   * returns.
//   *
//   * Caller is in charge of unlocking and therewith releasing MethodCall buffers
//   *
//   * \param mc Filled Method call buffer
//   * \param timeout Timeout in milliseconds
//   * \return Method call result - may be the same as parameter
//   */
//  protected MethodCall synchMethodCall(MethodCall mc, long timeout) throws MethodCallException {
//      assert(isReady());
//      mc.alreadyDeferred = false;
//      MethodCall ret = SynchMethodCallLogic.performSynchCall(mc, this, callIndex, timeout);
//      if (ret != null && ret.getStatus() == MethodCall.CONNECTION_EXCEPTION) { // not connected (?)
//          ret.autoRecycleRetVal = true;
//          ret.recycleComplete();
//          throw new MethodCallException(false);
//      }
//      return ret;
//  }
//
//  /**
//   * This description is more or less valid for all methods in this class
//   *
//   * \param retValueType Type of return value (e.g. MethodCall.RETURN_INT)
//   * \param methodId Unambiguous method identification (defined by concrete server/client classes)
//   * \param int1 first integer parameter (optional - defined by concrete server/client classes)
//   * \param int2 second integer parameter (optional - defined by concrete server/client classes)
//   * \param int3 third integer parameter (optional - defined by concrete server/client classes)
//   * \param dbl1 first double parameter (optional - defined by concrete server/client classes)
//   * \param dbl2 second double parameter (optional - defined by concrete server/client classes)
//   * \param dbl3 third double parameter (optional - defined by concrete server/client classes)
//   * \param obj1 first object parameter (optional - defined by concrete server/client classes)
//   * \param lockedOrCopied1 Is object parameter 1 already locked/copied (and can be unlocked/recycled by this class)
//   * \param obj2 second object parameter (optional - defined by concrete server/client classes)
//   * \param lockedOrCopied2 Is object parameter 2 already locked/copied (and can be unlocked/recycled by this class)
//   * \param autoRecycleRet Should return value automatically be recycled (relevant for synchronized object calls only)
//   * @return
//   */
//  @Inline protected MethodCall createCall(byte retValueType, byte methodId, long int1, long int2, long int3, double dbl1, double dbl2, double dbl3,
//          TypedObject obj1, boolean lockedOrCopied1, TypedObject obj2, boolean lockedOrCopied2, boolean autoRecycleRet) {
//      MethodCall mc = ThreadLocalCache.get().getUnusedMethodCall();
//      mc.setupCall(retValueType, methodId, int1, int2, int3, dbl1, dbl2, dbl3, obj1, lockedOrCopied1, obj2, lockedOrCopied2);
//      mc.autoRecycleRetVal = autoRecycleRet;
//      mc.alreadyDeferred = false;
//      return mc;
//  }
//
//  public long synchIntMethodCall(byte methodId, long int1, long int2, long int3, double dbl1, double dbl2, double dbl3,
//          TypedObject obj1, boolean lockedOrCopied1, TypedObject obj2, boolean lockedOrCopied2, long timeout) throws MethodCallException {
//      MethodCall mc = createCall(MethodCall.RETURN_INT, methodId, int1, int2, int3, dbl1, dbl2, dbl3, obj1, lockedOrCopied1, obj2, lockedOrCopied2, false);
//      //try {
//      mc = synchMethodCall(mc, timeout);
//      long ret = mc.getReturnInt();
//      mc.recycleComplete();
//      return ret;
//      //} catch (MethodCallException e) {
//      //  mc.genericRecycle();
//      //  throw e;
//      //}
//  }
//  public double synchDoubleMethodCall(byte methodId, long int1, long int2, long int3, double dbl1, double dbl2, double dbl3,
//          TypedObject obj1, boolean lockedOrCopied1, TypedObject obj2, boolean lockedOrCopied2, long timeout) throws MethodCallException {
//      MethodCall mc = createCall(MethodCall.RETURN_DOUBLE, methodId, int1, int2, int3, dbl1, dbl2, dbl3, obj1, lockedOrCopied1, obj2, lockedOrCopied2, false);
//      //try {
//      mc = synchMethodCall(mc, timeout);
//      double ret = mc.getReturnDouble();
//      mc.recycleComplete();
//      return ret;
//      //} catch (MethodCallException e) {
//      //  //mc.genericRecycle();
//      //  throw e;
//      //}
//  }
//  public TypedObject synchObjMethodCall(byte methodId, long int1, long int2, long int3, double dbl1, double dbl2, double dbl3,
//          TypedObject obj1, boolean lockedOrCopied1, TypedObject obj2, boolean lockedOrCopied2, long timeout, boolean autoRecycleRet) throws MethodCallException {
//      MethodCall mc = createCall(MethodCall.RETURN_OBJ, methodId, int1, int2, int3, dbl1, dbl2, dbl3, obj1, lockedOrCopied1, obj2, lockedOrCopied2, autoRecycleRet);
//      //try {
//      mc = synchMethodCall(mc, timeout);
//      TypedObject ret = mc.getReturnObject();
//      if (mc.autoRecycleRetVal) {
//          ThreadLocalCache.get().addAutoLock(ret);
//          mc.autoRecycleRetVal = false;
//      }
//      mc.recycleComplete();
//      return ret;
//      //} catch (MethodCallException e) {
//      //  //mc.genericRecycle();
//      //  throw e;
//      //}
//  }
//
//  public void asynchMethodCall(byte retValueType, byte methodId, long int1, long int2, long int3, double dbl1, double dbl2, double dbl3,
//          TypedObject obj1, boolean lockedOrCopied1, TypedObject obj2, boolean lockedOrCopied2) {
//      super.asynchMethodCall(createCall(retValueType, methodId, int1, int2, int3, dbl1, dbl2, dbl3, obj1, lockedOrCopied1, obj2, lockedOrCopied2, true));
//  }
//
//  // Various wrappers for the above
//  public void asynchMethodCall(byte retValueType, byte methodId, long int1, TypedObject obj1, boolean lockedOrCopied1) {
//      asynchMethodCall(retValueType, methodId, int1, 0, 0, 0, 0, 0, obj1, lockedOrCopied1, null, false);
//  }
//  public TypedObject synchObjMethodCall(byte methodId, long int1, long int2, long int3, long timeout, boolean autoRecycleRet) throws MethodCallException {
//      return synchObjMethodCall(methodId, int1, int2, int3, 0, 0, 0, null, true, null, true, timeout, autoRecycleRet);
//  }
//  public void asynchMethodCall(byte retValueType, byte methodId, TypedObject obj1, boolean lockedOrCopied1) {
//      asynchMethodCall(retValueType, methodId, 0, 0, 0, 0, 0, 0, obj1, lockedOrCopied1, null, false);
//  }
//  public TypedObject synchObjMethodCall(byte methodId, long timeout, boolean autoRecycleRet) throws MethodCallException {
//      return synchObjMethodCall(methodId, 0, 0, 0, timeout, autoRecycleRet);
//  }
//  public long synchIntMethodCall(byte methodId, long timeout) throws MethodCallException {
//      return synchIntMethodCall(methodId, 0, 0, 0, 0, 0, 0, null, false, null, false, timeout);
//  }

};

} // namespace finroc
} // namespace core

#endif // CORE__PORT__RPC__TINTERFACECLIENTPORT_H
