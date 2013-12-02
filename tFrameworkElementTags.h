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
/*!\file    core/tFrameworkElementTags.h
 *
 * \author  Max Reichardt
 *
 * \date    2012-08-20
 *
 * \brief   Contains tFrameworkElementTags
 *
 * \b tFrameworkElementTags
 *
 * This annotation can be used to assign arbitrary classification tags (strings) to framework elements.
 * These tags are mainly used for optimized visualization/representation in finstruct.
 * Common tags include "module", "group" and "behavior".
 */
//----------------------------------------------------------------------
#ifndef __core__tFrameworkElementTags_h__
#define __core__tFrameworkElementTags_h__

//----------------------------------------------------------------------
// External includes (system with <>, local with "")
//----------------------------------------------------------------------
#include "rrlib/rtti/rtti.h"

//----------------------------------------------------------------------
// Internal includes with ""
//----------------------------------------------------------------------
#include "core/tAnnotation.h"

//----------------------------------------------------------------------
// Namespace declaration
//----------------------------------------------------------------------
namespace finroc
{
namespace core
{
class tFrameworkElement;

//----------------------------------------------------------------------
// Forward declarations / typedefs / enums
//----------------------------------------------------------------------

//----------------------------------------------------------------------
// Class declaration
//----------------------------------------------------------------------
//! Annotation for framework elements containing classification tags.
/*!
 * This annotation can be used to assign arbitrary classification tags (strings) to framework elements.
 * These tags are mainly used for optimized visualization/representation in finstruct.
 *
 * Libraries and plugins that define their own tags, should prepend their namespace in order to avoid
 * name collisions (e.g. "ib2c" or "ib2c.whatever" or "ib2c_whatever")
 *
 * There are some common global tags:
 * - "module"  Displayed and handled as component in finstruct
 * - "group"   Displayed and handled as composite component in finstruct
 * - "initially show in tools:<priority>"  This will cause framework element to be initially shown/opened in tools such as
 *                                         finstruct and fingui. Use tFrameworkElement::InitiallyShowInTools() to set this
 * - "hidden in tools"  Marks element that should not be hidden in tools by default, because they are not of interest to
 *                      application developers (e.g. TCP helper elements)
 * - "finstructable structure file: <file>"  Provides name of structure file for any finstructable groups
 */
class tFrameworkElementTags : public tAnnotation
{

//----------------------------------------------------------------------
// Public methods and typedefs
//----------------------------------------------------------------------
public:

  tFrameworkElementTags();

  /*!
   * Adds tag to framework element.
   * If framework element already has this tag, function call has no effect.
   *
   * \param fe Framework element to add tag to
   * \param tag Tag to add to framework element
   */
  static void AddTag(tFrameworkElement& fe, const std::string& tag);

  /*!
   * Adds tags to framework element.
   * Any tags that framework element is already tagged with, are ignored.
   *
   * \param fe Framework element to add tagd to
   * \param tag Tags to add to framework element
   */
  static void AddTags(tFrameworkElement& fe, const std::vector<std::string>& tags);

  /*!
   * \param fe Framework element to check
   * \param tag Tag to check
   *
   * \return True if framework element is tagged with the specified tag
   */
  static bool IsTagged(const tFrameworkElement& fe, const std::string& tag);


  /*! "hidden in tools" - Tag that marks element that should not be hidden in tools by default */
  static const char* cHIDDEN_IN_TOOLS;

//----------------------------------------------------------------------
// Private fields and methods
//----------------------------------------------------------------------
private:

  friend rrlib::serialization::tOutputStream& operator << (rrlib::serialization::tOutputStream& stream, const tFrameworkElementTags& tags);
  friend rrlib::serialization::tInputStream& operator >> (rrlib::serialization::tInputStream& stream, tFrameworkElementTags& tags);

  /*! Classification tags (strings) assigned to framework element */
  std::vector<std::string> tags;

};

rrlib::serialization::tOutputStream& operator << (rrlib::serialization::tOutputStream& stream, const tFrameworkElementTags& tags);

rrlib::serialization::tInputStream& operator >> (rrlib::serialization::tInputStream& stream, tFrameworkElementTags& tags);

//----------------------------------------------------------------------
// End of namespace declaration
//----------------------------------------------------------------------
}
}


#endif
