//
// You received this file as part of Finroc
// A Framework for intelligent robot control
//
// Copyright (C) Finroc GbR (finroc.org)
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
/*!\file    core/port/tAbstractPortCreationInfo.h
 *
 * \author  Max Reichardt
 *
 * \date    2012-10-28
 *
 * \brief   Contains tAbstractPortCreationInfo
 *
 * \b tAbstractPortCreationInfo
 *
 * This class bundles various parameters for the creation of ports.
 *
 * Instead of providing suitable constructors for all types of sensible
 * combinations of the numerous (often optional) construction parameters,
 * there is only one constructor taking a single argument of this class.
 */
//----------------------------------------------------------------------
#ifndef __core__port__tAbstractPortCreationInfo_h__
#define __core__port__tAbstractPortCreationInfo_h__

//----------------------------------------------------------------------
// External includes (system with <>, local with "")
//----------------------------------------------------------------------
#include "rrlib/rtti/rtti.h"
#include "core/tFrameworkElement.h"

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
// Class declaration
//----------------------------------------------------------------------
//! Bundle of port creation parameters
/*!
 * This class bundles various parameters for the creation of ports.
 *
 * Instead of providing suitable constructors for all types of sensible
 * combinations of the numerous (often optional) construction parameters,
 * there is only one port constructor utilizing this class as well as derived
 * classes to process the provided arguments.
 *
 * This is a struct, because this is merely a collection of parameters
 * (more or less).
 */
struct tAbstractPortCreationInfo
{

  /*! Port flags */
  core::tFrameworkElement::tFlags flags;

  /*! Data type of port */
  rrlib::rtti::tType data_type;

  /*! Parent of port */
  core::tFrameworkElement* parent;

  /*! Port name */
  tString name;


  tAbstractPortCreationInfo() :
    flags(),
    data_type(),
    parent(NULL),
    name()
  {}


  /*! Various Set methods for different port properties */
  void Set(core::tFrameworkElement* parent)
  {
    this->parent = parent;
  }

  void Set(const tString& name)
  {
    this->name = name;
  }

  void Set(core::tFrameworkElement::tFlags flags)
  {
    this->flags |= flags;
  }

  void Set(const rrlib::rtti::tType& type)
  {
    this->data_type = type;
  }

  void Set(const tAbstractPortCreationInfo& creation_info)
  {
    *this = creation_info;
  }

  /*!
   * Catches all invalid constructor arguments that were not handled by this
   * or any derived class
   */
  struct tBase
  {
    static void Set(...) {}
  };
};

//----------------------------------------------------------------------
// End of namespace declaration
//----------------------------------------------------------------------
}
}


#endif
