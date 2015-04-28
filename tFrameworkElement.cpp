//
// You received this file as part of Finroc
// A framework for intelligent robot control
//
// Copyright (C) Finroc GbR (finroc.org)
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along
// with this program; if not, write to the Free Software Foundation, Inc.,
// 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
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
#include "rrlib/thread/tThread.h"
#include <sstream>

//----------------------------------------------------------------------
// Internal includes with ""
//----------------------------------------------------------------------
#include "core/tFrameworkElementTags.h"
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
typedef rrlib::thread::tLock tLock;

//----------------------------------------------------------------------
// Const values
//----------------------------------------------------------------------
constexpr tFrameworkElementFlags tFrameworkElement::cSTATUS_FLAGS;

const std::string cUNNAMED_ELEMENT_STRING("(Unnamed Framework Element)");
const std::string cDELETED_ELEMENT_STRING("(Deleted Framework Element)");

/*! Maximum depth of framework element hierarchy (introduced so that accidental recursive instantiation does not lead to infinite loops and application hangups) */
const size_t MAX_HIERARCHY_DEPTH = 100;

//----------------------------------------------------------------------
// Implementation
//----------------------------------------------------------------------
tFrameworkElement::tChildSet tFrameworkElement::empty_child_set;

namespace internal
{

/*!
 * Annotation that is created if additional string buffers are needed to
 * store a framework element's name in a thread-safe way.
 */
class tStringBufferAnnotation : public tAnnotation
{
public:

  /*! Additional string buffer */
  tString string_buffer;
};

}

tFrameworkElement::tFrameworkElement(tFrameworkElement* parent, const tString& name, tFlags flags) :
  handle(flags.Get(tFlag::RUNTIME) && (!flags.Get(tFlag::PORT)) ? 0 : tRuntimeEnvironment::GetInstance().RegisterElement(*this, flags.Get(tFlag::PORT))),
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
  if (name.length() > 0)
  {
    primary.name_buffer = name;
    primary.name = &primary.name_buffer;
  }

  if (!IsRuntime())
  {
    if (!parent)
    {
      parent = &(tRuntimeEnvironment::GetInstance().GetElement(tSpecialRuntimeElement::UNRELATED));
    }
    parent->AddChild(primary);
  }

  FINROC_LOG_PRINT(DEBUG_VERBOSE_1, "Constructing tFrameworkElement (" , this, ")");
}

tFrameworkElement::~tFrameworkElement()
{
  if (!(GetFlag(tFlag::DELETED) || IsRuntime()))
  {
    throw rrlib::util::tTraceableException<std::runtime_error>("Framework element '" + GetName() + "' was not deleted with ManagedDelete()");
  }
  FINROC_LOG_PRINT(DEBUG_VERBOSE_1, "FrameworkElement destructor (" , this, ")");
  if (!IsRuntime())
  {
    // synchronizes on runtime - to ensure that no elements (e.g. this one) are deleted while possibly someone else has runtime locked
    rrlib::thread::tLock lock(GetStructureMutex());
    //GetRuntime().UnregisterElement(*this);
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

  // Basic check for hierarchy depth (sufficient to detect common programming errors)
  size_t parent_count = 0;
  tFrameworkElement* parent = this;
  while (parent)
  {
    parent_count++;
    parent = parent->GetParent();
  }
  if (parent_count > MAX_HIERARCHY_DEPTH)
  {
    FINROC_LOG_PRINT(ERROR, "Maximum framework element hierarchy depth (", MAX_HIERARCHY_DEPTH, ") exceeded. This is typically a programming error (e.g. infinite recursive instantiation).");
    abort();
  }

  // lock runtime (required to perform structural changes)
  tLock lock(GetStructureMutex());

  // perform checks
  assert(child.GetChild().IsConstructing() && "tree structure is fixed for initialized children - is child initialized twice (?)");
  assert(child.GetChild().IsCreator() && "may only be called by child creator thread");
  if (IsDeleted() || (child.parent && child.parent->IsDeleted()) || child.GetChild().IsDeleted())
  {
    throw std::runtime_error("Child to add has been deleted or has deleted parent.");
  }

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
  if (IsRuntime())
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
      SetFlag(tFlag::PUBLISHED); // structure mutex acquired
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
                         "  1) Set the tFrameworkElementFlags::AUTO_RENAME flag when constructing parent framework element.\n",
                         "  2) Explicitly allow duplicate names by calling tRuntimeSettings::AllowDuplicateQualifiedNames() and be careful.");
        abort();
      }
    }
  }
}

size_t tFrameworkElement::ChildCount() const
{
  size_t count = 0;
  for (auto it = ChildrenBegin(); it != ChildrenEnd(); ++it)
  {
    count++;
  }
  return count;
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
  for (auto it = children->Begin(); it != children->End(); ++it)
  {
    if ((*it)->GetChild().IsReady())
    {
      if ((*it)->GetName() == name)
      {
        return &(*it)->GetChild();
      }
    }
    else
    {
      tLock lock(GetStructureMutex());
      if (IsDeleted())
      {
        return NULL;
      }
      if ((*it)->GetChild().IsDeleted())
      {
        continue;
      }
      if ((*it)->GetChild().GetName() == name)
      {
        return &(*it)->GetChild();
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
    if (name.compare(name_index, child->name->length(), *(child->name)) == 0 && (!child->GetChild().IsDeleted()))
    {
      if (name.length() == name_index + child->name->length())
      {
        if (!only_globally_unique_children || child->GetChild().GetFlag(tFlag::GLOBALLY_UNIQUE_LINK))
        {
          return &child->GetChild();
        }
      }
      if (name[name_index + child->name->length()] == '/')
      {
        tFrameworkElement* result = child->GetChild().GetChildElement(name, name_index + child->name->length() + 1, only_globally_unique_children, root);
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

void tFrameworkElement::GetNameHelper(tString& sb, const tLink& l, bool abort_at_link_root)
{
  if (l.parent == NULL || (abort_at_link_root && l.GetChild().GetFlag(tFlag::ALTERNATIVE_LINK_ROOT)))    // runtime?
  {
    return;
  }
  GetNameHelper(sb, l.parent->primary, abort_at_link_root);
  sb.append("/");
  sb.append(*(l.name));
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
    length += l->name->length() + 1u;
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
    throw std::runtime_error("Cannot initialize deleted element");
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

void tFrameworkElement::InitiallyShowInTools(int32_t priority)
{
  if (IsReady())
  {
    FINROC_LOG_PRINT(ERROR, "Element already initialized. Ignoring.");
    return;
  }
  tFrameworkElementTags::AddTag(*this, "initially show in tools:" + std::to_string(priority));
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
      tFrameworkElement& child = (*it)->GetChild();

      // We only initialize the 'unrelated' element if it has children that are ready
      if (&tRuntimeEnvironment::GetInstance().GetElement(tSpecialRuntimeElement::UNRELATED) == &child)
      {
        bool any_ready = false;
        for (auto unrelated_it = child.children->Begin(); unrelated_it != child.children->End(); ++unrelated_it)
        {
          any_ready |= (*unrelated_it)->GetChild().IsReady();
        }
        if (!any_ready)
        {
          continue;
        }
      }

      if ((*it)->IsPrimaryLink() && (!child.IsDeleted()))
      {
        child.InitImplementation();
      }
    }
  }

  // Call post-child-init callbacks and set READY flag
  if (init_this)
  {
    PostChildInit();
    {
      //tLock lock(*this); // we have structure lock
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
      if (l->parent->IsChildOf(re, ignore_delete_flag))
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

  // lock runtime (required to perform structural changes)
  tLock lock(GetStructureMutex());
  if (IsDeleted() || parent.IsDeleted())
  {
    throw std::runtime_error("Element and/or parent has been deleted.");
  }
  if (GetLinkCount() >= 127)
  {
    FINROC_LOG_PRINT(ERROR, "Maximum number of links exceeded.");
    throw std::runtime_error("Maximum number of links exceeded.");
  }

  tLink* l = new tLink(*this);
  l->name_buffer = link_name;
  l->name = &(l->name_buffer);
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
  // synchronizes on runtime - so no elements will be deleted while runtime is locked
  {
    tLock lock4(GetStructureMutex());

    if (IsDeleted())    // can happen if two threads delete concurrently - no problem, since this is - if at all - called when GarbageCollector-safety period has just started
    {
      return;
    }

    FINROC_LOG_PRINT(DEBUG_VERBOSE_1, "FrameworkElement ManagedDelete");


    NotifyAnnotationsDelete();

    FINROC_LOG_PRINT(DEBUG_VERBOSE_1, "Deleting");
    assert(!GetFlag(tFlag::DELETED));
    assert(((primary.GetParent() != NULL) || IsRuntime()));
    tFlags new_flags = flags | tFlag::DELETED;
    new_flags.Set(tFlag::READY, false);
    for (size_t i = 0; i < this->GetLinkCount(); i++)
    {
      this->GetLinkInternal(i)->name = &cDELETED_ELEMENT_STRING;
    }
    flags = new_flags;

    if (!IsRuntime())
    {
      GetRuntime().UnregisterElement(*this);
    }

    // perform custom cleanup (stopping/deleting threads can be done here)
    PrepareDelete();

    // remove children (thread-safe, because delete flag is set - and addChild etc. checks that)
    DeleteChildren();

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

  // add garbage collector task
  internal::tGarbageDeleter::DeleteDeferred(this);
}

bool tFrameworkElement::NameEquals(const tString& other) const
{
  if (IsReady())
  {
    return primary.name->compare(other) == 0;
  }
  else
  {
    tLock lock(GetStructureMutex());
    if (IsDeleted())
    {
      return false;
    }
    return primary.name->compare(other) == 0;
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

  output << GetName() << " (" << (IsReady() ? (GetFlag(tFlag::PUBLISHED) ? "published" : "ready") : IsDeleted() ? "deleted" : "constructing") << ")" << std::endl;

  // print child element info
  for (auto it = ChildrenBegin(); it != ChildrenEnd(); ++it)
  {
    it->PrintStructure(indent + 2, output);
  }
}

void tFrameworkElement::PublishUpdatedEdgeInfo(tRuntimeListener::tEvent change_type, tAbstractPort& target)
{
  GetRuntime().RuntimeChange(change_type, *this, &target, !GetFlag(tFlag::PUBLISHED));
}

void tFrameworkElement::PublishUpdatedInfo(tRuntimeListener::tEvent change_type)
{
  if (change_type == tRuntimeListener::tEvent::ADD || GetFlag(tFlag::PUBLISHED))
  {
    GetRuntime().RuntimeChange(change_type, *this, NULL);
  }
}

void tFrameworkElement::SetFlag(tFlag flag, bool value)
{
  assert(flag >= tFlag::READY || (IsPort() && flag == tFlag::HIJACKED_PORT));
  flags.Set(flag, value);
}

void tFrameworkElement::SetName(const tString& name)
{
  if (!IsCreator())
  {
    FINROC_LOG_PRINT(ERROR, "May only be called by creator thread. Ignoring.");
    return;
  }
  if (IsRuntime())
  {
    FINROC_LOG_PRINT(ERROR, "May not be called on Finroc Runtime Root Element. Ignoring.");
    return;
  }
  if (!IsConstructing())
  {
    FINROC_LOG_PRINT(ERROR, "May not be called after element has been initialized. Ignoring.");
    return;
  }

  internal::tStringBufferAnnotation* string_buffer_annotation = this->GetAnnotation<internal::tStringBufferAnnotation>();
  if (string_buffer_annotation)
  {
    FINROC_LOG_PRINT(ERROR, "Name may only be changed once. Ignoring.");
    return;
  }

  tLock lock(GetStructureMutex());  // synchronize, C++ strings may not be thread safe (e.g. for GetQualifiedName())
  if (primary.name != &(primary.name_buffer))
  {
    primary.name_buffer = name;
    primary.name = &(primary.name_buffer);
  }
  else
  {
    FINROC_LOG_PRINT(DEBUG, "Creating another string buffer for name change to '", name, "'.");
    string_buffer_annotation = &EmplaceAnnotation<internal::tStringBufferAnnotation>();
    string_buffer_annotation->string_buffer = name;
    primary.name = &(string_buffer_annotation->string_buffer);
  }

}

tFrameworkElement::tLink::tLink(tFrameworkElement& pointed_to) :
  points_to(pointed_to),
  name(&cUNNAMED_ELEMENT_STRING),
  name_buffer(),
  parent(NULL),
  next(NULL)
{}

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
      if (current_depth == 0)
      {
        return *this; // end
      }

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
  if (!fe.IsRuntime())
  {
    if (fe.GetParent())
    {
      StreamQualifiedName(output, *fe.GetParent(), false);
    }
    output << fe.GetName();
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
