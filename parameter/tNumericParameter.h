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
#include "rrlib/finroc_core_utils/tJCBase.h"

#ifndef CORE__PARAMETER__TNUMERICPARAMETER_H
#define CORE__PARAMETER__TNUMERICPARAMETER_H

#include "core/datatype/tBounds.h"
#include "core/port/cc/tBoundedNumberPort.h"
#include "core/port/cc/tCCPortListener.h"

namespace finroc
{
namespace core
{
class tParameterInfo;
class tUnit;
class tCoreNumber;

/*!
 * \author Max Reichardt
 *
 * Parameter template class for numeric types
 */
template<typename T>
class tNumericParameter : public tBoundedNumberPort, public tCCPortListener<tCoreNumber>
{
private:

  /*! Paramater info */
  tParameterInfo* info;

  /*! Cached current value (we will much more often that it will be changed) */
  volatile T current_value;

protected:

  virtual void PostChildInit();

public:

  tNumericParameter(const util::tString& description, tFrameworkElement* parent, tUnit* u, const T& default_value, tBounds b, const util::tString& config_entry);

  tNumericParameter(const util::tString& description, tFrameworkElement* parent, const T& default_value, tBounds b);

  tNumericParameter(const util::tString& description, tFrameworkElement* parent, tUnit* u, const T& default_value, tBounds b);

  /*!
   * \return Current parameter value
   */
  inline T Get()
  {
    return current_value;
  }

  virtual void PortChanged(tCCPortBase* origin, const tCoreNumber* value);

  /*!
   * \param b new value
   */
  void Set(T v);

};

} // namespace finroc
} // namespace core

#include "core/parameter/tNumericParameter.hpp"

#endif // CORE__PARAMETER__TNUMERICPARAMETER_H
