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

#ifndef core__datatype__tCoreString_h__
#define core__datatype__tCoreString_h__

#include "rrlib/finroc_core_utils/definitions.h"

#include "rrlib/serialization/tDataTypeBase.h"
#include "rrlib/serialization/tStringInputStream.h"
#include "rrlib/serialization/tStringOutputStream.h"
#include "rrlib/serialization/tSerializable.h"

namespace rrlib
{
namespace serialization
{
class tInputStream;
} // namespace rrlib
} // namespace serialization

namespace finroc
{
namespace core
{
/*!
 * \author Max Reichardt
 *
 * Simple string (buffer) type to use in ports
 * Has 512 bytes initially.
 */
class tCoreString : public rrlib::serialization::tSerializable
{
private:

  /*! String buffer */
  util::tStringBuilder buffer;

public:

  /*! Data Type */
  static rrlib::serialization::tDataTypeBase cTYPE;

  tCoreString();

  virtual void Deserialize(rrlib::serialization::tInputStream& is)
  {
    is >> buffer;
  }

  virtual void Deserialize(rrlib::serialization::tStringInputStream& s)
  {
    Set(s.ReadAll());
  }

  /*!
   * Copy contents to provided StringBuilder
   *
   * \param sb StringBuilder that will contain result
   */
  inline void Get(util::tStringBuilder& sb)
  {
    sb.Delete(0, sb.Length());
    sb.Append(buffer);
  }

  /*!
   * \return String buffer
   */
  inline util::tStringBuilder& GetBuffer()
  {
    return buffer;
  }

  virtual void Serialize(rrlib::serialization::tOutputStream& os) const
  {
    os << buffer;
  }

  virtual void Serialize(rrlib::serialization::tStringOutputStream& os) const
  {
    os.Append(buffer.ToString());
  }

  /*!
   * \param s String to set buffer to
   */
  inline void Set(const util::tString& s)
  {
    buffer.Delete(0, buffer.Length());
    buffer.Append(s);
  }

  virtual const util::tString ToString() const
  {
    return buffer.ToString();
  }

};

} // namespace finroc
} // namespace core

#include "rrlib/serialization/tDataType.h"

extern template class ::rrlib::serialization::tDataType<finroc::core::tCoreString>;

#endif // core__datatype__tCoreString_h__
