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

#ifndef core__port__tPortWrapperBase_h__
#define core__port__tPortWrapperBase_h__

#include "rrlib/finroc_core_utils/definitions.h"

#include "rrlib/serialization/tDataTypeBase.h"
#include "core/portdatabase/tFinrocTypeInfo.h"
#include "core/port/tThreadLocalCache.h"

namespace finroc
{
namespace core
{
class tAbstractPort;

/*!
 * \author Max Reichardt
 *
 * Port classes are not directly used by an application developer.
 * Rather a wrapped class based on this class is used.
 * This has the following advantages:
 * - Wrapped wraps and manages pointer to actual port. No pointers are needed to work with these classes
 * - Only parts of the API meant to be used by the application developer are exposed.
 * - Connect() methods can be hidden/reimplemented (via name hiding). This can be used to enforce that only certain connections can be created at compile time.
 */
template<typename T>
class tPortWrapperBase : public util::tObject
{
protected:

  /*! Wrapped port */
  T* wrapped;

public:

  /*! Log domain for this class */
  RRLIB_LOG_CREATE_NAMED_DOMAIN(log_domain, "ports");

  tPortWrapperBase() :
      wrapped(NULL)
  {}

  /*!
   * Connect port to specified source port
   *
   * \param source Source port
   */
  inline void ConnectToSource(tAbstractPort* source)
  {
    wrapped->ConnectToSource(source);
  }

  /*!
   * Connect port to specified source port
   *
   * \param source Source port
   */
  inline void ConnectToSource(const tPortWrapperBase<T>& source)
  {
    wrapped->ConnectToSource(source.wrapped);
  }

  /*!
   * Connect port to specified source port
   * (connection is (re)established when link is available)
   *
   * \param link_name Link name of source port (relative to parent framework element)
   */
  inline void ConnectToSource(const util::tString& src_link)
  {
    wrapped->ConnectToSource(src_link);
  }

  /*!
   * Connect port to specified target port
   *
   * \param target Target port
   */
  inline void ConnectToTarget(tAbstractPort* target)
  {
    wrapped->ConnectToTarget(target);
  }

  /*!
   * Connect port to specified target port
   *
   * \param target Target port
   */
  inline void ConnectToTarget(const tPortWrapperBase<T>& target)
  {
    wrapped->ConnectToTarget(target.wrapped);
  }

  /*!
   * Connect port to specified target port
   * (connection is (re)established when link is available)
   *
   * \param link_name Link name of target port (relative to parent framework element)
   */
  inline void ConnectToTarget(const util::tString& dest_link)
  {
    wrapped->ConnectToTarget(dest_link);
  }

  virtual ~tPortWrapperBase()
  {
    //wrapped.managedDelete(); - this is unsuitable for pass by value; it should work without anyway
  }

  /*!
   * Are description of this element and String 'other' identical?
   * (result is identical to getDescription().equals(other); but more efficient in C++)
   *
   * \param other Other String
   * \return Result
   */
  inline bool DescriptionEquals(const util::tString& other) const
  {
    return wrapped->DescriptionEquals(other);
  }

  /*!
   * same as getDescription()
   * except that we return a const char* in C++ - this way, no memory needs to be allocated
   */
  inline const char* GetCDescription() const
  {
    return wrapped->GetCDescription();
  }

  // using this operator, it can be checked conveniently in PortListener's portChanged()
  // whether origin port is the same port as this object wraps
  bool operator ==(const T* p) const
  {
    return wrapped == p;
  }

  /*!
   * \return Number of connections to this port (incoming and outgoing)
   */
  inline int GetConnectionCount()
  {
    return wrapped->GetConnectionCount();
  }

  /*!
   * \return Type of port data
   */
  inline const rrlib::serialization::tDataTypeBase GetDataType() const
  {
    return wrapped->GetDataType();
  }

  /*!
   * \return Additional type info for port data
   */
  inline tFinrocTypeInfo GetDataTypeInfo() const
  {
    return tFinrocTypeInfo::Get(wrapped->GetDataType());
  }

  /*!
   * \return Name/Description
   */
  inline const util::tString GetDescription() const
  {
    return wrapped->GetDescription();
  }

  /*!
   * @return
   * @see org.finroc.core.FrameworkElement#getLogDescription()
   */
  inline util::tString GetLogDescription() const
  {
    return wrapped->GetLogDescription();
  }

  /*!
   * \return Minimum Network Update Interval (only-port specific one; -1 if there's no specific setting for port)
   */
  inline int16 GetMinNetUpdateInterval()
  {
    return wrapped->GetMinNetUpdateInterval();
  }

  /*!
   * \return Wrapped port. For rare case that someone really needs to access ports.
   */
  inline T* GetWrapped()
  {
    return wrapped;
  }

  /*!
   * (relevant for input ports only)
   *
   * \return Has port changed since last changed-flag-reset?
   */
  inline bool HasChanged() const
  {
    return wrapped->HasChanged();
  }

  /*!
   * Initialize this port.
   * This must be called prior to using port
   * (usually done by initializing parent)
   * - and in order port being published.
   */
  inline void Init()
  {
    wrapped->Init();
  }

  /*!
   * (slightly expensive)
   * \return Is port currently connected?
   */
  inline bool IsConnected() const
  {
    return wrapped->IsConnected();
  }

  /*!
   * \return Has port been deleted? (you should not encounter this)
   */
  inline bool IsDeleted() const
  {
    return wrapped->IsDeleted();
  }

  /*!
   * \return Is framework element ready/fully initialized and not yet deleted?
   */
  inline bool IsReady() const
  {
    return wrapped->IsReady();
  }

  /*!
   * Is data to this port pushed or pulled?
   *
   * \return Answer
   */
  inline bool PushStrategy() const
  {
    return wrapped->PushStrategy();
  }

  /*!
   * Releases all automatically acquired locks
   */
  inline void ReleaseAutoLocks()
  {
    tThreadLocalCache::GetFast()->ReleaseAllLocks();
  }

  /*!
   * (relevant for input ports only)
   *
   * Reset changed flag.
   */
  inline void ResetChanged()
  {
    wrapped->ResetChanged();
  }

  /*!
   * Is data to this port pushed or pulled (in reverse direction)?
   *
   * \return Answer
   */
  inline bool ReversePushStrategy() const
  {
    return wrapped->ReversePushStrategy();
  }

  /*!
   * (relevant for input ports only)
   *
   * Sets changed flag
   */
  inline void SetChanged()
  {
    wrapped->SetChanged();
  }

  /*!
   * \param interval2 Minimum Network Update Interval
   */
  inline void SetMinNetUpdateInterval(int interval2)
  {
    wrapped->SetMinNetUpdateInterval(interval2);
  }

  /*!
   * Set whether data should be pushed or pulled
   *
   * \param push Push data?
   */
  inline void SetPushStrategy(bool push)
  {
    wrapped->SetPushStrategy(push);
  }

  /*!
   * Set whether data should be pushed or pulled in reverse direction
   *
   * \param push Push data?
   */
  inline void SetReversePushStrategy(bool push)
  {
    wrapped->SetReversePushStrategy(push);
  }

};

} // namespace finroc
} // namespace core

#endif // core__port__tPortWrapperBase_h__
