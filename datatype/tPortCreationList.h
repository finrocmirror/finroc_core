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
#include "rrlib/finroc_core_utils/tJCBase.h"

#ifndef CORE__DATATYPE__TPORTCREATIONLIST_H
#define CORE__DATATYPE__TPORTCREATIONLIST_H

#include "core/tCoreFlags.h"
#include "core/port/tPortFlags.h"
#include "rrlib/finroc_core_utils/container/tSimpleList.h"
#include "core/tFrameworkElement.h"
#include "core/port/std/tPortDataImpl.h"
#include "core/datatype/tDataTypeReference.h"

namespace rrlib
{
namespace xml2
{
class tXMLNode;
} // namespace rrlib
} // namespace xml2

namespace finroc
{
namespace core
{
class tDataType;
class tAbstractPort;
class tCoreInput;
class tCoreOutput;

/*!
 * \author Max Reichardt
 *
 * List of ports to create.
 * Is only meant to be used in StructureParameters
 * For this reason, it is not real-time capable and a little more memory-efficient.
 */
class tPortCreationList : public tPortData
{
public:

  /*!
   * Entry in list
   */
  struct tEntry
  {
public:

    /*! Port name */
    util::tString name;

    /*! Port type - as string (used remote) */
    tDataTypeReference* type;

    /*! Output port? */
    bool output_port;

    tEntry(const util::tString& name_, const util::tString& type_, bool output_port_);

  };

public:

  /*!
   * Callback interface for changes to ports
   */
  class tListener : public util::tInterface
  {
  public:

    /*!
     * Called whenever a port was created
     *
     * \param ap Created port
     * \param prototype Prototype after which port was created
     */
    virtual void PortCreated(tAbstractPort* ap, tAbstractPort* prototype) = 0;

  };

private:

  /*! Relevant flags for comparison */
  static const int cRELEVANT_FLAGS = tCoreFlags::cSHARED | tPortFlags::cIS_VOLATILE;

  /*! Should output port selection be visible in finstruct? */
  bool show_output_port_selection;

  /*! List backend (for remote Runtimes) */
  util::tSimpleList<tEntry> list;

  /*! FrameworkElement that list is wrapping (for local Runtimes) */
  tFrameworkElement* io_vector;

  /*! Flags for port creation */
  int flags;

  /*! (Local) change listener */
  tListener* listener;

public:

  /*! Data Type */
  static tDataType* cTYPE;

private:

  /*!
   * Check whether we need to make adjustments to port
   *
   * \param ap Port to check
   * \param io_vector Parent
   * \param flags Creation flags
   * \param name New name
   * \param dt new data type
   * \param output output port
   * \param prototype Port prototype (only interesting for listener)
   */
  void CheckPort(tAbstractPort* ap, tFrameworkElement* io_vector_, int flags_, const util::tString& name, tDataType* dt, bool output, tAbstractPort* prototype);

  /*!
   * Returns all child ports of specified framework element
   *
   * \param elem Framework Element
   * \param result List containing result
   */
  static void GetPorts(const tFrameworkElement* elem, util::tSimpleList<tAbstractPort*>& result);

public:

  /*! Further io vectors that list might be wrapping */
  tPortCreationList();

  //    /**
  //     * Add another IO vector that will be managed by this list
  //     * (if its already in list, flags will be adjusted)
  //     *
  //     * \param ioVector IO vector to manage
  //     * \param flags Flags to use
  //     */
  //    public void addManaged(FrameworkElement ioVector, int flags) {
  //        synchronized(managed.ioVector) {
  //            for (int i = 0; i < moreIoVectors.size(); i++) {
  //                if (moreIoVectors.get(i).ioVector == ioVector) {
  //                    moreIoVectors.get(i).flags = flags;
  //                    applyChanges(moreIoVectors.get(i));
  //                    return;
  //                }
  //            }
  //            moreIoVectors.add(new ManagedIOVector(ioVector, flags));
  //            applyChanges(moreIoVectors.get(moreIoVectors.size() - 1));
  //        }
  //    }
  //
  //    /**
  //     * Remove framework element from list of managed IO vectors
  //     *
  //     * \param ioVector IO vector to remove
  //     */
  //    public void removeManaged(FrameworkElement ioVector) {
  //        synchronized(managed.ioVector) {
  //            for (int i = 0; i < moreIoVectors.size(); i++) {
  //                if (moreIoVectors.get(i).ioVector == ioVector) {
  //                    moreIoVectors.remove(i);
  //                }
  //            }
  //        }
  //    }

  /*!
   * Add entry to list
   *
   * \param name Port name
   * \param dt Data type
   * \param output Output port? (possibly irrelevant)
   */
  void Add(const util::tString& name, tDataType* dt, bool output);

  /*!
   * Applies changes to another IO vector
   *
   * \param io_vector Other io vector
   * \param flags Flags to use for port creation
   */
  void ApplyChanges(tFrameworkElement* io_vector_, int flags_);

  virtual void Deserialize(tCoreInput& is);

  virtual void Deserialize(const rrlib::xml2::tXMLNode& node);

  /*!
   * \return (Local) change listener
   */
  inline tListener* GetListener() const
  {
    return listener;
  }

  /*!
   * \return size of list
   */
  inline int GetSize() const
  {
    return io_vector == NULL ? list.Size() : io_vector->ChildCount();
  }

  /*!
   * Initially set up list for local operation
   *
   * \param managed_io_vector FrameworkElement that list is wrapping
   * \param port_creation_flags Flags for port creation
   * \param show_output_port_selection Should output port selection be visible in finstruct?
   */
  void InitialSetup(tFrameworkElement* managed_io_vector, int port_creation_flags, bool show_output_port_selection_);

  virtual void Serialize(tCoreOutput& os) const;

  virtual void Serialize(rrlib::xml2::tXMLNode& node) const;

  /*!
   * \param listener (Local) change listener
   */
  inline void SetListener(tListener* listener_)
  {
    this->listener = listener_;
  }

};

} // namespace finroc
} // namespace core

#endif // CORE__DATATYPE__TPORTCREATIONLIST_H
