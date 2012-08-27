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
  typedef typename tPort<T>::tPortBaseType tPortBaseType;

  static tPortCreationInfo<T> CreatePortCreationInfo()
  {
    tPortCreationInfo<T> pci;
    pci.flags = tPortFlags::cINPUT_PORT;
    return pci;
  }

  template<typename ... ARGS>
  static tPortCreationInfo<T> CreatePortCreationInfo(const ARGS&... args)
  {
    tPortCreationInfo<T> pci(args...);
    pci.flags = tPortFlags::cINPUT_PORT;
    return pci;
  }

public:

  template<typename ... ARGS>
  tParameterBase(const ARGS&... args) :
    tPort<T>(CreatePortCreationInfo(args...))
  {
    this->wrapped->AddAnnotation(new tParameterInfo());
    tPortCreationInfo<T> pci = CreatePortCreationInfo(args...);
    SetConfigEntry(pci.config_entry);
  }

  /*!
   * \param config_entry New Place in Configuration tree, this parameter is configured from (nodes are separated with dots)
   */
  inline void SetConfigEntry(const util::tString& config_entry)
  {
    if (config_entry.length() > 0)
    {
      tParameterInfo* info = static_cast<tParameterInfo*>(this->wrapped->GetAnnotation(tParameterInfo::cTYPE));
      info->SetConfigEntry(config_entry);
    }
  }

  /*!
   * \param new_value new value
   */
  void Set(const T& new_value)
  {
    tPortUtil<T>::BrowserPublish(static_cast<tPortBaseType*>(tPortWrapperBase::wrapped), new_value);
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
extern template class tParameterBase<std::string>;
extern template class tParameterBase<bool>;
extern template class tParameterBase<rrlib::serialization::tMemoryBuffer>;

} // namespace finroc
} // namespace core

#endif // core__parameter__tParameterBase_h__
