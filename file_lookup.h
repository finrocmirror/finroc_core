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
/*!\file    core/file_lookup.h
 *
 * \author  Max Reichardt
 *
 * \date    2013-05-07
 *
 * Functions to lookup/retrieve files by their name.
 * These functions check several locations ($FINROC_HOME, $FINROC_PROJECT_HOME,
 * Finroc system installation etc.)
 *
 */
//----------------------------------------------------------------------
#ifndef __core__file_lookup_h__
#define __core__file_lookup_h__

//----------------------------------------------------------------------
// External includes (system with <>, local with "")
//----------------------------------------------------------------------
#ifdef _LIB_RRLIB_XML_PRESENT_
#include "rrlib/xml/tDocument.h"
#endif

//----------------------------------------------------------------------
// Internal includes with ""
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
// Function declarations
//----------------------------------------------------------------------

/*!
 * Does a file with the specified name exist?
 *
 * \param filename File name
 * \return Answer
 */
bool FileExists(const std::string& filename);

/*!
 * Does file with specified name exist in finroc repository?
 *
 * \param raw_filename Raw file name
 * \return Answer (true, when GetFinrocFile(raw_filename).Length() > 0 - but possibly more efficient)
 */
bool FinrocFileExists(const std::string& raw_filename);

/*!
 * Lookup file in finroc repository.
 *
 * Searches in $FINROC_PROJECT_HOME, $FINROC_HOME, $FINROC_HOME/sources/cpp, current path, system installation (in this order).
 *
 * \param raw_filename Raw file name
 * \return Filename to open (can possibly be temp file somewhere). "" if no file was found.
 */
std::string GetFinrocFile(const std::string& raw_filename);

#ifdef _LIB_RRLIB_XML_PRESENT_
/*!
 * Open XML document in finroc repository.
 * (when dealing with archives, this can be more efficient than getFinrocFile which might create a temp file)
 *
 * Searches in $FINROC_PROJECT_HOME, $FINROC_HOME, $FINROC_HOME/sources/cpp, current path, system installation (in this order).
 *
 * \param raw_filename Raw file name.
 * \param validate Whether the validation should be processed or not
 * \return XML Document. Throws exception if file cannot be found.
 * \throws std::runtime_error If file cannot be founde
 */
rrlib::xml::tDocument GetFinrocXMLDocument(const std::string& raw_filename, bool validate);
#endif

/*!
 * Determine where to save file to.
 * If a suitable file already exists, it is returned (and typically overwritten).
 * Otherwise the most suitable location is returned (paths should alredy exist).
 * Locations are considered in this order: $FINROC_PROJECT_HOME, $FINROC_HOME, $FINROC_HOME/sources/cpp, current path.
 * If rawFilename has no path, either $FINROC_PROJECT_HOME is returned if set - otherwise the current path.
 *
 * \param raw_filename Raw file name
 * \return File to save to. "" if no location seems suitable
 */
std::string GetFinrocFileToSaveTo(const std::string& raw_filename);


//----------------------------------------------------------------------
// End of namespace declaration
//----------------------------------------------------------------------
}
}


#endif
