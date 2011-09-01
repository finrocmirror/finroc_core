/**
 * You received this file as part of an advanced experimental
 * robotics framework prototype ('finroc')
 *
 * Copyright (C) 2010-2011 Max Reichardt,
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

#ifndef core__parameter__tStructureParameterImplString_h__
#define core__parameter__tStructureParameterImplString_h__

#include "rrlib/finroc_core_utils/definitions.h"

#include "core/parameter/tStructureParameterBase.h"
#include "core/datatype/tCoreString.h"
#include "core/parameter/tStructureParameterImplStandard.h"
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
class tStructureParameterImplString : public tStructureParameterImplStandard<tCoreString>
{
public:

  tStructureParameterImplString(const util::tString& name, const util::tString& default_value, bool constructor_prototype = false) :
      tStructureParameterImplStandard<tCoreString>(name, default_value, constructor_prototype)
  {
  }

  tStructureParameterImplString(const util::tString& name, bool constructor_prototype = false) :
      tStructureParameterImplStandard<tCoreString>(name, "", constructor_prototype)
  {
  }

  /*!
   * \return Current value
   */
  inline std::string Get()
  {
    return GetValue()->ToString().GetStdString();
  }

  /*!
   * \param sb Buffer to store current value in
   */
  inline void Get(util::tStringBuilder sb)
  {
    GetValue()->Get(sb);
  }

  /*!
   * \param s Value to set parameter to
   */
  inline void SetValue(const util::tString& s)
  {
    GetValue()->Set(s);
  }

  /*!
   * \param s Value to set parameter to
   */
  inline void Set(const util::tString& s)
  {
    SetValue(s);
  }

  virtual ::finroc::core::tStructureParameterBase* DeepCopy()
  {
    return new tStructureParameterImplString(GetName(), Get(), false);
  }
};

} // namespace finroc
} // namespace core

#endif // core__parameter__tStructureParameterImplString_h__
