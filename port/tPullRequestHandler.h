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

#ifndef core__port__std__tPullRequestHandler_h__
#define core__port__std__tPullRequestHandler_h__

#include "core/port/std/tPullRequestHandlerRaw.h"
#include "core/port/cc/tCCPullRequestHandlerRaw.h"

namespace finroc
{
namespace core
{

/*!
 * \author Max Reichardt
 *
 * Can be used to handle pull requests of - typically - output ports
 */
template <typename T, bool CC = typeutil::tIsCCType<T>::value>
class tPullRequestHandler : public tPullRequestHandlerRaw
{

public:

  /*!
   * Called whenever a pull request is intercepted
   *
   * \param origin (Output) Port pull request comes from
   * \param result Buffer to store result in
   * \return Was buffer filled? - return false if pull should be handled by port (now)
   */
  virtual bool PullRequest(const tPort<T>& origin, T& result) = 0;

  virtual const tPortDataManager* PullRequest(tPortBase* origin, int8 add_locks, bool intermediate_assign)
  {
    tPortDataManager* mgr = origin->GetUnusedBufferRaw();
    if (PullRequest(tPort<T>(*origin), mgr->GetObject()->GetData<T>()))
    {
      mgr->GetCurrentRefCounter()->SetOrAddLocks(add_locks);
      return mgr;
    }
    mgr->DangerousDirectRecycle();
    return NULL;
  }
};

template <typename T>
class tPullRequestHandler<T, false> : public tCCPullRequestHandlerRaw
{
public:

  /*!
   * Called whenever a pull request is intercepted
   *
   * \param origin (Output) Port pull request comes from
   * \param result Buffer to store result in
   * \return Was buffer filled? - return false if pull should be handled by port (now)
   */
  virtual bool PullRequest(const tPort<T>& origin, T& result) = 0;

  virtual bool PullRequest(tCCPortBase* origin, tCCPortDataManagerTL* result_buffer, bool intermediate_assign)
  {
    return PullRequest(tPort<T>(*origin), result_buffer->GetObject()->GetData<T>());
  }

};

} // namespace finroc
} // namespace core

#endif // core__port__std__tPullRequestHandler_h__
