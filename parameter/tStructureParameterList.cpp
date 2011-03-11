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
#include "core/parameter/tStructureParameterBase.h"
#include "rrlib/serialization/tInputStream.h"
#include "core/tFrameworkElement.h"
#include "rrlib/xml2_wrapper/tXMLNode.h"
#include "rrlib/serialization/tTypedObject.h"
#include "core/parameter/tConstructorParameters.h"
#include "rrlib/serialization/tOutputStream.h"

namespace finroc
{
namespace core
{
rrlib::serialization::tDataType<tStructureParameterList> tStructureParameterList::cTYPE;
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

void tStructureParameterList::Deserialize(rrlib::serialization::tInputStream& is)
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
  size_t number_of_children = std::distance(node.GetChildrenBegin(), node.GetChildrenEnd());
  if (number_of_children != Size())
  {
    FINROC_LOG_STREAM(rrlib::logging::eLL_WARNING, log_domain, "Parameter list size and number of xml parameters differ. Trying anyway");
  }
  int count = std::min(number_of_children, Size());
  rrlib::xml2::tXMLNode::const_iterator child = node.GetChildrenBegin();
  for (int i = 0; i < count; i++)
  {
    tStructureParameterBase* param = Get(i);
    param->Deserialize(*child);
    ++child;
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

void tStructureParameterList::Serialize(rrlib::serialization::tOutputStream& os) const
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
    rrlib::xml2::tXMLNode& child = node.AddChildNode("parameter");
    tStructureParameterBase* param = Get(i);
    child.SetAttribute("name", param->GetName());
    param->Serialize(child);
  }
}

} // namespace finroc
} // namespace core

