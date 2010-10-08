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
#include "finroc_core_utils/tJCBase.h"

#ifndef CORE__PARAMETER__TSTRUCTUREPARAMETER_H
#define CORE__PARAMETER__TSTRUCTUREPARAMETER_H

#include "core/parameter/tStructureParameterBase.h"

namespace finroc
{
namespace core
{
class tDataType;

template <typename T, bool C>
struct tStructureParameterBufferHelper
{
  static T* Get(tPortData* pd, tCCInterThreadContainer<>* cc)
  {
    return (T*)cc->GetData();
  }
};

template <typename T>
struct tStructureParameterBufferHelper<T, true>
{
  static T* Get(tPortData* pd, tCCInterThreadContainer<>* cc)
  {
    return (T*)pd;
  }
};

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

  typedef tStructureParameterBufferHelper<T, boost::is_base_of<tPortData, T>::value> tHelper;

  /*!
   * \param name Name of parameter
   * \param type DataType of parameter
   * \param const_parameter Constant parameter (usually the case, with constructor parameters)
   * \param constructor_prototype Is this a CreteModuleActionPrototype (no buffer will be allocated)
   * \param default_value Default value
   */
  tStructureParameter(const util::tString& name, tDataType* type, bool const_parameter, bool constructor_prototype, const util::tString& default_value);

  /*!
   * Typical constructor for modules with empty constructor
   * (non-const parameter)
   *
   * \param name Name of parameter
   * \param type DataType of parameter
   * \param default_value Default value
   */
  tStructureParameter(const util::tString& name, tDataType* type, const util::tString& default_value);

  /*!
   * \param name Name of parameter
   * \param type DataType of parameter
   */
  tStructureParameter(const util::tString& name, tDataType* type);

  /*!
   * \return Current parameter value (without lock)
   * (without additional locks value is deleted, when parameter is - which doesn't happen while a module is running)
   */
  inline T* GetValue()
  {
    return tHelper::Get(value, cc_value);
  }

};

} // namespace finroc
} // namespace core

#include "core/parameter/tStructureParameter.hpp"

#endif // CORE__PARAMETER__TSTRUCTUREPARAMETER_H
