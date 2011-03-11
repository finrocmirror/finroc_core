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

#ifndef core__port__rpc__tInterfaceClientPort_h__
#define core__port__rpc__tInterfaceClientPort_h__

#include "rrlib/finroc_core_utils/definitions.h"

#include "core/port/rpc/tInterfacePort.h"
#include "core/port/tPortWrapperBase.h"

namespace rrlib
{
namespace serialization
{
class tDataTypeBase;
} // namespace rrlib
} // namespace serialization

namespace finroc
{
namespace core
{
class tFrameworkElement;
class tAbstractPort;

/*! Base class for client interface ports */
class tInterfaceClientPort : public tPortWrapperBase<tInterfacePort>
{
  /*! Special Port class to load value when initialized */
  class tPortImpl : public tInterfacePort
  {
  private:

    // Outer class InterfaceClientPort
    tInterfaceClientPort* const outer_class_ptr;

  protected:

    virtual void ConnectionRemoved(tAbstractPort* partner);

    virtual void NewConnection(tAbstractPort* partner);

  public:

    tPortImpl(tInterfaceClientPort* const outer_class_ptr_, const util::tString& description, tFrameworkElement* parent, const rrlib::serialization::tDataTypeBase& type, tInterfacePort::tType client);

  };

protected:

  /*!
   * Called whenever a connection to this port was removed
   * (meant to be overridden by subclasses)
   * (called with runtime-registry lock)
   *
   * \param partner Port at other end of connection
   */
  virtual void ConnectionRemoved(tAbstractPort* partner)
  {
  }

  /*!
   * Called whenever a new connection to this port was established
   * (meant to be overridden by subclasses)
   * (called with runtime-registry lock)
   *
   * \param partner Port at other end of connection
   */
  virtual void NewConnection(tAbstractPort* partner)
  {
  }

public:

  tInterfaceClientPort(const util::tString& description, tFrameworkElement* parent, const rrlib::serialization::tDataTypeBase& type);

  /*!
   * Get buffer to use in method call (has one lock)
   *
   * (for non-cc types only)
   * \param dt Data type of object to get buffer of
   * \return Unused buffer of type
   */
  template <typename T>
  inline ::std::shared_ptr<T> GetBufferForCall(const rrlib::serialization::tDataTypeBase& dt = NULL)
  {
    return this->wrapped->GetBufferForCall<T>(dt);
  }

  /*!
   * (Usually called on client ports)
   *
   * \return "Server" Port that handles method call - either InterfaceServerPort or InterfaceNetPort (the latter if we have remote server)
   */
  inline tInterfacePort* GetServer()
  {
    return this->wrapped->GetServer();
  }

  /*!
   * \return Is server for port in remote runtime environment?
   */
  inline bool HasRemoteServer()
  {
    tInterfacePort* server = GetServer();
    return (server != NULL) && (server->GetType() == tInterfacePort::eNetwork);
  }

};

} // namespace finroc
} // namespace core

#endif // core__port__rpc__tInterfaceClientPort_h__
