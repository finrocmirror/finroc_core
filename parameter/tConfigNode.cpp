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
#include "core/parameter/tConfigNode.h"
#include "core/parameter/tConfigFile.h"
#include "rrlib/serialization/tDataType.h"
#include "core/tFrameworkElement.h"

namespace finroc
{
namespace core
{
rrlib::serialization::tDataTypeBase tConfigNode::cTYPE = rrlib::serialization::tDataType<tConfigNode>();

tConfigNode::tConfigNode(const util::tString& node) :
  node(node)
{
}

util::tString tConfigNode::GetConfigNode(tFrameworkElement* fe)
{
  tConfigFile* cf = tConfigFile::Find(fe);
  if (cf == NULL)
  {
    return "";
  }

  util::tString result = "";
  while (true)
  {

    tConfigNode* cn = fe->GetAnnotation<tConfigNode>();
    if (cn != NULL)
    {
      result = cn->node + (cn->node.EndsWith("/") ? "" : "/") + result;
      if (cn->node.StartsWith("/"))
      {
        return result;
      }
    }
    fe = fe->GetParent();

    if (fe == cf->GetAnnotated())
    {
      return result;
    }
  }
}

util::tString tConfigNode::GetFullConfigEntry(tFrameworkElement* parent, const util::tString& config_entry)
{
  if (config_entry.StartsWith("/"))
  {
    return config_entry;
  }
  util::tString node = GetConfigNode(parent);
  return node + (node.EndsWith("/") ? "" : "/") + config_entry;
}

} // namespace finroc
} // namespace core

