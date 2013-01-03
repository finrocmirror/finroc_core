//
// You received this file as part of Finroc
// A Framework for intelligent robot control
//
// Copyright (C) Finroc GbR (finroc.org)
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
//
//----------------------------------------------------------------------
/*!\file    core/tFrameworkElement.cpp
 *
 * \author  Max Reichardt
 *
 * \date    2012-10-26
 *
 */
//----------------------------------------------------------------------
#include "core/tFrameworkElement.h"

//----------------------------------------------------------------------
// External includes (system with <>, local with "")
//----------------------------------------------------------------------
#include "rrlib/finroc_core_utils/tException.h"
#include "rrlib/thread/tThread.h"
#include <boost/algorithm/string/predicate.hpp>
#include <sstream>

//----------------------------------------------------------------------
// Internal includes with ""
//----------------------------------------------------------------------
#include "core/tRuntimeEnvironment.h"
#include "core/tRuntimeSettings.h"
#include "core/internal/tGarbageDeleter.h"

//----------------------------------------------------------------------
// Debugging
//----------------------------------------------------------------------
#include <cassert>

//----------------------------------------------------------------------
// Namespace usage
//----------------------------------------------------------------------

//----------------------------------------------------------------------
// Namespace declaration
//----------------------------------------------------------------------
namespace finroc
{
namespace core
{

//----------------------------------------------------------------------
// Forward declarations / typedefs / enums
//----------------------------------------------------------------------

//----------------------------------------------------------------------
// Const values
//----------------------------------------------------------------------
constexpr tFrameworkElementFlags tFrameworkElement::cSTATUS_FLAGS;

//----------------------------------------------------------------------
// Implementation
//----------------------------------------------------------------------
tFrameworkElement::tChildSet tFrameworkElement::empty_child_set;

tFrameworkElement::tFrameworkElement(tFrameworkElement* parent, const tString& name, tFlags flags, int lock_order) :
  tAnnotatable<rrlib::thread::tRecursiveMutex>("Framework Element", GetLockOrder(flags, parent, lock_order),
      flags.Get(tFlag::RUNTIME) ? std::numeric_limits<int>::min() : tRuntimeEnvironment::GetInstance().RegisterElement(*this, flags.Get(tFlag::PORT))),
  primary(*this),
#ifndef RRLIB_SINGLE_THREADED
  creater_thread_uid(rrlib::thread::tThread::CurrentThreadId()),
#endif
  flags(flags),
  children(flags.Get(tFlag::PORT) ? &empty_child_set : new tChildSet())
{
  if (flags.Raw() & cSTATUS_FLAGS.Raw())
  {
    FINROC_LOG_PRINT(ERROR, "No status flags may be set in constructor");
    abort();
  }
  primary.name = name;

  if (!GetFlag(tFlag::RUNTIME))
  {
    parent->AddChild(primary);
  }

  FINROC_LOG_PRINT(DEBUG_VERBOSE_1, "Constructing tFrameworkElement (" , this, ")");
}

tFrameworkElement::~tFrameworkElement()
{
  assert((GetFlag(tFlag::DELETED) || GetFlag(tFlag::RUNTIME)) && "Framework element was not deleted with ManagedDelete()");
  FINROC_LOG_PRINT(DEBUG_VERBOSE_1, "FrameworkElement destructor (" , this, ")");
  if (!GetFlag(tFlag::RUNTIME))
  {
    // synchronizes on runtime - so no elements will be deleted while runtime is locked
    GetRuntime().UnregisterElement(*this);
  }

  // delete links
  tLink* l = primary.next;
  while (l != NULL)
  {
    tLink* tmp = l;
    l = l->next;
    delete tmp;
  }

  // delete child list
  if (children != &empty_child_set)
  {
    delete children;
  }
}

void tFrameworkElement::AddChild(tLink& child)
{
  if (IsPort())
  {
    FINROC_LOG_PRINT(ERROR, "Cannot add children to port");
    return;
  }

  if (child.parent == this)
  {
    FINROC_LOG_PRINT(ERROR, "Attempt to add element as a child of itself");
    return;
  }

  // lock runtime (required to perform structural changes)
  tLock lock(GetStructureMutex());

  // perform checks
  assert(child.GetChild().IsConstructing() && "tree structure is fixed for initialized children - is child initialized twice (?)");
  assert(child.GetChild().IsCreator() && "may only be called by child creator thread");
  if (IsDeleted() || (child.parent && child.parent->IsDeleted()) || child.GetChild().IsDeleted())
  {
    throw util::tRuntimeException("Child has been deleted or has deleted parent. Thread exit is probably the safest behaviour.", CODE_LOCATION_MACRO);
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
    child.parent->children->Remove(&child);
  }

  // Check if child with same name already exists and possibly rename?
  if (GetFlag(tFlag::AUTO_RENAME) && (!GetFlag(tFlag::PORT)) && GetChild(child.GetName()))
  {
    // Ok, rename
    char pointer_buffer[100];
    sprintf(pointer_buffer, " (%p)", &child.GetChild());
    child.GetChild().SetName(child.GetChild().GetName() + pointer_buffer);
    while (GetChild(child.GetName()))
    {
      FINROC_LOG_PRINT(DEBUG_WARNING, "Spooky framework elements name duplicates: " + child.GetName());
      child.GetChild().SetName(child.GetChild().GetName() + pointer_buffer);
    }
  }

  child.parent = this;
  children->Add(&child);
  // child.init(); - do this separately
}

bool tFrameworkElement::AllParentsReady() const
{
  if (GetFlag(tFlag::RUNTIME))
  {
    return true;
  }
  for (const tLink* l = &(primary); l != NULL; l = l->next)
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
    if (!GetFlag(tFlag::PUBLISHED) && AllParentsReady())
    {
      SetFlag(tFlag::PUBLISHED);
      FINROC_LOG_PRINT(DEBUG_VERBOSE_1, "Publishing");
      PublishUpdatedInfo(tRuntimeListener::tEvent::ADD);
    }

    // publish any children?
    if (GetFlag(tFlag::PUBLISHED))
    {
      for (auto it = ChildrenBegin(); it != ChildrenEnd(); ++it)
      {
        if (!it->IsDeleted())
        {
          it->CheckPublish();
        }
      }
    }
  }
}

void tFrameworkElement::CheckForNameClash(const tLink& link) const
{
  if (!tRuntimeSettings::DuplicateQualifiedNamesAllowed() && link.parent && (!link.GetChild().GetFlag(tFlag::NETWORK_ELEMENT))) // we cannot influence naming of elements in other runtime environments
  {
    for (auto it = link.parent->ChildrenBegin(); it != link.parent->ChildrenEnd(); ++it)
    {
      if (it->IsReady() && it->GetName().compare(primary.GetName()) == 0)
      {
        FINROC_LOG_PRINT(ERROR, "Framework elements with the same qualified names are not allowed ('", it->GetQualifiedName(),
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
  for (auto it = children->Begin(); it != children->End(); ++it)
  {
    tLink* child = *it;
    child->GetChild().ManagedDelete(child);
  }

  tLock lock(GetStructureMutex());
  children->Clear();
}

tFrameworkElement* tFrameworkElement::GetChild(const tString& name) const
{
  for (auto it = ChildrenBegin(); it != ChildrenEnd(); ++it)
  {
    if (it->IsReady())
    {
      if (boost::equals(it->GetName(), name))
      {
        return &(*it);
      }
    }
    else
    {
      tLock lock(GetStructureMutex());
      if (IsDeleted())
      {
        return NULL;
      }
      if (it->IsDeleted())
      {
        continue;
      }
      if (boost::equals(it->GetName(), name))
      {
        return &(*it);
      }
    }
  }
  return NULL;
}

tFrameworkElement* tFrameworkElement::GetChildElement(const tString& name, bool only_globally_unique_children)
{
  return GetChildElement(name, 0, only_globally_unique_children, GetRuntime());
}

tFrameworkElement* tFrameworkElement::GetChildElement(const tString& name, int name_index, bool only_globally_unique_children, tFrameworkElement& root)
{
  // lock runtime (might not be absolutely necessary... ensures, however, that result is valid)
  tLock lock(GetStructureMutex());

  if (IsDeleted())
  {
    return NULL;
  }

  if (name[name_index] == '/')
  {
    return root.GetChildElement(name, name_index + 1, only_globally_unique_children, root);
  }

  only_globally_unique_children &= (!GetFlag(tFlag::GLOBALLY_UNIQUE_LINK));
  for (auto it = children->Begin(); it != children->End(); ++it)
  {
    tLink* child = &(**it);
    if (name.compare(name_index, child->name.length(), child->name) == 0 && (!child->GetChild().IsDeleted()))
    {
      if (name.length() == name_index + child->name.length())
      {
        if (!only_globally_unique_children || child->GetChild().GetFlag(tFlag::GLOBALLY_UNIQUE_LINK))
        {
          return &child->GetChild();
        }
      }
      if (name[name_index + child->name.length()] == '/')
      {
        tFrameworkElement* result = child->GetChild().GetChildElement(name, name_index + child->name.length() + 1, only_globally_unique_children, root);
        if (result)
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
  tLock lock(GetStructureMutex());  // absolutely safe this way
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
    tLock lock(GetStructureMutex());  // absolutely safe this way
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

int tFrameworkElement::GetLockOrder(tFlags flags, tFrameworkElement*& parent, int lock_order)
{
  if (!flags.Get(tFlag::RUNTIME))
  {
    parent = parent ? parent : GetRuntime().unrelated;
    if (lock_order < 0)
    {
      return parent->GetLockOrder() + 1;
    }
  }
  return lock_order;
}

tString tFrameworkElement::GetName() const
{
  if (IsReady() || GetFlag(tFlag::RUNTIME))
  {
    return primary.name.length() == 0 ? "(anonymous)" : primary.name;
  }
  else
  {
    tLock lock(GetStructureMutex());  // synchronize, while name can be changed (C++ strings may not be thread safe...)
    if (IsDeleted())
    {
      return "(deleted element)";
    }
    return primary.name.length() == 0 ? "(anonymous)" : primary.name;
  }
}

void tFrameworkElement::GetNameHelper(tString& sb, const tLink& l, bool abort_at_link_root)
{
  if (l.parent == NULL || (abort_at_link_root && l.GetChild().GetFlag(tFlag::ALTERNATIVE_LINK_ROOT)))    // runtime?
  {
    return;
  }
  GetNameHelper(sb, l.parent->primary, abort_at_link_root);
  sb.append("/");
  sb.append(l.name);
}

tFrameworkElement* tFrameworkElement::GetParent(int link_index) const
{
  tLock lock(GetStructureMutex());  // absolutely safe this way
  if (IsDeleted())
  {
    return NULL;
  }
  return GetLink(link_index)->parent;
}

tFrameworkElement* tFrameworkElement::GetParentWithFlags(tFlags parent_flags) const
{
  if (primary.parent == NULL)
  {
    return NULL;
  }
  tLock lock(GetStructureMutex());  // not really necessary after element has been initialized
  if (IsDeleted())
  {
    return NULL;
  }

  tFrameworkElement* result = primary.parent;
  while (!((result->GetAllFlags().Raw() & parent_flags.Raw()) == parent_flags.Raw()))
  {
    result = result->primary.parent;
    if (result == NULL || result->IsDeleted())
    {
      break;
    }
  }
  return result;
}

bool tFrameworkElement::GetQualifiedName(tString& sb, const tLink& start, bool force_full_link) const
{
  if (IsReady())
  {
    return GetQualifiedNameImpl(sb, start, force_full_link);
  }
  else
  {
    tLock lock(GetStructureMutex());  // synchronize while element is under construction
    return GetQualifiedNameImpl(sb, start, force_full_link);
  }
}

bool tFrameworkElement::GetQualifiedNameImpl(tString& sb, const tLink& start, bool force_full_link) const
{
  size_t length = 0u;
  bool abort_at_link_root = false;
  for (const tLink* l = &start; l->parent != NULL && !(abort_at_link_root && l->GetChild().GetFlag(tFlag::ALTERNATIVE_LINK_ROOT)); l = &(l->parent->primary))
  {
    abort_at_link_root |= (!force_full_link) && l->GetChild().GetFlag(tFlag::GLOBALLY_UNIQUE_LINK);
    if (abort_at_link_root && l->GetChild().GetFlag(tFlag::ALTERNATIVE_LINK_ROOT))    // if unique_link element is at the same time a link root
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

tRuntimeEnvironment& tFrameworkElement::GetRuntime()
{
  //return getParent(RuntimeEnvironment.class);
  return tRuntimeEnvironment::GetInstance();
}

rrlib::thread::tRecursiveMutex& tFrameworkElement::GetStructureMutex() const
{
  return GetRuntime().GetStructureMutex();
}

void tFrameworkElement::Init()
{
  tLock lock(GetStructureMutex());
  //SimpleList<FrameworkElement> publishThese = new SimpleList<FrameworkElement>();
  // assert(getFlag(CoreFlags.IS_RUNTIME) || getParent().isReady());
  if (IsDeleted())
  {
    throw util::tRuntimeException("Cannot initialize deleted element", CODE_LOCATION_MACRO);
  }

  InitImplementation();

  CheckPublish();

  /*for (@SizeT int i = 0, n = publishThese.size(); i < n; i++) {
      publishThese.get(i).publishUpdatedInfo(RuntimeListener.ADD);
  }*/
}

void tFrameworkElement::InitAll()
{
  GetRuntime().Init();
}

void tFrameworkElement::InitImplementation()
{
  assert((!IsDeleted()) && "Deleted element cannot be reinitialized");
  bool init_this = !IsReady() && IsCreator();

  // Call pre-child-init callbacks and check for name clash
  if (init_this)
  {
    PreChildInit();
    GetRuntime().PreElementInit(*this);
    CheckForNameClash(primary);
  }

  // Initialize children and check for name clashes
  if (init_this || IsReady())
  {
    for (auto it = children->Begin(); it != children->End(); ++it)
    {
      if ((*it)->IsPrimaryLink() && (!(*it)->GetChild().IsDeleted()))
      {
        (*it)->GetChild().InitImplementation();
      }
    }
  }

  // Call post-child-init callbacks and set READY flag
  if (init_this)
  {
    PostChildInit();
    {
      tLock lock(*this);
      flags |= tFlag::READY;
    }

    NotifyAnnotationsInitialized();
  }
}

bool tFrameworkElement::IsChildOf(const tFrameworkElement& re, bool ignore_delete_flag) const
{
  tLock lock(GetStructureMutex());  // absolutely safe this way
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

bool tFrameworkElement::IsCreator() const
{
#ifndef RRLIB_SINGLE_THREADED
  return rrlib::thread::tThread::CurrentThreadId() == creater_thread_uid;
#else
  return true;
#endif
}

void tFrameworkElement::Link(tFrameworkElement& parent, const tString& link_name)
{
  assert(IsCreator() && "May only be called by creator thread");
  assert(LockAfter(parent));

  // lock runtime (required to perform structural changes)
  tLock lock(GetStructureMutex());
  if (IsDeleted() || parent.IsDeleted())
  {
    throw util::tRuntimeException("Element and/or parent has been deleted. Thread exit is probably the safest behaviour.", CODE_LOCATION_MACRO);
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

      FINROC_LOG_PRINT(DEBUG_VERBOSE_1, "FrameworkElement ManagedDelete");

      // synchronizes on runtime - so no elements will be deleted while runtime is locked
      {
        tLock lock4(GetStructureMutex());

        NotifyAnnotationsDelete();

        FINROC_LOG_PRINT(DEBUG_VERBOSE_1, "Deleting");
        assert(!GetFlag(tFlag::DELETED));
        assert(((primary.GetParent() != NULL) | GetFlag(tFlag::RUNTIME)));
        tFlags new_flags = flags | tFlag::DELETED;
        new_flags.RemoveFlag(tFlag::READY);
        flags = new_flags;

        if (!GetFlag(tFlag::RUNTIME))
        {
          GetRuntime().MarkElementDeleted(*this);
        }
      }

      // perform custom cleanup (stopping/deleting threads can be done here)
      PrepareDelete();

      // remove children (thread-safe, because delete flag is set - and addChild etc. checks that)
      DeleteChildren();

      // synchronized on runtime for removement from hierarchy
      {
        tLock lock4(GetStructureMutex());

        // remove element itself
        PublishUpdatedInfo(tRuntimeListener::tEvent::REMOVE);

        // remove from hierarchy
        for (tLink* l = &(primary); l != NULL;)
        {
          if (l != dont_detach && l->parent != NULL)
          {
            l->parent->children->Remove(l);
          }
          l = l->next;
        }

        primary.parent = NULL;
      }
    }
  }

  // add garbage collector task
  internal::tGarbageDeleter::DeleteDeferred(this);
}

bool tFrameworkElement::NameEquals(const tString& other) const
{
  if (IsReady())
  {
    return primary.name.compare(other) == 0;
  }
  else
  {
    tLock lock(GetStructureMutex());
    if (IsDeleted())
    {
      return false;
    }
    return primary.name.compare(other) == 0;
  }
}

void tFrameworkElement::PrintStructure() const
{
  std::stringstream ls;
  ls << "" << std::endl;
  PrintStructure(0, ls);
  FINROC_LOG_PRINT(USER, ls.str());
}

void tFrameworkElement::PrintStructure(int indent, std::stringstream& output) const
{
  tLock lock(GetStructureMutex());

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

  output << GetCName() << " (" << (IsReady() ? (GetFlag(tFlag::PUBLISHED) ? "published" : "ready") : IsDeleted() ? "deleted" : "constructing") << ")" << std::endl;

  // print child element info
  for (auto it = ChildrenBegin(); it != ChildrenEnd(); ++it)
  {
    it->PrintStructure(indent + 2, output);
  }
}

void tFrameworkElement::PublishUpdatedEdgeInfo(tRuntimeListener::tEvent change_type, tAbstractPort& target)
{
  if (GetFlag(tFlag::PUBLISHED))
  {
    GetRuntime().RuntimeChange(change_type, *this, &target);
  }
}

void tFrameworkElement::PublishUpdatedInfo(tRuntimeListener::tEvent change_type)
{
  if (change_type == tRuntimeListener::tEvent::ADD || GetFlag(tFlag::PUBLISHED))
  {
    GetRuntime().RuntimeChange(change_type, *this, NULL);
  }
}

void tFrameworkElement::RemoveFlag(tFlag flag)
{
  tLock lock2(*this);
  assert(flag >= tFlag::READY);
  flags.RemoveFlag(flag);
}

const rrlib::thread::tRecursiveMutex& tFrameworkElement::RuntimeLockHelper() const
{
  if (ValidAfter(GetStructureMutex()))
  {
    return GetStructureMutex();
  }

  return *this;
}

void tFrameworkElement::SetFlag(tFlag flag, bool value)
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

void tFrameworkElement::SetFlag(tFlag flag)
{
  tLock lock2(*this);
  assert(flag >= tFlag::READY);
  flags |= flag;
}

void tFrameworkElement::SetName(const tString& name)
{
  assert(!GetFlag(tFlag::RUNTIME));

  tLock lock(GetStructureMutex());  // synchronize, C++ strings may not be thread safe...
  assert(IsConstructing());
  assert(IsCreator());
  primary.name = name;
}

tFrameworkElement::tSubElementIterator::tSubElementIterator(tFrameworkElement& framework_element, bool include_root) :
  current_depth(0),
  at_root(include_root ? &framework_element : NULL)
{
  if (!include_root)
  {
    // first element
    iterator_stack[0] = tNoLinkChildIterator(framework_element.children->Begin(), framework_element.children->End());
    if (iterator_stack[0] != tNoLinkChildIterator())
    {
      current_depth = 1;
    }
  }
}

tFrameworkElement::tSubElementIterator& tFrameworkElement::tSubElementIterator::operator++()
{
  if (current_depth || at_root)
  {
    tFrameworkElement& current_element = **this;
    at_root = NULL;
    iterator_stack[current_depth] = tNoLinkChildIterator(current_element.children->Begin(), current_element.children->End());
    if (iterator_stack[current_depth] == tNoLinkChildIterator())
    {
      ++iterator_stack[current_depth - 1];
      while (iterator_stack[current_depth - 1] == tNoLinkChildIterator())
      {
        current_depth--;
        if (current_depth == 0)
        {
          return *this; // end
        }
        ++iterator_stack[current_depth - 1];
      }
    }
    else
    {
      current_depth++;
    }
  }
  return *this;
}

tFrameworkElement::tSubElementIterator tFrameworkElement::tSubElementIterator::operator ++ (int)
{
  tSubElementIterator temp(*this);
  operator++();
  return temp;
}

static void StreamQualifiedName(std::ostream& output, const tFrameworkElement& fe, bool first)
{
  if (!fe.GetFlag(tFrameworkElement::tFlag::RUNTIME))
  {
    StreamQualifiedName(output, *fe.GetParent(), false);
    output << fe.GetCName();
    if (!first)
    {
      output << '/';
    }
  }
}

std::ostream& operator << (std::ostream& output, const tFrameworkElement& fe)
{
  StreamQualifiedName(output, fe, true);
  output << " (" << ((void*)&fe) << ")";
  return output;
}

//----------------------------------------------------------------------
// End of namespace declaration
//----------------------------------------------------------------------
}
}
