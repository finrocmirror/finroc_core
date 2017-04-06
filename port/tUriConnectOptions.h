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
/*!\file    core/port/tUriConnectOptions.h
 *
 * \author  Max Reichardt
 *
 * \date    2017-01-28
 *
 * \brief   Contains tUriConnectOptions
 *
 * \b tUriConnectOptions
 *
 * Connect options for URI connectors.
 * They allow to specify additional parameters as key/value pairs.
 * These parameter are scheme-specific - and could be e.g. QoS parameters.
 *
 */
//----------------------------------------------------------------------
#ifndef __core__port__tUriConnectOptions_h__
#define __core__port__tUriConnectOptions_h__

//----------------------------------------------------------------------
// External includes (system with <>, local with "")
//----------------------------------------------------------------------

//----------------------------------------------------------------------
// Internal includes with ""
//----------------------------------------------------------------------
#include "core/port/tConnectOptions.h"

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
// Class declaration
//----------------------------------------------------------------------
//! Connect options for URI connectors
/*!
 * Connect options for URI connectors.
 * They allow to specify additional parameters as key/value pairs.
 * These parameter are scheme-specific - and could be e.g. QoS parameters.
 */
struct tUriConnectOptions : public tConnectOptions
{
  using tConnectOptions::tConnectOptions;

  /*! Additional parameters as key/value pairs */
  std::map<std::string, std::string> parameters;
};

inline rrlib::serialization::tOutputStream& operator << (rrlib::serialization::tOutputStream& stream, const tUriConnectOptions& options)
{
  stream << static_cast<const tConnectOptions&>(options);
  stream << options.parameters;
  return stream;
}

inline rrlib::serialization::tInputStream& operator >> (rrlib::serialization::tInputStream& stream, tUriConnectOptions& options)
{
  stream >> static_cast<tConnectOptions&>(options);
  stream >> options.parameters;
  return stream;
}

//----------------------------------------------------------------------
// End of namespace declaration
//----------------------------------------------------------------------
}
}


#endif
