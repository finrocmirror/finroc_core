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
#include "core/portdatabase/tDataType.h"

namespace finroc
{
namespace core
{
template<typename T>
tTypedObjectContainer<T>::tTypedObjectContainer(tDataType* type_, util::tObject* object) :
    port_data()
{
  this->type = type_;
}

template<typename T>
void tTypedObjectContainer<T>::Assign(const tCCPortData* other)
{
  assert((GetType()->IsCCType()));

  size_t off = GetType()->virtual_offset;
  memcpy(((char*)&port_data) + off, ((char*)other) + off, GetType()->memcpy_size);

}

template<typename T>
void tTypedObjectContainer<T>::AssignTo(tCCPortData* other) const
{
  assert((GetType()->IsCCType()));

  size_t off = GetType()->virtual_offset;
  memcpy(((char*)other) + off, ((char*)&port_data) + off, GetType()->memcpy_size);

}

template<typename T>
bool tTypedObjectContainer<T>::ContentEquals(const tCCPortData* other) const
{
  assert((GetType()->IsCCType()));

  size_t off = GetType()->virtual_offset;
  return memcmp(((char*)&port_data) + off, ((char*)other) + off, GetType()->memcpy_size) == 0;

}

} // namespace finroc
} // namespace core

