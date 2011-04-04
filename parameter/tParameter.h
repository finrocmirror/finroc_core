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

#include "core/port/tPortCreationInfo.h"
#include "core/port/tPortFlags.h"
#include "core/port/tAbstractPort.h"
#include "core/parameter/tParameterInfo.h"
#include "rrlib/serialization/tDataTypeBase.h"
#include "core/port/tPort.h"

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
class tParameter : public tPort<T>
{
  inline static rrlib::serialization::tDataTypeBase GetType(const rrlib::serialization::tDataTypeBase& dt)
  {
    return dt != NULL ? dt : rrlib::serialization::tDataType<T>();
  }

public:

  tParameter(const util::tString& description, tFrameworkElement* parent, const util::tString& config_entry, const rrlib::serialization::tDataTypeBase& dt = NULL) :
      tPort<T>(tPortCreationInfo(description, parent, GetType(dt), tPortFlags::cINPUT_PORT))
  {
    // this(description,parent,dt);
    this->wrapped->AddAnnotation(new tParameterInfo());
    SetConfigEntry(config_entry);
  }

  tParameter(const util::tString& description, tFrameworkElement* parent, const rrlib::serialization::tDataTypeBase& dt = NULL) :
      tPort<T>(tPortCreationInfo(description, parent, GetType(dt), tPortFlags::cINPUT_PORT))
  {
    this->wrapped->AddAnnotation(new tParameterInfo());
  }

  template < typename Q = T >
  tParameter(const util::tString& description, tFrameworkElement* parent, const util::tString& config_entry, const typename boost::enable_if_c<tPortTypeMap<Q>::boundable, tBounds<T> >::type& b, const rrlib::serialization::tDataTypeBase& dt = NULL, tUnit* u = NULL) :
      tPort<T>(tPortCreationInfo(description, parent, GetType(dt), tPortFlags::cINPUT_PORT, u), b)
  {
    // this(description,parent,b,dt,u);
    this->wrapped->AddAnnotation(new tParameterInfo());
    SetConfigEntry(config_entry);
  }

  template < typename Q = T >
  tParameter(const util::tString& description, tFrameworkElement* parent, const typename boost::enable_if_c<tPortTypeMap<Q>::boundable, tBounds<T> >::type& b, const rrlib::serialization::tDataTypeBase& dt = NULL, tUnit* u = NULL) :
      tPort<T>(tPortCreationInfo(description, parent, GetType(dt), tPortFlags::cINPUT_PORT, u), b)
  {
    this->wrapped->AddAnnotation(new tParameterInfo());
  }

  /*!
   * \param config_entry New Place in Configuration tree, this parameter is configured from (nodes are separated with dots)
   */
  inline void SetConfigEntry(const util::tString& config_entry)
  {
    tParameterInfo* info = static_cast<tParameterInfo*>(this->wrapped->GetAnnotation(tParameterInfo::cTYPE));
    info->SetConfigEntry(config_entry);
  }

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
extern template class tParameter<tBoolean>;
extern template class tParameter<tEnumValue>;
extern template class tParameter<rrlib::serialization::tMemoryBuffer>;

} // namespace finroc
} // namespace core

#endif // core__parameter__tParameter_h__
