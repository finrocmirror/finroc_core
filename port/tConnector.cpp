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
/*!\file    core/port/tConnector.cpp
 *
 * \author  Max Reichardt
 *
 * \date    2015-12-15
 *
 */
//----------------------------------------------------------------------
#include "core/port/tConnector.h"

//----------------------------------------------------------------------
// External includes (system with <>, local with "")
//----------------------------------------------------------------------

//----------------------------------------------------------------------
// Internal includes with ""
//----------------------------------------------------------------------
#include "core/tRuntimeEnvironment.h"
#include "core/port/tAbstractPort.h"

//----------------------------------------------------------------------
// Debugging
//----------------------------------------------------------------------
#include <cassert>

//----------------------------------------------------------------------
// Namespace usage
//----------------------------------------------------------------------

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
// Const values
//----------------------------------------------------------------------

//----------------------------------------------------------------------
// Implementation
//----------------------------------------------------------------------

namespace internal
{

//! Annotation to link tConnector and tUriConnector classes
/*!
 * Annotation for tConnector objects to attach tUriConnector objects.
 * They are notified if tConnector is removed.
 *
 * This class was introduced as it saves memory not to have an additional pointer
 * in every tConnector that is rarely used.
 */
class tUriConnectorOwnerAnnotation : public tAnnotation
{

//----------------------------------------------------------------------
// Public methods and typedefs
//----------------------------------------------------------------------
public:

  /*!
   * \param connector Reference to tUriConnector
   */
  tUriConnectorOwnerAnnotation(tUriConnector& connector) :
    connector(connector)
  {}

  /*!
   * \return Reference to tUriConnector
   */
  tUriConnector& Get()
  {
    return connector;
  }

//----------------------------------------------------------------------
// Private fields and methods
//----------------------------------------------------------------------
private:

  /*! Reference to tUriConnector */
  tUriConnector& connector;

};
}

tConnector::tConnector(tAbstractPort& source_port, tAbstractPort& destination_port, const tConnectOptions& connect_options, const rrlib::rtti::conversion::tConversionOperationSequence& conversion_sequence_storage) :
  source_port(source_port),
  destination_port(destination_port),
  flags(tConnectOptions::UnsetAutoFlags(connect_options.flags) | (&conversion_sequence_storage != &rrlib::rtti::conversion::tConversionOperationSequence::cNONE ? tFlags(tFlag::CONVERSION) : tFlags()))
{
  if (flags.Get(tFlag::CONVERSION))
  {
    assert(&conversion_sequence_storage == &ConversionOperations() && "Conversion operations must be first member in subclass");
  }
}

tConnector::~tConnector()
{
}

void tConnector::Disconnect()
{
  rrlib::thread::tLock lock(tRuntimeEnvironment::GetInstance().GetStructureMutex());
  tAbstractPort::DisconnectImplementation(*this, true);
}

void tConnector::NotifyOnDisconnect(tUriConnector& owner)
{
  this->EmplaceAnnotation<internal::tUriConnectorOwnerAnnotation>(owner);
}

void tConnector::OnDisconnect(bool stop_any_reconnecting)
{
  flags.Set(tConnectionFlag::DISCONNECTED, true);

  internal::tUriConnectorOwnerAnnotation* annotation = this->GetAnnotation<internal::tUriConnectorOwnerAnnotation>();
  if (annotation)
  {
    annotation->Get().OnConnectorDisconnect(stop_any_reconnecting);
  }
}


//----------------------------------------------------------------------
// End of namespace declaration
//----------------------------------------------------------------------
}
}
