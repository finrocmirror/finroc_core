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
#include "finroc_core_utils/tJCBase.h"

#ifndef CORE__PORT__RPC__TMETHODCALLEXCEPTION_H
#define CORE__PORT__RPC__TMETHODCALLEXCEPTION_H

namespace finroc
{
namespace core
{
/*!
 * \author Max Reichardt
 *
 * Class for any exceptions that can occur during method calls
 */
class tMethodCallException : public util::tException
{
public:

  enum tType { eTIMEOUT, eNO_CONNECTION, eUNKNOWN_METHOD, eINVALID_PARAM, ePROGRAMMING_ERROR };

private:

  tMethodCallException::tType type;

public:

  /*!
   * \param timeout Timeout exception (or rather connection exception)?
   */
  tMethodCallException(tMethodCallException::tType type_) :
      type(type_)
  {
  }

  tMethodCallException(int type2) :
      type()
  {
    this->type = static_cast<tType>(type2);
  }

  inline tMethodCallException::tType GetType() const
  {
    return type;
  }

  inline int8 GetTypeId() const
  {
    return static_cast<int8>(type);
  }

};

} // namespace finroc
} // namespace core

#endif // CORE__PORT__RPC__TMETHODCALLEXCEPTION_H
