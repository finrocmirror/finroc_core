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

#ifndef CORE__TCOREREGISTER_H
#define CORE__TCOREREGISTER_H

namespace finroc
{
namespace core
{
/*!
 * \author Max Reichardt
 *
 * This is a very efficient lookup table.
 * It manages handles for Framework elements.
 *
 * Its size is constant and it guarantees that handles are
 * unique... (strictly speaking only for a very long time)
 * so client requests with outdated handles simply
 * fail and do not operate on wrong ports.
 *
 * The class is completely thread safe.
 *
 * Current format of handle is <1 bit sign><15 bit uid at index><16 bit index>
 */
template < typename T = finroc::util::tObject* >
class tCoreRegister : public util::tUncopyableObject
{
private:

  /*! Sign of handles... either 0 or 0x80000000 */
  const int sign;

  /*! Element handle that is used next */
  int current_element_index;

  /*! Array with elements */
  util::tArrayWrapper<T> elements;

  /*! Marks deleted elements in array below */
  static const int cDELETE_MARK = 0x40000000;

  /*! Array with current uid for every element index (the second bit from the front is used to mark deleted elements) */
  util::tIntArrayWrapper element_uid;

  /*! number of elements in register */
  int elem_count;

public:

  // for synchronization on an object of this class
  mutable util::tMutex obj_mutex;

  /*! Maximum number of elements */
  static const int cMAX_ELEMENTS = 0xFFFF;

  /*! Maximum UID index */
  static const int cMAX_UID = 0x7FFF;

  /*! Element index mask */
  static const int cELEM_INDEX_MASK = 0xFFFF;

  /*! Element UID mask */
  static const int cELEM_UID_MASK = 0x7FFF0000;

  /*! Amount of bits the UID needs to be shifted */
  static const int cUID_SHIFT = 16;

private:

  /*!
   * Increment currentElementIndex taking MAX_ELEMENTS into account
   */
  void IncrementCurElementIndex();

public:

  /*!
   * \param positive_indices Positive handles? (or rather negative??)
   */
  tCoreRegister(bool positive_indices);

  /*!
   * Add element to this register
   *
   * \param elem Element to add
   * \return Handle of element. This handle can be used to retrieve it later.
   */
  int Add(const T& elem);

  /*!
   * \param handle Handle of element
   * \return Element
   */
  T Get(int handle);

  /*!
   * Get element by raw index.
   * Shouldn't be used - normally.
   * Some framework-internal mechanism (ThreadLocalCache cleanup) needs it.
   *
   * \param index Raw Index of element
   * \return Element
   */
  inline T GetByRawIndex(int index) const
  {
    return elements.Get(index);
  }

  /*!
   * Mark specified framework element as (soon completely) deleted
   *
   * get() won't return it anymore.
   * getByRawIndex() , however, will.
   *
   * \param handle Handle of element
   */
  void MarkDeleted(int handle);

  /*!
   * Remove element with specified handle
   *
   * \param handle Handle
   */
  void Remove(int handle);

};

} // namespace finroc
} // namespace core

#include "core/tCoreRegister.hpp"

#endif // CORE__TCOREREGISTER_H
