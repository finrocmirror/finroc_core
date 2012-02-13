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
#include "rrlib/finroc_core_utils/sFiles.h"
#include "rrlib/xml2_wrapper/tXMLNode.h"
#include "rrlib/xml2_wrapper/tXML2WrapperException.h"
#include "rrlib/finroc_core_utils/container/tSimpleList.h"
#include "rrlib/serialization/serialization.h"

#include "core/parameter/tConfigFile.h"
#include "core/tFrameworkElement.h"
#include "core/tFrameworkElementTreeFilter.h"
#include "core/tCoreFlags.h"
#include "core/parameter/tParameterInfo.h"

namespace finroc
{
namespace core
{
rrlib::rtti::tDataTypeBase tConfigFile::cTYPE = rrlib::rtti::tDataType<tConfigFile>();
util::tString tConfigFile::cSEPARATOR = "/";
util::tString tConfigFile::cXML_BRANCH_NAME = "node";
util::tString tConfigFile::cXML_LEAF_NAME = "value";

tConfigFile::tConfigFile(const util::tString& filename_) :
  wrapped(),
  filename(filename_),
  temp_buffer(),
  active(true)
{
  if (util::sFiles::FinrocFileExists(filename_))
  {
    try
    {
      wrapped = util::sFiles::GetFinrocXMLDocument(filename_, false); // false = do not validate with dtd
    }
    catch (const std::exception& e)
    {
      FINROC_LOG_PRINT(rrlib::logging::eLL_ERROR, e);
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

tConfigFile::tConfigFile() :
  wrapped(),
  filename(),
  temp_buffer(),
  active(true)
{
  wrapped.AddRootNode(cXML_BRANCH_NAME);
}

void tConfigFile::Deserialize(rrlib::serialization::tInputStream& is)
{
  active = is.ReadBoolean();
  util::tString file = is.ReadString();
  util::tString content = is.ReadString();

  if (active && file.Length() > 0 && content.Length() == 0 && (!file.Equals(filename)))
  {
    // load file
    if (util::sFiles::FinrocFileExists(file))
    {
      try
      {
        wrapped = util::sFiles::GetFinrocXMLDocument(file, false);
      }
      catch (const std::exception & e)
      {
        FINROC_LOG_PRINT(rrlib::logging::eLL_ERROR, e);
        wrapped = rrlib::xml2::tXMLDocument();
        try
        {
          wrapped.AddRootNode(cXML_BRANCH_NAME);
        }
        catch (const rrlib::xml2::tXML2WrapperException& e1)
        {
          FINROC_LOG_PRINT(rrlib::logging::eLL_ERROR, e);
        }
      }
    }
    filename = file;
  }
  else if (active && content.Length() > 0)
  {
    if (file.Length() > 0)
    {
      filename = file;
    }

    try
    {
      wrapped = rrlib::xml2::tXMLDocument(content.GetCString(), content.Length() + 1);
    }
    catch (const util::tException& e)
    {
      FINROC_LOG_PRINT(rrlib::logging::eLL_ERROR, e);
    }
  }
}

tConfigFile* tConfigFile::Find(const tFrameworkElement* element)
{
  ::finroc::core::tFinrocAnnotation* ann = element->GetAnnotation(cTYPE);
  if (ann != NULL && (static_cast<tConfigFile*>(ann))->active == true)
  {
    return static_cast<tConfigFile*>(ann);
  }
  tFrameworkElement* parent = element->GetParent();
  if (parent != NULL)
  {
    return Find(parent);
  }
  return NULL;
}

rrlib::xml2::tXMLNode& tConfigFile::GetEntry(const util::tString& entry, bool create)
{
  util::tSimpleList<util::tString> nodes;
  nodes.AddAll(entry.Split(cSEPARATOR));
  size_t idx = 0u;
  rrlib::xml2::tXMLNode::iterator current = &wrapped.RootNode();
  rrlib::xml2::tXMLNode::iterator parent = current;
  bool created = false;
  while (idx < nodes.Size())
  {
    bool found = false;
    for (rrlib::xml2::tXMLNode::iterator child = current->ChildrenBegin(); child != current->ChildrenEnd(); ++child)
    {
      if (cXML_BRANCH_NAME.Equals(child->Name()) || cXML_LEAF_NAME.Equals(child->Name()))
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
          FINROC_LOG_PRINT(rrlib::logging::eLL_WARNING, "tree node without name");
        }
      }
    }
    if (!found)
    {
      if (create)
      {
        parent = current;
        current = &(current->AddChildNode((idx == nodes.Size() - 1) ? cXML_LEAF_NAME : cXML_BRANCH_NAME));
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
  if (!cXML_LEAF_NAME.Equals(current->Name()))
  {
    throw util::tRuntimeException("Node no leaf", CODE_LOCATION_MACRO);
  }

  // Recreate node?
  if (create && (!created))
  {
    parent->RemoveChildNode(*current);
    current = &(parent->AddChildNode(cXML_LEAF_NAME));
    current->SetAttribute("name", nodes.Get(nodes.Size() - 1));
  }

  return *current;
}

util::tString tConfigFile::GetStringEntry(const util::tString& entry)
{
  if (this->HasEntry(entry))
  {
    try
    {
      return GetEntry(entry, false).GetTextContent();
    }
    catch (const rrlib::xml2::tXML2WrapperException& e)
    {
      return "";
    }
  }
  else
  {
    return "";
  }
}

bool tConfigFile::HasEntry(const util::tString& entry)
{
  util::tSimpleList<util::tString> nodes;
  nodes.AddAll(entry.Split(cSEPARATOR));
  size_t idx = 0u;
  rrlib::xml2::tXMLNode::const_iterator current = &wrapped.RootNode();
  while (idx < nodes.Size())
  {
    bool found = false;
    for (rrlib::xml2::tXMLNode::const_iterator child = current->ChildrenBegin(); child != current->ChildrenEnd(); ++child)
    {
      if (cXML_BRANCH_NAME.Equals(child->Name()) || cXML_LEAF_NAME.Equals(child->Name()))
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
          FINROC_LOG_PRINT(rrlib::logging::eLL_WARNING, "tree node without name");
        }
      }
    }
    if (!found)
    {
      return false;
    }
  }
  return cXML_LEAF_NAME.Equals(current->Name());
}

void tConfigFile::LoadParameterValues()
{
  LoadParameterValues(static_cast<tFrameworkElement*>(GetAnnotated()));
}

void tConfigFile::LoadParameterValues(tFrameworkElement* fe)
{
  assert((fe != NULL));
  {
    util::tLock lock2(fe->GetRegistryLock());  // nothing should change while we're doing this
    tFrameworkElementTreeFilter fet(tCoreFlags::cSTATUS_FLAGS | tCoreFlags::cIS_PORT, tCoreFlags::cREADY | tCoreFlags::cPUBLISHED | tCoreFlags::cIS_PORT);
    fet.TraverseElementTree(fe, this, true, temp_buffer);
  }
}

void tConfigFile::SaveFile()
{
  // first: update tree
  tFrameworkElement* ann = static_cast<tFrameworkElement*>(GetAnnotated());
  assert((ann != NULL));
  {
    util::tLock lock2(ann->GetRegistryLock()); // nothing should change while we're doing this
    tFrameworkElementTreeFilter fet(tCoreFlags::cSTATUS_FLAGS | tCoreFlags::cIS_PORT, tCoreFlags::cREADY | tCoreFlags::cPUBLISHED | tCoreFlags::cIS_PORT);
    fet.TraverseElementTree(ann, this, false, temp_buffer);
  }

  try
  {
    util::tString save_to = util::sFiles::GetFinrocFileToSaveTo(filename);
    if (save_to.Length() == 0)
    {
      util::tString save_to_alt = util::sFiles::GetFinrocFileToSaveTo(filename.Replace('/', '_'));
      FINROC_LOG_PRINT(rrlib::logging::eLL_ERROR, "There does not seem to be any suitable location for: '", filename, "' . For now, using '", save_to_alt, "'.");
      save_to = save_to_alt;
    }

    // write new tree to file
    wrapped.WriteToFile(save_to);
  }
  catch (std::exception& e)
  {
    FINROC_LOG_PRINT(rrlib::logging::eLL_ERROR, e);
  }
}

void tConfigFile::Serialize(rrlib::serialization::tOutputStream& os) const
{
  os.WriteBoolean(active);
  os.WriteString(GetFilename());

  try
  {
    os.WriteString(wrapped.RootNode().GetXMLDump());
  }
  catch (const rrlib::xml2::tXML2WrapperException& e)
  {
    FINROC_LOG_PRINT(rrlib::logging::eLL_ERROR, e);
  }
  catch (const util::tException& e)
  {
    FINROC_LOG_PRINT(rrlib::logging::eLL_ERROR, e);
  }
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
          FINROC_LOG_PRINT(rrlib::logging::eLL_ERROR, e);
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
          FINROC_LOG_PRINT(rrlib::logging::eLL_ERROR, e);
        }
      }
    }
  }
}

} // namespace finroc
} // namespace core

template class rrlib::rtti::tDataType<finroc::core::tConfigFile>;
