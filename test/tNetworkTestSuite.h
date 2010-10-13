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

#ifndef CORE__TEST__TNETWORKTESTSUITE_H
#define CORE__TEST__TNETWORKTESTSUITE_H

#include "core/portdatabase/tDataType.h"
#include "core/port/cc/tNumberPort.h"
#include "plugins/blackboard/tRawBlackboardClient.h"
#include "plugins/blackboard/tBlackboardServer.h"
#include "plugins/blackboard/tSingleBufferedBlackboardServer.h"
#include "core/tRuntimeEnvironment.h"
#include "core/port/tThreadLocalCache.h"
#include "core/port/tPortCreationInfo.h"
#include "core/port/tPortFlags.h"
#include "core/portdatabase/tDataTypeRegister.h"
#include "core/buffers/tMemBuffer.h"
#include "plugins/blackboard/tBlackboardManager.h"
#include "rrlib/finroc_core_utils/stream/tInputStreamBuffer.h"
#include "rrlib/finroc_core_utils/stream/tOutputStreamBuffer.h"
#include "core/datatype/tCoreNumber.h"
#include "plugins/blackboard/tBlackboardBuffer.h"
#include "core/port/std/tPort.h"

namespace finroc
{
namespace core
{
/*!
 * \author Max Reichardt
 *
 *
 */
class tNetworkTestSuite : public util::tObject
{
public:

  class tTestStdPort : public tPort<tMemBuffer>
  {
  private:

    // Outer class NetworkTestSuite
    tNetworkTestSuite* const outer_class_ptr;

  public:

    util::tOutputStreamBuffer os;

    util::tInputStreamBuffer is;

    tTestStdPort(tNetworkTestSuite* const outer_class_ptr_, tPortCreationInfo pci) :
        tPort<tMemBuffer>(pci),
        outer_class_ptr(outer_class_ptr_),
        os(),
        is()
    {
    }

    inline int GetIntRaw()
    {
      const tMemBuffer* mb = GetLockedUnsafe();
      is.Reset(mb);
      int result = -1;
      if (is.MoreDataAvailable())
      {
        result = is.ReadInt();
      }
      is.Close();
      mb->GetManager()->ReleaseLock();
      return result;
    }

    inline void Publish(int i)
    {
      tMemBuffer* mb = GetUnusedBuffer();
      os.Reset(mb);
      os.WriteInt(i);
      os.Close();
      ::finroc::core::tPortBase::Publish(mb);
    }

  };

public:

  static const bool cCC_TESTS = false, cSTD_TESTS = false;

  static const bool cBB_TESTS = true;

  static const bool cPUSH_TESTS = false, cPULL_PUSH_TESTS = false, cREVERSE_PUSH_TESTS = false, cQ_TESTS = false;

  util::tString blackboard_name, partner_blackboard_name;

  static const int16 cPUBLISH_FREQ = 200, cRECV_FREQ = 1000;

  int stop_cycle;

  tNumberPort* cc_push_out, * cc_pull_push_out, * cc_rev_push_out, * cc_rev_push_out_local, * cc_qOut;

  tNumberPort* cc_push_in, * cc_pull_push_in, * cc_rev_push_in, * cc_qIn;

  tTestStdPort* std_push_out, * std_pull_push_out, * std_rev_push_out, * std_rev_push_out_local, * std_qOut;

  tTestStdPort* std_push_in, * std_pull_push_in, * std_rev_push_in, * std_qIn;

  blackboard::tRawBlackboardClient* bb_client, * local_bb_client;

  blackboard::tBlackboardServer* bb_server;

  blackboard::tSingleBufferedBlackboardServer* sbb_server;

  tNetworkTestSuite(const util::tString& bb_name, const util::tString& partner_bBName, int stop_cycle_) :
      util::tObject(),
      blackboard_name(bb_name),
      partner_blackboard_name(partner_bBName),
      stop_cycle(stop_cycle_),
      cc_push_out(NULL),
      cc_pull_push_out(NULL),
      cc_rev_push_out(NULL),
      cc_rev_push_out_local(NULL),
      cc_qOut(NULL),
      cc_push_in(NULL),
      cc_pull_push_in(NULL),
      cc_rev_push_in(NULL),
      cc_qIn(NULL),
      std_push_out(NULL),
      std_pull_push_out(NULL),
      std_rev_push_out(NULL),
      std_rev_push_out_local(NULL),
      std_qOut(NULL),
      std_push_in(NULL),
      std_pull_push_in(NULL),
      std_rev_push_in(NULL),
      std_qIn(NULL),
      bb_client(NULL),
      local_bb_client(NULL),
      bb_server(NULL),
      sbb_server(NULL)
  {
    tRuntimeEnvironment::GetInstance();
    tThreadLocalCache::Get();

    if (cCC_TESTS)
    {
      if (cPUSH_TESTS)
      {
        cc_push_out = new tNumberPort(tPortCreationInfo("CCPush Output", tPortFlags::cSHARED_OUTPUT_PORT));
        cc_push_in = new tNumberPort(tPortCreationInfo("CCPush Input", tPortFlags::cSHARED_INPUT_PORT));
        cc_push_in->SetMinNetUpdateInterval(cRECV_FREQ);
      }
      if (cPULL_PUSH_TESTS)
      {
        cc_pull_push_out = new tNumberPort(tPortCreationInfo("CCPullPush Output", tPortFlags::cSHARED_OUTPUT_PORT));
        cc_pull_push_in = new tNumberPort(tPortCreationInfo("CCPullPush Input", tPortFlags::cSHARED_INPUT_PORT));
        cc_pull_push_in->SetMinNetUpdateInterval(cRECV_FREQ);
        cc_pull_push_in->SetPushStrategy(false);
      }
      if (cREVERSE_PUSH_TESTS)
      {
        cc_rev_push_out = new tNumberPort(tPortCreationInfo("CCRevPush Output", tPortFlags::cSHARED_OUTPUT_PORT | tPortFlags::cACCEPTS_REVERSE_DATA_PUSH));
        cc_rev_push_out_local = new tNumberPort(tPortCreationInfo("CCRevPush Output Local", tPortFlags::cSHARED_OUTPUT_PORT));
        cc_rev_push_in = new tNumberPort(tPortCreationInfo("CCRevPush Input", tPortFlags::cSHARED_INPUT_PORT));
        cc_rev_push_in->SetMinNetUpdateInterval(cRECV_FREQ);
        cc_rev_push_out_local->ConnectToTarget(cc_rev_push_in);
      }
      if (cQ_TESTS)
      {
        cc_qOut = new tNumberPort(tPortCreationInfo("CCPush Queue Output", tPortFlags::cSHARED_OUTPUT_PORT));
        cc_qIn = new tNumberPort(tPortCreationInfo("CCPush Queue Input", tPortFlags::cSHARED_INPUT_PORT, 0));
        cc_qIn->SetMinNetUpdateInterval(cRECV_FREQ);
      }
    }
    if (cSTD_TESTS)
    {
      tDataType* bt = tDataTypeRegister::GetInstance()->GetDataType(util::tTypedClass<tMemBuffer>());
      if (cPUSH_TESTS)
      {
        std_push_out = new tTestStdPort(this, tPortCreationInfo("StdPush Output", bt, tPortFlags::cSHARED_OUTPUT_PORT));
        std_push_in = new tTestStdPort(this, tPortCreationInfo("StdPush Input", bt, tPortFlags::cSHARED_INPUT_PORT));
        std_push_in->SetMinNetUpdateInterval(cRECV_FREQ);
      }
      if (cPULL_PUSH_TESTS)
      {
        std_pull_push_out = new tTestStdPort(this, tPortCreationInfo("StdPullPush Output", bt, tPortFlags::cSHARED_OUTPUT_PORT));
        std_pull_push_in = new tTestStdPort(this, tPortCreationInfo("StdPullPush Input", bt, tPortFlags::cSHARED_INPUT_PORT));
        std_pull_push_in->SetMinNetUpdateInterval(cRECV_FREQ);
        std_pull_push_in->SetPushStrategy(false);
      }
      if (cREVERSE_PUSH_TESTS)
      {
        std_rev_push_out = new tTestStdPort(this, tPortCreationInfo("StdRevPush Output", bt, tPortFlags::cSHARED_OUTPUT_PORT | tPortFlags::cACCEPTS_REVERSE_DATA_PUSH));
        std_rev_push_out_local = new tTestStdPort(this, tPortCreationInfo("StdRevPush Output Local", bt, tPortFlags::cSHARED_OUTPUT_PORT));
        std_rev_push_in = new tTestStdPort(this, tPortCreationInfo("StdRevPush Input", bt, tPortFlags::cSHARED_INPUT_PORT));
        std_rev_push_in->SetMinNetUpdateInterval(cRECV_FREQ);
        std_rev_push_out_local->ConnectToTarget(std_rev_push_in);
      }
      if (cQ_TESTS)
      {
        std_qOut = new tTestStdPort(this, tPortCreationInfo("StdPush Queue Output", bt, tPortFlags::cSHARED_OUTPUT_PORT));
        std_qIn = new tTestStdPort(this, tPortCreationInfo("StdPush Queue Input", bt, tPortFlags::cSHARED_INPUT_PORT, 0));
        std_qIn->SetMinNetUpdateInterval(cRECV_FREQ);
      }
    }

    if (cBB_TESTS)
    {
      blackboard::tBlackboardManager::GetInstance();
      //Plugins.getInstance().addPlugin(new Blackboard2Plugin());
      //bbServer = new BlackboardServer(blackboardName);
      sbb_server = new blackboard::tSingleBufferedBlackboardServer(blackboard_name);
      bb_client = new blackboard::tRawBlackboardClient(blackboard::tRawBlackboardClient::GetDefaultPci().Derive(partner_blackboard_name));
      local_bb_client = new blackboard::tRawBlackboardClient(blackboard::tRawBlackboardClient::GetDefaultPci().Derive(blackboard_name));
    }
  }

  inline void MainLoop()
  {
    // write new values to ports and read input ports
    int i = 0;
    util::tInputStreamBuffer is;
    util::tOutputStreamBuffer os;

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
            util::tSystem::out.Println(util::tStringBuilder("ccPushIn received: ") + cc_push_in->GetIntRaw());
          }
        }

        if (cPULL_PUSH_TESTS)
        {
          // publish value and pull or check for change
          cc_pull_push_out->Publish(i);

          if (cc_pull_push_in->HasChanged())
          {
            cc_pull_push_in->ResetChanged();
            util::tSystem::out.Println(util::tStringBuilder("ccPullPushIn received: ") + cc_pull_push_in->GetIntRaw());
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
            util::tSystem::out.Println(util::tStringBuilder("Pulling ccPullPushIn: ") + cc_pull_push_in->GetIntRaw());
          }
        }

        if (cREVERSE_PUSH_TESTS)
        {
          if (cc_rev_push_in->HasChanged())
          {
            int val = cc_rev_push_in->GetIntRaw();
            if (val < 0)
            {
              util::tSystem::out.Println(util::tStringBuilder("ccRevPushIn received: ") + val);
            }
          }

          // publish value and check whether something has changed
          cc_rev_push_out_local->Publish(i);
          if (cc_rev_push_out->HasChanged())
          {
            cc_rev_push_out->ResetChanged();
            util::tSystem::out.Println(util::tStringBuilder("ccRevPushOut received: ") + cc_rev_push_out->GetIntRaw());
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
            util::tSystem::out.Print("ccPushIn received: ");
            tCoreNumber* cn = NULL;
            while ((cn = cc_qIn->DequeueSingleAutoLocked()) != NULL)
            {
              util::tSystem::out.Print(util::tStringBuilder(" ") + cn->IntValue());
            }
            tThreadLocalCache::GetFast()->ReleaseAllLocks();
            util::tSystem::out.Println();
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
            util::tSystem::out.Println(util::tStringBuilder("stdPushIn received: ") + std_push_in->GetIntRaw());
          }
        }

        if (cPULL_PUSH_TESTS)
        {
          // publish value and pull or check for change
          std_pull_push_out->Publish(i);

          if (std_pull_push_in->HasChanged())
          {
            std_pull_push_in->ResetChanged();
            util::tSystem::out.Println(util::tStringBuilder("stdPullPushIn received: ") + std_pull_push_in->GetIntRaw());
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
            util::tSystem::out.Println(util::tStringBuilder("Pulling stdPullPushIn: ") + std_pull_push_in->GetIntRaw());
          }
        }

        if (cREVERSE_PUSH_TESTS)
        {
          if (std_rev_push_in->HasChanged())
          {
            int val = std_rev_push_in->GetIntRaw();
            if (val < 0)
            {
              util::tSystem::out.Println(util::tStringBuilder("stdRevPushIn received: ") + val);
            }
          }

          // publish value and check whether something has changed
          std_rev_push_out_local->Publish(i);
          if (std_rev_push_out->HasChanged())
          {
            std_rev_push_out->ResetChanged();
            util::tSystem::out.Println(util::tStringBuilder("stdRevPushOut received: ") + std_rev_push_out->GetIntRaw());
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
            util::tSystem::out.Print("stdPushIn received: ");
            tMemBuffer* cn = NULL;
            while ((cn = std_qIn->DequeueSingleAutoLocked()) != NULL)
            {
              is.Reset(cn);
              int result = -1;
              if (is.MoreDataAvailable())
              {
                result = is.ReadInt();
              }
              util::tSystem::out.Print(util::tStringBuilder(" ") + result);
              is.Close();
            }
            tThreadLocalCache::GetFast()->ReleaseAllLocks();
            util::tSystem::out.Println();
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
        blackboard::tBlackboardBuffer* bb = bb_client->WriteLock(2000);
        if (bb != NULL)
        {
          if (bb->GetElements() == 0)
          {
            bb->Resize(1, 1, 4, false);
          }
          os.Reset(bb);
          os.WriteInt(i);
          os.Close();
          bb_client->Unlock();
        }

        // read local blackboard
        const blackboard::tBlackboardBuffer* bb2 = local_bb_client->ReadLock(2000);
        if (bb2 != NULL)
        {
          is.Reset(bb2);
          if (is.MoreDataAvailable())
          {
            util::tSystem::out.Println(util::tStringBuilder("Reading Blackboard: ") + is.ReadInt());
          }
          is.Close();
          local_bb_client->Unlock();
        }
      }

      try
      {
        util::tThread::Sleep(cPUBLISH_FREQ);
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

} // namespace finroc
} // namespace core

#endif // CORE__TEST__TNETWORKTESTSUITE_H
