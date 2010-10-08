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
#include "finroc_core_utils/tJCBase.h"

#ifndef CORE__PORT__STD__TCCDATALIST_H
#define CORE__PORT__STD__TCCDATALIST_H

#include "core/port/cc/tCCInterThreadContainer.h"
#include "core/portdatabase/tTypedObjectList.h"

namespace finroc
{
namespace core
{
class tDataType;
class tTypedObject;

/*!
 * \author Max Reichardt
 *
 * List of "cheap copy"-port data that can be used in port itself
 * (in this package, because it is used in standard ports)
 */
template < typename T = tCCPortData >
class tCCDataList : public tTypedObjectList
{
protected:

  virtual void DiscardBuffer(tTypedObject* to)
  {
    (static_cast<tCCInterThreadContainer<T>*>(to))->Recycle2();
  }

public:

  /*!
   * \param element_type DataType for T
   */
  tCCDataList(tDataType* element_type) :
      tTypedObjectList(element_type)
  {
  }

  /*!
   * \param index Index
   * \param t Object to write result of get operation to
   */
  inline const void Get(size_t index, T& t) const
  {
    (static_cast<tCCInterThreadContainer<T>*>(::finroc::core::tTypedObjectList::GetElement(index)))->AssignTo(&(t));
  }

  /*!
   * \param index Index
   * \return locked port data at specified index auto-locked (unlock with getThreadLocalCache.releaseAllLocks())
   */
  inline const T* GetAutoLocked(size_t index) const
  {
    tCCInterThreadContainer<T>* c = (static_cast<tCCInterThreadContainer<T>*>(::finroc::core::tTypedObjectList::GetElement(index)));
    AddAutoLock(c);
    return c->GetData();
  }

  /*!
   * (careful: only locked as long as list is)
   *
   * \param index Index
   * \return port data at specified index
   */
  inline const T* GetWithoutExtraLock(size_t index) const
  {
    return (static_cast<tCCInterThreadContainer<T>*>(::finroc::core::tTypedObjectList::GetElement(index)))->GetData();
  }

  /*!
   * Replace buffer at specified index
   *
   * \param index Index
   * \param t New Buffer
   * \param add_lock Add lock to buffer (usually we do - unless lock for this list has been added already)
   */
  inline void Set(size_t index, T* t)
  {
    (static_cast<tCCInterThreadContainer<T>*>(::finroc::core::tTypedObjectList::GetElement(index)))->Assign(t);
  }

};

} // namespace finroc
} // namespace core

#endif // CORE__PORT__STD__TCCDATALIST_H
