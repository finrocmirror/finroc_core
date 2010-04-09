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
#include "core/port/tPortFlags.h"

namespace finroc
{
namespace core
{
template<typename B>
tTransactionalData<B>::tTransactionalData(const util::tString& description, bool input_, bool output_, bool local, tDataType* bclass, int commit_interval, bool input_shared, bool output_shared) :
    port_set(new tTDPortSet(this, description)),
    output(output_ ? new tOutputTransactionStreamPort<B>(tPortCreationInfo("output transactions", port_set, bclass, output_shared ? tPortFlags::cSHARED_OUTPUT_PORT : tPortFlags::cOUTPUT_PORT), commit_interval, this) : NULL),
    input(input_ ? new tInputTransactions(this, tPortCreationInfo(bclass, input_shared ? tPortFlags::cSHARED_INPUT_PORT : tPortFlags::cINPUT_PORT)) : NULL),
    local_data(local ? new tSingletonPort<tTransactionalData<B>*>(tPortCreationInfo("data", this->GetType(), tPortFlags::cOUTPUT_PORT), this) : NULL),
    handling_new_connection(false)
{
  tStreamCommitThread::GetInstance()->Register(this);
}

template<typename B>
void tTransactionalData<B>::CommitData(B data)
{
  if (output != NULL)
  {
    output->CommitData(data);
  }
}

template<typename B>
void tTransactionalData<B>::HandleNewConnection(tAbstractPort* partner)
{
  handling_new_connection = true;
  const tTransactionPacket* b = static_cast<const tTransactionPacket*>(input->GetPullLockedUnsafe(false));
  this->ProcessPacket(b);
  b->GetManager()->GetCurrentRefCounter()->ReleaseLock();
  handling_new_connection = false;
}

template<typename B>
tTransactionalData<B>::tTDPortSet::tTDPortSet(tTransactionalData* const outer_class_ptr_, const util::tString& description) :
    tFrameworkElement(description),
    outer_class_ptr(outer_class_ptr_)
{
}

template<typename B>
tTransactionalData<B>::tInputTransactions::tInputTransactions(tTransactionalData* const outer_class_ptr_, tPortCreationInfo pci) :
    tInputTransactionStreamPort<B>("input transactions", pci, outer_class_ptr_),
    outer_class_ptr(outer_class_ptr_)
{
}

} // namespace finroc
} // namespace core

