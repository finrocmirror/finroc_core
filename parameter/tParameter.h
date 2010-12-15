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

#ifndef CORE__PARAMETER__TPARAMETER_H
#define CORE__PARAMETER__TPARAMETER_H

#include "core/port/tPortCreationInfo.h"
#include "core/port/tPortFlags.h"
#include "core/parameter/tParameterInfo.h"
#include "core/port/std/tPort.h"
#include "core/tAnnotatable.h"
#include "rrlib/finroc_core_utils/log/tLogUser.h"
#include "core/tFrameworkElement.h"
#include "core/port/std/tPortBase.h"

namespace finroc
{
namespace core
{
class tDataType;

/*!
 * \author Max Reichardt
 *
 * Parameter template class for standard types
 */
template<typename T>
class tParameter : public tPort<T>
{
  /*! Special Port class to load value when initialized */
  class tPortImpl : public tPortBase
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
        FINROC_LOG_STREAM(rrlib::logging::eLL_ERROR, ::finroc::core::tFrameworkElement::log_domain, e);
      }
    }

  public:

    tPortImpl(tPortCreationInfo pci) :
        tPortBase(pci),
        info(new tParameterInfo())
    {
      AddAnnotation(info);
    }

  };

  inline static tDataType* GetType(tDataType* dt)
  {
    return dt != NULL ? dt : tDataTypeRegister::GetInstance()->GetDataType<T>();
  }

public:

  tParameter(const util::tString& description, tFrameworkElement* parent, const util::tString& config_entry, tDataType* dt = NULL)
  {
    // this(description,parent,dt);
    this->wrapped = new tPortImpl(tPortCreationInfo(description, parent, GetType(dt), tPortFlags::cINPUT_PORT));
    (static_cast<tPortImpl*>(this->wrapped))->info->SetConfigEntry(config_entry);
  }

  tParameter(const util::tString& description, tFrameworkElement* parent, tDataType* dt = NULL)
  {
    this->wrapped = new tPortImpl(tPortCreationInfo(description, parent, GetType(dt), tPortFlags::cINPUT_PORT));
  }

};

} // namespace finroc
} // namespace core

#endif // CORE__PARAMETER__TPARAMETER_H
