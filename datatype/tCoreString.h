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

#ifndef CORE__DATATYPE__TCORESTRING_H
#define CORE__DATATYPE__TCORESTRING_H

#include "core/buffers/tCoreInput.h"
#include "core/buffers/tCoreOutput.h"
#include "core/port/std/tPortDataImpl.h"

namespace finroc
{
namespace core
{
class tDataType;

/*!
 * \author Max Reichardt
 *
 * Simple string (buffer) type to use in ports
 * Has 512 bytes initially.
 */
class tCoreString : public tPortData
{
private:

  /*! String buffer */
  util::tStringBuilder buffer;

public:

  /*! Data Type */
  static tDataType* cTYPE;

  tCoreString();

  virtual void Deserialize(tCoreInput& is)
  {
    is.ReadString(buffer);
  }

  virtual void Deserialize(const util::tString& s)
  {
    Set(s);
  }

  /*!
   * Copy contents to provided StringBuilder
   *
   * \param sb StringBuilder that will contain result
   */
  inline void Get(util::tStringBuilder& sb)
  {
    sb.Delete(0, sb.Length());
    sb.Append(buffer);
  }

  /*!
   * \return String buffer
   */
  inline util::tStringBuilder& GetBuffer()
  {
    return buffer;
  }

  virtual void Serialize(tCoreOutput& os) const
  {
    os.WriteString(buffer);
  }

  virtual util::tString Serialize() const
  {
    return buffer.ToString();
  }

  /*!
   * \param s String to set buffer to
   */
  inline void Set(const util::tString& s)
  {
    buffer.Delete(0, buffer.Length());
    buffer.Append(s);
  }

  virtual const util::tString ToString() const
  {
    return buffer.ToString();
  }

};

} // namespace finroc
} // namespace core

#endif // CORE__DATATYPE__TCORESTRING_H
