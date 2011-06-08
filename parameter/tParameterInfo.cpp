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
#include "core/parameter/tParameterInfo.h"
#include "rrlib/serialization/tDataType.h"
#include "rrlib/finroc_core_utils/log/tLogUser.h"
#include "core/tFrameworkElement.h"
#include "rrlib/serialization/tInputStream.h"
#include "rrlib/serialization/tStringInputStream.h"
#include "core/port/tAbstractPort.h"
#include "core/parameter/tConfigFile.h"
#include "rrlib/xml2_wrapper/tXMLNode.h"
#include "core/portdatabase/tFinrocTypeInfo.h"
#include "core/port/cc/tCCPortBase.h"
#include "core/port/cc/tCCPortDataManagerTL.h"
#include "core/port/tThreadLocalCache.h"
#include "rrlib/serialization/tGenericObject.h"
#include "core/port/std/tPortBase.h"
#include "core/port/std/tPortDataManager.h"
#include "core/port/cc/tCCPortDataManager.h"
#include "rrlib/serialization/tTypedObject.h"

namespace finroc
{
namespace core
{
rrlib::serialization::tDataTypeBase tParameterInfo::cTYPE = rrlib::serialization::tDataType<tParameterInfo>();

tParameterInfo::tParameterInfo() :
    config_entry()
{}

void tParameterInfo::AnnotatedObjectInitialized()
{
  try
  {
    LoadValue(true);
  }
  catch (const util::tException& e)
  {
    FINROC_LOG_STREAM(rrlib::logging::eLL_ERROR, tFrameworkElement::log_domain, e);
  }
}

void tParameterInfo::Deserialize(rrlib::serialization::tInputStream& is)
{
  SetConfigEntry(is.ReadString());
}

void tParameterInfo::Deserialize(rrlib::serialization::tStringInputStream& is)
{
  SetConfigEntry(is.ReadAll());
}

void tParameterInfo::LoadValue(bool ignore_ready)
{
  tAbstractPort* ann = static_cast<tAbstractPort*>(GetAnnotated());
  {
    util::tLock lock2(ann->GetRegistryLock());
    if (ann != NULL && (ignore_ready || ann->IsReady()))
    {
      tConfigFile* cf = tConfigFile::Find(ann);
      if (cf == NULL)
      {
        return;
      }
      if (cf->HasEntry(config_entry))
      {
        rrlib::xml2::tXMLNode& node = cf->GetEntry(config_entry, false);
        if (tFinrocTypeInfo::IsCCType(ann->GetDataType()))
        {
          tCCPortBase* port = static_cast<tCCPortBase*>(ann);
          tCCPortDataManagerTL* c = tThreadLocalCache::Get()->GetUnusedBuffer(port->GetDataType());
          try
          {
            c->GetObject()->Deserialize(node);
            port->BrowserPublishRaw(c);
          }
          catch (const util::tException& e)
          {
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
          }
          catch (const util::tException& e)
          {
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
  tConfigFile* cf = tConfigFile::Find(ann);
  bool has_entry = cf->HasEntry(config_entry);
  if (tFinrocTypeInfo::IsCCType(ann->GetDataType()))
  {
    tCCPortBase* port = static_cast<tCCPortBase*>(ann);
    if (has_entry || (!port->ContainsDefaultValue()))
    {
      rrlib::xml2::tXMLNode& node = cf->GetEntry(config_entry, true);
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
      rrlib::xml2::tXMLNode& node = cf->GetEntry(config_entry, true);
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

void tParameterInfo::SetConfigEntry(const util::tString& config_entry_)
{
  if (!this->config_entry.Equals(config_entry_))
  {
    this->config_entry = config_entry_;
    try
    {
      LoadValue();
    }
    catch (const util::tException& e)
    {
      FINROC_LOG_STREAM(rrlib::logging::eLL_ERROR, log_domain, e);
    }
  }
}

} // namespace finroc
} // namespace core

