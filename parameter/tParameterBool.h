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

#include "core/parameter/tParameterBase.h"
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
class tParameterBool : public tParameterBase<bool>
{
  /*!
   * Caches bool value of parameter port (optimization)
   */
  class tBoolCache : public util::tUncopyableObject, public tPortListener<bool>
  {
  public:

    /*! Cached current value (we will much more often read than it will be changed) */
    std::atomic<bool> current_value;

    tBoolCache() :
      current_value(false)
    {}

    virtual void PortChanged(tAbstractPort& origin, const bool& value)
    {
      current_value = value;
    }

  };

public:

  /*! Bool cache instance used for this parameter */
  std::shared_ptr<tBoolCache> cache;

  template<typename ... ARGS>
  tParameterBool(const ARGS&... args) :
    tParameterBase<bool>(args...),
    cache(new tBoolCache())
  {
    this->AddPortListener(*cache);
    cache->current_value = tPort<bool>::Get();
  }

  /*!
   * \return Current parameter value
   */
  inline bool Get() const
  {
    return cache->current_value;
  }

  /*!
   * \param b new value
   */
  void Set(bool b);

};

} // namespace finroc
} // namespace core

#endif // core__parameter__tParameterBool_h__
