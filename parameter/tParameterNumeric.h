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

#ifndef core__parameter__tParameterNumeric_h__
#define core__parameter__tParameterNumeric_h__

#include "rrlib/finroc_core_utils/definitions.h"

#include "core/parameter/tParameterBase.h"

namespace finroc
{
namespace core
{
class tFrameworkElement;
class tUnit;

/*!
 * \author Max Reichardt
 *
 * Parameter template class for numeric types
 */
template<typename T>
class tParameterNumeric : public tParameterBase<T>
{
  /*!
   * Caches numeric value of parameter port (optimization)
   */
  class tNumberCache : public tPortListener<T>
  {
  public:

    /*! Cached current value (we will much more often read than it will be changed) */
    volatile T current_value;

    tNumberCache();

    virtual void PortChanged(tAbstractPort* origin, const T& value)
    {
      current_value = value;
    }

  };

public:

  /*! Number cache instance used for this parameter */
  std::shared_ptr<tNumberCache> cache;

  template<typename ... ARGS>
  tParameterNumeric(const ARGS&... args) :
      tParameterBase<T>(args...),
      cache(new tNumberCache())
  {
    cache->current_value = this->Get();
    this->AddPortListener(cache.get());
  }

  /*!
   * \return Current parameter value
   */
  inline T GetValue() const
  {
    return cache->current_value;
  }

  /*!
   * \param b new value
   */
  void Set(T v);

};

} // namespace finroc
} // namespace core

#include "core/parameter/tParameterNumeric.hpp"

namespace finroc
{
namespace core
{
extern template class tParameterNumeric<int>;
extern template class tParameterNumeric<long long int>;
extern template class tParameterNumeric<float>;
extern template class tParameterNumeric<double>;

} // namespace finroc
} // namespace core

#endif // core__parameter__tParameterNumeric_h__
