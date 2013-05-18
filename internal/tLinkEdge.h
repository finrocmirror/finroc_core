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
/*!\file    core/internal/tLinkEdge.h
 *
 * \author  Max Reichardt
 *
 * \date    2012-10-28
 *
 * \brief   Contains tLinkEdge
 *
 * \b tLinkEdge
 *
 * Edge that operates on links.
 * (Re)Establishes real edges when links are available.
 *
 */
//----------------------------------------------------------------------
#ifndef __core__internal__tLinkEdge_h__
#define __core__internal__tLinkEdge_h__

//----------------------------------------------------------------------
// External includes (system with <>, local with "")
//----------------------------------------------------------------------
#include <array>
#include "rrlib/util/tNoncopyable.h"

//----------------------------------------------------------------------
// Internal includes with ""
//----------------------------------------------------------------------
#include "core/definitions.h"

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
class tRuntimeEnvironment;

namespace internal
{

//----------------------------------------------------------------------
// Class declaration
//----------------------------------------------------------------------
//! Link edge
/*!
 * Edge that operates on links.
 * (Re)Establishes real edges when ports with specified links are available.
 */
class tLinkEdge : private rrlib::util::tNoncopyable
{

//----------------------------------------------------------------------
// Public methods and typedefs
//----------------------------------------------------------------------
public:

  /*!
   * Reference to a port - either link or pointer
   */
  class tPortReference
  {
    friend class tLinkEdge;

    const tString link;
    tAbstractPort* const pointer;
  public:
    tPortReference(const tString& link) : link(link), pointer(NULL) {}
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
  inline tString GetSourceLink() const
  {
    return ports[0].link;
  }

  /*!
   * \return Link of second port - possibly empty if a port was provided directly instead
   */
  inline tString GetTargetLink() const
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

//----------------------------------------------------------------------
// Private fields and methods
//----------------------------------------------------------------------
private:

  friend class finroc::core::tRuntimeEnvironment;

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
  void LinkAdded(tRuntimeEnvironment& re, const tString& link, tAbstractPort& port) const;

};

//----------------------------------------------------------------------
// End of namespace declaration
//----------------------------------------------------------------------
}
}
}


#endif
