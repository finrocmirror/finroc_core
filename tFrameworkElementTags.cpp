//
// You received this file as part of Finroc
// A Framework for intelligent robot control
//
// Copyright (C) Finroc GbR (finroc.org)
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
//
//----------------------------------------------------------------------
/*!\file    core/tFrameworkElementTags.cpp
 *
 * \author  Max Reichardt
 *
 * \date    2012-08-20
 *
 */
//----------------------------------------------------------------------
#include "core/tFrameworkElementTags.h"

//----------------------------------------------------------------------
// External includes (system with <>, local with "")
//----------------------------------------------------------------------

//----------------------------------------------------------------------
// Internal includes with ""
//----------------------------------------------------------------------
#include "core/tFrameworkElement.h"

//----------------------------------------------------------------------
// Debugging
//----------------------------------------------------------------------
#include <cassert>

//----------------------------------------------------------------------
// Namespace usage
//----------------------------------------------------------------------

//----------------------------------------------------------------------
// Namespace declaration
//----------------------------------------------------------------------
namespace finroc
{
namespace core
{

//----------------------------------------------------------------------
// Forward declarations / typedefs / enums
//----------------------------------------------------------------------

//----------------------------------------------------------------------
// Const values
//----------------------------------------------------------------------

//----------------------------------------------------------------------
// Implementation
//----------------------------------------------------------------------
tFrameworkElementTags::tFrameworkElementTags() :
  tAnnotation(),
  tags()
{}

void tFrameworkElementTags::AddTag(tFrameworkElement& fe, const std::string& tag)
{
  if (!IsTagged(fe, tag))
  {
    tFrameworkElementTags* tags = fe.GetAnnotation<tFrameworkElementTags>();
    if (!tags)
    {
      tags = new tFrameworkElementTags();
      fe.AddAnnotation(*tags);
    }
    tags->tags.push_back(tag);
  }
}

void tFrameworkElementTags::AddTags(tFrameworkElement& fe, const std::vector<std::string>& tags)
{
  for (auto it = tags.begin(); it < tags.end(); it++)
  {
    AddTag(fe, *it);
  }
}

bool tFrameworkElementTags::IsTagged(const tFrameworkElement& fe, const std::string& tag)
{
  tFrameworkElementTags* tags = fe.GetAnnotation<tFrameworkElementTags>();
  if (!tags)
  {
    return false;
  }
  return std::find(tags->tags.begin(), tags->tags.end(), tag) != tags->tags.end();
}

rrlib::serialization::tOutputStream& operator << (rrlib::serialization::tOutputStream& stream, const tFrameworkElementTags& tags)
{
  stream << tags.tags;
  return stream;
}

rrlib::serialization::tInputStream& operator >> (rrlib::serialization::tInputStream& stream, tFrameworkElementTags& tags)
{
  stream >> tags.tags;
  return stream;
}

//----------------------------------------------------------------------
// End of namespace declaration
//----------------------------------------------------------------------
}
}
