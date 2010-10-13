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
#include "rrlib/finroc_core_utils/tJCBase.h"

#ifndef CORE__PORT__STREAM__TTRANSACTIONPACKET_H
#define CORE__PORT__STREAM__TTRANSACTIONPACKET_H

#include "core/buffers/tCoreOutput.h"
#include "core/portdatabase/tCoreSerializableImpl.h"
#include "core/port/stream/tTransaction.h"
#include "core/buffers/tChunkBuffer.h"

namespace finroc
{
namespace core
{
class tDataType;
class tCoreInput;

/*!
 * \author Max Reichardt
 *
 * Packet containing an arbitrary number of grouped transactions.
 *
 * Not thread-safe when writing
 */
class tTransactionPacket : public tChunkBuffer
{
private:

  /*! Writer for transactions */
  tCoreOutput serializer;

public:

  /*! Is this special/initial packet in stream? */
  bool initial_packet;

  /*! Data type of chunk */
  static tDataType* const cBUFFER_TYPE;

  tTransactionPacket() :
      serializer(this),
      initial_packet(false)
  {}

  inline void Add(tCoreSerializable& data)
  {
    data.Serialize(serializer);
  }

  inline void AddTransaction(tTransaction t)
  {
    t.Serialize(serializer);
  }

  virtual void Deserialize(tCoreInput& is);

  inline void Reset()
  {
    initial_packet = false;
    ::finroc::util::tChunkedBuffer::Clear();
  }

  virtual void Serialize(tCoreOutput& os) const;

};

} // namespace finroc
} // namespace core

#endif // CORE__PORT__STREAM__TTRANSACTIONPACKET_H
