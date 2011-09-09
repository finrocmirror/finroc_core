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

#ifndef core__plugin__sDynamicLoading_h__
#define core__plugin__sDynamicLoading_h__

#include <vector>
#include <set>

namespace finroc
{
namespace core
{

/*!
 * \author Max Reichardt
 *
 * Contains utility/convenience methods for dynamic loading
 * (especially with respect to finroc libraries)
 */
class sDynamicLoading
{
  // closes dlopen-ed libraries
  class tDLCloser
  {
  public:
    std::vector<void*> loaded;

    tDLCloser() : loaded() {}
    ~tDLCloser();
  };

public:

  /*!
   * dlopen specified library
   * (also takes care of closing library again on program shutdown)
   *
   * \param open Shared library to open (.so file)
   * \return Returns true if successful
   */
  static bool DLOpen(const char* open);

  /*!
   * \return Returns vector with all finroc libraries available on hard disk.
   */
  static std::set<std::string> GetAvailableFinrocLibraries();

  /*!
   * \return Returns .so file in which address provided as argument is found by dladdr.
   */
  static std::string GetBinary(void* addr);

  /*!
   * \return Returns vector with all libfinroc*.so files loaded by current process.
   */
  static std::set<std::string> GetLoadedFinrocLibraries();

  /*!
   * \return Returns vector with all available finroc libraries that haven't been loaded yet.
   */
  static std::vector<std::string> GetLoadableFinrocLibraries();
};

} // namespace finroc
} // namespace core

#endif // core__plugin__sDynamicLoading_h__
