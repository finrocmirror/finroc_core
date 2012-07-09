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

#ifndef core__tCoreFlags_h__
#define core__tCoreFlags_h__

#include "rrlib/finroc_core_utils/definitions.h"

namespace finroc
{
namespace core
{
/*!
 * \author Max Reichardt
 *
 * This class contains flags which are relevant for
 * framework elements.
 * The lower 22 bit are constant flags which may not change
 * at runtime whereas the upper 9 may change
 * (we omit the last bit, because the sign may have ugly side effects)
 * Any free flags may be used and accessed by subclasses (using
 * the protected constFlags and flags members).
 *
 * Using flags instead of variables saves a lot of memory.
 */
struct tCoreFlags
{

  /*! Mask for constant flags (first 22 bit) */
  static const uint cCONSTANT_FLAGS = 0x3FFFFF;

  /*! Mask for changeable flags (second 9 bit)*/
  static const uint cNON_CONSTANT_FLAGS = 0x7FC00000;

  // Constant flags (both ports and non-ports - first 8 bits)

  /*! Is this framework element a port? */
  static const uint cIS_PORT = 1 << 0;

  /*! Is this object globally unique - that means: it is reachable in any runtime environment using the same name */
  static const uint cGLOBALLY_UNIQUE_LINK = 1 << 1;

  /*! Is this the one and only Runtime environment? */
  static const uint cIS_RUNTIME = 1 << 2;

  /*! Is this an edge aggregating framework element? */
  static const uint cEDGE_AGGREGATOR = 1 << 3;

  /*! Is this an alternate root for links to globally unique objects (such as a remote runtime mapped into this one) */
  static const uint cALTERNATE_LINK_ROOT = 1 << 4;

  /*! Is this a network port or framework element? */
  static const uint cNETWORK_ELEMENT = 1 << 5;

  /*! Can framework element have children - typically true */
  static const uint cALLOWS_CHILDREN = 1 << 6;

  /*! Should framework element be visible/available in other RuntimeEnvironments? - (TreeFilter specified by other runtime may override this) */
  static const uint cSHARED = 1 << 7;

  /*! First flag whose meaning differs between ports and non-ports */
  static const uint cFIRST_PORT_FLAG = 1 << 8;

  // Non-port constant flags

  /*! Automatically rename children with duplicate names? */
  static const uint cAUTO_RENAME = cFIRST_PORT_FLAG << 0;

  /*! Non-port subclass may use flags beginning from this */
  static const uint cFIRST_CUSTOM_CONST_FLAG = cFIRST_PORT_FLAG << 1;

  // non-constant flags - need to be changed synchronously

  // State-related (automatically set) - if none is set it means it is constructing
  /*! Is framework element ready? */
  static const uint cREADY = 1 << 22;

  /*! Has framework element been published? */
  static const uint cPUBLISHED = 1 << 23;

  /*! Has framework element been deleted? - dangerous if you actually encounter this in C++... */
  static const uint cDELETED = 1 << 24;

  /*! Is this an element created by finstruct? */
  static const uint cFINSTRUCTED = 1 << 25;

  /*! Is this a finstructable group? */
  static const uint cFINSTRUCTABLE_GROUP = 1 << 26;

  /*! Client may use flags beginning from this */
  static const uint cFIRST_CUSTOM_NON_CONST_FLAG = 1 << 27;

  /*! All status flags */
  static const uint cSTATUS_FLAGS = cREADY | cPUBLISHED | cDELETED;
};

} // namespace finroc
} // namespace core

#endif // core__tCoreFlags_h__
