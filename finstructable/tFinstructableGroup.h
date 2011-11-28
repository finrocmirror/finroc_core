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

#ifndef core__finstructable__tFinstructableGroup_h__
#define core__finstructable__tFinstructableGroup_h__

#include "rrlib/finroc_core_utils/definitions.h"

#include "rrlib/finroc_core_utils/container/tSimpleList.h"
#include "core/plugin/tStandardCreateModuleAction.h"
#include "core/tFrameworkElement.h"
#include "core/parameter/tStaticParameter.h"

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
class tAbstractPort;

/*!
 * \author Max Reichardt
 *
 * The contents of FinstructableGroups can be edited using Finstruct.
 *
 * They get an XML file and optionally an attribute tree in the constructor.
 * The contents of the group are determined entirely by the contents of the
 * XML file.
 * Changes made using finstruct can be saved back to these files.
 */
class tFinstructableGroup : public tFrameworkElement
{
public:

  /*! contains name of XML to use */
  tStaticParameter<util::tString> xml_file;

private:

  /*! Temporary variable for save operation: List to store connected ports in */
  util::tSimpleList<tAbstractPort*> connect_tmp;

  /*! Temporary variable for save operation: Qualified link to this group */
  util::tString link_tmp;

  /*! Temporary variable for save operation: Save parameter config entries in callback (instead of edges)? */
  bool save_parameter_config_entries;

  /*! Default name when group is main part */
  util::tString main_name;

  /*! CreateModuleAction */
  static tStandardCreateModuleAction<tFinstructableGroup> cCREATE_ACTION;

public:

  /*! Log domain for edges */
  RRLIB_LOG_CREATE_NAMED_DOMAIN(edge_log, "finstructable");

private:

  /*!
   * \param cRelative port link
   * \return Port - or null if it couldn't be found
   */
  tAbstractPort* GetChildPort(const util::tString& link);

  /*!
   * Intantiate element
   *
   * \param node xml node that contains data for instantiation
   * \param parent Parent element
   */
  void Instantiate(const rrlib::xml2::tXMLNode& node, tFrameworkElement* parent);

  /*!
   * Loads and instantiates contents of xml file
   *
   * \param xml_file xml file to load
   */
  void LoadXml(const util::tString& xml_file_);

  /*!
   * Log exception (convenience method)
   *
   * \param e Exception
   */
  void LogException(const std::exception& e);

  /*!
   * Make fully-qualified link from relative one
   *
   * \param link Relative Link
   * \return Fully-qualified link
   */
  util::tString QualifyLink(const util::tString& link);

  /*!
   * Serialize children of specified framework element
   *
   * \param node XML node to serialize to
   * \param current Framework element
   */
  void SerializeChildren(rrlib::xml2::tXMLNode& node, tFrameworkElement* current);

  /*!
   * Recursive helper function for ScanForCommandLineArgs
   *
   * \param result Result list
   * \param parent Node to scan childs of
   */
  static void ScanForCommandLineArgsHelper(std::vector<util::tString>& result, const rrlib::xml2::tXMLNode& parent);

protected:

  /*!
   * \param link (as from link edge)
   * \return Relative link to this port (or absolute link if it is globally unique)
   */
  util::tString GetEdgeLink(const util::tString& target_link);

  /*!
   * \param ap Port
   * \return Relative link to this port (or absolute link if it is globally unique)
   */
  util::tString GetEdgeLink(tAbstractPort* ap);

public:

  tFinstructableGroup(tFrameworkElement* parent, const util::tString& name, uint flags = 0);

  /*!
   * (if the provided file does not exist, it is created, when contents are saved - and a warning is displayed)
   * (if the provided file exists, its contents are loaded when group is initialized)
   *
   * \param xml_file name of XML file (relative to finroc repository) that determines contents of this group
   */
  tFinstructableGroup(tFrameworkElement* parent, const util::tString& name, const util::tString& xml_file_, uint flags = 0);

  /*!
   * Helper method to collect .so files that need to be loaded before the contents of
   * this XML file can be instantiated.
   * (only has an effect if the current thread is currently saving this group to a file)
   *
   * \param dependency .so file that needs to be loaded
   */
  static void AddDependency(const util::tString& dependency);

  /*!
   * Helper method to collect data types that need to be loaded before the contents of
   * this XML file can be instantiated.
   * (only has an effect if the current thread is currently saving this group to a file)
   *
   * \param dt Data type required to instantiate this .xml
   */
  static void AddDependency(const rrlib::serialization::tDataTypeBase& dt);

  virtual void EvaluateStaticParameters();

  /*!
   * Is this finstructable group the one responsible for saving parameter's config entry?
   *
   * \param ap Framework element to check this for (usually parameter port)
   * \return Answer.
   */
  bool IsResponsibleForConfigFileConnections(tFrameworkElement* ap) const;

  /*!
   * Save contents of group back to Xml file
   */
  void SaveXml();

  /*!
   * Scan for command line arguments in specified .finroc xml file.
   * (for finroc executable)
   *
   * \param finroc_file File to scan in.
   * \return List of command line arguments.
   */
  static std::vector<util::tString> ScanForCommandLineArgs(const util::tString& finroc_file);

  /*!
   * \param main_name Default name when group is main part
   */
  void SetMainName(const util::tString main_name)
  {
    this->main_name = main_name;
  }

  /*!
   * Perform some static initialization w.r.t. to state at program startup
   */
  static void StaticInit();

  void TreeFilterCallback(tFrameworkElement* fe, rrlib::xml2::tXMLNode* root);

};

} // namespace finroc
} // namespace core

#endif // core__finstructable__tFinstructableGroup_h__
