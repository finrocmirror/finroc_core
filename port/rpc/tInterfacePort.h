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

#ifndef CORE__PORT__RPC__TINTERFACEPORT_H
#define CORE__PORT__RPC__TINTERFACEPORT_H

#include "core/portdatabase/tDataType.h"
#include "core/port/tAbstractPort.h"
#include "core/port/tMultiTypePortDataBufferPool.h"
#include "core/tFrameworkElement.h"
#include "core/port/tPortCreationInfo.h"
#include "core/port/std/tPortDataImpl.h"
#include "core/port/cc/tCCInterThreadContainer.h"
#include "core/port/tThreadLocalCache.h"

#include "finroc_core_utils/container/tSafeConcurrentlyIterableList.h"

namespace finroc
{
namespace core
{
/*!
 * \author Max Reichardt
 *
 * This is a port that can be used for remote procedure calls -
 * synchronous and asynchronous
 *
 * Server is source port.
 * Client is target port.
 * One source may have multiple targets. However, a target may only
 * have one source in order to receive only one return value.
 */
class tInterfacePort : public tAbstractPort
{
public:

  enum tType { eServer, eClient, eNetwork, eRouting };

private:

  /*! Type of interface port */
  tInterfacePort::tType type;

protected:

  /*! Edges emerging from this port */
  tAbstractPort::tEdgeList<tInterfacePort*> edges_src;

  /*! Edges ending at this port - maximum of one in this class */
  tAbstractPort::tEdgeList<tInterfacePort*> edges_dest;

public:

  // for monitor functionality
  mutable util::tMonitor monitor;

  //  /** Does port handle method calls? In this case this points to the class that will handle the method calls */
  //  private CallHandler callHandler;
  //
  //  /** Does port accept return values from asynchronous method calls? In this case, this points to the class that will handle them */
  //  private ReturnHandler returnHandler;

  /*! Pool with diverse data buffers */
  tMultiTypePortDataBufferPool* buf_pool;

private:

  /*! makes adjustment to flags passed through constructor */
  static tPortCreationInfo ProcessPci(tPortCreationInfo pci, tInterfacePort::tType type_, int lock_level);

protected:

  virtual void ClearQueueImpl()
  {
    // do nothing in interface port
  }

  virtual int GetMaxQueueLengthImpl() const
  {
    // do nothing in interface port
    return 0;
  }

  virtual int16 GetStrategyRequirement() const
  {
    return 0;
  }

  virtual void InitialPushTo(tAbstractPort* target, bool reverse)
  {
    // do nothing in interface port
  }

  virtual void RawConnectToTarget(tAbstractPort* target);

  virtual void SetMaxQueueLengthImpl(int length)
  {
    // do nothing in interface port
  }

public:

  tInterfacePort(const util::tString& description, tFrameworkElement* parent, tDataType* data_type, tInterfacePort::tType type_);

  tInterfacePort(const util::tString& description, tFrameworkElement* parent, tDataType* data_type, tInterfacePort::tType type_, int custom_flags);

  tInterfacePort(const util::tString& description, tFrameworkElement* parent, tDataType* data_type, tInterfacePort::tType type_, int custom_flags, int lock_level);

  tInterfacePort(tPortCreationInfo pci, tInterfacePort::tType type_, int lock_level);

  virtual ~tInterfacePort();

  /*!
   * (Usually called on client ports)
   *
   * \return "Server" Port that handles method call - either InterfaceServerPort or InterfaceNetPort (the latter if we have remote server)
   */
  tInterfacePort* GetServer();

  /*!
   * \return type of interface port
   */
  inline tInterfacePort::tType GetType()
  {
    return type;
  }

  //  /**
  //   * (low-level method - only use when you know what you're doing)
  //   *
  //   * Perform asynchronous method call with parameters in
  //   * specified method call buffer.
  //   *
  //   * MethodCall buffer will be unlocked and recycled by receiver
  //   *
  //   * \param mc Filled Method call buffer
  //   * \return Method call result - may be the same as parameter
  //   */
  //  protected void asynchMethodCall(MethodCall mc) {
  //      mc.setupAsynchCall();
  //      mc.pushCaller(this);
  //      mc.alreadyDeferred = false;
  //      sendMethodCall(mc);
  //  }
  //
  //  /**
  //   * Return from synchronous method call
  //   *
  //   * \param mc Method call data
  //   */
  //  @NonVirtual protected void returnValue(MethodCall mc) {
  //      if (mc.callerStackSize() > 0) {
  //
  //          // return value to network port
  //          mc.returnToCaller();
  //
  //      } else if (mc.getStatus() == MethodCall.SYNCH_RETURN || mc.getStatus() == MethodCall.CONNECTION_EXCEPTION) {
  //
  //          SynchMethodCallLogic.handleMethodReturn(mc);
  //
  //      } else if (mc.getStatus() == MethodCall.ASYNCH_RETURN) {
  //
  //          handleAsynchReturn(mc, false);
  //      }
  //  }
  //
  //  // These methods should typically not be called by subclasses
  //
  //  /**
  //   * Receive method call (synch and asynch) - either forward or handle it
  //   *
  //   * \param mc Method call
  //   */
  //  @Inline
  //  protected void receiveMethodCall(MethodCall mc) {
  //      if (callHandler != null) {
  //          handleCall(mc, false);
  //      } else {
  //          sendMethodCall(mc);
  //      }
  //  }
  //
  //  /**
  //   * Handle method call (current or deferred)
  //   *
  //   * \param mc Method call
  //   * \param deferredCall Is this a deferred call?
  //   * \return Was call deferred?
  //   */
  //  protected boolean handleCall(MethodCall mc, boolean deferredCall) {
  //      mc.autoRecycleTParam1 = true;
  //      mc.autoRecycleTParam2 = true;
  //      //mc.curServerPort = this;
  //      mc.returnValueSet = false;
  //      mc.defer = false;
  //      mc.call(callHandler, deferredCall);
  //      if (!mc.defer) {
  //          if (mc.rType == MethodCall.NONE) {
  //              mc.recycleComplete();
  //              assert(!mc.returnValueSet);
  //          } else {
  //              mc.recycleParams();
  //              assert(mc.returnValueSet);
  //              mc.setStatus(mc.getStatus() == MethodCall.SYNCH_CALL ? MethodCall.SYNCH_RETURN : MethodCall.ASYNCH_RETURN);
  //              returnValue(mc);
  //          }
  //      }
  //      return mc.defer;
  //  }
  //
  //  /**
  //   * Handle method call (current or deferred)
  //   *
  //   * \param mc Method call
  //   * \param deferredCall Deferred return call
  //   * \return Was call deferred?
  //   */
  //  protected boolean handleAsynchReturn(MethodCall mc, boolean deferredCall) {
  //      mc.autoRecycleRetVal = true;
  //      //mc.curServerPort = this;
  //      mc.defer = false;
  //      returnHandler.handleMethodReturn(mc, mc.getMethodID(), mc.ri, mc.rd, mc.rt);
  //      if (mc.defer) {
  //          assert(mc.returnValueSet == false);
  //      } else {
  //          mc.recycleComplete();
  //      }
  //      return mc.defer;
  //  }
  //
  //  /**
  //   * Send/forward method call (synch and asynch)
  //   *
  //   * \param mc Method call data
  //   */
  //  @NonVirtual protected void sendMethodCall(MethodCall mc) {
  //      @Ptr ArrayWrapper<InterfacePort> it = edgesDest.getIterable();
  //      for (@SizeT int i = 0, n = it.size(); i < n; i++) {
  //          InterfacePort ip = (InterfacePort)it.get(i);
  //          if (ip != null) {
  //              ip.receiveMethodCall(mc);
  //              return;
  //          }
  //      }
  //
  //      // return NULL if not connected
  //      if (mc.getStatus() == MethodCall.SYNCH_CALL) {
  //          mc.setStatus(MethodCall.CONNECTION_EXCEPTION);
  //          mc.autoRecycleTParam1 = true;
  //          mc.autoRecycleTParam2 = true;
  //          mc.recycleParams();
  //          returnValue(mc);
  //      } else {
  //          mc.genericRecycle();
  //      }
  //  }

  /*!
   * (for non-cc types only)
   * \param dt Data type of object to get buffer of
   * \return Unused buffer of type
   */
  virtual tPortData* GetUnusedBuffer(tDataType* dt);

  /*!
   * (for cc types only)
   * \param dt Data type of object to get buffer of
   * \return Unused buffer of type
   */
  inline tCCInterThreadContainer<>* GetUnusedCCBuffer(tDataType* dt)
  {
    assert((dt->IsCCType()));
    return tThreadLocalCache::Get()->GetUnusedInterThreadBuffer(dt);
  }

  //  protected void setReturnHandler(ReturnHandler rh) {
  //      assert(returnHandler == null);
  //      returnHandler = rh;
  //  }
  //
  //  protected void setCallHandler(CallHandler ch) {
  //      assert(callHandler == null);
  //      callHandler = ch;
  //  }

  virtual void NotifyDisconnect()    /* don't do anything here... only in network ports */
  {
  }

  //  @Override
  //  public TypedObject universalGetAutoLocked() {
  //      System.out.println("warning: cannot get current value from interface port");
  //      return null;
  //  }

  //  @Override
  //  public void invokeCall(MethodCall call) {
  //      call.pushCaller(this);
  //      sendMethodCall(call);
  //  }

  virtual void SetMaxQueueLength(int length)
  {
    throw util::tRuntimeException("InterfacePorts do not have a queue");
  }

};

} // namespace finroc
} // namespace core

#endif // CORE__PORT__RPC__TINTERFACEPORT_H
