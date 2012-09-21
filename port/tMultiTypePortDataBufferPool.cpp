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
#include "core/port/tMultiTypePortDataBufferPool.h"

namespace finroc
{
namespace core
{
tMultiTypePortDataBufferPool::tMultiTypePortDataBufferPool() :
  tOrderedMutex("tMultiTypePortDataBufferPool", tLockOrderLevels::cINNER_MOST - 20),
  pools()
{}

tMultiTypePortDataBufferPool::~tMultiTypePortDataBufferPool()
{
  // now there shouldn't be the hazard that a new pool is/will be created
  for (auto it = pools.begin(); it != pools.end(); ++it)
  {
    (*it)->ControlledDelete();
  }
  pools.clear();
}

tPortDataManager* tMultiTypePortDataBufferPool::PossiblyCreatePool(const rrlib::rtti::tDataTypeBase& data_type)
{
  rrlib::thread::tLock lock1(*this);

  // search for correct pool
  for (auto it = pools.begin(); it != pools.end(); ++it)
  {
    if ((*it)->data_type == data_type)
    {
      return (*it)->GetUnusedBuffer();
    }
  }

  // create new pool
  tPortDataBufferPool* new_pool = new tPortDataBufferPool(data_type, 2);
  pools.push_back(new_pool);
  return new_pool->GetUnusedBuffer();
}

void tMultiTypePortDataBufferPool::PrintStructure(int indent, std::stringstream& output)
{
  for (int i = 0; i < indent; i++)
  {
    output << " ";
  }
  output << "MultiTypePortDataBufferPool:" << std::endl;
  for (auto it = pools.begin(); it != pools.end(); ++it)
  {
    (*it)->PrintStructure(indent + 2, output);
  }
}

} // namespace finroc
} // namespace core

