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

#ifndef core__buffers__tCoreOutput_h__
#define core__buffers__tCoreOutput_h__

#include "rrlib/finroc_core_utils/definitions.h"

#include "rrlib/serialization/tSink.h"
#include "rrlib/serialization/tOutputStream.h"

namespace rrlib
{
namespace serialization
{
class tGenericObject;
} // namespace rrlib
} // namespace serialization

namespace finroc
{
namespace core
{
/*!
 * \author Max Reichardt
 *
 * This is a specialized version of the StreamBuffer that is used
 * throughout the framework
 */
class tCoreOutput : public rrlib::serialization::tOutputStream
{
public:

  tCoreOutput();

  tCoreOutput(::std::shared_ptr<rrlib::serialization::tSink> sink);

  tCoreOutput(rrlib::serialization::tSink* sink);

  /*!
   * Serialize Object of arbitrary type to stream
   *
   * \param to Object to write (may be null)
   */
  void WriteObject(const rrlib::serialization::tGenericObject* to);

};

} // namespace finroc
} // namespace core

#endif // core__buffers__tCoreOutput_h__
