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
#include "finroc_core_utils/tJCBase.h"

#ifndef CORE__FINSTRUCTABLE__TFINSTRUCTABLEGROUP_H
#define CORE__FINSTRUCTABLE__TFINSTRUCTABLEGROUP_H

#include "core/parameter/tStringStructureParameter.h"
#include "finroc_core_utils/container/tSimpleList.h"
#include "core/plugin/tStandardCreateModuleAction.h"
#include "core/tFrameworkElement.h"
#include "core/tFrameworkElementTreeFilter.h"

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
class tStructureParameterList;

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
class tFinstructableGroup : public tFrameworkElement, public tFrameworkElementTreeFilter::tCallback
{
private:

  /*! contains name of XML to use */
  tStringStructureParameter* xml_file;

  /*! contains name of XML that is currently used (variable is used to detect changes to xmlFile parameter) */
  util::tString current_xml_file;

  /*! Temporary variable for save operation: XML root node */
  rrlib::xml2::tXMLNode* root_tmp;

  /*! Temporary variable for save operation: List to store connected ports in */
  util::tSimpleList<tAbstractPort*> connect_tmp;

  /*! Temporary variable for save operation: Qualified link to this group */
  util::tString link_tmp;

  /*! CreateModuleAction */
  static tStandardCreateModuleAction<tFinstructableGroup> cCREATE_ACTION;

public:

  /*! Log domain for edges */
  RRLIB_LOG_CREATE_NAMED_DOMAIN(edge_log, "finstructable");

private:

  /*!
   * Deserialize parameter list
   *
   * \param node XML node containing parameters
   * \param params StructureParameterList to deserialize values of
   */
  void DeserializeParameterList(rrlib::xml2::tXMLNode& node, tStructureParameterList* params);

  /*!
   * \param cRelative port link
   * \return Port - or null if it couldn't be found
   */
  tAbstractPort* GetChildPort(const util::tString& link);

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
  void LogException(const rrlib::xml2::tXML2WrapperException& e);

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

public:

  tFinstructableGroup(const util::tString& name, tFrameworkElement* parent);

  /*!
   * (if the provided file does not exist, it is created, when contents are saved)
   * (if the provided file exists, its contents are loaded)
   *
   * \param xml_file name of XML file (relative to finroc repository) that determines contents of this group
   */
  tFinstructableGroup(const util::tString& name, tFrameworkElement* parent, const util::tString& xml_file_);

  /*!
   * Save contents of group back to Xml file
   */
  void SaveXml();

  virtual void StructureParametersChanged();

  void TreeFilterCallback(tFrameworkElement* fe);

};

} // namespace finroc
} // namespace core

#endif // CORE__FINSTRUCTABLE__TFINSTRUCTABLEGROUP_H
