/**
 * You received this file as part of an advanced experimental
 * robotics framework prototype ('finroc')
 *
 * Copyright (C) 2010-2011 Max Reichardt,
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
#ifndef core__port__cc__tCCPortBounded_h__
#define core__port__cc__tCCPortBounded_h__

#include "rrlib/finroc_core_utils/definitions.h"
#include "rrlib/rtti/rtti.h"

#include "core/datatype/tBounds.h"
#include "core/datatype/tNumber.h"
#include "core/port/tPortCreationInfo.h"
#include "core/port/cc/tCCPortDataManagerTL.h"
#include "core/port/cc/tCCPortDataRef.h"
#include "core/port/tThreadLocalCache.h"
#include "core/port/tPortFlags.h"
#include "core/port/cc/tCCPortDataManager.h"
#include "core/tFrameworkElement.h"
#include "core/port/cc/tCCPortBase.h"
#include "core/port/tAbstractPort.h"

namespace finroc
{
namespace core
{
/*!
 * \author Max Reichardt
 *
 * Port with upper and lower bounds for values
 */
template<typename T>
class tCCPortBounded : public tCCPortBase
{
private:

  /*! Bounds of this port */
  tBounds<T> bounds;

  /*!
   * Make sure non-standard assign flag is set
   */
  inline static tPortCreationInfoBase ProcessPciBNP(tPortCreationInfo<T> pci)
  {
    pci.flags = pci.flags | tPortFlags::cNON_STANDARD_ASSIGN;
    return pci;
  }

protected:

  virtual void NonStandardAssign(tThreadLocalCache* tc)
  {
    const T* val = tc->data->GetObject()->GetData<T>();
    if (!bounds.InBounds(*val))
    {
      if (tc->ref->GetContainer()->GetRefCounter() == 0)    // still unused
      {
        FINROC_LOG_PRINT(DEBUG_WARNING, "Attempt to publish value that is out-of-bounds of output (!) port. This is undesirable.");
        tc->ref->GetContainer()->RecycleUnused();
      }
      if (bounds.Discard())
      {
        tc->ref = this->value;
        tc->data = tc->ref->GetContainer();
      }
      else if (bounds.AdjustToRange())
      {
        tCCPortDataManagerTL* container = ::finroc::core::tCCPortBase::GetUnusedBuffer(tc);
        T* cnc = container->GetObject()->GetData<T>();
        tc->data = container;
        tc->ref = container->GetCurrentRef();
        rrlib::rtti::sStaticTypeInfo<T>::DeepCopy(bounds.ToBounds(*val), *cnc);
      }
      else if (bounds.ApplyDefault())
      {
        tc->data = tc->GetUnusedBuffer(tNumber::cTYPE);
        T* cnc = tc->data->GetObject()->GetData<T>();
        tc->ref = tc->data->GetCurrentRef();
        rrlib::rtti::sStaticTypeInfo<T>::DeepCopy(bounds.GetOutOfBoundsDefault(), *cnc);
        tc->data->SetRefCounter(0);  // locks will be added during assign
      }
    }
    //super.assign(tc); done anyway
  }

  /*!
   * Generates error message for BrowserPublishRaw
   *
   * \param current_vale Value to generate out of bounds error message for
   * \return Error message that value is out of bounds
   */
  template < bool ENABLE = rrlib::serialization::tIsStringSerializable<T>::value>
  std::string GenerateErrorMessage(const typename std::enable_if<ENABLE, T>::type& current_value)
  {
    rrlib::serialization::tStringOutputStream sos;
    sos << "Value " << current_value << " is out of bounds [" << bounds.GetMin() << "; " << bounds.GetMax() << "]";
    return sos.ToString();
  }
  template < bool ENABLE = rrlib::serialization::tIsStringSerializable<T>::value>
  std::string GenerateErrorMessage(const typename std::enable_if < !ENABLE, T >::type& current_value)
  {
    return "Value is out of bounds";
  }

public:

  /*!
   * \param pci Construction parameters in Port Creation Info Object
   */
  tCCPortBounded(tPortCreationInfo<T> pci) :
    tCCPortBase(ProcessPciBNP(pci)),
    bounds(pci.GetBounds())
  {
  }

  virtual std::string BrowserPublishRaw(tCCPortDataManagerTL* buffer)
  {
    if (buffer->GetObject()->GetType() != GetDataType())
    {
      return "Buffer has wrong type";
    }
    T val = *buffer->GetObject()->GetData<T>();
    if (!bounds.InBounds(val))
    {
      return GenerateErrorMessage(val);
    }
    return tCCPortBase::BrowserPublishRaw(buffer);
  }

  /*!
   * \return the bounds of this port
   */
  inline tBounds<T> GetBounds() const
  {
    return bounds;
  }

  /*!
   * Set Bounds
   * (This is not thread-safe and must only be done in "pause mode")
   *
   * \param bounds2 new Bounds for this port
   */
  inline void SetBounds(const tBounds<T>& bounds2)
  {
    bounds.Set(bounds2);
    tCCPortDataManager* mgr = ::finroc::core::tCCPortBase::GetInInterThreadContainer();
    const T* val = mgr->GetObject()->GetData<T>();
    if (!bounds.InBounds(*val))
    {
      if (bounds.Discard())
      {
        FINROC_LOG_PRINT(WARNING, "Cannot discard value - applying default");
        ApplyDefaultValue();
      }
      else if (bounds.AdjustToRange())
      {
        tCCPortDataManagerTL* buf = tThreadLocalCache::GetFast()->GetUnusedBuffer(GetDataType());
        T* cnc = buf->GetObject()->GetData<T>();
        rrlib::rtti::sStaticTypeInfo<T>::DeepCopy(bounds.ToBounds(*val), *cnc);
        ::finroc::core::tCCPortBase::Publish(buf);
      }
      else if (bounds.ApplyDefault())
      {
        ApplyDefaultValue();
      }
    }
    mgr->Recycle2();
  }

};

} // namespace finroc
} // namespace core

#endif // core__port__cc__tCCPortBounded_h__
