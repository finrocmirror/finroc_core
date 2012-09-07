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
#include "rrlib/rtti/rtti.h"
#include "rrlib/serialization/serialization.h"
#include <boost/lexical_cast.hpp>

#include "core/admin/tAdminServer.h"
#include "core/tCoreFlags.h"
#include "core/port/tAbstractPort.h"
#include "core/tFrameworkElement.h"
#include "core/tRuntimeEnvironment.h"
#include "core/thread/tExecutionControl.h"
#include "core/plugin/tCreateFrameworkElementAction.h"
#include "core/plugin/tPlugins.h"
#include "core/plugin/sDynamicLoading.h"
#include "core/plugin/runtime_construction_actions.h"
#include "core/parameter/tStaticParameterList.h"
#include "core/tFinrocAnnotation.h"
#include "core/parameter/tConfigFile.h"
#include "core/portdatabase/tFinrocTypeInfo.h"
#include "core/port/cc/tCCPortBase.h"
#include "core/port/std/tPortBase.h"
#include "core/port/std/tPortDataManager.h"
#include "core/port/cc/tCCPortDataManagerTL.h"
#include "core/port/tThreadLocalCache.h"
#include "core/parameter/tConstructorParameters.h"
#include "core/parameter/tStaticParameterBase.h"
#include "core/finstructable/tFinstructableGroup.h"
#include "core/parameter/tParameterInfo.h"

namespace finroc
{
namespace core
{
tPortInterface tAdminServer::cMETHODS("Admin Interface", true);
tVoidMethod<tAdminServer, int, int> tAdminServer::cCONNECT(tAdminServer::cMETHODS, "Connect", "source port handle", "destination port handle", false);
tVoidMethod<tAdminServer, int, int> tAdminServer::cDISCONNECT(tAdminServer::cMETHODS, "Disconnect", "source port handle", "destination port handle", false);
tVoidMethod<tAdminServer, int, int> tAdminServer::cDISCONNECT_ALL(tAdminServer::cMETHODS, "DisconnectAll", "source port handle", "dummy", false);
tMethod<tAdminServer, tPortDataPtr<std::string>, int, tPortDataPtr<const rrlib::serialization::tMemoryBuffer>, int> tAdminServer::cSET_PORT_VALUE(tAdminServer::cMETHODS, "SetPortValue", "port handle", "data", "dummy", false);
tMethod<tAdminServer, tPortDataPtr<rrlib::serialization::tMemoryBuffer>> tAdminServer::cGET_CREATE_MODULE_ACTIONS(tAdminServer::cMETHODS, "GetCreateModuleActions", false);
tMethod<tAdminServer, tPortDataPtr<std::string>, int, tPortDataPtr<std::string>, int, tPortDataPtr<const rrlib::serialization::tMemoryBuffer>> tAdminServer::cCREATE_MODULE(tAdminServer::cMETHODS, "CreateModule", "create action index", "module name", "parent handle", "module creation parameters", false);
tVoidMethod<tAdminServer, int> tAdminServer::cSAVE_FINSTRUCTABLE_GROUP(tAdminServer::cMETHODS, "Save Finstructable Group", "finstructable handle", false);
tMethod<tAdminServer, tPortDataPtr<rrlib::serialization::tMemoryBuffer>, int, tPortDataPtr<std::string>> tAdminServer::cGET_ANNOTATION(tAdminServer::cMETHODS, "Get Annotation", "handle", "annotation type", false);
tVoidMethod<tAdminServer, int, tPortDataPtr<std::string>, int, tPortDataPtr<const rrlib::serialization::tMemoryBuffer>> tAdminServer::cSET_ANNOTATION(tAdminServer::cMETHODS, "Set Annotation", "handle", "dummy", "dummy", "annotation", false);
tVoidMethod<tAdminServer, int> tAdminServer::cDELETE_ELEMENT(tAdminServer::cMETHODS, "Delete Framework element", "handle", false);
tVoidMethod<tAdminServer, int> tAdminServer::cSTART_EXECUTION(tAdminServer::cMETHODS, "Start execution", "Framework element handle", false);
tVoidMethod<tAdminServer, int> tAdminServer::cPAUSE_EXECUTION(tAdminServer::cMETHODS, "Pause execution", "Framework element handle", false);
tMethod<tAdminServer, int, int> tAdminServer::cIS_RUNNING(tAdminServer::cMETHODS, "Is Framework element running", "handle", false);
tMethod<tAdminServer, tPortDataPtr<rrlib::serialization::tMemoryBuffer>, int, tPortDataPtr<std::string>> tAdminServer::cGET_PARAMETER_INFO(tAdminServer::cMETHODS, "GetParameterInfo", "handle", "dummy", false);
tMethod<tAdminServer, tPortDataPtr<rrlib::serialization::tMemoryBuffer>> tAdminServer::cGET_MODULE_LIBRARIES(tAdminServer::cMETHODS, "GetModuleLibraries", false);
tMethod<tAdminServer, tPortDataPtr<rrlib::serialization::tMemoryBuffer>, int, tPortDataPtr<std::string>> tAdminServer::cLOAD_MODULE_LIBRARY(tAdminServer::cMETHODS, "LoadModuleLibrary", "dummy", "library name", false);

tRPCInterfaceType tAdminServer::cDATA_TYPE("Administration method calls", &(tAdminServer::cMETHODS));
util::tString tAdminServer::cPORT_NAME = "Administration";
util::tString tAdminServer::cQUALIFIED_PORT_NAME = "Unrelated/Administration";
const int tAdminServer::cNOTHING, tAdminServer::cSTOPPED, tAdminServer::cSTARTED, tAdminServer::cBOTH;

tAdminServer::tAdminServer() :
  tInterfaceServerPort(cPORT_NAME, NULL, cDATA_TYPE, NULL, tCoreFlags::cSHARED)
{
  SetCallHandler(this);
}

void tAdminServer::Connect(tAbstractPort& src, tAbstractPort& dest)
{
  if (src.IsVolatile() && (!dest.IsVolatile()))
  {
    dest.ConnectTo(src.GetQualifiedLink(), tAbstractPort::tConnectDirection::AUTO, true);
  }
  else if (dest.IsVolatile() && (!src.IsVolatile()))
  {
    src.ConnectTo(dest.GetQualifiedLink(), tAbstractPort::tConnectDirection::AUTO, true);
  }
  else
  {
    src.ConnectTo(dest, tAbstractPort::tConnectDirection::AUTO, true);
  }
}

void tAdminServer::GetExecutionControls(std::vector<tExecutionControl*>& result, int element_handle)
{
  core::tFrameworkElement* fe = GetRuntime().GetElement(element_handle);
  if (fe)
  {
    tExecutionControl::FindAll(result, *fe);
    if (result.size() == 0)
    {
      tExecutionControl* ec = tExecutionControl::Find(*fe);
      if (ec != NULL)
      {
        result.push_back(ec);
      }
    }
  }
}

tPortDataPtr<rrlib::serialization::tMemoryBuffer> tAdminServer::HandleCall(const tAbstractMethod& method)
{
  assert((method == cGET_CREATE_MODULE_ACTIONS) || (method == cLOAD_MODULE_LIBRARY) || (method == cGET_MODULE_LIBRARIES));

  tPortDataPtr<rrlib::serialization::tMemoryBuffer> mb = this->GetBufferForReturn<rrlib::serialization::tMemoryBuffer>();
  rrlib::serialization::tOutputStream co(mb.get(), rrlib::serialization::tTypeEncoding::NAMES);

  if (method == cGET_MODULE_LIBRARIES)
  {
    std::vector<std::string> libs = sDynamicLoading::GetLoadableFinrocLibraries();
    for (size_t i = 0; i < libs.size(); i++)
    {
      co.WriteString(libs[i]);
    }
  }
  else
  {
    const std::vector<core::tCreateFrameworkElementAction*>& module_types = runtime_construction::GetConstructibleElements();
    for (size_t i = 0u; i < module_types.size(); i++)
    {
      const tCreateFrameworkElementAction& cma = *module_types[i];
      co.WriteString(cma.GetName());
      co.WriteString(cma.GetModuleGroup());
      if (cma.GetParameterTypes() != NULL)
      {
        cma.GetParameterTypes()->Serialize(co);
      }
      else
      {
        tStaticParameterList::cEMPTY.Serialize(co);
      }
    }
  }

  co.Close();
  return mb;
}

tPortDataPtr<rrlib::serialization::tMemoryBuffer> tAdminServer::HandleCall(const tAbstractMethod& method, int handle, tPortDataPtr<std::string>& type)
{
  if (method == cGET_ANNOTATION)
  {
    ::finroc::core::tFrameworkElement* fe = GetRuntime().GetElement(handle);
    tFinrocAnnotation* result = NULL;
    rrlib::rtti::tDataTypeBase dt = rrlib::rtti::tDataTypeBase::FindType(*type);
    if (fe != NULL && fe->IsReady() && dt != NULL)
    {
      result = fe->GetAnnotation(dt);
    }
    else
    {
      FINROC_LOG_PRINT(ERROR, "Could not query element for annotation type ", *type);
    }

    if (result == NULL)
    {
      return tPortDataPtr<rrlib::serialization::tMemoryBuffer>();
    }
    else
    {
      tPortDataPtr<rrlib::serialization::tMemoryBuffer> buf = this->GetBufferForReturn<rrlib::serialization::tMemoryBuffer>();
      rrlib::serialization::tOutputStream co(buf.get(), rrlib::serialization::tTypeEncoding::NAMES);
      co << result->GetType();
      result->Serialize(co);
      co.Close();
      return buf;
    }
  }
  else if (method == cLOAD_MODULE_LIBRARY)
  {
    FINROC_LOG_PRINT(USER, "Loading library ", *type);
    sDynamicLoading::DLOpen(type->c_str());
    return HandleCall(method); // return stuff from GET_CREATE_MODULE_ACTIONS
  }
  else
  {
    assert((method == cGET_PARAMETER_INFO));

    core::tFrameworkElement* fe = GetRuntime().GetElement(handle);
    if (fe == NULL || (!fe->IsReady()))
    {
      FINROC_LOG_PRINT(ERROR, "Could not get parameter info for framework element ", handle);

      return tPortDataPtr<rrlib::serialization::tMemoryBuffer>();
    }

    tConfigFile* cf = tConfigFile::Find(*fe);
    tPortDataPtr<rrlib::serialization::tMemoryBuffer> buf = this->GetBufferForReturn<rrlib::serialization::tMemoryBuffer>();
    rrlib::serialization::tOutputStream co(buf.get(), rrlib::serialization::tTypeEncoding::NAMES);
    if (cf == NULL)
    {
      co.WriteBoolean(false);
    }
    else
    {
      co.WriteBoolean(true);
      co.WriteInt((static_cast<core::tFrameworkElement*>(cf->GetAnnotated()))->GetHandle());
      cf->Serialize(co);
      for (auto it = fe->SubElementsBegin(true); it != fe->SubElementsEnd(); ++it)
      {
        tConfigFile* fe_config = static_cast<tConfigFile*>(it->GetAnnotation(tConfigFile::cTYPE));
        if (fe_config)
        {
          co.WriteByte(1);
          co.WriteInt(it->GetHandle());
          co.WriteString(fe_config->GetFilename());
          co.WriteBoolean(fe_config->IsActive());
        }
        else
        {
          tParameterInfo* pi = static_cast<tParameterInfo*>(it->GetAnnotation(tParameterInfo::cTYPE));
          if (pi && cf == tConfigFile::Find(*it))
          {
            co.WriteByte(2);
            co.WriteInt(it->GetHandle());
            co.WriteString(pi->GetConfigEntry());
          }
        }
      }
    }
    co.Close();
    return buf;
  }
}

int tAdminServer::HandleCall(const tAbstractMethod& method, int handle)
{
  assert(method == cIS_RUNNING);
  std::vector<tExecutionControl*> ecs;
  GetExecutionControls(ecs, handle);

  bool stopped = false;
  bool running = false;
  for (auto it = ecs.begin(); it < ecs.end(); it++)
  {
    stopped |= (!(*it)->IsRunning());
    running |= (*it)->IsRunning();
  }
  if (running && stopped)
  {
    return cBOTH;
  }
  else if (running)
  {
    return cSTARTED;
  }
  else if (stopped)
  {
    return cSTOPPED;
  }
  else
  {
    return cNOTHING;
  }
}

tPortDataPtr<std::string> tAdminServer::HandleCall(const tAbstractMethod& method, int port_handle, tPortDataPtr<const rrlib::serialization::tMemoryBuffer>& buf, int unused)
{
  assert(method == cSET_PORT_VALUE);
  tPortDataPtr<std::string> result = this->GetBufferForReturn<std::string>();
  result->clear();

  core::tAbstractPort* port = tRuntimeEnvironment::GetInstance()->GetPort(port_handle);
  if (port != NULL && port->IsReady())
  {
    tLock lock3(GetRegistryLock()); // TODO: obtaining registry lock is quite heavy-weight - however, set calls should not occur often
    if (port->IsReady())
    {
      try
      {
        rrlib::serialization::tInputStream ci(buf.get(), rrlib::serialization::tTypeEncoding::NAMES);
        rrlib::serialization::tDataEncoding enc;
        rrlib::rtti::tDataTypeBase dt;
        ci >> enc >> dt;
        if (tFinrocTypeInfo::IsCCType(port->GetDataType()) && tFinrocTypeInfo::IsCCType(dt))
        {
          tCCPortBase* p = static_cast<tCCPortBase*>(port);
          tCCPortDataManagerTL* c = tThreadLocalCache::Get()->GetUnusedBuffer(dt);
          c->GetObject()->Deserialize(ci, enc);
          (*result) = p->BrowserPublishRaw(c);
          if (result->length() > 0)
          {
            FINROC_LOG_PRINT(WARNING, "Setting value of port '", port->GetQualifiedName(), "' failed: ", *result);
          }
        }
        else if (tFinrocTypeInfo::IsStdType(port->GetDataType()) && tFinrocTypeInfo::IsStdType(dt))
        {
          tPortBase* p = static_cast<tPortBase*>(port);
          tPortDataManager* port_data = p->GetDataType() != dt ? p->GetUnusedBufferRaw(dt) : p->GetUnusedBufferRaw();
          port_data->GetObject()->Deserialize(ci, enc);
          p->BrowserPublish(port_data);
        }
      }
      catch (const std::exception& e)
      {
        FINROC_LOG_PRINT(WARNING, "Setting value of port '", port->GetQualifiedName(), "' failed: ", e);
        (*result) = e.what();
      }
      return result;
    }
  }
  (*result) += "Port with handle " + boost::lexical_cast<std::string>(port_handle) + " is not available.";
  FINROC_LOG_PRINT(WARNING, "Setting value of port failed: ", *result);
  return result;
}

tPortDataPtr<std::string> tAdminServer::HandleCall(const tAbstractMethod& method, int cma_index, tPortDataPtr<std::string>& name, int parent_handle, tPortDataPtr<const rrlib::serialization::tMemoryBuffer>& params_buffer)
{
  tConstructorParameters* params = NULL;
  assert(method == cCREATE_MODULE);
  tPortDataPtr<std::string> result = this->GetBufferForReturn<std::string>();
  result->clear();

  try
  {
    tLock lock4(GetRegistryLock());
    tCreateFrameworkElementAction* cma = runtime_construction::GetConstructibleElements()[cma_index];
    core::tFrameworkElement* parent = tRuntimeEnvironment::GetInstance()->GetElement(parent_handle);
    if (parent == NULL || (!parent->IsReady()))
    {
      (*result) = "Parent not available. Cancelling remote module creation.";
      FINROC_LOG_PRINT(ERROR, *result);
    }
    else if ((!tRuntimeSettings::DuplicateQualifiedNamesAllowed()) && parent->GetChild(*name))
    {
      (*result) = std::string("Element with name '") + *name + "' already exists. Creating another module with this name is not allowed.";
      FINROC_LOG_PRINT(ERROR, *result);
    }
    else
    {
      FINROC_LOG_PRINT(USER, "Creating Module ", parent->GetQualifiedLink(), "/", *name);

      if (cma->GetParameterTypes() != NULL && cma->GetParameterTypes()->Size() > 0)
      {
        params = cma->GetParameterTypes()->Instantiate();
        rrlib::serialization::tInputStream ci(params_buffer.get());
        for (size_t i = 0u; i < params->Size(); i++)
        {
          tStaticParameterBase* param = params->Get(i);
          try
          {
            param->DeserializeValue(ci);
          }
          catch (const std::exception& e)
          {
            (*result) = std::string("Error parsing value for parameter ") + param->GetName();
            FINROC_LOG_PRINT(ERROR, *result);
            FINROC_LOG_PRINT(ERROR, e);
          }
        }
        ci.Close();
      }
      core::tFrameworkElement* created = cma->CreateModule(parent, *name, params);
      tFinstructableGroup::SetFinstructed(*created, cma, params);
      created->Init();
      params = NULL;

      FINROC_LOG_PRINT(USER, "Creating Module succeeded");
    }
  }
  catch (const std::exception& e)
  {
    FINROC_LOG_PRINT(ERROR, e);
    (*result) = e.what();
  }

  // if something's gone wrong, delete parameter list
  delete params;
  return result;
}

void tAdminServer::HandleVoidCall(const tAbstractMethod& method, int p1, int p2)
{
  tRuntimeEnvironment* re = tRuntimeEnvironment::GetInstance();
  tAbstractPort* src = re->GetPort(p1);

  if (method == cDISCONNECT_ALL)
  {
    if (src == NULL)
    {
      FINROC_LOG_PRINT(WARNING, "Can't disconnect port that doesn't exist");
      return;
    }
    src->DisconnectAll();
    FINROC_LOG_PRINT(USER, "Disconnected port ", src->GetQualifiedName());
    return;
  }

  tAbstractPort* dest = re->GetPort(p2);
  if (src == NULL || dest == NULL)
  {
    FINROC_LOG_PRINT(WARNING, "Can't (dis)connect ports that do not exists");
    return;
  }
  if (method == cCONNECT)
  {
    if (src->IsVolatile() && dest->IsVolatile())
    {
      FINROC_LOG_PRINT(WARNING, "Cannot really persistently connect two network ports: ", src->GetQualifiedLink(), ", ", dest->GetQualifiedLink());
    }
    Connect(*src, *dest);
    if (!src->IsConnectedTo(*dest))
    {
      FINROC_LOG_PRINT(WARNING, "Could not connect ports '", src->GetQualifiedName(), "' and '", dest->GetQualifiedName(), "'.");
    }
    else
    {
      FINROC_LOG_PRINT(USER, "Connected ports ", src->GetQualifiedName(), " ", dest->GetQualifiedName());
    }
  }
  else if (method == cDISCONNECT)
  {
    if (src->IsVolatile())
    {
      dest->DisconnectFrom(src->GetQualifiedLink());
    }
    if (dest->IsVolatile())
    {
      src->DisconnectFrom(dest->GetQualifiedLink());
    }
    src->DisconnectFrom(*dest);
    if (src->IsConnectedTo(*dest))
    {
      FINROC_LOG_PRINT(WARNING, "Could not disconnect ports ", src->GetQualifiedName(), " ", dest->GetQualifiedName());
    }
    else
    {
      FINROC_LOG_PRINT(USER, "Disconnected ports ", src->GetQualifiedName(), " ", dest->GetQualifiedName());
    }
  }
}

void tAdminServer::HandleVoidCall(const tAbstractMethod& method, int cma_index, tPortDataPtr<std::string>& name, int parent_handle, tPortDataPtr<const rrlib::serialization::tMemoryBuffer>& params_buffer)
{
  assert(method == cSET_ANNOTATION);
  assert(!name);
  ::finroc::core::tFrameworkElement* elem = tRuntimeEnvironment::GetInstance()->GetElement(cma_index);
  if (elem == NULL || (!elem->IsReady()))
  {
    FINROC_LOG_PRINT(ERROR, "Parent not available. Cancelling setting of annotation.");
  }
  else
  {
    rrlib::serialization::tInputStream ci(params_buffer.get(), rrlib::serialization::tTypeEncoding::NAMES);
    rrlib::rtti::tDataTypeBase dt;
    ci >> dt;
    if (dt == NULL)
    {
      FINROC_LOG_PRINT(ERROR, "Data type not available. Cancelling setting of annotation.");
    }
    else
    {
      tFinrocAnnotation* ann = elem->GetAnnotation(dt);
      if (ann == NULL)
      {
        FINROC_LOG_PRINT(ERROR, "Creating new annotations not supported yet. Cancelling setting of annotation.");
      }
      else if (ann->GetType() != dt)
      {
        FINROC_LOG_PRINT(ERROR, "Existing annotation has wrong type?!. Cancelling setting of annotation.");
      }
      else
      {
        ann->Deserialize(ci);
      }
    }
    ci.Close();
  }
}

void tAdminServer::HandleVoidCall(const tAbstractMethod& method, int handle)
{
  if (method == cDELETE_ELEMENT)
  {
    ::finroc::core::tFrameworkElement* fe = GetRuntime().GetElement(handle);
    if (fe != NULL && (!fe->IsDeleted()))
    {
      FINROC_LOG_PRINT(USER, "Deleting element ", fe->GetQualifiedLink());
      fe->ManagedDelete();
    }
    else
    {
      FINROC_LOG_PRINT(ERROR, "Could not delete Framework element, because it does not appear to be available.");
    }
    return;
  }

  if (method == cSTART_EXECUTION || method == cPAUSE_EXECUTION)
  {
    std::vector<tExecutionControl*> ecs;
    GetExecutionControls(ecs, handle);
    if (ecs.size() == 0)
    {
      FINROC_LOG_PRINT(WARNING, "Start/Pause command has not effect");
    }
    if (method == cSTART_EXECUTION)
    {
      for (auto it = ecs.begin(); it < ecs.end(); it++)
      {
        if (!(*it)->IsRunning())
        {
          (*it)->Start();
        }
      }
    }
    else if (method == cPAUSE_EXECUTION)
    {
      for (auto it = ecs.begin(); it < ecs.end(); it++)
      {
        if ((*it)->IsRunning())
        {
          (*it)->Pause();
        }
      }
    }
    return;
  }

  assert(method == cSAVE_FINSTRUCTABLE_GROUP);
  ::finroc::core::tFrameworkElement* fe = GetRuntime().GetElement(handle);
  if (fe != NULL && fe->IsReady() && fe->GetFlag(tCoreFlags::cFINSTRUCTABLE_GROUP))
  {
    try
    {
      (static_cast<tFinstructableGroup*>(fe))->SaveXml();
    }
    catch (const util::tException& e)
    {
      FINROC_LOG_PRINT(ERROR, "Error saving finstructable group ", fe->GetQualifiedLink());
      FINROC_LOG_PRINT(ERROR, e);
    }
  }
  else
  {
    FINROC_LOG_PRINT(ERROR, "Could not save finstructable group, because it does not appear to be available.");
  }
}

} // namespace finroc
} // namespace core

