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
/*!\file    core/port/tConnectOptions.h
 *
 * \author  Max Reichardt
 *
 * \date    2016-07-12
 *
 * \brief   Contains tConnectOptions
 *
 * \b tConnectOptions
 *
 * Bundles available options for connecting ports (bundled for convenience and locality of changes)
 *
 */
//----------------------------------------------------------------------
#ifndef __core__port__tConnectOptions_h__
#define __core__port__tConnectOptions_h__

//----------------------------------------------------------------------
// External includes (system with <>, local with "")
//----------------------------------------------------------------------
#include "rrlib/util/tEnumBasedFlags.h"
#include "rrlib/rtti/rtti.h"
#include "rrlib/rtti_conversion/tConversionOperationSequence.h"

//----------------------------------------------------------------------
// Internal includes with ""
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
/*!
 * Flags that can be set for connections/connectors.
 * First 16 Bits are sent to Finstruct
 */
enum class tConnectionFlag
{
  // constant flags that are always transferred to finstruct
  FINSTRUCTED,           //!< Was this connector created using finstruct (or loaded from XML file)?
  RECONNECT,             //!< Recreate connection when connected port was deleted and a port with the same qualified name appears (Reconnecting is automatically done for network connections and connections to VOLATILE-flagged ports)
  OPTIONAL,              //!< Flags optional connections. They are handled and reported as non-critical if they cannot be established.
  SCHEDULING_NEUTRAL,    //!< Flags connections that should not have any effect on scheduling order

  // Connect direction
  // If none is set, connection direction is determined automatically. This should be appropriate for almost any situation. However, another direction may be enforced)
  // Both flags must not be set at the same time (typically causes an invalid_argument exception)
  DIRECTION_TO_DESTINATION, //!< Specified port is destination port (port passed as argument to ConnectTo member function; or second port passed to static method)
  DIRECTION_TO_SOURCE,      //!< Specified port is destination port (port passed as argument to ConnectTo member function; or second port passed to static method)

  // internal flag
  NON_PRIMARY_CONNECTOR, //!< Connector was created and is represented by higher-level connector (that still exists) (-> e.g. not necessary to transfer this connector to finstruct)

  // unused flags for future use
  UNUSED_8, UNUSED_9, UNUSED_10, UNUSED_11, UNUSED_12, UNUSED_13, UNUSED_14,

  // these flags are automatically set
  CONVERSION,            //!< Connector has type conversion operations attached that must be applied on publishing values
  NAMED_PARAMETERS,      //!< Indicates whether connector options include named parameters (relevant mainly for uniform serialization of ConnectOptions and UriConnectorOptions (only the latter supports named parameters))
  PUBLISHED,             //!< Only relevant for URI connectors: Has been published
  DISCONNECTED,          //!< Connector has been removed
};

/*!
 * Set of framework element flags.
 */
typedef rrlib::util::tEnumBasedFlags<tConnectionFlag, uint32_t> tConnectionFlags;

//----------------------------------------------------------------------
// Class declaration
//----------------------------------------------------------------------
//! Connect Options
/*!
 * Bundles available options for connecting ports (bundled for convenience and locality of changes)
 */
struct tConnectOptions
{
  /*! Any explicit conversion operations to apply (implicit type conversions are applied automatically) */
  rrlib::rtti::conversion::tConversionOperationSequence conversion_operations;

  /*! Flags for connection */
  tConnectionFlags flags;


  tConnectOptions() : conversion_operations(), flags() {}

  /*!
   * Convenience constructor so that e.g. single flags can also be used in ConnectTo methods
   *
   * \param option1 Any valid option (valid parameter for Set() method)
   */
  template <typename TOption1>
  tConnectOptions(const TOption1& option1) : tConnectOptions()
  {
    Set(option1);
  }
  /*!
   * \param conversion_operations Any explicit conversion operations to apply (implicit type conversions are applied automatically)
   * \param flags Flags for connection
   */
  tConnectOptions(const rrlib::rtti::conversion::tConversionOperationSequence& conversion_operations, const tConnectionFlags& flags) :
    conversion_operations(conversion_operations),
    flags(flags)
  {}

  /**
   * Serialize URI connect options
   *
   * \param stream Stream to write to
   * \param auto_set_flags Whether to auto-adjust flags
   */
  void Serialize(rrlib::serialization::tOutputStream& stream, bool auto_set_flags) const;

  /**
   * \param conversion_operations New conversion operations
   */
  void Set(const rrlib::rtti::conversion::tConversionOperationSequence& conversion_operations)
  {
    this->conversion_operations = conversion_operations;
  }
  /**
   * \param conversion_operations New flags
   */
  void Set(const tConnectionFlags& flags)
  {
    this->flags = flags;
  }

  /*!
   * Unsets all flags that are automatically set by Finroc core.
   * (Used for protection against misuse/undefined behavior)
   *
   * \param flags Flags to clear from auto-flags
   * \return Flags with all auto-flags unset
   */
  static tConnectionFlags UnsetAutoFlags(const tConnectionFlags& flags);
};


constexpr inline tConnectionFlags operator | (const tConnectionFlags& flags1, const tConnectionFlags& flags2)
{
  return tConnectionFlags(flags1.Raw() | flags2.Raw());
}

constexpr inline tConnectionFlags operator | (tConnectionFlag flag1, tConnectionFlag flag2)
{
  return tConnectionFlags(flag1) | tConnectionFlags(flag2);
}

constexpr inline tConnectionFlags operator & (const tConnectionFlags& flags1, const tConnectionFlags& flags2)
{
  return tConnectionFlags(flags1.Raw() & flags2.Raw());
}

constexpr inline bool operator == (const tConnectionFlags& flags1, const tConnectionFlags& flags2)
{
  return flags1.Raw() == flags2.Raw();
}


inline rrlib::serialization::tOutputStream& operator << (rrlib::serialization::tOutputStream& stream, const tConnectOptions& options)
{
  options.Serialize(stream, true);
  return stream;
}
rrlib::serialization::tInputStream& operator >> (rrlib::serialization::tInputStream& stream, tConnectOptions& options);

//----------------------------------------------------------------------
// End of namespace declaration
//----------------------------------------------------------------------
}
}


#endif
