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
#include "core/parameter/tStructureParameterBase.h"
#include "core/portdatabase/tFinrocTypeInfo.h"
#include "core/port/tThreadLocalCache.h"
#include "rrlib/serialization/tInputStream.h"
#include "rrlib/xml2_wrapper/tXMLNode.h"
#include "rrlib/serialization/tTypedObject.h"
#include "rrlib/serialization/tOutputStream.h"
#include "rrlib/serialization/tStringInputStream.h"

namespace finroc
{
namespace core
{
tStructureParameterBase::tStructureParameterBase(const util::tString& name_, rrlib::serialization::tDataTypeBase type_, bool constructor_prototype) :
    name(name_),
    type(type_),
    value(NULL),
    cc_value(NULL),
    list_index(0)
{
  if (!constructor_prototype)
  {
    CreateBuffer(type_);
  }
}

void tStructureParameterBase::CreateBuffer(rrlib::serialization::tDataTypeBase type_)
{
  DeleteBuffer();
  if (tFinrocTypeInfo::IsStdType(type_))
  {
    tPortDataManager* pdm = tPortDataManager::Create(type_);  //new PortDataManagerRaw(type, null);
    pdm->GetCurrentRefCounter()->SetOrAddLocks(static_cast<int8>(1));
    value = pdm;
    assert((value != NULL));
  }
  else
  {
    cc_value = tThreadLocalCache::Get()->GetUnusedInterThreadBuffer(type_);
  }
}

void tStructureParameterBase::DeleteBuffer()
{
  if (value != NULL)
  {
    value->ReleaseLock();
  }
  if (cc_value != NULL)
  {
    cc_value->Recycle2();
  }
}

void tStructureParameterBase::Deserialize(rrlib::serialization::tInputStream& is)
{
  if (RemoteValue())
  {
    assert(false && "not supported");
  }
  else
  {
    is.ReadString();
    is.ReadType();
  }
  if (is.ReadBoolean())
  {
    try
    {
      Set(is.ReadString());
    }
    catch (const util::tException& e)
    {
      FINROC_LOG_STREAM(rrlib::logging::eLL_ERROR, log_domain, e);
    }
  }
}

void tStructureParameterBase::Deserialize(const rrlib::xml2::tXMLNode& node)
{
  rrlib::serialization::tDataTypeBase dt = type;
  if (node.HasAttribute("type"))
  {
    dt = rrlib::serialization::tDataTypeBase::FindType(node.GetStringAttribute("type"));
  }
  rrlib::serialization::tTypedObject* val = ValPointer();
  if (val == NULL || val->GetType() != dt)
  {
    CreateBuffer(dt);
    val = ValPointer();
  }
  val->Deserialize(node);
}

void tStructureParameterBase::Serialize(rrlib::serialization::tOutputStream& os) const
{
  os.WriteString(name);
  os.WriteType(type);
  rrlib::serialization::tTypedObject* val = ValPointer();

  os.WriteBoolean(val != NULL);
  if (val != NULL)
  {
    os.WriteString(sSerializationHelper::TypedStringSerialize(type, val));
  }
}

void tStructureParameterBase::Serialize(rrlib::xml2::tXMLNode& node) const
{
  rrlib::serialization::tTypedObject* val = ValPointer();
  if (val->GetType() != type)
  {
    node.SetAttribute("type", val->GetType().GetName());
  }
  val->Serialize(node);
}

void tStructureParameterBase::Set(const util::tString& s)
{
  if (RemoteValue())
  {
  }
  else
  {
    assert((type != NULL));
    rrlib::serialization::tDataTypeBase dt = sSerializationHelper::GetTypedStringDataType(type, s);
    rrlib::serialization::tTypedObject* val = ValPointer();
    if (val->GetType() != dt)
    {
      CreateBuffer(dt);
      val = ValPointer();
    }

    rrlib::serialization::tStringInputStream sis(s);
    val->Deserialize(sis);
  }
}

} // namespace finroc
} // namespace core

