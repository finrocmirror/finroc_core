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
#include "core/test/tChunkBufferTest.h"
#include "finroc_core_utils/stream/tInputStreamBuffer.h"
#include "finroc_core_utils/stream/tOutputStreamBuffer.h"

namespace finroc
{
namespace core
{
const bool tChunkBufferTest::cDESTRUCTIVE_SOURCE, tChunkBufferTest::cBLOCKING_READER;
const bool tChunkBufferTest::cBYTE_WRITE;
util::tChunkedBuffer* tChunkBufferTest::buffer;

void tChunkBufferTest::Main(::finroc::util::tArrayWrapper<util::tString>& args)
{
  util::tChunkedBuffer::StaticInit();

  buffer = new util::tChunkedBuffer(cBLOCKING_READER);

  tChunkBufferTest cbt;
  cbt.Start();

  int i = 0;
  util::tInputStreamBuffer* isb = cDESTRUCTIVE_SOURCE ? new util::tInputStreamBuffer(&(buffer->GetDestructiveSource())) : new util::tInputStreamBuffer(buffer);
  while (i < 1000000)
  {
    if (cBLOCKING_READER || isb->MoreDataAvailable())
    {
      int tmp = isb->ReadInt();
      assert((tmp == i));
      tmp = isb->ReadInt();
      assert((tmp == 0));
      int64 tmp3 = isb->ReadLong();
      assert((tmp3 == 42));
      tmp = isb->ReadInt();
      assert((tmp == 0));
      i++;
    }
  }
  isb->Close();
}

void tChunkBufferTest::Run()
{
  util::tOutputStreamBuffer* osb = new util::tOutputStreamBuffer(buffer);
  for (int i = 0; i < 1000000; i++)
  {
    osb->WriteInt(i);
    if (cBYTE_WRITE)
    {
      for (int j = 0; j < 16; j++)
      {
        osb->WriteByte((j == 4) ? 42 : 0);
      }
    }
    else
    {
      osb->WriteInt(0);
      osb->WriteLong(42);
      osb->WriteInt(0);
    }
    osb->Flush();
  }
  osb->Close();
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
  ::finroc::core::tChunkBufferTest::Main(sa);
}
