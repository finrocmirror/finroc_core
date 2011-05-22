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

#ifndef core__port__rpc__tCallParameter_h__
#define core__port__rpc__tCallParameter_h__

#include "rrlib/finroc_core_utils/definitions.h"

#include "core/port/cc/tCCPortDataManager.h"
#include "core/datatype/tNumber.h"
#include "rrlib/serialization/tGenericObject.h"

#include "core/port/tPortDataPtr.h"

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
 * Storage for a parameter
 *
 * CC Objects: If call is executed in the same runtime environment, object is stored inside
 * otherwise it is directly serialized
 */
class tCallParameter : public util::tObject
{
  friend class tAbstractCall;

public:

  /*! Constants for different types of parameters in serialization */
  static const int8 cNULLPARAM = 0, cNUMBER = 1, cOBJECT = 2;

  /*! Storage for numeric parameter */
  tNumber number;

  /*! Object Parameter */
  tPortDataPtr<rrlib::serialization::tGenericObject> value;

  /*! Type of parameter (see constants at beginning of class) */
  int8 type;

private:

  tPortDataPtr<rrlib::serialization::tGenericObject> Lock(rrlib::serialization::tGenericObject* tmp);

public:

  tCallParameter();

  inline void Clear()
  {
    type = cNULLPARAM;
    value.reset();
  }

  virtual ~tCallParameter()
  {
    Recycle();
  }

  void Deserialize(rrlib::serialization::tInputStream& is);

  void Recycle();

  void Serialize(rrlib::serialization::tOutputStream& oos) const;

};

} // namespace finroc
} // namespace core

#endif // core__port__rpc__tCallParameter_h__
