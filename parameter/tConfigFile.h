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

#ifndef core__parameter__tConfigFile_h__
#define core__parameter__tConfigFile_h__

#include "rrlib/finroc_core_utils/definitions.h"

#include "rrlib/rtti/rtti.h"
#include "rrlib/xml2_wrapper/tXMLDocument.h"
#include "core/tFinrocAnnotation.h"

namespace rrlib
{
namespace xml2
{
class tXMLNode;
} // namespace rrlib
} // namespace xml2

namespace finroc
{
namespace core
{
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

  /*! Is config file active? (false when config file is deleted via finstruct) */
  bool active;

public:

  /*! Data Type */
  static rrlib::rtti::tDataTypeBase cTYPE;

  /*!
   * \param file File name of configuration file (loaded if it exists already)
   */
  tConfigFile(const util::tString& filename_);

  /*!
   * Create empty config file with no filename (should only be used to deserialize from stream shortly afterwards)
   */
  tConfigFile();

  virtual void Deserialize(rrlib::serialization::tInputStream& is);

  /*!
   * Find ConfigFile which specified element is configured from
   *
   * \param element Element
   * \return ConfigFile - or null if none could be found
   */
  static tConfigFile* Find(const tFrameworkElement* element);

  // TODO: reduce code duplication in hasEntry() and getEntry()

  /*!
   * Get entry from configuration file
   *
   * \param entry Entry
   * \param create (Re)create entry node?
   * \return XMLNode representing entry
   */
  rrlib::xml2::tXMLNode& GetEntry(const util::tString& entry, bool create = false);

  /*!
   * \return Filename of current config file
   */
  inline util::tString GetFilename() const
  {
    return filename;
  }

  /*!
   * Searches given entry in config file and returns its value as string if present.
   * \param entry the entry in the config file to be searched
   * \return string value of entry if present, empty string otherwise
   */
  util::tString GetStringEntry(const util::tString& entry);

  /*!
   * Does configuration file have the specified entry?
   *
   * \param entry Entry
   * \return Answer
   */
  bool HasEntry(const util::tString& entry);

  /*!
   * (Should only be used when Annotatable::getAnnotation() is called manually)
   *
   * \return Is config file active (does it "exist")?
   */
  inline bool IsActive() const
  {
    return active;
  }

  /*!
   * set parameters of all child nodes to current values in tree
   */
  void LoadParameterValues();

  /*!
   * set parameters of all framework element's child nodes to current values in tree
   *
   * \param fe Framework element
   */
  void LoadParameterValues(tFrameworkElement* fe);

  /*!
   * Saves configuration file back to HDD
   */
  void SaveFile();

  virtual void Serialize(rrlib::serialization::tOutputStream& os) const;

  void TreeFilterCallback(tFrameworkElement* fe, bool loading_parameters);

};

} // namespace finroc
} // namespace core

extern template class rrlib::rtti::tDataType<finroc::core::tConfigFile>;

#endif // core__parameter__tConfigFile_h__
