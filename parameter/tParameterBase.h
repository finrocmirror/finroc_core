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

#ifndef core__parameter__tParameterBase_h__
#define core__parameter__tParameterBase_h__

#include "rrlib/finroc_core_utils/definitions.h"

#include "core/port/tPortCreationInfo.h"
#include "core/port/tPortFlags.h"
#include "core/port/tAbstractPort.h"
#include "core/parameter/tParameterInfo.h"
#include "core/port/tPort.h"
#include "rrlib/serialization/tDataTypeBase.h"

namespace finroc
{
namespace core
{
class tFrameworkElement;
class tUnit;

/*!
 * \author Max Reichardt
 *
 * Parameter template class for standard types
 */
template<typename T>
class tParameterBase : public tPort<T>
{
  inline static rrlib::serialization::tDataTypeBase GetType(const rrlib::serialization::tDataTypeBase& dt)
  {
    return dt != NULL ? dt : rrlib::serialization::tDataType<T>();
  }

public:

  tParameterBase(const util::tString& description, tFrameworkElement* parent, const util::tString& config_entry = "", const rrlib::serialization::tDataTypeBase& dt = NULL) :
      tPort<T>(tPortCreationInfo(description, parent, GetType(dt), tPortFlags::cINPUT_PORT))
  {
    this->wrapped->AddAnnotation(new tParameterInfo());
    SetConfigEntry(config_entry);
  }

  tParameterBase(const util::tString& description, tFrameworkElement* parent, const T& default_value, tUnit* u, const util::tString& config_entry = "", const rrlib::serialization::tDataTypeBase& dt = NULL) :
      tPort<T>(tPortCreationInfo(description, parent, GetType(dt), tPortFlags::cINPUT_PORT, u))
  {
    SetDefault(default_value);
    SetConfigEntry(config_entry);
    this->wrapped->AddAnnotation(new tParameterInfo());
  }

  template < typename Q = T >
  tParameterBase(const util::tString& description, tFrameworkElement* parent, const T& default_value, const typename boost::enable_if_c<tPortTypeMap<Q>::boundable, tBounds<T> >::type& b, tUnit* u = NULL, const util::tString& config_entry = "", const rrlib::serialization::tDataTypeBase& dt = NULL) :
      tPort<T>(tPortCreationInfo(description, parent, GetType(dt), tPortFlags::cINPUT_PORT, u), b)
  {
    SetDefault(default_value);
    SetConfigEntry(config_entry);
    this->wrapped->AddAnnotation(new tParameterInfo());
  }

  /*!
   * \param config_entry New Place in Configuration tree, this parameter is configured from (nodes are separated with dots)
   */
  inline void SetConfigEntry(const util::tString& config_entry)
  {
    if (config_entry.Length() > 0)
    {
      tParameterInfo* info = static_cast<tParameterInfo*>(this->wrapped->GetAnnotation(tParameterInfo::cTYPE));
      info->SetConfigEntry(config_entry);
    }
  }

};

} // namespace finroc
} // namespace core

namespace finroc
{
namespace core
{
extern template class tParameterBase<int>;
extern template class tParameterBase<long long int>;
extern template class tParameterBase<float>;
extern template class tParameterBase<double>;
extern template class tParameterBase<tNumber>;
extern template class tParameterBase<tCoreString>;
extern template class tParameterBase<bool>;
extern template class tParameterBase<tEnumValue>;
extern template class tParameterBase<rrlib::serialization::tMemoryBuffer>;

} // namespace finroc
} // namespace core

#endif // core__parameter__tParameterBase_h__
