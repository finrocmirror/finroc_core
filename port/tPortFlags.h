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

#ifndef core__port__tPortFlags_h__
#define core__port__tPortFlags_h__

#include "rrlib/finroc_core_utils/definitions.h"

#include "core/tCoreFlags.h"

namespace finroc
{
namespace core
{
/*!
 * \author Max Reichardt
 *
 * Contains Flags for Ports.
 */
class tPortFlags : public tCoreFlags
{
public:

  // Constant flags

  /*! Does Port have a queue for storing incoming data? */
  static const uint cHAS_QUEUE = cFIRST_PORT_FLAG;

  /*! Push data immediately after port creation... mostly makes sense for singleton ports */
  static const uint cPUSH_DATA_IMMEDIATELY = cFIRST_PORT_FLAG << 1;

  /*! Does the flag ACCEPTS_REVERSE_DATA may change at runtime? */
  static const uint cMAY_ACCEPT_REVERSE_DATA = cFIRST_PORT_FLAG << 2;

  /*! Does port accept incoming data? - fixed */
  static const uint cACCEPTS_DATA = cFIRST_PORT_FLAG << 3;

  /*! Does port emit data (normal direction)? - fixed */
  static const uint cEMITS_DATA = cFIRST_PORT_FLAG << 4;

  /*! From it's general characteristics... is port input or output port? (for proxies from the outside group view) - fixed */
  static const uint cIS_OUTPUT_PORT = cFIRST_PORT_FLAG << 5;

  /*! Transport data for this port through the network with low priority */
  static const uint cIS_BULK_PORT = cFIRST_PORT_FLAG << 6;

  /*! Does port have special ReuseQueue? - fixed, set (not unset!) automatically */
  static const uint cSPECIAL_REUSE_QUEUE = cFIRST_PORT_FLAG << 7;

  /*!
   * Are port value assigned to ports in a non-standard way?... In this case
   * the virtual method nonStandardAssign is called in several port methods
   * instead of standardAssign.
   * Fixed, set automatically by port classes
   */
  static const uint cNON_STANDARD_ASSIGN = cFIRST_PORT_FLAG << 8;

  /*! Transport data for this port through the network with high priority */
  static const uint cIS_EXPRESS_PORT = cFIRST_PORT_FLAG << 9;

  /*! Is this port volatile (meaning that it's not always there and connections to it should preferably be links) */
  static const uint cIS_VOLATILE = cFIRST_PORT_FLAG << 10;

  /*! First custom flag for special port types */
  static const uint cFIRST_CUSTOM_PORT_FLAG = cFIRST_PORT_FLAG << 11;

  // Non-constant flags

  /*! Does Port currently store incoming data in queue? - changeable - requires HAS_QUEUE */
  static const uint cUSES_QUEUE = cFIRST_CUSTOM_NON_CONST_FLAG;

  /*! Restore default value, if port is disconnected? - changeable */
  static const uint cDEFAULT_ON_DISCONNECT = cFIRST_CUSTOM_NON_CONST_FLAG << 1;

  /*! Use push strategy rather than pull strategy? - changeable */
  static const uint cPUSH_STRATEGY = cFIRST_CUSTOM_NON_CONST_FLAG << 2;

  /*! Use push strategy rather than pull strategy in reverse direction? - changeable */
  static const uint cPUSH_STRATEGY_REVERSE = cFIRST_CUSTOM_NON_CONST_FLAG << 3;

  ////////// Derived Flags ////////

  /*! Does port have copy queue? */
  static const uint cHAS_AND_USES_QUEUE = cHAS_QUEUE | cUSES_QUEUE;

  /*! Does port accept reverse incoming data? */
  static const uint cACCEPTS_REVERSE_DATA_PUSH = cMAY_ACCEPT_REVERSE_DATA | cPUSH_STRATEGY_REVERSE;

  ///////////// Complete flags for types of ports //////////////

  /*! Simple Output Port */
  static const uint cOUTPUT_PORT = cEMITS_DATA | cIS_OUTPUT_PORT;

  static const uint cSHARED_OUTPUT_PORT = cOUTPUT_PORT | cSHARED;

  /*! Simple Input Port */
  static const uint cINPUT_PORT = cPUSH_STRATEGY | cACCEPTS_DATA;

  static const uint cSHARED_INPUT_PORT = cINPUT_PORT | cSHARED;

  static const uint cPROXY = cEMITS_DATA | cACCEPTS_DATA;

  /*! Simple Proxy Port */
  static const uint cOUTPUT_PROXY = cPROXY | cPUSH_STRATEGY | cIS_OUTPUT_PORT;

  /*! Simple Proxy Port */
  static const uint cINPUT_PROXY = cPROXY | cPUSH_STRATEGY;

  tPortFlags() {}
};

} // namespace finroc
} // namespace core

#endif // core__port__tPortFlags_h__
