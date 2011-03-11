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

#ifndef core__port__rpc__tInterfacePort_h__
#define core__port__rpc__tInterfacePort_h__

#include "rrlib/finroc_core_utils/definitions.h"

#include "core/port/tAbstractPort.h"
#include "core/port/tPortCreationInfo.h"
#include "rrlib/serialization/tDataTypeBase.h"
#include "core/portdatabase/tFinrocTypeInfo.h"
#include "core/port/tThreadLocalCache.h"

namespace finroc
{
namespace core
{
class tMultiTypePortDataBufferPool;
class tPortDataManager;
class tCCPortDataManager;

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

  friend class tInterfaceClientPort;
private:

  /*! Type of interface port */
  tInterfacePort::tType type;

protected:

  /*! Edges emerging from this port */
  tAbstractPort::tEdgeList<tInterfacePort*> edges_src;

  /*! Edges ending at this port - maximum of one in this class */
  tAbstractPort::tEdgeList<tInterfacePort*> edges_dest;

public:

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

  /*!
   * Get buffer to use in method call or return (has one lock)
   *
   * (for non-cc types only)
   * \param dt Data type of object to get buffer of
   * \return Unused buffer of type
   */
  template <typename T>
  inline ::std::shared_ptr<T> GetBufferForCall(const rrlib::serialization::tDataTypeBase& dt = NULL)
  {
    tPortDataManager* mgr = GetUnusedBufferRaw(rrlib::serialization::tDataType<T>());
    mgr->GetCurrentRefCounter()->SetOrAddLocks((int8_t)1);
    return std::shared_ptr<T>(mgr->GetObject()->GetData<T>(), tSharedPtrDeleteHandler<tPortDataManager>(mgr));
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

  tInterfacePort(const util::tString& description, tFrameworkElement* parent, const rrlib::serialization::tDataTypeBase& data_type, tInterfacePort::tType type_);

  tInterfacePort(const util::tString& description, tFrameworkElement* parent, const rrlib::serialization::tDataTypeBase& data_type, tInterfacePort::tType type_, int custom_flags);

  tInterfacePort(const util::tString& description, tFrameworkElement* parent, const rrlib::serialization::tDataTypeBase& data_type, tInterfacePort::tType type_, int custom_flags, int lock_level);

  tInterfacePort(tPortCreationInfo pci, tInterfacePort::tType type_, int lock_level);

  virtual ~tInterfacePort();

  virtual void ForwardData(tAbstractPort* other)
  {
    // do nothing in interface port
  }

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

  /*!
   * (for non-cc types only)
   * \param dt Data type of object to get buffer of
   * \return Unused buffer of type
   */
  virtual tPortDataManager* GetUnusedBufferRaw(rrlib::serialization::tDataTypeBase dt);

  /*!
   * (for cc types only)
   * \param dt Data type of object to get buffer of
   * \return Unused buffer of type
   */
  inline tCCPortDataManager* GetUnusedCCBuffer(const rrlib::serialization::tDataTypeBase& dt)
  {
    assert((tFinrocTypeInfo::IsCCType(dt)));
    return tThreadLocalCache::Get()->GetUnusedInterThreadBuffer(dt);
  }

  virtual void NotifyDisconnect()    /* don't do anything here... only in network ports */
  {
  }

  virtual void SetMaxQueueLength(int length)
  {
    throw util::tRuntimeException("InterfacePorts do not have a queue", CODE_LOCATION_MACRO);
  }

};

} // namespace finroc
} // namespace core

#endif // core__port__rpc__tInterfacePort_h__
