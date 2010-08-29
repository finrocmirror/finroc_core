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

#ifndef CORE__PORT__NET__TNETPORT_H
#define CORE__PORT__NET__TNETPORT_H

#include "core/port/tAbstractPort.h"
#include "core/port/tPortCreationInfo.h"
#include "core/admin/tAdminClient.h"
#include "core/port/rpc/tAbstractCall.h"
#include "core/port/rpc/tSynchMethodCallLogic.h"
#include "core/port/std/tPortBase.h"
#include "core/port/std/tPortDataImpl.h"
#include "core/port/cc/tCCPortBase.h"
#include "core/port/cc/tCCPortData.h"
#include "core/buffers/tCoreInput.h"
#include "core/buffers/tCoreOutput.h"
#include "finroc_core_utils/log/tLogUser.h"
#include "core/port/std/tPortListener.h"
#include "core/port/cc/tCCPortListener.h"
#include "core/port/rpc/tPullCall.h"
#include "core/port/cc/tCCPortDataContainer.h"
#include "core/tCoreFlags.h"
#include "core/port/cc/tCCPullRequestHandler.h"
#include "core/port/rpc/tCallable.h"
#include "core/port/tPortFlags.h"
#include "core/port/std/tPullRequestHandler.h"
#include "core/port/rpc/tMethodCall.h"
#include "core/port/rpc/tInterfaceNetPort.h"

namespace finroc
{
namespace core
{
/*!
 * \author Max Reichardt
 *
 * Port that is used for networking.
 * Uniform wrapper class for Std, CC, and Interface ports.
 */
class tNetPort : public util::tLogUser, public tPortListener<>, public tCCPortListener<>
{
public:
  class tCCNetPort; // inner class forward declaration
public:
  class tStdNetPort; // inner class forward declaration
public:
  class tInterfaceNetPortImpl; // inner class forward declaration
private:

  /*! Wrapped port */
  tAbstractPort* wrapped;

protected:

  /*! TCPServerConnection or RemoteServer instance that this port belongs to */
  ::finroc::util::tObject* belongs_to;

  /*! Handle of Remote port */
  int remote_handle;

public:

  /*! Default timeout for pulling data over the net */
  static const int cPULL_TIMEOUT = 1000;

  /*! Last time the value was updated (used to make sure that minimum update interval is kept) */
  int64 last_update;

protected:

  /*!
   * Called whenever connection was removed
   */
  virtual void ConnectionRemoved()
  {
  }

  /*!
   * Called whenever connection was established
   */
  virtual void NewConnection()
  {
  }

  /*!
   * Notify port of disconnect
   */
  virtual void NotifyDisconnect()
  {
  }

  /*!
   * Called whenever current value of port has changed
   */
  virtual void PortChanged()
  {
  }

  /*!
   * Initializes this runtime element.
   * The tree structure should be established by now (Uid is valid)
   * so final initialization can be made.
   *
   * Called before children are initialized
   */
  virtual void PostChildInit()
  {
  }

  // Overridable methods from ports
  /*!
   * Initializes this runtime element.
   * The tree structure should be established by now (Uid is valid)
   * so final initialization can be made.
   *
   * Called before children are initialized
   */
  virtual void PreChildInit()
  {
  }

  /*!
   * Prepares element for deletion.
   * Port, for instance, are removed from edge lists etc.
   * The final deletion will be done by the GarbageCollector thread after
   * a few seconds (to ensure no other thread is working on this object
   * any more).
   */
  virtual void PrepareDelete()
  {
  }

  virtual void PropagateStrategyOverTheNet() = 0;

  virtual void SendCall(tAbstractCall* mc) = 0;

public:

  tNetPort(tPortCreationInfo pci, util::tObject* belongs_to_);

  /*!
   * \return Returns admin interface for this element - or null if there's no such interface
   */
  virtual tAdminClient* GetAdminInterface() = 0;

  /*!
   * \return TCPServerConnection or RemoteServer instance that this port belongs to
   */
  inline ::finroc::util::tObject* GetBelongsTo()
  {
    return belongs_to;
  }

  /*!
   * \return Last time the value was updated (used to make sure that minimum update interval is kept)
   */
  inline int64 GetLastUpdate()
  {
    return last_update;
  }

  /*!
   * \return Wrapped port
   */
  inline tAbstractPort* GetPort()
  {
    return wrapped;
  }

  /*!
   * \return Handle of Remote port
   */
  inline int GetRemoteHandle()
  {
    return remote_handle;
  }

  //  /**
  //   * Process incoming pull call from the network
  //   *
  //   * \param mc PullCall
  //   * \return Did call immediately return with valid result? - Should often be the case
  //   */
  //  public boolean handlePullFromNet(PullCall mc) {
  //      AbstractPort ap = getPort();
  //      if (ap instanceof StdNetPort) {
  //          return ((StdNetPort)ap).handlePullFromNet(mc);
  //      } else if (ap instanceof CCNetPort) {
  //          return ((CCNetPort)ap).handlePullFromNet(mc);
  //      } else {
  //          throw new RuntimeException("Port can't handle call from net");
  //      }
  //  }
  //
  //  /**
  //   * Process incoming (pull) returning call from the network
  //   *
  //   * \param mc Call
  //   */
  //  public void handleCallReturnFromNet(AbstractCall mc) {
  //      SynchMethodCallLogic.handleMethodReturn(mc);
  //      AbstractPort ap = getPort();
  //      if (ap instanceof StdNetPort) {
  //          ((StdNetPort)ap).handleCallReturnFromNet(mc);
  //      } else if (ap instanceof CCNetPort) {
  //          ((CCNetPort)ap).handleCallReturnFromNet(mc);
  //      } else if (ap instanceof InterfaceNetPortImpl) {
  //          ((InterfaceNetPortImpl)ap).handleCallReturnFromNet(mc);
  //      } else {
  //          throw new RuntimeException("Port can't handle call return from net");
  //      }
  //  }

  /*!
   * Process incoming (pull) returning call from the network
   *
   * \param mc Call
   */
  inline void HandleCallReturnFromNet(tAbstractCall* mc)
  {
    tSynchMethodCallLogic::HandleMethodReturn(mc);
  }

  /*! Delete port */
  inline void ManagedDelete()
  {
    GetPort()->ManagedDelete();
  }

  virtual void PortChanged(tPortBase* origin, const tPortData* value)
  {
    PortChanged();
  }

  virtual void PortChanged(tCCPortBase* origin, const tCCPortData* value)
  {
    PortChanged();
  }

  /*!
   * Process incoming strategy update from the net
   *
   * \param new strategy
   */
  virtual void PropagateStrategyFromTheNet(int16 strategy);

  /*!
   * Decode incoming data from stream
   *
   * \param ci Stream
   * \param timestamp Time stamp
   */
  void ReceiveDataFromStream(tCoreInput* ci, int64 timestamp, int8 changed_flag);

  virtual void SendCallReturn(tAbstractCall* mc) = 0;

  /*!
   * \param last_update Last time the value was updated (used to make sure that minimum update interval is kept)
   */
  inline void SetLastUpdate(int64 last_update_)
  {
    this->last_update = last_update_;
  }

  /*!
   * \param remote_handle Handle of Remote port
   */
  inline void SetRemoteHandle(int remote_handle_)
  {
    this->remote_handle = remote_handle_;
  }

  void UpdateFlags(int flags);

  /*!
   * Write data to stream
   *
   * \param co Stream
   * \param start_time Time stamp
   */
  void WriteDataToNetwork(tCoreOutput* co, int64 start_time);

public:

  /*!
   * Wrapped cc port
   */
  class tCCNetPort : public tCCPortBase, public tCCPullRequestHandler, public tCallable<tPullCall>
  {
  private:

    // Outer class NetPort
    tNetPort* const outer_class_ptr;

  protected:

    //      public boolean handlePullFromNet(PullCall mc) {
    //          return super.pullValueRaw(mc, ThreadLocalCache.getFast());
    //      }
    //
    //      public void handleCallReturnFromNet(AbstractCall mc) {
    //          super.handleCallReturn(mc);
    //      }

    virtual void ConnectionRemoved(tAbstractPort* partner)
    {
      outer_class_ptr->ConnectionRemoved();
    }

    virtual void InitialPushTo(tAbstractPort* target, bool reverse);

    virtual void NewConnection(tAbstractPort* partner)
    {
      outer_class_ptr->NewConnection();
    }

    virtual void PostChildInit()
    {
      ::finroc::core::tFrameworkElement::PostChildInit();
      outer_class_ptr->PostChildInit();
    }

    virtual void PreChildInit()
    {
      ::finroc::core::tFrameworkElement::PreChildInit();
      outer_class_ptr->PreChildInit();
    }

    virtual void PrepareDelete();

  public:

    tCCNetPort(tNetPort* const outer_class_ptr_, tPortCreationInfo pci);

    virtual tNetPort* AsNetPort()
    {
      return outer_class_ptr;
    }

    virtual void InvokeCall(tPullCall* call)
    {
      outer_class_ptr->SendCall(call);
    }

    virtual void NotifyDisconnect()
    {
      ::finroc::core::tCCPortBase::NotifyDisconnect();
      outer_class_ptr->NotifyDisconnect();
    }

    //      @Override
    //      public boolean pullValueRaw(PullCall call, ThreadLocalCache tc) {
    //          if (isOutputPort()) { // pull to forward over the network?
    //              assert(!call.isReturning(true));
    //              sendCall(call);
    //              return false;
    //          } else { // returning pull call
    //              assert(call.isReturning(true));
    //              return super.pullValueRaw(call, tc);
    //          }
    //      }
    //
    //      @Override
    //      public void handleCallReturn(AbstractCall call) {
    //          if (isOutputPort()) {
    //              assert(call.isReturning(true));
    //              super.handleCallReturn(call);
    //          } else {
    //              assert(!call.isReturning(true));
    //              NetPort.this.sendCallReturn(call);
    //          }
    //      }

    virtual bool PropagateStrategy(tAbstractPort* push_wanter, tAbstractPort* new_connection_partner);

    //      @Override
    //      protected short getStrategyRequirement() {
    //          if (isOutputPort()) {
    //              return 0; // should be zero - since it's a proxy
    //          } else {
    //              return getStrategy();
    //          }
    //      }

    void PropagateStrategy(int16 strategy);

    void PublishFromNet(tCCPortDataContainer<>* read_object, int8 changed_flag);

    virtual void PullRequest(tCCPortBase* origin, void* result_buffer);

    //      protected void setCallHandler(CallHandler ch) {
    //          setCallHandler(ch);
    //      }

    inline void UpdateFlags(int flags)
    {
      SetFlag(flags & tCoreFlags::cNON_CONSTANT_FLAGS);
    }

  };

public:

  /*!
   * Wrapped standard port
   */
  class tStdNetPort : public tPortBase, public tPullRequestHandler, public tCallable<tPullCall>
  {
  private:

    // Outer class NetPort
    tNetPort* const outer_class_ptr;

  protected:

    //      public boolean handlePullFromNet(PullCall mc) {
    //          return super.pullValueRaw(mc);
    //      }
    //
    //      public void handleCallReturnFromNet(AbstractCall mc) {
    //          SynchMethodCallLogic.
    //      }

    virtual void ConnectionRemoved(tAbstractPort* partner)
    {
      outer_class_ptr->ConnectionRemoved();
    }

    virtual void InitialPushTo(tAbstractPort* target, bool reverse);

    virtual void NewConnection(tAbstractPort* partner)
    {
      outer_class_ptr->NewConnection();
    }

    virtual void PostChildInit()
    {
      ::finroc::core::tFrameworkElement::PostChildInit();
      outer_class_ptr->PostChildInit();
    }

    virtual void PreChildInit()
    {
      ::finroc::core::tFrameworkElement::PreChildInit();
      outer_class_ptr->PreChildInit();
    }

    virtual void PrepareDelete();

  public:

    tStdNetPort(tNetPort* const outer_class_ptr_, tPortCreationInfo pci);

    virtual tNetPort* AsNetPort()
    {
      return outer_class_ptr;
    }

    virtual void InvokeCall(tPullCall* call)
    {
      outer_class_ptr->SendCall(call);
    }

    virtual void NotifyDisconnect()
    {
      ::finroc::core::tPortBase::NotifyDisconnect();
      outer_class_ptr->NotifyDisconnect();
    }

    //      @Override
    //      public boolean pullValueRaw(PullCall call) {
    //          if (isOutputPort()) { // pull to forward over the network?
    //              assert(!call.isReturning(true));
    //              sendCall(call);
    //              return false;
    //          } else { // returning pull call
    //              assert(call.isReturning(true));
    //              return super.pullValueRaw(call);
    //          }
    //      }
    //
    //      @Override
    //      public void handleCallReturn(AbstractCall call) {
    //          if (isOutputPort()) {
    //              assert(call.isReturning(true));
    //              super.handleCallReturn(call);
    //          } else {
    //              assert(!call.isReturning(true));
    //              NetPort.this.sendCallReturn(call);
    //          }
    //      }

    virtual bool PropagateStrategy(tAbstractPort* push_wanter, tAbstractPort* new_connection_partner);

    //      @Override
    //      protected short getStrategyRequirement() {
    //          if (isOutputPort()) {
    //              return 0; // should be zero - since it's a proxy
    //          } else {
    //              return getStrategy();
    //          }
    //      }

    inline void PropagateStrategy(int16 strategy)
    {
      SetFlag(tPortFlags::cPUSH_STRATEGY, strategy > 0);
      ::finroc::core::tAbstractPort::SetMaxQueueLength(strategy);
    }

    void PublishFromNet(tPortData* read_object, int8 changed_flag);

    virtual const tPortData* PullRequest(tPortBase* origin, int8 add_locks);

    inline void UpdateFlags(int flags)
    {
      SetFlag(flags & tCoreFlags::cNON_CONSTANT_FLAGS);
    }

  };

public:

  /*!
   * Wrapped standard port
   */
  class tInterfaceNetPortImpl : public tInterfaceNetPort, public tCallable<tMethodCall>
  {
  private:

    // Outer class NetPort
    tNetPort* const outer_class_ptr;

  protected:

    //      @Override
    //      protected boolean handleCall(MethodCall mc, boolean deferredCall) {
    //          NetPort.this.handleCall(mc);
    //          return false;
    //      }
    //
    //      @Override
    //      public void returnValue(MethodCall mc) {
    //          super.returnValue(mc);
    //      }
    //
    //      @Override
    //      public void sendMethodCall(MethodCall mc) {
    //          super.sendMethodCall(mc);
    //      }
    //
    //      public void handleCallReturnFromNet(AbstractCall mc) {
    //          SynchMethodCallLogic.handleMethodReturn(mc);
    //      }
    //
    //      @Override
    //      public void handleMethodCall(MethodCall mc, byte methodId,
    //              boolean deferred, long int1, long int2, long int3, double dbl1,
    //              double dbl2, double dbl3, TypedObject obj1, TypedObject obj2) {
    //          assert(false) : "Programming error: should not be called";
    //      }

    virtual void ConnectionRemoved(tAbstractPort* partner)
    {
      outer_class_ptr->ConnectionRemoved();
    }

    virtual void NewConnection(tAbstractPort* partner)
    {
      outer_class_ptr->NewConnection();
    }

    virtual void PostChildInit()
    {
      ::finroc::core::tFrameworkElement::PostChildInit();
      outer_class_ptr->PostChildInit();
    }

    virtual void PreChildInit()
    {
      ::finroc::core::tFrameworkElement::PreChildInit();
      outer_class_ptr->PreChildInit();
    }

    //      public void receiveDataFromStream(CoreInput ci) {
    //          MethodCall mc = (MethodCall)ci.readObject();
    //          if (!mc.isReturning(true)) {
    //              mc.pushCaller(belongsTo);
    //              this.sendMethodCall(mc);
    //          } else {
    //              mc.returnToCaller();
    //          }
    //      }

    //      @Override
    //      protected void setCallHandler(CallHandler ch) {
    //          super.setCallHandler(ch);
    //      }

    virtual void PrepareDelete()
    {
      util::tLock lock3(this);
      ::finroc::core::tAbstractPort::PrepareDelete();
      outer_class_ptr->PrepareDelete();
    }

  public:

    tInterfaceNetPortImpl(tNetPort* const outer_class_ptr_, tPortCreationInfo pci);

    virtual tNetPort* AsNetPort()
    {
      return outer_class_ptr;
    }

    virtual void InvokeCall(tMethodCall* call)
    {
      outer_class_ptr->SendCall(call);
    }

    virtual void NotifyDisconnect()
    {
      ::finroc::core::tInterfacePort::NotifyDisconnect();
      outer_class_ptr->NotifyDisconnect();
    }

    virtual void SendAsyncCall(tMethodCall* mc)
    {
      mc->SetupAsynchCall();
      outer_class_ptr->SendCall(mc);
    }

    virtual void SendSyncCallReturn(tMethodCall* mc)
    {
      outer_class_ptr->SendCall(mc);
    }

    virtual tMethodCall* SynchCallOverTheNet(tMethodCall* mc, int timeout)
    {
      assert((mc->GetMethod() != NULL));
      return tSynchMethodCallLogic::PerformSynchCall(mc, this, timeout);
    }

    inline void UpdateFlags(int flags)
    {
      SetFlag(flags & tCoreFlags::cNON_CONSTANT_FLAGS);
    }

  };

  /*!
   * \return Sources of remote edges
   */
  //@JavaOnly
  //public abstract List<FrameworkElement> getRemoteEdgeSources();

};

} // namespace finroc
} // namespace core

#endif // CORE__PORT__NET__TNETPORT_H
