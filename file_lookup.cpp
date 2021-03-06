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
/*!\file    core/file_lookup.cpp
 *
 * \author  Max Reichardt
 *
 * \date    2013-05-07
 *
 */
//----------------------------------------------------------------------
#include "core/file_lookup.h"

//----------------------------------------------------------------------
// External includes (system with <>, local with "")
//----------------------------------------------------------------------
#include <sys/stat.h> // for stat() function
#include <unistd.h>
#include <limits.h>

//----------------------------------------------------------------------
// Internal includes with ""
//----------------------------------------------------------------------
#include "core/log_messages.h"

//----------------------------------------------------------------------
// Debugging
//----------------------------------------------------------------------
#include <cassert>
#include <fstream>

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

//----------------------------------------------------------------------
// Implementation
//----------------------------------------------------------------------

namespace internal
{

/** $FINROC_PROJECT_HOME path if it exists - otherwise "" */
std::string project_home_string;

/** Current working directory absolute path */
std::string cwd;

std::vector<std::string> GetPathsToCheck()
{
  std::vector<std::string> paths;
  const char* finroc_home = getenv("FINROC_HOME");
  const char* project_home = getenv("FINROC_PROJECT_HOME");
  char buffer[PATH_MAX];
  if (project_home != NULL)
  {
    project_home_string = std::string(project_home) + "/";
    paths.push_back(project_home_string);
  }
  if (finroc_home != NULL)
  {
    paths.push_back(std::string(finroc_home) + "/");
    paths.push_back(std::string(finroc_home) + "/sources/cpp/");
  }
  char *cwd_cstring = getcwd(buffer, sizeof(buffer));
  if (cwd_cstring == NULL)
  {
    FINROC_LOG_PRINT_STATIC(WARNING, "Could not look up the current working directory using getcwd(). Something seems to be wrong, but for now it just will not be included in the search path.");
  }
  else
  {
    cwd = std::string(cwd_cstring) + "/";
    paths.push_back(cwd);
  }
  return paths;
}

/*! Paths in which to search for finroc files */
static std::vector<std::string> paths_to_check = GetPathsToCheck();

} // namespace


bool FileExists(const std::string& filename, tFileType type)
{
  struct stat file_stat;
  if (stat(filename.c_str(), &file_stat))
  {
    return false;
  }
  switch (type)
  {
  case tFileType::ANY:
    return true;
  case tFileType::REGULAR:
    return S_ISREG(file_stat.st_mode);
  case tFileType::DIRECTORY:
    return S_ISDIR(file_stat.st_mode);
  case tFileType::CHARACTER_DEVICE:
    return S_ISCHR(file_stat.st_mode);
  case tFileType::BLOCK_DEVICE:
    return S_ISBLK(file_stat.st_mode);
  case tFileType::FIFO_PIPE:
    return S_ISFIFO(file_stat.st_mode);
  case tFileType::SOCKET:
    return S_ISSOCK(file_stat.st_mode);
  }
  return false;
}

bool FinrocFileExists(const std::string& raw_filename, tFileType type)
{
  return GetFinrocFile(raw_filename, type).length() > 0;
}

std::string GetFinrocFile(const std::string& raw_filename, tFileType type)
{
  if (raw_filename.length() == 0)
  {
    return "";
  }
  if (raw_filename[0] == '/')
  {
    return FileExists(raw_filename, type) ? raw_filename : std::string();
  }
  for (size_t i = 0; i < internal::paths_to_check.size(); i++)
  {
    std::string name = internal::paths_to_check[i] + raw_filename;
    if (FileExists(name, type))
    {
      return name;
    }
  }
  return "";
}

std::string GetFinrocFileToSaveTo(const std::string& raw_filename)
{
  if (raw_filename.length() == 0)
  {
    return "";
  }
  std::string file = GetFinrocFile(raw_filename, tFileType::REGULAR);
  if (file.length() > 0)
  {
    return file;
  }
  if (raw_filename.find('/') == std::string::npos) // does not contain "/"?
  {
    return (internal::project_home_string.length() > 0 ? (internal::project_home_string + raw_filename) : (internal::cwd + raw_filename));
  }
  std::string rawpath = raw_filename.substr(0, raw_filename.rfind('/'));
  if (raw_filename[0] == '/' && (rawpath.length() == 0 || FileExists(rawpath, tFileType::DIRECTORY)))
  {
    return raw_filename;
  }
  for (size_t i = 0; i < internal::paths_to_check.size(); i++)
  {
    std::string name = internal::paths_to_check[i] + rawpath;
    if (FileExists(name, tFileType::DIRECTORY))
    {
      return internal::paths_to_check[i] + raw_filename;
    }
  }

  return "";
}

#ifdef _LIB_RRLIB_XML_PRESENT_
rrlib::xml::tDocument GetFinrocXMLDocument(const std::string& raw_filename, bool validate)
{
  std::string file = GetFinrocFile(raw_filename, tFileType::REGULAR);
  if (file.length() == 0)
  {
    throw rrlib::xml::tException("File not found");
  }
  return rrlib::xml::tDocument(file, validate);
}
#endif

//----------------------------------------------------------------------
// End of namespace declaration
//----------------------------------------------------------------------
}
}
