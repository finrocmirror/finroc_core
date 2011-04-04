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

#ifndef core__datatype__tEnumValue_h__
#define core__datatype__tEnumValue_h__

#include "rrlib/finroc_core_utils/definitions.h"

#include "rrlib/serialization/tDataType.h"
#include "rrlib/finroc_core_utils/container/tSimpleList.h"
#include "rrlib/serialization/tInputStream.h"
#include "rrlib/serialization/tOutputStream.h"
#include "rrlib/xml2_wrapper/tXMLNode.h"
#include "rrlib/serialization/tSerializable.h"
#include "core/portdatabase/tCCType.h"

namespace rrlib
{
namespace serialization
{
class tStringInputStream;
} // namespace rrlib
} // namespace serialization

namespace finroc
{
namespace core
{
/*!
 * \author Max Reichardt
 *
 * Generic enum value.
 * Currently only meant for use in structure parameters.
 * (In port-classes it's probably better to wrap port classes)
 */
class tEnumValue : public rrlib::serialization::tSerializable, public tCCType
{
private:

  /*! Enum value as int */
  int value;

  /*! String constants for enum values */
  const util::tSimpleList<util::tString>* string_constants;

public:

  /*! Data Type */
  static rrlib::serialization::tDataType<tEnumValue> cTYPE;

  /*! Log domain for serialization */
  RRLIB_LOG_CREATE_NAMED_DOMAIN(log_domain, "enum");

private:

  /*!
   * \param name Enum Constant as string
   * \return Enum int value (-1 if string cannot be found)
   */
  int GetStringAsValue(const util::tString& name);

public:

  tEnumValue() :
      value(-1),
      string_constants(new util::tSimpleList<util::tString>())
  {}

  inline void CopyFrom(const tEnumValue& source)
  {
    value = source.value;
    string_constants = source.string_constants;
  }

  virtual void Deserialize(rrlib::serialization::tInputStream& is)
  {
    value = is.ReadInt();
  }

  virtual void Deserialize(rrlib::serialization::tStringInputStream& is);

  virtual void Deserialize(const rrlib::xml2::tXMLNode& node);

  /*!
   * \return current value
   */
  inline int Get() const
  {
    return value;
  }

  virtual void Serialize(rrlib::serialization::tOutputStream& os) const
  {
    os.WriteInt(value);
  }

  virtual void Serialize(rrlib::serialization::tStringOutputStream& sb) const;

  virtual void Serialize(rrlib::xml2::tXMLNode& node) const
  {
    node.SetAttribute("value", util::tInteger::ToString(value));
    node.SetContent(string_constants->Get(value));
  }

  /*!
   * \param e new Value (as integer)
   */
  inline void Set(int e)
  {
    value = e;
  }

  /*!
   * \param string_constants String constants for enum type
   */
  inline void SetStringConstants(const util::tSimpleList<util::tString>* string_constants_)
  {
    this->string_constants = string_constants_;
  }

};

} // namespace finroc
} // namespace core

#endif // core__datatype__tEnumValue_h__
