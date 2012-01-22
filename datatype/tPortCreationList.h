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

#ifndef core__datatype__tPortCreationList_h__
#define core__datatype__tPortCreationList_h__

#include "rrlib/finroc_core_utils/definitions.h"

#include "core/tCoreFlags.h"
#include "core/port/tPortFlags.h"
#include "rrlib/serialization/tDataTypeBase.h"
#include "rrlib/finroc_core_utils/container/tSimpleList.h"
#include "core/tFrameworkElement.h"
#include "core/datatype/tDataTypeReference.h"

#include "rrlib/serialization/tSerializable.h"
#include "rrlib/serialization/tStlContainerSuitable.h"

namespace rrlib
{
namespace serialization
{
class tInputStream;
} // namespace rrlib
} // namespace serialization

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
class tAbstractPort;

/*!
 * \author Max Reichardt
 *
 * List of ports to create.
 * Is only meant to be used in StaticParameters
 * For this reason, it is not real-time capable and a little more memory-efficient.
 */
class tPortCreationList : public rrlib::serialization::tSerializable, public rrlib::serialization::tStlUnsuitable
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
    tDataTypeReference type;

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
  static const uint cRELEVANT_FLAGS = tCoreFlags::cSHARED | tPortFlags::cIS_VOLATILE;

  /*! Should output port selection be visible in finstruct? */
  bool show_output_port_selection;

  /*! List backend (for remote Runtimes) */
  util::tSimpleList<tEntry> list;

  /*! FrameworkElement that list is wrapping (for local Runtimes) */
  tFrameworkElement* io_vector;

  /*! Flags for port creation */
  uint flags;

  /*! (Local) change listener */
  tListener* listener;

public:

  /*! Data Type */
  static rrlib::serialization::tDataTypeBase cTYPE;

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
  void CheckPort(tAbstractPort* ap, tFrameworkElement* io_vector_, uint flags_, const util::tString& name, rrlib::serialization::tDataTypeBase dt, bool output, tAbstractPort* prototype);

  /*!
   * Returns all child ports of specified framework element
   *
   * \param elem Framework Element
   * \param result List containing result
   */
  static void GetPorts(const tFrameworkElement* elem, util::tSimpleList<tAbstractPort*>& result);

public:

  tPortCreationList();

  /*!
   * Add entry to list
   *
   * \param name Port name
   * \param dt Data type
   * \param output Output port? (possibly irrelevant)
   */
  void Add(const util::tString& name, rrlib::serialization::tDataTypeBase dt, bool output);

  /*!
   * Applies changes to another IO vector
   *
   * \param io_vector Other io vector
   * \param flags Flags to use for port creation
   */
  void ApplyChanges(tFrameworkElement* io_vector_, uint flags_);

  virtual void Deserialize(rrlib::serialization::tInputStream& is);

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
  void InitialSetup(tFrameworkElement* managed_io_vector, uint port_creation_flags, bool show_output_port_selection_);

  virtual void Serialize(rrlib::serialization::tOutputStream& os) const;

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

#include "rrlib/serialization/tDataType.h"

extern template class ::rrlib::serialization::tDataType<finroc::core::tPortCreationList>;

#endif // core__datatype__tPortCreationList_h__
