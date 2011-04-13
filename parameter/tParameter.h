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

#ifndef core__parameter__tParameter_h__
#define core__parameter__tParameter_h__

#include "rrlib/finroc_core_utils/definitions.h"

#include "core/parameter/tParameterBool.h"
#include "core/parameter/tParameterNumeric.h"

namespace finroc
{
namespace core
{

template <typename T, bool NUMERIC>
class tParameterParent : public tParameterBase<T>
{

  typedef tParameterBase<T> tParent;

public:

  tParameterParent(const util::tString& description, tFrameworkElement* parent, const util::tString& config_entry = "") : tParent(description, parent, config_entry) {}

  tParameterParent(const util::tString& description, tFrameworkElement* parent, const T& default_value, tUnit* unit = &(tUnit::cNO_UNIT), const util::tString& config_entry = "") : tParent(description, parent, default_value, unit, config_entry) {}

  template < typename Q = T >
  tParameterParent(const util::tString& description, tFrameworkElement* parent, const T& default_value, typename boost::enable_if_c<tPortTypeMap<Q>::boundable, tBounds<T> >::type b, tUnit* unit = &(tUnit::cNO_UNIT), const util::tString& config_entry = "") : tParent(description, parent, default_value, b, unit, config_entry) {}
};

template <typename T>
class tParameterParent<T, true> : public tParameterNumeric<T>
{

  typedef tParameterNumeric<T> tParent;

public:

  tParameterParent(const util::tString& description, tFrameworkElement* parent, const util::tString& config_entry = "") : tParent(description, parent, config_entry) {}

  tParameterParent(const util::tString& description, tFrameworkElement* parent, const T& default_value, tUnit* unit = &(tUnit::cNO_UNIT), const util::tString& config_entry = "") : tParent(description, parent, default_value, unit, config_entry) {}

  tParameterParent(const util::tString& description, tFrameworkElement* parent, const T& default_value, tBounds<T> b, tUnit* unit = &(tUnit::cNO_UNIT), const util::tString& config_entry = "") : tParent(description, parent, default_value, b, unit, config_entry) {}
};

/*!
 * \author Max Reichardt
 *
 * Parameter template class for all types
 */
template <typename T>
class tParameter : public tParameterParent < T, boost::is_integral<T>::value || boost::is_floating_point<T>::value || boost::is_enum<T>::value >
{
  typedef tParameterParent < T, boost::is_integral<T>::value || boost::is_floating_point<T>::value || boost::is_enum<T>::value > tParent;

public:

  tParameter(const util::tString& description, tFrameworkElement* parent, const util::tString& config_entry = "") : tParent(description, parent, config_entry) {}

  tParameter(const util::tString& description, tFrameworkElement* parent, const T& default_value, tUnit* unit = &(tUnit::cNO_UNIT), const util::tString& config_entry = "") : tParent(description, parent, default_value, unit, config_entry) {}

  template < typename Q = T >
  tParameter(const util::tString& description, tFrameworkElement* parent, const T& default_value, typename boost::enable_if_c<tPortTypeMap<Q>::boundable, tBounds<T> >::type b, tUnit* unit = &(tUnit::cNO_UNIT), const util::tString& config_entry = "") : tParent(description, parent, default_value, b, unit, config_entry) {}

};

template <>
class tParameter<bool> : public tParameterBool
{
public:

  tParameter(const util::tString& description, tFrameworkElement* parent, const util::tString& config_entry = "") : tParameterBool(description, parent, false, config_entry) {}

  tParameter(const util::tString& description, tFrameworkElement* parent, const bool& default_value, tUnit* unit = &(tUnit::cNO_UNIT), const util::tString& config_entry = "") : tParameterBool(description, parent, default_value, config_entry) {}
};

} // namespace finroc
} // namespace core

namespace finroc
{
namespace core
{
extern template class tParameter<int>;
extern template class tParameter<long long int>;
extern template class tParameter<float>;
extern template class tParameter<double>;
extern template class tParameter<tNumber>;
extern template class tParameter<tCoreString>;
extern template class tParameter<bool>;
extern template class tParameter<tEnumValue>;
extern template class tParameter<rrlib::serialization::tMemoryBuffer>;

} // namespace finroc
} // namespace core

#endif // core__parameter__tParameter_h__
