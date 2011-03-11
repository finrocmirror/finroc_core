/**
 * You received this file as part of an advanced experimental
 * robotics framework prototype ('finroc')
 *
 * Copyright (C) 2011 Max Reichardt,
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
#ifndef core__portdatabase__tSharedPtrDeleteHandler_h__
#define core__portdatabase__tSharedPtrDeleteHandler_h__

#include "rrlib/finroc_core_utils/definitions.h"

namespace finroc
{
namespace core
{

/*!
 * \author Max Reichardt
 *
 * Custom delete handler for shared pointers.
 * Used in finroc to decrease reference counter instead of deleting buffer.
 */
template <typename T>
class tSharedPtrDeleteHandler
{
public:
  T* manager;
  bool active;

  tSharedPtrDeleteHandler(T* m) : manager(m), active(true) {}

  void operator()(void* irrelevant)
  {
    T::SharedPointerRelease(manager, active);
  }

  /**
   * Retrieve manager for data
   *
   * \param ptr data
   * \param reset_active_flag Reset active flag (set when unused buffers are handed to user)
   * \return Manager for data - or null if it does not exist
   */
  template <typename S>
  static T* GetManager(const std::shared_ptr<S>& ptr, bool reset_active_flag = false)
  {
    typedef tSharedPtrDeleteHandler<T> tHandler;
    tHandler* h = std::get_deleter<tHandler>(ptr);
    if (h == NULL)
    {
      return NULL;
    }
    h->active &= (!reset_active_flag);
    return h->manager;
  }
};

} // namespace finroc
} // namespace core

#endif // core__portdatabase__tSharedPtrDeleteHandler_h__
