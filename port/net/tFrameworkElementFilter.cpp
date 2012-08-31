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
#include "core/port/net/tFrameworkElementFilter.h"

namespace finroc
{
namespace core
{

tFrameworkElementFilter::tFrameworkElementFilter(uint relevant_flags, uint flag_result, bool send_tags) :
  relevant_flags(relevant_flags),
  flag_result(flag_result),
  send_tags(send_tags)
{
}

bool tFrameworkElementFilter::Accept(const tFrameworkElement& element, int ignore_flags) const
{
  int not_ignore = ~ignore_flags;
  return (element.GetAllFlags() & relevant_flags & not_ignore) == (flag_result & not_ignore);
}

rrlib::serialization::tInputStream& operator >> (rrlib::serialization::tInputStream& is, tFrameworkElementFilter& filter)
{
  int relevant_flags = is.ReadInt();
  int flag_result = is.ReadInt();
  filter = tFrameworkElementFilter(relevant_flags, flag_result, is.ReadBoolean());
  return is;
}

rrlib::serialization::tOutputStream& operator << (rrlib::serialization::tOutputStream& os, const tFrameworkElementFilter& filter)
{
  os.WriteInt(filter.relevant_flags);
  os.WriteInt(filter.flag_result);
  os.WriteBoolean(filter.send_tags);
  return os;
}

} // namespace finroc
} // namespace core

