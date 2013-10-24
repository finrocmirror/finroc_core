//
// You received this file as part of Finroc
// A framework for intelligent robot control
//
// Copyright (C) Finroc GbR (finroc.org)
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along
// with this program; if not, write to the Free Software Foundation, Inc.,
// 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
//
//----------------------------------------------------------------------
/*!\file    core/port/tPortGroup.h
 *
 * \author  Max Reichardt
 *
 * \date    2012-12-02
 *
 * \brief   Contains tPortGroup
 *
 * \b tPortGroup
 *
 * Group of ports.
 *
 * Sensor Inputs, Controller Inputs etc. of a module are such groups.
 *
 * They can be used to conveniently connect whole groups of ports at once -
 * for instance, by-name and possibly even create ports in target group
 * if they do not exist yet.
 *
 * All convenience functions to connect groups of ports should be added
 * to this class.
 */
//----------------------------------------------------------------------
#ifndef __core__port__tPortGroup_h__
#define __core__port__tPortGroup_h__

//----------------------------------------------------------------------
// External includes (system with <>, local with "")
//----------------------------------------------------------------------

//----------------------------------------------------------------------
// Internal includes with ""
//----------------------------------------------------------------------
#include "core/port/tEdgeAggregator.h"
#include "core/port/tPortWrapperBase.h"

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
//! Group of ports.
/*!
 * Group of ports.
 *
 * Sensor Inputs, Controller Inputs etc. of a module are such groups.
 *
 * They can be used to conveniently connect whole groups of ports at once -
 * for instance, by-name and possibly even create ports in target group
 * if they do not exist yet.
 *
 * All convenience functions to connect groups of ports should be added
 * to this class.
 */
class tPortGroup : public tEdgeAggregator
{

//----------------------------------------------------------------------
// Public methods and typedefs
//----------------------------------------------------------------------
public:

  /*!
   * \param default_port_flags Default flags for any ports to be created in this Group
   * (see FrameworkElement for other parameter description)
   */
  tPortGroup(tFrameworkElement* parent, const std::string& name, tFlags flags, tFlags default_port_flags);

  /*!
   * Implementation of several Connect... functions (to avoid major code duplication)
   *
   * \param other_group Partner port group
   * \param group_link Partner port group link (if port group is null)
   * \param create_missing_ports Create ports in source, if this group has ports with names that cannot be found in source.
   * \param start_with Port to start connecting with (NULL = first port)
   * \param count Number of ports to connect - starting with start port (-1 = all ports)
   * \param port_prefix Prefix of ports in this group. Prefix is cut off when comparing names. Ports without this prefix are skipped.
   * \param other_port_prefix Prefix of ports in source group to ignore. This is prepended when ports are created.
   */
  void ConnectImpl(tPortGroup* other_group, const std::string& group_link, bool create_missing_ports, tAbstractPort* start_with = NULL, int count = -1, const std::string& port_prefix = "", const std::string& other_port_prefix = "");

  /*!
   * Connects all ports to ports with the same name in other port group.
   *
   * \param other_group Other port group
   * \param create_missing_ports Create ports in source, if this group has ports with names that cannot be found in source.
   * \param start_with Port to start connecting with (NULL = first port)
   * \param count Number of ports to connect - starting with start port (-1 = all ports)
   * \param port_prefix Prefix of ports in this group. Prefix is cut off when comparing names. Ports without this prefix are skipped.
   * \param source_port_prefix Prefix of ports in source group to ignore. This is prepended when ports are created.
   */
  void ConnectByName(tPortGroup& other_group, bool create_missing_ports, tAbstractPort* start_with = NULL, int count = -1, const std::string& port_prefix = "", const std::string& source_port_prefix = "")
  {
    ConnectImpl(&other_group, "", create_missing_ports, start_with, count, port_prefix, source_port_prefix);
  }
  void ConnectByName(tPortGroup& other_group, bool create_missing_ports, tPortWrapperBase start_with, int count = -1, const std::string& port_prefix = "", const std::string& source_port_prefix = "")
  {
    ConnectByName(other_group, create_missing_ports, start_with.GetWrapped(), count, port_prefix, source_port_prefix);
  }

  /*!
   * Connects/links all ports to ports with the same name in other port group.
   * (connections are (re)established when links are available)
   *
   * \param group_link Link name of other port group (relative to this port group)
   * \param start_with Port to start connecting with (NULL = first port)
   * \param count Number of ports to connect - starting with start port (-1 = all ports)
   * \param port_prefix Prefix of ports in this group. Prefix is cut off when comparing names. Ports without this prefix are skipped.
   * \param source_port_prefix Prefix of ports in source group to ignore. This is prepended when ports are created.
   */
  void ConnectByName(const std::string& group_link, tAbstractPort* start_with = NULL, int count = -1, const std::string& port_prefix = "", const std::string& source_port_prefix = "")
  {
    ConnectImpl(NULL, group_link, false, start_with, count, port_prefix, source_port_prefix);
  }
  void ConnectByName(const std::string& group_link, tPortWrapperBase start_with, int count = -1, const std::string& port_prefix = "", const std::string& source_port_prefix = "")
  {
    ConnectByName(group_link, start_with.GetWrapped(), count, port_prefix, source_port_prefix);
  }

  /*!
   * Create port in this group
   *
   * \param name Name of port
   * \param type Data type of port
   * \param extra_flags Any extra flags for port
   * \return Created port
   */
  tAbstractPort* CreatePort(const std::string& name, rrlib::rtti::tType type, tFlags extra_flags = tFlags());

  /*!
   * Disconnect all of port group's ports
   *
   * \param incoming Disconnect incoming edges?
   * \param outgoing Disconnect outgoing edges?
   * \param start_with Port to start disconnecting with (NULL = first port)
   * \param count Number of ports to disconnect - starting with start port (-1 = all ports)
   */
  void DisconnectAll(bool incoming = true, bool outgoing = true, tAbstractPort* start_with = NULL, int count = -1);
  void DisconnectAll(bool incoming, bool outgoing, tPortWrapperBase start_with, int count = -1)
  {
    DisconnectAll(incoming, outgoing, start_with.GetWrapped());
  }

  /*!
   * \return Default flags for any ports to be created in this Group
   */
  tFlags GetDefaultPortFlags()
  {
    return default_port_flags;
  }

  /*!
   * \param index of port in this group (throws util::tException if index is out of bounds; use ChildCount() to check for size)
   * \return nth Port in this port group
   */
  tAbstractPort& operator[](size_t index);

//----------------------------------------------------------------------
// Private fields and methods
//----------------------------------------------------------------------
private:

  /*! Default flags for any ports to be created in this Group */
  tFlags default_port_flags;

};

//----------------------------------------------------------------------
// End of namespace declaration
//----------------------------------------------------------------------
}
}


#endif
