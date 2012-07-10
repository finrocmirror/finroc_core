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
#include "rrlib/finroc_core_utils/log/tLogUser.h"
#include "rrlib/finroc_core_utils/tGarbageDeleter.h"
#include "rrlib/serialization/tOutputStream.h"
#include <boost/lexical_cast.hpp>

#include "core/tFrameworkElement.h"
#include "core/tRuntimeEnvironment.h"
#include "core/tRuntimeListener.h"
#include "core/port/tAbstractPort.h"

namespace finroc
{
namespace core
{
tFrameworkElement::tFrameworkElement(tFrameworkElement* parent_, const util::tString& name, uint flags, int lock_order) :
  rrlib::thread::tRecursiveMutex("Framework Element", GetLockOrder(flags, parent_, lock_order), (flags & tCoreFlags::cIS_RUNTIME) ? util::tInteger::cMIN_VALUE : tRuntimeEnvironment::GetInstance()->RegisterElement(*this, (flags & tCoreFlags::cIS_PORT))),
  primary(*this),
  creater_thread_uid(rrlib::thread::tThread::CurrentThreadId()),
  const_flags(flags & tCoreFlags::cCONSTANT_FLAGS),
  flags(flags & tCoreFlags::cNON_CONSTANT_FLAGS),
  children(GetFlag(tCoreFlags::cALLOWS_CHILDREN) ? 4 : 0)
{
  assert((flags & tCoreFlags::cSTATUS_FLAGS) == 0);

  primary.name = name;

  if (!GetFlag(tCoreFlags::cIS_RUNTIME))
  {
    tFrameworkElement* parent = (parent_) ? parent_ : tRuntimeEnvironment::GetInstance()->unrelated;
    if (lock_order < 0)
    {
      lock_order = parent->GetLockOrder() + 1;
    }
    parent->AddChild(primary);
  }

  FINROC_LOG_PRINT_TO(framework_elements, rrlib::logging::eLL_DEBUG_VERBOSE_1, "Constructing FrameworkElement");
}

void tFrameworkElement::AddChild(tLink& child)
{
  if (child.parent == this)
  {
    return;
  }

  // lock runtime (required to perform structural changes)
  tLock lock2(GetRegistryLock());

  // perform checks
  assert(child.GetChild().IsConstructing() && "tree structure is fixed for initialized children - is child initialized twice (?)");
  assert(child.GetChild().IsCreator() && "may only be called by child creator thread");
  if (IsDeleted() || (child.parent && child.parent->IsDeleted()) || child.GetChild().IsDeleted())
  {
    throw util::tRuntimeException("Child has been deleted or has deleted parent. Thread exit is likely the intended behaviour.", CODE_LOCATION_MACRO);
  }
  if (child.parent != NULL)
  {
    assert(child.GetChild().LockAfter(*child.parent) && "lockOrder level of child needs to be higher than of former parent");
  }
  assert(child.GetChild().LockAfter(*this) && "lockOrder level of child needs to be higher than of parent");
  // avoid cycles
  assert(&child.GetChild() != this);
  assert(!this->IsChildOf(child.GetChild()));

  // detach from former parent
  if (child.parent)
  {
    //assert(!child.parent.isInitialized()) : "This is truly strange - should not happen";
    child.parent->children.Remove(&child);
  }

  // Check if child with same name already exists and possibly rename?
  if (GetFlag(tCoreFlags::cAUTO_RENAME) && (!GetFlag(tCoreFlags::cIS_PORT)) && GetChild(child.GetName()))
  {
    // Ok, rename
    char pointer_buffer[100];
    sprintf(pointer_buffer, " (%p)", &child.GetChild());
    child.GetChild().SetName(child.GetChild().GetName() + pointer_buffer);
    while (GetChild(child.GetName()))
    {
      FINROC_LOG_PRINT(rrlib::logging::eLL_DEBUG_WARNING, "Spooky framework elements name duplicates: " + child.GetName());
      child.GetChild().SetName(child.GetChild().GetName() + pointer_buffer);
    }
  }

  child.parent = this;
  children.Add(&child, false);
  // child.init(); - do this separately
}

bool tFrameworkElement::AllParentsReady()
{
  if (GetFlag(tCoreFlags::cIS_RUNTIME))
  {
    return true;
  }
  for (tLink* l = &(primary); l != NULL; l = l->next)
  {
    if (l->GetParent() == NULL || (!l->GetParent()->IsReady()))
    {
      return false;
    }
    if (!l->GetParent()->AllParentsReady())
    {
      return false;
    }
  }
  return true;
}

void tFrameworkElement::CheckPublish()
{
  if (IsReady())
  {
    if (!GetFlag(tCoreFlags::cPUBLISHED) && AllParentsReady())
    {
      SetFlag(tCoreFlags::cPUBLISHED);
      FINROC_LOG_PRINT_TO(framework_elements, rrlib::logging::eLL_DEBUG_VERBOSE_1, "Publishing");
      PublishUpdatedInfo(tRuntimeListener::cADD);
    }

    // publish any children?
    if (GetFlag(tCoreFlags::cPUBLISHED))
    {
      util::tArrayWrapper<tLink*>* iterable = children.GetIterable();
      for (int i = 0, n = iterable->Size(); i < n; i++)
      {
        tLink* child = iterable->Get(i);
        if (child != NULL && (!child->GetChild().IsDeleted()))
        {
          child->GetChild().CheckPublish();
        }
      }
    }
  }

}

size_t tFrameworkElement::ChildCount() const
{
  int count = 0;
  util::tArrayWrapper<tLink*>* iterable = children.GetIterable();
  for (int i = 0, n = iterable->Size(); i < n; i++)
  {
    if (iterable->Get(i) != NULL)
    {
      count++;
    }
  }
  return count;
}

tFrameworkElement::~tFrameworkElement()
{
  assert(((GetFlag(tCoreFlags::cDELETED) || GetFlag(tCoreFlags::cIS_RUNTIME))) && "Frameworkelement was not deleted with managedDelete()");
  FINROC_LOG_PRINT_TO(framework_elements, rrlib::logging::eLL_DEBUG_VERBOSE_1, "FrameworkElement destructor");
  if (!GetFlag(tCoreFlags::cIS_RUNTIME))
  {
    // synchronizes on runtime - so no elements will be deleted while runtime is locked
    tRuntimeEnvironment::GetInstance()->UnregisterElement(*this);
  }

  // delete links
  tLink* l = primary.next;
  while (l != NULL)
  {
    tLink* tmp = l;
    l = l->next;
    delete tmp;
  }
}

void tFrameworkElement::CheckForNameClash(const tLink& link)
{
  if (!tRuntimeSettings::DuplicateQualifiedNamesAllowed() && link.parent && (!link.GetChild().GetFlag(tCoreFlags::cNETWORK_ELEMENT))) // we cannot influence naming of elements in other runtime environments
  {
    util::tArrayWrapper<tLink*>* iterable = link.parent->children.GetIterable();
    for (int i = 0, n = iterable->Size(); i < n; i++)
    {
      tLink* child = iterable->Get(i);
      if (child && child->GetChild().IsReady() && child->GetName().compare(primary.GetName()) == 0)
      {
        FINROC_LOG_PRINT(rrlib::logging::eLL_ERROR, "Framework elements with the same qualified names are not allowed ('", child->GetChild().GetQualifiedName(),
                         "'), since this causes undefined behavior with port connections by qualified names (e.g. in fingui or in finstructable groups). Apart from manually choosing another name, there are two ways to solve this:\n",
                         "  1) Set the tCoreFlags::cAUTO_RENAME flag constructing parent framework element.\n",
                         "  2) Explicitly allow duplicate names by calling tRuntimeSettings::AllowDuplicateQualifiedNames() and be careful.");
        abort();
      }
    }
  }
}

void tFrameworkElement::DeleteChildren()
{
  util::tArrayWrapper<tLink*>* iterable = children.GetIterable();
  for (int i = 0, n = iterable->Size(); i < n; i++)
  {
    tLink* child = iterable->Get(i);
    if (child != NULL)
    {
      child->GetChild().ManagedDelete(child);
    }
  }

  tLock lock(GetRegistryLock());
  children.Clear();
}

tFrameworkElement* tFrameworkElement::GetChild(const util::tString& name) const
{
  util::tArrayWrapper<tLink*>* iterable = children.GetIterable();
  for (int i = 0, n = iterable->Size(); i < n; i++)
  {
    tLink* child = iterable->Get(i);
    if (child && child->GetChild().IsReady())
    {
      if (boost::equals(child->GetName(), name))
      {
        return &child->GetChild();
      }
    }
    else if (child)
    {
      tLock lock4(GetRegistryLock());
      if (IsDeleted())
      {
        return NULL;
      }
      if (child->GetChild().IsDeleted())
      {
        continue;
      }
      if (boost::equals(child->GetName(), name))
      {
        return &child->GetChild();
      }
    }
  }
  return NULL;
}

tFrameworkElement* tFrameworkElement::GetChildElement(const util::tString& name, bool only_globally_unique_children)
{
  return GetChildElement(name, 0, only_globally_unique_children, *tRuntimeEnvironment::GetInstance());
}

tFrameworkElement* tFrameworkElement::GetChildElement(const util::tString& name, int name_index, bool only_globally_unique_children, tFrameworkElement& root)
{
  // lock runtime (might not be absolutely necessary... ensures, however, that result is valid)
  tLock lock2(GetRegistryLock());

  if (IsDeleted())
  {
    return NULL;
  }

  if (name[name_index] == '/')
  {
    return root.GetChildElement(name, name_index + 1, only_globally_unique_children, root);
  }

  only_globally_unique_children &= (!GetFlag(tCoreFlags::cGLOBALLY_UNIQUE_LINK));
  util::tArrayWrapper<tLink*>* iterable = children.GetIterable();
  for (int i = 0, n = iterable->Size(); i < n; i++)
  {
    tLink* child = iterable->Get(i);
    if (child != NULL && name.compare(name_index, child->name.length(), child->name) == 0 && (!child->GetChild().IsDeleted()))
    {
      if (name.length() == name_index + child->name.length())
      {
        if (!only_globally_unique_children || child->GetChild().GetFlag(tCoreFlags::cGLOBALLY_UNIQUE_LINK))
        {
          return &child->GetChild();
        }
      }
      if (name[name_index + child->name.length()] == '/')
      {
        tFrameworkElement* result = child->GetChild().GetChildElement(name, name_index + child->name.length() + 1, only_globally_unique_children, root);
        if (result != NULL)
        {
          return result;
        }
        // continue, because links may contain '/'... (this is slightly ugly... better solution? TODO)
      }
    }
  }
  return NULL;
}

const char* tFrameworkElement::GetCName() const
{
  return primary.name.length() == 0 ? "(anonymous)" : primary.name.c_str();
}

const tFrameworkElement::tLink* tFrameworkElement::GetLink(size_t link_index) const
{
  tLock lock2(GetRegistryLock());  // absolutely safe this way
  if (IsDeleted())
  {
    return NULL;
  }
  const tLink* l = &(primary);
  for (size_t i = 0u; i < link_index; i++)
  {
    l = l->next;
    if (l == NULL)
    {
      return NULL;
    }
  }
  return l;
}

size_t tFrameworkElement::GetLinkCount() const
{
  if (IsReady())
  {
    return GetLinkCountHelper();
  }
  else
  {
    tLock lock3(GetRegistryLock());  // absolutely safe this way
    return GetLinkCountHelper();
  }
}

size_t tFrameworkElement::GetLinkCountHelper() const
{
  if (IsDeleted())
  {
    return 0u;
  }
  size_t i = 0u;
  for (const tLink* l = &(primary); l != NULL; l = l->next)
  {
    i++;
  }
  return i;
}

tFrameworkElement::tLink* tFrameworkElement::GetLinkInternal(size_t link_index)
{
  tLink* l = &(primary);
  for (size_t i = 0u; i < link_index; i++)
  {
    l = l->next;
    if (l == NULL)
    {
      return NULL;
    }
  }
  return l;
}

int tFrameworkElement::GetLockOrder(uint flags_, tFrameworkElement* parent, int lock_order)
{
  if ((flags_ & tCoreFlags::cIS_RUNTIME) == 0)
  {
    parent = (parent != NULL) ? parent : tRuntimeEnvironment::GetInstance()->unrelated;
    if (lock_order < 0)
    {
      return parent->GetLockOrder() + 1;
    }
    return lock_order;
  }
  else
  {
    return lock_order;
  }
}

const util::tString tFrameworkElement::GetName() const
{
  if (IsReady() || GetFlag(tCoreFlags::cIS_RUNTIME))
  {
    return primary.name.length() == 0 ? "(anonymous)" : primary.name;
  }
  else
  {
    tLock lock3(GetRegistryLock());  // synchronize, while name can be changed (C++ strings may not be thread safe...)
    if (IsDeleted())
    {
      return "(deleted element)";
    }
    return primary.name.length() == 0 ? "(anonymous)" : primary.name;
  }
}

void tFrameworkElement::GetNameHelper(util::tString& sb, const tLink& l, bool abort_at_link_root)
{
  if (l.parent == NULL || (abort_at_link_root && l.GetChild().GetFlag(tCoreFlags::cALTERNATE_LINK_ROOT)))    // runtime?
  {
    return;
  }
  GetNameHelper(sb, l.parent->primary, abort_at_link_root);
  sb.append("/");
  sb.append(l.name);
}

tFrameworkElement* tFrameworkElement::GetParent(int link_index) const
{
  tLock lock2(GetRegistryLock());  // absolutely safe this way
  if (IsDeleted())
  {
    return NULL;
  }
  return GetLink(link_index)->parent;
}

tFrameworkElement* tFrameworkElement::GetParentWithFlags(uint flags_) const
{
  if (primary.parent == NULL)
  {
    return NULL;
  }
  tLock lock2(GetRegistryLock());  // not really necessary after element has been initialized
  if (IsDeleted())
  {
    return NULL;
  }

  tFrameworkElement* result = primary.parent;
  while (!((result->GetAllFlags() & flags_) == flags_))
  {
    result = result->primary.parent;
    if (result == NULL || result->IsDeleted())
    {
      break;
    }
  }
  return result;
}

bool tFrameworkElement::GetQualifiedName(util::tString& sb, const tLink& start, bool force_full_link) const
{
  if (IsReady())
  {
    return GetQualifiedNameImpl(sb, start, force_full_link);
  }
  else
  {
    tLock lock3(GetRegistryLock());  // synchronize while element is under construction
    return GetQualifiedNameImpl(sb, start, force_full_link);
  }
}

bool tFrameworkElement::GetQualifiedNameImpl(util::tString& sb, const tLink& start, bool force_full_link) const
{
  size_t length = 0u;
  bool abort_at_link_root = false;
  for (const tLink* l = &start; l->parent != NULL && !(abort_at_link_root && l->GetChild().GetFlag(tCoreFlags::cALTERNATE_LINK_ROOT)); l = &(l->parent->primary))
  {
    abort_at_link_root |= (!force_full_link) && l->GetChild().GetFlag(tCoreFlags::cGLOBALLY_UNIQUE_LINK);
    if (abort_at_link_root && l->GetChild().GetFlag(tCoreFlags::cALTERNATE_LINK_ROOT))    // if unique_link element is at the same time a link root
    {
      break;
    }
    length += l->name.length() + 1u;
  }
  sb.clear();
  if (sb.capacity() < length)
  {
    sb.reserve(length);
  }

  GetNameHelper(sb, start, abort_at_link_root);
  assert(sb.length() == length);

  // remove any characters if buffer is too long
  //      if (len2 < sb.length()) {
  //          sb.delete(len2, sb.length());
  //      }
  return abort_at_link_root;
}

const rrlib::thread::tRecursiveMutex& tFrameworkElement::GetRegistryLock() const
{
  return tRuntimeEnvironment::GetInstance()->GetRegistryHelper().mutex;
}

tRuntimeEnvironment& tFrameworkElement::GetRuntime() const
{
  //return getParent(RuntimeEnvironment.class);
  return *tRuntimeEnvironment::GetInstance();
}

void tFrameworkElement::Init()
{
  tLock lock2(GetRuntime().GetRegistryLock());
  //SimpleList<FrameworkElement> publishThese = new SimpleList<FrameworkElement>();
  // assert(getFlag(CoreFlags.IS_RUNTIME) || getParent().isReady());
  if (IsDeleted())
  {
    throw util::tRuntimeException("Cannot initialize deleted element", CODE_LOCATION_MACRO);
  }

  InitImpl();

  CheckPublish();

  /*for (@SizeT int i = 0, n = publishThese.size(); i < n; i++) {
      publishThese.get(i).publishUpdatedInfo(RuntimeListener.ADD);
  }*/
}

void tFrameworkElement::InitAll()
{
  tRuntimeEnvironment::GetInstance()->Init();
}

void tFrameworkElement::InitImpl()
{
  assert((!IsDeleted()) && "Deleted element cannot be reinitialized");
  bool init_this = !IsReady() && IsCreator();

  // Call pre-child-init callbacks and check for name clash
  if (init_this)
  {
    PreChildInit();
    tRuntimeEnvironment::GetInstance()->PreElementInit(*this);
    CheckForNameClash(primary);
  }

  // Initialize children and check for name clashes
  if (init_this || IsReady())
  {
    util::tArrayWrapper<tLink*>* iterable = children.GetIterable();
    for (int i = 0, n = iterable->Size(); i < n; i++)
    {
      tLink* child = iterable->Get(i);
      if (child && child->IsPrimaryLink() && (!child->GetChild().IsDeleted()))
      {
        child->GetChild().InitImpl();
      }
    }
  }

  // Call post-child-init callbacks and set READY flag
  if (init_this)
  {
    PostChildInit();
    {
      tLock lock3(simple_mutex);
      flags |= tCoreFlags::cREADY;
    }

    NotifyAnnotationsInitialized();
  }
}

bool tFrameworkElement::IsChildOf(const tFrameworkElement& re, bool ignore_delete_flag) const
{
  tLock lock2(GetRegistryLock());  // absolutely safe this way
  if ((!ignore_delete_flag) && IsDeleted())
  {
    return false;
  }
  for (const tLink* l = &(primary); l != NULL; l = l->next)
  {
    if (l->parent == &re)
    {
      return true;
    }
    else if (l->parent == NULL)
    {
      continue;
    }
    else
    {
      if (l->parent->IsChildOf(re))
      {
        return true;
      }
    }
  }
  return false;
}

void tFrameworkElement::Link(tFrameworkElement& parent, const util::tString& link_name)
{
  assert(IsCreator() && "may only be called by creator thread");
  assert(LockAfter(parent));

  // lock runtime (required to perform structural changes)
  tLock lock2(GetRegistryLock());
  if (IsDeleted() || parent.IsDeleted())
  {
    throw util::tRuntimeException("Element and/or parent has been deleted. Thread exit is likely the intended behaviour.", CODE_LOCATION_MACRO);
  }

  tLink* l = new tLink(*this);
  l->name = link_name;
  l->parent = NULL;  // will be set in addChild
  tLink* lprev = GetLinkInternal(GetLinkCount() - 1u);
  assert(lprev->next == NULL);
  lprev->next = l;
  CheckForNameClash(*l);
  parent.AddChild(*l);
  //RuntimeEnvironment.getInstance().link(this, linkName);
}

void tFrameworkElement::ManagedDelete(tLink* dont_detach)
{
  {
    tLock lock2(RuntimeLockHelper());
    {
      tLock lock3(*this);

      if (IsDeleted())    // can happen if two threads delete concurrently - no problem, since this is - if at all - called when GarbageCollector-safety period has just started
      {
        return;
      }

      FINROC_LOG_PRINT_TO(framework_elements, rrlib::logging::eLL_DEBUG_VERBOSE_1, "FrameworkElement managedDelete");

      // synchronizes on runtime - so no elements will be deleted while runtime is locked
      {
        tLock lock4(GetRegistryLock());

        NotifyAnnotationsDelete();

        FINROC_LOG_PRINT_TO(framework_elements, rrlib::logging::eLL_DEBUG_VERBOSE_1, "Deleting");
        //System.out.println("Deleting " + toString() + " (" + hashCode() + ")");
        assert(!GetFlag(tCoreFlags::cDELETED));
        assert(((primary.GetParent() != NULL) | GetFlag(tCoreFlags::cIS_RUNTIME)));

        {
          tLock lock5(simple_mutex);
          flags = (flags | tCoreFlags::cDELETED) & ~tCoreFlags::cREADY;
        }

        if (!GetFlag(tCoreFlags::cIS_RUNTIME))
        {
          tRuntimeEnvironment::GetInstance()->MarkElementDeleted(*this);
        }
      }

      // perform custom cleanup (stopping/deleting threads can be done here)
      PrepareDelete();

      // remove children (thread-safe, because delete flag is set - and addChild etc. checks that)
      DeleteChildren();

      // synchronized on runtime for removement from hierarchy
      {
        tLock lock4(GetRegistryLock());

        // remove element itself
        PublishUpdatedInfo(tRuntimeListener::cREMOVE);

        // remove from hierarchy
        for (tLink* l = &(primary); l != NULL;)
        {
          if (l != dont_detach && l->parent != NULL)
          {
            l->parent->children.Remove(l);
          }
          l = l->next;
        }

        primary.parent = NULL;

      }
    }
  }

  // add garbage collector task
  util::tGarbageDeleter::DeleteDeferred(this);
}

bool tFrameworkElement::NameEquals(const util::tString& other) const
{
  if (IsReady())
  {
    return primary.name.compare(other) == 0;
  }
  else
  {
    tLock lock3(GetRegistryLock());
    if (IsDeleted())
    {
      return false;
    }
    return primary.name.compare(other) == 0;
  }
}

void tFrameworkElement::PrintStructure(rrlib::logging::tLogLevel ll)
{
  std::stringstream ls;
  ls << "" << std::endl;
  PrintStructure(0, ls);
  FINROC_LOG_PRINT(ll, ls.str());
}

void tFrameworkElement::PrintStructure(int indent, std::stringstream& output)
{
  tLock lock2(GetRegistryLock());

  // print element info
  for (int i = 0; i < indent; i++)
  {
    output << " ";
  }

  if (IsDeleted())
  {
    output << "deleted FrameworkElement" << std::endl;
    return;
  }

  output << GetCName() << " (" << (IsReady() ? (GetFlag(tCoreFlags::cPUBLISHED) ? "published" : "ready") : IsDeleted() ? "deleted" : "constructing") << ")" << std::endl;

  // print child element info
  util::tArrayWrapper<tLink*>* iterable = children.GetIterable();
  for (int i = 0, n = iterable->Size(); i < n; i++)
  {
    tLink* child = iterable->Get(i);
    if (child != NULL)
    {
      child->GetChild().PrintStructure(indent + 2, output);
    }
  }
}

void tFrameworkElement::PublishUpdatedEdgeInfo(int8 change_type, tAbstractPort& target)
{
  if (GetFlag(tCoreFlags::cPUBLISHED))
  {
    tRuntimeEnvironment::GetInstance()->RuntimeChange(change_type, *this, &target);
  }
}

void tFrameworkElement::PublishUpdatedInfo(int8 change_type)
{
  if (change_type == tRuntimeListener::cADD || GetFlag(tCoreFlags::cPUBLISHED))
  {
    tRuntimeEnvironment::GetInstance()->RuntimeChange(change_type, *this, NULL);
  }
}

void tFrameworkElement::RemoveFlag(int flag)
{
  tLock lock2(simple_mutex);
  assert((flag & tCoreFlags::cNON_CONSTANT_FLAGS) != 0);
  flags &= ~flag;
}

const rrlib::thread::tRecursiveMutex& tFrameworkElement::RuntimeLockHelper() const
{
  if (ValidAfter(GetRuntime().GetRegistryHelper().mutex))
  {
    return GetRegistryLock();
  }

  return *this;
}

void tFrameworkElement::SetFlag(int flag, bool value)
{
  if (value)
  {
    SetFlag(flag);
  }
  else
  {
    RemoveFlag(flag);
  }
}

void tFrameworkElement::SetFlag(int flag)
{
  tLock lock2(simple_mutex);
  assert((flag & tCoreFlags::cCONSTANT_FLAGS) == 0);
  flags |= flag;
}

void tFrameworkElement::SetName(const util::tString& name)
{
  assert((!GetFlag(tCoreFlags::cIS_RUNTIME)));

  tLock lock2(GetRegistryLock());  // synchronize, C++ strings may not be thread safe...
  assert(IsConstructing());
  assert(IsCreator());
  primary.name = name;
}

void tFrameworkElement::WriteName(rrlib::serialization::tOutputStream& os, int i) const
{
  if (IsReady())
  {
    os.WriteString(GetLink(i)->name);
  }
  else
  {
    tLock lock3(GetRegistryLock());  // synchronize, while name can be changed (C++ strings may not be thread safe...)
    os.WriteString(IsDeleted() ? "deleted element" : GetLink(i)->name);
  }
}

tFrameworkElement::tChildIterator::tChildIterator(const tFrameworkElement& parent, uint flags, uint result, bool only_ready_elements) :
  next_elem(NULL),
  last(NULL),
  flags(0),
  result(0),
  cur_parent(NULL)
{
  Reset(parent, flags, result, only_ready_elements);
}

tFrameworkElement::tChildIterator::tChildIterator(const tFrameworkElement& parent, bool only_ready_elements) :
  next_elem(NULL),
  last(NULL),
  flags(0),
  result(0),
  cur_parent(NULL)
{
  Reset(parent, only_ready_elements);
}


tFrameworkElement* tFrameworkElement::tChildIterator::Next()
{
  while (next_elem <= last)
  {
    tFrameworkElement::tLink* nex = *next_elem;
    if (nex && (nex->GetChild().GetAllFlags() & flags) == result)
    {
      next_elem++;
      return &nex->GetChild();
    }
    next_elem++;
  }

  return NULL;
}

tAbstractPort* tFrameworkElement::tChildIterator::NextPort()
{
  while (true)
  {
    tFrameworkElement* result = Next();
    if (result == NULL)
    {
      return NULL;
    }
    if (result->IsPort())
    {
      return static_cast<tAbstractPort*>(result);
    }
  }
}

void tFrameworkElement::tChildIterator::Reset(const tFrameworkElement& parent, uint flags, uint result, bool only_ready_elements)
{
  this->flags = flags | tCoreFlags::cDELETED;
  this->result = result;
  if (only_ready_elements)
  {
    this->flags |= tCoreFlags::cREADY;
    this->result |= tCoreFlags::cREADY;
  }
  cur_parent = &parent;

  const util::tArrayWrapper<tFrameworkElement::tLink*>* array = parent.GetChildren();
  next_elem = array->GetPointer();
  last = (next_elem + array->Size()) - 1;

}

} // namespace finroc
} // namespace core

