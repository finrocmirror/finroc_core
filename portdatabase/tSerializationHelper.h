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
#ifndef CORE__PORTDATABASE__TSERIALIZATIONHELPER_H
#define CORE__PORTDATABASE__TSERIALIZATIONHELPER_H

#include "core/buffers/tCoreOutput.h"
#include "core/buffers/tCoreInput.h"
#include "core/portdatabase/tCoreSerializable.h"

namespace finroc
{
namespace core
{

class tDataType;

class tSerializationHelper
{
public:
  inline static void Serialize2(tCoreOutput& os, const tCoreSerializable* const port_data2, tDataType* type)
  {
    port_data2->Serialize(os); // should not be a virtual call with a proper compiler
  }
  static void Serialize2(tCoreOutput& os, const void* const port_data2, tDataType* type);

  inline static void Deserialize2(tCoreInput& is, tCoreSerializable* port_data2, tDataType* type)
  {
    port_data2->Deserialize(is); // should not be a virtual call with a proper compiler
  }
  static void Deserialize2(tCoreInput& is, void* port_data2, tDataType* type);
};

} // namespace finroc
} // namespace core


#endif // CORE__PORTDATABASE__TSERIALIZATIONHELPER_H
