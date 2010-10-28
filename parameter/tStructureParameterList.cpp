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
#include "rrlib/xml2_wrapper/tXMLNode.h"
#include "core/portdatabase/tTypedObjectImpl.h"
#include "core/parameter/tConstructorParameters.h"
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
    param->list_index = parameters.Size();
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

void tStructureParameterList::Deserialize(const rrlib::xml2::tXMLNode& node)
{
  util::tSimpleList<rrlib::xml2::tXMLNode> vec;
  vec.AddAll(node.GetChildren());
  if (vec.Size() != Size())
  {
    FINROC_LOG_STREAM(rrlib::logging::eLL_WARNING, log_domain, "Parameter list size and number of xml parameters differ. Trying anyway");
  }
  int count = util::tMath::Min(vec.Size(), Size());
  for (int i = 0; i < count; i++)
  {
    tStructureParameterBase* param = Get(i);
    param->Deserialize(vec.Get(i));
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

tConstructorParameters* tStructureParameterList::Instantiate() const
{
  tConstructorParameters* cp = new tConstructorParameters();
  tStructureParameterList* c = cp;
  c->create_action = create_action;
  for (size_t i = 0u; i < parameters.Size(); i++)
  {
    tStructureParameterBase* p = parameters.Get(i);
    c->Add(p->DeepCopy());
  }
  return cp;
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

void tStructureParameterList::Serialize(rrlib::xml2::tXMLNode& node) const
{
  for (size_t i = 0u; i < Size(); i++)
  {
    rrlib::xml2::tXMLNode p = node.AddChildNode("parameter");
    tStructureParameterBase* param = Get(i);
    p.SetAttribute("name", param->GetName());
    param->Serialize(p);
  }
}

} // namespace finroc
} // namespace core

