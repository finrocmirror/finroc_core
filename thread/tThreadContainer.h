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

#ifndef core__thread__tThreadContainer_h__
#define core__thread__tThreadContainer_h__

#include "rrlib/finroc_core_utils/definitions.h"

#include "core/thread/tThreadContainerElement.h"
#include "core/finstructable/tFinstructableGroup.h"

namespace finroc
{
namespace core
{
/*!
 * \author Max Reichardt
 *
 * Contains thread that executes OrderedPeriodicTasks of all children.
 * Execution in performed in the order of the graph.
 */
class tThreadContainer : public tThreadContainerElement<tGroup>
{
private:
  /*! CreateModuleAction */
  static tStandardCreateModuleAction<tThreadContainer> cCREATE_ACTION;

public:

  /*!
   * \param description Name
   * \param parent parent
   */
  tThreadContainer(tFrameworkElement* parent, const util::tString& description) :
      tThreadContainerElement<tGroup>(parent, description)
  {}
};

/*!
 * \author Max Reichardt
 *
 * Contains thread that executes OrderedPeriodicTasks of all children.
 * Execution in performed in the order of the graph.
 */
class tFinstructableThreadContainer : public tThreadContainerElement<tFinstructableGroup>
{
private:
  /*! CreateModuleAction */
  static tStandardCreateModuleAction<tFinstructableThreadContainer> cCREATE_ACTION;

  /*! Log domain for this class */
  RRLIB_LOG_CREATE_NAMED_DOMAIN(log_domain, "framework_elements");

public:

  /*!
   * \param description Name
   * \param parent parent
   */
  tFinstructableThreadContainer(tFrameworkElement* parent, const util::tString& description) :
      tThreadContainerElement<tFinstructableGroup>(parent, description)
  {}

  /*!
   * (if the provided file does not exist, it is created, when contents are saved - and a warning is displayed)
   * (if the provided file exists, its contents are loaded when group is initialized)
   *
   * \param description Name
   * \param parent parent
   * \param xml_file name of XML file (relative to finroc repository) that determines contents of this group
   */
  tFinstructableThreadContainer(tFrameworkElement* parent, const util::tString& description, const util::tString& xml_file) :
      tThreadContainerElement<tFinstructableGroup>(parent, description)
  {
    try
    {
      this->xml_file.Set(xml_file);
    }
    catch (const util::tException& e)
    {
      FINROC_LOG_PRINT(rrlib::logging::eLL_ERROR, log_domain, e);
    }
  }
};

extern template class tThreadContainerElement<tGroup>;
extern template class tThreadContainerElement<tFinstructableGroup>;

} // namespace finroc
} // namespace core

#endif // core__thread__tThreadContainer_h__
