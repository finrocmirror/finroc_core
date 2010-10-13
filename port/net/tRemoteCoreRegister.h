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
#include "rrlib/finroc_core_utils/tJCBase.h"

#ifndef CORE__PORT__NET__TREMOTECOREREGISTER_H
#define CORE__PORT__NET__TREMOTECOREREGISTER_H

namespace finroc
{
namespace core
{
/*!
 * \author Max Reichardt
 *
 * Used to store data of another runtime environment's core register.
 *
 * The solution here is a two-level-lookup-table. Should be very time and reasonably space efficient.
 *
 * Allow threads to iterate concurrently, while another one makes modifications.
 */
template < typename T = finroc::util::tObject* >
class tRemoteCoreRegister : public util::tUncopyableObject
{
public:
  class tIterator; // inner class forward declaration

public:

  // for synchronization on an object of this class
  mutable util::tMutex obj_mutex;

  /*!
   * First and second level block sizes
   * multiplied this must be CoreRegister.MAX_ELEMENTS
   */
  static const int cLEVEL_ONE_BLOCK_SIZE = 256;

  static const int cLEVEL_TWO_BLOCK_SIZE = 256;

  /*! Masks for blocks */
  static const int cLEVEL_TWO_MASK = cLEVEL_ONE_BLOCK_SIZE - 1;

  static const int cLEVEL_ONE_MASK = ((cLEVEL_TWO_BLOCK_SIZE * cLEVEL_ONE_BLOCK_SIZE) - 1) & (~cLEVEL_TWO_MASK);

  static const int cLEVEL_ONE_SHIFT = 8;

  /*! Two-dimensional array [LEVEL_ONE_BLOCK][LEVEL_TWO_BLOCK] */
  util::tArrayWrapper<util::tArrayWrapper<T>*> elements;

private:

  /*!
   * Wrapper for simpler java/c++ conversion
   */
  inline util::tArrayWrapper<T>* GetLvl2Element(int index)
  {
    return elements.Get(index);
  }

  /*!
   * Wrapper for simpler java/c++ conversion
   */
  inline void SetLvl2Element(int index, util::tArrayWrapper<T>* elem)
  {
    elements.Set(index, elem);
  }

public:

  tRemoteCoreRegister() :
      obj_mutex(),
      elements(cLEVEL_ONE_BLOCK_SIZE)
  {}

  virtual ~tRemoteCoreRegister();

  /*!
   * \param index handle
   * \return Framework element with specified handle
   */
  T Get(int handle);

  /*! Returns iterator for register */
  inline tIterator GetIterator()
  {
    return tIterator(this);
  }

  /*!
   * \param index handle
   * \param elem Framework to put to that position
   */
  void Put(int handle, const T& elem);

  /*!
   * \param i Handle of element to remove
   */
  void Remove(int handle);

public:

  class tIterator : public util::tObject
  {
  private:

    // Outer class RemoteCoreRegister
    tRemoteCoreRegister* const outer_class_ptr;

    /*! Current level one and level two index */
    int lvl1_idx, lvl2_idx;

  public:

    /*! Current level 2 block */
    util::tArrayWrapper<T>* cur_lvl2_block;

    tIterator(tRemoteCoreRegister* const outer_class_ptr_) :
        outer_class_ptr(outer_class_ptr_),
        lvl1_idx(-1),
        lvl2_idx(cLEVEL_TWO_BLOCK_SIZE),
        cur_lvl2_block(NULL)
    {}

    /*! @return Next element in RemoteCoreRegister */
    T Next();

    void Reset();

  };

};

} // namespace finroc
} // namespace core

#include "core/port/net/tRemoteCoreRegister.hpp"

#endif // CORE__PORT__NET__TREMOTECOREREGISTER_H
