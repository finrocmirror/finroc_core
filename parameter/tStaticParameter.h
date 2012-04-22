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

#ifndef core__parameter__tStaticParameter_h__
#define core__parameter__tStaticParameter_h__

#include "rrlib/finroc_core_utils/definitions.h"

#include "rrlib/rtti/rtti.h"
#include "core/parameter/tStaticParameterBase.h"
#include "core/parameter/tStaticParameterList.h"
#include "core/port/tPortTypeMap.h"
#include "core/parameter/tStaticParameterImplStandard.h"
#include "core/parameter/tStaticParameterImplString.h"
#include "core/parameter/tStaticParameterImplNumeric.h"

namespace finroc
{
namespace core
{

/*!
 * \author Max Reichardt
 *
 * Static Parameter.
 *
 * Unlike "normal" parameters, static parameters cannot be changed while
 * a Finroc application is executing.
 * Thus, static paratemers are more or less construction parameters
 * of modules and groups.
 * They often influence the port structure of these modules and groups.
 */
template <typename T>
class tStaticParameter
{

  typedef typename tPortTypeMap<T>::tStaticParameterImpl tImpl;

  /*! Static Parameter Implementation */
  tImpl* impl;

public:

  /*!
   * Constructor takes variadic argument list... just any properties you want to assign to parameter.
   *
   * The first string is interpreted as parameter name, the second possibly as config entry.
   * A framework element pointer is interpreted as parent.
   * tBounds<T> are parameter's bounds.
   * tUnit argument is parameter's unit.
   * const T& is interpreted as parameter's default value.
   * tPortCreationInfo<T> argument is copied. This is only allowed as first argument.
   *
   * This becomes a little tricky when parameter has numeric or string type.
   * There we have these rules:
   *
   * string type: The second string argument is interpreted as default value. The third as config entry.
   * numeric type: The first numeric argument is interpreted as default value.
   */
  template <typename ... ARGS>
  tStaticParameter(const ARGS&... args) :
    impl(NULL)
  {
    tPortCreationInfo<T> pci(args...);
    impl = new tImpl(pci);
    assert(pci.parent != NULL);
    tStaticParameterList::GetOrCreate(pci.parent)->Add(impl);
  }

  /*!
   * Attach this static parameter to another one.
   * They will share the same value/buffer.
   *
   * \param other Other parameter to attach this one to. Use null or this to detach.
   */
  void AttachTo(tStaticParameter<T>& other)
  {
    impl->AttachTo(other.impl);
  }

  /*!
   * Attach to parameter in outer framework element (e.g. group).
   *
   * \param outer_parameter_attachment Name of outer parameter of finstructable group to configure parameter with.
   * (set by finstructable group containing module with this parameter)
   * \param create_outer Create outer parameter if it does not exist yet?
   */
  void AttachToOuterParameter(const util::tString& outer_parameter_attachment = "", bool create_outer = true)
  {
    impl->SetOuterParameterAttachment(outer_parameter_attachment.length() > 0 ? outer_parameter_attachment : impl->GetName(), create_outer);
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
    return *impl->GetValue();
  }
//  template < bool ENABLED = std::is_same<T, util::tString>::value >
//  const util::tString Get(typename std::enable_if<ENABLED, void*>::type dummy = NULL)
//  {
//    return impl->GetValue()->ToString();
//  }

  /*!
   * \return Current parameter value (without lock)
   * (without additional locks value is deleted, when parameter is - which doesn't happen while a module is running)
   *
   * (not available for numeric and string types)
   */
  template < bool ENABLED = std::is_same<tImpl, tStaticParameterImplStandard<T>>::value >
      inline T* GetValue(typename std::enable_if<ENABLED, void*>::type dummy = NULL)
  {
    return impl->GetValue();
  }

  /*!
   * \return Has parameter changed since last call to "ResetChanged" (or creation).
   */
  inline bool HasChanged()
  {
    return impl->HasChanged();
  }

  /*!
   * Reset "changed flag".
   * The current value will now be the one any new value is compared with when
   * checking whether value has changed.
   */
  inline void ResetChanged()
  {
    impl->ResetChanged();
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

  void SetConfigEntry(const util::tString& config_entry)
  {
    this->impl->SetConfigEntry(config_entry);
  }
};

} // namespace finroc
} // namespace core

#endif // core__parameter__tStaticParameter_h__
