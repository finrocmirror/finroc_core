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
/*!\file    core/internal/tLocalUriConnector.h
 *
 * \author  Max Reichardt
 *
 * \date    2015-12-22
 *
 * \brief   Contains tLocalUriConnector
 *
 * \b tLocalUriConnector
 *
 * Connects two ports with at least one port referenced by a local URI (path).
 * Ports do not need to be available when this object is created.
 * Instantiates connection (tConnector) when both ports are available.
 * If the RECONNECT flag is set or one port is flagged VOLATILE, automatically reconnects
 * when connected port was deleted and a port with the same qualified name appears.
 */
//----------------------------------------------------------------------
#ifndef __core__internal__tLocalUriConnector_h__
#define __core__internal__tLocalUriConnector_h__

//----------------------------------------------------------------------
// External includes (system with <>, local with "")
//----------------------------------------------------------------------
#include <array>

//----------------------------------------------------------------------
// Internal includes with ""
//----------------------------------------------------------------------
#include "core/port/tUriConnector.h"
#include "core/port/tConnector.h"
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
class tConnector;

namespace internal
{

/*!
 * Checks provided connection flags for sanity (for a Connect method).
 * \param flags Flags to check
 * \throws Throws std::invalid_argument if flags are invalid
 */
void CheckConnectionFlags(const tConnectionFlags& flags);

//----------------------------------------------------------------------
// Class declaration
//----------------------------------------------------------------------
//! Connector operating on port URIs
/*!
 * Connects two ports with at least one port referenced by a local URI (path).
 * Ports do not need to be available when this object is created.
 * Instantiates connection (tConnector) when both ports are available.
 * If the RECONNECT flag is set or one port is flagged VOLATILE, automatically reconnects
 * when connected port was deleted and a port with the same qualified name appears.
 *
 * This class registers and unregisters connector at runtime environment in Create() and Remove()
 */
class tLocalUriConnector : public tUriConnector
{

//----------------------------------------------------------------------
// Public methods and typedefs
//----------------------------------------------------------------------
public:

  /*!
   * Reference to a port - either path or pointer
   */
  struct tPortReference
  {
    const tPath path;
    tAbstractPort* const pointer;

    tPortReference(const tPath& path) : path(path), pointer(nullptr) {}
    tPortReference(tAbstractPort& port) : path(), pointer(&port) {}

    bool operator==(const tPortReference& other) const
    {
      return path == other.path && pointer == other.pointer;
    }
  };

  virtual ~tLocalUriConnector();

  /*!
   * Creates and registers connector.
   * If equivalent connector already exists, does nothing.
   * Note: Must call Publish() on created tUriConnector
   *
   * \param port1 Path or reference of first port
   * \param port2 Path of second port
   * \param connect_options Connect options
   * \param owner Owner of connecter
   * \param existing_connection If a connection that this connector should possibly reconnect already exists, it must be provided here
   * \return True if connector was created. False if an equivalent connector already exists.
   */
  static bool Create(const tPortReference& port1, const tPath& port2, const tConnectOptions& connect_options,
                     tOwner& owner, tConnector* existing_connection = nullptr);

  /*!
   * \return If an actual connection created by this object currently exists, contains a pointer to respective connector
   */
  tConnector* GetConnection() const
  {
    return connection;
  }

  /*!
   * \return Ports that this connector operates on. At least one of the two is referenced by string.
   */
  inline const std::array<tPortReference, 2>& GetPortReferences() const
  {
    return ports;
  }

  virtual bool IsOutgoingConnector(core::tAbstractPort& owning_port) const override;

//----------------------------------------------------------------------
// Private fields and methods
//----------------------------------------------------------------------
private:

  friend class core::tRuntimeEnvironment;
  friend class core::tConnector;
  friend class core::tAbstractPort;

  /*!
   * Ports that this connector operates on.
   * At least one of the two is referenced by string.
   */
  const std::array<tPortReference, 2> ports;

  /*! If an actual connection created by this object currently exists, contains a pointer to respective connector */
  tConnector* connection;


  /*!
   * Constructor must not be called. Call create instead. Is public so that std::make_shared works.
   *
   * \param port1 Path or reference of first port
   * \param port2 Path of second port
   * \param connect_options Connect options
   * \param owner Owner of connecter
   * \param existing_connection If a connection that this connector should possibly reconnect already exists, it must be provided here
   */
  tLocalUriConnector(const tPortReference& port1, const tPath& port2, const tConnectOptions& connect_options,
                     tOwner& owner, tConnector* existing_connection);

  /*!
   * Called by 'connection' connector when it is disconnected
   */
  virtual void OnConnectorDisconnect(bool stop_any_reconnecting) override;

  virtual void OnDisconnect() override;

  /*!
   * Called by RuntimeEnvironment when port with path that this object is interested in has been added/created
   *
   * Note 1: must only be called with lock on runtime-registry
   * Note 2: May cause call of Disconnect() including removal from owner (which might be the caller)
   *
   * \param runtime RuntimeEnvironment
   * \param path Path of port that has been added
   * \param port Port with path
   */
  void OnPortAdded(tRuntimeEnvironment& runtime, const tPath& path, tAbstractPort& port);
};


//----------------------------------------------------------------------
// End of namespace declaration
//----------------------------------------------------------------------
}
}
}


#endif
