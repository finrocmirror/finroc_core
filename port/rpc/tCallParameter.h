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

#ifndef CORE__PORT__RPC__TCALLPARAMETER_H
#define CORE__PORT__RPC__TCALLPARAMETER_H

#include "core/port/cc/tCCInterThreadContainer.h"

namespace finroc
{
namespace core
{
/*!
 * Storage for a parameter
 *
 * CC Objects: If call is executed in the same runtime environment, object is stored inside
 * otherwise it is directly serialized
 */
class tCallParameter : public util::tObject
{
  friend class tAbstractCall;

public:

  /*! Constants for different types of parameters in serialization */
  static const int8 cNULLPARAM = 0, cINT = 1, cLONG = 2, cFLOAT = 3, cDOUBLE = 4, cPORTDATA = 5, cCCDATA = 6, cCCCONTAINER = 7, cBYTE = 8, cSHORT = 9;

  /*! Parameter */
  union
  {
    int ival;
    int64 lval;
    float fval;
    double dval;
    int8 bval;
    int16 sval;
    tCCInterThreadContainer<>* ccval;
    const tPortData* value;
  };

  /*! Type of parameter (see constants at beginning of class) */
  int8 type;

  tCallParameter();

  inline void Clear()
  {
    type = cNULLPARAM;
    value = NULL;
  }

  void Recycle();

};

} // namespace finroc
} // namespace core

#endif // CORE__PORT__RPC__TCALLPARAMETER_H
