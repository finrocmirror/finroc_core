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

#ifndef core__parameter__tConfigNode_h__
#define core__parameter__tConfigNode_h__

#include "rrlib/finroc_core_utils/definitions.h"

#include "rrlib/rtti/rtti.h"
#include "core/tFinrocAnnotation.h"

namespace finroc
{
namespace core
{
class tFrameworkElement;

/*!
 * \author Max Reichardt
 *
 * Using this annotation, a common parent config file node for all a module's/group's
 * parameter config entries can be stored.
 */
class tConfigNode : public tFinrocAnnotation
{
public:

  /*! Data Type */
  static rrlib::rtti::tDataTypeBase cTYPE;

  /*! Config file entry for node (starting with '/' => absolute link - otherwise relative) */
  util::tString node;

  tConfigNode(const util::tString& node = "");

  /*!
   * Get config file node to use for the specified framework element.
   * It searches in parent framework elements for any entries to
   * determine which one to use.
   *
   * \param fe Framework element
   */
  static util::tString GetConfigNode(tFrameworkElement* fe);

  /*!
   * Get full config entry for specified parent - taking any common config file node
   * stored in parents into account
   *
   * \param parent Parent framework element
   * \param config_entry Config entry (possibly relative to parent config file node - if not starting with '/')
   * \return Config entry to use
   */
  static util::tString GetFullConfigEntry(tFrameworkElement* parent, const util::tString& config_entry);

};

} // namespace finroc
} // namespace core

extern template class rrlib::rtti::tDataType<finroc::core::tConfigNode>;

#endif // core__parameter__tConfigNode_h__
