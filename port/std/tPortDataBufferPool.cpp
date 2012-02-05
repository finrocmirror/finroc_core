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
#include "core/port/std/tPortDataBufferPool.h"
#include "rrlib/finroc_core_utils/container/tAbstractReusablesPool.h"

namespace finroc
{
namespace core
{
tPortDataBufferPool::tPortDataBufferPool() :
  data_type(NULL)
{
}

tPortDataBufferPool::tPortDataBufferPool(const rrlib::rtti::tDataTypeBase& data_type_, int initial_size) :
  data_type(data_type_)
{
  for (int i = 0; i < initial_size; i++)
  {
    //enqueue(createBuffer());
    Attach(CreateBufferRaw(), true);
  }
}

tPortDataManager* tPortDataBufferPool::CreateBuffer()
{
  tPortDataManager* pdm = CreateBufferRaw();
  Attach(pdm, false);
  return pdm;
}

void tPortDataBufferPool::PrintElement(int indent, const tPortDataManager* pdm, std::stringstream& output) const
{
  if (pdm == NULL)
  {
    return;
  }
  PrintElement(indent, pdm->GetNextInBufferPool(), output);
  for (int i = 0; i < indent; i++)
  {
    output << " ";
  }
  //System.out.print("PortDataManager (");
  //System.out.print(pdm.getCurrentRefCounter().get());
  //System.out.print(" locks): ");
  output << pdm->ToString() << std::endl;
}

void tPortDataBufferPool::PrintStructure(int indent, std::stringstream& output) const
{
  for (int i = 0; i < indent; i++)
  {
    output << " ";
  }
  output << util::tStringBuilder("PortDataBufferPool (") + data_type.GetName() + ")" << std::endl;
  PrintElement(indent + 2, GetLastCreated(), output);
}

} // namespace finroc
} // namespace core

