/**
 * You received this file as part of an advanced experimental
 * robotics framework prototype ('finroc')
 *
 * Copyright (C) 2007-2012 Max Reichardt,
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

#ifndef core__tLinkEdge_h__
#define core__tLinkEdge_h__

#include "rrlib/finroc_core_utils/definitions.h"
#include <array>

namespace finroc
{
namespace core
{
class tRuntimeEnvironment;
class tAbstractPort;

/*!
 * \author Max Reichardt
 *
 * Edge that operates on links.
 *
 * (re)Establishes real edges when links are available.
 */
class tLinkEdge : boost::noncopyable
{
public:

  /*!
   * Reference to a port - either link or pointer
   */
  class tPortReference
  {
    friend class tLinkEdge;

    const util::tString link;
    tAbstractPort* const pointer;
  public:
    tPortReference(const util::tString& link) : link(link), pointer(NULL) {}
    tPortReference(tAbstractPort& port) : link(""), pointer(&port) {}
  };

  /*!
   * Creates link edge for handle and link
   *
   * \param port1 Link or pointer to first port
   * \param port2 Link or pointer to second port
   * \param both_connect_directions If false, only connections from port1 to port2 will be created
   * \param finstructed Is this a finstructed link edge?
   */
  tLinkEdge(const tPortReference& port1, const tPortReference& port2, bool both_connect_directions, bool finstructed = false);

  ~tLinkEdge();

  /*!
   * \return Link of first port - possibly empty if a port was provided directly instead
   */
  inline util::tString GetSourceLink() const
  {
    return ports[0].link;
  }

  /*!
   * \return Link of second port - possibly empty if a port was provided directly instead
   */
  inline util::tString GetTargetLink() const
  {
    return ports[1].link;
  }

  /*!
   * \return Was this link edge finstructed?
   */
  bool IsFinstructed() const
  {
    return finstructed;
  }


private:

  friend class tRuntimeEnvironment;

  /*!
   * Ports that edge operates on.
   * At least one of the two is linked
   */
  const std::array<tPortReference, 2> ports;

  /*!
   * Should the two ports be connected in any direction?
   * If false, only connections from ports[0] to ports[1] will be created
   */
  const bool both_connect_directions;

  /*! Pointer to next edge - for a singly linked list */
  tLinkEdge* next_edge;

  /*! Is this a finstructed link edge? */
  const bool finstructed;

  /*!
   * \return Pointer to next edge - for a singly linked list
   */
  inline tLinkEdge* GetNextEdge() const
  {
    return next_edge;
  }

  /*!
   * \param next Pointer to next edge - for a singly linked list
   */
  inline void SetNextEdge(tLinkEdge* next_edge)
  {
    this->next_edge = next_edge;
  }

  /*!
   * Called by RuntimeEnvironment when link that this object is obviously interested in has been added/created
   * (must only be called with lock on runtime-registry)
   *
   * \param re RuntimeEnvironment
   * \param link Link that has been added
   * \param port port linked to
   */
  void LinkAdded(tRuntimeEnvironment& re, const util::tString& link, tAbstractPort& port) const;

};

} // namespace finroc
} // namespace core

#endif // core__tLinkEdge_h__
