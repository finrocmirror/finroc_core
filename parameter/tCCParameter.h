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

#ifndef CORE__PARAMETER__TCCPARAMETER_H
#define CORE__PARAMETER__TCCPARAMETER_H

#include "core/port/tPortCreationInfo.h"
#include "core/port/tPortFlags.h"
#include "core/parameter/tParameterInfo.h"
#include "core/tAnnotatable.h"
#include "core/port/cc/tCCPort.h"
#include "rrlib/finroc_core_utils/log/tLogUser.h"

namespace finroc
{
namespace core
{
class tDataType;

/*!
 * \author Max Reichardt
 *
 * Parameter template class for cc types
 */
template<typename T>
class tCCParameter : public tCCPort<T>
{
private:

  /*! Paramater info */
  tParameterInfo* info;

protected:

  virtual void PostChildInit()
  {
    ::finroc::core::tFrameworkElement::PostChildInit();
    try
    {
      info->LoadValue(true);
    }
    catch (const util::tException& e)
    {
      FINROC_LOG_STREAM(rrlib::logging::eLL_ERROR, log_domain, e);
    }
  }

public:

  tCCParameter(const util::tString& description, tFrameworkElement* parent, const T* default_value, const util::tString& config_entry, tDataType* dt = NULL) :
      tCCPort<T>(tPortCreationInfo(description, parent, dt, tPortFlags::cINPUT_PORT)),
      info(new tParameterInfo())
  {
    // this(description,parent,defaultValue,dt);
    AddAnnotation(info);
    SetDefault(*default_value);
    info->SetConfigEntry(config_entry);
  }

  tCCParameter(const util::tString& description, tFrameworkElement* parent, const T* default_value, tDataType* dt = NULL) :
      tCCPort<T>(tPortCreationInfo(description, parent, dt, tPortFlags::cINPUT_PORT)),
      info(new tParameterInfo())
  {
    AddAnnotation(info);
    SetDefault(*default_value);
  }

};

} // namespace finroc
} // namespace core

#endif // CORE__PARAMETER__TCCPARAMETER_H
