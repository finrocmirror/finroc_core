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
#include "core/plugin/tPlugins.h"

namespace finroc
{
namespace core
{
template<typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9, typename P10, typename P11, typename P12>
tConstructorCreateModuleActionBase<P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12>::tConstructorCreateModuleActionBase(const util::tString& type_name, const util::tString& param_names) :
  spl(),
  p1(NULL),
  p2(NULL),
  p3(NULL),
  p4(NULL),
  p5(NULL),
  p6(NULL),
  p7(NULL),
  p8(NULL),
  p9(NULL),
  p10(NULL),
  p11(NULL),
  p12(NULL),
  name(type_name),
  group()
{
  tPlugins::GetInstance()->AddModuleType(this);

  static const util::tString cPARAMETER = "Parameter ";
  std::vector<util::tString> names_temp = param_names.Split(",");
  util::tSimpleList<util::tString> names;
  for (size_t i = 0; i < names_temp.size(); i++)
  {
    names.Add(names_temp[i].Trim());
  }
  while (names.Size() < 12)
  {
    names.Add(cPARAMETER + names.Size());
  }

  p1 = SP1::Create(names.Get(0));
  p2 = SP2::Create(names.Get(1));
  p3 = SP3::Create(names.Get(2));
  p4 = SP4::Create(names.Get(3));
  p5 = SP5::Create(names.Get(4));
  p6 = SP6::Create(names.Get(5));
  p7 = SP7::Create(names.Get(6));
  p8 = SP8::Create(names.Get(7));
  p9 = SP9::Create(names.Get(8));
  p10 = SP10::Create(names.Get(9));
  p11 = SP11::Create(names.Get(10));
  p12 = SP12::Create(names.Get(11));

  CheckStaticParameterList();

}

template<typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9, typename P10, typename P11, typename P12>
void tConstructorCreateModuleActionBase<P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12>::CheckStaticParameterList() const
{
  if (spl.Size() > 0 || p1 == NULL)
  {
    return;
  }
  Add(p1);
  Add(p2);
  Add(p3);
  Add(p4);
  Add(p5);
  Add(p6);
  Add(p7);
  Add(p8);
  Add(p9);
  Add(p10);
  Add(p11);
  Add(p12);
}

} // namespace finroc
} // namespace core

