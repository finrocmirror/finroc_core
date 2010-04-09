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

#ifndef CORE__PORT__STD__TPORTQUEUEELEMENT_H
#define CORE__PORT__STD__TPORTQUEUEELEMENT_H

#include "finroc_core_utils/container/tBoundedQElementContainer.h"

namespace finroc
{
namespace core
{
/*!
 * \author Max Reichardt
 *
 * Chunk/fragment as it is used in port queues.
 */
class tPortQueueElement : public util::tBoundedQElementContainer
{
protected:

  void RecycleContent();

public:

  tPortQueueElement() {}

  //PortDataReference ref;

  //  /** Number of bits for fragment index/size - currently 128 elements */
  //  public static final @SizeT int BITS = 7;
  //
  //  /** Size of one backend chunk - must be 2^n */
  //  public static final @SizeT int SIZE = 1 << BITS;
  //
  //  /** Mask to add absolute index with */
  //  public static final @SizeT int MASK = SIZE - 1;
  //
  //  /** Array backend for chunk */
  //  @InCpp("void* array[SIZE];")
  //  Object[] array = new Object[SIZE];
  //
  //  public PortQueueFragment() {
  //
  //    // initialize array
  //    for (size_t i = 0; i < CHUNK_SIZE; i++) {
  //      array[i] = NULL;
  //    }
  //
  //  }
  //
  /*  @Override
    public void recycle() {
      super.recycle();
    }*/

  void Recycle(bool recycle_content);

  void RecycleContent(void* content);

};

} // namespace finroc
} // namespace core

#endif // CORE__PORT__STD__TPORTQUEUEELEMENT_H
