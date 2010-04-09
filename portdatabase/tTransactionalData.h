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

#ifndef CORE__PORTDATABASE__TTRANSACTIONALDATA_H
#define CORE__PORTDATABASE__TTRANSACTIONALDATA_H

#include "core/portdatabase/tDataType.h"
#include "core/port/stream/tOutputTransactionStreamPort.h"
#include "core/port/stream/tSingletonPort.h"
#include "core/buffers/tCoreInput.h"
#include "core/tFrameworkElement.h"
#include "core/port/tAbstractPort.h"
#include "core/buffers/tCoreOutput.h"
#include "core/port/std/tPortDataImpl.h"
#include "core/port/stream/tInputPacketProcessor.h"
#include "core/port/stream/tTransactionPacket.h"
#include "core/port/stream/tStreamCommitThread.h"
#include "core/port/std/tPullRequestHandler.h"
#include "core/port/tPortCreationInfo.h"
#include "core/port/stream/tInputTransactionStreamPort.h"

namespace finroc
{
namespace core
{
/*!
 * \author Max Reichardt
 *
 * This is the base class for large complex data that is shared in the whole
 * runtime. It's characteristics are that data is large, singleton, but usually only minor
 * changes are made, so that copying - especially over network - would result
 * in a waste of resources. Therefore instances of transactional data
 * are synchronized using transactions.
 * The object may be modified - non-blocking - in several runtime environments at the same time.
 * The programmer needs to make sure that the incoming transactions result in the same
 * data in each runtime environment. If multiple threads access the same Java object
 * the programmer also needs to take care of locking.
 *
 * This class already creates the necessary ports in a PortSet. This can be retrieved using getPortSet()
 */
template<typename B>
class tTransactionalData : public tPortData, public tInputPacketProcessor<tTransactionPacket>, public tStreamCommitThread::tCallback, public tPullRequestHandler
{
private:
  class tTDPortSet; // inner class forward declaration
protected:
  class tInputTransactions; // inner class forward declaration
private:

  /*! Port Set for Transactional Data */
  tTDPortSet* port_set;

protected:

  /*! Port for outgoing transactions */
  tOutputTransactionStreamPort<B>* output;

  /*! Port for incoming transactions */
  tInputTransactions* input;

  /*! Port sharing data locally */
  tSingletonPort<tTransactionalData<B>*>* local_data;

  /*! Are we currently handling new connection? */
  volatile bool handling_new_connection;

  virtual void HandleNewConnection(tAbstractPort* partner);

  /*! may be overridden */
  inline void PostChildInit()
  {
  }

  /*!
   * Can be overridden... is called regularly
   */
  virtual void Update(int64 time)
  {
  }

public:

  tTransactionalData(const util::tString& description, bool input_, bool output_, bool local, tDataType* bclass, int commit_interval, bool input_shared, bool output_shared);

  /*!
   * Writes data to output stream (is existent)
   *
   * \param data Transaction to write (is copied and can instantly be reused)
   */
  void CommitData(B data);

  virtual void Deserialize(tCoreInput& is)
  {
    throw util::tRuntimeException("Deserialization not meant to be done");
  }

  /*!
   * \return Port Set for Transactional Data
   */
  inline tFrameworkElement* GetPortSet()
  {
    return port_set;
  }

  virtual void Serialize(tCoreOutput& os) const
  {
    throw util::tRuntimeException("Serialization not meant to be done");
  }

  virtual void StreamThreadCallback(int64 time)
  {
    Update(time);
  }

  /*!
   * Port Set for transactional data class
   */
  class tTDPortSet : public tFrameworkElement
  {
  private:

    // Outer class TransactionalData
    tTransactionalData* const outer_class_ptr;

  protected:

    virtual void PostChildInit()
    {
      outer_class_ptr->PostChildInit();
      ::finroc::core::tFrameworkElement::PostChildInit();
    }

    virtual void PrepareDelete()
    {
      tStreamCommitThread::GetInstance()->Unregister(outer_class_ptr);
      ::finroc::core::tFrameworkElement::PrepareDelete();
    }

  public:

    tTDPortSet(tTransactionalData* const outer_class_ptr_, const util::tString& description);

  };

protected:

  /*!
   * Special port for input transactions
   */
  class tInputTransactions : public tInputTransactionStreamPort<B>
  {
  private:

    // Outer class TransactionalData
    tTransactionalData* const outer_class_ptr;

  protected:

    // we have a new connection
    virtual void NewConnection(tAbstractPort* partner)
    {
      HandleNewConnection(partner);
    }

  public:

    tInputTransactions(tTransactionalData* const outer_class_ptr_, tPortCreationInfo pci);

  };

};

} // namespace finroc
} // namespace core

#include "core/portdatabase/tTransactionalData.hpp"

#endif // CORE__PORTDATABASE__TTRANSACTIONALDATA_H
