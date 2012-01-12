/**
 * You received this file as part of an advanced experimental
 * robotics framework prototype ('finroc')
 *
 * Copyright (C) 2011 Max Reichardt,
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

#ifndef core__port__tPortGroup_h__
#define core__port__tPortGroup_h__

#include "rrlib/finroc_core_utils/definitions.h"

#include "core/port/tEdgeAggregator.h"
#include "core/port/tPortWrapperBase.h"

namespace finroc
{
namespace core
{

/*!
 * \author Max Reichardt
 *
 * Group of ports.
 *
 * Sensor Inputs, Contoller Inputs etc. of a module are such groups.
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
private:

  /*! Default flags for any ports to be created in this Group */
  const uint default_port_flags;

public:

  /*!
   * \param default_port_flags Default flags for any ports to be created in this Group
   * (see FrameworkElement for other parameter description)
   */
  tPortGroup(tFrameworkElement* parent, const util::tString& name, uint flags, int default_port_flags);

  /*!
   * Implementation of several Connect... functions (to avoid major code duplication)
   *
   * \param op Internal Opcode
   * \param group Partner port group
   * \param group_link Partner port group link (if port group is null)
   * \param create_missing_ports Create ports in source, if this group has ports with names that cannot be found in source.
   * \param start_with Port to start connecting with (NULL = first port)
   * \param count Number of ports to connect - starting with start port (-1 = all ports)
   * \param port_prefix Prefix of ports in this group. Prefix is cut off when comparing names. Ports without this prefix are skipped.
   * \param other_port_prefix Prefix of ports in source group to ignore. This is prepended when ports are created.
   */
  void ConnectImpl(int op, tPortGroup* group, const util::tString& group_link, bool create_missing_ports, tAbstractPort* start_with = NULL, int count = -1, const util::tString& port_prefix = "", const util::tString& other_port_prefix = "");

  /*!
   * Connects all ports to ports with the same name in source port group.
   *
   * \param source source port group
   * \param create_missing_ports Create ports in source, if this group has ports with names that cannot be found in source.
   * \param start_with Port to start connecting with (NULL = first port)
   * \param count Number of ports to connect - starting with start port (-1 = all ports)
   * \param port_prefix Prefix of ports in this group. Prefix is cut off when comparing names. Ports without this prefix are skipped.
   * \param source_port_prefix Prefix of ports in source group to ignore. This is prepended when ports are created.
   */
  void ConnectToSourceByName(tPortGroup& source, bool create_missing_ports, tAbstractPort* start_with = NULL, int count = -1, const util::tString& port_prefix = "", const util::tString& source_port_prefix = "")
  {
    ConnectImpl(0, &source, "", create_missing_ports, start_with, count, port_prefix, source_port_prefix);
  }
  template <typename T>
  void ConnectToSourceByName(tPortGroup& source, bool create_missing_ports, tPortWrapperBase<T> start_with, int count = -1, const util::tString& port_prefix = "", const util::tString& source_port_prefix = "")
  {
    ConnectToSourceByName(source, create_missing_ports, start_with.GetWrapped(), count, port_prefix, source_port_prefix);
  }

  /*!
   * Connects/links all ports to ports with the same name in source port group.
   * (connection is (re)established when link is available)
   *
   * \param source_link Link name of source port group (relative to this port group)
   * \param start_with Port to start connecting with (NULL = first port)
   * \param count Number of ports to connect - starting with start port (-1 = all ports)
   * \param port_prefix Prefix of ports in this group. Prefix is cut off when comparing names. Ports without this prefix are skipped.
   * \param source_port_prefix Prefix of ports in source group to ignore. This is prepended when ports are created.
   */
  void ConnectToSourceByName(const util::tString& source_link, tAbstractPort* start_with = NULL, int count = -1, const util::tString& port_prefix = "", const util::tString& source_port_prefix = "")
  {
    ConnectImpl(2, NULL, source_link, false, start_with, count, port_prefix, source_port_prefix);
  }
  template <typename T>
  void ConnectToSourceByName(const util::tString& source_link, tPortWrapperBase<T> start_with, int count = -1, const util::tString& port_prefix = "", const util::tString& source_port_prefix = "")
  {
    ConnectToSourceByName(source_link, start_with.GetWrapped(), count, port_prefix, source_port_prefix);
  }

  /*!
   * Connects all ports to ports with the same name in target port group.
   *
   * \param target target port group
   * \param create_missing_ports Create ports in target, if this group has ports with names that cannot be found in target.
   * \param start_with Port to start connecting with (NULL = first port)
   * \param count Number of ports to connect - starting with start port (-1 = all ports)
   * \param port_prefix Prefix of ports in this group. Prefix is cut off when comparing names. Ports without this prefix are skipped.
   * \param target_port_prefix Prefix of ports in target group to ignore. This is prepended when ports are created.
   */
  void ConnectToTargetByName(tPortGroup& target, bool create_missing_ports, tAbstractPort* start_with = NULL, int count = -1, const util::tString& port_prefix = "", const util::tString& target_port_prefix = "")
  {
    ConnectImpl(1, &target, "", create_missing_ports, start_with, count, port_prefix, target_port_prefix);
  }
  template <typename T>
  void ConnectToTargetByName(tPortGroup& target, bool create_missing_ports, tPortWrapperBase<T> start_with, int count = -1, const util::tString& port_prefix = "", const util::tString& target_port_prefix = "")
  {
    ConnectToTargetByName(target, create_missing_ports, start_with.GetWrapped(), count, port_prefix, target_port_prefix);
  }

  /*!
   * Connects/links all ports to ports with the same name in target port group.
   * (connection is (re)established when link is available)
   *
   * \param target_link Link name of target port group (relative to this port group)
   * \param start_with Port to start connecting with (NULL = first port)
   * \param count Number of ports to connect - starting with start port (-1 = all ports)
   * \param port_prefix Prefix of ports in this group. Prefix is cut off when comparing names. Ports without this prefix are skipped.
   * \param target_port_prefix Prefix of ports in target group to ignore. This is prepended when ports are created.
   */
  void ConnectToTargetByName(const util::tString& target_link, tAbstractPort* start_with = NULL, int count = -1, const util::tString& port_prefix = "", const util::tString& target_port_prefix = "")
  {
    ConnectImpl(3, NULL, target_link, false, start_with, count, port_prefix, target_port_prefix);
  }
  template <typename T>
  void ConnectToTargetByName(const util::tString& target_link, tPortWrapperBase<T> start_with, int count = -1, const util::tString& port_prefix = "", const util::tString& target_port_prefix = "")
  {
    ConnectToTargetByName(target_link, start_with.GetWrapped(), count, port_prefix, target_port_prefix);
  }

  /*!
   * Create port in this group
   *
   * \param name Name of port
   * \param type Data type of port
   * \param extra_flags Any extra flags for port
   * \return Created port
   */
  tAbstractPort* CreatePort(const util::tString& name, rrlib::serialization::tDataTypeBase type, int extra_flags = 0);

  /*!
   * Disconnect all of port group's ports
   *
   * \param incoming Disconnect incoming edges?
   * \param outgoing Disconnect outgoing edges?
   * \param start_with Port to start disconnecting with (NULL = first port)
   * \param count Number of ports to disconnect - starting with start port (-1 = all ports)
   */
  void DisconnectAll(bool incoming = true, bool outgoing = true, tAbstractPort* start_with = NULL, int count = -1);
  template <typename T>
  void DisconnectAll(bool incoming, bool outgoing, tPortWrapperBase<T> start_with, int count = -1)
  {
    DisconnectAll(incoming, outgoing, start_with.GetWrapped());
  }

  /*!
   * \return Default flags for any ports to be created in this Group
   */
  uint GetDefaultPortFlags()
  {
    return default_port_flags;
  }
};

} // namespace finroc
} // namespace core

#endif // core__port__tPortGroup_h__
