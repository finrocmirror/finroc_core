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
#include "rrlib/finroc_core_utils/tJCBase.h"

#ifndef CORE__PORT__STD__TEMPTYPORTDATAIMPL_H
#define CORE__PORT__STD__TEMPTYPORTDATAIMPL_H

#include "core/port/std/tPortDataImpl.h"

namespace finroc
{
namespace core
{
class tCoreInput;
class tCoreOutput;

/*!
 * \author Max Reichardt
 *
 * Base class for any empty/dummy port data
 */
class tEmptyPortDataImpl : public tPortData
{
public:

  tEmptyPortDataImpl() {}

  virtual void Deserialize(tCoreInput& is)
  {
  }

  virtual void Deserialize(const util::tString& s)
  {
  }

  virtual void Serialize(tCoreOutput& os) const
  {
  }

  virtual util::tString Serialize() const
  {
    return "";
  }

};

} // namespace finroc
} // namespace core

#endif // CORE__PORT__STD__TEMPTYPORTDATAIMPL_H
