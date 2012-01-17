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
#include "core/parameter/tConstructorParameters.h"

#include "core/tFrameworkElement.h"
#include "core/tRuntimeEnvironment.h"
#include "rrlib/finroc_core_utils/log/tLogUser.h"
#include "core/tRuntimeListener.h"
#include "rrlib/finroc_core_utils/tGarbageCollector.h"
#include "core/parameter/tStaticParameterList.h"
#include "core/parameter/tStaticParameterBase.h"
#include "core/parameter/tConfigFile.h"
#include "core/parameter/tConfigNode.h"
#include "rrlib/serialization/tOutputStream.h"
#include "core/port/tAbstractPort.h"

namespace finroc
{
namespace core
{
tFrameworkElement::tFrameworkElement(tFrameworkElement* parent_, const util::tString& description_, uint flags_, int lock_order_) :
    primary(this),
    flag_mutex(),
    creater_thread_uid(util::sThreadUtil::GetCurrentThreadId()),
    const_flags(flags_ & tCoreFlags::cCONSTANT_FLAGS),
    flags(flags_ & tCoreFlags::cNON_CONSTANT_FLAGS),
    children(GetFlag(tCoreFlags::cALLOWS_CHILDREN) ? 4 : 0, GetFlag(tCoreFlags::cALLOWS_CHILDREN) ? 4 : 0),
    obj_mutex(GetLockOrder(flags_, parent_, lock_order_), GetFlag(tCoreFlags::cIS_RUNTIME) ? util::tInteger::cMIN_VALUE : tRuntimeEnvironment::GetInstance()->RegisterElement(this))
{
  assert(((flags_ & tCoreFlags::cSTATUS_FLAGS) == 0));

  primary.description = description_;

  if (!GetFlag(tCoreFlags::cIS_RUNTIME))
  {
    tFrameworkElement* parent = (parent_ != NULL) ? parent_ : tRuntimeEnvironment::GetInstance()->unrelated;
    if (lock_order_ < 0)
    {
      lock_order_ = parent->GetLockOrder() + 1;
    }
    parent->AddChild(&(primary));
  }

  //      if (parent != null) {
  //          assert !parent.isInitialized() : "shouldn't add half-constructed objects to initialized/operating parent - could cause concurrency problems in init process";
  //          parent.addChild(primary);
  //      }

  FINROC_LOG_PRINT_TO(framework_elements, rrlib::logging::eLL_DEBUG_VERBOSE_1, "Constructing FrameworkElement");
}

void tFrameworkElement::AddChild(tLink* child)
{
  if (child->parent == this)
  {
    return;
  }

  // lock runtime (required to perform structural changes)
  {
    util::tLock lock2(GetRegistryLock());

    // perform checks
    assert(((child->GetChild()->IsConstructing())) && "tree structure is fixed for initialized children - is child initialized twice (?)");
    assert(((child->GetChild()->IsCreator())) && "may only be called by child creator thread");
    if (IsDeleted() || (child->parent != NULL && child->parent->IsDeleted()) || child->GetChild()->IsDeleted())
    {
      throw util::tRuntimeException("Child has been deleted or has deleted parent. Thread exit is likely the intended behaviour.", CODE_LOCATION_MACRO);
    }
    if (child->parent != NULL)
    {
      assert(((child->GetChild()->LockAfter(child->parent))) && "lockOrder level of child needs to be higher than of former parent");
    }
    assert(((child->GetChild()->LockAfter(this))) && "lockOrder level of child needs to be higher than of parent");
    // avoid cycles
    assert(child->GetChild() != this);
    assert((!this->IsChildOf(child->GetChild())));

    // detach from former parent
    if (child->parent != NULL)
    {
      //assert(!child.parent.isInitialized()) : "This is truly strange - should not happen";
      child->parent->children.Remove(child);
    }

    // Check if child with same name already exists and possibly rename (?)
    if (GetFlag(tCoreFlags::cAUTO_RENAME) && (!GetFlag(tCoreFlags::cIS_PORT)))
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
        if (child != NULL && (!child->GetChild()->IsDeleted()))
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
  ;
  assert(((GetFlag(tCoreFlags::cDELETED) || GetFlag(tCoreFlags::cIS_RUNTIME))) && "Frameworkelement was not deleted with managedDelete()");
  FINROC_LOG_PRINT_TO(framework_elements, rrlib::logging::eLL_DEBUG_VERBOSE_1, "FrameworkElement destructor");
  if (!GetFlag(tCoreFlags::cIS_RUNTIME))
  {
    // synchronizes on runtime - so no elements will be deleted while runtime is locked
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

bool tFrameworkElement::DescriptionEquals(const util::tString& other) const
{
  if (IsReady())
  {
    return primary.description.Equals(other);
  }
  else
  {
    {
      util::tLock lock3(GetRegistryLock());
      if (IsDeleted())
      {
        return false;
      }
      return primary.description.Equals(other);
    }
  }
}

void tFrameworkElement::DoStaticParameterEvaluation()
{
  util::tLock lock2(GetRegistryLock());

  // all parameters attached to any of the module's parameters
  util::tSimpleList<tStaticParameterBase*> attached_parameters;
  util::tSimpleList<tStaticParameterBase*> attached_parameters_tmp;

  tStaticParameterList* spl = GetAnnotation<tStaticParameterList>();
  if (spl != NULL)
  {

    // Reevaluate parameters and check whether they have changed
    bool changed = false;
    for (size_t i = 0; i < spl->Size(); i++)
    {
      spl->Get(i)->LoadValue();
      changed |= spl->Get(i)->HasChanged();
      spl->Get(i)->GetAllAttachedParameters(attached_parameters_tmp);
      attached_parameters.AddAll(attached_parameters_tmp);
    }

    if (changed)
    {
      EvaluateStaticParameters();

      // Reset change flags for all parameters
      for (size_t i = 0; i < spl->Size(); i++)
      {
        spl->Get(i)->ResetChanged();
      }

      // initialize any new child elements
      if (IsReady())
      {
        Init();
      }
    }
  }

  // evaluate children's static parameters
  util::tArrayWrapper<tLink*>* iterable = children.GetIterable();
  for (int i = 0, n = iterable->Size(); i < n; i++)
  {
    tLink* child = iterable->Get(i);
    if (child != NULL && child->IsPrimaryLink() && (!child->GetChild()->IsDeleted()))
    {
      child->GetChild()->DoStaticParameterEvaluation();
    }
  }

  // evaluate any attached parameters that have changed, too
  for (size_t i = 0; i < attached_parameters.Size(); i++)
  {
    if (attached_parameters.Get(i)->HasChanged())
    {
      attached_parameters.Get(i)->GetParentList()->GetAnnotated()->DoStaticParameterEvaluation();
    }
  }
}

const char* tFrameworkElement::GetCDescription() const
{
  return primary.description.Length() == 0 ? "(anonymous)" : primary.description.GetCString();
}

tFrameworkElement* tFrameworkElement::GetChild(const util::tString& name) const
{
  util::tArrayWrapper<tLink*>* iterable = children.GetIterable();
  for (int i = 0, n = iterable->Size(); i < n; i++)
  {
    tLink* child = iterable->Get(i);
    if (child->GetChild()->IsReady())
    {
      if (child->GetDescription().Equals(name))
      {
        return child->GetChild();
      }
    }
    else
    {
      {
        util::tLock lock4(GetRegistryLock());
        if (IsDeleted())
        {
          return NULL;
        }
        if (child->GetChild()->IsDeleted())
        {
          continue;
        }
        if (child->GetDescription().Equals(name))
        {
          return child->GetChild();
        }
      }
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
  // lock runtime (might not be absolutely necessary... ensures, however, that result is valid)
  {
    util::tLock lock2(GetRegistryLock());

    if (IsDeleted())
    {
      return NULL;
    }

    if (name.CharAt(name_index) == '/')
    {
      return root->GetChildElement(name, name_index + 1, only_globally_unique_children, root);
    }

    only_globally_unique_children &= (!GetFlag(tCoreFlags::cGLOBALLY_UNIQUE_LINK));
    util::tArrayWrapper<tLink*>* iterable = children.GetIterable();
    for (int i = 0, n = iterable->Size(); i < n; i++)
    {
      tLink* child = iterable->Get(i);
      if (child != NULL && name.RegionMatches(name_index, child->description, 0, child->description.Length()) && (!child->GetChild()->IsDeleted()))
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
}

const util::tString tFrameworkElement::GetDescription() const
{
  if (IsReady() || GetFlag(tCoreFlags::cIS_RUNTIME))
  {
    return primary.description.Length() == 0 ? "(anonymous)" : primary.description;
  }
  else
  {
    {
      util::tLock lock3(GetRegistryLock());  // synchronize, while description can be changed (C++ strings may not be thread safe...)
      if (IsDeleted())
      {
        return "(deleted element)";
      }
      return primary.description.Length() == 0 ? "(anonymous)" : primary.description;
    }
  }
}

const tFrameworkElement::tLink* tFrameworkElement::GetLink(size_t link_index) const
{
  {
    util::tLock lock2(GetRegistryLock());  // absolutely safe this way
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
}

size_t tFrameworkElement::GetLinkCount() const
{
  if (IsReady())
  {
    return GetLinkCountHelper();
  }
  else
  {
    {
      util::tLock lock3(GetRegistryLock());  // absolutely safe this way
      return GetLinkCountHelper();
    }
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

tConfigFile* tFrameworkElement::GetConfigFile() const
{
  return tConfigFile::Find(this);
}


tFrameworkElement* tFrameworkElement::GetParent(int link_index) const
{
  {
    util::tLock lock2(GetRegistryLock());  // absolutely safe this way
    if (IsDeleted())
    {
      return NULL;
    }
    return GetLink(link_index)->parent;
  }
}

tFrameworkElement* tFrameworkElement::GetParentWithFlags(uint flags_) const
{
  if (primary.parent == NULL)
  {
    return NULL;
  }
  {
    util::tLock lock2(GetRegistryLock());  // not really necessary after element has been initialized
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
}

bool tFrameworkElement::GetQualifiedName(util::tStringBuilder& sb, const tLink* start, bool force_full_link) const
{
  if (IsReady())
  {
    return GetQualifiedNameImpl(sb, start, force_full_link);
  }
  else
  {
    {
      util::tLock lock3(GetRegistryLock());  // synchronize while element is under construction
      return GetQualifiedNameImpl(sb, start, force_full_link);
    }
  }
}

bool tFrameworkElement::GetQualifiedNameImpl(util::tStringBuilder& sb, const tLink* start, bool force_full_link) const
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
  //      if (len2 < sb.length()) {
  //          sb.delete(len2, sb.length());
  //      }
  return abort_at_link_root;
}

util::tMutexLockOrder& tFrameworkElement::GetRegistryLock() const
{
  return tRuntimeEnvironment::GetInstance()->GetRegistryHelper()->obj_mutex;
}

tRuntimeEnvironment* tFrameworkElement::GetRuntime() const
{
  //return getParent(RuntimeEnvironment.class);
  return tRuntimeEnvironment::GetInstance();
}

util::tMutexLockOrder& tFrameworkElement::GetThreadLocalCacheInfosLock() const
{
  return tRuntimeEnvironment::GetInstance()->GetRegistryHelper()->infos_lock->obj_mutex;
}

void tFrameworkElement::Init()
{
  {
    util::tLock lock2(GetRuntime()->GetRegistryLock());
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
}

void tFrameworkElement::InitAll()
{
  tRuntimeEnvironment::GetInstance()->Init();
}

void tFrameworkElement::InitImpl()
{
  //System.out.println("init: " + toString() + " " + parent.toString());
  assert(((!IsDeleted())) && "Deleted element cannot be reinitialized");

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
      if (child != NULL && child->IsPrimaryLink() && (!child->GetChild()->IsDeleted()))
      {
        child->GetChild()->InitImpl();
      }
    }
  }

  if (init_this)
  {
    PostChildInit();
    //System.out.println("Setting Ready " + toString() + " Thread: " + ThreadUtil.getCurrentThreadId());
    {
      util::tLock lock3(flag_mutex);
      flags |= tCoreFlags::cREADY;
    }

    DoStaticParameterEvaluation();

    NotifyAnnotationsInitialized();
  }

}

bool tFrameworkElement::IsChildOf(tFrameworkElement* re, bool ignore_delete_flag) const
{
  {
    util::tLock lock2(GetRegistryLock());  // absolutely safe this way
    if ((!ignore_delete_flag) && IsDeleted())
    {
      return false;
    }
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
}

void tFrameworkElement::Link(tFrameworkElement* parent, const util::tString& link_name)
{
  assert(((IsCreator())) && "may only be called by creator thread");
  assert((LockAfter(parent)));

  // lock runtime (required to perform structural changes)
  {
    util::tLock lock2(GetRegistryLock());
    if (IsDeleted() || parent->IsDeleted())
    {
      throw util::tRuntimeException("Element and/or parent has been deleted. Thread exit is likely the intended behaviour.", CODE_LOCATION_MACRO);
    }

    tLink* l = new tLink(this);
    l->description = link_name;
    l->parent = NULL;  // will be set in addChild
    tLink* lprev = GetLinkInternal(GetLinkCount() - 1u);
    assert(lprev->next == NULL);
    lprev->next = l;
    parent->AddChild(l);
    //RuntimeEnvironment.getInstance().link(this, linkName);
  }
}

void tFrameworkElement::ManagedDelete(tLink* dont_detach)
{
  {
    util::tLock lock2(RuntimeLockHelper());
    {
      util::tLock lock3(this);

      if (IsDeleted())    // can happen if two threads delete concurrently - no problem, since this is - if at all - called when GarbageCollector-safety period has just started
      {
        return;
      }

      FINROC_LOG_PRINT_TO(framework_elements, rrlib::logging::eLL_DEBUG_VERBOSE_1, "FrameworkElement managedDelete");

      // synchronizes on runtime - so no elements will be deleted while runtime is locked
      {
        util::tLock lock4(GetRegistryLock());

        NotifyAnnotationsDelete();

        FINROC_LOG_PRINT_TO(framework_elements, rrlib::logging::eLL_DEBUG_VERBOSE_1, "Deleting");
        //System.out.println("Deleting " + toString() + " (" + hashCode() + ")");
        assert(!GetFlag(tCoreFlags::cDELETED));
        assert(((primary.GetParent() != NULL) | GetFlag(tCoreFlags::cIS_RUNTIME)));

        {
          util::tLock lock5(flag_mutex);
          flags = (flags | tCoreFlags::cDELETED) & ~tCoreFlags::cREADY;
        }

        if (!GetFlag(tCoreFlags::cIS_RUNTIME))
        {
          tRuntimeEnvironment::GetInstance()->MarkElementDeleted(this);
        }
      }

      // perform custom cleanup (stopping/deleting threads can be done here)
      PrepareDelete();

      // remove children (thread-safe, because delete flag is set - and addChild etc. checks that)
      DeleteChildren();

      // synchronized on runtime for removement from hierarchy
      {
        util::tLock lock4(GetRegistryLock());

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

        // TODO
        //              // JavaOnlyBlock
        //              if (treeNode != null) {
        //                  parent.treeNode.remove(treeNode);
        //              }

        primary.parent = NULL;

      }
    }
  }

  // add garbage collector task
  util::tGarbageCollector::DeleteDeferred(this);
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
  {
    util::tLock lock2(GetRegistryLock());

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

    output << GetCDescription() << " (" << (IsReady() ? (GetFlag(tCoreFlags::cPUBLISHED) ? "published" : "ready") : IsDeleted() ? "deleted" : "constructing") << ")" << std::endl;

    // print child element info
    util::tArrayWrapper<tLink*>* iterable = children.GetIterable();
    for (int i = 0, n = iterable->Size(); i < n; i++)
    {
      tLink* child = iterable->Get(i);
      if (child != NULL)
      {
        child->GetChild()->PrintStructure(indent + 2, output);
      }
    }
  }
}

void tFrameworkElement::PublishUpdatedEdgeInfo(int8 change_type, tAbstractPort* target)
{
  if (GetFlag(tCoreFlags::cPUBLISHED))
  {
    tRuntimeEnvironment::GetInstance()->RuntimeChange(change_type, this, target);
  }
}

void tFrameworkElement::PublishUpdatedInfo(int8 change_type)
{
  if (change_type == tRuntimeListener::cADD || GetFlag(tCoreFlags::cPUBLISHED))
  {
    tRuntimeEnvironment::GetInstance()->RuntimeChange(change_type, this, NULL);
  }
}

void tFrameworkElement::RemoveFlag(int flag)
{
  {
    util::tLock lock2(flag_mutex);
    assert((flag & tCoreFlags::cNON_CONSTANT_FLAGS) != 0);
    flags &= ~flag;
  }
}

util::tMutexLockOrder& tFrameworkElement::RuntimeLockHelper() const
{
  if (obj_mutex.ValidAfter(GetRuntime()->GetRegistryHelper()->obj_mutex))
  {
    return GetRegistryLock();
  }

  return this->obj_mutex;
}

void tFrameworkElement::SetConfigNode(const util::tString& node)
{
  util::tLock lock2(GetRegistryLock());
  tConfigNode* cn = GetAnnotation<tConfigNode>();
  if (cn != NULL)
  {
    if (cn->node.Equals(node))
    {
      return;
    }
    cn->node = node;
  }
  else
  {
    cn = new tConfigNode(node);
    AddAnnotation(cn);
  }

  // reevaluate static parameters
  DoStaticParameterEvaluation();

  // reload parameters
  if (IsReady())
  {
    tConfigFile* cf = tConfigFile::Find(this);
    if (cf != NULL)
    {
      cf->LoadParameterValues(this);
    }
  }
}

void tFrameworkElement::SetDescription(const util::tString& description)
{
  assert((!GetFlag(tCoreFlags::cIS_RUNTIME)));
  {
    util::tLock lock2(GetRegistryLock());  // synchronize, C++ strings may not be thread safe...
    assert((IsConstructing()));
    assert((IsCreator()));
    primary.description = description;
  }
}

void tFrameworkElement::SetFinstructed(tCreateFrameworkElementAction* create_action, tConstructorParameters* params)
{
  assert((!GetFlag(tCoreFlags::cFINSTRUCTED)));
  tStaticParameterList* list = tStaticParameterList::GetOrCreate(this);
  list->SetCreateAction(create_action);
  SetFlag(tCoreFlags::cFINSTRUCTED);
  if (params != NULL)
  {
    AddAnnotation(params);
  }
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
  {
    util::tLock lock2(flag_mutex);
    assert((flag & tCoreFlags::cCONSTANT_FLAGS) == 0);
    flags |= flag;
  }
}

void tFrameworkElement::WriteDescription(rrlib::serialization::tOutputStream& os, int i) const
{
  if (IsReady())
  {
    os.WriteString(GetLink(i)->description);
  }
  else
  {
    {
      util::tLock lock3(GetRegistryLock());  // synchronize, while description can be changed (C++ strings may not be thread safe...)
      os.WriteString(IsDeleted() ? "deleted element" : GetLink(i)->description);
    }
  }
}

tFrameworkElement::tChildIterator::tChildIterator(const tFrameworkElement* parent) :
    next_elem(NULL),
    last(NULL),
    flags(0),
    result(0),
    cur_parent(NULL)
{
  Reset(parent);
}

tFrameworkElement::tChildIterator::tChildIterator(const tFrameworkElement* parent, uint flags_) :
    next_elem(NULL),
    last(NULL),
    flags(0),
    result(0),
    cur_parent(NULL)
{
  Reset(parent, flags_);
}

tFrameworkElement::tChildIterator::tChildIterator(const tFrameworkElement* parent, uint flags_, uint result_) :
    next_elem(NULL),
    last(NULL),
    flags(0),
    result(0),
    cur_parent(NULL)
{
  Reset(parent, flags_, result_);
}

tFrameworkElement::tChildIterator::tChildIterator(const tFrameworkElement* parent, uint flags_, uint result_, bool include_non_ready) :
    next_elem(NULL),
    last(NULL),
    flags(0),
    result(0),
    cur_parent(NULL)
{
  Reset(parent, flags_, result_, include_non_ready);
}

tFrameworkElement* tFrameworkElement::tChildIterator::Next()
{
  while (next_elem <= last)
  {
    tFrameworkElement::tLink* nex = *next_elem;
    if (nex != NULL && (nex->GetChild()->GetAllFlags() & flags) == result)
    {
      next_elem++;
      return nex->GetChild();
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

void tFrameworkElement::tChildIterator::Reset(const tFrameworkElement* parent, uint flags_, uint result_, bool include_non_ready)
{
  assert((parent != NULL));
  this->flags = flags_ | tCoreFlags::cDELETED;
  this->result = result_;
  if (!include_non_ready)
  {
    flags_ |= tCoreFlags::cREADY;
    result_ |= tCoreFlags::cREADY;
  }
  cur_parent = parent;

  const util::tArrayWrapper<tFrameworkElement::tLink*>* array = parent->GetChildren();
  next_elem = array->GetPointer();
  last = (next_elem + array->Size()) - 1;

}

} // namespace finroc
} // namespace core

