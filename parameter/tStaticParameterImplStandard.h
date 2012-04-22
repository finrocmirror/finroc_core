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

#ifndef core__parameter__tStaticParameterImplStandard_h__
#define core__parameter__tStaticParameterImplStandard_h__

#include "rrlib/finroc_core_utils/definitions.h"

#include "rrlib/rtti/rtti.h"
#include "core/parameter/tStaticParameterBase.h"

namespace finroc
{
namespace core
{
/*!
 * \author Max Reichardt
 *
 * Static Parameter.
 *
 * Structure paratemers are more or less construction parameters
 * of modules and groups.
 * They typically influence the port structure of these modules and groups.
 *
 * Unlike "normal" parameters, ...
 * - ...they cannot be changed while modules are running.
 * - ...they are stored in an FinstructableGroup's XML-file rather than
 * the attribute tree.
 */
template<typename T>
class tStaticParameterImplStandard : public tStaticParameterBase
{
public:

  /*!
   * \param name Name of parameter
   * \param default_value default value in string representation
   * \param constructor_prototype Is this a CreateModuleAction prototype (no buffer will be allocated)
   */
  template < bool ENABLE = !std::is_same<util::tString, T>::value >
  tStaticParameterImplStandard(const util::tString& name, const util::tString& default_value, typename std::enable_if<ENABLE, bool>::type constructor_prototype = false) :
    tStaticParameterBase(name, rrlib::rtti::tDataType<T>(), constructor_prototype)
  {
    if ((!constructor_prototype) && default_value.length() > 0)
    {
      try
      {
        tStaticParameterBase::Set(default_value);
      }
      catch (const util::tException& e)
      {
        throw util::tRuntimeException(e, CODE_LOCATION_MACRO);
      }
    }
  }

  /*!
   * \param name Name of parameter
   * \param default_value default value
   * \param constructor_prototype Is this a CreateModuleAction prototype (no buffer will be allocated)
   */
  tStaticParameterImplStandard(const util::tString& name, const T& default_value, bool constructor_prototype = false) :
    tStaticParameterBase(name, rrlib::rtti::tDataType<T>(), constructor_prototype)
  {
    if (!constructor_prototype)
    {
      SetValue(default_value);
    }
  }

  /*!
   * \param name Name of parameter
   * \param constructor_prototype Is this a CreateModuleAction prototype (no buffer will be allocated)
   *
   * (disabled for bool to avoid ambiguities with first constructor)
   */
  template < bool NONBOOL = !std::is_same<bool, T>::value >
  tStaticParameterImplStandard(const typename std::enable_if<NONBOOL, util::tString>::type& name, bool constructor_prototype = false) :
    tStaticParameterBase(name, rrlib::rtti::tDataType<T>(), constructor_prototype)
  {}

  tStaticParameterImplStandard(const tPortCreationInfo<T>& pci) :
    tStaticParameterBase(pci.name, rrlib::rtti::tDataType<T>(), false, false, pci.config_entry)
  {
    if (pci.DefaultValueSet())
    {
      rrlib::serialization::tInputStream is(&pci.GetDefaultGeneric());
      is >> *(ValPointer());
    }
  }

  tStaticParameterImplStandard(const tPortCreationInfoBase& pci) :
    tStaticParameterBase(pci.name, rrlib::rtti::tDataType<T>(), false, false, pci.config_entry)
  {
  }

  virtual ::finroc::core::tStaticParameterBase* DeepCopy()
  {
    return new tStaticParameterImplStandard<T>(GetName(), false);
  }

  /*!
   * \return Current parameter value (without lock)
   * (without additional locks value is deleted, when parameter is - which doesn't happen while a module is running)
   */
  template < bool CC = typeutil::tIsCCType<T>::value >
  T Get(typename std::enable_if<CC, void>::type* dummy = NULL)
  {
    return *GetValue();
  }

  /*!
   * \return Current parameter value (without lock)
   * (without additional locks value is deleted, when parameter is - which doesn't happen while a module is running)
   */
  inline T* GetValue()
  {
    rrlib::rtti::tGenericObject* go = ::finroc::core::tStaticParameterBase::ValPointer();
    return go->GetData<T>();
  }

  /*!
   * \param new_value New value
   */
  inline void SetValue(const T& new_value)
  {
    rrlib::rtti::tGenericObject* go = ::finroc::core::tStaticParameterBase::ValPointer();
    rrlib::rtti::sStaticTypeInfo<T>::DeepCopy(new_value, *(go->GetData<T>()));
  }

  /*!
   * (same as SetValue)
   *
   * \param new_value New value
   */
  template < bool ENABLE = !std::is_same<util::tString, T>::value >
  inline void Set(const typename std::enable_if<ENABLE, T>::type& new_value)
  {
    SetValue(new_value);
  }

  /*!
   * (same as SetValue)
   *
   * \param new_value New value
   */
  virtual void Set(const util::tString& new_value)
  {
    tStaticParameterBase::Set(new_value);
  }

};

} // namespace finroc
} // namespace core

#endif // core__parameter__tStaticParameterImplStandard_h__
