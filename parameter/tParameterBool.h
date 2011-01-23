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

#ifndef CORE__PARAMETER__TPARAMETERBOOL_H
#define CORE__PARAMETER__TPARAMETERBOOL_H

#include "core/port/cc/tCCPort.h"
#include "core/datatype/tBoolean.h"
#include "core/parameter/tParameterInfo.h"
#include "core/port/cc/tCCPortDataContainer.h"
#include "core/parameter/tCCParameter.h"
#include "core/port/tPortCreationInfo.h"
#include "core/port/tPortFlags.h"
#include "core/port/cc/tCCPortBase.h"
#include "core/port/cc/tCCPortListener.h"

namespace finroc
{
namespace core
{
class tFrameworkElement;
class tCCPortData;

/*!
 * \author Max Reichardt
 *
 * Parameter template class for cc types
 */
class tParameterBool : public tCCParameter<tBoolean>
{
  /*! Special Port class to load value when initialized */
  class tPortImpl2 : public tCCParameter< ::finroc::core::tBoolean>::tPortImpl, public tCCPortListener<>
  {
  public:

    /*! Cached current value (we will much more often that it will be changed) */
    volatile bool current_value;

    tPortImpl2(const util::tString& description, tFrameworkElement* parent) :
        tCCParameter< ::finroc::core::tBoolean>::tPortImpl(tPortCreationInfo(description, parent, tBoolean::cTYPE, tPortFlags::cINPUT_PORT)),
        current_value(false)
    {
      AddPortListenerRaw(this);
    }

    virtual void PortChanged(tCCPortBase* origin, const tCCPortData* value)
    {
      current_value = (reinterpret_cast<const tBoolean*>(value))->Get();
    }

  };

public:

  tParameterBool(const util::tString& description, tFrameworkElement* parent, bool default_value, const util::tString& config_entry)
  {
    // this(description,parent,defaultValue);
    this->wrapped = new tPortImpl2(description, parent);
    (static_cast<tPortImpl2*>(this->wrapped))->current_value = default_value;
    SetDefault(*tBoolean::GetInstance(default_value));
    (static_cast<tPortImpl2*>(this->wrapped))->info->SetConfigEntry(config_entry);
  }

  tParameterBool(const util::tString& description, tFrameworkElement* parent, bool default_value)
  {
    this->wrapped = new tPortImpl2(description, parent);
    (static_cast<tPortImpl2*>(this->wrapped))->current_value = default_value;
    SetDefault(*tBoolean::GetInstance(default_value));
  }

  /*!
   * \return Current parameter value
   */
  inline bool Get() const
  {
    return (static_cast<tPortImpl2*>(this->wrapped))->current_value;
  }

  /*!
   * \param b new value
   */
  inline void Set(bool b)
  {
    tCCPortDataContainer<tBoolean>* cb = GetUnusedBuffer();
    cb->GetData()->Set(b);
    BrowserPublish(cb);
    (static_cast<tPortImpl2*>(this->wrapped))->current_value = b;
  }

  /*!
   * Set parameter to value in config file that is associated with given string
   * \param config_entry name of parameter entry in config value
   */
  void SetConfigEntry(const util::tString& config_entry)
  {
    (static_cast<tPortImpl2*>(this->wrapped))->info->SetConfigEntry(config_entry);
  }

};

} // namespace finroc
} // namespace core

#endif // CORE__PARAMETER__TPARAMETERBOOL_H
