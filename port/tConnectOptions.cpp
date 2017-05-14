//
// You received this file as part of Finroc
// A framework for intelligent robot control
//
// Copyright (C) Finroc GbR (finroc.org)
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along
// with this program; if not, write to the Free Software Foundation, Inc.,
// 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
//
//----------------------------------------------------------------------
/*!\file    core/port/tConnectOptions.cpp
 *
 * \author  Max Reichardt
 *
 * \date    2017-05-14
 *
 */
//----------------------------------------------------------------------
#include "core/port/tConnectOptions.h"

//----------------------------------------------------------------------
// External includes (system with <>, local with "")
//----------------------------------------------------------------------

//----------------------------------------------------------------------
// Internal includes with ""
//----------------------------------------------------------------------
#include "core/log_messages.h"
#include "core/port/tUriConnectOptions.h"

//----------------------------------------------------------------------
// Debugging
//----------------------------------------------------------------------
#include <cassert>

//----------------------------------------------------------------------
// Namespace usage
//----------------------------------------------------------------------

//----------------------------------------------------------------------
// Namespace declaration
//----------------------------------------------------------------------
namespace finroc
{
namespace core
{

//----------------------------------------------------------------------
// Forward declarations / typedefs / enums
//----------------------------------------------------------------------

//----------------------------------------------------------------------
// Const values
//----------------------------------------------------------------------
namespace
{
const tConnectionFlags cAUTO_FLAGS = tConnectionFlag::PUBLISHED | tConnectionFlag::DISCONNECTED | tConnectionFlag::CONVERSION | tConnectionFlag::NAMED_PARAMETERS;

inline void DeserializeBase(rrlib::serialization::tInputStream& stream, tConnectOptions& options)
{
  options.flags = tConnectionFlags(stream.ReadShort());
  if (options.flags.Get(tConnectionFlag::CONVERSION))
  {
    stream >> options.conversion_operations;
  }
  else
  {
    options.conversion_operations = rrlib::rtti::conversion::tConversionOperationSequence();
  }
}

inline void SerializeBase(rrlib::serialization::tOutputStream& stream, const tConnectOptions& options, tConnectionFlags flags)
{
  if (options.conversion_operations.Size())
  {
    flags.Set(tConnectionFlag::CONVERSION);
  }
  stream << static_cast<uint16_t>(flags.Raw() & 0xFFFF);
  if (options.conversion_operations.Size())
  {
    stream << options.conversion_operations;
  }
}

}

//----------------------------------------------------------------------
// Implementation
//----------------------------------------------------------------------

void tConnectOptions::Serialize(rrlib::serialization::tOutputStream& stream, bool auto_set_flags) const
{
  SerializeBase(stream, *this, auto_set_flags ? tConnectOptions::UnsetAutoFlags(flags) : flags);
}

tConnectionFlags tConnectOptions::UnsetAutoFlags(const tConnectionFlags& flags)
{
  return tConnectionFlags(flags.Raw() & (~cAUTO_FLAGS.Raw()));
}

rrlib::serialization::tInputStream& operator >> (rrlib::serialization::tInputStream& stream, tConnectOptions& options)
{
  DeserializeBase(stream, options);
  if (options.flags.Get(tConnectionFlag::NAMED_PARAMETERS))
  {
    FINROC_LOG_PRINT(WARNING, "Named parameters not supported in tConnectOptions - ignoring");
    std::map<std::string, std::string> ignored;
    stream >> ignored;
  }
  return stream;
}

rrlib::serialization::tOutputStream& operator << (rrlib::serialization::tOutputStream& stream, const tUriConnectOptions& options)
{
  bool has_parameters = options.flags.Get(tConnectionFlag::NAMED_PARAMETERS);
  SerializeBase(stream, options, tConnectOptions::UnsetAutoFlags(options.flags) | (has_parameters ? tConnectionFlags(tConnectionFlag::NAMED_PARAMETERS) : tConnectionFlags()));
  if (has_parameters)
  {
    stream << options.parameters;
  }
  return stream;
}

rrlib::serialization::tInputStream& operator >> (rrlib::serialization::tInputStream& stream, tUriConnectOptions& options)
{
  DeserializeBase(stream, options);
  if (options.flags.Get(tConnectionFlag::NAMED_PARAMETERS))
  {
    stream >> options.parameters;
  }
  else
  {
    options.parameters.clear();
  }
  return stream;
}

//----------------------------------------------------------------------
// End of namespace declaration
//----------------------------------------------------------------------
}
}
