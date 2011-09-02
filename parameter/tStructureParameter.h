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

#ifndef core__parameter__tStructureParameter_h__
#define core__parameter__tStructureParameter_h__

#include "rrlib/finroc_core_utils/definitions.h"

#include "rrlib/serialization/tDataTypeBase.h"
#include "core/parameter/tStructureParameterBase.h"
#include "core/parameter/tStructureParameterList.h"
#include "rrlib/serialization/tGenericObject.h"
#include "core/port/tPortTypeMap.h"
#include "core/parameter/tStructureParameterImplStandard.h"
#include "core/parameter/tStructureParameterImplString.h"
#include "core/parameter/tStructureParameterImplNumeric.h"

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
template <typename T>
class tStructureParameter
{

  typedef typename tPortTypeMap<T>::tStructureParameterImpl tImpl;

  /*! Structure Parameter Implementation */
  tImpl* impl;

public:

  /*!
   * \param name Name of parameter
   * \param parent Parent framework element
   */
  tStructureParameter(const util::tString& name, tFrameworkElement* parent) :
      impl(new tImpl(name))
  {
    tStructureParameterList::GetOrCreate(parent)->Add(impl);
  }

  /*!
   * \param name Name of parameter
   * \param parent Parent framework element
   * \param default_value default value in string representation
   */
  tStructureParameter(const util::tString& name, tFrameworkElement* parent, const util::tString& default_value) :
      impl(new tImpl(name, default_value))
  {
    tStructureParameterList::GetOrCreate(parent)->Add(impl);
  }

  /*!
   * \param name Name of parameter
   * \param parent Parent framework element
   * \param default_value default value
   *
   * (disabled for util::tString to avoid ambiguities with first constructor)
   */
  template < bool NOSTRING = !std::is_same<T, util::tString>::value >
  tStructureParameter(const typename std::enable_if<NOSTRING, util::tString&>::type name, tFrameworkElement* parent, const T& default_value) :
      impl(new tImpl(name, default_value))
  {
    tStructureParameterList::GetOrCreate(parent)->Add(impl);
  }

  /*!
   * (Numeric constructor)
   *
   * \param name Name of parameter
   * \param parent Parent framework element
   * \param default_value default value in string representation
   * \oaram unit Unit of parameter
   */
  template < bool NUMERIC = tPortTypeMap<T>::numeric >
  tStructureParameter(const typename std::enable_if<NUMERIC, util::tString>::type& name, tFrameworkElement* parent, const T& default_value, tUnit* unit) :
      impl(new tImpl(name, default_value, unit))
  {
    tStructureParameterList::GetOrCreate(parent)->Add(impl);
  }

  /*!
   * (Numeric constructor)
   *
   * \param name Name of parameter
   * \param parent Parent framework element
   * \param default_value default value in string representation
   * \param bounds Bounds of parameter
   * \oaram unit Unit of parameter
   */
  template < bool NUMERIC = tPortTypeMap<T>::numeric >
  tStructureParameter(const typename std::enable_if<NUMERIC, util::tString>::type& name, tFrameworkElement* parent, const T& default_value, tBounds<T> bounds = tBounds<T>(), tUnit* unit = &tUnit::cNO_UNIT) :
      impl(new tImpl(name, default_value, bounds, unit))
  {
    tStructureParameterList::GetOrCreate(parent)->Add(impl);
  }

  /*!
   * \return Current parameter value
   * (reference without lock for T = std::string)
   * (without additional locks value is deleted, when parameter is - which doesn't happen while a module is running)
   *
   * (only available for "cheap copy" and string types)
   */
  template < bool ENABLED = typeutil::tIsCCType<T>::value >
  T Get(typename std::enable_if<ENABLED, void*>::type dummy = NULL)
  {
    return impl->Get();
  }
  template < bool ENABLED = std::is_same<T, std::string>::value >
  const std::string& Get(typename std::enable_if<ENABLED, void*>::type dummy = NULL)
  {
    return impl->GetValue()->GetBuffer().GetStdStringRef();
  }
  template < bool ENABLED = std::is_same<T, util::tString>::value >
  const util::tString Get(typename std::enable_if<ENABLED, void*>::type dummy = NULL)
  {
    return impl->GetValue()->ToString();
  }

  /*!
   * \return Current parameter value (without lock)
   * (without additional locks value is deleted, when parameter is - which doesn't happen while a module is running)
   *
   * (not available for numeric and string types)
   */
  template < bool ENABLED = std::is_same<tImpl, tStructureParameterImplStandard<T>>::value >
  inline T* GetValue(typename std::enable_if<ENABLED, void*>::type dummy = NULL)
  {
    return impl->GetValue();
  }

  /*!
   * \param new_value New value
   */
  inline void SetValue(const T& new_value)
  {
    impl->SetValue(new_value);
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
};

} // namespace finroc
} // namespace core

#endif // core__parameter__tStructureParameter_h__
