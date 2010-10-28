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

#ifndef CORE__FINSTRUCTABLE__TGROUPINTERFACE_H
#define CORE__FINSTRUCTABLE__TGROUPINTERFACE_H

#include "core/plugin/tConstructorCreateModuleAction.h"
#include "core/parameter/tStructureParameter.h"
#include "core/datatype/tPortCreationList.h"
#include "core/plugin/tStandardCreateModuleAction.h"
#include "core/port/tEdgeAggregator.h"

namespace finroc
{
namespace core
{
/*!
 * \author Max Reichardt
 *
 * Group Interface Port Vector
 */
class tGroupInterface : public tEdgeAggregator
{
public:

public:

  enum tDataClassification { eSENSOR_DATA, eCONTROLLER_DATA, eANY };

  enum tPortDirection { eINPUT_ONLY, eOUTPUT_ONLY, eBOTH };

private:

  /*! List of ports */
  tStructureParameter<tPortCreationList>* ports;

  /*! CreateModuleAction */
  static tStandardCreateModuleAction<tGroupInterface> cCREATE_ACTION;

  /*! CreateModuleAction */
  static tConstructorCreateModuleAction<tGroupInterface, tGroupInterface::tDataClassification, tGroupInterface::tPortDirection, bool, bool> cCOMPLEX_CREATE_ACTION;

  /*!
   * Compute port flags
   *
   * \param data_class Classifies data in this interface
   * \param port_dir Which types of ports can be created in this interface?
   * \param shared Shared interface/ports?
   * \param unique_link Do ports habe globally unique link
   * \return flags for these parameters
   */
  static int ComputePortFlags(tGroupInterface::tDataClassification data_class, tGroupInterface::tPortDirection port_dir, bool shared, bool unique_link);

public:

  /*!
   * Default constructor
   *
   * \param description Interface description
   * \param parent Parent element
   */
  tGroupInterface(const util::tString& description, tFrameworkElement* parent);

  /*!
   * Advanced constructor
   *
   * \param description Interface description
   * \param parent Parent element
   * \param data_class Classifies data in this interface
   * \param port_dir Which types of ports can be created in this interface?
   * \param shared Shared interface/ports?
   * \param unique_link Do ports habe globally unique link
   * \return flags for these parameters
   */
  tGroupInterface(const util::tString& description, tFrameworkElement* parent, tGroupInterface::tDataClassification data_class, tGroupInterface::tPortDirection port_dir, bool shared, bool unique_link);

};

} // namespace finroc
} // namespace core

#endif // CORE__FINSTRUCTABLE__TGROUPINTERFACE_H
