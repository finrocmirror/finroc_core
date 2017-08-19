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
/*!\file    core/tFrameworkElementFlags.h
 *
 * \author  Max Reichardt
 *
 * \date    2012-10-27
 *
 * \brief   Contains tFrameworkElementFlags
 *
 * \b tFrameworkElementFlags
 *
 * Set of framework element flags.
 * As enums do not support OR operations, this type is used to handle combining of flags.
 */
//----------------------------------------------------------------------
#ifndef __core__tFrameworkElementFlags_h__
#define __core__tFrameworkElementFlags_h__

//----------------------------------------------------------------------
// External includes (system with <>, local with "")
//----------------------------------------------------------------------
#include "rrlib/util/tEnumBasedFlags.h"

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
//! Flags that can be set for framework elements
/*!
 * This enum contains flags for framework elements.
 * The lower 22 bit are constant flags which may not change
 * at runtime whereas the upper 10 may change
 */
enum class tFrameworkElementFlag
{
  PORT,                    //!< Is this framework element a port?

  // Non-Port flags
  EDGE_AGGREGATOR,         //!< Is this an edge aggregating framework element? (edges connect ports)
  INTERFACE,               //!< Is this framework element (usually also edge aggregator) an interface of its parent? (one of possibly many)
  SENSOR_DATA,             //!< Is the data processed in this framework element and all framework elements below only sensor data? (hint for visualization; relevant for interface especially)
  CONTROLLER_DATA,         //!< Is the data processed in this framework element and all framework elements below only controller data? (hint for visualization; relevant for interface especially)
  NETWORK_ELEMENT,         //!< Is this a network port or framework element?
  GLOBALLY_UNIQUE_LINK,    //!< Has this framework element a globally unique path? (may be used by tools such as fingui to decide whether to store host name with connection)
  ALTERNATIVE_LOCAL_URI_ROOT, //!< Is this an alternative root for local URIs (such as a remote runtime environments mapped into this one)

  RUNTIME,                 //!< Non-port use: Is this the one and only runtime environment (in this process)? Port use: flag hijacked ports (see below).
  SHARED,                  //!< Should ports below this framework element be visible/accessible from other runtime environments?
  AUTO_RENAME,             //!< Automatically rename children with duplicate names?

  // Interface classification flags (note that SENSOR_DATA and CONTROLLER_DATA also classify interfaces)
  INTERFACE_FOR_RPC_PORTS,        //!< Can interface contain RPC ports?
  INTERFACE_FOR_DATA_PORTS,       //!< Can interface contain data ports?
  INTERFACE_FOR_INPUTS,           //!< Can interface contain input ports?
  INTERFACE_FOR_OUTPUTS,          //!< Can interface contain output ports?
  PROXY_INTERFACE,                //!< Is this an interface of a composite component? (-> will contain proxy ports intead of pure output and input ports)
  PARAMETER_INTERFACE,            //!< Is this an interface for parameters?

  UNUSED_CONSTANT_NON_PORT_FLAG_6_USED_IN_JAVA,
  UNUSED_CONSTANT_NON_PORT_FLAG_5,
  UNUSED_CONSTANT_NON_PORT_FLAG_4,
  UNUSED_CONSTANT_NON_PORT_FLAG_3,
  UNUSED_CONSTANT_NON_PORT_FLAG_2,
  UNUSED_CONSTANT_NON_PORT_FLAG_1,

  // Non-constant flags - need to be changed synchronously
  READY,                   //!< Is framework element ready?
  PUBLISHED,               //!< Has framework element been published?
  DELETED,                 //!< Has framework element been deleted? - dangerous if you actually encounter this in C++...
  FINSTRUCTED,             //!< Is this an element created by finstruct?
  FINSTRUCTABLE_GROUP,     //!< Is this a finstructable group?

  MANAGES_PARAMETER_CONFIGURATION,  //!< Whether this (finstructable group) component stores and loads configuration of parameters below (config file links in particular)
  UNUSED_DYNAMIC_NON_PORT_FLAG_3,
  UNUSED_DYNAMIC_NON_PORT_FLAG_2,
  UNUSED_DYNAMIC_NON_PORT_FLAG_1,

  // Port flags
  // Constant flags
  UNUSED_CONSTANT_PORT_FLAG_4 = EDGE_AGGREGATOR,
  UNUSED_CONSTANT_PORT_FLAG_3,
  SENSOR_DATA_PORT,        //!< Is the data processed in this port sensor data? (hint for visualization; relevant for interface especially)
  CONTROLLER_DATA_PORT,    //!< Is the data processed in this port controller data? (hint for visualization; relevant for interface especially)
  NETWORK_PORT,            //!< Is this a network port?
  GLOBALLY_UNIQUE_PATH,    //!< Has this port a globally unique path? (may be used by tools such as fingui to decide whether to store host name with connection)
  UNUSED_CONSTANT_PORT_FLAG_2,

  HIJACKED_PORT,           //!< Is this a port hijacked e.g. by the data_playback plugin? If set, values set/received by components are discarded.
  SHARED_PORT = SHARED,    //!< Should port be visible/accessible from other runtime environments?
  UNUSED_CONSTANT_PORT_FLAG_1,

  HAS_QUEUE,               //!< Does Port have a queue for storing incoming data?
  HAS_DEQUEUE_ALL_QUEUE,   //!< Does Port have a queue with tDequeueMode::ALL instead of tDequeueMode::FIFO?
  ACCEPTS_DATA,            //!< Does port accept incoming data? Also set for server RPC ports, since they accept RPC calls
  EMITS_DATA,              //!< Does port emit data (normal direction)? Also set for client RPC ports, since they "emit" RPC calls
  MULTI_TYPE_BUFFER_POOL,  //!< Does port have buffer pool with multiple data types?
  EXPRESS_PORT,            //!< Transport data for this port through the network with high priority? */
  VOLATILE,                //!< Is this port volatile (meaning that it's not always there and connections to it should preferably be links) */
  NO_INITIAL_PUSHING,      //!< Deactivates initial pushing when this (output) port is connected to another port with push strategy */
  TOOL_PORT,               //!< Port from network connection from tooling (e.g. finstruct/fingui). Connection constraints are ignored for such ports
  FINSTRUCT_READ_ONLY,     //!< Port value cannot be set from finstruct

  /*!
   * From it's general characteristics: Is port input or output port?
   * (for ports in interfaces of composite components: from the outside component view)
   * (for RPC ports client ports are output port since they "emit" RPC calls)
   */
  OUTPUT_PORT,

  /*!
   * Are port values assigned to port in a non-standard way?
   * In this case, a port's virtual method NonStandardAssign() is used instead of plain Assign() in order to change the data buffer of a port.
   * Constant flag automatically set by data port classes.
   */
  NON_STANDARD_ASSIGN,

  // Non-constant flags - need to be changed synchronously
  READY_PORT = READY,             //!< Is framework element ready?
  PUBLISHED_PORT = PUBLISHED,     //!< Has framework element been published?
  DELETED_PORT = DELETED,         //!< Has framework element been deleted? - dangerous if you actually encounter this in C++...
  FINSTRUCTED_PORT = FINSTRUCTED, //!< Is this a port created by finstruct?
  UNUSED_DYNAMIC_PORT_FLAG_1,

  USES_QUEUE,                     //!< Does Port currently store incoming data in queue? - requires HAS_QUEUE
  DEFAULT_ON_DISCONNECT,          //!< Restore default value, if port is disconnected?
  PUSH_STRATEGY,                  //!< Use push strategy rather than pull strategy?
  LEGACY_PUSH_STRATEGY_REVERSE    //!< Unused legacy flag: use push strategy in reverse direction?
};

static_assert(static_cast<uint>(tFrameworkElementFlag::LEGACY_PUSH_STRATEGY_REVERSE) < 32, "Too many flags");
static_assert(static_cast<uint>(tFrameworkElementFlag::LEGACY_PUSH_STRATEGY_REVERSE) == static_cast<uint>(tFrameworkElementFlag::UNUSED_DYNAMIC_NON_PORT_FLAG_1), "The same number of flags should be defined for ports and non-ports");


/*!
 * Set of framework element flags.
 */
typedef rrlib::util::tEnumBasedFlags<tFrameworkElementFlag> tFrameworkElementFlags;


constexpr inline tFrameworkElementFlags operator | (const tFrameworkElementFlags& flags1, const tFrameworkElementFlags& flags2)
{
  return tFrameworkElementFlags(flags1.Raw() | flags2.Raw());
}

constexpr inline tFrameworkElementFlags operator | (tFrameworkElementFlag flag1, tFrameworkElementFlag flag2)
{
  return tFrameworkElementFlags(flag1) | tFrameworkElementFlags(flag2);
}

//----------------------------------------------------------------------
// End of namespace declaration
//----------------------------------------------------------------------
}
}


#endif
