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

#ifndef core__port__net__tNetPort_h__
#define core__port__net__tNetPort_h__

#include "rrlib/finroc_core_utils/definitions.h"

#include "core/port/rpc/tPullCall.h"
#include "core/port/tPortCreationInfoBase.h"
#include "core/port/rpc/tSynchMethodCallLogic.h"
#include "core/portdatabase/tFinrocTypeInfo.h"
#include "core/port/tAbstractPort.h"
#include "core/port/tPortListener.h"
#include "core/tFrameworkElement.h"
#include "core/tCoreFlags.h"
#include "core/tFinrocAnnotation.h"
#include "core/port/cc/tCCPortBase.h"
#include "core/port/cc/tCCPullRequestHandler.h"
#include "core/port/rpc/tCallable.h"
#include "core/port/tPortFlags.h"
#include "core/port/std/tPortBase.h"
#include "core/port/std/tPullRequestHandler.h"
#include "core/port/rpc/tMethodCall.h"
#include "core/port/rpc/tInterfaceNetPort.h"

namespace rrlib
{
namespace serialization
{
class tInputStream;
} // namespace rrlib
} // namespace serialization

namespace finroc
{
namespace core
{
class tAbstractCall;
class tCCPortDataManagerTL;
class tPortDataManager;

/*!
 * \author Max Reichardt
 *
 * Port that is used for networking.
 * Uniform wrapper class for Std, CC, and Interface ports.
 */
class tNetPort : public tPortListener<>, public tCallable<tPullCall>, public tFinrocAnnotation, public rrlib::rtti::tFactory
{

  /*! Wrapped port */
  tAbstractPort* wrapped;

protected:

  /*! TCPServerConnection or RemoteServer instance that this port belongs to */
  ::finroc::util::tObject* belongs_to;

  /*! Handle of Remote port */
  int remote_handle;

  /*! Finroc Type info type */
  tFinrocTypeInfo::tType ftype;

public:

  /*! Default timeout for pulling data over the net */
  static const int cPULL_TIMEOUT = 1000;

  /*! Last time the value was updated (used to make sure that minimum update interval is kept) */
  int64 last_update;

private:

  /*! Helper methods for data type type */
  inline bool IsStdType()
  {
    return ftype == tFinrocTypeInfo::eSTD;
  }

  inline bool IsUnknownType()
  {
    return ftype >= tFinrocTypeInfo::eUNKNOWN_STD;
  }

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
   * Ports, for instance, are removed from edge lists etc.
   * The final deletion will be done by the GarbageCollector thread after
   * a few seconds (to ensure no other thread is working on this object
   * any more).
   */
  virtual void PrepareDelete()
  {
  }

  virtual void PropagateStrategyOverTheNet() = 0;

  virtual void SendCall(tAbstractCall::tPtr& mc) = 0;

public:

  tNetPort(tPortCreationInfoBase pci, util::tObject* belongs_to_);

  virtual std::shared_ptr<void> CreateBuffer(const rrlib::rtti::tDataTypeBase& dt)
  {
    return std::shared_ptr<void>(dt.CreateInstance());
  }

  virtual rrlib::rtti::tGenericObject* CreateGenericObject(const rrlib::rtti::tDataTypeBase& dt, void* factory_parameter);

  /*!
   * Find network port connected to this port that belongs to specified object
   *
   * \param belongs_to Instance (usually TCPServerConnection or RemoteServer) that this port belongs to
   * \return Network port if it could be found - otherwise null
   */
  static tNetPort* FindNetPort(tAbstractPort& port, util::tObject* belongs_to);

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

  /*!
   * Process incoming (pull) returning call from the network
   *
   * \param mc Call
   */
  inline void HandleCallReturnFromNet(tAbstractCall::tPtr& mc)
  {
    tSynchMethodCallLogic::HandleMethodReturn(mc);
  }

  virtual void InvokeCall(tPullCall::tPtr& call)
  {
    tAbstractCall::tPtr tmp(std::move(call));
    SendCallReturn(tmp);
  }

  inline bool IsCCType()
  {
    return ftype == tFinrocTypeInfo::eCC;
  }

  inline bool IsMethodType()
  {
    return ftype == tFinrocTypeInfo::eMETHOD;
  }

  inline bool IsTransactionType()
  {
    return ftype == tFinrocTypeInfo::eTRANSACTION;
  }

  /*! Delete port */
  inline void ManagedDelete()
  {
    GetPort()->ManagedDelete();
  }

  virtual void PortChanged(tAbstractPort* origin, const void* const& value)
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
  void ReceiveDataFromStream(rrlib::serialization::tInputStream& ci, int64 timestamp, int8 changed_flag);

  virtual void SendCallReturn(tAbstractCall::tPtr& mc) = 0;

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

  void UpdateFlags(uint flags);

  /*!
   * Write data to stream
   *
   * \param co Stream
   * \param start_time Time stamp
   */
  void WriteDataToNetwork(rrlib::serialization::tOutputStream& co, int64 start_time);

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

    tCCNetPort(tNetPort* const outer_class_ptr_, tPortCreationInfoBase pci);

    virtual tNetPort* AsNetPort()
    {
      return outer_class_ptr;
    }

    virtual void InvokeCall(tPullCall::tPtr& call)
    {
      tAbstractCall::tPtr tmp(std::move(call));
      outer_class_ptr->SendCall(tmp);
    }

    virtual void NotifyDisconnect()
    {
      ::finroc::core::tCCPortBase::NotifyDisconnect();
      outer_class_ptr->NotifyDisconnect();
    }

    virtual bool PropagateStrategy(tAbstractPort* push_wanter, tAbstractPort* new_connection_partner);

    void PropagateStrategy(int16 strategy);

    void PublishFromNet(tCCPortDataManagerTL* read_object, int8 changed_flag);

    virtual bool PullRequest(tCCPortBase* origin, tCCPortDataManagerTL* result_buffer);

    inline void UpdateFlags(uint flags)
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

    tStdNetPort(tNetPort* const outer_class_ptr_, tPortCreationInfoBase pci);

    virtual tNetPort* AsNetPort()
    {
      return outer_class_ptr;
    }

    virtual void InvokeCall(tPullCall::tPtr& call)
    {
      tAbstractCall::tPtr tmp(std::move(call));
      outer_class_ptr->SendCall(tmp);
    }

    virtual void NotifyDisconnect()
    {
      ::finroc::core::tPortBase::NotifyDisconnect();
      outer_class_ptr->NotifyDisconnect();
    }

    virtual bool PropagateStrategy(tAbstractPort* push_wanter, tAbstractPort* new_connection_partner);

    inline void PropagateStrategy(int16 strategy)
    {
      SetFlag(tPortFlags::cPUSH_STRATEGY, strategy > 0);
      ::finroc::core::tAbstractPort::SetMaxQueueLength(strategy);
    }

    void PublishFromNet(tPortDataManager* read_object, int8 changed_flag);

    virtual const tPortDataManager* PullRequest(tPortBase* origin, int8 add_locks);

    inline void UpdateFlags(uint flags)
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

    virtual void PrepareDelete()
    {
      util::tLock lock3(this);
      ::finroc::core::tAbstractPort::PrepareDelete();
      outer_class_ptr->PrepareDelete();
    }

  public:

    tInterfaceNetPortImpl(tNetPort* const outer_class_ptr_, tPortCreationInfoBase pci);

    virtual tNetPort* AsNetPort()
    {
      return outer_class_ptr;
    }

    virtual void InvokeCall(tMethodCall::tPtr& call)
    {
      tAbstractCall::tPtr tmp(std::move(call));
      outer_class_ptr->SendCall(tmp);
    }

    virtual void NotifyDisconnect()
    {
      ::finroc::core::tInterfacePort::NotifyDisconnect();
      outer_class_ptr->NotifyDisconnect();
    }

    virtual void SendAsyncCall(tMethodCall::tPtr& mc)
    {
      mc->SetupAsynchCall();
      tAbstractCall::tPtr tmp(std::move(mc));
      outer_class_ptr->SendCall(tmp);
    }

    virtual void SendSyncCallReturn(tMethodCall::tPtr& mc)
    {
      tAbstractCall::tPtr tmp(std::move(mc));
      outer_class_ptr->SendCall(tmp);
    }

    virtual tMethodCall::tPtr SynchCallOverTheNet(tMethodCall::tPtr& mc, int timeout);

    inline void UpdateFlags(uint flags)
    {
      SetFlag(flags & tCoreFlags::cNON_CONSTANT_FLAGS);
    }

  };

};

} // namespace finroc
} // namespace core

#endif // core__port__net__tNetPort_h__
