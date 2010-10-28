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
#include "core/port/std/tPortDataManager.h"
#include "core/port/tThreadLocalCache.h"
#include "core/port/std/tPortDataImpl.h"
#include "core/buffers/tCoreInput.h"
#include "rrlib/xml2_wrapper/tXMLNode.h"
#include "core/portdatabase/tDataTypeRegister.h"
#include "core/buffers/tCoreOutput.h"

namespace finroc
{
namespace core
{
tStructureParameterBase::tStructureParameterBase(const util::tString& name_, tDataType* type_, bool constructor_prototype) :
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

void tStructureParameterBase::CreateBuffer(tDataType* type_)
{
  DeleteBuffer();
  if (type_->IsStdType())
  {
    tPortDataManager* pdm = new tPortDataManager(type_, NULL);
    pdm->GetCurrentRefCounter()->SetOrAddLocks(static_cast<int8>(1));
    value = pdm->GetData();
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
    value->GetManager()->ReleaseLock();
  }
  if (cc_value != NULL)
  {
    cc_value->Recycle2();
  }
}

void tStructureParameterBase::Deserialize(tCoreInput& is)
{
  if (RemoteValue())
  {
    assert(false && "not supported");
  }
  else
  {
    is.ReadString();
    is.ReadString();
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
  tDataType* dt = type;
  if (node.HasAttribute("type"))
  {
    dt = tDataTypeRegister::GetInstance()->GetDataType(node.GetStringAttribute("type"));
  }
  tTypedObject* val = ValPointer();
  if (val == NULL || val->GetType() != dt)
  {
    CreateBuffer(dt);
    val = ValPointer();
  }
  val->Deserialize(node);
}

void tStructureParameterBase::Serialize(tCoreOutput& os) const
{
  os.WriteString(name);
  os.WriteString(type->GetName());
  tTypedObject* val = ValPointer();

  os.WriteBoolean(val != NULL);
  if (val != NULL)
  {
    os.WriteString(sSerializationHelper::TypedStringSerialize(type, val));
  }
}

void tStructureParameterBase::Serialize(rrlib::xml2::tXMLNode& node) const
{
  tTypedObject* val = ValPointer();
  if (val->GetType() != type)
  {
    node.SetAttribute("type", val->GetType()->GetName());
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
    tDataType* dt = sSerializationHelper::GetTypedStringDataType(type, s);
    tTypedObject* val = ValPointer();
    if (val->GetType() != dt)
    {
      CreateBuffer(dt);
      val = ValPointer();
    }
    val->Deserialize(s);
  }
}

} // namespace finroc
} // namespace core

