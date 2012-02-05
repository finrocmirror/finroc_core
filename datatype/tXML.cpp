/**
 * You received this file as part of an advanced experimental
 * robotics framework prototype ('finroc')
 *
 * Copyright (C) 2011 Max Reichardt,
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
#include "core/datatype/tXML.h"

#include "rrlib/xml2_wrapper/tXMLDocument.h"

namespace finroc
{
namespace core
{
rrlib::rtti::tDataTypeBase tXML::cTYPE = rrlib::rtti::tDataType<tXML>();

tXML::tXML() :
  tCoreString()
{
}

void tXML::Deserialize(const rrlib::xml2::tXMLNode& node)
{
  Set(node.FirstChild().GetXMLDump(true));
}

void tXML::Serialize(rrlib::xml2::tXMLNode& node) const
{
  util::tString s = ToString();
  if (s.Length() > 0)
  {
    rrlib::xml2::tXMLDocument doc(static_cast<const void*>(s.GetCString()), s.Length() + 1);
    node.AddChildNode(doc.RootNode(), true);
  }
}

void tXML::Set(const rrlib::xml2::tXMLNode& n)
{
  Set(n.GetXMLDump(true));
}

} // namespace finroc
} // namespace core

