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
/*!\file    core/port/tPortConnectionConstraint.h
 *
 * \author  Max Reichardt
 *
 * \date    2012-09-04
 *
 * \brief   Contains tPortConnectionConstraint
 *
 * \b tPortConnectionConstraint
 *
 * This is the abstract base class for a single constraint (or rule) regarding connections among ports.
 * Such constraints can be added to ports in order to disallow certain kinds of connections
 * (e.g. in behavior networks).
 */
//----------------------------------------------------------------------
#ifndef __core__port__tPortConnectionConstraint_h__
#define __core__port__tPortConnectionConstraint_h__

//----------------------------------------------------------------------
// External includes (system with <>, local with "")
//----------------------------------------------------------------------

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
class tAbstractPort;

//----------------------------------------------------------------------
// Class declaration
//----------------------------------------------------------------------
//! Abstract constraint for port connections.
/*!
 * This is the abstract base class for a single constraint (or rule) regarding connections among ports.
 * Such constraints can be created globally in order to disallow certain kinds of connections.
 * (e.g. in behavior networks).
 *
 * The constructor will add the constraint to the global list of constraints.
 * The destructor will remove it.
 */
class tPortConnectionConstraint : boost::noncopyable
{

//----------------------------------------------------------------------
// Public methods and typedefs
//----------------------------------------------------------------------
public:

  tPortConnectionConstraint()
  {
    tAbstractPort::GetConnectionConstraintList().push_back(this);
  }

  virtual ~tPortConnectionConstraint()
  {
    try
    {
      auto& list = tAbstractPort::GetConnectionConstraintList();
      list.erase(std::remove(list.begin(), list.end(), this), list.end());
    }
    catch (std::logic_error &)
    {}
  }

  /*!
   * Called to inquire whether this constraint allows a connection between
   * the specified ports.
   *
   * \param source_port Start port of potential connection
   * \param destination_port Destination port of potential connection
   * \return True, if connection is allowed
   */
  virtual bool AllowPortConnection(const tAbstractPort& source_port, const tAbstractPort& destination_port) const = 0;

  /*!
   * \return Description of constraint (or rule). Will be displayed in error messages. May be several sentences long.
   */
  virtual const char* Description() const = 0;

  /*!
   * Called to inquire whether all constraints allow a connection between
   * the specified ports.
   *
   * \param source_port Start port of potential connection
   * \param destination_port Destination port of potential connection
   * \return A constraint disallowing this connection - NULL if there is no such constraint
   */
  static tPortConnectionConstraint* FindConstraintDisallowingConnection(tAbstractPort& source_port, tAbstractPort& destination_port);
};

//----------------------------------------------------------------------
// End of namespace declaration
//----------------------------------------------------------------------

}
}


#endif
