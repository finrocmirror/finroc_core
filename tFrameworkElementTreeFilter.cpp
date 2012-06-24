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
#include "core/tFrameworkElementTreeFilter.h"

namespace finroc
{
namespace core
{

tFrameworkElementTreeFilter::tFrameworkElementTreeFilter(uint relevant_flags, uint flag_result, const util::tString& paths) :
  relevant_flags(relevant_flags),
  flag_result(flag_result),
  paths()
{
  if (paths.length() > 0)
  {
    boost::split(this->paths, paths, boost::is_any_of(","));
  }
}

bool tFrameworkElementTreeFilter::Accept(tFrameworkElement& element, std::string& tmp, int ignore_flags) const
{
  int not_ignore = ~ignore_flags;
  if ((element.GetAllFlags() & relevant_flags & not_ignore) == (flag_result & not_ignore))
  {
    if (paths.size() == 0)
    {
      return true;
    }
    element.GetQualifiedName(tmp);
    for (auto it = paths.begin(); it < paths.end(); it++)
    {
      if (boost::starts_with(tmp, *it))
      {
        return true;
      }
    }
  }
  return false;
}

void tFrameworkElementTreeFilter::Deserialize(rrlib::serialization::tInputStream& is)
{
  relevant_flags = is.ReadInt();
  flag_result = is.ReadInt();
  paths.clear();
  int8 count = is.ReadByte();
  for (int i = 0; i < count; i++)
  {
    paths.push_back(is.ReadString());
  }
}

const util::tString& tFrameworkElementTreeFilter::GetEmptyString()
{
  static util::tString cEMPTY;
  return cEMPTY;
}

void tFrameworkElementTreeFilter::Serialize(rrlib::serialization::tOutputStream& os) const
{
  os.WriteInt(relevant_flags);
  os.WriteInt(flag_result);
  os.WriteByte(paths.size());
  for (auto it = paths.begin(); it < paths.end(); it++)
  {
    os.WriteString(*it);
  }
}

} // namespace finroc
} // namespace core

