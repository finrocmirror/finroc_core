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

#ifndef CORE__PORTDATABASE__TTYPEDOBJECTCONTAINER_H
#define CORE__PORTDATABASE__TTYPEDOBJECTCONTAINER_H

#include "core/portdatabase/tDataType.h"
#include "core/port/cc/tCCPortData.h"
#include "core/buffers/tCoreInput.h"
#include "core/buffers/tCoreOutput.h"
#include "core/portdatabase/tTypedObjectImpl.h"

#include "core/portdatabase/tSerializationHelper.h"

namespace finroc
{
namespace core
{
/*!
 * \author Max Reichardt
 *
 * Simple container for an object of an arbitrary data type together
 * with data type information.
 */
template<typename T>
class tTypedObjectContainer : public tTypedObject, public util::tUncopyableObject
{
protected:

  /*! Data in container */
  T port_data;

public:

  tTypedObjectContainer(tDataType* type_, util::tObject* object = NULL) :
      port_data()
  {
    this->type = type_;
  }

  /*!
   * Assign/Copy other data to this container - only works with CC data
   *
   * \param other Data to copy to this object
   */
  inline void Assign(const tCCPortData* other)
  {
    assert((GetType()->IsCCType()));

    size_t off = GetType()->virtual_offset;
    memcpy(((char*)&port_data) + off, ((char*)other) + off, GetType()->memcpy_size);

  }

  /*!
   * Assign current value to target object
   *
   * \param other Target object
   */
  inline void AssignTo(tCCPortData* other) const
  {
    assert((GetType()->IsCCType()));

    size_t off = GetType()->virtual_offset;
    memcpy(((char*)other) + off, ((char*)&port_data) + off, GetType()->memcpy_size);

  }

  inline bool ContentEquals(const tCCPortData* other) const
  {
    assert((GetType()->IsCCType()));

    size_t off = GetType()->virtual_offset;
    return memcmp(((char*)&port_data) + off, ((char*)other) + off, GetType()->memcpy_size) == 0;

  }

  virtual void Deserialize(tCoreInput& is)
  {
    tSerializationHelper::Deserialize2(is, &port_data, type);
  }

  /*!
   * \return Data in container
   */
  inline const T* GetData() const
  {
    return &(port_data);
  }

  inline T* GetData()
  {
    return &(port_data);
  }

  virtual void Serialize(tCoreOutput& os) const
  {
    tSerializationHelper::Serialize2(os, &port_data, type);
  }

  static util::tString ToString2(const util::tObject* obj)
  {
    return obj->ToString();
  }
  static util::tString ToString2(const void* obj)
  {
    return obj;
  }

  virtual const util::tString ToString() const
  {
    return ToString2(&port_data);
  }

};

} // namespace finroc
} // namespace core

#endif // CORE__PORTDATABASE__TTYPEDOBJECTCONTAINER_H
