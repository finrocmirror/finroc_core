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

#ifndef core__portdatabase__tReusableGenericObjectManagerTL_h__
#define core__portdatabase__tReusableGenericObjectManagerTL_h__

#include "rrlib/finroc_core_utils/definitions.h"

#include "rrlib/finroc_core_utils/container/tReusableTL.h"
#include "rrlib/finroc_core_utils/tGarbageCollector.h"
#include "rrlib/serialization/tGenericObjectManager.h"

namespace finroc
{
namespace core
{
/*!
 * \author Max Reichardt
 *
 * Reusable GenericObjectManager
 */
class tReusableGenericObjectManagerTL : public util::tReusableTL, public rrlib::serialization::tGenericObjectManager
{
protected:

  virtual void DeleteThis()
  {
    util::tGarbageCollector::DeleteRT(this);
  }

public:

  tReusableGenericObjectManagerTL() {}

  virtual void CustomDelete(bool called_from_gc)
  {
    this->~tReusableGenericObjectManagerTL();
    delete GetObject();
  }

  inline util::tString GetContentString() const
  {
    return util::tStringBuilder(GetObject()->GetType().GetName()) + " (" + GetObject()->GetRawDataPtr() + ")";
  }

};

} // namespace finroc
} // namespace core

#endif // core__portdatabase__tReusableGenericObjectManagerTL_h__
