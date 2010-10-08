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
#include "finroc_core_utils/tJCBase.h"

#ifndef CORE__BUFFERS__TCHUNKBUFFER_H
#define CORE__BUFFERS__TCHUNKBUFFER_H

#include "core/buffers/tCoreInput.h"
#include "core/buffers/tCoreOutput.h"
#include "finroc_core_utils/stream/tChunkedBuffer.h"
#include "core/port/std/tPortDataImpl.h"

namespace finroc
{
namespace core
{
class tDataType;

/*!
 * \author Max Reichardt
 *
 * ChunkedBuffer that can be used as port data
 */
class tChunkBuffer : public util::tChunkedBuffer, public tPortData
{
public:

  /*! Data type of chunk */
  static tDataType* const cBUFFER_TYPE;

  tChunkBuffer(bool blocking_readers);

  tChunkBuffer();

  virtual void Deserialize(tCoreInput& is)
  {
    ::finroc::util::tChunkedBuffer::Deserialize(is);
  }

  virtual void HandleRecycle()
  {
    Clear();
  }

  virtual void Serialize(tCoreOutput& os) const
  {
    ::finroc::util::tChunkedBuffer::Serialize(os);
  }

};

} // namespace finroc
} // namespace core

#endif // CORE__BUFFERS__TCHUNKBUFFER_H
