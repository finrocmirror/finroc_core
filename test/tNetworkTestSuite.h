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

#ifndef core__test__tNetworkTestSuite_h__
#define core__test__tNetworkTestSuite_h__

#include "rrlib/finroc_core_utils/definitions.h"

#include "core/port/tPort.h"
#include "plugins/blackboard/tBlackboardClient.h"
#include "plugins/blackboard/tBlackboardServer.h"
#include "plugins/blackboard/tSingleBufferedBlackboardServer.h"
#include "core/tRuntimeEnvironment.h"
#include "core/port/tThreadLocalCache.h"
#include "core/port/tPortCreationInfo.h"
#include "core/port/tPortFlags.h"
#include "plugins/blackboard/tBlackboardManager.h"
#include "core/datatype/tNumber.h"
#include "plugins/blackboard/tBlackboardWriteAccess.h"
#include "plugins/blackboard/tBBLockException.h"
#include "plugins/blackboard/tBlackboardReadAccess.h"
#include "rrlib/finroc_core_utils/log/tLogUser.h"
#include "core/port/tPortWrapperBase.h"

namespace finroc
{
namespace core
{
/*!
 * \author Max Reichardt
 *
 *
 */
class tNetworkTestSuite : public util::tLogUser
{
public:

  class tTestStdPort : public tPort<rrlib::serialization::tMemoryBuffer>
  {
  public:

    rrlib::serialization::tOutputStream os;

    rrlib::serialization::tInputStream is;

    template <typename ... ARGS>
    tTestStdPort(const ARGS& ... args) :
      tPort<rrlib::serialization::tMemoryBuffer>(args...),
      os(),
      is()
    {
    }

    inline int GetIntRaw()
    {
      const rrlib::serialization::tMemoryBuffer* mb = GetAutoLocked();
      is.Reset(mb);
      int result = -1;
      if (is.MoreDataAvailable())
      {
        result = is.ReadInt();
      }
      is.Close();
      ReleaseAutoLocks();
      return result;
    }

    inline void Publish(int i)
    {
      tPortDataPtr<rrlib::serialization::tMemoryBuffer> mb = GetUnusedBuffer();
      os.Reset(mb.get());
      os.WriteInt(i);
      os.Close();
      ::finroc::core::tPort<rrlib::serialization::tMemoryBuffer>::Publish(mb);
    }

  };

public:

  static const bool cCC_TESTS = false, cSTD_TESTS = false;

  static const bool cBB_TESTS = true;

  static const bool cPUSH_TESTS = false, cPULL_PUSH_TESTS = false, cREVERSE_PUSH_TESTS = false, cQ_TESTS = false;

  util::tString blackboard_name, partner_blackboard_name;

  static const int16 cPUBLISH_FREQ = 200, cRECV_FREQ = 1000;

  int stop_cycle;

  std::shared_ptr<tPort<int> > cc_push_out, cc_pull_push_out, cc_rev_push_out, cc_rev_push_out_local, cc_qOut;

  std::shared_ptr<tPort<int> > cc_push_in, cc_pull_push_in, cc_rev_push_in, cc_qIn;

  std::shared_ptr<tTestStdPort> std_push_out, std_pull_push_out, std_rev_push_out, std_rev_push_out_local, std_qOut;

  std::shared_ptr<tTestStdPort> std_push_in, std_pull_push_in, std_rev_push_in, std_qIn;

  std::shared_ptr<blackboard::tBlackboardClient<rrlib::serialization::tMemoryBuffer> > bb_client, local_bb_client;

  blackboard::tBlackboardServer<rrlib::serialization::tMemoryBuffer>* bb_server;

  blackboard::tSingleBufferedBlackboardServer<rrlib::serialization::tMemoryBuffer>* sbb_server;

  tNetworkTestSuite(const util::tString& bb_name, const util::tString& partner_bBName, int stop_cycle_) :
    util::tLogUser(),
    blackboard_name(bb_name),
    partner_blackboard_name(partner_bBName),
    stop_cycle(stop_cycle_),
    cc_push_out(),
    cc_pull_push_out(),
    cc_rev_push_out(),
    cc_rev_push_out_local(),
    cc_qOut(),
    cc_push_in(),
    cc_pull_push_in(),
    cc_rev_push_in(),
    cc_qIn(),
    std_push_out(),
    std_pull_push_out(),
    std_rev_push_out(),
    std_rev_push_out_local(),
    std_qOut(),
    std_push_in(),
    std_pull_push_in(),
    std_rev_push_in(),
    std_qIn(),
    bb_client(),
    local_bb_client(),
    bb_server(NULL),
    sbb_server(NULL)
  {
    tRuntimeEnvironment::GetInstance();
    tThreadLocalCache::Get();

    if (cCC_TESTS)
    {
      if (cPUSH_TESTS)
      {
        cc_push_out = std::shared_ptr<tPort<int> >(new tPort<int>("CCPush Output", tPortFlags::cSHARED_OUTPUT_PORT));
        cc_push_in = std::shared_ptr<tPort<int> >(new tPort<int>("CCPush Input", tPortFlags::cSHARED_INPUT_PORT));
        cc_push_in->SetMinNetUpdateInterval(cRECV_FREQ);
      }
      if (cPULL_PUSH_TESTS)
      {
        cc_pull_push_out = std::shared_ptr<tPort<int> >(new tPort<int>("CCPullPush Output", tPortFlags::cSHARED_OUTPUT_PORT));
        cc_pull_push_in = std::shared_ptr<tPort<int> >(new tPort<int>("CCPullPush Input", tPortFlags::cSHARED_INPUT_PORT));
        cc_pull_push_in->SetMinNetUpdateInterval(cRECV_FREQ);
        cc_pull_push_in->SetPushStrategy(false);
      }
      if (cREVERSE_PUSH_TESTS)
      {
        cc_rev_push_out = std::shared_ptr<tPort<int> >(new tPort<int>("CCRevPush Output", tPortFlags::cSHARED_OUTPUT_PORT | tPortFlags::cACCEPTS_REVERSE_DATA_PUSH));
        cc_rev_push_out_local = std::shared_ptr<tPort<int> >(new tPort<int>("CCRevPush Output Local", tPortFlags::cSHARED_OUTPUT_PORT));
        cc_rev_push_in = std::shared_ptr<tPort<int> >(new tPort<int>("CCRevPush Input", tPortFlags::cSHARED_INPUT_PORT));
        cc_rev_push_in->SetMinNetUpdateInterval(cRECV_FREQ);
        cc_rev_push_out_local->ConnectTo(*cc_rev_push_in);
      }
      if (cQ_TESTS)
      {
        cc_qOut = std::shared_ptr<tPort<int> >(new tPort<int>("CCPush Queue Output", tPortFlags::cSHARED_OUTPUT_PORT));
        cc_qIn = std::shared_ptr<tPort<int> >(new tPort<int>("CCPush Queue Input", tPortFlags::cSHARED_INPUT_PORT, 0));
        cc_qIn->SetMinNetUpdateInterval(cRECV_FREQ);
      }
    }
    if (cSTD_TESTS)
    {
      if (cPUSH_TESTS)
      {
        std_push_out = std::shared_ptr<tTestStdPort>(new tTestStdPort("StdPush Output", tPortFlags::cSHARED_OUTPUT_PORT));
        std_push_in = std::shared_ptr<tTestStdPort>(new tTestStdPort("StdPush Input", tPortFlags::cSHARED_INPUT_PORT));
        std_push_in->SetMinNetUpdateInterval(cRECV_FREQ);
      }
      if (cPULL_PUSH_TESTS)
      {
        std_pull_push_out = std::shared_ptr<tTestStdPort>(new tTestStdPort("StdPullPush Output", tPortFlags::cSHARED_OUTPUT_PORT));
        std_pull_push_in = std::shared_ptr<tTestStdPort>(new tTestStdPort("StdPullPush Input", tPortFlags::cSHARED_INPUT_PORT));
        std_pull_push_in->SetMinNetUpdateInterval(cRECV_FREQ);
        std_pull_push_in->SetPushStrategy(false);
      }
      if (cREVERSE_PUSH_TESTS)
      {
        std_rev_push_out = std::shared_ptr<tTestStdPort>(new tTestStdPort("StdRevPush Output", tPortFlags::cSHARED_OUTPUT_PORT | tPortFlags::cACCEPTS_REVERSE_DATA_PUSH));
        std_rev_push_out_local = std::shared_ptr<tTestStdPort>(new tTestStdPort("StdRevPush Output Local", tPortFlags::cSHARED_OUTPUT_PORT));
        std_rev_push_in = std::shared_ptr<tTestStdPort>(new tTestStdPort("StdRevPush Input", tPortFlags::cSHARED_INPUT_PORT));
        std_rev_push_in->SetMinNetUpdateInterval(cRECV_FREQ);
        std_rev_push_out_local->ConnectTo(*std_rev_push_in);
      }
      if (cQ_TESTS)
      {
        std_qOut = std::shared_ptr<tTestStdPort>(new tTestStdPort("StdPush Queue Output", tPortFlags::cSHARED_OUTPUT_PORT));
        std_qIn = std::shared_ptr<tTestStdPort>(new tTestStdPort("StdPush Queue Input", tPortFlags::cSHARED_INPUT_PORT, 0));
        std_qIn->SetMinNetUpdateInterval(cRECV_FREQ);
      }
    }

    if (cBB_TESTS)
    {
      blackboard::tBlackboardManager::GetInstance();
      //Plugins.getInstance().addPlugin(new Blackboard2Plugin());
      //bbServer = new BlackboardServer(blackboardName);
      sbb_server = new blackboard::tSingleBufferedBlackboardServer<rrlib::serialization::tMemoryBuffer>(blackboard_name);
      bb_client = std::shared_ptr<blackboard::tBlackboardClient<rrlib::serialization::tMemoryBuffer> >(new blackboard::tBlackboardClient<rrlib::serialization::tMemoryBuffer>(partner_blackboard_name));
      local_bb_client = std::shared_ptr<blackboard::tBlackboardClient<rrlib::serialization::tMemoryBuffer> >(new blackboard::tBlackboardClient<rrlib::serialization::tMemoryBuffer>(blackboard_name));
    }
  }

  inline void MainLoop()
  {
    // write new values to ports and read input ports
    int i = 0;
    rrlib::serialization::tInputStream is;
    rrlib::serialization::tOutputStream os;

    while (true)
    {
      i++;
      int period_idx = i % 20;

      if (cCC_TESTS)
      {
        if (cPUSH_TESTS)
        {
          // publish value and check whether something has changed
          cc_push_out->Publish(i);
          if (cc_push_in->HasChanged())
          {
            cc_push_in->ResetChanged();
            FINROC_LOG_PRINT(USER, "ccPushIn received: ", cc_push_in->Get());
          }
        }

        if (cPULL_PUSH_TESTS)
        {
          // publish value and pull or check for change
          cc_pull_push_out->Publish(i);

          if (cc_pull_push_in->HasChanged())
          {
            cc_pull_push_in->ResetChanged();
            FINROC_LOG_PRINT(USER, "ccPullPushIn received: ", cc_pull_push_in->Get());
          }

          // do some stuff with push strategy
          if (period_idx == 0)
          {
            cc_pull_push_in->SetPushStrategy(false);
          }
          else if (period_idx == 10)
          {
            cc_pull_push_in->SetPushStrategy(true);
          }
          else if (period_idx < 10 && (period_idx % 3) == 0)
          {
            FINROC_LOG_PRINT(USER, "Pulling ccPullPushIn: ", cc_pull_push_in->Get());
          }
        }

        if (cREVERSE_PUSH_TESTS)
        {
          if (cc_rev_push_in->HasChanged())
          {
            int val = cc_rev_push_in->Get();
            if (val < 0)
            {
              FINROC_LOG_PRINT(USER, "ccRevPushIn received: ", val);
            }
          }

          // publish value and check whether something has changed
          cc_rev_push_out_local->Publish(i);
          if (cc_rev_push_out->HasChanged())
          {
            cc_rev_push_out->ResetChanged();
            FINROC_LOG_PRINT(USER, "ccRevPushOut received: ", cc_rev_push_out->Get());
          }
          if (period_idx == 17)
          {
            cc_rev_push_out->Publish(-i);
          }

        }

        if (cQ_TESTS)
        {
          // publish value and check whether something has changed
          cc_qOut->Publish(i);
          if (cc_qIn->HasChanged())
          {
            cc_qIn->ResetChanged();
            std::ostringstream oss;
            oss << "ccPushIn received: ";
            int cn = 0;
            while (cc_qIn->Dequeue(cn))
            {
              oss << " " << cn;
            }
            tThreadLocalCache::GetFast()->ReleaseAllLocks();
            FINROC_LOG_PRINT(USER, oss.str());
          }
          if (period_idx == 9)
          {
            cc_qIn->SetMinNetUpdateInterval(800);
          }
          else if (period_idx == 19)
          {
            cc_qIn->SetMinNetUpdateInterval(400);
          }
        }
      }
      if (cSTD_TESTS)
      {
        if (cPUSH_TESTS)
        {
          // publish value and check whether something has changed
          std_push_out->Publish(i);
          if (std_push_in->HasChanged())
          {
            std_push_in->ResetChanged();
            FINROC_LOG_PRINT(USER, "stdPushIn received: ", std_push_in->GetIntRaw());
          }
        }

        if (cPULL_PUSH_TESTS)
        {
          // publish value and pull or check for change
          std_pull_push_out->Publish(i);

          if (std_pull_push_in->HasChanged())
          {
            std_pull_push_in->ResetChanged();
            FINROC_LOG_PRINT(USER, "stdPullPushIn received: ", std_pull_push_in->GetIntRaw());
          }

          // do some stuff with push strategy
          if (period_idx == 0)
          {
            std_pull_push_in->SetPushStrategy(false);
          }
          else if (period_idx == 10)
          {
            std_pull_push_in->SetPushStrategy(true);
          }
          else if (period_idx < 10 && (period_idx % 3) == 0)
          {
            FINROC_LOG_PRINT(USER, "Pulling stdPullPushIn: ", std_pull_push_in->GetIntRaw());
          }
        }

        if (cREVERSE_PUSH_TESTS)
        {
          if (std_rev_push_in->HasChanged())
          {
            int val = std_rev_push_in->GetIntRaw();
            if (val < 0)
            {
              FINROC_LOG_PRINT(USER, "stdRevPushIn received: ", val);
            }
          }

          // publish value and check whether something has changed
          std_rev_push_out_local->Publish(i);
          if (std_rev_push_out->HasChanged())
          {
            std_rev_push_out->ResetChanged();
            FINROC_LOG_PRINT(USER, "stdRevPushOut received: ", std_rev_push_out->GetIntRaw());
          }
          if (period_idx == 17)
          {
            std_rev_push_out->Publish(-i);
          }
        }

        if (cQ_TESTS)
        {
          // publish value and check whether something has changed
          std_qOut->Publish(i);
          if (std_qIn->HasChanged())
          {
            std_qIn->ResetChanged();
            std::ostringstream oss;
            oss << "stdPushIn received: ";
            const rrlib::serialization::tMemoryBuffer* cn = NULL;
            while ((cn = std_qIn->DequeueAutoLocked()) != NULL)
            {
              is.Reset(cn);
              int result = -1;
              if (is.MoreDataAvailable())
              {
                result = is.ReadInt();
              }
              oss << " " << result;
              is.Close();
            }
            tThreadLocalCache::GetFast()->ReleaseAllLocks();
            FINROC_LOG_PRINT(USER, oss.str());
          }
          if (period_idx == 9)
          {
            std_qIn->SetMinNetUpdateInterval(800);
          }
          else if (period_idx == 19)
          {
            std_qIn->SetMinNetUpdateInterval(400);
          }
        }
      }

      if (cBB_TESTS)
      {
        // write to remote blackboard
        try
        {
          blackboard::tBlackboardWriteAccess<rrlib::serialization::tMemoryBuffer> bb(*bb_client, std::chrono::seconds(2));
          if (bb.Size() == 0)
          {
            bb.Resize(1u);
          }
          os.Reset(&(bb.Get(0u)));
          os.WriteInt(i);
          os.Close();

        }
        catch (const blackboard::tBBLockException& ex)
        {
          //log(LogLevel.LL_WARNING, logDomain, "Write-locking blackboard failed");
        }

        // read local blackboard
        try
        {
          blackboard::tBlackboardReadAccess<rrlib::serialization::tMemoryBuffer> bb(*local_bb_client, std::chrono::seconds(2));
          if (bb.Size() > 0)
          {
            is.Reset(&(bb.Get(0u)));
            if (is.MoreDataAvailable())
            {
              FINROC_LOG_PRINT(USER, "Reading Blackboard: ", is.ReadInt());
            }
            is.Close();
          }

        }
        catch (const blackboard::tBBLockException& ex)
        {
          FINROC_LOG_PRINT(WARNING, "Read-locking blackboard failed");
        }
      }

      try
      {
        rrlib::thread::tThread::Sleep(std::chrono::milliseconds(cPUBLISH_FREQ), true);
      }
      catch (const util::tException& e)
      {
        e.PrintStackTrace();
      }

      if (i % 10 == 0)
      {
        printf("Cycle %d of %d\n", i, stop_cycle);
      }
      if (i == stop_cycle)
      {
        break;
      }
    }

  }

};

const int16 tNetworkTestSuite::cPUBLISH_FREQ; // not clean - but only included from one .cpp file

} // namespace finroc
} // namespace core

#endif // core__test__tNetworkTestSuite_h__
