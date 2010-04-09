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
#include "core/tRuntimeEnvironment.h"
#include "core/port/cc/tNumberPort.h"
#include "core/port/rpc/tAbstractCall.h"

#include "core/tFrameworkElement.h"
#include "core/tRuntimeSettings.h"
#include "core/tRuntimeListener.h"
#include "finroc_core_utils/tGarbageCollector.h"

namespace finroc
{
namespace core
{
tFrameworkElement::tFrameworkElement(const util::tString& description_, tFrameworkElement* parent_, int flags_) :
    primary(this),
    creater_thread_uid(util::sThreadUtil::GetCurrentThreadId()),
    const_flags(flags_ & tCoreFlags::cCONSTANT_FLAGS),
    flags(flags_ & tCoreFlags::cNON_CONSTANT_FLAGS),
    children(GetFlag(tCoreFlags::cALLOWS_CHILDREN) ? 4 : 0, GetFlag(tCoreFlags::cALLOWS_CHILDREN) ? 4 : 0),
    handle(GetFlag(tCoreFlags::cIS_RUNTIME) ? util::tInteger::cMIN_VALUE : tRuntimeEnvironment::GetInstance()->RegisterElement(this)),
    obj_synch()
{
  primary.description = description_;
  if (!GetFlag(tCoreFlags::cIS_RUNTIME))
  {
    tFrameworkElement* parent = (parent_ != NULL) ? parent_ : tRuntimeEnvironment::GetInstance()->unrelated;
    parent->AddChild(&(primary));
  }
  //    if (parent != null) {
  //      assert !parent.isInitialized() : "shouldn't add half-constructed objects to initialized/operating parent - could cause concurrency problems in init process";
  //      parent.addChild(primary);
  //    }

  if (tRuntimeSettings::cDISPLAY_CONSTRUCTION_DESTRUCTION->Get())
  {
    util::tSystem::out.Println(util::tStringBuilder("Constructing FrameworkElement: ") + GetDescription());
  }
}

void tFrameworkElement::AddChild(tLink* child)
{
  assert(((child->GetChild()->IsConstructing())) && "tree structure is fixed for initialized children - is child initialized twice (?)");
  assert(((child->GetChild()->IsCreator())) && "may only be called by child creator thread");

  if (child->parent == this)
  {
    return;
  }

  // detach from former parent
  if (child->parent != NULL)
  {
    //assert(!child.parent.isInitialized()) : "This is truly strange - should not happen";
    {
      util::tLock lock3(child->parent->obj_synch);
      child->parent->children.Remove(child);
    }
  }

  {
    util::tLock lock2(children.obj_synch);
    {
      util::tLock lock3(child->GetChild()->children.obj_synch);

      // avoid cycles
      assert(child->GetChild() != this);
      assert((!this->IsChildOf(child->GetChild())));

      // Check if child with same name already exists and possibly rename (?)
      if (GetFlag(tCoreFlags::cAUTO_RENAME))
      {
        util::tString child_desc = child->GetDescription();
        int postfix_index = 1;
        util::tArrayWrapper<tLink*>* ch = children.GetIterable();
        for (int i = 0, n = ch->Size(); i < n; i++)
        {
          tLink* re = ch->Get(i);
          if (re != NULL && re->GetDescription().Equals(child_desc))
          {
            // name clash
            /*if (postfixIndex == 1) {
            System.out.println("Warning: name conflict in " + getUid() + " - " + child.getDescription());
            }*/
            re->GetChild()->SetDescription(child_desc + "[" + postfix_index + "]");
            postfix_index++;
            continue;
          }
        }
      }

      child->parent = this;
      children.Add(child, false);
      // child.init(); - do this separately
      // child.init(); - do this separately
    }
    // child.init(); - do this separately
  }
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
      util::tSystem::out.Println(util::tStringBuilder("Publishing ") + GetQualifiedName());
      PublishUpdatedInfo(tRuntimeListener::cADD);
    }

    // publish any children?
    if (GetFlag(tCoreFlags::cPUBLISHED))
    {
      util::tArrayWrapper<tLink*>* iterable = children.GetIterable();
      for (int i = 0, n = iterable->Size(); i < n; i++)
      {
        tLink* child = iterable->Get(i);
        if (child != NULL)
        {
          child->GetChild()->CheckPublish();
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
  util::tSystem::out.Println(util::tStringBuilder("FrameworkElement destructor: ") + GetDescription());
  if (!GetFlag(tCoreFlags::cIS_RUNTIME))
  {
    tRuntimeEnvironment::GetInstance()->UnregisterElement(this);
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

void tFrameworkElement::DeleteChildren()
{
  //    deleteRange(0, childEntryCount());
  //  }
  //
  //  /**
  //   * Deletes a range of children of this framework element
  //   *
  //   * \param start Start index
  //   * \param end End index (exclusive-9
  //   */
  //  public void deleteRange(@SizeT int start, @SizeT int end) {
  {
    util::tLock lock2(children.obj_synch);
    util::tArrayWrapper<tLink*>* iterable = children.GetIterable();
    for (int i = 0, n = iterable->Size(); i < n; i++)
    {
      tLink* child = iterable->Get(i);
      if (child != NULL)
      {
        child->GetChild()->ManagedDelete(child);
      }
    }

    children.Clear();
  }
}

tFrameworkElement* tFrameworkElement::GetChild(const util::tString& name) const
{
  util::tArrayWrapper<tLink*>* iterable = children.GetIterable();
  for (int i = 0, n = iterable->Size(); i < n; i++)
  {
    tLink* child = iterable->Get(i);
    if (child->GetDescription().Equals(name))
    {
      return child->GetChild();
    }
  }
  return NULL;
}

tFrameworkElement* tFrameworkElement::GetChildElement(const util::tString& name, bool only_globally_unique_children)
{
  return GetChildElement(name, 0, only_globally_unique_children, tRuntimeEnvironment::GetInstance());
}

tFrameworkElement* tFrameworkElement::GetChildElement(const util::tString& name, int name_index, bool only_globally_unique_children, tFrameworkElement* root)
{
  if (name.CharAt(name_index) == '/')
  {
    return root->GetChildElement(name, name_index + 1, only_globally_unique_children, root);
  }

  only_globally_unique_children &= (!GetFlag(tCoreFlags::cGLOBALLY_UNIQUE_LINK));
  util::tArrayWrapper<tLink*>* iterable = children.GetIterable();
  for (int i = 0, n = iterable->Size(); i < n; i++)
  {
    tLink* child = iterable->Get(i);
    if (child != NULL && name.RegionMatches(name_index, child->description, 0, child->description.Length()))
    {
      if (name.Length() == name_index + child->description.Length())
      {
        if (!only_globally_unique_children || child->GetChild()->GetFlag(tCoreFlags::cGLOBALLY_UNIQUE_LINK))
        {
          return child->GetChild();
        }
      }
      if (name.CharAt(name_index + child->description.Length()) == '/')
      {
        tFrameworkElement* result = child->GetChild()->GetChildElement(name, name_index + child->description.Length() + 1, only_globally_unique_children, root);
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

const tFrameworkElement::tLink* tFrameworkElement::GetLink(size_t link_index) const
{
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

void tFrameworkElement::GetNameHelper(util::tStringBuilder& sb, const tLink* l, bool abort_at_link_root)
{
  if (l->parent == NULL || (abort_at_link_root && l->GetChild()->GetFlag(tCoreFlags::cALTERNATE_LINK_ROOT)))    // runtime?
  {
    return;
  }
  GetNameHelper(sb, &(l->parent->primary), abort_at_link_root);
  sb.Append('/');
  sb.Append(l->description);
}

bool tFrameworkElement::GetQualifiedName(util::tStringBuilder& sb, const tLink* start, bool force_full_link) const
{
  size_t length = 0u;
  bool abort_at_link_root = false;
  for (const tLink* l = start; l->parent != NULL && !(abort_at_link_root && l->GetChild()->GetFlag(tCoreFlags::cALTERNATE_LINK_ROOT)); l = &(l->parent->primary))
  {
    abort_at_link_root |= (!force_full_link) && l->GetChild()->GetFlag(tCoreFlags::cGLOBALLY_UNIQUE_LINK);
    if (abort_at_link_root && l->GetChild()->GetFlag(tCoreFlags::cALTERNATE_LINK_ROOT))    // if unique_link element is at the same time a link root
    {
      break;
    }
    length += l->description.Length() + 1u;
  }
  sb.Delete(0, sb.Length());
  sb.EnsureCapacity(length);

  GetNameHelper(sb, start, abort_at_link_root);
  assert((sb.Length() == length));

  // remove any characters if buffer is too long
  //    if (len2 < sb.length()) {
  //      sb.delete(len2, sb.length());
  //    }
  return abort_at_link_root;
}

tRuntimeEnvironment* tFrameworkElement::GetRuntime() const
{
  //return getParent(RuntimeEnvironment.class);
  return tRuntimeEnvironment::GetInstance();
}

void tFrameworkElement::Init()
{
  util::tLock lock1(obj_synch);
  {
    util::tLock lock2(tRuntimeEnvironment::GetInstance()->obj_synch);
    //SimpleList<FrameworkElement> publishThese = new SimpleList<FrameworkElement>();
    //assert(getFlag(CoreFlags.IS_RUNTIME) || getParent().isReady());

    InitImpl();

    CheckPublish();

    /*for (@SizeT int i = 0, n = publishThese.size(); i < n; i++) {
      publishThese.get(i).publishUpdatedInfo(RuntimeListener.ADD);
    }*/
  }
}

void tFrameworkElement::InitAll()
{
  tRuntimeEnvironment::GetInstance()->Init();
}

void tFrameworkElement::InitImpl()
{
  util::tLock lock1(obj_synch);
  //System.out.println("init: " + toString() + " " + parent.toString());
  assert(((!IsDeleted())) && "Deleted element cannot be reinitialized");

  {
    util::tLock lock2(children.obj_synch);

    bool init_this = !IsReady() && IsCreator();

    if (init_this)
    {
      PreChildInit();
      tRuntimeEnvironment::GetInstance()->PreElementInit(this);
    }

    if (init_this || IsReady())
    {
      util::tArrayWrapper<tLink*>* iterable = children.GetIterable();
      for (int i = 0, n = iterable->Size(); i < n; i++)
      {
        tLink* child = iterable->Get(i);
        if (child != NULL && child->IsPrimaryLink())
        {
          child->GetChild()->InitImpl();
        }
      }
    }

    if (init_this)
    {
      PostChildInit();
      //System.out.println("Setting Ready " + toString() + " Thread: " + ThreadUtil.getCurrentThreadId());
      flags |= tCoreFlags::cREADY;
    }
  }
}

bool tFrameworkElement::IsChildOf(tFrameworkElement* re) const
{
  for (const tLink* l = &(primary); l != NULL; l = l->next)
  {
    if (l->parent == re)
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

void tFrameworkElement::Link(tFrameworkElement* parent, const util::tString& link_name)
{
  util::tLock lock1(obj_synch);
  assert(((IsCreator())) && "may only be called by creator thread");
  tLink* l = new tLink(this);
  l->description = link_name;
  l->parent = NULL;  // will be set in addChild
  tLink* lprev = GetLinkInternal(GetLinkCount() - 1u);
  assert(lprev->next == NULL);
  lprev->next = l;
  parent->AddChild(l);
  //RuntimeEnvironment.getInstance().link(this, linkName);
}

void tFrameworkElement::ManagedDelete(tLink* dont_detach)
{
  util::tLock lock1(obj_synch);

  if (tRuntimeSettings::cDISPLAY_CONSTRUCTION_DESTRUCTION->Get())
  {
    util::tSystem::out.Println(util::tStringBuilder("FrameworkElement managedDelete: ") + GetQualifiedName());
  }

  {
    util::tLock lock2(children.obj_synch);
    assert(!GetFlag(tCoreFlags::cDELETED));
    assert(((primary.GetParent() != NULL) | GetFlag(tCoreFlags::cIS_RUNTIME)));

    flags = (flags | tCoreFlags::cDELETED) & ~tCoreFlags::cREADY;

    // remove children
    DeleteChildren();

    // remove element itself
    PrepareDelete();
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

    // TODO
    //        // JavaOnlyBlock
    //        if (treeNode != null) {
    //          parent.treeNode.remove(treeNode);
    //        }

    primary.parent = NULL;
  }

  // add garbage collector task
  lock1.unlock();
  util::tGarbageCollector::DeleteDeferred(this);
}

void tFrameworkElement::PrintStructure(int indent)
{
  // print element info
  for (int i = 0; i < indent; i++)
  {
    util::tSystem::out.Print(" ");
  }
  util::tSystem::out.Print(GetDescription());
  util::tSystem::out.Print(" (");
  util::tSystem::out.Print(IsReady() ? (GetFlag(tCoreFlags::cPUBLISHED) ? "published" : "ready") : IsDeleted() ? "deleted" : "constructing");
  util::tSystem::out.Println(")");

  // print child element info
  util::tArrayWrapper<tLink*>* iterable = children.GetIterable();
  for (int i = 0, n = iterable->Size(); i < n; i++)
  {
    tLink* child = iterable->Get(i);
    if (child != NULL)
    {
      child->GetChild()->PrintStructure(indent + 2);
    }
  }
}

void tFrameworkElement::PublishUpdatedInfo(int8 change_type)
{
  util::tLock lock1(obj_synch);
  tRuntimeEnvironment::GetInstance()->RuntimeChange(change_type, this);
}

void tFrameworkElement::SetDescription(const util::tString& description)
{
  util::tLock lock1(obj_synch);
  assert((IsConstructing()));
  assert((IsCreator()));
  primary.description = description;
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

} // namespace finroc
} // namespace core

