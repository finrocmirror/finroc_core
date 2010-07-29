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

#ifndef CORE__PORT__RPC__TPULLCALL_H
#define CORE__PORT__RPC__TPULLCALL_H

#include "core/buffers/tCoreInput.h"
#include "core/buffers/tCoreOutput.h"
#include "core/port/rpc/tAbstractCall.h"
#include "finroc_core_utils/thread/tTask.h"

namespace finroc
{
namespace core
{
class tThreadLocalCache;
class tNetPort;

/*!
 * \author Max Reichardt
 *
 * This class is used for port-pull-requests/calls - locally and over the net.
 *
 * (Caller stack will contain every port in chain - pulled value will be assigned to each of them)
 */
class tPullCall : public tAbstractCall, public util::tTask
{
public:

  //  /** Maximum size of caller stack */
  //  private final static int MAX_CALL_DEPTH = 16;

  //  /** Data type of method */
  //  public static DataType METHOD_TYPE;

  //  /** Stores information about pulled port data */
  //  public final @PassByValue PublishCache info = new PublishCache();
  //
  //  /** Reference to pulled "cheap copy" port data */
  //  public CCInterThreadContainer<?> ccData;
  //
  //  /** Reference to pulled port data */
  //  public PortData data;
  //
  //  /** ThreadLocalCache - is != null - if it has been set up to perform assignments with current cc data */
  //  public ThreadLocalCache tc;

  /*! Assign pulled value to ports in between? */
  bool intermediate_assign;

  /*! Is this a pull call for a cc port? */
  bool cc_pull;

  /*! when received through network and executed in separate thread: Port to call pull on and port to send result back over */
  tNetPort* port;

  /*! Log domain for this class */
  RRLIB_LOG_CREATE_NAMED_DOMAIN(log_domain, "rpc");

private:

  //  public static void staticInit() {
  //      // JavaOnlyBlock
  //      METHOD_TYPE = DataTypeRegister.getInstance().addDataType(MethodCall.class);
  //
  //       cMETHOD_TYPE = tDataTypeRegister::GetInstance()->AddDataType<tMethodCall>("MethodCall");
  //  }
  //
  //  @Override @JavaOnly
  //  public DataType getType() {
  //      return METHOD_TYPE;
  //  }

  /*!
   * Reset all variable in order to reuse object
   */
  void Reset();

public:

  tPullCall();

  //  /**
  //   * Deserializes PullCall.
  //   * If skipObject is true - the call's object (when returning) is not deserialized.
  //   * In this case, the caller should skip the stream to the next mark.
  //   *
  //   * \param is Input Stream
  //   * \param skipObject Skip Object? (see above)
  //   */
  //  public void possiblyIncompleteDeserialize(@Ref CoreInput is, boolean skipObject) {
  //      super.deserialize(is);
  //      intermediateAssign = is.readBoolean();
  //      ccPull = is.readBoolean();
  //      if (!skipObject && isReturning(true)) {
  //          if (ccPull) {
  //              data = (CCInterThreadContainer<?>)is.readObjectInInterThreadContainer();
  //          } else {
  //              PortData tmp = (PortData)is.readObject();
  //              if (tmp != null) {
  //                  info.curRef = tmp.getCurReference();
  //                  //info.lockEstimate = 5; // just take 5... performance is not critical here
  //                  //info.setLocks = 1; // one for this call
  //                  info.curRefCounter = info.curRef.getRefCounter();
  //                  info.curRefCounter.setLocks((byte)5);
  //              } else {
  //                  info.curRef = null;
  //                  //info.lockEstimate = 5; // just take 5... performance is not critical here
  //                  //info.setLocks = 1; // one for this call
  //                  info.curRefCounter = null;
  //              }
  //          }
  //      }
  //  }

  /* (non-Javadoc)
   * @see core.port7.rpc.AbstractCall#deserialize(core.buffers.CoreInput)
   */
  virtual void Deserialize(tCoreInput& is);

  virtual void ExecuteTask();

  /*!
   * Prepare Execution of call received over network in extra thread
   *
   * \param port Port to execute pull on and to return value over later
   */
  inline void PrepareForExecution(tNetPort* port_)
  {
    this->port = port_;
  }

  /* (non-Javadoc)
   * @see core.port7.rpc.AbstractCall#serialize(core.buffers.CoreBuffer)
   */
  virtual void Serialize(tCoreOutput& oos) const;

  //  @Override
  //  public void genericRecycle() {
  //      if (isResponsible()) {
  //          //System.out.println("Recycling pull call: " + toString());
  //          if (ccPull) {
  //              if (data != null) {
  //                  data.recycle2();
  //              }
  //          } else if (info.curRef != null) {
  //              info.setLocks--; // release pull call's lock
  //              info.releaseObsoleteLocks();
  //          }
  //          reset();
  //          super.recycle();
  //      }
  //  }

  //  /**
  //   * Initializes thread local cache in order to perform assignments in current runtime environment
  //   *
  //   * New buffer in thread local cache won't be locked - since only current thread may recycle it
  //   */
  //  @InCppFile
  //  public void setupThreadLocalCache() {
  //      if (tc == null) {
  //          tc = ThreadLocalCache.getFast();
  //          tc.data = tc.getUnusedBuffer(data.getType());
  //          tc.data.setRefCounter(0);
  //          tc.data.assign(data.getDataPtr());
  //          tc.ref = tc.data.getCurrentRef();
  //      } else {
  //          assert(ThreadLocalCache.getFast() == tc) : "Programming error";
  //      }
  //  }

  virtual const util::tString ToString() const
  {
    return util::tStringBuilder("PullCall (") + GetStatusString() + ", callid: " + ::finroc::core::tAbstractCall::GetMethodCallIndex() + ", threaduid: " + ::finroc::core::tAbstractCall::GetThreadUid() + ")";
  }

};

} // namespace finroc
} // namespace core

#endif // CORE__PORT__RPC__TPULLCALL_H
