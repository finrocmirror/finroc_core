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
#include "core/admin/tAdminServer.h"
#include "core/tCoreFlags.h"
#include "core/port/tAbstractPort.h"
#include "rrlib/serialization/tOutputStream.h"
#include "rrlib/finroc_core_utils/container/tSimpleList.h"
#include "core/plugin/tCreateFrameworkElementAction.h"
#include "core/plugin/tPlugins.h"
#include "core/parameter/tStructureParameterList.h"
#include "core/tFrameworkElement.h"
#include "core/tRuntimeEnvironment.h"
#include "core/tFinrocAnnotation.h"
#include "rrlib/serialization/tDataTypeBase.h"
#include "core/parameter/tConfigFile.h"
#include "core/tFrameworkElementTreeFilter.h"
#include "core/thread/tExecutionControl.h"
#include "core/portdatabase/tFinrocTypeInfo.h"
#include "core/port/cc/tCCPortBase.h"
#include "rrlib/serialization/tGenericObject.h"
#include "rrlib/serialization/sSerialization.h"
#include "core/port/std/tPortBase.h"
#include "core/port/std/tPortDataManager.h"
#include "rrlib/serialization/tInputStream.h"
#include "core/port/cc/tCCPortDataManagerTL.h"
#include "core/port/tThreadLocalCache.h"
#include "rrlib/serialization/tStringInputStream.h"
#include "rrlib/finroc_core_utils/log/tLogUser.h"
#include "core/parameter/tConstructorParameters.h"
#include "core/parameter/tStructureParameterBase.h"
#include "core/finstructable/tFinstructableGroup.h"
#include "core/parameter/tParameterInfo.h"

namespace finroc
{
namespace core
{
tPortInterface tAdminServer::cMETHODS("Admin Interface", true);
tVoid2Method<tAdminServer*, int, int> tAdminServer::cCONNECT(tAdminServer::cMETHODS, "Connect", "source port handle", "destination port handle", false);
tVoid2Method<tAdminServer*, int, int> tAdminServer::cDISCONNECT(tAdminServer::cMETHODS, "Disconnect", "source port handle", "destination port handle", false);
tVoid2Method<tAdminServer*, int, int> tAdminServer::cDISCONNECT_ALL(tAdminServer::cMETHODS, "DisconnectAll", "source port handle", "dummy", false);
tVoid3Method<tAdminServer*, int, tPortDataPtr<const rrlib::serialization::tMemoryBuffer>, int> tAdminServer::cSET_PORT_VALUE(tAdminServer::cMETHODS, "SetPortValue", "port handle", "data", "as string?", false);
tPort0Method<tAdminServer*, tPortDataPtr<rrlib::serialization::tMemoryBuffer> > tAdminServer::cGET_CREATE_MODULE_ACTIONS(tAdminServer::cMETHODS, "GetCreateModuleActions", false);
tVoid4Method<tAdminServer*, int, tPortDataPtr<tCoreString>, int, tPortDataPtr<const rrlib::serialization::tMemoryBuffer> > tAdminServer::cCREATE_MODULE(tAdminServer::cMETHODS, "CreateModule", "create action index", "module name", "parent handle", "module creation parameters", false);
tVoid1Method<tAdminServer*, int> tAdminServer::cSAVE_FINSTRUCTABLE_GROUP(tAdminServer::cMETHODS, "Save Finstructable Group", "finstructable handle", false);
tPort2Method<tAdminServer*, tPortDataPtr<rrlib::serialization::tMemoryBuffer>, int, tPortDataPtr<tCoreString> > tAdminServer::cGET_ANNOTATION(tAdminServer::cMETHODS, "Get Annotation", "handle", "annotation type", false);
tVoid4Method<tAdminServer*, int, tPortDataPtr<tCoreString>, int, tPortDataPtr<const rrlib::serialization::tMemoryBuffer> > tAdminServer::cSET_ANNOTATION(tAdminServer::cMETHODS, "Set Annotation", "handle", "dummy", "dummy", "annotation", false);
tVoid1Method<tAdminServer*, int> tAdminServer::cDELETE_ELEMENT(tAdminServer::cMETHODS, "Delete Framework element", "handle", false);
tVoid0Method<tAdminServer*> tAdminServer::cSTART_EXECUTION(tAdminServer::cMETHODS, "Start execution", false);
tVoid0Method<tAdminServer*> tAdminServer::cPAUSE_EXECUTION(tAdminServer::cMETHODS, "Pause execution", false);
tPort1Method<tAdminServer*, int, int> tAdminServer::cIS_RUNNING(tAdminServer::cMETHODS, "Is Framework element running", "handle", false);
tPort3Method<tAdminServer*, tPortDataPtr<tCoreString>, int, int, int > tAdminServer::cGET_PORT_VALUE_AS_STRING(tAdminServer::cMETHODS, "Get port value as string", "handle", "dummy", "dummy", false);
tPort2Method<tAdminServer*, tPortDataPtr<rrlib::serialization::tMemoryBuffer>, int, tPortDataPtr<tCoreString> > tAdminServer::cGET_PARAMETER_INFO(tAdminServer::cMETHODS, "Get Annotation", "handle", "annotation type", false);
tRPCInterfaceType tAdminServer::cDATA_TYPE("Administration method calls", &(tAdminServer::cMETHODS));
util::tString tAdminServer::cPORT_NAME = "Administration";
util::tString tAdminServer::cQUALIFIED_PORT_NAME = "Unrelated/Administration";

tAdminServer::tAdminServer() :
    tInterfaceServerPort(cPORT_NAME, NULL, cDATA_TYPE, NULL, tCoreFlags::cSHARED)
{
  SetCallHandler(this);
}

void tAdminServer::Connect(tAbstractPort* src, tAbstractPort* dest)
{
  if (src->IsVolatile())
  {
    dest->ConnectToSource(src->GetQualifiedLink());
  }
  else if (dest->IsVolatile())
  {
    src->ConnectToTarget(dest->GetQualifiedLink());
  }
  else
  {
    src->ConnectToTarget(dest);
  }
}

tPortDataPtr<rrlib::serialization::tMemoryBuffer> tAdminServer::HandleCall(tAbstractMethod* method)
{
  assert((method == &(cGET_CREATE_MODULE_ACTIONS)));

  tPortDataPtr<rrlib::serialization::tMemoryBuffer> mb = this->GetBufferForReturn<rrlib::serialization::tMemoryBuffer>();
  rrlib::serialization::tOutputStream co(mb.get(), rrlib::serialization::tOutputStream::eNames);
  const util::tSimpleList<tCreateFrameworkElementAction*>& module_types = tPlugins::GetInstance()->GetModuleTypes();
  for (size_t i = 0u; i < module_types.Size(); i++)
  {
    const tCreateFrameworkElementAction& cma = *module_types.Get(i);
    co.WriteString(cma.GetName());
    co.WriteString(cma.GetModuleGroup());
    if (cma.GetParameterTypes() != NULL)
    {
      cma.GetParameterTypes()->Serialize(co);
    }
    else
    {
      tStructureParameterList::cEMPTY.Serialize(co);
    }
  }
  co.Close();
  return mb;
}

tPortDataPtr<rrlib::serialization::tMemoryBuffer> tAdminServer::HandleCall(tAbstractMethod* method, int handle, tPortDataPtr<tCoreString>& type)
{
  if (method == &(cGET_ANNOTATION))
  {
    ::finroc::core::tFrameworkElement* fe = GetRuntime()->GetElement(handle);
    tFinrocAnnotation* result = NULL;
    rrlib::serialization::tDataTypeBase dt = rrlib::serialization::tDataTypeBase::FindType(type->ToString());
    if (fe != NULL && fe->IsReady() && dt != NULL)
    {
      result = fe->GetAnnotation(dt);
    }
    else
    {
      FINROC_LOG_STREAM(rrlib::logging::eLL_ERROR, log_domain, "Could not query element for annotation type ", type->ToString());
    }

    if (result == NULL)
    {
      return tPortDataPtr<rrlib::serialization::tMemoryBuffer>();
    }
    else
    {
      tPortDataPtr<rrlib::serialization::tMemoryBuffer> buf = this->GetBufferForReturn<rrlib::serialization::tMemoryBuffer>();
      rrlib::serialization::tOutputStream co(buf.get(), rrlib::serialization::tOutputStream::eNames);
      co.WriteType(result->GetType());
      result->Serialize(co);
      co.Close();
      return buf;
    }
  }
  else
  {
    assert((method == &(cGET_PARAMETER_INFO)));

    ::finroc::core::tFrameworkElement* fe = GetRuntime()->GetElement(handle);
    if (fe == NULL || (!fe->IsReady()))
    {
      FINROC_LOG_STREAM(rrlib::logging::eLL_ERROR, log_domain, "Could not get parameter info for framework element ", handle);

      return tPortDataPtr<rrlib::serialization::tMemoryBuffer>();
    }

    tConfigFile* cf = tConfigFile::Find(fe);
    tPortDataPtr<rrlib::serialization::tMemoryBuffer> buf = this->GetBufferForReturn<rrlib::serialization::tMemoryBuffer>();
    rrlib::serialization::tOutputStream co(buf.get(), rrlib::serialization::tOutputStream::eNames);
    if (cf == NULL)
    {
      co.WriteBoolean(false);
    }
    else
    {
      co.WriteBoolean(true);
      tCallbackParameters params(cf, &(co));
      co.WriteInt((static_cast< ::finroc::core::tFrameworkElement*>(cf->GetAnnotated()))->GetHandle());
      cf->Serialize(co);
      tFrameworkElementTreeFilter filter;
      filter.TraverseElementTree(fe, this, params);
    }
    co.Close();
    return buf;
  }
}

int tAdminServer::HandleCall(const tAbstractMethod* method, int handle)
{
  assert((method == &(cIS_RUNNING)));
  ::finroc::core::tFrameworkElement* fe = GetRuntime()->GetElement(handle);
  if (fe != NULL && (fe->IsReady()))
  {
    tExecutionControl* ec = tExecutionControl::Find(fe);
    if (ec == NULL)
    {
      return 0;
    }
    return ec->IsRunning() ? 1 : 0;
  }
  return -1;
}

tPortDataPtr<tCoreString> tAdminServer::HandleCall(const tAbstractMethod* method, int p1, int p2, int p3)
{
  assert((method == &(cGET_PORT_VALUE_AS_STRING)));
  ::finroc::core::tAbstractPort* ap = GetRuntime()->GetPort(p1);
  if (ap != NULL && ap->IsReady())
  {
    if (tFinrocTypeInfo::IsCCType(ap->GetDataType()))
    {
      tPortDataPtr<tCoreString> cs = this->GetBufferForReturn<tCoreString>();
      tCCPortBase* p = static_cast<tCCPortBase*>(ap);
      const rrlib::serialization::tGenericObject* go = p->GetAutoLockedRaw();
      cs->Set(rrlib::serialization::sSerialization::Serialize(*go));
      p->ReleaseAutoLocks();
      return cs;
    }
    else if (tFinrocTypeInfo::IsStdType(ap->GetDataType()))
    {
      tPortDataPtr<tCoreString> cs = this->GetBufferForReturn<tCoreString>();
      tPortBase* p = static_cast<tPortBase*>(ap);
      tPortDataManager* go = p->GetAutoLockedRaw();
      cs->Set(rrlib::serialization::sSerialization::Serialize(*go->GetObject()));
      p->ReleaseAutoLocks();
      return cs;
    }
  }

  return tPortDataPtr<tCoreString>();
}

void tAdminServer::HandleVoidCall(const tAbstractMethod* method, int p1, int p2)
{
  tRuntimeEnvironment* re = tRuntimeEnvironment::GetInstance();
  ::finroc::core::tAbstractPort* src = re->GetPort(p1);

  if (method == &(cDISCONNECT_ALL))
  {
    if (src == NULL)
    {
      FINROC_LOG_STREAM(rrlib::logging::eLL_WARNING, log_domain, "Can't disconnect port that doesn't exist");
      return;
    }
    src->DisconnectAll();
    FINROC_LOG_STREAM(rrlib::logging::eLL_USER, log_domain, "Disconnected port ", src->GetQualifiedName());
    return;
  }

  ::finroc::core::tAbstractPort* dest = re->GetPort(p2);
  if (src == NULL || dest == NULL)
  {
    FINROC_LOG_STREAM(rrlib::logging::eLL_WARNING, log_domain, "Can't (dis)connect ports that do not exists");
    return;
  }
  if (method == &(cCONNECT))
  {
    if (src->IsVolatile() && dest->IsVolatile())
    {
      FINROC_LOG_STREAM(rrlib::logging::eLL_WARNING, log_domain, "Cannot really persistently connect two network ports: ", src->GetQualifiedLink(), ", ", dest->GetQualifiedLink());
    }
    if (src->MayConnectTo(dest))
    {
      Connect(src, dest);
    }
    else if (dest->MayConnectTo(src))
    {
      Connect(dest, src);
    }
    if (!src->IsConnectedTo(dest))
    {
      FINROC_LOG_STREAM(rrlib::logging::eLL_WARNING, log_domain, "Could not connect ports ", src->GetQualifiedName(), " ", dest->GetQualifiedName());
    }
    else
    {
      FINROC_LOG_STREAM(rrlib::logging::eLL_USER, log_domain, "Connected ports ", src->GetQualifiedName(), " ", dest->GetQualifiedName());
    }
  }
  else if (method == &(cDISCONNECT))
  {
    if (src->IsVolatile())
    {
      dest->DisconnectFrom(src->GetQualifiedLink());
    }
    if (dest->IsVolatile())
    {
      src->DisconnectFrom(dest->GetQualifiedLink());
    }
    src->DisconnectFrom(dest);
    if (src->IsConnectedTo(dest))
    {
      FINROC_LOG_STREAM(rrlib::logging::eLL_WARNING, log_domain, "Could not disconnect ports ", src->GetQualifiedName(), " ", dest->GetQualifiedName());
    }
    else
    {
      FINROC_LOG_STREAM(rrlib::logging::eLL_USER, log_domain, "Disconnected ports ", src->GetQualifiedName(), " ", dest->GetQualifiedName());
    }
  }
}

void tAdminServer::HandleVoidCall(tAbstractMethod* method, int port_handle, tPortDataPtr<const rrlib::serialization::tMemoryBuffer>& buf, int as_string)
{
  assert((method == &(cSET_PORT_VALUE)));
  ::finroc::core::tAbstractPort* port = tRuntimeEnvironment::GetInstance()->GetPort(port_handle);
  if (port != NULL && port->IsReady())
  {
    {
      util::tLock lock3(port);
      if (port->IsReady())
      {
        rrlib::serialization::tInputStream ci(buf.get(), rrlib::serialization::tInputStream::eNames);
        rrlib::serialization::tDataTypeBase dt = ci.ReadType();
        if (tFinrocTypeInfo::IsCCType(port->GetDataType()) && tFinrocTypeInfo::IsCCType(dt))
        {
          tCCPortBase* p = static_cast<tCCPortBase*>(port);
          tCCPortDataManagerTL* c = tThreadLocalCache::Get()->GetUnusedBuffer(dt);
          if (as_string == 0)
          {
            c->GetObject()->Deserialize(ci);
          }
          else
          {
            util::tString s = ci.ReadString();
            rrlib::serialization::tStringInputStream sis(s);
            try
            {
              c->GetObject()->Deserialize(sis);
            }
            catch (const util::tException& e)
            {
              c->RecycleUnused();
              FINROC_LOG_STREAM(rrlib::logging::eLL_WARNING, log_domain, "Cannot deserialize from string ", s, e);
            }
          }
          p->BrowserPublishRaw(c);
        }
        else if (tFinrocTypeInfo::IsStdType(port->GetDataType()) && tFinrocTypeInfo::IsStdType(dt))
        {
          tPortBase* p = static_cast<tPortBase*>(port);
          tPortDataManager* port_data = p->GetDataType() != dt ? p->GetUnusedBufferRaw(dt) : p->GetUnusedBufferRaw();
          if (as_string == 0)
          {
            port_data->GetObject()->Deserialize(ci);
          }
          else
          {
            util::tString s = ci.ReadString();
            rrlib::serialization::tStringInputStream sis(s);
            try
            {
              port_data->GetObject()->Deserialize(sis);
            }
            catch (const util::tException& e)
            {
              port_data->RecycleUnused();
              FINROC_LOG_STREAM(rrlib::logging::eLL_WARNING, log_domain, "Cannot deserialize from string ", s, e);
            }
          }
          p->BrowserPublish(port_data);
          port_data = NULL;
        }
      }
    }
  }
}

void tAdminServer::HandleVoidCall(tAbstractMethod* method, int cma_index, tPortDataPtr<tCoreString>& name, int parent_handle, tPortDataPtr<const rrlib::serialization::tMemoryBuffer>& params_buffer)
{
  tConstructorParameters* params = NULL;
  if (method == &(cSET_ANNOTATION))
  {
    assert(!name);
    ::finroc::core::tFrameworkElement* elem = tRuntimeEnvironment::GetInstance()->GetElement(cma_index);
    if (elem == NULL || (!elem->IsReady()))
    {
      FINROC_LOG_STREAM(rrlib::logging::eLL_ERROR, log_domain, "Parent not available. Cancelling setting of annotation.");
    }
    else
    {
      rrlib::serialization::tInputStream ci(params_buffer.get(), rrlib::serialization::tInputStream::eNames);
      rrlib::serialization::tDataTypeBase dt = ci.ReadType();
      if (dt == NULL)
      {
        FINROC_LOG_STREAM(rrlib::logging::eLL_ERROR, log_domain, "Data type not available. Cancelling setting of annotation.");
      }
      else
      {
        tFinrocAnnotation* ann = elem->GetAnnotation(dt);
        if (ann == NULL)
        {
          FINROC_LOG_STREAM(rrlib::logging::eLL_ERROR, log_domain, "Creating new annotations not supported yet. Cancelling setting of annotation.");
        }
        else if (ann->GetType() != dt)
        {
          FINROC_LOG_STREAM(rrlib::logging::eLL_ERROR, log_domain, "Existing annotation has wrong type?!. Cancelling setting of annotation.");
        }
        else
        {
          ann->Deserialize(ci);
        }
      }
      ci.Close();
    }

  }
  else if (method == &(cCREATE_MODULE))
  {
    try
    {
      {
        util::tLock lock4(GetRegistryLock());
        tCreateFrameworkElementAction* cma = tPlugins::GetInstance()->GetModuleTypes().Get(cma_index);
        ::finroc::core::tFrameworkElement* parent = tRuntimeEnvironment::GetInstance()->GetElement(parent_handle);
        if (parent == NULL || (!parent->IsReady()))
        {
          FINROC_LOG_STREAM(rrlib::logging::eLL_ERROR, log_domain, "Parent not available. Cancelling remote module creation.");
        }
        else
        {
          FINROC_LOG_STREAM(rrlib::logging::eLL_USER, log_domain, "Creating Module ", parent->GetQualifiedLink(), "/", name->ToString());

          if (cma->GetParameterTypes() != NULL && cma->GetParameterTypes()->Size() > 0)
          {
            params = cma->GetParameterTypes()->Instantiate();
            rrlib::serialization::tInputStream ci(params_buffer.get());
            for (size_t i = 0u; i < params->Size(); i++)
            {
              tStructureParameterBase* param = params->Get(i);
              util::tString s = ci.ReadString();
              try
              {
                param->Set(s);
              }
              catch (const util::tException& e)
              {
                FINROC_LOG_STREAM(rrlib::logging::eLL_ERROR, log_domain, "Error parsing '", s, "' for parameter ", param->GetName());
                FINROC_LOG_STREAM(rrlib::logging::eLL_ERROR, log_domain, e);
              }
            }
            ci.Close();
          }
          ::finroc::core::tFrameworkElement* created = cma->CreateModule(parent, name->ToString(), params);
          created->SetFinstructed(cma, params);
          created->Init();
          params = NULL;

          FINROC_LOG_STREAM(rrlib::logging::eLL_USER, log_domain, "Creating Module succeeded");
        }
      }
    }
    catch (const util::tException& e)
    {
      FINROC_LOG_STREAM(rrlib::logging::eLL_ERROR, log_domain, e);
    }
  }

  // if something's gone wrong, delete parameter list
  if (params != NULL)
  {
    delete params;
  }

  // release locks

  // release locks

}

void tAdminServer::HandleVoidCall(const tAbstractMethod* method, int handle)
{
  if (method == &(cDELETE_ELEMENT))
  {
    ::finroc::core::tFrameworkElement* fe = GetRuntime()->GetElement(handle);
    if (fe != NULL && (!fe->IsDeleted()))
    {
      FINROC_LOG_STREAM(rrlib::logging::eLL_USER, log_domain, "Deleting element ", fe->GetQualifiedLink());
      fe->ManagedDelete();
    }
    else
    {
      FINROC_LOG_STREAM(rrlib::logging::eLL_ERROR, log_domain, "Could not delete Framework element, because it does not appear to be available.");
    }
    return;
  }

  assert((method == &(cSAVE_FINSTRUCTABLE_GROUP)));
  ::finroc::core::tFrameworkElement* fe = GetRuntime()->GetElement(handle);
  if (fe != NULL && fe->IsReady() && fe->GetFlag(tCoreFlags::cFINSTRUCTABLE_GROUP))
  {
    try
    {
      (static_cast<tFinstructableGroup*>(fe))->SaveXml();
    }
    catch (const util::tException& e)
    {
      FINROC_LOG_STREAM(rrlib::logging::eLL_ERROR, log_domain, "Error saving finstructable group ", fe->GetQualifiedLink());
      FINROC_LOG_STREAM(rrlib::logging::eLL_ERROR, log_domain, e);
    }
  }
  else
  {
    FINROC_LOG_STREAM(rrlib::logging::eLL_ERROR, log_domain, "Could not save finstructable group, because it does not appear to be available.");
  }
}

void tAdminServer::HandleVoidCall(const tAbstractMethod* method)
{
  assert((method == &(cSTART_EXECUTION) || method == &(cPAUSE_EXECUTION)));
  if (method == &(cSTART_EXECUTION))
  {
    GetRuntime()->StartExecution();
  }
  else if (method == &(cPAUSE_EXECUTION))
  {
    GetRuntime()->StopExecution();
  }

}

void tAdminServer::TreeFilterCallback(tFrameworkElement* fe, const tCallbackParameters& custom_param)
{
  tConfigFile* cf = static_cast<tConfigFile*>(fe->GetAnnotation(tConfigFile::cTYPE));
  if (cf != NULL)
  {
    custom_param.co->WriteByte(1);
    custom_param.co->WriteInt(fe->GetHandle());
    custom_param.co->WriteString(cf->GetFilename());
    custom_param.co->WriteBoolean(cf->IsActive());
  }
  else
  {
    tParameterInfo* pi = static_cast<tParameterInfo*>(fe->GetAnnotation(tParameterInfo::cTYPE));
    if (pi != NULL && custom_param.cf == tConfigFile::Find(fe))
    {
      custom_param.co->WriteByte(2);
      custom_param.co->WriteInt(fe->GetHandle());
      custom_param.co->WriteString(pi->GetConfigEntry());
    }
  }
}

} // namespace finroc
} // namespace core

