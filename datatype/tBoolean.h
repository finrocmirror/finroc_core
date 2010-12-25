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

#ifndef CORE__DATATYPE__TBOOLEAN_H
#define CORE__DATATYPE__TBOOLEAN_H

#include "core/buffers/tCoreInput.h"
#include "core/buffers/tCoreOutput.h"
#include "core/portdatabase/tCoreSerializable.h"

namespace finroc
{
namespace core
{
class tDataType;

/*!
 * \author Max Reichardt
 *
 * boolean type
 */
class tBoolean : public tCoreSerializable
{
private:

  /*! value */
  bool value;

public:

  /*! Data Type */
  static tDataType* cTYPE;

  /*! Instances for True and false */
  static const tBoolean cTRUE, cFALSE;

  tBoolean();

  tBoolean(bool value_);

  virtual void Assign(tCCPortData* other)
  {
    value = (reinterpret_cast<tBoolean*>(other))->value;
  }

  virtual void Deserialize(tCoreInput& is)
  {
    value = is.ReadBoolean();
  }

  virtual void Deserialize(const util::tString& s)
  {
    value = s.Trim().ToLowerCase().Equals("true");
  }

  /*!
   * \return Current value
   */
  inline bool Get() const
  {
    return value;
  }

  inline static const tBoolean* GetInstance(bool value_)
  {
    return value_ ? &(cTRUE) : &(cFALSE);
  }

  inline tDataType* GetType()
  {
    return cTYPE;
  }

  virtual void Serialize(tCoreOutput& os) const
  {
    os.WriteBoolean(value);
  }

  virtual util::tString Serialize() const
  {
    return value ? "true" : "false";
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

#endif // CORE__DATATYPE__TBOOLEAN_H