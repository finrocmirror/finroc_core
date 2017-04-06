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
/*!\file    core/port/tUriConnector.h
 *
 * \author  Max Reichardt
 *
 * \date    2017-01-15
 *
 * \brief   Contains tUriConnector
 *
 * \b tUriConnector
 *
 * Base class for URI connectors.
 * URI connectors connect a port (owner) to another port whose address is specified by a URI.
 * The owner can be source or destination port of the connection.
 * Realization of the connection is up to the subclass (internal URI connectors e.g. create a tConnector).
 *
 */
//----------------------------------------------------------------------
#ifndef __core__port__tUriConnector_h__
#define __core__port__tUriConnector_h__

//----------------------------------------------------------------------
// External includes (system with <>, local with "")
//----------------------------------------------------------------------
#include "rrlib/util/tNoncopyable.h"
#include "rrlib/util/tIteratorRange.h"

//----------------------------------------------------------------------
// Internal includes with ""
//----------------------------------------------------------------------
#include "core/tFrameworkElement.h"
#include "core/port/tUriConnectOptions.h"

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
//! Base class for URI connectors
/*!
 * Base class for URI connectors.
 * URI connectors connect a port (owner) to another port whose address is specified by a URI.
 * The owner can be source or destination port of the connection.
 * Realization of the connection is up to the subclass (internal URI connectors e.g. create a tConnector).
 */
class tUriConnector : public rrlib::util::tNoncopyable
{

//----------------------------------------------------------------------
// Public methods and typedefs
//----------------------------------------------------------------------
public:

  /*! Status of connector (e.g. displayed in tools) */
  enum class tStatus : uint8_t
  {
    DISCONNECTED,
    CONNECTED,
    ERROR
  };

  typedef tConnectionFlag tFlag;
  typedef tConnectionFlags tFlags;

  typedef rrlib::rtti::tConstParameterDefinitionRange tConstParameterDefinitionRange;
  typedef rrlib::util::tIteratorRange<const rrlib::rtti::tTypedConstPointer*> tParameterValueRange;
  class tSchemeHandler;
  class tOwner;
  typedef rrlib::serialization::tRegister<tSchemeHandler*, 16, 16, uint8_t> tSchemeHandlerRegister;

  virtual ~tUriConnector();

  /*!
   * Creates and registers connector.
   * If equivalent connector already exists, does nothing.
   *
   * \param owner_port Port that owns this connector
   * \param uri URI of partner port
   * \param connect_options Connect options
   * \return True if connector was created. False if an equivalent connector already exists.
   * \throw Throws exception on invalid arguments
   */
  static bool Create(core::tAbstractPort& owner_port, const tURI& uri, const tUriConnectOptions& connect_options);

  /*!
   * \return Conversion operations for this connector
   */
  const rrlib::rtti::conversion::tConversionOperationSequence& ConversionOperations() const
  {
    return conversion_operations;
  }

  /*!
   * Disconnects and deletes this connector.
   * Removes it from owner and possibly from any places it registered as listener.
   */
  void Disconnect()
  {
    owner.Disconnect(*this);
  }

  /*!
   * \return Flags set for this connector
   */
  inline const tFlags& Flags() const
  {
    return flags;
  }

  /*!
   * \return Iterator range over parameter definitions
   */
  const tConstParameterDefinitionRange& GetParameterDefinitions() const
  {
    return scheme_handler.GetParameterDefinitions();
  }

  /*!
   * \return Iterator range over parameters that URI connector may have.
   */
  virtual tParameterValueRange GetParameterValues() const
  {
    return tParameterValueRange();
  }

  /*!
   * \return Scheme handler for connector's URI scheme
   */
  const tSchemeHandler& GetSchemeHandler() const
  {
    return scheme_handler;
  }

  /*!
   * \return Regiser with all registered scheme handlers
   */
  static const tSchemeHandlerRegister& GetSchemeHandlerRegister();

  /*!
   * \return Status of connector
   */
  tStatus GetStatus() const
  {
    return status;
  }

  /*!
   * \param Port that owns connector
   * \return Whether this is an outgoing connector (relative to owner)
   */
  virtual bool IsOutgoingConnector(core::tAbstractPort& owning_port) const = 0;

  /*!
   * \return Reference to owner
   */
  tOwner& Owner() const
  {
    return owner;
  }

  /*!
   * Publish() needs to called for every tUriConnector object that is fully constructed so that is visible in finstruct.
   * May be called multiple times: all calls to this method after the first do nothing.
   */
  void Publish();

  /*!
   * \param index Index of parameter in parameter definitions
   * \param new_value New value for parameter
   * \return Whether parameter changed
   */
  virtual bool SetParameter(size_t index, const rrlib::rtti::tTypedConstPointer& new_value);

  /*!
   * \return URI of partner port (preferably normalized)
   */
  const tURI& Uri() const
  {
    return uri;
  }


  /*! Owner of URI connectors */
  class tOwner : public core::tFrameworkElement
  {
  public:

    using tFrameworkElement::tFrameworkElement;

    virtual ~tOwner();

    /*!
     * Removes and deletes all connectors owned by this object
     */
    void ClearUriConnectors();

    /*!
     * Disconnects and deletes connector.
     * Removes it from from any places it registered as listener.
     *
     * \param connector Connector to delete
     */
    void Disconnect(tUriConnector& connector);

    /*!
     * \param connector Connector to check
     * \return Whether this object owns specified connector
     */
    bool Owns(tUriConnector& connector) const
    {
      return &connector.owner == this;
    }

    /*!
     * \return List with connectors owned by this object. Pointers in vector may be empty (to uniquely identify URI connectors by their index)
     */
    const std::vector<std::unique_ptr<tUriConnector>>& UriConnectors() const
    {
      return connectors ? (*connectors) : cEMPTY_CONNECTOR_LIST;
    }

  private:

    friend class tUriConnector;

    static const std::vector<std::unique_ptr<tUriConnector>> cEMPTY_CONNECTOR_LIST;

    /*!
     * Contains any URI connectors owned and managed by this object.
     * Implementation note: std::vector is lazily initialized to save memory - as currently not many potential owners actually have URI connectors.
     *                      This can be changed if required (e.g. template parameter).
     *                      Pointers in vector may be empty (to uniquely identify URI connectors by their index)
     */
    std::unique_ptr<std::vector<std::unique_ptr<tUriConnector>>> connectors;
  };

  /*!
   * Handler of URI schemes
   * Auto-registers on instantiation.
   */
  class tSchemeHandler
  {
  public:

    /*!
     * \return Name of scheme that is handled by this handler
     */
    const char* GetSchemeName() const
    {
      return scheme_name;
    }

    /*!
     * \return Parameters that connectors with this scheme have
     */
    const tConstParameterDefinitionRange& GetParameterDefinitions() const
    {
      return parameter_definitions;
    }

    friend rrlib::serialization::tOutputStream& operator << (rrlib::serialization::tOutputStream& stream, const tUriConnector::tSchemeHandler& handler)
    {
      if (tUriConnector::GetSchemeHandlerRegister().WriteEntry(stream, handler.handle))
      {
        stream << handler.scheme_name;
      }
      return stream;
    }

  protected:
    /*!
     * \param scheme_name Name of scheme that is handled by this handler
     * \param parameter_definitions Parameters that connectors with this scheme have
     */
    tSchemeHandler(const char* scheme_name, const tConstParameterDefinitionRange& parameter_definitions);

    virtual ~tSchemeHandler() = default;

  private:

    friend class tUriConnector;

    /*! Name of scheme that is handled by this handler */
    const char* scheme_name;

    /*! Parameters that connectors with this scheme have */
    tConstParameterDefinitionRange parameter_definitions;

    uint8_t handle;

    /*!
     * Creates and registers connector.
     * If equivalent connector already exists, does nothing.
     *
     * \param owner_port Port that owns this connector
     * \param uri URI of partner port
     * \param parsed_uri Parsed URI of partner port
     * \param connect_options Connect options
     * \return True if connector was created. False if an equivalent connector already exists.
     * \throw Throws exception on invalid arguments
     */
    virtual bool Create(core::tAbstractPort& owner_port, const tURI& uri, const rrlib::uri::tURIElements& parsed_uri, const tUriConnectOptions& connect_options) = 0;
  };

//----------------------------------------------------------------------
// Protected fields
//----------------------------------------------------------------------
protected:

  /*!
   * Note that constructor transfers ownership of this object to owner.
   * Note for UriConnectors that can be constructed without using Create() method: Publish() needs to called for every tUriConnector object that is fully constructed so that is visible in finstruct.
   *
   * \param owner Reference to object that owns this connector
   * \param uri URI of partner port (preferably normalized)
   * \param connect_options Connect options for this connector
   * \param scheme_handler Scheme handler for connector's URI scheme
   */
  tUriConnector(tOwner& owner, const tURI& uri, const tConnectOptions& connect_options, const tSchemeHandler& scheme_handler);

  /*! Flags set for this connector */
  tFlags flags;

  /*! Reference to owner */
  tOwner& owner;

  /*! Conversion operations for this connector */
  const rrlib::rtti::conversion::tConversionOperationSequence conversion_operations;

  /*! URI of partner port (preferably normalized) */
  const tURI uri;

  /*! Scheme handler for connector's URI scheme */
  const tSchemeHandler& scheme_handler;

  /*! Status of connector */
  tStatus status;


  /*!
   * Set parameters in constructor from connect options
   *
   * \param connect_options Connect options with parameters
   */
  void SetParametersInConstructor(const core::tUriConnectOptions& connect_options);

  /*!
   * If status changes, change is published to runtime listeners
   *
   * \param new_status New status of connector.
   */
  void SetStatus(tStatus new_status);

//----------------------------------------------------------------------
// Private fields and methods
//----------------------------------------------------------------------
private:

  friend class tOwner;
  friend class tConnector;

  /*!
   * Called when URI connector is disconnected/removed
   */
  virtual void OnDisconnect() = 0;

  /*!
   * Callback by tConnector when it is disconnected
   *
   * \param stop_any_reconnecting Stop any reconnecting of connection? (Usually done if disconnect is explicitly called - usually not done when port is deleted)
   */
  virtual void OnConnectorDisconnect(bool stop_any_reconnecting)
  {}
};

//----------------------------------------------------------------------
// End of namespace declaration
//----------------------------------------------------------------------
}
}


#endif
