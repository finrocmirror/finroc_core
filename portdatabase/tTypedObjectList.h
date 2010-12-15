/**
 * You received this file as part of an advanced experimental
 * robotics framework prototype ('finroc')
 *
 * Copyright (C) 2010 Max Reichardt,
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

#ifndef CORE__PORTDATABASE__TTYPEDOBJECTLIST_H
#define CORE__PORTDATABASE__TTYPEDOBJECTLIST_H

#include "rrlib/finroc_core_utils/container/tSimpleList.h"
#include "core/port/cc/tCCInterThreadContainer.h"
#include "core/port/std/tPortData.h"

namespace rrlib
{
namespace xml2
{
class tXMLNode;
} // namespace rrlib
} // namespace xml2

namespace finroc
{
namespace core
{
class tTypedObject;
class tDataType;
class tCoreInput;
class tCoreOutput;

/*!
 * \author Max Reichardt
 *
 * List of typed objects
 *
 * Thread-safety: As with other port data, modifying list is not thread-safe.
 */
class tTypedObjectList : public tPortData
{
private:

  /*! Wrapped List backend */
  util::tSimpleList< ::finroc::core::tTypedObject*> list;

  /*! Default type of list elements */
  tDataType* element_type;

  /*! Size of list - capacity is size of 'list' */
  size_t size;

protected:

  /*!
   * Add auto-lock to specified object
   * (calls ThreadLocalCache::get()->addAutoLock(t);
   * for classes who can't, because of cyclic dependencies)
   *
   * \param t object
   */
  void AddAutoLock(const tPortData* t);

  /*!
   * Add auto-lock to specified object
   * (calls ThreadLocalCache::get()->addAutoLock(t);
   * for classes who can't, because of cyclic dependencies)
   *
   * \param t object
   */
  void AddAutoLock(tCCInterThreadContainer<>* t);

  /*!
   * Create new buffer/instance of port data
   *
   * \param dt Data type of buffer to create
   * \return Returns new instance
   */
  ::finroc::core::tTypedObject* CreateBuffer(tDataType* dt);

  /*!
   * \param to Object/buffer to discard (usually it is deleted or recycled)
   */
  virtual void DiscardBuffer(tTypedObject* to) = 0;

  /*!
   * \param index Index
   * \return Element at specified index in this list
   */
  inline ::finroc::core::tTypedObject* GetElement(int index) const
  {
    return list.Get(index);
  }

  /*!
   * Replace buffer at specified index
   *
   * \param index Index
   * \param element (Locked!) Buffer to add to specified index (the replaced one is discarded)
   */
  void SetElement(int index, tTypedObject* element);

public:

  /*!
   * \param element_type Default type of list elements
   */
  tTypedObjectList(tDataType* element_type_);

  virtual ~tTypedObjectList()
  {
    SetCapacity(0u, NULL);
  }

  virtual void Deserialize(tCoreInput& is);

  virtual void Deserialize(const rrlib::xml2::tXMLNode& node);

  /*!
   * Ensure that buffer has at least the specified capacity
   *
   * \param capacity Capacity
   */
  inline void EnsureCapacity(size_t capacity)
  {
    EnsureCapacity(capacity, element_type);
  }

  /*!
   * Ensure that buffer has at least the specified capacity
   *
   * \param capacity Capacity
   * \param dt Data type to fill entries with if size increases (null fills entries with NULL)
   */
  void EnsureCapacity(size_t capacity, tDataType* dt);

  /*!
   * \return capacity of list
   */
  inline size_t GetCapacity() const
  {
    return list.Size();
  }

  /*!
   * \return Type of list elements
   */
  inline tDataType* GetElementType() const
  {
    return element_type;
  }

  /*!
   * \return size of list
   */
  inline size_t GetSize() const
  {
    return size;
  }

  /*!
   * Removes element at specified index
   * (buffer is appended to end of list => capacity remains unchanged)
   *
   * \param index Index at which to remove element
   */
  void RemoveElement(size_t index);

  virtual void Serialize(tCoreOutput& os) const;

  virtual void Serialize(rrlib::xml2::tXMLNode& node) const;

  /*!
   * \param new_capacity New Capacity (new buffers are allocated or deleted if capacity changes)
   * \param dt Data type to fill entries with if size increases (null fills entries with NULL)
   */
  void SetCapacity(size_t new_capacity, tDataType* dt);

  /*!
   * \param new_size New list size (does not change capacity - except of capacity being to small)
   * (fills list with standard element type)
   */
  void SetSize(size_t new_size);

  /*!
   * \param new_size New list size (does not change capacity - except of capacity being to small)
   * \param dt Data type of buffers to fill new entries with if size increases
   */
  void SetSize(size_t new_size, tDataType* dt);

  /*!
   * \param dt Data type
   * \return May value of this type be added to list?
   */
  virtual bool ValidListType(tDataType* dt);

};

} // namespace finroc
} // namespace core

#endif // CORE__PORTDATABASE__TTYPEDOBJECTLIST_H
