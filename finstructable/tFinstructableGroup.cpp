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
#include "core/finstructable/tFinstructableGroup.h"
#include "core/tCoreFlags.h"
#include "core/tAnnotatable.h"
#include "core/parameter/tStructureParameterList.h"
#include "rrlib/finroc_core_utils/log/tLogUser.h"
#include "core/tRuntimeEnvironment.h"
#include "core/port/tAbstractPort.h"
#include "rrlib/xml2_wrapper/tXMLNode.h"
#include "core/plugin/tCreateModuleAction.h"
#include "core/plugin/tPlugins.h"
#include "core/parameter/tConstructorParameters.h"
#include "rrlib/xml2_wrapper/tXML2WrapperException.h"
#include "rrlib/xml2_wrapper/tXMLDocument.h"
#include "core/tFrameworkElementTreeFilter.h"
#include "core/tChildIterator.h"
#include "core/datatype/tCoreString.h"
#include "rrlib/finroc_core_utils/sFiles.h"
#include "core/tLinkEdge.h"

namespace finroc
{
namespace core
{
tStandardCreateModuleAction<tFinstructableGroup> tFinstructableGroup::cCREATE_ACTION("core", "Finstructable Group", util::tTypedClass<tFinstructableGroup>());

tFinstructableGroup::tFinstructableGroup(const util::tString& name, tFrameworkElement* parent) :
    tFrameworkElement(name, parent, tCoreFlags::cFINSTRUCTABLE_GROUP | tCoreFlags::cALLOWS_CHILDREN, -1),
    xml_file(new tStringStructureParameter("XML file", "")),
    current_xml_file(""),
    connect_tmp(),
    link_tmp("")
{
  AddAnnotation(new tStructureParameterList(xml_file));
}

tFinstructableGroup::tFinstructableGroup(const util::tString& name, tFrameworkElement* parent, const util::tString& xml_file_) :
    tFrameworkElement(name, parent, tCoreFlags::cFINSTRUCTABLE_GROUP | tCoreFlags::cALLOWS_CHILDREN, -1),
    xml_file(new tStringStructureParameter("XML file", "")),
    current_xml_file(""),
    connect_tmp(),
    link_tmp("")
{
  // this(name,parent);
  AddAnnotation(new tStructureParameterList(xml_file));
  try
  {
    this->xml_file->Set(xml_file_);
    StructureParametersChanged();
  }
  catch (const util::tException& e)
  {
    FINROC_LOG_STREAM(rrlib::logging::eLL_ERROR, log_domain, e);
  }
}

tAbstractPort* tFinstructableGroup::GetChildPort(const util::tString& link)
{
  if (link.StartsWith("/"))
  {
    return GetRuntime()->GetPort(link);
  }
  ::finroc::core::tFrameworkElement* fe = GetChildElement(link, false);
  if (fe != NULL && fe->IsPort())
  {
    return static_cast<tAbstractPort*>(fe);
  }
  return NULL;
}

util::tString tFinstructableGroup::GetEdgeLink(const util::tString& target_link)
{
  if (target_link.StartsWith(link_tmp))
  {
    return target_link.Substring(link_tmp.Length());
  }
  return target_link;
}

util::tString tFinstructableGroup::GetEdgeLink(tAbstractPort* ap)
{
  ::finroc::core::tFrameworkElement* alt_root = ap->GetParentWithFlags(tCoreFlags::cALTERNATE_LINK_ROOT);
  if (alt_root != NULL && alt_root->IsChildOf(this))
  {
    return ap->GetQualifiedLink();
  }
  return ap->GetQualifiedName().Substring(link_tmp.Length());
}

void tFinstructableGroup::Instantiate(const rrlib::xml2::tXMLNode& node, tFrameworkElement* parent)
{
  try
  {
    util::tString name = node.GetStringAttribute("name");
    util::tString group = node.GetStringAttribute("group");
    util::tString type = node.GetStringAttribute("type");

    // find action
    const util::tSimpleList<tCreateModuleAction*>& cmas = tPlugins::GetInstance()->GetModuleTypes();
    tCreateModuleAction* action = NULL;
    for (size_t i = 0u; i < cmas.Size(); i++)
    {
      tCreateModuleAction* cma = cmas.Get(i);
      if (cma->GetModuleGroup().Equals(group) && cma->GetName().Equals(type))
      {
        action = cma;
        break;
      }
    }

    if (action == NULL)
    {
      FINROC_LOG_STREAM(rrlib::logging::eLL_WARNING, log_domain, "Failed to instantiate element. No module type ", group, "/", type, " available. Skipping...");
      return;
    }

    // read parameters
    util::tSimpleList<rrlib::xml2::tXMLNode> children;
    children.AddAll(node.GetChildren());
    int idx = 0;
    rrlib::xml2::tXMLNode* parameters = NULL;
    rrlib::xml2::tXMLNode* constructor_params = NULL;
    rrlib::xml2::tXMLNode xn = children.Get(idx);
    util::tString p_name = xn.GetName();
    if (p_name.Equals("constructor"))
    {
      constructor_params = &(xn);
      idx++;
      xn = children.Get(idx);
      p_name = xn.GetName();
    }
    if (p_name.Equals("parameters"))
    {
      parameters = &(xn);
      idx++;
    }

    // create mode
    ::finroc::core::tFrameworkElement* created = NULL;
    tConstructorParameters* spl = NULL;
    if (constructor_params != NULL)
    {
      spl = action->GetParameterTypes()->Instantiate();
      spl->Deserialize(*constructor_params);
    }
    created = action->CreateModule(name, parent, spl);
    created->SetFinstructed(action, spl);
    created->Init();
    if (parameters != NULL)
    {
      (static_cast<tStructureParameterList*>(created->GetAnnotation(tStructureParameterList::cTYPE)))->Deserialize(*parameters);
      created->StructureParametersChanged();
    }

    // continue with children
    for (size_t i = idx; i < children.Size(); i++)
    {
      rrlib::xml2::tXMLNode node2 = children.Get(i);
      util::tString name2 = node2.GetName();
      if (name2.Equals("element"))
      {
        Instantiate(node2, created);
      }
      else
      {
        FINROC_LOG_STREAM(rrlib::logging::eLL_WARNING, log_domain, "Unknown XML tag: ", name2);
      }
    }

  }
  catch (const rrlib::xml2::tXML2WrapperException& e)
  {
    FINROC_LOG_STREAM(rrlib::logging::eLL_WARNING, log_domain, "Failed to instantiate element. Skipping...");
    LogException(e);
  }
  catch (const util::tException& e)
  {
    FINROC_LOG_STREAM(rrlib::logging::eLL_WARNING, log_domain, "Failed to instantiate element. Skipping...");
    FINROC_LOG_STREAM(rrlib::logging::eLL_WARNING, log_domain, e);
  }
}

void tFinstructableGroup::LoadXml(const util::tString& xml_file_)
{
  util::tLock lock1(this);
  {
    util::tLock lock2(GetRegistryLock());
    try
    {
      FINROC_LOG_STREAM(rrlib::logging::eLL_DEBUG, log_domain, "Loading XML: ", xml_file_);
      rrlib::xml2::tXMLDocument doc(xml_file_);
      rrlib::xml2::tXMLNode root = doc.GetRootNode();
      link_tmp = GetQualifiedName() + "/";

      util::tSimpleList<rrlib::xml2::tXMLNode> children;
      children.AddAll(root.GetChildren());
      for (size_t i = 0u; i < children.Size(); i++)
      {
        rrlib::xml2::tXMLNode node = children.Get(i);
        util::tString name = node.GetName();
        if (name.Equals("element"))
        {
          Instantiate(node, this);
        }
        else if (name.Equals("edge"))
        {
          util::tString src = node.GetStringAttribute("src");
          util::tString dest = node.GetStringAttribute("dest");
          tAbstractPort* src_port = GetChildPort(src);
          tAbstractPort* dest_port = GetChildPort(dest);
          if (src_port == NULL && dest_port == NULL)
          {
            FINROC_LOG_STREAM(rrlib::logging::eLL_WARNING, log_domain, "Cannot edge because neither port is available: ", src, ", ", dest);
          }
          else if (src_port == NULL || src_port->IsVolatile())    // source volatile
          {
            dest_port->ConnectToSource(QualifyLink(src));
          }
          else if (dest_port == NULL || dest_port->IsVolatile())    // destination volatile
          {
            src_port->ConnectToTarget(QualifyLink(dest));
          }
          else
          {
            src_port->ConnectToTarget(dest_port);
          }
        }
        else
        {
          FINROC_LOG_STREAM(rrlib::logging::eLL_WARNING, log_domain, "Unknown XML tag: ", name);
        }
      }
      FINROC_LOG_STREAM(rrlib::logging::eLL_DEBUG, log_domain, "Loading XML successful");
    }
    catch (const rrlib::xml2::tXML2WrapperException& e)
    {
      FINROC_LOG_STREAM(rrlib::logging::eLL_WARNING, log_domain, "Loading XML failed: ", xml_file_);
      LogException(e);
    }
  }
}

void tFinstructableGroup::LogException(const rrlib::xml2::tXML2WrapperException& e)
{
  const char* msg = e.what();
  FINROC_LOG_STREAM(rrlib::logging::eLL_ERROR, log_domain, msg);
}

util::tString tFinstructableGroup::QualifyLink(const util::tString& link)
{
  if (link.StartsWith("/"))
  {
    return link;
  }
  return link_tmp + link;
}

void tFinstructableGroup::SaveXml()
{
  {
    util::tLock lock2(GetRegistryLock());
    FINROC_LOG_STREAM(rrlib::logging::eLL_USER, log_domain, "Saving XML: ", current_xml_file);
    rrlib::xml2::tXMLDocument doc;
    try
    {
      rrlib::xml2::tXMLNode root = doc.AddRootNode("FinstructableGroup");

      // serialize framework elements
      SerializeChildren(root, this);

      // serialize edges
      link_tmp = GetQualifiedName() + "/";
      tFrameworkElementTreeFilter filter(tCoreFlags::cSTATUS_FLAGS | tCoreFlags::cIS_PORT, tCoreFlags::cREADY | tCoreFlags::cPUBLISHED | tCoreFlags::cIS_PORT);
      filter.TraverseElementTree(this, this, root);
      doc.WriteToFile(current_xml_file);
      FINROC_LOG_STREAM(rrlib::logging::eLL_USER, log_domain, "Saving successful");
    }
    catch (const rrlib::xml2::tXML2WrapperException& e)
    {
      const char* msg = e.what();
      FINROC_LOG_STREAM(rrlib::logging::eLL_USER, log_domain, "Saving failed: ", msg);
      throw util::tException(msg);
    }
  }
}

void tFinstructableGroup::SerializeChildren(rrlib::xml2::tXMLNode& node, tFrameworkElement* current)
{
  tChildIterator ci(current);
  ::finroc::core::tFrameworkElement* fe = NULL;
  while ((fe = ci.Next()) != NULL)
  {
    tStructureParameterList* spl = static_cast<tStructureParameterList*>(fe->GetAnnotation(tStructureParameterList::cTYPE));
    tConstructorParameters* cps = static_cast<tConstructorParameters*>(fe->GetAnnotation(tConstructorParameters::cTYPE));
    if (fe->IsReady() && fe->GetFlag(tCoreFlags::cFINSTRUCTED))
    {
      // serialize framework element
      rrlib::xml2::tXMLNode n = node.AddChildNode("element");
      n.SetAttribute("name", fe->GetCDescription());
      tCreateModuleAction* cma = tPlugins::GetInstance()->GetModuleTypes().Get(spl->GetCreateAction());
      n.SetAttribute("group", cma->GetModuleGroup());
      n.SetAttribute("type", cma->GetName());
      if (cps != NULL)
      {
        rrlib::xml2::tXMLNode pn = n.AddChildNode("constructor");
        cps->Serialize(pn);
      }
      if (spl != NULL)
      {
        rrlib::xml2::tXMLNode pn = n.AddChildNode("parameters");
        spl->Serialize(pn);
      }

      // serialize its children
      SerializeChildren(n, fe);
    }
  }
}

void tFinstructableGroup::StructureParametersChanged()
{
  util::tLock lock1(this);
  if (!current_xml_file.Equals(xml_file->GetValue()->ToString()))
  {
    current_xml_file = xml_file->Get();
    if (this->ChildCount() == 0 && util::sFiles::Exists(current_xml_file))
    {
      LoadXml(current_xml_file);
    }
  }
}

void tFinstructableGroup::TreeFilterCallback(tFrameworkElement* fe, rrlib::xml2::tXMLNode& root)
{
  assert((fe->IsPort()));
  tAbstractPort* ap = static_cast<tAbstractPort*>(fe);
  ap->GetConnectionPartners(connect_tmp, true, false);

  for (size_t i = 0u; i < connect_tmp.Size(); i++)
  {
    tAbstractPort* ap2 = connect_tmp.Get(i);

    // save edge?
    // check1: different finstructed elements as parent?
    if (ap->GetParentWithFlags(tCoreFlags::cFINSTRUCTED) == ap2->GetParentWithFlags(tCoreFlags::cFINSTRUCTED))
    {
      continue;
    }

    // check2: their deepest common finstructable_group parent is this
    ::finroc::core::tFrameworkElement* common_parent = ap->GetParent();
    while (!ap2->IsChildOf(common_parent))
    {
      common_parent = common_parent->GetParent();
    }
    ::finroc::core::tFrameworkElement* common_finstructable_parent = common_parent->GetParentWithFlags(tCoreFlags::cFINSTRUCTABLE_GROUP);
    if (common_finstructable_parent != this)
    {
      continue;
    }

    // check3: only save non-volatile connections in this step
    if (ap->IsVolatile() || ap2->IsVolatile())
    {
      continue;
    }

    // save edge
    rrlib::xml2::tXMLNode edge = root.AddChildNode("edge");
    edge.SetAttribute("src", GetEdgeLink(ap));
    edge.SetAttribute("dest", GetEdgeLink(ap2));
  }

  // serialize link edges
  if (ap->GetLinkEdges() != NULL)
  {
    for (size_t i = 0u; i < ap->GetLinkEdges()->Size(); i++)
    {
      tLinkEdge* le = ap->GetLinkEdges()->Get(i);
      if (le->GetSourceLink().Length() > 0)
      {
        // save edge
        rrlib::xml2::tXMLNode edge = root.AddChildNode("edge");
        edge.SetAttribute("src", GetEdgeLink(le->GetSourceLink()));
        edge.SetAttribute("dest", GetEdgeLink(ap));
      }
      else
      {
        // save edge
        rrlib::xml2::tXMLNode edge = root.AddChildNode("edge");
        edge.SetAttribute("src", GetEdgeLink(ap));
        edge.SetAttribute("dest", GetEdgeLink(le->GetTargetLink()));
      }
    }
  }
}

} // namespace finroc
} // namespace core

