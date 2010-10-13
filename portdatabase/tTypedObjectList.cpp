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
#include "core/portdatabase/tDataType.h"
#include "core/portdatabase/tTypedObjectList.h"
#include "core/port/tThreadLocalCache.h"
#include "core/port/std/tPortDataManager.h"
#include "core/buffers/tCoreInput.h"
#include "core/portdatabase/tTypedObjectImpl.h"
#include "rrlib/xml2_wrapper/tXMLNode.h"
#include "core/portdatabase/tDataTypeRegister.h"
#include "core/buffers/tCoreOutput.h"

namespace finroc
{
namespace core
{
tTypedObjectList::tTypedObjectList(tDataType* element_type_) :
    list(),
    element_type(element_type_),
    size(0)
{
}

void tTypedObjectList::AddAutoLock(const tPortData* t)
{
  tThreadLocalCache::Get()->AddAutoLock(t);
}

void tTypedObjectList::AddAutoLock(tCCInterThreadContainer<>* t)
{
  tThreadLocalCache::Get()->AddAutoLock(t);
}

::finroc::core::tTypedObject* tTypedObjectList::CreateBuffer(tDataType* dt)
{
  if (element_type->IsStdType())
  {
    tPortDataManager* pdm = new tPortDataManager(dt, static_cast< ::finroc::core::tPortData*>((GetSize() > 0 ? GetElement(GetSize() - 1) : NULL)));
    pdm->GetCurrentRefCounter()->SetOrAddLocks(static_cast<int8>(1));
    return pdm->GetData();
  }
  else if (element_type->IsCCType())
  {
    assert((dt->IsCCType()));
    return dt->CreateInterThreadInstance();  // not attached to any queue
    //return ThreadLocalCache.get().getUnusedInterThreadBuffer(dt);
  }
  else
  {
    throw util::tRuntimeException("Invalid data type for list", CODE_LOCATION_MACRO);
  }
}

void tTypedObjectList::Deserialize(tCoreInput& is)
{
  size_t size = is.ReadInt();
  if (size > list.Size())
  {
    SetCapacity(size, NULL);
  }
  for (size_t i = 0u; i < size; i++)
  {
    tDataType* dt = is.ReadType();
    if (dt == NULL)
    {
      throw util::tRuntimeException("Datatype is not available here", CODE_LOCATION_MACRO);
    }
    ::finroc::core::tTypedObject* to = list.Get(i);
    if (to->GetType() != dt)    // Correct buffer type? (if not, replace)
    {
      DiscardBuffer(to);
      assert((ValidListType(dt)));
      to = CreateBuffer(dt);
      list.Set(i, to);
    }
    to->Deserialize(is);
  }
  SetSize(size);
}

void tTypedObjectList::Deserialize(const rrlib::xml2::tXMLNode& node)
{
  list.Clear();
  util::tSimpleList<rrlib::xml2::tXMLNode> children;
  children.AddAll(node.GetChildren());
  if (children.Size() > list.Size())
  {
    SetCapacity(size, NULL);
  }
  for (size_t i = 0u; i < children.Size(); i++)
  {
    rrlib::xml2::tXMLNode n = children.Get(i);
    tDataType* dt = element_type;
    if (n.HasAttribute("type"))
    {
      dt = tDataTypeRegister::GetInstance()->GetDataType(n.GetStringAttribute("type"));
    }
    if (dt == NULL)
    {
      throw util::tRuntimeException("Datatype is not available here", CODE_LOCATION_MACRO);
    }
    ::finroc::core::tTypedObject* to = list.Get(i);
    if (to->GetType() != dt)    // Correct buffer type? (if not, replace)
    {
      DiscardBuffer(to);
      assert((ValidListType(dt)));
      to = CreateBuffer(dt);
      list.Set(i, to);
    }
    to->Deserialize(n);
  }
  SetSize(size);
}

void tTypedObjectList::EnsureCapacity(size_t capacity, tDataType* dt)
{
  if (list.Size() < capacity)
  {
    SetCapacity(capacity, dt);
  }
}

void tTypedObjectList::RemoveElement(size_t index)
{
  if (index == size - 1)
  {
    SetSize(GetSize() - 1u);
  }
  else if (index >= 0 && index < size)
  {
    ::finroc::core::tTypedObject* removed = list.Remove(index);
    list.Add(removed);
    SetSize(GetSize() - 1u);
  }
}

void tTypedObjectList::Serialize(tCoreOutput& os) const
{
  os.WriteInt(list.Size());
  for (size_t i = 0u; i < list.Size(); i++)
  {
    ::finroc::core::tTypedObject* to = list.Get(i);
    os.WriteShort(to->GetType()->GetUid());
    to->Serialize(os);
  }
}

void tTypedObjectList::Serialize(rrlib::xml2::tXMLNode& node) const
{
  for (size_t i = 0u; i < list.Size(); i++)
  {
    ::finroc::core::tTypedObject* to = list.Get(i);
    rrlib::xml2::tXMLNode n = node.AddChildNode("element");
    if (to->GetType() != element_type)
    {
      n.SetAttribute("type", to->GetType()->GetName());
    }
    to->Serialize(n);
  }
}

void tTypedObjectList::SetCapacity(size_t new_capacity, tDataType* dt)
{
  assert((ValidListType(dt)));
  while (list.Size() > new_capacity)
  {
    ::finroc::core::tTypedObject* to = list.Remove(list.Size() - 1);
    DiscardBuffer(to);
  }
  while (list.Size() < new_capacity)
  {
    list.Add(dt == NULL ? NULL : CreateBuffer(dt));
  }
}

void tTypedObjectList::SetElement(int index, tTypedObject* element)
{
  assert((ValidListType(element->GetType())));
  DiscardBuffer(list.Get(index));
  list.Set(index, element);
}

void tTypedObjectList::SetSize(size_t new_size)
{
  if (new_size > GetCapacity())
  {
    SetCapacity(new_size, element_type);
  }
  size = new_size;
}

void tTypedObjectList::SetSize(size_t new_size, tDataType* dt)
{
  if (new_size > GetCapacity())
  {
    SetCapacity(new_size, dt);
  }
  size = new_size;
}

bool tTypedObjectList::ValidListType(tDataType* dt)
{
  return dt->IsConvertibleTo(GetElementType());
}

} // namespace finroc
} // namespace core

