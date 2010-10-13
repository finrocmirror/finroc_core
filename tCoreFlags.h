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

#ifndef CORE__TCOREFLAGS_H
#define CORE__TCOREFLAGS_H

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
class tCoreFlags : public util::tUncopyableObject
{
public:

  /*! Mask for constant flags (first 22 bit) */
  static const int cCONSTANT_FLAGS = 0x3FFFFF;

  /*! Mask for changeable flags (second 9 bit)*/
  static const int cNON_CONSTANT_FLAGS = 0x7FC00000;

  // Constant flags (both ports and non-ports - first 9 bits)

  /*! Is this framework element a port? */
  static const int cIS_PORT = 1 << 0;

  /*! Is this object globally unique - that means: it is reachable in any runtime environment using the same name */
  static const int cGLOBALLY_UNIQUE_LINK = 1 << 1;

  /*! Is this a finstructable group? */
  static const int cFINSTRUCTABLE_GROUP = 1 << 2;

  /*! Is this the one and only Runtime environment? */
  static const int cIS_RUNTIME = 1 << 3;

  /*! Is this an edge aggregating framework element? */
  static const int cEDGE_AGGREGATOR = 1 << 4;

  /*! Is this an alternate root for links to globally unique objects (such as a remote runtime mapped into this one) */
  static const int cALTERNATE_LINK_ROOT = 1 << 5;

  /*! Is this a network port or framework element? */
  static const int cNETWORK_ELEMENT = 1 << 6;

  /*! Can framework element have children - typically true */
  static const int cALLOWS_CHILDREN = 1 << 7;

  /*! Should framework element be visible/available in other RuntimeEnvironments? - (TreeFilter specified by other runtime may override this) */
  static const int cSHARED = 1 << 8;

  /*! First flag whose meaning differs between ports and non-ports */
  static const int cFIRST_PORT_FLAG = 1 << 9;

  // Non-port constant flags (second 8 bit)

  /*! Automatically rename children with duplicate names? */
  static const int cAUTO_RENAME = cFIRST_PORT_FLAG << 0;

  /*! Non-port subclass may use flags beginning from this */
  static const int cFIRST_CUSTOM_CONST_FLAG = cFIRST_PORT_FLAG << 1;

  // non-constant flags - need to be changed synchronously

  // State-related (automatically set) - if none is set it means it is constructing
  /*! Is framework element ready? */
  static const int cREADY = 1 << 22;

  /*! Has framework element been published? */
  static const int cPUBLISHED = 1 << 23;

  /*! Has framework element been deleted? - dangerous if you actually encounter this in C++... */
  static const int cDELETED = 1 << 24;

  /*! Is this an element created by finstruct? */
  static const int cFINSTRUCTED = 1 << 25;

  /*! Client may use flags beginning from this */
  static const int cFIRST_CUSTOM_NON_CONST_FLAG = 1 << 26;

  /*! All status flags */
  static const int cSTATUS_FLAGS = cREADY | cPUBLISHED | cDELETED;

  tCoreFlags() {}
};

} // namespace finroc
} // namespace core

#endif // CORE__TCOREFLAGS_H
