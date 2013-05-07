//
// You received this file as part of Finroc
// A Framework for intelligent robot control
//
// Copyright (C) Finroc GbR (finroc.org)
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
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
#include <sys/stat.h> // for IsDirectory()

//----------------------------------------------------------------------
// Internal includes with ""
//----------------------------------------------------------------------

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
  char buffer[1024];
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
  cwd = std::string(getcwd(buffer, 1024)) + "/";
  paths.push_back(cwd);
  return paths;
}

bool IsDirectory(std::string directory_name)
{
  struct stat directory_stat;
  if (stat(directory_name.c_str(), &directory_stat))
  {
    return false;
  }
  return S_ISDIR(directory_stat.st_mode);
}

/*! Paths in which to search for finroc files */
static std::vector<std::string> paths_to_check = GetPathsToCheck();

} // namespace


bool FileExists(const std::string& filename)
{
  std::ifstream file_stream(filename);
  return file_stream.good();
}

bool FinrocFileExists(const std::string& raw_filename)
{
  return GetFinrocFile(raw_filename).length() > 0;
}

std::string GetFinrocFile(const std::string& raw_filename)
{
  if (raw_filename.length() == 0)
  {
    return "";
  }
  if (raw_filename[0] == '/')
  {
    return FileExists(raw_filename) ? raw_filename : std::string();
  }
  for (size_t i = 0; i < internal::paths_to_check.size(); i++)
  {
    std::string name = internal::paths_to_check[i] + raw_filename;
    if (FileExists(name))
    {
      return name;
    }
  }
  return "";
}

std::string GetFinrocFileToSaveTo(const std::string& raw_filename)
{
  std::string file = GetFinrocFile(raw_filename);
  if (file.length() > 0)
  {
    return file;
  }
  if (raw_filename.find('/') == std::string::npos) // does not contain "/"?
  {
    return (internal::project_home_string.length() > 0 ? (internal::project_home_string + raw_filename) : (internal::cwd + raw_filename));
  }
  std::string rawpath = raw_filename.substr(0, raw_filename.rfind('/'));
  for (size_t i = 0; i < internal::paths_to_check.size(); i++)
  {
    std::string name = internal::paths_to_check[i] + rawpath;
    if (FileExists(name) && internal::IsDirectory(name))
    {
      return internal::paths_to_check[i] + raw_filename;
    }
  }

  return "";
}

rrlib::xml::tDocument GetFinrocXMLDocument(const std::string& raw_filename, bool validate)
{
  std::string file = GetFinrocFile(raw_filename);
  if (file.length() == 0)
  {
    throw rrlib::xml::tException("File not found");
  }
  return rrlib::xml::tDocument(file, validate);
}

//----------------------------------------------------------------------
// End of namespace declaration
//----------------------------------------------------------------------
}
}
