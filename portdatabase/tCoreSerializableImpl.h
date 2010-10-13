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

#ifndef CORE__PORTDATABASE__TCORESERIALIZABLEIMPL_H
#define CORE__PORTDATABASE__TCORESERIALIZABLEIMPL_H

namespace rrlib
{
namespace xml2
{
class tXMLNode;
} // namespace rrlib
} // namespace xml2

namespace finroc
{
namespace core
{
class tCoreInput;
class tCoreOutput;

/*!
 * \author Max Reichardt
 *
 * Default implementation of CoreSerializable
 */
class tCoreSerializable
{
public:

  tCoreSerializable() {}

  virtual ~tCoreSerializable() {}

  /*!
   * Deserialize object. Object has to already exists.
   * Should be suited for reusing old objects.
   *
   * \param read_view Stream to deserialize from
   */
  virtual void Deserialize(tCoreInput& is) = 0;

  /*!
   * Deserialize object. Object has to already exists.
   * Should be suited for reusing old objects.
   *
   * Parsing errors should throw an Exception - and set object to
   * sensible (default?) value
   *
   * \param s String to deserialize from
   */
  virtual void Deserialize(const util::tString& s);

  /*!
   * Deserialize from XML Node
   *
   * \param node Node to deserialize from
   */
  virtual void Deserialize(const rrlib::xml2::tXMLNode& node);

  /*!
   * \param os Stream to serialize object to
   */
  virtual void Serialize(tCoreOutput& os) const = 0;

  /*!
   * \return Object serialized as string (e.g. for xml output)
   */
  virtual util::tString Serialize() const;

  /*!
   * Serialize object to XML
   *
   * \param node Empty XML node (name shouldn't be changed)
   */
  virtual void Serialize(rrlib::xml2::tXMLNode& node) const;

};

} // namespace finroc
} // namespace core

#endif // CORE__PORTDATABASE__TCORESERIALIZABLEIMPL_H
