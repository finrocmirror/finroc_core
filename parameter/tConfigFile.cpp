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
#include "rrlib/xml/tNode.h"
#include "rrlib/xml/tException.h"
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
      FINROC_LOG_PRINT(ERROR, e);
      wrapped = rrlib::xml::tDocument();
      wrapped.AddRootNode(cXML_BRANCH_NAME);
    }
  }
  else
  {
    wrapped = rrlib::xml::tDocument();
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

  if (active && file.length() > 0 && content.length() == 0 && (!boost::equals(file, filename)))
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
        FINROC_LOG_PRINT(ERROR, e);
        wrapped = rrlib::xml::tDocument();
        try
        {
          wrapped.AddRootNode(cXML_BRANCH_NAME);
        }
        catch (const rrlib::xml::tException& e1)
        {
          FINROC_LOG_PRINT(ERROR, e);
        }
      }
    }
    filename = file;
  }
  else if (active && content.length() > 0)
  {
    if (file.length() > 0)
    {
      filename = file;
    }

    try
    {
      wrapped = rrlib::xml::tDocument(content.c_str(), content.length() + 1);
    }
    catch (const util::tException& e)
    {
      FINROC_LOG_PRINT(ERROR, e);
    }
  }
}

tConfigFile* tConfigFile::Find(const tFrameworkElement& element)
{
  tFinrocAnnotation* ann = element.GetAnnotation(cTYPE);
  if (ann && (static_cast<tConfigFile*>(ann))->active == true)
  {
    return static_cast<tConfigFile*>(ann);
  }
  tFrameworkElement* parent = element.GetParent();
  if (parent)
  {
    return Find(*parent);
  }
  return NULL;
}

rrlib::xml::tNode& tConfigFile::GetEntry(const util::tString& entry, bool create)
{
  std::vector<std::string> nodes;
  boost::split(nodes, entry, boost::is_any_of(cSEPARATOR));
  size_t idx = (nodes.size() > 0 && nodes[0].length() == 0) ? 1 : 0; // if entry starts with '/', skip first empty string
  rrlib::xml::tNode::iterator current = &wrapped.RootNode();
  rrlib::xml::tNode::iterator parent = current;
  bool created = false;
  while (idx < nodes.size())
  {
    if (nodes[idx].length() == 0)
    {
      FINROC_LOG_PRINT(WARNING, "Entry '", entry, "' is not clean. Skipping empty string now, but please fix this!");
      idx++;
      continue;
    }
    bool found = false;
    for (rrlib::xml::tNode::iterator child = current->ChildrenBegin(); child != current->ChildrenEnd(); ++child)
    {
      if (boost::equals(cXML_BRANCH_NAME, child->Name()) || boost::equals(cXML_LEAF_NAME, child->Name()))
      {
        try
        {
          if (boost::equals(nodes[idx], child->GetStringAttribute("name")))
          {
            idx++;
            parent = current;
            current = child;
            found = true;
            break;
          }
        }
        catch (const rrlib::xml::tException& e)
        {
          FINROC_LOG_PRINT(WARNING, "tree node without name");
        }
      }
    }
    if (!found)
    {
      if (create)
      {
        parent = current;
        current = &(current->AddChildNode((idx == nodes.size() - 1) ? cXML_LEAF_NAME : cXML_BRANCH_NAME));
        created = true;
        current->SetAttribute("name", nodes[idx]);
        idx++;
      }
      else
      {
        throw util::tRuntimeException(util::tString("Node not found: ") + entry, CODE_LOCATION_MACRO);
      }
    }
  }
  if (!boost::equals(cXML_LEAF_NAME, current->Name()))
  {
    throw util::tRuntimeException("Node no leaf", CODE_LOCATION_MACRO);
  }

  // Recreate node?
  if (create && (!created))
  {
    parent->RemoveChildNode(*current);
    current = &(parent->AddChildNode(cXML_LEAF_NAME));
    current->SetAttribute("name", nodes[nodes.size() - 1]);
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
    catch (const rrlib::xml::tException& e)
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
  try
  {
    GetEntry(entry, false);
  }
  catch (const util::tRuntimeException& e)
  {
    return false;
  }
  return true;
}

void tConfigFile::LoadParameterValues()
{
  LoadParameterValues(static_cast<tFrameworkElement&>(*GetAnnotated()));
}

void tConfigFile::LoadParameterValues(tFrameworkElement& fe)
{
  rrlib::thread::tLock lock2(fe.GetRegistryLock());  // nothing should change while we're doing this
  tFrameworkElementTreeFilter fet(tCoreFlags::cSTATUS_FLAGS | tCoreFlags::cIS_PORT, tCoreFlags::cREADY | tCoreFlags::cPUBLISHED | tCoreFlags::cIS_PORT);
  fet.TraverseElementTree(fe, temp_buffer, [&](tFrameworkElement & fe)
  {
    if (Find(fe) == this)    // Does element belong to this configuration file?
    {
      tParameterInfo* pi = static_cast<tParameterInfo*>(fe.GetAnnotation(tParameterInfo::cTYPE));
      if (pi)
      {
        try
        {
          pi->LoadValue();
        }
        catch (const util::tException& e)
        {
          FINROC_LOG_PRINT_STATIC(ERROR, e);
        }
      }
    }
  });
}

void tConfigFile::SaveFile()
{
  // first: update tree
  tFrameworkElement* ann = static_cast<tFrameworkElement*>(GetAnnotated());
  assert(ann);
  {
    rrlib::thread::tLock lock2(ann->GetRegistryLock()); // nothing should change while we're doing this
    tFrameworkElementTreeFilter fet(tCoreFlags::cSTATUS_FLAGS | tCoreFlags::cIS_PORT, tCoreFlags::cREADY | tCoreFlags::cPUBLISHED | tCoreFlags::cIS_PORT);
    fet.TraverseElementTree(*ann, temp_buffer, [&](tFrameworkElement & fe)
    {
      if (Find(fe) == this)    // Does element belong to this configuration file?
      {
        tParameterInfo* pi = static_cast<tParameterInfo*>(fe.GetAnnotation(tParameterInfo::cTYPE));
        if (pi)
        {
          try
          {
            pi->SaveValue();
          }
          catch (const util::tException& e)
          {
            FINROC_LOG_PRINT_STATIC(ERROR, e);
          }
        }
      }
    });
  }

  try
  {
    util::tString save_to = util::sFiles::GetFinrocFileToSaveTo(filename);
    if (save_to.length() == 0)
    {
      util::tString save_to_alt = util::sFiles::GetFinrocFileToSaveTo(boost::replace_all_copy(filename, "/", "_"));
      FINROC_LOG_PRINT(ERROR, "There does not seem to be any suitable location for: '", filename, "' . For now, using '", save_to_alt, "'.");
      save_to = save_to_alt;
    }

    // write new tree to file
    wrapped.WriteToFile(save_to);
  }
  catch (std::exception& e)
  {
    FINROC_LOG_PRINT(ERROR, e);
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
  catch (const rrlib::xml::tException& e)
  {
    FINROC_LOG_PRINT(ERROR, e);
  }
  catch (const util::tException& e)
  {
    FINROC_LOG_PRINT(ERROR, e);
  }
}

} // namespace finroc
} // namespace core

template class rrlib::rtti::tDataType<finroc::core::tConfigFile>;
