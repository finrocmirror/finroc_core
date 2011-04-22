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
/*!\file    tConveniencePort.h
 *
 * \author  Max Reichardt
 *
 * \date    2011-04-18
 *
 * \brief Contains tConveniencePort
 *
 * \b tConveniencePort
 *
 */
//----------------------------------------------------------------------
#ifndef _core__structure__tConveniencePort_h_
#define _core__structure__tConveniencePort_h_

//----------------------------------------------------------------------
// External includes (system with <>, local with "")
//----------------------------------------------------------------------

//----------------------------------------------------------------------
// Internal includes with ""
//----------------------------------------------------------------------
#include "core/structure/tStructureElementRegister.h"

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

/**
 * Base class for ports that are typically used inside modules by application developer.
 */
template <typename BASE>
class tConveniencePort
{
protected:

  /*! \return Parent module of parameter */
  BASE* FindParent()
  {
    return static_cast<BASE*>(tStructureElementRegister::FindParent(this));
  }

  /*! Get auto-generated port name */
  util::tString GetPortName()
  {
    BASE* parent = FindParent();
    util::tString result = tStructureElementRegister::GetAutoGeneratedPortName(parent, UpdateCurrentPortNameIndex());
    return result;
  }

  /*! Get & update current index for auto-generated port names */
  int UpdateCurrentPortNameIndex()
  {
    BASE* parent = FindParent();
    if (typeid(*parent).name() != parent->count_for_type) // detect class change when traversing module type hierarchy
    {
      parent->count_for_type = typeid(*parent).name();
      parent->auto_name_port_count = 0;
    }
    int current_index = parent->auto_name_port_count;
    parent->auto_name_port_count++;
    return current_index;
  }
};

//----------------------------------------------------------------------
// End of namespace declaration
//----------------------------------------------------------------------
}
}
}

#endif
