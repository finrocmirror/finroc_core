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
#include "rrlib/finroc_core_utils/tJCBase.h"

#ifndef CORE__ADMIN__TADMINSERVER_H
#define CORE__ADMIN__TADMINSERVER_H

#include "core/port/rpc/method/tPortInterface.h"
#include "core/port/rpc/method/tVoid2Method.h"
#include "core/port/rpc/method/tVoid3Method.h"
#include "core/port/cc/tCCInterThreadContainer.h"
#include "core/port/rpc/method/tPort0Method.h"
#include "core/port/rpc/method/tVoid4Method.h"
#include "core/port/rpc/method/tVoid1Method.h"
#include "core/port/rpc/method/tPort2Method.h"
#include "core/port/rpc/method/tVoid0Method.h"
#include "core/port/rpc/method/tPort1Method.h"
#include "core/port/rpc/tInterfaceServerPort.h"
#include "core/port/rpc/method/tAbstractMethodCallHandler.h"

namespace finroc
{
namespace core
{
class tPortData;
class tMemBuffer;
class tCoreString;
class tDataType;
class tAbstractMethod;

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
  static tVoid2Method<tAdminServer*, int, int> cDISCONNECT_ALL;

  /*! Set a port's value */
  static tVoid3Method<tAdminServer*, int, tCCInterThreadContainer<>*, tPortData*> cSET_PORT_VALUE;

  /*! Set a port's value */
  static tPort0Method<tAdminServer*, tMemBuffer*> cGET_CREATE_MODULE_ACTIONS;

  /*! Set a port's value */
  static tVoid4Method<tAdminServer*, int, tCoreString*, int, tMemBuffer*> cCREATE_MODULE;

  /*! Save finstructable group */
  static tVoid1Method<tAdminServer*, int> cSAVE_FINSTRUCTABLE_GROUP;

  /*! Get annotation */
  static tPort2Method<tAdminServer*, tMemBuffer*, int, tCoreString*> cGET_ANNOTATION;

  /*! Set annotation */
  static tVoid4Method<tAdminServer*, int, tCoreString*, int, tMemBuffer*> cSET_ANNOTATION;

  /*! Delete element */
  static tVoid1Method<tAdminServer*, int> cDELETE_ELEMENT;

  /*! Start/Resume application execution */
  static tVoid0Method<tAdminServer*> cSTART_EXECUTION;

  /*! Stop/Pause application execution */
  static tVoid0Method<tAdminServer*> cPAUSE_EXECUTION;

  /*! Is framework element with specified handle currently executing? */
  static tPort1Method<tAdminServer*, int, int> cIS_RUNNING;

  /*! Data Type of method calls to this port */
  static tDataType* cDATA_TYPE;

  /*! Port name of admin interface */
  static util::tString cPORT_NAME;

  /*! Qualified port name */
  static util::tString cQUALIFIED_PORT_NAME;

private:

  /*!
   * Connects two ports taking volatility into account
   *
   * \param src Source port
   * \param dest Destination port
   */
  void Connect(tAbstractPort* src, tAbstractPort* dest);

public:

  tAdminServer();

  tMemBuffer* HandleCall(const tAbstractMethod* method);

  tMemBuffer* HandleCall(const tAbstractMethod* method, int handle, tCoreString* type);

  int HandleCall(const tAbstractMethod* method, int handle);

  void HandleVoidCall(const tAbstractMethod* method, int p1, int p2);

  void HandleVoidCall(const tAbstractMethod* method, int port_handle, tCCInterThreadContainer<>* cc_data, tPortData* port_data);

  void HandleVoidCall(const tAbstractMethod* method, int cma_index, tCoreString* name, int parent_handle, tMemBuffer* params_buffer);

  void HandleVoidCall(const tAbstractMethod* method, int handle);

  void HandleVoidCall(const tAbstractMethod* method);

};

} // namespace finroc
} // namespace core

#endif // CORE__ADMIN__TADMINSERVER_H
