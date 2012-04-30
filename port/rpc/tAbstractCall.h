/**
 * You received this file as part of an advanced experimental
 * robotics framework prototype ('finroc')
 *
 * Copyright (C) 2007-2012 Max Reichardt,
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

#ifndef core__port__rpc__tAbstractCall_h__
#define core__port__rpc__tAbstractCall_h__

#include "rrlib/finroc_core_utils/definitions.h"
#include "rrlib/rtti/rtti.h"

#include "core/portdatabase/tSerializableReusable.h"
#include "core/port/rpc/tMethodCallException.h"
#include "core/port/rpc/tCallParameter.h"
#include "core/port/rpc/tParameterUtil.h"

namespace finroc
{
namespace core
{
class tMethodCallSyncher;

/*!
 * \author Max Reichardt
 *
 * This is the base abstract class for (possibly synchronous) calls
 * (such as pull calls and method calls)
 */
class tAbstractCall : public tSerializableReusableTask
{
public:
  /*! Enum for status of method call */
  enum class tStatus
  {
    NONE, SYNCH_CALL, ASYNCH_CALL, SYNCH_RETURN, ASYNCH_RETURN, EXCEPTION
  };

private:

  friend class tMethodCallSyncher;
  friend class tSynchMethodCallLogic;

  /*! Method Syncher index of calling method - in case this is a synchronous method call - otherwise -1 - valid only on calling system */
  int8 syncher_id;

  /*! Unique Thread ID of calling method */
  int thread_uid;

  /*! Status of this method call */
  tStatus status;
  tMethodCallException::tType exception_type;

  /*! Index of method call - used to filter out obsolete returns */
  int16 method_call_index;

  /*! Local port handle - only used while call is enqueued in network queue */
  int local_port_handle;

  /*! Destination port handle - only used while call is enqueued in network queue */
  int remote_port_handle;

protected:

  virtual void GenericRecycle()
  {
    Recycle();
  }

  inline int16 GetMethodCallIndex() const
  {
    return method_call_index;
  }

  /*!
   * \return Method Syncher index of calling method
   */
  inline int GetSyncherID() const
  {
    return syncher_id;
  }

  void Recycle();

  inline void SetMethodCallIndex(int16 method_call_index_)
  {
    this->method_call_index = method_call_index_;
  }

  /*!
   * \param syncher_iD Method Syncher index of calling method
   */
  inline void SetSyncherID(int syncher_id)
  {
    assert(syncher_id <= 127);
    this->syncher_id = static_cast<int8>(syncher_id);
  }

public:

  typedef std::unique_ptr<tAbstractCall, tRecycler> tPtr;

  /*!
   * \param max_call_depth Maximum size of call stack
   */
  tAbstractCall();

  virtual void Deserialize(rrlib::serialization::tInputStream& is)
  {
    DeserializeImpl(is);
  }

  void DeserializeImpl(rrlib::serialization::tInputStream& is);

  /*!
   * \return Type of exception (only makes sense, if HasException() is true)
   */
  inline tMethodCallException::tType GetExceptionType()
  {
    return exception_type;
  }

  /*!
   * \return Local port handle - only used while call is enqueued in network queue
   */
  inline int GetLocalPortHandle() const
  {
    return local_port_handle;
  }

  /*!
   * \return Destination port handle - only used while call is enqueued in network queue
   */
  inline int GetRemotePortHandle() const
  {
    return remote_port_handle;
  }

  inline tStatus GetStatus() const
  {
    return status;
  }

  inline const util::tString GetStatusString() const
  {
    return make_builder::GetEnumString(status);
  }

  inline int GetThreadUid() const
  {
    return thread_uid;
  }

  /*!
   * \return Does call cause a connection exception
   */
  inline bool HasException() const
  {
    return status == tStatus::EXCEPTION;
  }

  /*!
   * \return Is call (already) returning?
   */
  inline bool IsReturning(bool include_exception) const
  {
    return status == tStatus::ASYNCH_RETURN || status == tStatus::SYNCH_RETURN || (include_exception && status == tStatus::EXCEPTION);
  }

  virtual void Serialize(rrlib::serialization::tOutputStream& oos) const;

  /*!
   * Clear parameters and set method call status to exception
   *
   * \param type Type of exception
   */
  virtual void SetExceptionStatus(tMethodCallException::tType type);

  /*!
   * \param local_port_handle Local port handle - only used while call is enqueued in network queue
   */
  inline void SetLocalPortHandle(int local_port_handle_)
  {
    this->local_port_handle = local_port_handle_;
  }

  /*!
   * \param remote_port_handle Destination port handle - only used while call is enqueued in network queue
   */
  inline void SetRemotePortHandle(int remote_port_handle_)
  {
    this->remote_port_handle = remote_port_handle_;
  }

  inline void SetStatus(tStatus status)
  {
    this->status = status;
  }

  /*!
   * Set status to RETURNING.
   *
   * Depending on whether we have a synch or asynch call is will be SYNCH_RETURN or ASYNCH_RETURN
   */
  inline void SetStatusReturn()
  {
    assert((status == tStatus::SYNCH_CALL || status == tStatus::ASYNCH_CALL));
    status = (status == tStatus::SYNCH_CALL) ? tStatus::SYNCH_RETURN : tStatus::ASYNCH_RETURN;
  }

  /*!
   * (only called by interface port)
   * Prepare asynchronous method call
   */
  void SetupAsynchCall();

  /*!
   * (only called by interface port)
   * Prepare synchronous method call
   *
   * \param mcs MethodSyncher object to use
   */
  void SetupSynchCall(tMethodCallSyncher& mcs);

};

} // namespace finroc
} // namespace core

#endif // core__port__rpc__tAbstractCall_h__
