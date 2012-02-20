/**
 * You received this file as part of an advanced experimental
 * robotics framework prototype ('finroc')
 *
 * Copyright (C) 2007-2010 Max Reichardt,
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

#ifndef core__portdatabase__tSerializableReusable_h__
#define core__portdatabase__tSerializableReusable_h__

#include "rrlib/finroc_core_utils/definitions.h"

#include "rrlib/finroc_core_utils/container/tReusable.h"
#include "rrlib/serialization/tSerializable.h"

namespace finroc
{
namespace core
{
/*!
 * \author Max Reichardt
 *
 * This is the base class for some classes that are both Serializable and Reusable
 *
 * They are all meant to be stored in a std::unique_ptr to ensure that they are always recycled
 * when no longer referenced.
 * Furthermore, it is ensured that they are not recycled more than once.
 */
class tSerializableReusable : public util::tReusable, public rrlib::serialization::tSerializable
{
  friend class tRecycler;

protected:

  /*!
   * Recycle call object - after calling this method, object is available in ReusablesPool it originated from
   *
   * (may be overridden by subclasses to perform custom cleanup)
   */
  virtual void GenericRecycle()
  {
    //responsibleThread = -1;
    ::finroc::util::tReusable::Recycle();
  }

  tSerializableReusable() {}

public:

  /*!
   * Deleter for std::unique_ptr that will recycle object instead of deleting it
   */
  class tRecycler
  {
  public:
    void operator()(tSerializableReusable* p) const
    {
      p->GenericRecycle();
    }
  };

  typedef std::unique_ptr<tSerializableReusable, tRecycler> tPtr;
};

/*!
 * Serializable reusable task (e.g. to be executed in separate thread).
 */
class tSerializableReusableTask : public tSerializableReusable
{
public:

  typedef std::unique_ptr<tSerializableReusableTask, tRecycler> tPtr;

  /*!
   * Executes this task.
   * \param self Unique pointer to self - for clean, safe recycling
   */
  virtual void ExecuteTask(tPtr& self) = 0;
};

} // namespace finroc
} // namespace core

#endif // core__portdatabase__tSerializableReusable_h__
