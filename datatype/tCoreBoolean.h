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

#ifndef CORE__DATATYPE__TCOREBOOLEAN_H
#define CORE__DATATYPE__TCOREBOOLEAN_H

#include "core/buffers/tCoreInput.h"
#include "core/buffers/tCoreOutput.h"
#include "core/portdatabase/tCoreSerializableImpl.h"

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
class tCoreBoolean : public tCoreSerializable
{
private:

  /*! value */
  bool value;

public:

  /*! Data Type */
  static tDataType* cTYPE;

  tCoreBoolean() :
      value(false)
  {}

  virtual void Assign(tCCPortData* other)
  {
    value = (reinterpret_cast<tCoreBoolean*>(other))->value;
  }

  virtual void Deserialize(tCoreInput& is)
  {
    value = is.ReadBoolean();
  }

  virtual void Deserialize(const util::tString& s)
  {
    value = s.Trim().ToLowerCase().Equals("true");
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

};

} // namespace finroc
} // namespace core

#endif // CORE__DATATYPE__TCOREBOOLEAN_H
