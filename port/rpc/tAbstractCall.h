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
 * (such as Pull-Calls and method calls)
 */
class tAbstractCall : public tSerializableReusableTask
{
  friend class tMethodCallSyncher;
  friend class tSynchMethodCallLogic;
private:

  /*! Method Syncher index of calling method - in case this is a synchronous method call - otherwise -1 - valid only on calling system */
  int8 syncher_iD;

  /*! Unique Thread ID of calling method */
  int thread_uid;

  static ::finroc::util::tArrayWrapper<util::tString> cSTATUS_STRINGS;

  //  /** Caller stack - contains port handle to which return value will be forwarded - only relevant for network connections */
  //  private final CallStack callerStack;

  /*! Index of method call - used to filter out obsolete returns */
  int16 method_call_index;

  /*! Local port handle - only used while call is enqueued in network queue */
  int local_port_handle;

  /*! Destination port handle - only used while call is enqueued in network queue */
  int remote_port_handle;

  /*! Maximum number of parameters */
  static const size_t cMAX_PARAMS = 4u;

  /*! Storage for parameters that are used in call - for usage in local runtime (fixed size, since this is smaller & less hassle than dynamic array) */
  tCallParameter params[cMAX_PARAMS];

protected:

  int8 status;

public:

  /*! Status of this method call */
  static const int8 cNONE = 0, cSYNCH_CALL = 1, cASYNCH_CALL = 2, cSYNCH_RETURN = 3, cASYNCH_RETURN = 4, cCONNECTION_EXCEPTION = 5;

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
    return syncher_iD;
  }

  void Recycle();

  inline void SetMethodCallIndex(int16 method_call_index_)
  {
    this->method_call_index = method_call_index_;
  }

  void SetParametersHelper(int arg_no) {}

  template <typename Arg, typename ... TArgs>
  void SetParametersHelper(int arg_no, Arg& arg, const TArgs&... args)
  {
    SetParameter(arg_no, arg);
    SetParametersHelper(arg_no + 1, args...);
  }

  /*!
   * \param syncher_iD Method Syncher index of calling method
   */
  inline void SetSyncherID(int syncher_iD_)
  {
    assert((syncher_iD_ <= 127));
    this->syncher_iD = static_cast<int8>(syncher_iD_);
  }

public:

  typedef std::unique_ptr<tAbstractCall, tRecycler> tPtr;

  /*!
   * \param max_call_depth Maximum size of call stack
   */
  tAbstractCall();

  virtual void Deserialize(rrlib::serialization::tInputStream& is)
  {
    DeserializeImpl(is, false);
  }

  void DeserializeImpl(rrlib::serialization::tInputStream& is, bool skip_parameters);

  /*!
   * \return Local port handle - only used while call is enqueued in network queue
   */
  inline int GetLocalPortHandle() const
  {
    return local_port_handle;
  }

  template <typename T>
  void GetParam(int index, T& pd)
  {
    tParameterUtil<T>::GetParam(&(params[index]), pd);
  }

  tPortDataPtr<rrlib::rtti::tGenericObject> GetParamGeneric(int index);

  /*!
   * \return Destination port handle - only used while call is enqueued in network queue
   */
  inline int GetRemotePortHandle() const
  {
    return remote_port_handle;
  }

  inline int8 GetStatus() const
  {
    return status;
  }

  inline const util::tString& GetStatusString() const
  {
    return cSTATUS_STRINGS[status];
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
    return status == cCONNECTION_EXCEPTION;
  }

  /*!
   * \return Is call (already) returning?
   */
  inline bool IsReturning(bool include_exception) const
  {
    return status == cASYNCH_RETURN || status == cSYNCH_RETURN || (include_exception && status == cCONNECTION_EXCEPTION);
  }

  /*!
   * Recycle all parameters, but keep empty method call
   */
  void RecycleParameters();

  virtual void Serialize(rrlib::serialization::tOutputStream& oos) const;

  inline void SetExceptionStatus(tMethodCallException::tType type)
  {
    SetExceptionStatus((int8)type);
  }

  /*!
   * Clear parameters and set method call status to exception
   *
   * \param type_id Type of exception
   */
  void SetExceptionStatus(int8 type_id);

  /*!
   * \param local_port_handle Local port handle - only used while call is enqueued in network queue
   */
  inline void SetLocalPortHandle(int local_port_handle_)
  {
    this->local_port_handle = local_port_handle_;
  }

  /*!
   * Set Parameter with specified in index to specified value
   */
  template <typename T>
  void SetParameter(int index, T& pd)
  {
    tParameterUtil<T>::AddParam(&(params[index]), pd);
  }

  /*!
   * Set Parameters to specified values
   */
  template <typename ... TArgs>
  void SetParameters(TArgs&... args)
  {
    SetParametersHelper(0, args...);
  }

  /*!
   * \param remote_port_handle Destination port handle - only used while call is enqueued in network queue
   */
  inline void SetRemotePortHandle(int remote_port_handle_)
  {
    this->remote_port_handle = remote_port_handle_;
  }

  inline void SetStatus(int8 status_)
  {
    this->status = status_;
  }

  /*!
   * Set status to RETURNING.
   *
   * Depending on whether we have a synch or asynch call is will be SYNCH_RETURN or ASYNCH_RETURN
   */
  inline void SetStatusReturn()
  {
    assert((status == cSYNCH_CALL || status == cASYNCH_CALL));
    status = (status == cSYNCH_CALL) ? cSYNCH_RETURN : cASYNCH_RETURN;
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
