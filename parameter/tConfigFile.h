/**
 * You received this file as part of an advanced experimental
 * robotics framework prototype ('finroc')
 *
 * Copyright (C) 2010 Max Reichardt,
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
#include "rrlib/finroc_core_utils/tJCBase.h"

#ifndef CORE__PARAMETER__TCONFIGFILE_H
#define CORE__PARAMETER__TCONFIGFILE_H

#include "rrlib/xml2_wrapper/tXMLDocument.h"
#include "core/tFinrocAnnotation.h"
#include "rrlib/xml2_wrapper/tXMLNode.h"

namespace finroc
{
namespace core
{
class tDataType;
class tFrameworkElement;

/*!
 * \author Max Reichardt
 *
 * Configuration File. Is a tree of nodes with values as leafs
 */
class tConfigFile : public tFinrocAnnotation
{
private:

  /*! (Wrapped) XML document */
  rrlib::xml2::tXMLDocument wrapped;

  /*! File name of configuration file */
  util::tString filename;

  /*! Separator entries are divided with */
  static util::tString cSEPARATOR;

  /*! Branch name in XML */
  static util::tString cXML_BRANCH_NAME;

  /*! Leaf name in XML */
  static util::tString cXML_LEAF_NAME;

  /*! Temp buffer - only used in synchronized context */
  util::tStringBuilder temp_buffer;

public:

  /*! Data Type */
  static tDataType* cTYPE;

  /*! Log domain */
  RRLIB_LOG_CREATE_NAMED_DOMAIN(edge_log, "parameter");

private:

  /*!
   * \param file File name of configuration file (loaded if it exists already)
   */
  tConfigFile(const util::tString& filename_);

public:

  /*!
   * Find ConfigFile which specified element is configured from
   *
   * \param element Element
   * \return ConfigFile - or null if none could be found
   */
  inline static tConfigFile* Find(tFrameworkElement* element)
  {
    return static_cast<tConfigFile*>(FindParentWithAnnotation(element, cTYPE));
  }

  // TODO: reduce code duplication in hasEntry() and getEntry()

  /*!
   * Get entry from configuration file
   *
   * \param entry Entry
   * \param create (Re)create entry node?
   * \return XMLNode representing entry
   */
  rrlib::xml2::tXMLNode GetEntry(const util::tString& entry, bool create);

  /*!
   * Does configuration file have the specified entry?
   *
   * \param entry Entry
   * \return Answer
   */
  bool HasEntry(const util::tString& entry);

  /*!
   * set parameters of all child nodes to current values in tree
   */
  void LoadParameterValues();

  /*!
   * Saves configuration file back to HDD
   */
  void SaveFile();

  void TreeFilterCallback(tFrameworkElement* fe, bool loading_parameters);

};

} // namespace finroc
} // namespace core

#endif // CORE__PARAMETER__TCONFIGFILE_H
