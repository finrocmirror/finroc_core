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

#ifndef CORE__PORTDATABASE__TSERIALIZATIONHELPER_H
#define CORE__PORTDATABASE__TSERIALIZATIONHELPER_H

#include "core/portdatabase/tCoreSerializable.h"

namespace finroc
{
namespace core
{
class tCoreInput;
class tCoreOutput;
class tDataType;
class tMultiTypePortDataBufferPool;
class tTypedObject;

/*!
 * \author Max Reichardt
 *
 * Helper class:
 * Serializes binary CoreSerializables to hex string - and vice versa.
 */
class sSerializationHelper : public util::tUncopyableObject
{
private:

  /*! int -> hex char */
  static ::finroc::util::tArrayWrapper<char> cTO_HEX;

  /*! hex char -> int */
  static ::finroc::util::tArrayWrapper<int> cTO_INT;

public:

  sSerializationHelper() {}

  /*!
   * Deserializes binary CoreSerializable from hex string
   *
   * \param cs CoreSerializable
   * \param s Hex String to deserialize from
   */
  static void DeserializeFromHexString(tCoreSerializable* cs, const util::tString& s);

  /*!
   * \param expected Expected data type
   * \param s String to deserialize from
   * \return Data type (null - if type is not available in this runtime)
   */
  static tDataType* GetTypedStringDataType(tDataType* expected, const util::tString& s);

  inline static void Serialize2(tCoreOutput& os, const tCoreSerializable* const port_data2, tDataType* type)
  {
    port_data2->Serialize(os); // should not be a virtual call with a proper compiler
  }
  static void Serialize2(tCoreOutput& os, const void* const port_data2, tDataType* type);

  inline static void Deserialize2(tCoreInput& is, tCoreSerializable* port_data2, tDataType* type)
  {
    port_data2->Deserialize(is); // should not be a virtual call with a proper compiler
  }
  static void Deserialize2(tCoreInput& is, void* port_data2, tDataType* type);

  /*!
   * Serializes binary CoreSerializable to hex string
   *
   * \param cs CoreSerializable
   * \return Hex string
   */
  static util::tString SerializeToHexString(const tCoreSerializable* cs);

  static void StaticInit();

  /*!
   * Deserialize object from string
   * (Ignores any type information - buffer needs to have correct type!)
   *
   * \param cs buffer
   * \param s String to deserialize from
   */
  static void TypedStringDeserialize(tCoreSerializable* cs, const util::tString& s);

  /*!
   * Deserialize object from string
   * (Reads type information if type differs from expected data type)
   *
   * @param
   * \param s String to deserialize from
   * \return Typed object
   */
  static tTypedObject* TypedStringDeserialize(tDataType* expected, tMultiTypePortDataBufferPool* buffer_pool, const util::tString& s);

  /*!
   * Serialize object to string
   * (Stores type information if type differs from expected data type)
   *
   * \param expected Expected data type
   * \param cs Typed object
   */
  static util::tString TypedStringSerialize(tDataType* expected, tTypedObject* cs);

};

} // namespace finroc
} // namespace core

#endif // CORE__PORTDATABASE__TSERIALIZATIONHELPER_H
