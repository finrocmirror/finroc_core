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
/*!\file    core/port/tConnector.h
 *
 * \author  Max Reichardt
 *
 * \date    2015-12-15
 *
 * \brief   Contains tConnector
 *
 * \b tConnector
 *
 * A connector connects two ports.
 * This is the base class for more specialized connectors defined for specific port types.
 *
 */
//----------------------------------------------------------------------
#ifndef __core__port__tConnector_h__
#define __core__port__tConnector_h__

//----------------------------------------------------------------------
// External includes (system with <>, local with "")
//----------------------------------------------------------------------

//----------------------------------------------------------------------
// Internal includes with ""
//----------------------------------------------------------------------
#include "core/tAnnotatable.h"
#include "core/port/tConnectOptions.h"

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
class tUriConnector;

//----------------------------------------------------------------------
// Class declaration
//----------------------------------------------------------------------
//! Port connector
/*!
 * A connector connects two ports.
 * This is the base class for more specialized connectors defined for specific port types.
 *
 * Implementation note: if subclasses contain conversion operations, they must be the first member variable and passed to the constructor.
 */
class tConnector : public tAnnotatable
{

//----------------------------------------------------------------------
// Public methods and typedefs
//----------------------------------------------------------------------
public:

  typedef tConnectionFlag tFlag;
  typedef tConnectionFlags tFlags;

  /*!
   * \param source_port Source Port
   * \param destination_port Destination Port
   * \param connect_options Connect options for this connector
   * \param conversion_sequence_storage If subclasses contain conversion operations, they must be the first member variable and passed to the constructor here
   */
  tConnector(tAbstractPort& source_port, tAbstractPort& destination_port, const tConnectOptions& connect_options = tConnectOptions(),
             const rrlib::rtti::conversion::tConversionOperationSequence& conversion_sequence_storage = rrlib::rtti::conversion::tConversionOperationSequence::cNONE);

  virtual ~tConnector();

  /*!
   * \return Conversion operations for this connector
   */
  const rrlib::rtti::conversion::tConversionOperationSequence& ConversionOperations() const
  {
    return flags.Get(tFlag::CONVERSION) ? *reinterpret_cast<const rrlib::rtti::conversion::tConversionOperationSequence*>(this + 1) : rrlib::rtti::conversion::tConversionOperationSequence::cNONE;
  }

  /*!
   * \return Destination Port
   */
  tAbstractPort& Destination() const
  {
    return destination_port;
  }

  /*!
   * Disconnects this connector and stops any reconnecting.
   * This connector object will be disposed and should not be used after calling this method.
   */
  void Disconnect();

  /*!
   * \return Flags set for this connector
   */
  const tFlags& Flags() const
  {
    return flags;
  }

  /*!
   * Notify specified URI connector (typically owner) on disconnect.
   *
   * There are two constraints:
   * - URI connector object must exists longer than this connector
   * - Max. 1 URI connector may register for notification
   *
   * \param owner Connector to notify
   */
  void NotifyOnDisconnect(tUriConnector& owner);

  /*!
   * \return Source Port
   */
  tAbstractPort& Source() const
  {
    return source_port;
  }


//----------------------------------------------------------------------
// Private fields and methods
//----------------------------------------------------------------------
private:

  friend class tAbstractPort;


  /*! Source Port */
  tAbstractPort& source_port;

  /*! Destination Port */
  tAbstractPort& destination_port;

  /*! Flags set for this connector */
  tFlags flags;


  /*!
   * Called by tAbstractPort when connector is removed.
   * (Deleting this object is deferred due to thread-safety)
   *
   * \param stop_any_reconnecting Stop any reconnecting of connection? (Usually done if disconnect is explicitly called - usually not done when port is deleted)
   */
  void OnDisconnect(bool stop_any_reconnecting);
};

//----------------------------------------------------------------------
// End of namespace declaration
//----------------------------------------------------------------------
}
}


#endif
