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
#include "core/parameter/tStructureParameterList.h"
#include "core/portdatabase/tDataTypeRegister.h"
#include "core/parameter/tStructureParameterBase.h"
#include "core/buffers/tCoreInput.h"
#include "core/tFrameworkElement.h"
#include "core/buffers/tCoreOutput.h"

namespace finroc
{
namespace core
{
tDataType* tStructureParameterList::cTYPE = tDataTypeRegister::GetInstance()->GetDataType(util::tTypedClass<tStructureParameterList>());
tStructureParameterList tStructureParameterList::cEMPTY;

tStructureParameterList::tStructureParameterList() :
    parameters(),
    create_action(-1)
{}

void tStructureParameterList::Add(tStructureParameterBase* param)
{
  if (param != NULL)
  {
    parameters.Add(param);
  }
}

void tStructureParameterList::Clear()
{
  for (int i = parameters.Size() - 1; i >= 0; i--)
  {
    delete parameters.Remove(i);
  }
}

tStructureParameterList* tStructureParameterList::CloneList() const
{
  tStructureParameterList* c = new tStructureParameterList();
  c->create_action = create_action;
  for (size_t i = 0u; i < parameters.Size(); i++)
  {
    tStructureParameterBase* p = parameters.Get(i);
    c->parameters.Add(new tStructureParameterBase(p->GetName(), p->GetType(), p->IsConstParameter(), true));
  }
  return c;
}

void tStructureParameterList::Deserialize(tCoreInput& is)
{
  if (GetAnnotated() == NULL)
  {
    assert(false && "not supported");

  }
  else    // attached to module - only update parameter values
  {
    if (create_action != is.ReadInt() || (static_cast<int>(parameters.Size())) != is.ReadInt())
    {
      throw util::tRuntimeException("Invalid action id or parameter number", CODE_LOCATION_MACRO);
    }
    for (size_t i = 0u; i < parameters.Size(); i++)
    {
      tStructureParameterBase* param = parameters.Get(i);
      param->Deserialize(is);
    }
    (static_cast<tFrameworkElement*>(GetAnnotated()))->StructureParametersChanged();
  }
}

tStructureParameterList* tStructureParameterList::GetOrCreate(tFrameworkElement* fe)
{
  tStructureParameterList* result = static_cast<tStructureParameterList*>(fe->GetAnnotation(cTYPE));
  if (result == NULL)
  {
    result = new tStructureParameterList();
    fe->AddAnnotation(result);
  }
  return result;
}

void tStructureParameterList::Serialize(tCoreOutput& os) const
{
  os.WriteInt(create_action);
  os.WriteInt(parameters.Size());
  for (size_t i = 0u; i < parameters.Size(); i++)
  {
    parameters.Get(i)->Serialize(os);
  }
}

} // namespace finroc
} // namespace core

