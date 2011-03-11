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

#ifndef core__parameter__tStructureParameter_h__
#define core__parameter__tStructureParameter_h__

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
class tStructureParameter : public tStructureParameterBase
{
public:

  tStructureParameter(const util::tString& name) :
      tStructureParameterBase(name, rrlib::serialization::tDataType<T>(), false)
  {}

  /*!
   * \param name Name of parameter
   * \param type DataType of parameter
   * \param constructor_prototype Is this a CreateModuleAction prototype (no buffer will be allocated)
   */
  tStructureParameter(const util::tString& name, rrlib::serialization::tDataTypeBase type, bool constructor_prototype);

  /*!
   * \param name Name of parameter
   * \param type DataType of parameter
   * \param constructor_prototype Is this a CreateModuleAction prototype (no buffer will be allocated)
   * \param default_value Default value
   */
  tStructureParameter(const util::tString& name, rrlib::serialization::tDataTypeBase type, bool constructor_prototype, const util::tString& default_value);

  /*!
   * Typical constructor for modules with empty constructor
   * (non-const parameter)
   *
   * \param name Name of parameter
   * \param type DataType of parameter
   * \param default_value Default value
   */
  tStructureParameter(const util::tString& name, rrlib::serialization::tDataTypeBase type, const util::tString& default_value);

  /*!
   * \param name Name of parameter
   * \param type DataType of parameter
   */
  tStructureParameter(const util::tString& name, rrlib::serialization::tDataTypeBase type);

  virtual ::finroc::core::tStructureParameterBase* DeepCopy()
  {
    return new tStructureParameter<T>(GetName(), GetType(), false, "");
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

};

} // namespace finroc
} // namespace core

#include "core/parameter/tStructureParameter.hpp"

#endif // core__parameter__tStructureParameter_h__
