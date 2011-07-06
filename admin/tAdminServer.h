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

#ifndef core__admin__tAdminServer_h__
#define core__admin__tAdminServer_h__

#include "rrlib/finroc_core_utils/definitions.h"

#include "core/port/rpc/method/tPortInterface.h"
#include "core/port/rpc/method/tVoid2Method.h"
#include "core/port/rpc/method/tVoid3Method.h"
#include "core/port/rpc/method/tPort0Method.h"
#include "core/port/rpc/method/tVoid4Method.h"
#include "core/datatype/tCoreString.h"
#include "core/port/rpc/method/tVoid1Method.h"
#include "core/port/rpc/method/tPort2Method.h"
#include "core/port/rpc/method/tPort1Method.h"
#include "core/port/rpc/method/tPort3Method.h"
#include "core/portdatabase/tRPCInterfaceType.h"
#include "rrlib/finroc_core_utils/container/tSimpleList.h"
#include "rrlib/serialization/tMemoryBuffer.h"
#include "core/port/rpc/tInterfaceServerPort.h"
#include "core/port/rpc/method/tAbstractMethodCallHandler.h"

namespace rrlib
{
namespace serialization
{
class tOutputStream;
} // namespace rrlib
} // namespace serialization

namespace finroc
{
namespace core
{
class tExecutionControl;
class tAbstractMethod;
class tConfigFile;

/*!
 * \author Max Reichardt
 *
 * Administration interface server port
 */
class tAdminServer : public tInterfaceServerPort, public tAbstractMethodCallHandler
{
  /*! Struct for callback parameters for GET_PARAMETER_INFO method */
  struct tCallbackParameters
  {
public:

    rrlib::serialization::tOutputStream* co;

    tConfigFile* cf;

    util::tSimpleList<tExecutionControl*>* ecs;

    tCallbackParameters(tConfigFile* cf2, rrlib::serialization::tOutputStream* co2) :
        co(co2),
        cf(cf2),
        ecs(NULL)
    {
    }

    tCallbackParameters(util::tSimpleList<tExecutionControl*>* ecs_) :
        co(NULL),
        cf(NULL),
        ecs(ecs_)
    {
    }

  };

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
  static tVoid3Method<tAdminServer*, int, tPortDataPtr<const rrlib::serialization::tMemoryBuffer>, int> cSET_PORT_VALUE;

  /*! Get module types */
  static tPort0Method<tAdminServer*, tPortDataPtr<rrlib::serialization::tMemoryBuffer> > cGET_CREATE_MODULE_ACTIONS;

  /*! Create a module */
  static tVoid4Method<tAdminServer*, int, tPortDataPtr<tCoreString>, int, tPortDataPtr<const rrlib::serialization::tMemoryBuffer> > cCREATE_MODULE;

  /*! Save finstructable group */
  static tVoid1Method<tAdminServer*, int> cSAVE_FINSTRUCTABLE_GROUP;

  /*! Get annotation */
  static tPort2Method<tAdminServer*, tPortDataPtr<rrlib::serialization::tMemoryBuffer>, int, tPortDataPtr<tCoreString> > cGET_ANNOTATION;

  /*! Set annotation */
  static tVoid4Method<tAdminServer*, int, tPortDataPtr<tCoreString>, int, tPortDataPtr<const rrlib::serialization::tMemoryBuffer> > cSET_ANNOTATION;

  /*! Delete element */
  static tVoid1Method<tAdminServer*, int> cDELETE_ELEMENT;

  /*! Start/Resume application execution */
  static tVoid1Method<tAdminServer*, int> cSTART_EXECUTION;

  /*! Stop/Pause application execution */
  static tVoid1Method<tAdminServer*, int> cPAUSE_EXECUTION;

  /*! Is framework element with specified handle currently executing? */
  static tPort1Method<tAdminServer*, int, int> cIS_RUNNING;

  /*! Get current port value as string */
  static tPort3Method<tAdminServer*, tPortDataPtr<tCoreString>, int, int, int > cGET_PORT_VALUE_AS_STRING;

  /*! Get parameter info for specified framework element: ConfigFile, children with config file, info on all parameters with same config file  */
  static tPort2Method<tAdminServer*, tPortDataPtr<rrlib::serialization::tMemoryBuffer>, int, tPortDataPtr<tCoreString> > cGET_PARAMETER_INFO;

  /*! Data Type of method calls to this port */
  static tRPCInterfaceType cDATA_TYPE;

  /*! Port name of admin interface */
  static util::tString cPORT_NAME;

  /*! Qualified port name */
  static util::tString cQUALIFIED_PORT_NAME;

  /*! Return values for IS_RUNNING */
  static const int cNOTHING = -1, cSTOPPED = 0, cSTARTED = 1, cBOTH = 2;

private:

  /*!
   * Connects two ports taking volatility into account
   *
   * \param src Source port
   * \param dest Destination port
   */
  void Connect(tAbstractPort* src, tAbstractPort* dest);

  /*!
   * Returns all relevant execution controls for start/stop command on specified element
   * (Helper method for IS_RUNNING, START_EXECUTION and PAUSE_EXECUTION)
   *
   * \param result Result buffer for list of execution controls
   * \param element_handle Handle of element
   */
  void GetExecutionControls(util::tSimpleList<tExecutionControl*>& result, int element_handle);

public:

  tAdminServer();

  tPortDataPtr<rrlib::serialization::tMemoryBuffer> HandleCall(tAbstractMethod* method);

  tPortDataPtr<rrlib::serialization::tMemoryBuffer> HandleCall(tAbstractMethod* method, int handle, tPortDataPtr<tCoreString>& type);

  int HandleCall(const tAbstractMethod* method, int handle);

  tPortDataPtr<tCoreString> HandleCall(const tAbstractMethod* method, int p1, int p2, int p3);

  void HandleVoidCall(const tAbstractMethod* method, int p1, int p2);

  void HandleVoidCall(tAbstractMethod* method, int port_handle, tPortDataPtr<const rrlib::serialization::tMemoryBuffer>& buf, int as_string);

  void HandleVoidCall(tAbstractMethod* method, int cma_index, tPortDataPtr<tCoreString>& name, int parent_handle, tPortDataPtr<const rrlib::serialization::tMemoryBuffer>& params_buffer);

  void HandleVoidCall(const tAbstractMethod* method, int handle);

  void TreeFilterCallback(tFrameworkElement* fe, const tCallbackParameters& custom_param);

};

} // namespace finroc
} // namespace core

#endif // core__admin__tAdminServer_h__
