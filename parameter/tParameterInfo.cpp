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
#include "rrlib/finroc_core_utils/log/tLogUser.h"
#include "rrlib/xml/tNode.h"
#include "rrlib/serialization/serialization.h"

#include "core/parameter/tParameterInfo.h"
#include "core/tFrameworkElement.h"
#include "core/port/tAbstractPort.h"
#include "core/parameter/tConfigFile.h"
#include "core/parameter/tConfigNode.h"
#include "core/portdatabase/tFinrocTypeInfo.h"
#include "core/port/cc/tCCPortBase.h"
#include "core/port/cc/tCCPortDataManagerTL.h"
#include "core/port/tThreadLocalCache.h"
#include "core/port/std/tPortBase.h"
#include "core/port/std/tPortDataManager.h"
#include "core/port/cc/tCCPortDataManager.h"
#include "core/tRuntimeEnvironment.h"

namespace finroc
{
namespace core
{
rrlib::rtti::tDataTypeBase tParameterInfo::cTYPE = rrlib::rtti::tDataType<tParameterInfo>();

tParameterInfo::tParameterInfo() :
  config_entry(),
  entry_set_from_finstruct(false),
  command_line_option(),
  finstruct_default()
{}

void tParameterInfo::AnnotatedObjectInitialized()
{
  try
  {
    LoadValue(true);
  }
  catch (const util::tException& e)
  {
    FINROC_LOG_PRINT(rrlib::logging::eLL_ERROR, e);
  }
}

void tParameterInfo::Deserialize(rrlib::serialization::tInputStream& is)
{
  entry_set_from_finstruct = is.ReadBoolean();
  util::tString config_entry_tmp = is.ReadString();
  util::tString command_line_option_tmp = is.ReadString();
  util::tString finstruct_default_tmp = is.ReadString();
  bool same = boost::equals(config_entry_tmp, config_entry) && boost::equals(command_line_option_tmp, command_line_option) && boost::equals(finstruct_default_tmp, finstruct_default);
  config_entry = config_entry_tmp;
  command_line_option = command_line_option_tmp;
  finstruct_default = finstruct_default_tmp;

  if (!same)
  {
    try
    {
      LoadValue();
    }
    catch (std::exception& e)
    {
      FINROC_LOG_PRINT(rrlib::logging::eLL_ERROR, e);
    }
  }
}

void tParameterInfo::Deserialize(const rrlib::xml::tNode& node)
{
  Deserialize(node, false, true);
}

void tParameterInfo::Deserialize(const rrlib::xml::tNode& node, bool finstruct_context, bool include_commmand_line)
{
  if (node.HasAttribute("config"))
  {
    config_entry = node.GetStringAttribute("config");
    entry_set_from_finstruct = finstruct_context;
  }
  else
  {
    config_entry = "";
  }
  if (include_commmand_line)
  {
    if (node.HasAttribute("cmdline"))
    {
      command_line_option = node.GetStringAttribute("cmdline");
    }
    else
    {
      command_line_option = "";
    }
  }
  if (node.HasAttribute("default"))
  {
    finstruct_default = node.GetStringAttribute("default");
  }
  else
  {
    finstruct_default = "";
  }
}

bool tParameterInfo::IsFinstructableGroupResponsibleForConfigFileConnections(const tFrameworkElement& finstructable_group, const tFrameworkElement& ap)
{
  tConfigFile* cf = tConfigFile::Find(ap);
  if (cf == NULL)
  {
    return finstructable_group.GetParentWithFlags(tCoreFlags::cFINSTRUCTABLE_GROUP) == NULL;
  }
  tFrameworkElement* config_element = static_cast<tFrameworkElement*>(cf->GetAnnotated());
  const tFrameworkElement* responsible = config_element->GetFlag(tCoreFlags::cFINSTRUCTABLE_GROUP) ? config_element : config_element->GetParentWithFlags(tCoreFlags::cFINSTRUCTABLE_GROUP);
  if (!responsible)
  {
    // ok, config file is probably attached to runtime. Choose outer-most finstructable group.
    responsible = &finstructable_group;
    const tFrameworkElement* tmp;
    while ((tmp = responsible->GetParentWithFlags(tCoreFlags::cFINSTRUCTABLE_GROUP)) != NULL)
    {
      responsible = tmp;
    }
  }
  return responsible == &finstructable_group;
}

void tParameterInfo::LoadValue(bool ignore_ready)
{
  tAbstractPort* ann = static_cast<tAbstractPort*>(GetAnnotated());
  {
    util::tLock lock2(ann->GetRegistryLock());
    if (ann != NULL && (ignore_ready || ann->IsReady()))
    {
      // command line option
      if (command_line_option.length() > 0)
      {
        util::tString arg = tRuntimeEnvironment::GetInstance()->GetCommandLineArgument(command_line_option);
        if (arg.length() > 0)
        {
          rrlib::serialization::tStringInputStream sis(arg);
          if (tFinrocTypeInfo::IsCCType(ann->GetDataType()))
          {
            tCCPortBase* port = static_cast<tCCPortBase*>(ann);
            tCCPortDataManagerTL* c = tThreadLocalCache::Get()->GetUnusedBuffer(port->GetDataType());
            try
            {
              c->GetObject()->Deserialize(sis);
              port->BrowserPublishRaw(c);
              return;
            }
            catch (const util::tException& e)
            {
              FINROC_LOG_PRINT(rrlib::logging::eLL_ERROR, "Failed to load parameter '", ann->GetQualifiedName(), "' from command line argument '", arg, "': ", e);
              c->RecycleUnused();
            }
          }
          else if (tFinrocTypeInfo::IsStdType(ann->GetDataType()))
          {
            tPortBase* port = static_cast<tPortBase*>(ann);
            tPortDataManager* pd = port->GetUnusedBufferRaw();
            try
            {
              pd->GetObject()->Deserialize(sis);
              port->BrowserPublish(pd);
              return;
            }
            catch (const util::tException& e)
            {
              FINROC_LOG_PRINT(rrlib::logging::eLL_ERROR, "Failed to load parameter '", ann->GetQualifiedName(), "' from command line argument '", arg, "': ", e);
              pd->RecycleUnused();
            }
          }
          else
          {
            throw util::tRuntimeException("Port Type not supported as a parameter", CODE_LOCATION_MACRO);
          }
        }
      }

      // config file entry
      tConfigFile* cf = tConfigFile::Find(*ann);
      if (cf != NULL && config_entry.length() > 0)
      {
        util::tString full_config_entry = tConfigNode::GetFullConfigEntry(*ann, config_entry);
        if (cf->HasEntry(full_config_entry))
        {
          rrlib::xml::tNode& node = cf->GetEntry(full_config_entry, false);
          if (tFinrocTypeInfo::IsCCType(ann->GetDataType()))
          {
            tCCPortBase* port = static_cast<tCCPortBase*>(ann);
            tCCPortDataManagerTL* c = tThreadLocalCache::Get()->GetUnusedBuffer(port->GetDataType());
            try
            {
              c->GetObject()->Deserialize(node);
              port->BrowserPublishRaw(c);
              return;
            }
            catch (const util::tException& e)
            {
              FINROC_LOG_PRINT(rrlib::logging::eLL_ERROR, "Failed to load parameter '", ann->GetQualifiedName(), "' from config entry '", full_config_entry, "': ", e);
              c->RecycleUnused();
            }
          }
          else if (tFinrocTypeInfo::IsStdType(ann->GetDataType()))
          {
            tPortBase* port = static_cast<tPortBase*>(ann);
            tPortDataManager* pd = port->GetUnusedBufferRaw();
            try
            {
              pd->GetObject()->Deserialize(node);
              port->BrowserPublish(pd);
              return;
            }
            catch (const util::tException& e)
            {
              FINROC_LOG_PRINT(rrlib::logging::eLL_ERROR, "Failed to load parameter '", ann->GetQualifiedName(), "' from config entry '", full_config_entry, "': ", e);
              pd->RecycleUnused();
            }
          }
          else
          {
            throw util::tRuntimeException("Port Type not supported as a parameter", CODE_LOCATION_MACRO);
          }
        }
      }

      // finstruct default
      if (finstruct_default.length() > 0)
      {
        rrlib::serialization::tStringInputStream sis(finstruct_default);
        if (tFinrocTypeInfo::IsCCType(ann->GetDataType()))
        {
          tCCPortBase* port = static_cast<tCCPortBase*>(ann);
          tCCPortDataManagerTL* c = tThreadLocalCache::Get()->GetUnusedBuffer(port->GetDataType());
          try
          {
            c->GetObject()->Deserialize(sis);
            port->BrowserPublishRaw(c);
            return;
          }
          catch (const util::tException& e)
          {
            FINROC_LOG_PRINT(rrlib::logging::eLL_ERROR, "Failed to load parameter '", ann->GetQualifiedName(), "' from finstruct default '", finstruct_default, "': ", e);
            c->RecycleUnused();
          }
        }
        else if (tFinrocTypeInfo::IsStdType(ann->GetDataType()))
        {
          tPortBase* port = static_cast<tPortBase*>(ann);
          tPortDataManager* pd = port->GetUnusedBufferRaw();
          try
          {
            pd->GetObject()->Deserialize(sis);
            port->BrowserPublish(pd);
            return;
          }
          catch (const util::tException& e)
          {
            FINROC_LOG_PRINT(rrlib::logging::eLL_ERROR, "Failed to load parameter '", ann->GetQualifiedName(), "' from finstruct default '", finstruct_default, "': ", e);
            pd->RecycleUnused();
          }
        }
        else
        {
          throw util::tRuntimeException("Port Type not supported as a parameter", CODE_LOCATION_MACRO);
        }

      }
    }
  }
}

void tParameterInfo::SaveValue()
{
  tAbstractPort* ann = static_cast<tAbstractPort*>(GetAnnotated());
  if (ann == NULL || (!ann->IsReady()))
  {
    return;
  }
  tConfigFile* cf = tConfigFile::Find(*ann);
  bool has_entry = cf->HasEntry(config_entry);
  if (tFinrocTypeInfo::IsCCType(ann->GetDataType()))
  {
    tCCPortBase* port = static_cast<tCCPortBase*>(ann);
    if (has_entry || (!port->ContainsDefaultValue()))
    {
      rrlib::xml::tNode& node = cf->GetEntry(config_entry, true);
      tCCPortDataManager* c = port->GetInInterThreadContainer();
      c->GetObject()->Serialize(node);
      c->Recycle2();
    }
  }
  else if (tFinrocTypeInfo::IsStdType(ann->GetDataType()))
  {
    tPortBase* port = static_cast<tPortBase*>(ann);
    if (has_entry || (!port->ContainsDefaultValue()))
    {
      rrlib::xml::tNode& node = cf->GetEntry(config_entry, true);
      tPortDataManager* pd = port->GetLockedUnsafeRaw();
      pd->GetObject()->Serialize(node);
      pd->ReleaseLock();
    }
  }
  else
  {
    throw util::tRuntimeException("Port Type not supported as a parameter", CODE_LOCATION_MACRO);
  }
}

void tParameterInfo::Serialize(rrlib::serialization::tOutputStream& os) const
{
  os.WriteBoolean(entry_set_from_finstruct);
  os.WriteString(config_entry);
  os.WriteString(command_line_option);
  os.WriteString(finstruct_default);
}

void tParameterInfo::Serialize(rrlib::xml::tNode& node) const
{
  Serialize(node, false, true);
}

void tParameterInfo::Serialize(rrlib::xml::tNode& node, bool finstruct_context, bool include_command_line) const
{
  assert(!(node.HasAttribute("default") || node.HasAttribute("cmdline") || node.HasAttribute("config")));
  if (config_entry.length() > 0 && (entry_set_from_finstruct || (!finstruct_context)))
  {
    node.SetAttribute("config", config_entry);
  }
  if (include_command_line)
  {
    if (command_line_option.length() > 0)
    {
      node.SetAttribute("cmdline", command_line_option);
    }
  }
  if (finstruct_default.length() > 0)
  {
    node.SetAttribute("default", finstruct_default);
  }
}

void tParameterInfo::SetConfigEntry(const util::tString& config_entry_, bool finstruct_set)
{
  if (!boost::equals(this->config_entry, config_entry_))
  {
    this->config_entry = config_entry_;
    this->entry_set_from_finstruct = finstruct_set;
    try
    {
      LoadValue();
    }
    catch (const std::exception& e)
    {
      FINROC_LOG_PRINT(rrlib::logging::eLL_ERROR, e);
    }
  }
}

} // namespace finroc
} // namespace core

template class rrlib::rtti::tDataType<finroc::core::tParameterInfo>;
