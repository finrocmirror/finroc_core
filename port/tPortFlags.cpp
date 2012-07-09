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
#include "core/port/tPortFlags.h"

namespace finroc
{
namespace core
{
const uint tPortFlags::cHAS_QUEUE;
const uint tPortFlags::cPUSH_DATA_IMMEDIATELY;
const uint tPortFlags::cMAY_ACCEPT_REVERSE_DATA;
const uint tPortFlags::cACCEPTS_DATA;
const uint tPortFlags::cEMITS_DATA;
const uint tPortFlags::cIS_OUTPUT_PORT;
const uint tPortFlags::cIS_BULK_PORT;
const uint tPortFlags::cSPECIAL_REUSE_QUEUE;
const uint tPortFlags::cNON_STANDARD_ASSIGN;
const uint tPortFlags::cIS_EXPRESS_PORT;
const uint tPortFlags::cIS_VOLATILE;
const uint tPortFlags::cNO_INITIAL_PUSHING;
const uint tPortFlags::cFIRST_CUSTOM_PORT_FLAG;
const uint tPortFlags::cUSES_QUEUE;
const uint tPortFlags::cDEFAULT_ON_DISCONNECT;
const uint tPortFlags::cPUSH_STRATEGY;
const uint tPortFlags::cPUSH_STRATEGY_REVERSE;
const uint tPortFlags::cHAS_AND_USES_QUEUE;
const uint tPortFlags::cACCEPTS_REVERSE_DATA_PUSH;
const uint tPortFlags::cOUTPUT_PORT;
const uint tPortFlags::cSHARED_OUTPUT_PORT;
const uint tPortFlags::cINPUT_PORT;
const uint tPortFlags::cSHARED_INPUT_PORT;
const uint tPortFlags::cPROXY;
const uint tPortFlags::cOUTPUT_PROXY;
const uint tPortFlags::cINPUT_PROXY;

static_assert((tPortFlags::cFIRST_CUSTOM_PORT_FLAG & tPortFlags::cCONSTANT_FLAGS) != 0, "Invalid flag configuration");
static_assert((tPortFlags::cPUSH_STRATEGY_REVERSE & tPortFlags::cNON_CONSTANT_FLAGS) != 0, "Invalid flag configuration");

} // namespace finroc
} // namespace core

