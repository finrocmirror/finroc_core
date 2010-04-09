/**
 * You received this file as part of an advanced experimental
 * robotics framework prototype ('finroc')
 *
 * Copyright (C) 2007-2010 Max Reichardt,
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
#include "finroc_core_utils/tJCBase.h"

#ifndef CORE__TLINKEDGE_H
#define CORE__TLINKEDGE_H

#include "core/tRuntimeEnvironment.h"
#include "core/port/tAbstractPort.h"

namespace finroc
{
namespace core
{
/*!
 * \author Max Reichardt
 *
 * Edge that operates on links.
 *
 * (re)Establishes real edges when links are available.
 */
class tLinkEdge : public util::tUncopyableObject
{
private:

  /*!
   * Links that edge operates on.
   * One may be empty ("")
   * SourceLink is link for source port, targetLink is link for target port
   */
  util::tString source_link, target_link;

  /*! If one link in null - this contains handle of partner port */
  int port_handle;

  /*! Pointer to next edge - for a singly linked list */
  tLinkEdge* next;

public:

  // for synchronization on an object of this class
  mutable util::tMutex obj_synch;

private:

  /*!
   * Creates link edge for two links
   *
   * \param source_link_ source link
   * \param target_link_ target link
   * \param port_handle_ If one link is null - this contains handle of partner port
   */
  tLinkEdge(const util::tString& source_link_, const util::tString& target_link_, int port_handle_);

public:

  /*!
   * Creates link edge for handle and link
   *
   * \param source_link_ source link
   * \param target_handle handle of target port
   */
  tLinkEdge(const util::tString& source_link_, int target_handle);

  /*!
   * Creates link edge for two links
   *
   * \param source_link_ source link
   * \param target_link_ target link
   */
  tLinkEdge(const util::tString& source_link_, const util::tString& target_link_);

  /*!
   * Creates link edge for handle and link
   *
   * \param source_handle handle of source port
   * \param target_link_ target link
   */
  tLinkEdge(int source_handle, const util::tString& target_link_);

  virtual ~tLinkEdge();

  /*!
   * (should only be called by RuntimeEnvironment)
   *
   * \return Pointer to next edge - for a singly linked list
   */
  inline tLinkEdge* GetNext()
  {
    return next;
  }

  inline int GetPortHandle()
  {
    return port_handle;
  }

  inline util::tString GetSourceLink()
  {
    return source_link;
  }

  inline util::tString GetTargetLink()
  {
    return target_link;
  }

  /*!
   * Called by RuntimeEnvironment when link that this object is obviously interested in has been added/created
   *
   * \param re RuntimeEnvironment
   * \param link Link that has been added
   * \param port port linked to
   */
  void LinkAdded(tRuntimeEnvironment* re, const util::tString& link, tAbstractPort* port);

  /*!
   * (should only be called by RuntimeEnvironment)
   *
   * \param next Pointer to next edge - for a singly linked list
   */
  inline void SetNext(tLinkEdge* next_)
  {
    this->next = next_;
  }

//  /**
//   * Called by RuntimeEnvironment when link that this object is obviously interested in has been removed
//   *
//   * \param re RuntimeEnvironment
//   * \param link Link that has been added
//   * \param port port linked to
//   */
//  void linkRemoved(RuntimeEnvironment re, String link, AbstractPort port) {
//    // do nothing... currently
//  }

};

} // namespace finroc
} // namespace core

#endif // CORE__TLINKEDGE_H
