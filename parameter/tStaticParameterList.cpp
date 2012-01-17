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
#include "core/parameter/tStaticParameterList.h"
#include "core/parameter/tStaticParameterBase.h"
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
rrlib::serialization::tDataType<tStaticParameterList> tStaticParameterList::cTYPE;
tStaticParameterList tStaticParameterList::cEMPTY;

tStaticParameterList::tStaticParameterList() :
    parameters(),
    create_action(-1)
{}

void tStaticParameterList::Add(tStaticParameterBase* param)
{
  if (param != NULL)
  {
    param->list_index = parameters.Size();
    param->parent_list = this;
    parameters.Add(param);
  }
}

void tStaticParameterList::Clear()
{
  for (int i = parameters.Size() - 1; i >= 0; i--)
  {
    delete parameters.Remove(i);
  }
}

void tStaticParameterList::Deserialize(rrlib::serialization::tInputStream& is)
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
    tFrameworkElement* ann = static_cast<tFrameworkElement*>(GetAnnotated());
    for (size_t i = 0u; i < parameters.Size(); i++)
    {
      tStaticParameterBase* param = parameters.Get(i);
      param->Deserialize(is);
    }
    ann->DoStaticParameterEvaluation();
  }
}

void tStaticParameterList::Deserialize(const rrlib::xml2::tXMLNode& node)
{
  Deserialize(node, false);
}

void tStaticParameterList::Deserialize(const rrlib::xml2::tXMLNode& node, bool finstruct_context)
{
  size_t number_of_children = std::distance(node.ChildrenBegin(), node.ChildrenEnd());
  if (number_of_children != Size())
  {
    FINROC_LOG_PRINT(rrlib::logging::eLL_WARNING, "Parameter list size and number of xml parameters differ. Trying anyway");
  }
  int count = std::min(number_of_children, Size());
  rrlib::xml2::tXMLNode::const_iterator child = node.ChildrenBegin();
  for (int i = 0; i < count; i++)
  {
    tStaticParameterBase* param = Get(i);
    param->Deserialize(*child, finstruct_context);
    ++child;
  }
}

tFrameworkElement* tStaticParameterList::GetAnnotated()
{
  return static_cast<tFrameworkElement*>(tFinrocAnnotation::GetAnnotated());
}

tStaticParameterList* tStaticParameterList::GetOrCreate(tFrameworkElement* fe)
{
  tStaticParameterList* result = static_cast<tStaticParameterList*>(fe->GetAnnotation(cTYPE));
  if (result == NULL)
  {
    result = new tStaticParameterList();
    fe->AddAnnotation(result);
  }
  return result;
}

tConstructorParameters* tStaticParameterList::Instantiate() const
{
  tConstructorParameters* cp = new tConstructorParameters();
  tStaticParameterList* c = cp;
  c->create_action = create_action;
  for (size_t i = 0u; i < parameters.Size(); i++)
  {
    tStaticParameterBase* p = parameters.Get(i);
    c->Add(p->DeepCopy());
  }
  return cp;
}

void tStaticParameterList::Serialize(rrlib::serialization::tOutputStream& os) const
{
  os.WriteInt(create_action);
  os.WriteInt(parameters.Size());
  for (size_t i = 0u; i < parameters.Size(); i++)
  {
    parameters.Get(i)->Serialize(os);
  }
}

void tStaticParameterList::Serialize(rrlib::xml2::tXMLNode& node) const
{
  Serialize(node, false);
}

void tStaticParameterList::Serialize(rrlib::xml2::tXMLNode& node, bool finstruct_context) const
{
  for (size_t i = 0u; i < Size(); i++)
  {
    rrlib::xml2::tXMLNode& child = node.AddChildNode("parameter");
    tStaticParameterBase* param = Get(i);
    child.SetAttribute("name", param->GetName());
    param->Serialize(child, finstruct_context);
  }
}


} // namespace finroc
} // namespace core

