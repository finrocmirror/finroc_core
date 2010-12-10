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

#ifndef CORE__PARAMETER__TSTRINGSTRUCTUREPARAMETER_H
#define CORE__PARAMETER__TSTRINGSTRUCTUREPARAMETER_H

#include "core/portdatabase/tDataTypeRegister.h"
#include "core/datatype/tCoreString.h"
#include "core/parameter/tStructureParameterBase.h"
#include "core/parameter/tStructureParameter.h"

namespace finroc
{
namespace core
{
/*!
 * \author Max Reichardt
 *
 * String StructureParameter class for convenience
 */
class tStringStructureParameter : public tStructureParameter<tCoreString>
{
public:

  tStringStructureParameter(const util::tString& name, const util::tString& default_value, bool constructor_prototype) :
      tStructureParameter<tCoreString>(name, tDataTypeRegister::GetInstance()->GetDataType(util::tTypedClass<tCoreString>()), constructor_prototype, default_value)
  {
  }

  tStringStructureParameter(const util::tString& name, const util::tString& default_value) :
      tStructureParameter<tCoreString>(name, tDataTypeRegister::GetInstance()->GetDataType(util::tTypedClass<tCoreString>()), default_value)
  {
  }

  tStringStructureParameter(const util::tString& name) :
      tStructureParameter<tCoreString>(name, tDataTypeRegister::GetInstance()->GetDataType(util::tTypedClass<tCoreString>()), "")
  {
  }

  /* (non-Javadoc)
   * @see org.finroc.core.parameter.StructureParameter#deepCopy()
   */
  virtual ::finroc::core::tStructureParameterBase* DeepCopy()
  {
    return new tStringStructureParameter(GetName(), "", false);
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

};

} // namespace finroc
} // namespace core

#endif // CORE__PARAMETER__TSTRINGSTRUCTUREPARAMETER_H
