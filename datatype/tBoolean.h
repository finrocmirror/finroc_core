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

#ifndef core__datatype__tBoolean_h__
#define core__datatype__tBoolean_h__

#include "rrlib/finroc_core_utils/definitions.h"

#include "rrlib/serialization/tDataTypeBase.h"
#include "rrlib/serialization/tInputStream.h"
#include "rrlib/serialization/tStringInputStream.h"
#include "rrlib/serialization/tOutputStream.h"
#include "rrlib/serialization/tStringOutputStream.h"
#include "rrlib/serialization/tSerializable.h"
#include "core/portdatabase/tCCType.h"

namespace finroc
{
namespace core
{
/*!
 * \author Max Reichardt
 *
 * boolean type
 */
class tBoolean : public rrlib::serialization::tSerializable, public tCCType
{
private:

  /*! value */
  bool value;

public:

  /*! Data Type */
  static rrlib::serialization::tDataTypeBase cTYPE;

  /*! Instances for True and false */
  static const tBoolean cTRUE, cFALSE;

  tBoolean();

  tBoolean(bool value_);

  inline void CopyFrom(const tBoolean& source)
  {
    value = source.value;
  }

  virtual void Deserialize(rrlib::serialization::tInputStream& is)
  {
    value = is.ReadBoolean();
  }

  virtual void Deserialize(rrlib::serialization::tStringInputStream& is)
  {
    util::tString s = is.ReadWhile("", rrlib::serialization::tStringInputStream::cLETTER | rrlib::serialization::tStringInputStream::cWHITESPACE, true);
    value = s.ToLowerCase().Equals("true");
  }

  /*!
   * \return Current value
   */
  inline bool Get() const
  {
    return value;
  }

  inline static const tBoolean GetInstance(bool value_)
  {
    return value_ ? cTRUE : cFALSE;
  }

  virtual void Serialize(rrlib::serialization::tOutputStream& os) const
  {
    os.WriteBoolean(value);
  }

  virtual void Serialize(rrlib::serialization::tStringOutputStream& os) const
  {
    os.Append(value ? "true" : "false");
  }

  /*!
   * \param new_value New Value
   */
  inline void Set(bool new_value)
  {
    value = new_value;
  }

};

} // namespace finroc
} // namespace core

#include "rrlib/serialization/tDataType.h"

extern template class ::rrlib::serialization::tDataType<finroc::core::tBoolean>;

#endif // core__datatype__tBoolean_h__
