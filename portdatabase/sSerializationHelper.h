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

#ifndef core__portdatabase__tSerializationHelper_h__
#define core__portdatabase__tSerializationHelper_h__

#include "rrlib/finroc_core_utils/definitions.h"

#include "rrlib/serialization/tSerializable.h"
#include "rrlib/rtti/tDataTypeBase.h"
#include "rrlib/rtti/tTypedObject.h"


namespace finroc
{
namespace core
{
class tMultiTypePortDataBufferPool;

/*!
 * \author Max Reichardt
 *
 * Helper class:
 * Serializes binary CoreSerializables to hex string - and vice versa.
 */
class sSerializationHelper : public util::tUncopyableObject
{
private:

  //TODO: SFINAE check whether stream operator is implemented
  template<typename T>
  static void Serialize(rrlib::serialization::tOutputStream& os, const T* const port_data2, rrlib::rtti::tDataTypeBase type)
  {
    os << (*port_data2);
  }

  //    template<typename T>
  //    static void serialize(OutputStreamBuffer& os, const T* const portData2, DataType* type) {
  //      throw new RuntimeException(util::tStringBuilder("Serialization not supported for type ") + typeid(T).name());
  //    }

  //TODO: SFINAE check whether stream operator is implemented
  template<typename T>
  inline static void Deserialize(rrlib::serialization::tInputStream& is, T* port_data2, rrlib::rtti::tDataTypeBase type)
  {
    is >> (*port_data2);
  }

public:

  sSerializationHelper() {}

  /*!
   * \param expected Expected data type
   * \param s String to deserialize from
   * \return Data type (null - if type is not available in this runtime)
   */
  static rrlib::rtti::tDataTypeBase GetTypedStringDataType(const rrlib::rtti::tDataTypeBase& expected, const util::tString& s);

  /*!
   * Deserialize object from string
   * (Ignores any type information - buffer needs to have correct type!)
   *
   * \param cs buffer
   * \param s String to deserialize from
   */
  static void TypedStringDeserialize(rrlib::serialization::tSerializable* cs, const util::tString& s);

  /*!
   * Deserialize object from string
   * (Reads type information if type differs from expected data type)
   *
   * @param
   * \param s String to deserialize from
   * \return Typed object
   */
  static rrlib::rtti::tGenericObject* TypedStringDeserialize(const rrlib::rtti::tDataTypeBase& expected, tMultiTypePortDataBufferPool* buffer_pool, const util::tString& s);

  /*!
   * Serialize object to string
   * (Stores type information if type differs from expected data type)
   *
   * \param expected Expected data type
   * \param cs Typed object
   */
  inline static util::tString TypedStringSerialize(const rrlib::rtti::tDataTypeBase& expected, rrlib::rtti::tTypedObject* cs)
  {
    return TypedStringSerialize(expected, cs, cs->GetType());
  }

  /*!
   * Serialize object to string
   * (Stores type information if type differs from expected data type)
   *
   * \param expected Expected data type
   * \param cs object
   * \param cs_type Type of object
   */
  static util::tString TypedStringSerialize(const rrlib::rtti::tDataTypeBase& expected, rrlib::serialization::tSerializable* cs, rrlib::rtti::tDataTypeBase cs_type);

};

} // namespace finroc
} // namespace core

#endif // core__portdatabase__tSerializationHelper_h__
