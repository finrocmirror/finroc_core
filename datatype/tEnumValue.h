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

#ifndef CORE__DATATYPE__TENUMVALUE_H
#define CORE__DATATYPE__TENUMVALUE_H

#include "rrlib/finroc_core_utils/container/tSimpleList.h"
#include "core/buffers/tCoreInput.h"
#include "core/buffers/tCoreOutput.h"
#include "rrlib/xml2_wrapper/tXMLNode.h"
#include "core/portdatabase/tCoreSerializable.h"

namespace finroc
{
namespace core
{
class tDataType;

/*!
 * \author Max Reichardt
 *
 * Generic enum value.
 * Currently only meant for use in structure parameters.
 * (In port-classes it's probably better to wrap port classes)
 */
class tEnumValue : public tCoreSerializable
{
private:

  /*! Enum value as int */
  int value;

  /*! String constants for enum values */
  const util::tSimpleList<util::tString>* string_constants;

public:

  /*! Data Type */
  static tDataType* cTYPE;

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

  virtual void Assign(tCCPortData* other)
  {
    value = (reinterpret_cast<tEnumValue*>(other))->value;
    string_constants = (reinterpret_cast<tEnumValue*>(other))->string_constants;
  }

  virtual void Deserialize(tCoreInput& is)
  {
    value = is.ReadInt();
  }

  virtual void Deserialize(const util::tString& s);

  virtual void Deserialize(const rrlib::xml2::tXMLNode& node);

  /*!
   * \return current value
   */
  inline int Get() const
  {
    return value;
  }

  virtual void Serialize(tCoreOutput& os) const
  {
    os.WriteInt(value);
  }

  virtual util::tString Serialize() const;

  virtual void Serialize(rrlib::xml2::tXMLNode& node) const
  {
    node.SetAttribute("value", util::tInteger::ToString(value));
    node.SetTextContent(string_constants->Get(value));
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

#endif // CORE__DATATYPE__TENUMVALUE_H
