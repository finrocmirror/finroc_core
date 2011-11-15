/**
 * You received this file as part of an advanced experimental
 * robotics framework prototype ('finroc')
 *
 * Copyright (C) 2011 Max Reichardt,
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

#ifndef core__datatype__tXML_h__
#define core__datatype__tXML_h__

#include "rrlib/finroc_core_utils/definitions.h"

#include "core/datatype/tCoreString.h"

namespace finroc
{
namespace core
{
/*!
 * \author Max Reichardt
 *
 * Custom XML data.
 * Special XML types may become subclasses of this.
 */
class tXML : public tCoreString
{

public:

  /*! Data Type */
  static rrlib::serialization::tDataTypeBase cTYPE;

  tXML();

  virtual void Deserialize(const rrlib::xml2::tXMLNode& node);

  virtual void Serialize(rrlib::xml2::tXMLNode& node) const;

  /*!
   * \param s String to set buffer to
   */
  inline void Set(const util::tString& s)
  {
    tCoreString::Set(s);
  }

  /*!
   * \param n Node to set buffer to
   */
  void Set(const rrlib::xml2::tXMLNode& n);
};

} // namespace finroc
} // namespace core

#include "rrlib/serialization/tDataType.h"

extern template class rrlib::serialization::tDataType<finroc::core::tXML>;

#endif // core__datatype__tCoreString_h__
