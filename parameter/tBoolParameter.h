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

#ifndef CORE__PARAMETER__TBOOLPARAMETER_H
#define CORE__PARAMETER__TBOOLPARAMETER_H

#include "core/datatype/tCoreBoolean.h"
#include "core/port/cc/tCCPort.h"
#include "core/port/cc/tCCPortDataContainer.h"
#include "core/parameter/tCCParameter.h"
#include "core/port/cc/tCCPortListener.h"

namespace finroc
{
namespace core
{
/*!
 * \author Max Reichardt
 *
 * Parameter template class for cc types
 */
class tBoolParameter : public tCCParameter<tCoreBoolean>, public tCCPortListener<tCoreBoolean>
{
private:

  /*! Cached current value (we will much more often that it will be changed) */
  volatile bool current_value;

public:

  tBoolParameter(const util::tString& description, tFrameworkElement* parent, bool default_value, const util::tString& config_entry) :
      tCCParameter<tCoreBoolean>(description, parent, *tCoreBoolean::GetInstance(default_value), config_entry, tCoreBoolean::cTYPE),
      current_value(default_value)
  {
    AddPortListener(this);
  }

  tBoolParameter(const util::tString& description, tFrameworkElement* parent, bool default_value) :
      tCCParameter<tCoreBoolean>(description, parent, *tCoreBoolean::GetInstance(default_value), tCoreBoolean::cTYPE),
      current_value(default_value)
  {
    AddPortListener(this);
  }

  /*!
   * \return Current parameter value
   */
  inline bool Get()
  {
    return current_value;
  }

  virtual void PortChanged(tCCPortBase* origin, const tCoreBoolean* value)
  {
    current_value = value->Get();
  }

  /*!
   * \param b new value
   */
  inline void Set(bool b)
  {
    tCCPortDataContainer<tCoreBoolean>* cb = GetUnusedBuffer();
    cb->GetData()->Set(b);
    BrowserPublish(cb);
    current_value = b;
  }

};

} // namespace finroc
} // namespace core

#endif // CORE__PARAMETER__TBOOLPARAMETER_H
