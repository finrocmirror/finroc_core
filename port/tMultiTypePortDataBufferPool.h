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

#ifndef core__port__tMultiTypePortDataBufferPool_h__
#define core__port__tMultiTypePortDataBufferPool_h__

#include "rrlib/finroc_core_utils/definitions.h"

#include "rrlib/finroc_core_utils/container/tSimpleList.h"
#include "core/tLockOrderLevels.h"
#include "rrlib/serialization/tDataTypeBase.h"
#include "core/port/std/tPortDataBufferPool.h"

namespace finroc
{
namespace core
{
class tPortDataManager;

/*!
 * \author Max Reichardt
 *
 * Buffer pool for specific port and thread.
 * Special version that supports buffers of multiple types.
 * This list is not real-time capable if new types are used.
 */
class tMultiTypePortDataBufferPool : public util::tUncopyableObject
{
private:

  /*! list contains pools for different data types... new pools are added when needed */
  util::tSimpleList<tPortDataBufferPool*> pools;

public:

  /*! Mutex lock order - needs to be locked before AllocationRegister */
  util::tMutexLockOrder obj_mutex;

private:

  /*!
   * \param data_type DataType of buffer to create
   * \return Returns unused buffer of possibly newly created pool
   */
  tPortDataManager* PossiblyCreatePool(rrlib::serialization::tDataTypeBase data_type);

public:

  tMultiTypePortDataBufferPool() :
      pools(2u),
      obj_mutex(tLockOrderLevels::cINNER_MOST - 20)
  {}

  /*!
   * \param data_type DataType of returned buffer.
   * \return Returns unused buffer. If there are no buffers that can be reused, a new buffer is allocated.
   */
  inline tPortDataManager* GetUnusedBuffer(rrlib::serialization::tDataTypeBase data_type)
  {
    // search for correct pool
    for (size_t i = 0u, n = pools.Size(); i < n; i++)
    {
      tPortDataBufferPool* pbp = pools.Get(i);
      if (pbp->data_type == data_type)
      {
        return pbp->GetUnusedBuffer();
      }
    }

    return PossiblyCreatePool(data_type);
  }

  /*!
   * Prints all pools including elements of multi-type pool
   *
   * \param indent Current indentation
   */
  void PrintStructure(int indent, rrlib::logging::tLogStream& output);

  virtual ~tMultiTypePortDataBufferPool()
  {
    // now there shouldn't be the hazard that a new pool is/will be created
    for (size_t i = 0, n = pools.Size(); i < n; i++)
    {
      pools.Get(i)->ControlledDelete();
    }
    pools.Clear();
  }

};

} // namespace finroc
} // namespace core

#endif // core__port__tMultiTypePortDataBufferPool_h__
