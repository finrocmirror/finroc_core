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
#include "rrlib/rtti/rtti.h"
#include "core/tFrameworkElement.h"
#include "rrlib/xml/tNode.h"
#include "core/parameter/tConstructorParameters.h"

namespace finroc
{
namespace core
{
rrlib::rtti::tDataType<tStaticParameterList> tStaticParameterList::cTYPE;
tStaticParameterList tStaticParameterList::cEMPTY;

tStaticParameterList::tStaticParameterList() :
  parameters(),
  create_action(-1)
{}

tStaticParameterList::~tStaticParameterList()
{
  Clear();
}

void tStaticParameterList::Add(tStaticParameterBase* param)
{
  if (param != NULL)
  {
    param->list_index = parameters.Size();
    param->parent_list = this;
    parameters.Add(param);
  }
}

void tStaticParameterList::AnnotatedObjectInitialized()
{
  DoStaticParameterEvaluation(*GetAnnotated());
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
    DoStaticParameterEvaluation(*ann);
  }
}

void tStaticParameterList::Deserialize(const rrlib::xml::tNode& node)
{
  Deserialize(node, false);
}

void tStaticParameterList::Deserialize(const rrlib::xml::tNode& node, bool finstruct_context)
{
  size_t number_of_children = std::distance(node.ChildrenBegin(), node.ChildrenEnd());
  if (number_of_children != Size())
  {
    FINROC_LOG_PRINT(rrlib::logging::eLL_WARNING, "Parameter list size and number of xml parameters differ. Trying anyway");
  }
  int count = std::min(number_of_children, Size());
  rrlib::xml::tNode::const_iterator child = node.ChildrenBegin();
  for (int i = 0; i < count; i++)
  {
    tStaticParameterBase* param = Get(i);
    param->Deserialize(*child, finstruct_context);
    ++child;
  }
}

void tStaticParameterList::DoStaticParameterEvaluation(tFrameworkElement& fe)
{
  rrlib::thread::tLock lock2(fe.GetRegistryLock());

  // all parameters attached to any of the module's parameters
  util::tSimpleList<tStaticParameterBase*> attached_parameters;
  util::tSimpleList<tStaticParameterBase*> attached_parameters_tmp;

  tStaticParameterList* spl = fe.GetAnnotation<tStaticParameterList>();
  if (spl)
  {

    // Reevaluate parameters and check whether they have changed
    bool changed = false;
    for (size_t i = 0; i < spl->Size(); i++)
    {
      spl->Get(i)->LoadValue();
      changed |= spl->Get(i)->HasChanged();
      spl->Get(i)->GetAllAttachedParameters(attached_parameters_tmp);
      attached_parameters.AddAll(attached_parameters_tmp);
    }

    if (changed)
    {
      fe.EvaluateStaticParameters();

      // Reset change flags for all parameters
      for (size_t i = 0; i < spl->Size(); i++)
      {
        spl->Get(i)->ResetChanged();
      }

      // initialize any new child elements
      if (fe.IsReady())
      {
        fe.Init();
      }
    }
  }

  // evaluate children's static parameters
  const util::tArrayWrapper<tFrameworkElement::tLink*>* iterable = fe.GetChildren();
  for (int i = 0, n = iterable->Size(); i < n; i++)
  {
    tFrameworkElement::tLink* child = iterable->Get(i);
    if (child != NULL && child->IsPrimaryLink() && (!child->GetChild()->IsDeleted()))
    {
      DoStaticParameterEvaluation(*child->GetChild());
    }
  }

  // evaluate any attached parameters that have changed, too
  for (size_t i = 0; i < attached_parameters.Size(); i++)
  {
    if (attached_parameters.Get(i)->HasChanged())
    {
      DoStaticParameterEvaluation(*attached_parameters.Get(i)->GetParentList()->GetAnnotated());
    }
  }
}

tFrameworkElement* tStaticParameterList::GetAnnotated()
{
  return static_cast<tFrameworkElement*>(tFinrocAnnotation::GetAnnotated());
}

tStaticParameterList* tStaticParameterList::GetOrCreate(tFrameworkElement& fe)
{
  tStaticParameterList* result = static_cast<tStaticParameterList*>(fe.GetAnnotation(cTYPE));
  if (result == NULL)
  {
    result = new tStaticParameterList();
    fe.AddAnnotation(result);
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

void tStaticParameterList::Serialize(rrlib::xml::tNode& node) const
{
  Serialize(node, false);
}

void tStaticParameterList::Serialize(rrlib::xml::tNode& node, bool finstruct_context) const
{
  for (size_t i = 0u; i < Size(); i++)
  {
    rrlib::xml::tNode& child = node.AddChildNode("parameter");
    tStaticParameterBase* param = Get(i);
    child.SetAttribute("name", param->GetName());
    param->Serialize(child, finstruct_context);
  }
}


} // namespace finroc
} // namespace core

template class rrlib::rtti::tDataType<finroc::core::tStaticParameterList>;
