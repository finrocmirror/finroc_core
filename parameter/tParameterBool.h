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

#ifndef core__parameter__tParameterBool_h__
#define core__parameter__tParameterBool_h__

#include "rrlib/finroc_core_utils/definitions.h"

#include "rrlib/serialization/tDataType.h"
#include "core/datatype/tBoolean.h"
#include "core/port/tPort.h"
#include "core/parameter/tParameter.h"
#include "core/port/tThreadLocalCache.h"
#include "rrlib/serialization/tGenericObject.h"
#include "core/port/cc/tCCPortDataManagerTL.h"
#include "core/port/cc/tCCPortBase.h"
#include "core/port/tPortListener.h"

namespace finroc
{
namespace core
{
class tFrameworkElement;
class tAbstractPort;

/*!
 * \author Max Reichardt
 *
 * Parameter template class for cc types
 */
class tParameterBool : public tParameter<tBoolean>
{
  /*!
   * Caches bool value of parameter port (optimization)
   */
  class tBoolCache : public util::tUncopyableObject, public tPortListener<tBoolean>
  {
  public:

    /*! Cached current value (we will much more often read than it will be changed) */
    volatile bool current_value;

    tBoolCache() :
        current_value(false)
    {}

    virtual void PortChanged(tAbstractPort* origin, const tBoolean& value)
    {
      current_value = (static_cast<const tBoolean&>(value)).Get();
    }

  };

public:

  /*! Bool cache instance used for this parameter */
  std::shared_ptr<tBoolCache> cache;

  /*! Data Type */
  static rrlib::serialization::tDataType<tBoolCache> cTYPE;

  tParameterBool(const util::tString& description, tFrameworkElement* parent, bool default_value, const util::tString& config_entry) :
      tParameter<tBoolean>(description, parent, tBoolean::cTYPE),
      cache(new tBoolCache())
  {
    // this(description,parent,defaultValue);
    this->AddPortListener(cache.get());
    cache->current_value = default_value;
    SetDefault(tBoolean::GetInstance(default_value));
    SetConfigEntry(config_entry);
  }

  tParameterBool(const util::tString& description, tFrameworkElement* parent, bool default_value) :
      tParameter<tBoolean>(description, parent, tBoolean::cTYPE),
      cache(new tBoolCache())
  {
    this->AddPortListener(cache.get());
    cache->current_value = default_value;
    SetDefault(tBoolean::GetInstance(default_value));
  }

  /*!
   * \return Current parameter value
   */
  inline bool GetValue() const
  {
    return cache->current_value;
  }

  /*!
   * \param b new value
   */
  inline void Set(bool b)
  {
    tCCPortDataManagerTL* cb = tThreadLocalCache::Get()->GetUnusedBuffer(tBoolean::cTYPE);
    cb->GetObject()->GetData<tBoolean>()->Set(b);
    (static_cast<tCCPortBase*>(this->wrapped))->BrowserPublishRaw(cb);
    cache->current_value = b;
  }

};

} // namespace finroc
} // namespace core

#endif // core__parameter__tParameterBool_h__
