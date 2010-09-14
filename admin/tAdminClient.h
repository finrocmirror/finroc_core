/**
 * You received this file as part of an advanced experimental
 * robotics framework prototype ('finroc')
 *
 * Copyright (C) 2010 Max Reichardt,
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

#ifndef CORE__ADMIN__TADMINCLIENT_H
#define CORE__ADMIN__TADMINCLIENT_H

#include "core/tFrameworkElement.h"
#include "core/port/cc/tCCInterThreadContainer.h"
#include "core/port/std/tPortDataImpl.h"
#include "core/port/rpc/tInterfaceClientPort.h"

namespace finroc
{
namespace core
{
class tNetPort;

/*!
 * \author Max Reichardt
 *
 * Client port for admin interface
 */
class tAdminClient : public tInterfaceClientPort
{
public:

  tAdminClient(const util::tString& description, tFrameworkElement* parent);

  /*!
   * Connect two ports in remote runtime
   *
   * \param np1 Port1
   * \param np2 Port2
   */
  void Connect(tNetPort* np1, tNetPort* np2);

  /*!
   * Disconnect two ports in remote runtime
   *
   * \param np1 Port1
   * \param np2 Port2
   */
  void Disconnect(tNetPort* np1, tNetPort* np2);

  /*!
   * Disconnect port in remote runtime
   *
   * \param np1 Port1
   */
  void DisconnectAll(tNetPort* np1);

  /*!
   * Sets value of remote port
   *
   * \param np network port of remote port
   * \param container Data to assign to remote port
   */
  void SetRemotePortValue(tNetPort* np, tCCInterThreadContainer<>* container);

  /*!
   * Sets value of remote port
   *
   * \param np network port of remote port
   * \param port_data Data to assign to remote port
   */
  void SetRemotePortValue(tNetPort* np, tPortData* port_data);

};

} // namespace finroc
} // namespace core

#endif // CORE__ADMIN__TADMINCLIENT_H
