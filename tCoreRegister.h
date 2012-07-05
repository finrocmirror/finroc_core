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

#ifndef core__tCoreRegister_h__
#define core__tCoreRegister_h__

#include "rrlib/finroc_core_utils/definitions.h"

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
 * Current default format of handle is <1 bit sign><15 bit uid at index><16 bit index>
 */
template <typename T>
class tCoreRegister : public boost::noncopyable
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

  // for synchronization on an object of this class
  mutable rrlib::thread::tMutex obj_mutex;

  /*! Maximum number of elements */
  static int cMAX_ELEMENTS;

  /*! Maximum UID index */
  static int cMAX_UID;

  /*! Element index mask */
  static int cELEM_INDEX_MASK;

  /*! Element UID mask */
  static int cELEM_UID_MASK;

  /*! Amount of bits the UID needs to be shifted */
  static int cUID_SHIFT;

  /*! Has any core register been created? In this case the static variables cannot be changed anymore */
  static bool register_created;

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
   * \return Element index mask
   */
  static int GetElementIndexMask()
  {
    return cELEM_INDEX_MASK;
  }

  /*!
   * \return Maximum number of elements
   */
  static int GetMaximumNumberOfElements()
  {
    return cMAX_ELEMENTS;
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

  /*!
   * Sets the maximum number of elements in core register.
   * This may only be called before a core register was created.
   */
  static void SetMaximumNumberOfElements(int max_elements);
};

} // namespace finroc
} // namespace core

#include "core/tCoreRegister.hpp"

#endif // core__tCoreRegister_h__
