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

#ifndef core__parameter__tStaticParameterImplString_h__
#define core__parameter__tStaticParameterImplString_h__

#include "rrlib/finroc_core_utils/definitions.h"

#include "core/parameter/tStaticParameterBase.h"
#include "core/parameter/tStaticParameterImplStandard.h"
#include "rrlib/rtti/tDataTypeBase.h"

namespace finroc
{
namespace core
{
/*!
 * \author Max Reichardt
 *
 * String StaticParameter class for convenience
 */
class tStaticParameterImplString : public tStaticParameterImplStandard<util::tString>
{
public:

  tStaticParameterImplString(const util::tString& name, const util::tString& default_value, bool constructor_prototype = false) :
    tStaticParameterImplStandard<util::tString>(name, default_value, constructor_prototype)
  {
  }

  tStaticParameterImplString(const util::tString& name, bool constructor_prototype = false) :
    tStaticParameterImplStandard<util::tString>(name, "", constructor_prototype)
  {
  }

  template <typename T>
  tStaticParameterImplString(const tPortCreationInfo<T>& pci) :
    tStaticParameterImplStandard<util::tString>(pci)
  {
    if (pci.DefaultValueSet())
    {
      SetValue(pci.GetDefault());
    }
  }


  /*!
   * \return Current value
   */
  inline std::string Get()
  {
    return *GetValue();
  }

  /*!
   * \param s Value to set parameter to
   */
  inline void SetValue(const util::tString& s)
  {
    GetValue()->assign(s);
  }

  /*!
   * \param s Value to set parameter to
   */
  inline void Set(const util::tString& s)
  {
    SetValue(s);
  }

  virtual ::finroc::core::tStaticParameterBase* DeepCopy()
  {
    return new tStaticParameterImplString(GetName(), "", false);
  }
};

} // namespace finroc
} // namespace core

#endif // core__parameter__tStaticParameterImplString_h__
