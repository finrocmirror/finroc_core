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

#ifndef CORE__PORTDATABASE__TTYPEDOBJECTCONTAINER_H
#define CORE__PORTDATABASE__TTYPEDOBJECTCONTAINER_H

#include "core/portdatabase/sSerializationHelper.h"
#include "core/portdatabase/tTypedObjectImpl.h"

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
class tDataType;
class tCCPortData;
class tCoreInput;
class tCoreOutput;

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

  tTypedObjectContainer(tDataType* type_, util::tObject* object = NULL);

  /*!
   * Assign/Copy other data to this container - only works with CC data
   *
   * \param other Data to copy to this object
   */
  void Assign(const tCCPortData* other);

  /*!
   * Assign current value to target object
   *
   * \param other Target object
   */
  void AssignTo(tCCPortData* other) const;

  bool ContentEquals(const tCCPortData* other) const;

  virtual void Deserialize(tCoreInput& is)
  {
    sSerializationHelper::Deserialize2(is, &port_data, type);
  }

  virtual void Deserialize(const util::tString& s)
  {
    Deserialize2(&port_data, s);
  }

  virtual void Deserialize(const rrlib::xml2::tXMLNode& node)
  {
    Deserialize2(&port_data, node);
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
    sSerializationHelper::Serialize2(os, &port_data, type);
  }

  virtual util::tString Serialize() const
  {
    return Serialize2(&port_data);
  }

  virtual void Serialize(rrlib::xml2::tXMLNode& node) const
  {
    return Serialize2(&port_data, node);
  }

  static util::tString ToString2(const util::tObject* obj)
  {
    return obj->ToString();
  }
  static util::tString ToString2(const void* obj)
  {
    return obj;
  }
  util::tString Serialize2(const tCoreSerializable* data) const
  {
    return data->Serialize();
  }
  util::tString Serialize2(const void* data) const
  {
    return sSerializationHelper::SerializeToHexString(this);
  }
  void Deserialize2(tCoreSerializable* data, const util::tString& s)
  {
    return data->Deserialize(s);
  }
  void Deserialize2(void* data, const util::tString& s)
  {
    return sSerializationHelper::DeserializeFromHexString(this, s);
  }
  void Serialize2(const tCoreSerializable* data, rrlib::xml2::tXMLNode node) const
  {
    return data->Serialize(node);
  }
  void Serialize2(const void* data, rrlib::xml2::tXMLNode node) const
  {
    tCoreSerializable::Serialize(node);
  }
  void Deserialize2(tCoreSerializable* data, rrlib::xml2::tXMLNode node)
  {
    return data->Deserialize(node);
  }
  void Deserialize2(void* data, rrlib::xml2::tXMLNode node)
  {
    tCoreSerializable::Deserialize(node);
  }

  virtual const util::tString ToString() const
  {
    return ToString2(&port_data);
  }

};

} // namespace finroc
} // namespace core

#include "core/portdatabase/tTypedObjectContainer.hpp"

#endif // CORE__PORTDATABASE__TTYPEDOBJECTCONTAINER_H
