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
#include "core/plugin/tCreateFrameworkElementAction.h"
#include "core/plugin/tPlugins.h"
#include "core/parameter/tConstructorParameters.h"
#include "rrlib/xml2_wrapper/tXML2WrapperException.h"
#include "rrlib/xml2_wrapper/tXMLDocument.h"
#include "core/tFrameworkElementTreeFilter.h"
#include "core/datatype/tCoreString.h"
#include "rrlib/finroc_core_utils/sFiles.h"
#include "core/tLinkEdge.h"
#include "core/parameter/tParameterInfo.h"
#include "core/parameter/tConfigFile.h"

namespace finroc
{
namespace core
{
tStandardCreateModuleAction<tFinstructableGroup> tFinstructableGroup::cCREATE_ACTION("Finstructable Group", util::tTypedClass<tFinstructableGroup>());

/*! Thread currently saving finstructable group */
static util::tThread* saving_thread = NULL;

/*! Temporary variable for saving: .so files that should be loaded prior to instantiating this group */
static std::set<std::string> dependencies_tmp;

/*! Number of types at startup */
static int startup_type_count = 0;

/*! Loaded finroc libraries at startup */
static std::set<std::string> startup_loaded_finroc_libs;

tFinstructableGroup::tFinstructableGroup(tFrameworkElement* parent, const util::tString& name) :
    tFrameworkElement(parent, name, tCoreFlags::cFINSTRUCTABLE_GROUP | tCoreFlags::cALLOWS_CHILDREN, -1),
    xml_file("XML file", this, ""),
    current_xml_file(""),
    connect_tmp(),
    link_tmp(""),
    save_parameter_config_entries(false),
    main_name()
{
}

tFinstructableGroup::tFinstructableGroup(tFrameworkElement* parent, const util::tString& name, const util::tString& xml_file_) :
    tFrameworkElement(parent, name, tCoreFlags::cFINSTRUCTABLE_GROUP | tCoreFlags::cALLOWS_CHILDREN, -1),
    xml_file("XML file", this, ""),
    current_xml_file(""),
    connect_tmp(),
    link_tmp(""),
    save_parameter_config_entries(false),
    main_name()
{
  // this(parent,name);
  try
  {
    this->xml_file.Set(xml_file_);
  }
  catch (const util::tException& e)
  {
    FINROC_LOG_PRINT(rrlib::logging::eLL_ERROR, log_domain, e);
  }
}

void tFinstructableGroup::AddDependency(const util::tString& dependency)
{
  if (util::tThread::CurrentThreadRaw() == saving_thread && startup_loaded_finroc_libs.find(dependency.GetCString()) == startup_loaded_finroc_libs.end())
  {
    dependencies_tmp.insert(dependency);
  }
}

void tFinstructableGroup::AddDependency(const rrlib::serialization::tDataTypeBase& dt)
{
  if (dt.GetUid() >= startup_type_count)
  {
    util::tString tmp(dt.GetBinary());
    if (tmp.Length() > 0)
    {
      if (tmp.Contains("/"))
      {
        tmp = tmp.Substring(tmp.LastIndexOf("/") + 1);
      }
      AddDependency(tmp);
    }
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
    tCreateFrameworkElementAction* action = tPlugins::GetInstance()->LoadModuleType(group, type);
    if (action == NULL)
    {
      FINROC_LOG_PRINT(rrlib::logging::eLL_WARNING, log_domain, "Failed to instantiate element. No module type ", group, "/", type, " available. Skipping...");
      return;
    }

    // read parameters
    rrlib::xml2::tXMLNode::const_iterator child_node = node.GetChildrenBegin();
    const rrlib::xml2::tXMLNode* parameters = NULL;
    const rrlib::xml2::tXMLNode* constructor_params = NULL;
    util::tString p_name = child_node->GetName();
    if (p_name.Equals("constructor"))
    {
      constructor_params = &(*child_node);
      ++child_node;
      p_name = child_node->GetName();
    }
    if (p_name.Equals("parameters"))
    {
      parameters = &(*child_node);
      ++child_node;
    }

    // create mode
    ::finroc::core::tFrameworkElement* created = NULL;
    tConstructorParameters* spl = NULL;
    if (constructor_params != NULL)
    {
      spl = action->GetParameterTypes()->Instantiate();
      spl->Deserialize(*constructor_params, true);
    }
    created = action->CreateModule(parent, name, spl);
    created->SetFinstructed(action, spl);
    created->Init();
    if (parameters != NULL)
    {
      (static_cast<tStructureParameterList*>(created->GetAnnotation(tStructureParameterList::cTYPE)))->Deserialize(*parameters);
      created->StructureParametersChanged();
    }

    // continue with children
    for (; child_node != node.GetChildrenEnd(); ++child_node)
    {
      util::tString name2 = child_node->GetName();
      if (name2.Equals("element"))
      {
        Instantiate(*child_node, created);
      }
      else
      {
        FINROC_LOG_PRINT(rrlib::logging::eLL_WARNING, log_domain, "Unknown XML tag: ", name2);
      }
    }

  }
  catch (const rrlib::xml2::tXML2WrapperException& e)
  {
    FINROC_LOG_PRINT(rrlib::logging::eLL_WARNING, log_domain, "Failed to instantiate element. Skipping...");
    LogException(e);
  }
  catch (const util::tException& e)
  {
    FINROC_LOG_PRINT(rrlib::logging::eLL_WARNING, log_domain, "Failed to instantiate element. Skipping...");
    FINROC_LOG_PRINT(rrlib::logging::eLL_WARNING, log_domain, e);
  }
}

bool tFinstructableGroup::IsResponsibleForConfigFileConnections(tFrameworkElement* ap) const
{
  tConfigFile* cf = tConfigFile::Find(ap);
  if (cf == NULL)
  {
    return false;
  }
  tFrameworkElement* config_element = static_cast<tFrameworkElement*>(cf->GetAnnotated());
  const tFrameworkElement* responsible = config_element->GetFlag(tCoreFlags::cFINSTRUCTABLE_GROUP) ? config_element : config_element->GetParentWithFlags(tCoreFlags::cFINSTRUCTABLE_GROUP);
  if (responsible == NULL)
  { // ok, config file is probably attached to runtime. Choose outer-most finstructable group.
    responsible = this;
    const tFrameworkElement* tmp;
    while ((tmp = responsible->GetParentWithFlags(tCoreFlags::cFINSTRUCTABLE_GROUP)) != NULL)
    {
      responsible = tmp;
    }
  }
  return responsible == this;
}

void tFinstructableGroup::LoadXml(const util::tString& xml_file_)
{
  util::tLock lock1(this);
  {
    util::tLock lock2(GetRegistryLock());
    try
    {
      FINROC_LOG_PRINT(rrlib::logging::eLL_DEBUG, log_domain, "Loading XML: ", xml_file_);
      rrlib::xml2::tXMLDocument doc(util::sFiles::GetFinrocXMLDocument(xml_file_, false));
      rrlib::xml2::tXMLNode& root = doc.GetRootNode();
      link_tmp = GetQualifiedName() + "/";
      if (main_name.Length() == 0 && root.HasAttribute("defaultname"))
      {
        main_name = root.GetStringAttribute("defaultname");
      }

      // load dependencies
      if (root.HasAttribute("dependencies"))
      {
        std::vector<util::tString> deps = util::tString(root.GetStringAttribute("dependencies")).Split(",");
        for (size_t i = 0; i < deps.size(); i++)
        {
          std::string dep = deps[i].Trim().GetCString();
          std::vector<std::string> loadable = sDynamicLoading::GetLoadableFinrocLibraries();
          bool loaded = false;
          for (size_t i = 0; i < loadable.size(); i++)
          {
            if (loadable[i].compare(dep) == 0)
            {
              sDynamicLoading::DLOpen(dep.c_str());
              loaded = true;
              break;
            }
          }
          if (!loaded)
          {
            std::set<std::string> loaded_libs = sDynamicLoading::GetLoadedFinrocLibraries();
            if (loaded_libs.find(dep) == loaded_libs.end())
            {
              FINROC_LOG_PRINT(rrlib::logging::eLL_WARNING, log_domain, "Dependency ", dep, " is not available.");
            }
          }
        }
      }

      for (rrlib::xml2::tXMLNode::const_iterator node = root.GetChildrenBegin(); node != root.GetChildrenEnd(); ++node)
      {
        util::tString name = node->GetName();
        if (name.Equals("structureparameter"))
        {
          tStructureParameterList* spl = tStructureParameterList::GetOrCreate(this);
          spl->Add(new tStructureParameterBase(node->GetStringAttribute("name"), rrlib::serialization::tDataTypeBase(), false, true));
        }
        else if (name.Equals("element"))
        {
          Instantiate(*node, this);
        }
        else if (name.Equals("edge"))
        {
          util::tString src = node->GetStringAttribute("src");
          util::tString dest = node->GetStringAttribute("dest");
          tAbstractPort* src_port = GetChildPort(src);
          tAbstractPort* dest_port = GetChildPort(dest);
          if (src_port == NULL && dest_port == NULL)
          {
            FINROC_LOG_PRINT(rrlib::logging::eLL_WARNING, log_domain, "Cannot create edge because neither port is available: ", src, ", ", dest);
          }
          else if (src_port == NULL || src_port->IsVolatile())    // source volatile
          {
            dest_port->ConnectToSource(QualifyLink(src), true);
          }
          else if (dest_port == NULL || dest_port->IsVolatile())    // destination volatile
          {
            src_port->ConnectToTarget(QualifyLink(dest), true);
          }
          else
          {
            src_port->ConnectToTarget(dest_port, true);
          }
        }
        else if (name.Equals("parameter"))
        {
          util::tString param = node->GetStringAttribute("link");
          tAbstractPort* parameter = GetChildPort(param);
          if (parameter == NULL)
          {
            FINROC_LOG_PRINT(rrlib::logging::eLL_WARNING, log_domain, "Cannot set config entry, because parameter is not available: ", param);
          }
          else
          {
            tParameterInfo* pi = parameter->GetAnnotation<tParameterInfo>();
            bool outermost_group = GetParent() == tRuntimeEnvironment::GetInstance();
            if (pi == NULL)
            {
              FINROC_LOG_PRINT(rrlib::logging::eLL_WARNING, log_domain, "Port is not parameter: ", param);
            }
            else
            {
              if (outermost_group && node->HasAttribute("cmdline") && (!IsResponsibleForConfigFileConnections(parameter)))
              {
                pi->SetCommandLineOption(node->GetStringAttribute("cmdline"));
              }
              else
              {
                pi->Deserialize(*node, true, outermost_group);
              }
              try
              {
                pi->LoadValue();
              }
              catch (std::exception& e)
              {
                FINROC_LOG_PRINT(rrlib::logging::eLL_WARNING, log_domain, "Unable to load parameter value: ", param, ". ", e);
              }
            }
          }
        }
        else
        {
          FINROC_LOG_PRINT(rrlib::logging::eLL_WARNING, log_domain, "Unknown XML tag: ", name);
        }
      }
      FINROC_LOG_PRINT(rrlib::logging::eLL_DEBUG, log_domain, "Loading XML successful");
    }
    catch (const std::exception& e)
    {
      FINROC_LOG_PRINT(rrlib::logging::eLL_WARNING, log_domain, "Loading XML failed: ", xml_file_);
      LogException(e);
    }
  }
}

void tFinstructableGroup::LogException(const std::exception& e)
{
  const char* msg = e.what();
  FINROC_LOG_PRINT(rrlib::logging::eLL_ERROR, log_domain, msg);
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
    saving_thread = util::tThread::CurrentThreadRaw();
    dependencies_tmp.clear();
    util::tString save_to = util::sFiles::GetFinrocFileToSaveTo(current_xml_file);
    if (save_to.Length() == 0)
    {
      util::tString save_to_alt = util::sFiles::GetFinrocFileToSaveTo(current_xml_file.Replace('/', '_'));
      FINROC_LOG_PRINT(rrlib::logging::eLL_USER, log_domain, "There does not seem to be any suitable location for: '", current_xml_file, "' . For now, using '", save_to_alt, "'.");
      save_to = save_to_alt;
    }
    FINROC_LOG_PRINT(rrlib::logging::eLL_USER, log_domain, "Saving XML: ", save_to);
    rrlib::xml2::tXMLDocument doc;
    try
    {
      rrlib::xml2::tXMLNode& root = doc.AddRootNode("FinstructableGroup");

      // serialize default main name
      if (main_name.Length() > 0)
      {
        root.SetAttribute("defaultname", main_name);
      }

      // serialize proxy parameters
      tStructureParameterList* spl = GetAnnotation<tStructureParameterList>();
      if (spl != NULL)
      {
        for (size_t i = 0; i < spl->Size(); i++)
        {
          tStructureParameterBase* sp = spl->Get(i);
          if (sp->IsStructureParameterProxy())
          {
            rrlib::xml2::tXMLNode& proxy = root.AddChildNode("structureparameter");
            proxy.SetAttribute("name", sp->GetName());
          }
        }
      }

      // serialize framework elements
      SerializeChildren(root, this);

      // serialize edges
      link_tmp = GetQualifiedName() + "/";
      tFrameworkElementTreeFilter filter(tCoreFlags::cSTATUS_FLAGS | tCoreFlags::cIS_PORT, tCoreFlags::cREADY | tCoreFlags::cPUBLISHED | tCoreFlags::cIS_PORT);

      save_parameter_config_entries = false;
      filter.TraverseElementTree(this, this, &root);
      save_parameter_config_entries = true;
      filter.TraverseElementTree(this, this, &root);

      // add dependencies
      if (dependencies_tmp.size() > 0)
      {
        std::stringstream s;
        for (std::set<std::string>::iterator it = dependencies_tmp.begin(); it != dependencies_tmp.end(); it++)
        {
          if (it != dependencies_tmp.begin())
          {
            s << ", ";
          }
          s << (*it);
        }

        root.SetAttribute("dependencies", s.str());
        dependencies_tmp.clear();
      }

      doc.WriteToFile(save_to);
      FINROC_LOG_PRINT(rrlib::logging::eLL_USER, log_domain, "Saving successful.");

    }
    catch (const rrlib::xml2::tXML2WrapperException& e)
    {
      const char* msg = e.what();
      FINROC_LOG_PRINT(rrlib::logging::eLL_USER, log_domain, "Saving failed: ", msg);
      throw util::tException(msg);
    }
  }
  saving_thread = NULL;
}

std::vector<util::tString> tFinstructableGroup::ScanForCommandLineArgs(const util::tString& finroc_file)
{
  std::vector<util::tString> result;
  try
  {
    rrlib::xml2::tXMLDocument doc(util::sFiles::GetFinrocXMLDocument(finroc_file, false));
    try
    {
      RRLIB_LOG_PRINT_STATIC(rrlib::logging::eLL_DEBUG, log_domain, "Scanning for command line options in ", finroc_file);
      rrlib::xml2::tXMLNode& root = doc.GetRootNode();
      ScanForCommandLineArgsHelper(result, root);
      RRLIB_LOG_PRINTF_STATIC(rrlib::logging::eLL_DEBUG, log_domain, "Scanning successful. Found %d additional options.", result.size());
    }
    catch (std::exception& e)
    {
      RRLIB_LOG_PRINT_STATIC(rrlib::logging::eLL_WARNING, log_domain, "FinstructableGroup", "Scanning failed: ", finroc_file, e);
    }
  }
  catch (std::exception& e)
    {}
  return result;
}

void tFinstructableGroup::ScanForCommandLineArgsHelper(std::vector<util::tString>& result, const rrlib::xml2::tXMLNode& parent)
{
  for (rrlib::xml2::tXMLNode::const_iterator node = parent.GetChildrenBegin(); node != parent.GetChildrenEnd(); ++node)
  {
    util::tString name(node->GetName());
    if (node->HasAttribute("cmdline") && (name.Equals("structureparameter") || name.Equals("parameter")))
    {
      result.push_back(node->GetStringAttribute("cmdline"));
    }
    ScanForCommandLineArgsHelper(result, *node);
  }
}

void tFinstructableGroup::SerializeChildren(rrlib::xml2::tXMLNode& node, tFrameworkElement* current)
{
  tFrameworkElement::tChildIterator ci(current);
  ::finroc::core::tFrameworkElement* fe = NULL;
  while ((fe = ci.Next()) != NULL)
  {
    tStructureParameterList* spl = static_cast<tStructureParameterList*>(fe->GetAnnotation(tStructureParameterList::cTYPE));
    tConstructorParameters* cps = static_cast<tConstructorParameters*>(fe->GetAnnotation(tConstructorParameters::cTYPE));
    if (fe->IsReady() && fe->GetFlag(tCoreFlags::cFINSTRUCTED))
    {
      // serialize framework element
      rrlib::xml2::tXMLNode& n = node.AddChildNode("element");
      n.SetAttribute("name", fe->GetCDescription());
      tCreateFrameworkElementAction* cma = tPlugins::GetInstance()->GetModuleTypes().Get(spl->GetCreateAction());
      n.SetAttribute("group", cma->GetModuleGroup());
      if (cma->GetModuleGroup().EndsWith(".so"))
      {
        AddDependency(cma->GetModuleGroup().GetStdString());
      }
      n.SetAttribute("type", cma->GetName());
      if (cps != NULL)
      {
        rrlib::xml2::tXMLNode& pn = n.AddChildNode("constructor");
        cps->Serialize(pn, true);
      }
      if (spl != NULL)
      {
        rrlib::xml2::tXMLNode& pn = n.AddChildNode("parameters");
        spl->Serialize(pn, true);
      }

      // serialize its children
      SerializeChildren(n, fe);
    }
  }
}

void tFinstructableGroup::StaticInit()
{
  startup_type_count = rrlib::serialization::tDataTypeBase::GetTypeCount();
  startup_loaded_finroc_libs = sDynamicLoading::GetLoadedFinrocLibraries();
}

void tFinstructableGroup::StructureParametersChanged()
{
  util::tLock lock1(this);
  if (!current_xml_file.Equals(xml_file.Get()))
  {
    current_xml_file = xml_file.Get();
    //if (this.childCount() == 0) { // TODO: original intension: changing xml files to mutliple existing ones in finstruct shouldn't load all of them
    if (util::sFiles::FinrocFileExists(current_xml_file))
    {
      LoadXml(current_xml_file);
    }
    else
    {
      FINROC_LOG_PRINT(rrlib::logging::eLL_WARNING, log_domain, "Cannot find XML file ", current_xml_file, ". Creating empty group. You may edit and save this group using finstruct.");
    }
  }
}

void tFinstructableGroup::TreeFilterCallback(tFrameworkElement* fe, rrlib::xml2::tXMLNode* root)
{
  assert((fe->IsPort()));
  tAbstractPort* ap = static_cast<tAbstractPort*>(fe);

  // second pass?
  if (save_parameter_config_entries)
  {

    bool outermostGroup = GetParent() == tRuntimeEnvironment::GetInstance();
    tParameterInfo* info = ap->GetAnnotation<tParameterInfo>();

    if (info != NULL && info->HasNonDefaultFinstructInfo())
    {
      if (!IsResponsibleForConfigFileConnections(ap))
      {

        if (outermostGroup && info->GetCommandLineOption().Length() >= 0)
        {
          rrlib::xml2::tXMLNode& config = root->AddChildNode("parameter");
          config.SetAttribute("link", GetEdgeLink(ap));
          config.SetAttribute("cmdline", info->GetCommandLineOption());
        }

        return;
      }

      rrlib::xml2::tXMLNode& config = root->AddChildNode("parameter");
      config.SetAttribute("link", GetEdgeLink(ap));
      info->Serialize(config, true, outermostGroup);
    }
    return;
  }

  // first pass
  ap->GetConnectionPartners(connect_tmp, true, false, true);  // only outgoing edges => we don't get any edges double

  for (size_t i = 0u; i < connect_tmp.Size(); i++)
  {
    tAbstractPort* ap2 = connect_tmp.Get(i);

    // save edge?
    // check1: different finstructed elements as parent?
    if (ap->GetParentWithFlags(tCoreFlags::cFINSTRUCTED) == ap2->GetParentWithFlags(tCoreFlags::cFINSTRUCTED))
    {
      // TODO: check why continue causes problems here
      // continue;
    }

    // check2: their deepest common finstructable_group parent is this
    ::finroc::core::tFrameworkElement* common_parent = ap->GetParent();
    while (!ap2->IsChildOf(common_parent))
    {
      common_parent = common_parent->GetParent();
    }
    ::finroc::core::tFrameworkElement* common_finstructable_parent = GetFlag(tCoreFlags::cFINSTRUCTABLE_GROUP) ? this : common_parent->GetParentWithFlags(tCoreFlags::cFINSTRUCTABLE_GROUP);
    if (common_finstructable_parent != this)
    {
      continue;
    }

    // check3: only save non-volatile connections in this step (finstruct creates link edges for volatile ports)
    if (ap->IsVolatile() || ap2->IsVolatile())
    {
      continue;
    }

    // save edge
    rrlib::xml2::tXMLNode& edge = root->AddChildNode("edge");
    edge.SetAttribute("src", GetEdgeLink(ap));
    edge.SetAttribute("dest", GetEdgeLink(ap2));
  }

  // serialize link edges
  if (ap->GetLinkEdges() != NULL)
  {
    for (size_t i = 0u; i < ap->GetLinkEdges()->Size(); i++)
    {
      tLinkEdge* le = ap->GetLinkEdges()->Get(i);
      if (!le->IsFinstructed())
      {
        continue;
      }
      if (le->GetSourceLink().Length() > 0)
      {
        // save edge
        rrlib::xml2::tXMLNode& edge = root->AddChildNode("edge");
        edge.SetAttribute("src", GetEdgeLink(le->GetSourceLink()));
        edge.SetAttribute("dest", GetEdgeLink(ap));
      }
      else
      {
        // save edge
        rrlib::xml2::tXMLNode& edge = root->AddChildNode("edge");
        edge.SetAttribute("src", GetEdgeLink(ap));
        edge.SetAttribute("dest", GetEdgeLink(le->GetTargetLink()));
      }
    }
  }
}

} // namespace finroc
} // namespace core

