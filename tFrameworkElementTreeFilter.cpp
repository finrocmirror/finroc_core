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
#include "core/buffers/tCoreInput.h"
#include "core/buffers/tCoreOutput.h"

namespace finroc
{
namespace core
{
tFrameworkElementTreeFilter::tFrameworkElementTreeFilter() :
    relevant_flags(tCoreFlags::cSTATUS_FLAGS),
    flag_result(tCoreFlags::cREADY | tCoreFlags::cPUBLISHED),
    paths(new util::tSimpleList<util::tString>())
{
  // this(CoreFlags.STATUS_FLAGS,CoreFlags.READY | CoreFlags.PUBLISHED,getEmptyString());
  if (GetEmptyString().Length() > 0)
  {
    this->paths->AddAll(GetEmptyString().Split(","));
  }
}

tFrameworkElementTreeFilter::tFrameworkElementTreeFilter(int relevant_flags_, int flag_result_) :
    relevant_flags(relevant_flags_),
    flag_result(flag_result_),
    paths(new util::tSimpleList<util::tString>())
{
  // this(relevantFlags,flagResult,getEmptyString());
  if (GetEmptyString().Length() > 0)
  {
    this->paths->AddAll(GetEmptyString().Split(","));
  }
}

tFrameworkElementTreeFilter::tFrameworkElementTreeFilter(int relevant_flags_, int flag_result_, const util::tString& paths_) :
    relevant_flags(relevant_flags_),
    flag_result(flag_result_),
    paths(new util::tSimpleList<util::tString>())
{
  if (paths_.Length() > 0)
  {
    this->paths->AddAll(paths_.Split(","));
  }
}

bool tFrameworkElementTreeFilter::Accept(tFrameworkElement* element, util::tStringBuilder& tmp) const
{
  if (element == NULL)
  {
    return false;
  }
  if ((element->GetAllFlags() & relevant_flags) == flag_result)
  {
    if (paths->Size() == 0)
    {
      return true;
    }
    bool found = (paths->Size() == 0);
    for (size_t i = 0u, n = paths->Size(); i < n && (!found); i++)
    {
      element->GetQualifiedName(tmp);

      if (tmp.StartsWith(paths->Get(i)))
      {
        return true;
      }

    }
  }
  return false;
}

void tFrameworkElementTreeFilter::Deserialize(tCoreInput& is)
{
  relevant_flags = is.ReadInt();
  flag_result = is.ReadInt();
  paths->Clear();
  int8 count = is.ReadByte();
  for (int i = 0; i < count; i++)
  {
    paths->Add(is.ReadString());
  }
}

const util::tString& tFrameworkElementTreeFilter::GetEmptyString()
{
  static util::tString cEMPTY;
  return cEMPTY;
}

void tFrameworkElementTreeFilter::Serialize(tCoreOutput& os) const
{
  os.WriteInt(relevant_flags);
  os.WriteInt(flag_result);
  os.WriteByte(paths->Size());
  for (size_t i = 0u; i < paths->Size(); i++)
  {
    os.WriteString(paths->Get(i));
  }
}

} // namespace finroc
} // namespace core

