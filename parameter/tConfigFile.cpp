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
#include "core/parameter/tConfigFile.h"
#include "core/portdatabase/tDataTypeRegister.h"
#include "rrlib/finroc_core_utils/sFiles.h"
#include "rrlib/xml2_wrapper/tXML2WrapperException.h"
#include "core/portdatabase/tTypedObject.h"
#include "rrlib/finroc_core_utils/container/tSimpleList.h"
#include "core/tFrameworkElement.h"
#include "core/tFrameworkElementTreeFilter.h"
#include "core/tCoreFlags.h"
#include "core/parameter/tParameterInfo.h"

namespace finroc
{
namespace core
{
tDataType* tConfigFile::cTYPE = tDataTypeRegister::GetInstance()->GetDataType(util::tTypedClass<tConfigFile>());
util::tString tConfigFile::cSEPARATOR = "/";
util::tString tConfigFile::cXML_BRANCH_NAME = "node";
util::tString tConfigFile::cXML_LEAF_NAME = "value";

tConfigFile::tConfigFile(const util::tString& filename_) :
    wrapped(),
    filename(filename_),
    temp_buffer()
{
  if (util::sFiles::Exists(filename_))
  {
    try
    {
      wrapped = rrlib::xml2::tXMLDocument(filename_);
    }
    catch (const rrlib::xml2::tXML2WrapperException& e)
    {
      FINROC_LOG_STREAM(rrlib::logging::eLL_ERROR, log_domain, e);
      wrapped = rrlib::xml2::tXMLDocument();
      wrapped.AddRootNode(cXML_BRANCH_NAME);
    }
  }
  else
  {
    wrapped = rrlib::xml2::tXMLDocument();
    wrapped.AddRootNode(cXML_BRANCH_NAME);
  }
}

rrlib::xml2::tXMLNode &tConfigFile::GetEntry(const util::tString& entry, bool create)
{
  util::tSimpleList<util::tString> nodes;
  nodes.AddAll(entry.Split(cSEPARATOR));
  size_t idx = 0u;
  rrlib::xml2::tXMLNode::iterator current = &wrapped.GetRootNode();
  rrlib::xml2::tXMLNode::iterator parent = current;
  bool created = false;
  while (idx < nodes.Size())
  {
    bool found = false;
    for (rrlib::xml2::tXMLNode::iterator child = current->GetChildrenBegin(); child != current->GetChildrenEnd(); ++child)
    {
      if (cXML_BRANCH_NAME.Equals(child->GetName()) || cXML_LEAF_NAME.Equals(child->GetName()))
      {
        try
        {
          if (nodes.Get(idx).Equals(child->GetStringAttribute("name")))
          {
            idx++;
            parent = current;
            current = child;
            found = true;
            break;
          }
        }
        catch (const rrlib::xml2::tXML2WrapperException& e)
        {
          FINROC_LOG_STREAM(rrlib::logging::eLL_WARNING, log_domain, "tree node without name");
        }
      }
    }
    if (!found)
    {
      if (create)
      {
        parent = current;
        current = &current->AddChildNode((idx == nodes.Size() - 1) ? cXML_LEAF_NAME : cXML_BRANCH_NAME);
        created = true;
        current->SetAttribute("name", nodes.Get(idx));
        idx++;
      }
      else
      {
        throw util::tRuntimeException("Node not found", CODE_LOCATION_MACRO);
      }
    }
  }
  if (!cXML_LEAF_NAME.Equals(current->GetName()))
  {
    throw util::tRuntimeException("Node no leaf", CODE_LOCATION_MACRO);
  }

  // Recreate node?
  if (create && (!created))
  {
    parent->RemoveChildNode(*current);
    current = &parent->AddChildNode(cXML_LEAF_NAME);
    current->SetAttribute("name", nodes.Get(nodes.Size() - 1));
  }

  return *current;
}

bool tConfigFile::HasEntry(const util::tString& entry)
{
  util::tSimpleList<util::tString> nodes;
  nodes.AddAll(entry.Split(cSEPARATOR));
  size_t idx = 0u;
  rrlib::xml2::tXMLNode::const_iterator current = &wrapped.GetRootNode();
  while (idx < nodes.Size())
  {
    bool found = false;
    for (rrlib::xml2::tXMLNode::const_iterator child = current->GetChildrenBegin(); child != current->GetChildrenEnd(); ++child)
    {
      if (cXML_BRANCH_NAME.Equals(child->GetName()) || cXML_LEAF_NAME.Equals(child->GetName()))
      {
        try
        {
          if (nodes.Get(idx).Equals(child->GetStringAttribute("name")))
          {
            idx++;
            current = child;
            found = true;
            break;
          }
        }
        catch (const rrlib::xml2::tXML2WrapperException& e)
        {
          FINROC_LOG_STREAM(rrlib::logging::eLL_WARNING, log_domain, "tree node without name");
        }
      }
    }
    if (!found)
    {
      return false;
    }
  }
  return cXML_LEAF_NAME.Equals(current->GetName());
}

void tConfigFile::LoadParameterValues()
{
  tFrameworkElement* ann = static_cast<tFrameworkElement*>(GetAnnotated());
  assert((ann != NULL));
  {
    util::tLock lock2(ann->GetRegistryLock());  // nothing should change while we're doing this
    tFrameworkElementTreeFilter fet(tCoreFlags::cSTATUS_FLAGS | tCoreFlags::cIS_PORT, tCoreFlags::cREADY | tCoreFlags::cPUBLISHED | tCoreFlags::cIS_PORT);
    fet.TraverseElementTree(ann, this, true, temp_buffer);
  }
}

void tConfigFile::SaveFile()
{
  // first: update tree
  tFrameworkElement* ann = static_cast<tFrameworkElement*>(GetAnnotated());
  assert((ann != NULL));
  {
    util::tLock lock2(ann->GetRegistryLock());  // nothing should change while we're doing this
    tFrameworkElementTreeFilter fet(tCoreFlags::cSTATUS_FLAGS | tCoreFlags::cIS_PORT, tCoreFlags::cREADY | tCoreFlags::cPUBLISHED | tCoreFlags::cIS_PORT);
    fet.TraverseElementTree(ann, this, false, temp_buffer);
  }

  // write new tree to file
  wrapped.WriteToFile(filename);
}

void tConfigFile::TreeFilterCallback(tFrameworkElement* fe, bool loading_parameters)
{
  if (Find(fe) == this)    // Does element belong to this configuration file?
  {
    tParameterInfo* pi = static_cast<tParameterInfo*>(fe->GetAnnotation(tParameterInfo::cTYPE));
    if (pi != NULL)
    {
      if (loading_parameters == true)
      {
        try
        {
          pi->LoadValue();
        }
        catch (const util::tException& e)
        {
          FINROC_LOG_STREAM(rrlib::logging::eLL_ERROR, log_domain, e);
        }
      }
      else
      {
        try
        {
          pi->SaveValue();
        }
        catch (const util::tException& e)
        {
          FINROC_LOG_STREAM(rrlib::logging::eLL_ERROR, log_domain, e);
        }
      }
    }
  }
}

} // namespace finroc
} // namespace core

