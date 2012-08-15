/**
 * You received this file as part of an advanced experimental
 * robotics framework prototype ('finroc')
 *
 * Copyright (C) 2012 Max Reichardt,
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

#ifndef core__port__rpc__tThreadLocalRPCData_h__
#define core__port__rpc__tThreadLocalRPCData_h__

#include "core/tFinrocAnnotation.h"
#include "core/port/tThreadLocalCache.h"

#include "core/port/rpc/tMethodCall.h"
#include "core/port/rpc/tPullCall.h"

namespace finroc
{
namespace core
{
class tMethodCallSyncher;

/*!
 * \author Max Reichardt
 *
 * Various thread-local objects used for RPC calls over the net
 */
class tThreadLocalRPCData : public tFinrocAnnotation
{
  /*! object to help synchronize method calls - lazily initialized */
  tMethodCallSyncher* method_syncher;

  /*! Reusable objects representing a method call */
  util::tReusablesPool<tMethodCall>* method_calls;

  /*! Reusable objects representing a pull call */
  util::tReusablesPool<tPullCall>* pull_calls;

  /*!
   * Is this a suitable thread for synchronous calls?
   * If not, performing a synchronous call will print a warning to the console.
   * An unsuitable thread, for instance, is the thread that would read the call result from a network stream.
   */
  bool is_suitable_thread_for_synchronous_calls;


  tMethodCall* CreateMethodCall();
  tMethodCallSyncher* CreateMethodSyncher();
  tPullCall* CreatePullCall();

public:

  tThreadLocalRPCData();

  virtual ~tThreadLocalRPCData();

  /*!
   * \return Reference to tThreadLocalRPCData for this thread (creates it if necessary)
   */
  inline static tThreadLocalRPCData& Get()
  {
    tThreadLocalRPCData* result = tThreadLocalCache::Get()->GetAnnotation<tThreadLocalRPCData>();
    if (!result)
    {
      result = new tThreadLocalRPCData();
    }
    return *result;
  }

  tMethodCallSyncher& GetMethodSyncher()
  {
    if (!method_syncher)
    {
      method_syncher = CreateMethodSyncher();
    }
    return *method_syncher;
  }

  inline tMethodCall::tPtr GetUnusedMethodCall()
  {
    tMethodCall* pf = method_calls->GetUnused();
    if (!pf)
    {
      pf = CreateMethodCall();
    }
    //pf.responsibleThread = ThreadUtil.getCurrentThreadId();
    return tMethodCall::tPtr(pf);
  }

  inline tPullCall::tPtr GetUnusedPullCall()
  {
    tPullCall* pf = pull_calls->GetUnused();
    if (!pf)
    {
      pf = CreatePullCall();
    }
    return tPullCall::tPtr(pf);
  }

  /*!
   * \return Is this a suitable thread for synchronous calls?
   */
  inline bool IsSuitableThreadForSynchronousCalls() const
  {
    return is_suitable_thread_for_synchronous_calls;
  }

  /*!
   * \param value Is this a suitable thread for synchronous calls?
   */
  void SetSuitableThreadForSynchronousCalls(bool value)
  {
    is_suitable_thread_for_synchronous_calls = value;
  }
};

} // namespace finroc
} // namespace core

#endif // core__port__rpc__tThreadLocalRPCData_h__
