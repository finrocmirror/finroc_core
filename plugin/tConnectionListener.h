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

#ifndef CORE__PLUGIN__TCONNECTIONLISTENER_H
#define CORE__PLUGIN__TCONNECTIONLISTENER_H

#include "finroc_core_utils/tListenerManager.h"

namespace finroc
{
namespace core
{
class tExternalConnection;

/*!
 * \author Max Reichardt
 *
 * This interface can be used to get notified whenever the connection status
 * of an external connection changes.
 */
class tConnectionListener : public util::tInterface
{
public:

  /*! Possible events */
  static const int cCONNECTED = 1, cNOT_CONNECTED = 2, cINTERFACE_UPDATED = 3;

  /*!
   * Called whenever an connection event occurs
   *
   * \param source Connection that event came from
   * \param e Event that occured (see constants above)
   */
  virtual void ConnectionEvent(tExternalConnection* source, int e) = 0;

};

/*!
 * Manager for connection listeners
 */
class tConnectionListenerManager : public util::tListenerManager<tExternalConnection, util::tObject, tConnectionListener, tConnectionListenerManager>
{
public:

  tConnectionListenerManager() {}

  inline void SingleNotify(tConnectionListener* listener, tExternalConnection* origin, const util::tObject* parameter, int call_id)
  {
    listener->ConnectionEvent(origin, call_id);
  }

};

} // namespace finroc
} // namespace core

#endif // CORE__PLUGIN__TCONNECTIONLISTENER_H
