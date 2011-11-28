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
#include "core/parameter/tStaticParameterBase.h"
#include "core/portdatabase/tFinrocTypeInfo.h"
#include "core/port/tThreadLocalCache.h"
#include "rrlib/serialization/tInputStream.h"
#include "rrlib/xml2_wrapper/tXMLNode.h"
#include "rrlib/serialization/tTypedObject.h"
#include "rrlib/serialization/tOutputStream.h"
#include "rrlib/serialization/tStringInputStream.h"
#include "core/tRuntimeEnvironment.h"
#include "core/finstructable/tFinstructableGroup.h"
#include "core/parameter/tConfigFile.h"
#include "core/parameter/tConfigNode.h"

namespace finroc
{
namespace core
{
tStaticParameterBase::tStaticParameterBase(const util::tString& name_, rrlib::serialization::tDataTypeBase type_, bool constructor_prototype, bool structure_parameter_proxy) :
    name(name_),
    type(type_),
    value(),
    last_value(),
    enforce_current_value(false),
    use_value_of(this),
    list_index(0),
    command_line_option(),
    outer_parameter_attachment(),
    create_outer_parameter(false),
    config_entry(),
    config_entry_set_by_finstruct(false),
    structure_parameter_proxy(structure_parameter_proxy)
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
  use_value_of = other == NULL ? this : other;
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

void tStaticParameterBase::CreateBuffer(rrlib::serialization::tDataTypeBase type_)
{
  tStaticParameterBase* sp = GetParameterWithBuffer();
  sp->value.reset(type.CreateInstanceGeneric());
  assert(sp->value);
}

void tStaticParameterBase::Deserialize(rrlib::serialization::tInputStream& is, tFrameworkElement* parameterized)
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

  util::tString command_line_option_tmp = is.ReadString();
  outer_parameter_attachment = is.ReadString();
  create_outer_parameter = is.ReadBoolean();
  util::tString config_entry_tmp = is.ReadString();
  config_entry_set_by_finstruct = is.ReadBoolean();
  enforce_current_value = is.ReadBoolean();
  UpdateOuterParameterAttachment(parameterized);
  UpdateAndPossiblyLoad(command_line_option_tmp, config_entry_tmp, parameterized);

  if (is.ReadBoolean())
  {
    try
    {
      Set(is.ReadString());
    }
    catch (const util::tException& e)
    {
      FINROC_LOG_PRINT(rrlib::logging::eLL_ERROR, log_domain, e);
    }
  }
}

void tStaticParameterBase::Deserialize(const rrlib::xml2::tXMLNode& node, bool finstructContext, tFrameworkElement* parameterized)
{
  rrlib::serialization::tDataTypeBase dt = type;
  if (node.HasAttribute("type"))
  {
    dt = rrlib::serialization::tDataTypeBase::FindType(node.GetStringAttribute("type"));
  }
  enforce_current_value = node.HasAttribute("enforcevalue") && node.GetBoolAttribute("enforcevalue");
  rrlib::serialization::tTypedObject* val = ValPointer();
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
    UpdateOuterParameterAttachment(parameterized);
  }
  else
  {
    outer_parameter_attachment = "";
    UpdateOuterParameterAttachment(parameterized);
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

  UpdateAndPossiblyLoad(command_line_option_tmp, config_entry_tmp, parameterized);
}

void tStaticParameterBase::LoadValue(tFrameworkElement* parent)
{
  if (!enforce_current_value)
  {
    // command line
    tFrameworkElement* fg = parent->GetParentWithFlags(tCoreFlags::cFINSTRUCTABLE_GROUP);
    if (command_line_option.Length() > 0 && (fg == NULL || fg->GetParent() == tRuntimeEnvironment::GetInstance()))
    { // outermost group?
      util::tString arg = tRuntimeEnvironment::GetInstance()->GetCommandLineArgument(command_line_option);
      if (arg.Length() > 0)
      {
        try
        {
          Set(arg);
          return;
        }
        catch (std::exception& e)
        {
          RRLIB_LOG_PRINT(rrlib::logging::eLL_ERROR, log_domain, "Failed to load parameter '", GetName(), "' from command line argument '", arg, "': ", e);
        }
      }
    }

    // config entry
    if (config_entry.Length() > 0)
    {
      if (config_entry_set_by_finstruct)
      {
        if (fg == NULL || (!static_cast<tFinstructableGroup*>(fg)->IsResponsibleForConfigFileConnections(parent)))
        {
          return;
        }
      }
      tConfigFile* cf = tConfigFile::Find(parent);
      util::tString full_config_entry = tConfigNode::GetFullConfigEntry(parent, config_entry);
      if (cf != NULL)
      {
        if (cf->HasEntry(full_config_entry))
        {
          rrlib::xml2::tXMLNode& node = cf->GetEntry(full_config_entry, false);
          try
          {
            value->Deserialize(node);
          }
          catch (std::exception& e)
          {
            RRLIB_LOG_PRINT(rrlib::logging::eLL_ERROR, log_domain, "Failed to load parameter '", GetName(), "' from config entry '", full_config_entry, "': ", e);
          }
        }
      }
    }
  }
}

bool tStaticParameterBase::HasChanged()
{
  tStaticParameterBase* sp = GetParameterWithBuffer();
  if (sp->value.get() == sp->last_value.get())
  {
    return false;
  }
  if ((!sp->value) || (!sp->last_value))
  {
    return true;
  }
  return !rrlib::serialization::sSerialization::Equals(*sp->value, *sp->last_value);
}

void tStaticParameterBase::ResetChanged()
{
  tStaticParameterBase* sp = GetParameterWithBuffer();

  assert(sp->value);
  if ((!sp->last_value) || sp->last_value->GetType() != sp->value->GetType())
  {
    sp->last_value.reset(sp->value->GetType().CreateInstanceGeneric());
  }
  assert(sp->last_value);

  rrlib::serialization::sSerialization::DeepCopy(*sp->value, *sp->last_value);
}

void tStaticParameterBase::Serialize(rrlib::serialization::tOutputStream& os) const
{
  os.WriteString(name);
  os.WriteType(type);
  os.WriteString(command_line_option);
  os.WriteString(outer_parameter_attachment);
  os.WriteBoolean(create_outer_parameter);
  os.WriteString(config_entry);
  os.WriteBoolean(config_entry_set_by_finstruct);
  os.WriteBoolean(enforce_current_value);
  rrlib::serialization::tTypedObject* val = ValPointer();

  os.WriteBoolean(val != NULL);
  if (val != NULL)
  {
    os.WriteString(sSerializationHelper::TypedStringSerialize(type, val));
  }
}

void tStaticParameterBase::Serialize(rrlib::xml2::tXMLNode& node, bool finstruct_context) const
{
  assert(!(node.HasAttribute("type") || node.HasAttribute("cmdline") || node.HasAttribute("config") || node.HasAttribute("attachouter")));
  rrlib::serialization::tTypedObject* val = ValPointer();
  if (val->GetType() != type || structure_parameter_proxy)
  {
    node.SetAttribute("type", val->GetType().GetName());
  }
  if (enforce_current_value)
  {
    node.SetAttribute("enforcevalue", true);
  }
  val->Serialize(node);

  if (command_line_option.Length() > 0)
  {
    node.SetAttribute("cmdline", command_line_option);
  }
  if (outer_parameter_attachment.Length() > 0)
  {
    node.SetAttribute("attachouter", outer_parameter_attachment);
  }
  if (config_entry.Length() > 0 && (config_entry_set_by_finstruct || (!finstruct_context)))
  {
    node.SetAttribute("config", config_entry);
  }
}

void tStaticParameterBase::Set(const util::tString& s)
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

void tStaticParameterBase::UpdateAndPossiblyLoad(const util::tString& command_line_option_tmp, const util::tString& config_entry_tmp, tFrameworkElement* parameterized)
{
  bool cmdline_changed = !command_line_option.Equals(command_line_option_tmp);
  bool config_entry_changed = !config_entry.Equals(config_entry_tmp);
  command_line_option = command_line_option_tmp;
  config_entry = config_entry_tmp;

  if (use_value_of == this && (cmdline_changed || config_entry_changed))
  {
    LoadValue(parameterized);
  }
}

void tStaticParameterBase::UpdateOuterParameterAttachment(tFrameworkElement* parameterized)
{
  if (parameterized == NULL)
  {
    return;
  }
  if (outer_parameter_attachment.Length() == 0)
  {
    if (use_value_of != this)
    {
      AttachTo(this);
    }
  }
  else
  {
    tStaticParameterBase* sp = GetParameterWithBuffer();
    if (!sp->GetName().Equals(outer_parameter_attachment))
    {

      // find parameter to attach to
      tFrameworkElement* fg = parameterized->GetParentWithFlags(tCoreFlags::cFINSTRUCTABLE_GROUP);
      if (fg == NULL)
      {
        FINROC_LOG_PRINT(rrlib::logging::eLL_ERROR, log_domain, "No parent finstructable group. Ignoring...");
        return;
      }

      tStaticParameterList* spl = tStaticParameterList::GetOrCreate(fg);
      for (size_t i = 0; i < spl->Size(); i++)
      {
        sp = spl->Get(i);
        if (sp->GetName().Equals(outer_parameter_attachment))
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
        FINROC_LOG_PRINT(rrlib::logging::eLL_DEBUG, log_domain, "Creating proxy parameter '", outer_parameter_attachment, "' in '", fg->GetQualifiedName() + "'.");
      }
      else
      {
        FINROC_LOG_PRINT(rrlib::logging::eLL_ERROR, log_domain, "No parameter named '", outer_parameter_attachment, "' found in parent group.");
      }
    }
  }
}

} // namespace finroc
} // namespace core

