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

#ifndef CORE__PORT__CC__TPORTNUMERICBOUNDED_H
#define CORE__PORT__CC__TPORTNUMERICBOUNDED_H

#include "core/port/tPortCreationInfo.h"
#include "core/datatype/tBounds.h"
#include "core/port/cc/tPortNumeric.h"

namespace finroc
{
namespace core
{
class tUnit;
class tThreadLocalCache;

/*!
 * \author Max Reichardt
 *
 * Number port with upper and lower bounds for values
 */
class tPortNumericBounded : public tPortNumeric
{
protected:

  /*! Special Port class to load value when initialized */
  class tPortImpl : public tPortNumeric::tPortImplNum
  {
    friend class tPortNumericBounded;
  private:

    /*! Bounds of this port */
    tBounds bounds;

  protected:

    virtual void NonStandardAssign(tThreadLocalCache* tc);

  public:

    tPortImpl(tPortCreationInfo pci, tBounds b, tUnit* u);

  };

  /*!
   * Make sure non-standard assign flag is set
   */
  static tPortCreationInfo ProcessPciBNP(tPortCreationInfo pci);

protected:

  /*!
   * (Constructor for subclasses with own port class)
   */
  tPortNumericBounded();

public:

  /*!
   * \param pci Port Creation info
   * \param b Bounds for this port
   */
  tPortNumericBounded(tPortCreationInfo pci, tBounds b);

  /*!
   * \return the bounds of this port
   */
  inline tBounds GetBounds() const
  {
    return (static_cast<tPortImpl*>(this->wrapped))->bounds;
  }

  /*!
   * Set Bounds
   * (This is not thread-safe and must only be done in "pause mode")
   *
   * \param bounds2 new Bounds for this port
   */
  void SetBounds(const tBounds& bounds2);

};

} // namespace finroc
} // namespace core

#endif // CORE__PORT__CC__TPORTNUMERICBOUNDED_H
