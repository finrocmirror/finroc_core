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

#ifndef CORE__PORT__RPC__TABSTRACTCALL_H
#define CORE__PORT__RPC__TABSTRACTCALL_H

#include "core/portdatabase/tDataType.h"
#include "finroc_core_utils/stream/tMemoryBuffer.h"
#include "core/port/rpc/tCallParameter.h"
#include "core/buffers/tCoreOutput.h"
#include "core/buffers/tCoreInput.h"
#include "finroc_core_utils/container/tSimpleList.h"
#include "core/port/std/tPortDataImpl.h"
#include "core/port/cc/tCCInterThreadContainer.h"
#include "core/portdatabase/tSerializableReusable.h"

#include "core/port/rpc/tMethodCallException.h"
#include "core/portdatabase/tDataTypeRegister.h"
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
class tAbstractCall : public tSerializableReusable
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

  /*! Storage buffer for any (serialized) parameters or return values - for sending over the network */
  util::tMemoryBuffer param_storage;

  /*! Storage for parameters that are used in call - for usage in local runtime (fixed size, since this is smaller & less hassle than dynamic array) */
  tCallParameter params[cMAX_PARAMS];

  /*! To write to paramStorage... */
  tCoreOutput os;

  /*! To read from paramStorage... */
  tCoreInput is;

  /*! Any (usually big) buffers that call is currently in charge of recycling */
  util::tSimpleList<const tPortData*> responsibilities;

  /*! For incoming commands: Is it possible to deserialize parameters in paramStorage? */
  bool deserializable_parameters;

protected:

  int8 status;

public:

  /*! Status of this method call */
  static const int8 cNONE = 0, cSYNCH_CALL = 1, cASYNCH_CALL = 2, cSYNCH_RETURN = 3, cASYNCH_RETURN = 4, cCONNECTION_EXCEPTION = 5;

protected:

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

  inline void SetMethodCallIndex(int16 method_call_index_)
  {
    this->method_call_index = method_call_index_;
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

  /*!
   * \param max_call_depth Maximum size of call stack
   */
  tAbstractCall();

  virtual void Deserialize(tCoreInput& is_)
  {
    DeserializeImpl(&(is_), false);
  }

  void DeserializeImpl(tCoreInput* is_, bool skip_parameters);

  template <typename T>
  void AddParamForSending(T t)
  {
    tParameterUtil<T>::AddParamForSending(responsibilities, os, t);
  }

  template <typename T>
  void GetParam(int index, T& pd)
  {
    tParameterUtil<T>::GetParam(&(params[index]), pd);
  }

  template <typename T>
  void AddParamForLocalCall(int index, T pd)
  {
    tParameterUtil<T>::AddParamForLocalCall(&(params[index]), pd);
  }

  //
  //  void addParamForSending(const PortData* pd) {
  //      if (pd == NULL) {
  //          os.writeByte(NULLPARAM);
  //      }
  //      os.writeByte(PORTDATA);
  //      responsibilities.add(pd);
  //  }
  //
  //  void addParamForSending(const int pd) {
  //      os.writeByte(INT);
  //      os.writeInt(pd);
  //  }
  //
  //  void addParamForSending(const int64 pd) {
  //      os.writeByte(LONG);
  //      os.writeLong(pd);
  //  }
  //
  //  void addParamForSending(const float pd) {
  //      os.writeByte(FLOAT);
  //      os.writeFloat(pd);
  //  }
  //
  //  void addParamForSending(const double pd) {
  //      os.writeByte(DOUBLE);
  //      os.writeDouble(pd);
  //  }
  //
  //  void addParamForSending(const int8 pd) {
  //      os.writeByte(BYTE);
  //      os.writeByte(pd);
  //  }
  //
  //  void addParamForSending(const short pd) {
  //      os.writeByte(SHORT);
  //      os.writeShort(pd);
  //  }
  //
  //
  //  template <typename T>
  //  void addParamForSending(CCInterThreadContainer<T>* container) {
  //      if (container == NULL) {
  //          os.writeByte(NULLPARAM);
  //      }
  //      //assert(container->isInterThreadContainer() && "only interthread container allowed in method calls");
  //      os.writeByte(CCCONTAINER);
  //      os.writeObject(container);
  //      container->recycle2();
  //  }
  //
  //  template <typename T>
  //  void addParamForSending(const T* c) {
  //      if (c == NULL) {
  //          os.writeByte(NULLPARAM);
  //      }
  //      DataType* dt = DataTypeRegister::getInstance()->getDataType<T>();
  //      assert(dt != NULL && dt->isCCType());
  //      os.writeByte(CCDATA);
  //      os.writeShort(dt->getUid());
  //      writeParam(c, dt);
  //  }
  //
  //  template <typename T>
  //  void addParamForSending(T c) {
  //      addParamForSending((const T)c);
  //  }
  //
  //  void writeParam(const CoreSerializable* cs, DataType* dt) {
  //      cs->serialize(os);
  //  }
  //
  //  void writeParam(void* cs, DataType* dt) {
  //      dt->directSerialize(cs, &os);
  //  }
  //
  //  void addParamForLocalCall(int paramIndex, const PortData* pd) {
  //      Parameter* p = &(params[paramIndex]);
  //      if (pd == NULL) {
  //          p->type = NULLPARAM;
  //      }
  //      p->type = PORTDATA;
  //      p->value = pd;
  //  }
  //
  //  void addParamForLocalCall(int paramIndex, const int pd) {
  //      Parameter* p = &(params[paramIndex]);
  //      p->type = INT;
  //      p->ival = pd;
  //  }
  //
  //  void addParamForLocalCall(int paramIndex, const int64 pd) {
  //      Parameter* p = &(params[paramIndex]);
  //      p->type = LONG;
  //      p->lval = pd;
  //  }
  //
  //  void addParamForLocalCall(int paramIndex, const float pd) {
  //      Parameter* p = &(params[paramIndex]);
  //      p->type = FLOAT;
  //      p->fval = pd;
  //  }
  //
  //  void addParamForLocalCall(int paramIndex, const double pd) {
  //      Parameter* p = &(params[paramIndex]);
  //      p->type = DOUBLE;
  //      p->dval = pd;
  //  }
  //
  //  void addParamForLocalCall(int paramIndex, const int8 pd) {
  //      Parameter* p = &(params[paramIndex]);
  //      p->type = BYTE;
  //      p->bval = pd;
  //  }
  //
  //  void addParamForLocalCall(int paramIndex, const int16 pd) {
  //      Parameter* p = &(params[paramIndex]);
  //      p->type = SHORT;
  //      p->sval = pd;
  //  }
  //
  //  template <typename T>
  //  void addParamForLocalCall(int paramIndex, CCInterThreadContainer<T>* container) {
  //      Parameter* p = &(params[paramIndex]);
  //      if (container == NULL) {
  //          p->type = NULLPARAM;
  //      }
  //      //assert(container->isInterThreadContainer() && "only interthread container allowed in method calls");
  //      p->ccval = container;
  //  }
  //
  //  template <typename T>
  //  void addParamForLocalCall(int paramIndex, const T* c) {
  //      Parameter* p = &(params[paramIndex]);
  //      if (c == NULL) {
  //          p->type = NULLPARAM;
  //      }
  //      DataType* dt = DataTypeRegister::getInstance()->getDataType<T>();
  //      assert(dt != NULL && dt->isCCType());
  //      CCInterThreadContainer<T>* cc = (CCInterThreadContainer<T>*)GetInterThreadBuffer(dt);
  //      cc->assign(c);
  //      p->type = CCDATA;
  //      p->ccval = cc;
  //  }
  //
  //  template <typename T>
  //  void addParamForLocalCall(int paramIndex, T c) {
  //      addParamForLocalCall(paramIndex, (const T)c);
  //  }
  //

  /*!
   * Prepare method call received from the net for local call.
   *
   * Deserializes parameters from storage to param objects
   */
  void DeserializeParamaters();

  virtual void GenericRecycle()
  {
    Recycle();
  }

  tCCInterThreadContainer<>* GetInterThreadBuffer(tDataType* dt);

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

  void Recycle();

  /*!
   * Recycle all parameters, but keep empty method call
   */
  void RecycleParameters();

  /*!
   * When sending parameters over the network: Call this when all parameters have been added
   */
  inline void SendParametersComplete()
  {
    os.Close();
  }

  //  /**
  //   * (Should only be called by network port)
  //   * Add caller to top of caller stack
  //   *
  //   * \param callerHandle caller handle
  //   */
  //  public void pushCaller(int callerHandle) {
  //      callerStack.add(callerHandle);
  //  }
  //
  //  /**
  //   * (Should only be called by network port)
  //   * Add caller to top of caller stack
  //   *
  //   * \param caller Caller
  //   */
  //  public void pushCaller(FrameworkElement caller) {
  //      callerStack.add(caller.getHandle());
  //  }
  //
  //  /**
  //   * (Should only be called by network port)
  //   * \return return current size of caller stack
  //   */
  //  @ConstMethod public @SizeT int callerStackSize() {
  //      return callerStack.size();
  //  }
  //
  //  /**
  //   * (Should only be called by network port)
  //   * Remove and return caller handle at top of stack
  //   *
  //   * \return Caller Handle
  //   */
  //  public int popCaller() {
  //      return callerStack.removeLast();
  //  }
  //
  //  /**
  //   * Remove and return caller at top of stack
  //   *
  //   * \return Caller
  //   */
  //  public FrameworkElement popCallerElement() {
  //      return RuntimeEnvironment.getInstance().getElement(popCaller());
  //  }
  //
  //  /**
  //   * Pop caller from stack and call its handleCallReturn method
  //   */
  //  public void returnToCaller() {
  //      FrameworkElement fe = popCallerElement();
  //      if (fe != null) {
  //          fe.handleCallReturn(this);
  //      } else {
  //           printf("%p ", this);
  //          System.out.println("warning: lost method call, because caller seems to have been deleted... caller will hopefully timeout(?)");
  //          //recycle();
  //      }
  //  }

  virtual void Serialize(tCoreOutput& oos) const;

  //
  //  void getParam(int index, const PortData*& pd) {
  //      Parameter* p = &(params[index]);
  //      pd = p->value;
  //      p->clear();
  //  }
  //
  //  void getParam(int index, PortData*& pd) {
  //      Parameter* p = &(params[index]);
  //      pd = const_cast<PortData*>(p->value); // not entirely clean... but everything else seems a lot of work (what's const and what isn't should be defined in methods)
  //      p->clear();
  //  }
  //
  //  void getParam(int index, int& pd) {
  //      Parameter* p = &(params[index]);
  //      pd = p->ival;
  //      p->clear();
  //  }
  //
  //  void getParam(int index, int64& pd) {
  //      Parameter* p = &(params[index]);
  //      pd = p->lval;
  //      p->clear();
  //  }
  //
  //  void getParam(int index, float& pd) {
  //      Parameter* p = &(params[index]);
  //      pd = p->fval;
  //      p->clear();
  //  }
  //
  //  void getParam(int index, double& pd) {
  //      Parameter* p = &(params[index]);
  //      pd = p->dval;
  //      p->clear();
  //  }
  //
  //  void getParam(int index, int8& pd) {
  //      Parameter* p = &(params[index]);
  //      pd = p->bval;
  //      p->clear();
  //  }
  //
  //  void getParam(int index, int16& pd) {
  //      Parameter* p = &(params[index]);
  //      pd = p->sval;
  //      p->clear();
  //  }
  //
  //  template <typename T>
  //  void getParam(int index, CCInterThreadContainer<T>*& container) {
  //      Parameter* p = &(params[index]);
  //      container = p->ccval;
  //      p->clear();
  //  }
  //
  //  template <typename T>
  //  void getParam(int index, T*& c) {
  //      Parameter* p = &(params[index]);
  //      c = p->ccval->getDataPtr();
  //      p->clear();
  //  }
  //

  /*!
   * Clear parameters and set method call status to exception
   *
   * \param type_id Type of exception
   */
  void SetExceptionStatus(int8 type_id);

  inline void SetExceptionStatus(tMethodCallException::tType type)
  {
    SetExceptionStatus((int8)type);
  }

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
  void SetupSynchCall(tMethodCallSyncher* mcs);

};

} // namespace finroc
} // namespace core

#endif // CORE__PORT__RPC__TABSTRACTCALL_H
