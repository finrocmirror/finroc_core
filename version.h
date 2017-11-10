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
/*!\file    core/version.h
 *
 * \author  Max Reichardt
 *
 * \date    2013-05-08
 *
 * \brief   Contains version information
 *
 * \b version
 *
 * Version information consists of the year - 2000 and month of the release.
 * For numeric comparison, both parts a combined into a 4-digit variant.
 * 9999 stands for "still in development".
 *
 */
//----------------------------------------------------------------------
#ifndef __core__version_h__
#define __core__version_h__

#define CONCATx(a, b) a##b
#define CONCAT(a, b) CONCATx(a, b)

#define FINROC_VERSION_YEAR 14
#define FINROC_VERSION_MONTH 8

#if FINROC_VERSION_MONTH < 10
#define FINROC_VERSION CONCAT(FINROC_VERSION_YEAR, CONCAT(0, FINROC_VERSION_MONTH))
#elif
#define FINROC_VERSION CONCAT(FINROC_VERSION_YEAR, FINROC_VERSION_MONTH)
#endif

#include <string>

namespace finroc
{
inline std::string VersionString()
{
  return std::to_string(FINROC_VERSION_YEAR) + "." + (FINROC_VERSION_MONTH < 10 ? "0" : "") + std::to_string(FINROC_VERSION_MONTH);
}
}


#endif
