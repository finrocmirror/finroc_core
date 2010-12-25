/**
 * You received this file as part of an advanced experimental
 * robotics framework prototype ('finroc')
 *
 * Copyright (C) 2007-2010 Max Reichardt,
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
#include "core/datatype/tPortCreationList.h"
#include "core/portdatabase/tDataTypeRegister.h"
#include "core/port/tAbstractPort.h"
#include "rrlib/finroc_core_utils/log/tLogUser.h"
#include "core/port/std/tPortBase.h"
#include "core/port/tPortCreationInfo.h"
#include "core/port/cc/tCCPortBase.h"
#include "core/port/rpc/tInterfacePort.h"
#include "core/buffers/tCoreInput.h"
#include "rrlib/xml2_wrapper/tXMLNode.h"
#include "core/buffers/tCoreOutput.h"

namespace finroc
{
namespace core
{
const int tPortCreationList::cRELEVANT_FLAGS;
tDataType* tPortCreationList::cTYPE = tDataTypeRegister::GetInstance()->GetDataType(util::tTypedClass<tPortCreationList>());

tPortCreationList::tPortCreationList() :
    show_output_port_selection(false),
    list(),
    io_vector(NULL),
    flags(-1),
    listener(NULL)
{}

void tPortCreationList::Add(const util::tString& name, tDataType* dt, bool output)
{
  {
    util::tLock lock2(io_vector);
    CheckPort(NULL, io_vector, flags, name, dt, output, NULL);
  }
}

void tPortCreationList::ApplyChanges(tFrameworkElement* io_vector_, int flags_)
{
  {
    util::tLock lock2(io_vector_);
    util::tSimpleList<tAbstractPort*> ports1;
    GetPorts(this->io_vector, ports1);
    util::tSimpleList<tAbstractPort*> ports2;
    GetPorts(io_vector_, ports2);

    for (size_t i = 0u; i < ports1.Size(); i++)
    {
      tAbstractPort* ap1 = ports1.Get(i);
      tAbstractPort* ap2 = i < ports2.Size() ? ports2.Get(i) : NULL;
      CheckPort(ap2, io_vector_, flags_, ap1->GetDescription(), ap1->GetDataType(), ap1->IsOutputPort(), ap1);
    }
    for (size_t i = ports1.Size(); i < ports2.Size(); i++)
    {
      ports2.Get(i)->ManagedDelete();
    }
  }
}

void tPortCreationList::CheckPort(tAbstractPort* ap, tFrameworkElement* io_vector_, int flags_, const util::tString& name, tDataType* dt, bool output, tAbstractPort* prototype)
{
  if (ap != NULL && ap->DescriptionEquals(name) && ap->GetDataType() == dt && (ap->GetAllFlags() & cRELEVANT_FLAGS) == (flags_ & cRELEVANT_FLAGS))
  {
    if ((!show_output_port_selection) || (output == ap->IsOutputPort()))
    {
      return;
    }
  }
  if (ap != NULL)
  {
    ap->ManagedDelete();
  }

  // compute flags to use
  int tmp = 0;
  if (show_output_port_selection)
  {
    tmp = output ? tPortFlags::cOUTPUT_PROXY : tPortFlags::cINPUT_PROXY;
  }
  flags_ |= tmp;

  FINROC_LOG_STREAM(rrlib::logging::eLL_DEBUG_VERBOSE_1, log_domain, "Creating port ", name, " in IOVector ", io_vector_->GetQualifiedLink());
  if (dt->IsStdType())
  {
    ap = new tPortBase(tPortCreationInfo(name, io_vector_, dt, flags_));
  }
  else if (dt->IsCCType())
  {
    ap = new tCCPortBase(tPortCreationInfo(name, io_vector_, dt, flags_));
  }
  else if (dt->IsMethodType())
  {
    ap = new tInterfacePort(name, io_vector_, dt, tInterfacePort::eRouting);
  }
  else
  {
    FINROC_LOG_STREAM(rrlib::logging::eLL_WARNING, log_domain, "Cannot create port with type: ", dt->GetName());
  }
  if (ap != NULL)
  {
    ap->Init();
  }
  if (ap != NULL && listener != NULL)
  {
    listener->PortCreated(ap, prototype);
  }
}

void tPortCreationList::Deserialize(tCoreInput& is)
{
  if (io_vector == NULL)
  {
    show_output_port_selection = is.ReadBoolean();
    size_t size = is.ReadInt();
    list.Clear();
    for (size_t i = 0u; i < size; i++)
    {
      list.Add(tEntry(is.ReadString(), is.ReadString(), is.ReadBoolean()));
    }
  }
  else
  {
    {
      util::tLock lock3(io_vector);
      show_output_port_selection = is.ReadBoolean();
      size_t size = is.ReadInt();
      util::tSimpleList<tAbstractPort*> ports;
      GetPorts(io_vector, ports);
      for (size_t i = 0u; i < size; i++)
      {
        tAbstractPort* ap = i < ports.Size() ? ports.Get(i) : NULL;
        util::tString name = is.ReadString();
        tDataType* dt = tDataTypeRegister::GetInstance()->GetDataType(is.ReadString());
        if (dt == NULL)
        {
          throw util::tRuntimeException(util::tStringBuilder("Type ") + (*dt) + " not available", CODE_LOCATION_MACRO);
        }
        bool output = is.ReadBoolean();
        CheckPort(ap, io_vector, flags, name, dt, output, NULL);
      }
      for (size_t i = size; i < ports.Size(); i++)
      {
        ports.Get(i)->ManagedDelete();
      }
    }
  }
}

void tPortCreationList::Deserialize(const rrlib::xml2::tXMLNode& node)
{
  assert(((io_vector != NULL)) && "Only available on local systems");
  {
    util::tLock lock2(io_vector);
    show_output_port_selection = node.GetBoolAttribute("showOutputSelection");
    util::tSimpleList<tAbstractPort*> ports;
    GetPorts(io_vector, ports);
    size_t i = 0;
    for (rrlib::xml2::tXMLNode::const_iterator port = node.GetChildrenBegin(); port != node.GetChildrenEnd(); ++port, ++i)
    {
      tAbstractPort* ap = i < ports.Size() ? ports.Get(i) : NULL;
      util::tString port_name = port->GetName();
      assert((port_name.Equals("port")));
      bool b = false;
      if (show_output_port_selection)
      {
        b = port->GetBoolAttribute("output");
      }
      tDataType* dt = tDataTypeRegister::GetInstance()->GetDataType(port->GetStringAttribute("type"));
      if (dt == NULL)
      {
        throw util::tRuntimeException(util::tStringBuilder("Type ") + (*dt) + " not available", CODE_LOCATION_MACRO);
      }
      CheckPort(ap, io_vector, flags, port->GetStringAttribute("name"), dt, b, NULL);
    }
    for (; i < ports.Size(); i++)
    {
      ports.Get(i)->ManagedDelete();
    }
  }
}

void tPortCreationList::GetPorts(const tFrameworkElement* elem, util::tSimpleList<tAbstractPort*>& result)
{
  result.Clear();
  tFrameworkElement::tChildIterator ci(elem);
  tAbstractPort* ap = NULL;
  while ((ap = ci.NextPort()) != NULL)
  {
    result.Add(ap);
  }
}

void tPortCreationList::InitialSetup(tFrameworkElement* managed_io_vector, int port_creation_flags, bool show_output_port_selection_)
{
  assert(((io_vector == NULL || io_vector == managed_io_vector) && list.IsEmpty()));
  io_vector = managed_io_vector;
  flags = port_creation_flags;
  this->show_output_port_selection = show_output_port_selection_;
}

void tPortCreationList::Serialize(tCoreOutput& os) const
{
  os.WriteBoolean(show_output_port_selection);
  if (io_vector == NULL)
  {
    int size = list.Size();
    os.WriteInt(size);
    for (int i = 0; i < size; i++)
    {
      const tEntry& e = list.Get(i);
      os.WriteString(e.name);
      os.WriteString(e.type->ToString());
      os.WriteBoolean(e.output_port);
    }
  }
  else
  {
    {
      util::tLock lock3(io_vector);
      util::tSimpleList<tAbstractPort*> ports;
      GetPorts(io_vector, ports);
      int size = ports.Size();
      os.WriteInt(size);
      for (int i = 0; i < size; i++)
      {
        tAbstractPort* p = ports.Get(i);
        os.WriteString(p->GetCDescription());
        os.WriteString(p->GetDataType()->GetName());
        os.WriteBoolean(p->IsOutputPort());
      }
    }
  }
}

void tPortCreationList::Serialize(rrlib::xml2::tXMLNode& node) const
{
  assert(((io_vector != NULL)) && "Only available on local systems");
  {
    util::tLock lock2(io_vector);
    node.SetAttribute("showOutputSelection", show_output_port_selection);
    util::tSimpleList<tAbstractPort*> ports;
    GetPorts(io_vector, ports);
    int size = ports.Size();
    for (int i = 0; i < size; i++)
    {
      tAbstractPort* p = ports.Get(i);
      rrlib::xml2::tXMLNode &child = node.AddChildNode("port");
      child.SetAttribute("name", p->GetCDescription());
      child.SetAttribute("type", p->GetDataType()->GetName());
      if (show_output_port_selection)
      {
        child.SetAttribute("output", p->IsOutputPort());
      }
    }
  }

}

tPortCreationList::tEntry::tEntry(const util::tString& name_, const util::tString& type_, bool output_port_) :
    name(name_),
    type(new tDataTypeReference()),
    output_port(output_port_)
{
  this->type->Deserialize(type_);
}

} // namespace finroc
} // namespace core

