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

#ifndef CORE__PARAMETER__TPARAMETERNUMERIC_H
#define CORE__PARAMETER__TPARAMETERNUMERIC_H

#include "core/datatype/tBounds.h"
#include "core/port/cc/tPortNumericBounded.h"
#include "core/parameter/tParameterInfo.h"
#include "core/port/tPortCreationInfo.h"
#include "core/port/cc/tCCPortListener.h"

namespace finroc
{
namespace core
{
class tFrameworkElement;
class tUnit;
class tCCPortBase;
class tCCPortData;

/*!
 * \author Max Reichardt
 *
 * Parameter template class for numeric types
 */
template<typename T>
class tParameterNumeric : public tPortNumericBounded
{
  /*! Special Port class to load value when initialized */
  class tPortImpl2 : public tPortNumericBounded::tPortImpl, public tCCPortListener<>
  {
  private:

    /*! Paramater info */
    tParameterInfo* info;

  public:

    /*! Cached current value (we will much more often that it will be changed) */
    volatile T current_value;

  protected:

    virtual void PostChildInit();

  public:

    tPortImpl2(tPortCreationInfo pci, tBounds b, tUnit* u);

    virtual void PortChanged(tCCPortBase* origin, const tCCPortData* value_raw);

  };

public:

  tParameterNumeric(const util::tString& description, tFrameworkElement* parent, tUnit* u, const T& default_value, tBounds b, const util::tString& config_entry);

  tParameterNumeric(const util::tString& description, tFrameworkElement* parent, const T& default_value, tBounds b);

  tParameterNumeric(const util::tString& description, tFrameworkElement* parent, tUnit* u, const T& default_value, tBounds b);

  /*!
   * \return Current parameter value
   */
  inline T Get() const
  {
    return (static_cast<tPortImpl2*>(this->wrapped))->current_value;
  }

  /*!
   * \param b new value
   */
  void Set(T v);

};

} // namespace finroc
} // namespace core

#include "core/parameter/tParameterNumeric.hpp"

#endif // CORE__PARAMETER__TPARAMETERNUMERIC_H
