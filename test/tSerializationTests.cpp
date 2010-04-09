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
#include "core/test/tSerializationTests.h"
#include "core/datatype/tCoreNumber.h"
#include "core/buffers/tMemBuffer.h"
#include "core/buffers/tCoreOutput.h"
#include "core/buffers/tCoreInput.h"

namespace finroc
{
namespace core
{
void tSerializationTests::Main(::finroc::util::tArrayWrapper<util::tString>& args)
{
  //testx((byte)0);
  tCoreNumber* num = new tCoreNumber();
  tMemBuffer* mb = new tMemBuffer();
  tCoreOutput* buf = new tCoreOutput(mb);
  for (int i = -100; i < 69000; i++)
  {
    num->SetValue(i);
    num->Serialize(*buf);
  }
  for (int64 l = 4000000000LL; l < 70000000000LL; l += 1000000000)
  {
    num->SetValue(l);
    num->Serialize(*buf);
  }
  for (float f = 0; f < 2000; f += 4)
  {
    num->SetValue(f);
    num->Serialize(*buf);
  }
  for (double d = 0; d < 5000; d += 4)
  {
    num->SetValue(d);
    num->Serialize(*buf);
  }
  buf->Flush();

  tCoreInput* ci = new tCoreInput(mb);
  util::tSystem::out.Println(ci->Remaining());

  while (ci->Remaining() > 0)
  {
    num->Deserialize(*ci);
    util::tSystem::out.Println(num->ToString());
  }
}

} // namespace finroc
} // namespace core

int main(int argc__, char **argv__)
{
  ::finroc::util::tArrayWrapper< ::finroc::util::tString> sa(argc__ <= 0 ? 0 : (argc__ - 1));
  for (int i = 1; i < argc__; i++)
  {
    sa[i - 1] = ::finroc::util::tString(argv__[i]);
  }
  ::finroc::core::tSerializationTests::Main(sa);
}
