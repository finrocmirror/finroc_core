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

namespace finroc
{
namespace core
{
template<typename T>
const int tCoreRegister<T>::cMAX_ELEMENTS;

template<typename T>
const int tCoreRegister<T>::cMAX_UID;

template<typename T>
const int tCoreRegister<T>::cELEM_INDEX_MASK;

template<typename T>
const int tCoreRegister<T>::cELEM_UID_MASK;

template<typename T>
const int tCoreRegister<T>::cUID_SHIFT;

template<typename T>
const int tCoreRegister<T>::cDELETE_MARK;

template<typename T>
tCoreRegister<T>::tCoreRegister(bool positive_indices) :
  sign(positive_indices ? 0 : 0x80000000),
  current_element_index(0),
  elements(cMAX_ELEMENTS),
  element_uid(cMAX_ELEMENTS),
  elem_count(0),
  obj_mutex()
{
}

template<typename T>
int tCoreRegister<T>::Add(const T& elem)
{
  util::tLock lock1(this);

  if (elem_count >= cMAX_ELEMENTS)
  {
    FINROC_LOG_PRINT_STATIC(rrlib::logging::eLL_ERROR, "Cannot create any more elements, because the core register is full (", elem_count, " elements limit).");
    throw util::tRuntimeException("Register full", CODE_LOCATION_MACRO);
  }

  // find free slot
  while (elements.Get(current_element_index) != NULL)
  {
    IncrementCurElementIndex();
  }

  // get new uid for this slot and update uid table
  int cur_uid = element_uid.Get(current_element_index);
  assert((cur_uid <= cMAX_UID));
  cur_uid++;
  if (cur_uid >= cMAX_UID)
  {
    cur_uid = 0;
  }
  element_uid.Set(current_element_index, cur_uid);

  // set element
  elements.Set(current_element_index, elem);

  // synthesize (virtually) unique handle
  int handle = sign | (cur_uid << cUID_SHIFT) | current_element_index;

  // increment index
  IncrementCurElementIndex();
  elem_count++;
  return handle;
}

template<typename T>
T tCoreRegister<T>::Get(int handle)
{
  int index = handle & cELEM_INDEX_MASK;
  int uid = (handle & cELEM_UID_MASK) >> cUID_SHIFT;
  T candidate = elements.Get(index);
  return element_uid.Get(index) == uid ? candidate : NULL;
}

template<typename T>
void tCoreRegister<T>::IncrementCurElementIndex()
{
  current_element_index++;
  if (current_element_index >= cMAX_ELEMENTS)
  {
    current_element_index = 0;
  }
}

template<typename T>
void tCoreRegister<T>::MarkDeleted(int handle)
{
  util::tLock lock1(this);
  int index = handle & cELEM_INDEX_MASK;
  int uid = (handle & cELEM_UID_MASK) >> cUID_SHIFT;
  assert((elements.Get(index) != NULL));
  assert((element_uid.Get(index) == uid));
  element_uid.Set(index, uid | cDELETE_MARK);
}

template<typename T>
void tCoreRegister<T>::Remove(int handle)
{
  util::tLock lock1(this);
  int index = handle & cELEM_INDEX_MASK;
  int uid = (handle & cELEM_UID_MASK) >> cUID_SHIFT;
  int clean_cur_uid = element_uid.Get(index) & cMAX_UID;
  if (clean_cur_uid == uid)
  {
    elements.Set(index, NULL);
    element_uid.Set(index, clean_cur_uid);
    elem_count--;
  }
  else
  {
    throw util::tRuntimeException("Element removed twice or does not exist", CODE_LOCATION_MACRO);
  }
}

} // namespace finroc
} // namespace core

