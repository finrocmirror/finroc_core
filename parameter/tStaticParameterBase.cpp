/**
 * You received this file as part of an advanced experimental
 * robotics framework prototype ('finroc')
 *
 * Copyright (C) 2010-2011 Max Reichardt,
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
#include "rrlib/xml/tNode.h"

#include "core/parameter/tStaticParameterBase.h"
#include "core/parameter/tStaticParameterList.h"
#include "core/parameter/tParameterInfo.h"
#include "core/portdatabase/tFinrocTypeInfo.h"
#include "core/port/tThreadLocalCache.h"
#include "core/tRuntimeEnvironment.h"
#include "core/parameter/tConfigFile.h"
#include "core/parameter/tConfigNode.h"

namespace finroc
{
namespace core
{
tStaticParameterBase::tStaticParameterBase(const util::tString& name_, rrlib::rtti::tDataTypeBase type_, bool constructor_prototype, bool structure_parameter_proxy, const util::tString& config_entry) :
  name(name_),
  type(type_),
  value(),
  last_value(),
  enforce_current_value(false),
  use_value_of(this),
  parent_list(NULL),
  list_index(0),
  command_line_option(),
  outer_parameter_attachment(),
  create_outer_parameter(false),
  config_entry(config_entry),
  config_entry_set_by_finstruct(false),
  structure_parameter_proxy(structure_parameter_proxy),
  attached_parameters()
{
  if (!constructor_prototype)
  {
    CreateBuffer(type_);
  }
}

tStaticParameterBase::~tStaticParameterBase()
{
}

void tStaticParameterBase::AttachTo(tStaticParameterBase* other)
{
  if (use_value_of != this)
  {
    use_value_of->attached_parameters.RemoveElem(this);
  }
  use_value_of = other == NULL ? this : other;
  if (use_value_of != this)
  {
    use_value_of->attached_parameters.Add(this);
  }

  tStaticParameterBase* sp = GetParameterWithBuffer();
  if (sp->type.GetInfo() == NULL)
  {
    sp->type = type;
  }
  if (!sp->value)
  {
    CreateBuffer(sp->type);

    if (sp != this)
    {
      // Swap buffers to have something sensible in it
      std::swap(value, sp->value);
    }
  }
}

void tStaticParameterBase::CreateBuffer(rrlib::rtti::tDataTypeBase type_)
{
  tStaticParameterBase* sp = GetParameterWithBuffer();
  sp->value.reset(type.CreateInstanceGeneric());
  assert(sp->value);
  assert(type_ != tStaticParameterList::cTYPE);
}

void tStaticParameterBase::Deserialize(rrlib::serialization::tInputStream& is)
{
  // Skip name and parameter type
  is.ReadString();
  rrlib::rtti::tDataTypeBase dt;
  is >> dt;

  util::tString command_line_option_tmp = is.ReadString();
  outer_parameter_attachment = is.ReadString();
  create_outer_parameter = is.ReadBoolean();
  util::tString config_entry_tmp = is.ReadString();
  config_entry_set_by_finstruct = is.ReadBoolean();
  enforce_current_value = is.ReadBoolean();
  UpdateOuterParameterAttachment();
  UpdateAndPossiblyLoad(command_line_option_tmp, config_entry_tmp);

  try
  {
    DeserializeValue(is);
  }
  catch (const util::tException& e)
  {
    FINROC_LOG_PRINT(ERROR, e);
  }
}

void tStaticParameterBase::Deserialize(const rrlib::xml::tNode& node, bool finstructContext)
{
  rrlib::rtti::tDataTypeBase dt = type;
  if (node.HasAttribute("type"))
  {
    dt = rrlib::rtti::tDataTypeBase::FindType(node.GetStringAttribute("type"));
  }
  enforce_current_value = node.HasAttribute("enforcevalue") && node.GetBoolAttribute("enforcevalue");
  rrlib::rtti::tTypedObject* val = ValPointer();
  if (val == NULL || val->GetType() != dt)
  {
    CreateBuffer(dt);
    val = ValPointer();
  }
  val->Deserialize(node);

  util::tString command_line_option_tmp;
  if (node.HasAttribute("cmdline"))
  {
    command_line_option_tmp = node.GetStringAttribute("cmdline");
  }
  else
  {
    command_line_option_tmp = "";
  }
  if (node.HasAttribute("attachouter"))
  {
    outer_parameter_attachment = node.GetStringAttribute("attachouter");
    UpdateOuterParameterAttachment();
  }
  else
  {
    outer_parameter_attachment = "";
    UpdateOuterParameterAttachment();
  }
  util::tString config_entry_tmp;
  if (node.HasAttribute("config"))
  {
    config_entry_tmp = node.GetStringAttribute("config");
    config_entry_set_by_finstruct = finstructContext;
  }
  else
  {
    config_entry_tmp = "";
  }

  UpdateAndPossiblyLoad(command_line_option_tmp, config_entry_tmp);
}

void tStaticParameterBase::DeserializeValue(rrlib::serialization::tInputStream& is)
{
  if (is.ReadBoolean())
  {
    rrlib::rtti::tDataTypeBase dt;
    is >> dt;
    rrlib::rtti::tGenericObject* val = ValPointer();
    if (val->GetType() != dt)
    {
      CreateBuffer(dt);
      val = ValPointer();
    }
    rrlib::serialization::Deserialize(is, *val, rrlib::serialization::tDataEncoding::XML);
  }
}

void tStaticParameterBase::GetAllAttachedParameters(util::tSimpleList<tStaticParameterBase*>& result)
{
  result.Clear();
  result.Add(this);

  for (size_t i = 0; i < result.Size(); i++)
  {
    tStaticParameterBase* param = result.Get(i);
    if (param->use_value_of != NULL && param->use_value_of != this && (!result.Contains(param)))
    {
      result.Add(param->use_value_of);
    }
    for (size_t j = 0; j < param->attached_parameters.Size(); j++)
    {
      tStaticParameterBase* at = param->attached_parameters.Get(j);
      if (at != this && (!result.Contains(at)))
      {
        result.Add(at);
      }
    }
  }
}

bool tStaticParameterBase::HasChanged()
{
  tStaticParameterBase* sp = GetParameterWithBuffer();
  if (sp->value.get() == last_value.get())
  {
    return false;
  }
  if ((!sp->value) || (!last_value))
  {
    return true;
  }
  return !sp->value->Equals(*last_value);
}

void tStaticParameterBase::LoadValue()
{
  tFrameworkElement* parent = parent_list->GetAnnotated();

  if (use_value_of == this && (!enforce_current_value))
  {
    // command line
    tFrameworkElement* fg = parent->GetParentWithFlags(tCoreFlags::cFINSTRUCTABLE_GROUP);
    if (command_line_option.length() > 0 && (fg == NULL || fg->GetParent() == tRuntimeEnvironment::GetInstance()))
    {
      // outermost group?
      util::tString arg = tRuntimeEnvironment::GetInstance()->GetCommandLineArgument(command_line_option);
      if (arg.length() > 0)
      {
        try
        {
          Set(arg);
          return;
        }
        catch (std::exception& e)
        {
          FINROC_LOG_PRINT(ERROR, "Failed to load parameter '", GetName(), "' from command line argument '", arg, "': ", e);
        }
      }
    }

    // config entry
    if (config_entry.length() > 0)
    {
      if (config_entry_set_by_finstruct)
      {
        if (fg == NULL || (!tParameterInfo::IsFinstructableGroupResponsibleForConfigFileConnections(*fg, *parent)))
        {
          return;
        }
      }
      tConfigFile* cf = tConfigFile::Find(*parent);
      util::tString full_config_entry = tConfigNode::GetFullConfigEntry(*parent, config_entry);
      if (cf != NULL)
      {
        if (cf->HasEntry(full_config_entry))
        {
          rrlib::xml::tNode& node = cf->GetEntry(full_config_entry, false);
          try
          {
            value->Deserialize(node);
          }
          catch (std::exception& e)
          {
            FINROC_LOG_PRINT(ERROR, "Failed to load parameter '", GetName(), "' from config entry '", full_config_entry, "': ", e);
          }
        }
      }
    }
  }
}

void tStaticParameterBase::ResetChanged()
{
  tStaticParameterBase* sp = GetParameterWithBuffer();

  assert(sp->value);
  if ((!last_value) || last_value->GetType() != sp->value->GetType())
  {
    last_value.reset(sp->value->GetType().CreateInstanceGeneric());
  }
  assert(last_value);

  FINROC_LOG_PRINT(DEBUG_VERBOSE_2, "Resetting change for buffers of type ", sp->value->GetType().GetName());
  //rrlib::serialization::sSerialization::DeepCopy(*sp->value, *last_value);
  last_value->DeepCopyFrom(sp->value.get());
  assert(!HasChanged());
}

void tStaticParameterBase::Serialize(rrlib::serialization::tOutputStream& os) const
{
  os.WriteString(name);
  os << type;
  os.WriteString(command_line_option);
  os.WriteString(outer_parameter_attachment);
  os.WriteBoolean(create_outer_parameter);
  os.WriteString(config_entry);
  os.WriteBoolean(config_entry_set_by_finstruct);
  os.WriteBoolean(enforce_current_value);

  SerializeValue(os);
}

void tStaticParameterBase::Serialize(rrlib::xml::tNode& node, bool finstruct_context) const
{
  assert(!(node.HasAttribute("type") || node.HasAttribute("cmdline") || node.HasAttribute("config") || node.HasAttribute("attachouter")));
  rrlib::rtti::tTypedObject* val = ValPointer();
  if (val->GetType() != type || structure_parameter_proxy)
  {
    node.SetAttribute("type", val->GetType().GetName());
  }
  if (enforce_current_value)
  {
    node.SetAttribute("enforcevalue", true);
  }
  val->Serialize(node);

  if (command_line_option.length() > 0)
  {
    node.SetAttribute("cmdline", command_line_option);
  }
  if (outer_parameter_attachment.length() > 0)
  {
    node.SetAttribute("attachouter", outer_parameter_attachment);
  }
  if (config_entry.length() > 0 && (config_entry_set_by_finstruct || (!finstruct_context)))
  {
    node.SetAttribute("config", config_entry);
  }
}

void tStaticParameterBase::SerializeValue(rrlib::serialization::tOutputStream& os) const
{
  rrlib::rtti::tGenericObject* val = ValPointer();
  os.WriteBoolean(val != NULL);
  if (val != NULL)
  {
    os << val->GetType();
    rrlib::serialization::Serialize(os, *val, rrlib::serialization::tDataEncoding::XML);
  }
}


void tStaticParameterBase::Set(const util::tString& s)
{
  assert(type != NULL);
  rrlib::rtti::tDataTypeBase dt = sSerializationHelper::GetTypedStringDataType(type, s);
  rrlib::rtti::tTypedObject* val = ValPointer();
  if (val->GetType() != dt)
  {
    CreateBuffer(dt);
    val = ValPointer();
  }

  rrlib::serialization::tStringInputStream sis(s);
  val->Deserialize(sis);
}

void tStaticParameterBase::SetConfigEntry(const util::tString& config_entry)
{
  config_entry_set_by_finstruct = false;
  if (!boost::equals(config_entry, this->config_entry))
  {
    this->config_entry = config_entry;
    if (GetParentList() && GetParentList()->GetAnnotated() && GetParentList()->GetAnnotated()->IsReady())
    {
      LoadValue();
    }
  }
}

void tStaticParameterBase::SetOuterParameterAttachment(const util::tString& outer_parameter_attachment, bool create_outer)
{
  this->outer_parameter_attachment = outer_parameter_attachment;
  create_outer_parameter = create_outer;

  UpdateOuterParameterAttachment();
}

void tStaticParameterBase::UpdateAndPossiblyLoad(const util::tString& command_line_option_tmp, const util::tString& config_entry_tmp)
{
  bool cmdline_changed = !boost::equals(command_line_option, command_line_option_tmp);
  bool config_entry_changed = !boost::equals(config_entry, config_entry_tmp);
  command_line_option = command_line_option_tmp;
  config_entry = config_entry_tmp;

  if (use_value_of == this && (cmdline_changed || config_entry_changed))
  {
    LoadValue();
  }
}

void tStaticParameterBase::UpdateOuterParameterAttachment()
{
  if (parent_list == NULL)
  {
    return;
  }
  if (outer_parameter_attachment.length() == 0)
  {
    if (use_value_of != this)
    {
      AttachTo(this);
    }
  }
  else
  {
    tStaticParameterBase* sp = GetParameterWithBuffer();
    if ((!boost::equals(sp->GetName(), outer_parameter_attachment)) || (sp == this))
    {

      // find parameter to attach to
      tFrameworkElement* fg = parent_list->GetAnnotated()->GetParentWithFlags(tCoreFlags::cFINSTRUCTABLE_GROUP);
      if (fg == NULL)
      {
        FINROC_LOG_PRINT(ERROR, "No parent finstructable group. Ignoring...");
        return;
      }

      tStaticParameterList* spl = tStaticParameterList::GetOrCreate(*fg);
      for (size_t i = 0; i < spl->Size(); i++)
      {
        sp = spl->Get(i);
        if (boost::equals(sp->GetName(), outer_parameter_attachment))
        {
          AttachTo(sp);
          return;
        }
      }

      if (create_outer_parameter)
      {
        sp = new tStaticParameterBase(outer_parameter_attachment, type, false, true);
        AttachTo(sp);
        spl->Add(sp);
        FINROC_LOG_PRINT(DEBUG, "Creating proxy parameter '", outer_parameter_attachment, "' in '", fg->GetQualifiedName() + "'.");
      }
      else
      {
        FINROC_LOG_PRINT(ERROR, "No parameter named '", outer_parameter_attachment, "' found in parent group.");
      }
    }
  }
}

} // namespace finroc
} // namespace core

