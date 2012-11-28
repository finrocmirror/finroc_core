//
// You received this file as part of Finroc
// A Framework for intelligent robot control
//
// Copyright (C) Finroc GbR (finroc.org)
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
//
//----------------------------------------------------------------------
/*!\file    core/internal/tFrameworkElementRegister.hpp
 *
 * \author  Max Reichardt
 *
 * \date    2012-10-28
 *
 */
//----------------------------------------------------------------------

//----------------------------------------------------------------------
// External includes (system with <>, local with "")
//----------------------------------------------------------------------

//----------------------------------------------------------------------
// Internal includes with ""
//----------------------------------------------------------------------

//----------------------------------------------------------------------
// Debugging
//----------------------------------------------------------------------
#include <cassert>

//----------------------------------------------------------------------
// Namespace declaration
//----------------------------------------------------------------------
namespace finroc
{
namespace core
{
namespace internal
{

//----------------------------------------------------------------------
// Forward declarations / typedefs / enums
//----------------------------------------------------------------------

//----------------------------------------------------------------------
// Const values
//----------------------------------------------------------------------

//----------------------------------------------------------------------
// Implementation
//----------------------------------------------------------------------
template<typename T>
int tFrameworkElementRegister<T>::cMAX_ELEMENTS = 0xFFFF;

template<typename T>
int tFrameworkElementRegister<T>::cMAX_UID = 0x7FFF;

template<typename T>
int tFrameworkElementRegister<T>::cELEM_INDEX_MASK = 0xFFFF;

template<typename T>
int tFrameworkElementRegister<T>::cELEM_UID_MASK = 0x7FFF0000;

template<typename T>
int tFrameworkElementRegister<T>::cUID_SHIFT = 16;

template<typename T>
bool tFrameworkElementRegister<T>::register_created = false;

template<typename T>
const int tFrameworkElementRegister<T>::cDELETE_MARK;

template<typename T>
tFrameworkElementRegister<T>::tFrameworkElementRegister(bool positive_indices) :
  sign(positive_indices ? 0 : 0x80000000),
  current_element_index(0),
  elements(new tSlot[cMAX_ELEMENTS]),
  elem_count(0),
  obj_mutex()
{
  register_created = true;
}

template<typename T>
int tFrameworkElementRegister<T>::Add(const T& elem)
{
  rrlib::thread::tLock lock(obj_mutex);

  if (elem_count >= cMAX_ELEMENTS)
  {
    FINROC_LOG_PRINT_STATIC(ERROR, "Cannot create any more elements, because the core register is full (", elem_count, " elements limit).");
    throw std::runtime_error("Register full");
  }

  // find free slot
  while (elements[current_element_index].element != NULL)
  {
    IncrementCurElementIndex();
  }

  // get new uid for this slot and update uid table
  int cur_uid = elements[current_element_index].uid;
  assert((cur_uid <= cMAX_UID));
  cur_uid++;
  if (cur_uid >= cMAX_UID)
  {
    cur_uid = 0;
  }
  elements[current_element_index].uid = cur_uid;

  // set element
  elements[current_element_index].element = elem;

  // synthesize (virtually) unique handle
  int handle = sign | (cur_uid << cUID_SHIFT) | current_element_index;

  // increment index
  IncrementCurElementIndex();
  elem_count++;
  return handle;
}

template<typename T>
T tFrameworkElementRegister<T>::Get(int handle)
{
  int index = handle & cELEM_INDEX_MASK;
  int uid = (handle & cELEM_UID_MASK) >> cUID_SHIFT;
  T candidate = elements[index].element;
  return elements[index].uid == uid ? candidate : NULL;
}

template<typename T>
void tFrameworkElementRegister<T>::IncrementCurElementIndex()
{
  current_element_index++;
  if (current_element_index >= cMAX_ELEMENTS)
  {
    current_element_index = 0;
  }
}

template<typename T>
void tFrameworkElementRegister<T>::MarkDeleted(int handle)
{
  rrlib::thread::tLock lock1(obj_mutex);
  int index = handle & cELEM_INDEX_MASK;
  int uid = (handle & cELEM_UID_MASK) >> cUID_SHIFT;
  assert(elements[index].element);
  assert(elements[index].uid == uid);
  elements[index].uid = uid | cDELETE_MARK;
}

template<typename T>
void tFrameworkElementRegister<T>::Remove(int handle)
{
  rrlib::thread::tLock lock1(obj_mutex);
  int index = handle & cELEM_INDEX_MASK;
  int uid = (handle & cELEM_UID_MASK) >> cUID_SHIFT;
  int clean_cur_uid = elements[index].uid & cMAX_UID;
  if (clean_cur_uid == uid)
  {
    elements[index].element = NULL;
    elements[index].uid = clean_cur_uid;
    elem_count--;
  }
  else
  {
    FINROC_LOG_PRINT_STATIC(ERROR, "Element removed twice or does not exist. This should not happen.");
  }
}

template<typename T>
void tFrameworkElementRegister<T>::SetMaximumNumberOfElements(int max_elements)
{
  if (register_created)
  {
    FINROC_LOG_PRINT_STATIC(ERROR, "Cannot change core register capacity after one was created.");
    return;
  }

  // count bits
  int tmp = max_elements;
  cUID_SHIFT = 0;
  while (tmp > 0)
  {
    tmp >>= 1;
    cUID_SHIFT++;
  }

  // set variables
  cMAX_ELEMENTS = max_elements;
  cMAX_UID = (1 << (31 - cUID_SHIFT)) - 1;
  cELEM_INDEX_MASK = (1 << cUID_SHIFT) - 1;
  cELEM_UID_MASK = 0x7FFFFFFF & (~cELEM_INDEX_MASK);

  FINROC_LOG_PRINTF_STATIC(DEBUG, "Changed maximum number of elements: %d (uid shift: %d, index mask: 0x%X, uid mask: 0x%X, max uid: 0x%X)", cMAX_ELEMENTS, cUID_SHIFT, cELEM_INDEX_MASK, cELEM_UID_MASK, cMAX_UID);
}

//----------------------------------------------------------------------
// End of namespace declaration
//----------------------------------------------------------------------
}
}
}
