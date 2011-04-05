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

#ifndef core__parameter__tStructureParameterString_h__
#define core__parameter__tStructureParameterString_h__

#include "rrlib/finroc_core_utils/definitions.h"

#include "core/parameter/tStructureParameterBase.h"
#include "core/datatype/tCoreString.h"
#include "core/parameter/tStructureParameter.h"
#include "rrlib/serialization/tDataTypeBase.h"

namespace finroc
{
namespace core
{
/*!
 * \author Max Reichardt
 *
 * String StructureParameter class for convenience
 */
class tStructureParameterString : public tStructureParameter<tCoreString>
{
public:

  tStructureParameterString(const util::tString& name, const util::tString& default_value, bool constructor_prototype) :
      tStructureParameter<tCoreString>(name, GetDataType(), constructor_prototype, default_value)
  {
  }

  tStructureParameterString(const util::tString& name, const util::tString& default_value) :
      tStructureParameter<tCoreString>(name, GetDataType(), default_value)
  {
  }

  tStructureParameterString(const util::tString& name) :
      tStructureParameter<tCoreString>(name, GetDataType(), "")
  {
  }

  /* (non-Javadoc)
   * @see org.finroc.core.parameter.StructureParameter#deepCopy()
   */
  virtual ::finroc::core::tStructureParameterBase* DeepCopy()
  {
    return new tStructureParameterString(GetName(), "", false);
  }

  /*!
   * \return Current value
   */
  inline util::tString Get()
  {
    return GetValue()->ToString();
  }

  /*!
   * \param sb Buffer to store current value in
   */
  inline void Get(util::tStringBuilder sb)
  {
    GetValue()->Get(sb);
  }

  /*! Helper to get this safely during static initialization */
  inline static rrlib::serialization::tDataTypeBase GetDataType()
  {
    return rrlib::serialization::tDataType<tCoreString>();
  }

};

} // namespace finroc
} // namespace core

#endif // core__parameter__tStructureParameterString_h__
