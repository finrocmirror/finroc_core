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
#include "rrlib/finroc_core_utils/tJCBase.h"

#ifndef CORE__PLUGIN__TEXTERNALCONNECTION_H
#define CORE__PLUGIN__TEXTERNALCONNECTION_H

#include "core/plugin/tConnectionListener.h"
#include "core/parameter/tStructureParameterString.h"
#include "core/tFrameworkElement.h"

namespace finroc
{
namespace core
{
/*!
 * \author Max Reichardt
 *
 * This is an abstract base class for all (network-like) connections to
 * outside the runtime environment.
 *
 * The GUI offers such Connections to be established in it's menu.
 */
class tExternalConnection : public tFrameworkElement
{
private:

  /*! Group name of external connections */
  //public final static String GROUP_NAME = "Connections";

  /*! last connection address */
  util::tString last_address;

  /*! is class currently connected ? */
  bool connected;

  /*! Connection Listeners */
  tConnectionListenerManager listener;

  /*! Is this the first connect ? */
  bool first_connect;

  /*! if set, this module automatically connects to this address */
  tStructureParameterString* auto_connect_to;

public:

  /*! Log domain for this class */
  RRLIB_LOG_CREATE_NAMED_DOMAIN(log_domain, "connections");

protected:

  /*!
   * Performs the actual connecting.
   * This methods needs to be implemented by the subclass.
   *
   * \param address Address that shall be connected to
   * \param same_address Is this the same address as with the last connect
   */
  virtual void ConnectImpl(const util::tString& address, bool same_address) = 0;

  /*!
   * Performs actual disconnecting.
   * This methods needs to be implemented by the subclass.
   */
  virtual void DisconnectImpl() = 0;

  /*!
   * \return Does this connection require entering an address?
   */
  inline bool NeedsAddress()
  {
    return true;
  }

  virtual void PrepareDelete();

public:

  /*!
   * \param description Description of class
   * \param default_address Default connection address (some string)
   */
  tExternalConnection(const util::tString& description, const util::tString& default_address);

  inline void AddConnectionListener(tConnectionListener* l)
  {
    listener.Add(l);
  }

  /*!
   * Universal connect method
   *
   * \param address Address to connect to
   */
  void Connect(const util::tString& address);

  /*!
   * Disconnect connection
   */
  void Disconnect();

  inline void FireConnectionEvent(int e)
  {
    connected = (e == tConnectionListener::cCONNECTED);
    listener.Notify(this, NULL, e);
  }

  /*!
   * \return Address that we are currently connected to or which we were last connected to
   */
  inline util::tString GetConnectionAddress()
  {
    return last_address;
  }

  /*!
   * \return Connection quality information (in a scale from 0 (bad) to 1 (good))
   */
  virtual float GetConnectionQuality() = 0;

  /*!
   * \param detailed Return more detailed information?
   * \return Connection status information for user
   */
  virtual util::tString GetStatus(bool detailed)
  {
    return GetConnectionAddress();
  }

  /*!
   * \return Is there currently a connection to another peer
   */
  inline bool IsConnected()
  {
    return connected;
  }

  /*!
   * Called if connecting was successful
   *
   * \param address Address that was connected to
   */
  void PostConnect(const util::tString& address);

  /*!
   * Connect (to same address as last time)
   */
  inline void Reconnect()
  {
    util::tLock lock2(this);
    Connect(last_address);
  }

  inline void RemoveConnectionListener(tConnectionListener* l)
  {
    listener.Remove(l);
  }

  /*!
   * may be overridden by subclass
   */
  inline void SetLoopTime(int64 ms)
  {
  }

  virtual void StructureParametersChanged();

  virtual const util::tString ToString() const
  {
    return GetClass().GetSimpleName() + " - " + last_address;
  }

};

} // namespace finroc
} // namespace core

#endif // CORE__PLUGIN__TEXTERNALCONNECTION_H
