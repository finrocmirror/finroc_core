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

#ifndef CORE__ADMIN__TADMINSERVER_H
#define CORE__ADMIN__TADMINSERVER_H

#include "core/portdatabase/tDataType.h"
#include "core/port/rpc/method/tPortInterface.h"
#include "core/port/rpc/method/tVoid2Method.h"
#include "core/port/rpc/method/tVoid1Method.h"
#include "core/port/rpc/method/tAbstractMethod.h"
#include "core/port/rpc/tInterfaceServerPort.h"
#include "core/port/rpc/method/tAbstractMethodCallHandler.h"

namespace finroc
{
namespace core
{
/*!
 * \author Max Reichardt
 *
 * Administration interface server port
 */
class tAdminServer : public tInterfaceServerPort, public tAbstractMethodCallHandler
{
public:

  /*! Admin interface */
  static tPortInterface cMETHODS;

  /*! Connect */
  static tVoid2Method<tAdminServer*, int, int> cCONNECT;

  /*! Disconnect */
  static tVoid2Method<tAdminServer*, int, int> cDISCONNECT;

  /*! Disconnect All */
  static tVoid1Method<tAdminServer*, int> cDISCONNECT_ALL;

  /*! Data Type of method calls to this port */
  static tDataType* cDATA_TYPE;

  /*! Port name of admin interface */
  static util::tString cPORT_NAME;

  /*! Qualified port name */
  static util::tString cQUALIFIED_PORT_NAME;

  tAdminServer();

  void HandleVoidCall(const tAbstractMethod* method, int p1, int p2);

  void HandleVoidCall(const tAbstractMethod* method, int p1);

};

} // namespace finroc
} // namespace core

#endif // CORE__ADMIN__TADMINSERVER_H
