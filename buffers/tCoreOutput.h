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

#ifndef CORE__BUFFERS__TCOREOUTPUT_H
#define CORE__BUFFERS__TCOREOUTPUT_H

#include "core/portdatabase/tDataType.h"
#include "finroc_core_utils/stream/tSink.h"
#include "core/portdatabase/tTypedObjectImpl.h"
#include "finroc_core_utils/stream/tOutputStreamBuffer.h"

namespace finroc
{
namespace core
{
class tCoreInput;

/*!
 * \author Max Reichardt
 *
 * This is a specialized version of the StreamBuffer that is used
 * throughout the framework
 */
class tCoreOutput : public util::tOutputStreamBuffer
{
public:

  tCoreOutput();

  tCoreOutput(::std::tr1::shared_ptr<util::tSink> sink);

  tCoreOutput(util::tSink* sink);

  /*!
   * Serialize Object of arbitrary type to stream
   *
   * \param to Object to write (may be null)
   */
  void WriteObject(const tTypedObject* to);

  inline void WriteType(tDataType* data_type)
  {
    WriteShort(data_type == NULL ? -1 : data_type->GetUid());
  }

//   tCoreInput GetInputStream(bool reset);
//
//  @InCpp("return CoreInput(this);") @InCppFile @ConstMethod
//  public CoreInput getInputStream(boolean reset) {
//    return (CoreInput)getReadView(reset);
//  }
//
//  @JavaOnly
//  protected InputStreamBuffer createReadView() {
//    return new CoreInput(this);
//  }
//
//  // constructors from super class...

};

} // namespace finroc
} // namespace core

#endif // CORE__BUFFERS__TCOREOUTPUT_H
