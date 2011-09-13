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

#ifndef core__parameter__tStructureParameterImplStandard_h__
#define core__parameter__tStructureParameterImplStandard_h__

#include "rrlib/finroc_core_utils/definitions.h"

#include "rrlib/serialization/tDataTypeBase.h"
#include "core/parameter/tStructureParameterBase.h"
#include "rrlib/serialization/tGenericObject.h"

namespace finroc
{
namespace core
{
/*!
 * \author Max Reichardt
 *
 * Structure Parameter.
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
class tStructureParameterImplStandard : public tStructureParameterBase
{
public:

  /*!
   * \param name Name of parameter
   * \param default_value default value in string representation
   * \param constructor_prototype Is this a CreateModuleAction prototype (no buffer will be allocated)
   */
  tStructureParameterImplStandard(const util::tString& name, const util::tString& default_value, bool constructor_prototype = false) :
      tStructureParameterBase(name, rrlib::serialization::tDataType<T>(), constructor_prototype)
  {
    if ((!constructor_prototype) && default_value.Length() > 0)
    {
      try
      {
        tStructureParameterBase::Set(default_value);
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
  tStructureParameterImplStandard(const util::tString& name, const T& default_value, bool constructor_prototype = false) :
      tStructureParameterBase(name, rrlib::serialization::tDataType<T>(), constructor_prototype)
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
  tStructureParameterImplStandard(const typename std::enable_if<NONBOOL, util::tString>::type& name, bool constructor_prototype = false) :
      tStructureParameterBase(name, rrlib::serialization::tDataType<T>(), constructor_prototype)
  {}

  virtual ::finroc::core::tStructureParameterBase* DeepCopy()
  {
    return new tStructureParameterImplStandard<T>(GetName(), false);
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
    rrlib::serialization::tGenericObject* go = ::finroc::core::tStructureParameterBase::ValPointer();
    return go->GetData<T>();
  }

  /*!
   * \param new_value New value
   */
  inline void SetValue(const T& new_value)
  {
    rrlib::serialization::tGenericObject* go = ::finroc::core::tStructureParameterBase::ValPointer();
    rrlib::serialization::sSerialization::DeepCopy(new_value, *(go->GetData<T>()));
  }

  /*!
   * (same as SetValue)
   *
   * \param new_value New value
   */
  inline void Set(const T& new_value)
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
    tStructureParameterBase::Set(new_value);
  }

};

} // namespace finroc
} // namespace core

#endif // core__parameter__tStructureParameterImplStandard_h__
