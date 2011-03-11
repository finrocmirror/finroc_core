//
// You received this file as part of Finroc
// A framework for innovative robot control
//
// Copyright (C) AG Robotersysteme TU Kaiserslautern
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
/*!\file    tGroup.h
 *
 * \author  Tobias Foehst
 * \author  Bernd-Helge Schaefer
 *
 * \date    2010-12-09
 *
 * \brief Contains tGroup
 *
 * \b tGroup
 *
 */
//----------------------------------------------------------------------
#ifndef _core__tGroup_h_
#define _core__tGroup_h_

#include "core/finstructable/tFinstructableGroup.h"

//----------------------------------------------------------------------
// External includes (system with <>, local with "")
//----------------------------------------------------------------------

//----------------------------------------------------------------------
// Internal includes with ""
//----------------------------------------------------------------------
#include "core/port/tEdgeAggregator.h"
#include "core/port/tPort.h"
#include "core/plugin/tStandardCreateModuleAction.h"

//----------------------------------------------------------------------
// Debugging
//----------------------------------------------------------------------

//----------------------------------------------------------------------
// Namespace declaration
//----------------------------------------------------------------------
namespace finroc
{
namespace core
{
namespace structure
{

//----------------------------------------------------------------------
// Forward declarations / typedefs / enums
//----------------------------------------------------------------------

//----------------------------------------------------------------------
// Class declaration
//----------------------------------------------------------------------
//!
/*!
 *
 */
class tGroup : public tFinstructableGroup
{
  finroc::core::tEdgeAggregator* controller_input;
  finroc::core::tEdgeAggregator* controller_output;

  finroc::core::tEdgeAggregator* sensor_input;
  finroc::core::tEdgeAggregator* sensor_output;

//----------------------------------------------------------------------
// Public methods
//----------------------------------------------------------------------
public:

  template < typename T = double >
  struct tCI : public tPort<T>
  {
    tCI(tGroup *parent, const finroc::util::tString &name)
        : tPort<T>(tPortCreationInfo(name, parent->controller_input, tPortFlags::cINPUT_PROXY))
    {}
  };
  template < typename T = double >
  struct tCO : public tPort<T>
  {
    tCO(tGroup *parent, const finroc::util::tString &name)
        : tPort<T>(tPortCreationInfo(name, parent->controller_output, tPortFlags::cOUTPUT_PROXY))
    {}
  };
  template < typename T = double >
  struct tSI : public tPort<T>
  {
    tSI(tGroup *parent, const finroc::util::tString &name)
        : tPort<T>(tPortCreationInfo(name, parent->sensor_input, tPortFlags::cINPUT_PROXY))
    {}
  };
  template < typename T = double >
  struct tSO : public tPort<T>
  {
    tSO(tGroup *parent, const finroc::util::tString &name)
        : tPort<T>(tPortCreationInfo(name, parent->sensor_output, tPortFlags::cOUTPUT_PROXY))
    {}
  };

  tGroup(finroc::core::tFrameworkElement *parent, const finroc::util::tString &name, const finroc::util::tString &structure_config_file);

//  void SetParameter(size_t index, const finroc::util::tString &new_value);
//  void SetParameter(const finroc::util::tString &name, const finroc::util::tString &new_value);

};

//----------------------------------------------------------------------
// End of namespace declaration
//----------------------------------------------------------------------
}
}
}

#endif
