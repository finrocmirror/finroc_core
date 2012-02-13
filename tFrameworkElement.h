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

#ifndef core__tFrameworkElement_h__
#define core__tFrameworkElement_h__

#include "rrlib/finroc_core_utils/definitions.h"
#include "rrlib/finroc_core_utils/container/tSafeConcurrentlyIterableList.h"
#include "rrlib/finroc_core_utils/thread/sThreadUtil.h"

#include "core/tCoreFlags.h"
#include "core/tAnnotatable.h"

namespace rrlib
{
namespace serialization
{
class tOutputStream;
} // namespace rrlib
} // namespace serialization

namespace finroc
{
namespace core
{
class tRuntimeEnvironment;
class tAbstractPort;

/*!
 * \author Max Reichardt
 *
 * Base functionality of Ports, PortSets, Modules, Groups and the Runtime.
 *
 * When dealing with unknown framework elements - check isReady() to make sure
 * they are fully initialized and not already deleted.
 * Init needs to be called before framework elements can be used, as well as being visible
 * to remote runtime environments/clients.
 *
 * Framework elements are arranged in a tree.
 * They may be linked/referenced from other parts of the tree.
 *
 * Everything is thread-safe as long as methods are used.
 *
 * To prevent deleting of framework element while using it over a longer period of time,
 * lock it - or the complete runtime environment.
 */
class tFrameworkElement : public tAnnotatable
{
public:

  /*!
   * \author Max Reichardt
   *
   * Framework elements are inserted as children of other framework element using this connector class.
   * Ratio: this allows links in the tree
   *
   * Framework elements "own" links - they are deleted with framework element
   */
  class tLink : public util::tUncopyableObject
  {
    friend class tFrameworkElement;
  private:

    // Outer class FrameworkElement
    tFrameworkElement* const outer_class_ptr;

    /*! Name of Framework Element - in link context */
    util::tString name;

    /*! Parent - Element in which this link was inserted */
    tFrameworkElement* parent;

    /*! Next link for this framework element (=> singly-linked list) */
    tFrameworkElement::tLink* next;

  public:

    tLink(tFrameworkElement* const outer_class_ptr_) :
      outer_class_ptr(outer_class_ptr_),
      name(),
      parent(NULL),
      next(NULL)
    {}

    virtual ~tLink()
    {
    }

    /*!
     * \return Element that this link points to
     */
    inline tFrameworkElement* GetChild() const
    {
      return outer_class_ptr;
    }

    /*!
     * \return Name of Framework Element - in link context
     */
    inline util::tString GetName() const
    {
      return name;
    }

    /*!
     * \return Parent - Element in which this link was inserted
     */
    inline tFrameworkElement* GetParent() const
    {
      return parent;
    }

    /*!
     * \return Is this a primary link?
     */
    inline bool IsPrimaryLink() const
    {
      return this == &(GetChild()->primary);
    }

  };

public:
  class tChildIterator; // inner class forward declaration

private:

  friend class util::tGarbageCollector;
  friend class tChildIterator;
  friend class tRuntimeEnvironment;
  friend class tFinstructableGroup;
  friend class tLock;

  /*! Primary link to framework element - the place at which it actually is in FrameworkElement tree - contains name etc. */
  tLink primary;

  /*!
   * Extra Mutex for changing flags
   */
  mutable util::tMutex flag_mutex;

protected:

  /*! Uid of thread that created this framework element */
  const int64 creater_thread_uid;

  // Splitting flags up might allow compiler optimizations??

  /*! Constant Flags - see CoreFlags */
  const uint const_flags;

  /*! Variable Flags - see CoreFlags */
  uint flags;

  /*! children - may contain null entries (for efficient thread-safe unsynchronized iteration) */
  util::tSafeConcurrentlyIterableList<tLink*> children;

public:

  /*!
   * Defines lock order in which framework elements can be locked.
   * Generally the framework element tree is locked from root to leaves.
   * So children's lock level needs to be larger than their parent's.
   *
   * The secondary component is the element's unique handle in local runtime environment.
   * ("normal" elements have negative handle, while ports have positive ones)
   */
  mutable util::tMutexLockOrder obj_mutex;

private:

  /*!
   * Adds child to parent (automatically called by constructor - may be called again though)
   * using specified link
   *
   * - Removes child from any former parent
   * - Init() method of child is not called - this has to be done
   *   separately prior to operation
   *
   * \param cLink link to child to use
   */
  void AddChild(tLink* child);

  /*!
   * \return Have all parents (including link parents) been initialized?
   * (may only be called in runtime-registry-synchronized context)
   */
  bool AllParentsReady();

  /*!
   * Initializes element and all child elements that were created by this thread
   * (helper method for init())
   * (may only be called in runtime-registry-synchronized context)
   */
  void CheckPublish();

  /*!
   * Deletes all children of this framework element.
   *
   * (may only be called in runtime-registry-synchronized context)
   */
  void DeleteChildren();

  /*!
   * \return Number of links to this port
   * (should be called in synchronized context)
   */
  size_t GetLinkCountHelper() const;

  /*!
   * same as above, but non-const
   * (should be called in synchronized context)
   */
  tLink* GetLinkInternal(size_t link_index);

  /*!
   * Helper for constructor (required for initializer-list in C++)
   *
   * \return Primary lock order
   */
  static int GetLockOrder(uint flags_, tFrameworkElement* parent, int lock_order);

  /*!
   * Recursive Helper function for above
   *
   * \param sb StringBuilder storing result
   * \param l Link to continue with
   * \param abort_at_link_root Abort when an alternative link root is reached?
   */
  static void GetNameHelper(util::tStringBuilder& sb, const tLink* l, bool abort_at_link_root);

  /*!
   * Very efficient implementation of above.
   * (StringBuilder may be reused)
   *
   * \param sb StringBuilder that will store result
   * \param start Link to start with
   * \param force_full_link Return full link from root (even if object has shorter globally unique link?)
   * \return Is this a globally unique link?
   */
  bool GetQualifiedName(util::tStringBuilder& sb, const tLink* start, bool force_full_link) const;

  /*!
   * Very efficient implementation of above.
   * (StringBuilder may be reused)
   *
   * \param sb StringBuilder that will store result
   * \param start Link to start with
   * \param force_full_link Return full link from root (even if object has shorter globally unique link?)
   * \return Is this a globally unique link?
   */
  bool GetQualifiedNameImpl(util::tStringBuilder& sb, const tLink* start, bool force_full_link) const;

  /*!
   * Initializes element and all child elements that were created by this thread
   * (helper method for init())
   * (may only be called in runtime-registry-synchronized context)
   */
  void InitImpl();

  /*!
   * \return Is current thread the thread that created this object?
   */
  inline bool IsCreator() const
  {
    return util::sThreadUtil::GetCurrentThreadId() == creater_thread_uid;
  }

  /*!
   * Deletes element and all child elements
   *
   * \param dont_detach Don't detach this link from parent (typically, because parent will clear child list)
   */
  void ManagedDelete(tLink* dont_detach);

  /*!
   * Helper method for deleting.
   * For some elements we need to lock runtime registry before locking this element.
   *
   * \return Returns runtime registry if this is the case - otherwise this-pointer.
   */
  util::tMutexLockOrder& RuntimeLockHelper() const;

protected:

  virtual ~tFrameworkElement();

  /*!
   * Helper for above
   *
   * \param name (relative) Qualified name
   * \param name_index Current index in string
   * \param only_globally_unique_children Only return child with globally unique link?
   * \param cLink root
   * \return Framework element - or null if non-existent
   */
  tFrameworkElement* GetChildElement(const util::tString& name, int name_index, bool only_globally_unique_children, tFrameworkElement* root);

  /*!
   * Create link to this framework element
   *
   * \param parent Parent framework element
   * \param link_name name of link
   */
  virtual void Link(tFrameworkElement* parent, const util::tString& link_name);

  /*!
   * Initializes this runtime element.
   * The tree structure should be established by now (Uid is valid)
   * so final initialization can be made.
   *
   * Called before children are initialized
   * (called in runtime-registry-synchronized context)
   */
  virtual void PostChildInit()
  {
  }

  /*!
   * Initializes this runtime element.
   * The tree structure should be established by now (Uid is valid)
   * so final initialization can be made.
   *
   * Called before children are initialized
   * (called in runtime-registry-synchronized context)
   */
  virtual void PreChildInit()
  {
  }

  /*!
   * Prepares element for deletion.
   * Port, for instance, are removed from edge lists etc.
   * The final deletion will be done by the GarbageCollector thread after
   * a few seconds (to ensure no other thread is working on this object
   * any more).
   *
   * Is called _BEFORE_ prepareDelete of children
   *
   * (is called with lock on this framework element and possibly all of its parent,
   *  but not runtime-registry. Keep this in mind when cleaning up & joining threads.
   *  This is btw the place to do that.)
   *
   */
  virtual void PrepareDelete()
  {
  }

  /*!
   * Helper for above
   *
   * \param indent Current indentation level
   * \param output Only used in C++ for streaming
   */
  virtual void PrintStructure(int indent, std::stringstream& output);

  /*!
   * Publish updated edge information
   *
   * \param change_type Type of change (see Constants in RuntimeListener class)
   * \param target Target of edge (this is source)
   *
   * (should only be called by AbstractPort class)
   */
  void PublishUpdatedEdgeInfo(int8 change_type, tAbstractPort* target);

  /*!
   * Publish updated port information
   *
   * \param change_type Type of change (see Constants in RuntimeListener class)
   *
   * (should only be called by FrameworkElement class)
   */
  void PublishUpdatedInfo(int8 change_type);

  /*!
   * (Needs to be synchronized because change operation is not atomic).
   *
   * \param flag Flag to remove
   */
  void RemoveFlag(int flag);

  /*!
   * \param flag Flag to set
   * \param value Value to set value to
   */
  void SetFlag(int flag, bool value);

  /*!
   * (Needs to be synchronized because change operation is not atomic).
   *
   * \param flag Flag to set
   */
  void SetFlag(int flag);

public:

  /*!
   * \param name Name of framework element (will be shown in browser etc.)
   * \param parent Parent of framework element (only use non-initialized parent! otherwise null and addChild() later; meant only for convenience)
   * \param flags Any special flags for framework element
   * \param lock_order Custom value for lock order (needs to be larger than parent's) - negative indicates unused.
   */
  tFrameworkElement(tFrameworkElement* parent = NULL, const util::tString& name = "", uint flags = tCoreFlags::cALLOWS_CHILDREN, int lock_order = -1);

  /*!
   * Add Child to framework element
   * (It will be initialized as soon as this framework element is - or instantly if this has already happened)
   *
   * \param fe Framework element to add (must not have been initialized already - structure is fixes in this case)
   */
  inline void AddChild(tFrameworkElement* fe)
  {
    AddChild(&(fe->primary));
  }

  /*!
   * \return Number of children (includes ones that are not initialized yet)
   * (thread-safe, however, call with runtime registry lock to get exact result when other threads might concurrently add/remove children)
   */
  size_t ChildCount() const;

  /*!
   * \return Number of children (this is only upper bound after deletion of objects - since some entries can be null)
   */
  inline size_t ChildEntryCount() const
  {
    return children.Size();
  }

  /*!
   * Called whenever static parameters of this framework element need to be (re)evaluated.
   * (can be overridden to handle this event)
   *
   * This typically happens at initialization and when user changes them via finstruct.
   * (This is never called when thread in surrounding thread container is running.)
   * (This must only be called with lock on runtime registry.)
   */
  virtual void EvaluateStaticParameters() {}

  /*!
   * \return Returns constant and non-constant flags
   */
  inline uint GetAllFlags() const
  {
    return flags | const_flags;
  }

  /*!
   * \param name Name
   * \return Returns first child with specified name - null if none exists
   */
  tFrameworkElement* GetChild(const util::tString& name) const;

  /*!
   * Same as getName()
   * (except that we return a const char*)
   */
  const char* GetCName() const;

  /*!
   * \param name (relative) Qualified name
   * \param only_globally_unique_children Only return child with globally unique link?
   * \return Framework element - or null if non-existent
   */
  tFrameworkElement* GetChildElement(const util::tString& name, bool only_globally_unique_children);

  /*!
   * (Should only be called by ChildIterator and FrameworkElementTreeFilter)
   * \return Array with child elements (careful, do not modify array!)
   */
  inline const util::tArrayWrapper<tLink*>* GetChildren() const
  {
    return children.GetIterable();
  }

  /*!
   * Is specified flag set?
   * for convenience - don't use in places with absolute maximum performance requirements (?)
   *
   * \param flag Flag to check
   * \return Answer
   */
  inline bool GetFlag(const uint flag) const
  {
    if (flag <= tCoreFlags::cCONSTANT_FLAGS)
    {
      return (const_flags & flag) == flag;
    }
    else
    {
      return (flags & flag) == flag;
    }
  }

  /*!
   * \return Element's handle in local runtime environment
   * ("normal" elements have negative handle, while ports have positive ones)
   */
  inline int GetHandle() const
  {
    return obj_mutex.GetSecondary();
  }

  /*!
   * \param link_index Index of link (0 = primary)
   * \return Link with specified index
   * (should be called in synchronized context)
   */
  const tLink* GetLink(size_t link_index) const;

  /*!
   * \return Number of links to this port
   * (should be called in synchronized context)
   */
  size_t GetLinkCount() const;

  /*!
   * \return Order value in which element needs to be locked (higher means later/after)
   */
  inline int GetLockOrder() const
  {
    return obj_mutex.GetPrimary();
  }

  inline const tFrameworkElement& GetLogDescription() const
  {
    return *this;
  }

  /*!
   * \return Name of this framework element
   */
  const util::tString GetName() const;

  /*!
   * \return Primary parent framework element
   */
  inline tFrameworkElement* GetParent() const
  {
    return primary.parent;
  }

  /*!
   * \param link_index Link that is referred to (0 = primary link)
   * \return Parent of framework element using specified link
   */
  tFrameworkElement* GetParent(int link_index) const;

  /*!
   * Returns first parent that has the specified flags
   *
   * \param flags Flags to look for
   * \return Parent or null
   */
  tFrameworkElement* GetParentWithFlags(uint flags_) const;

  /*!
   * (Use StringBuilder version if efficiency or real-time is an issue)
   * \return Qualified link to this element (may be shorter than qualified name, if object has a globally unique link)
   */
  inline util::tString GetQualifiedLink() const
  {
    util::tStringBuilder sb;
    GetQualifiedLink(sb);
    return sb.ToString();
  }

  /*!
   * Efficient variant of above.
   * (StringBuilder may be reused)
   *
   * \param sb StringBuilder that will store result
   * \return Is this link globally unique?
   */
  inline bool GetQualifiedLink(util::tStringBuilder& sb) const
  {
    return GetQualifiedLink(sb, &(primary));
  }

  /*!
   * Efficient variant of above.
   * (StringBuilder may be reused)
   *
   * \param sb StringBuilder that will store result
   * \param link_index Index of link to start with
   * \return Is this link globally unique?
   */
  inline bool GetQualifiedLink(util::tStringBuilder& sb, size_t link_index) const
  {
    return GetQualifiedLink(sb, GetLink(link_index));
  }

  /*!
   * Efficient variant of above.
   * (StringBuilder may be reused)
   *
   * \param sb StringBuilder that will store result
   * \param start Link to start with
   * \return Is this link globally unique?
   */
  inline bool GetQualifiedLink(util::tStringBuilder& sb, const tLink* start) const
  {
    return GetQualifiedName(sb, start, false);
  }

  /*!
   * (Use StringBuilder version if efficiency or real-time is an issue)
   * \return Concatenation of parent names and this element's name
   */
  inline util::tString GetQualifiedName() const
  {
    util::tStringBuilder sb;
    GetQualifiedName(sb);
    return sb.ToString();
  }

  /*!
   * Efficient variant of above.
   * (StringBuilder may be reused)
   *
   * \param sb StringBuilder that will store result
   */
  inline void GetQualifiedName(util::tStringBuilder& sb) const
  {
    GetQualifiedName(sb, &(primary));
  }

  /*!
   * Efficient variant of above.
   * (StringBuilder may be reused)
   *
   * \param sb StringBuilder that will store result
   * \param link_index Index of link to start with
   */
  inline void GetQualifiedName(util::tStringBuilder& sb, size_t link_index) const
  {
    GetQualifiedName(sb, GetLink(link_index));
  }

  /*!
   * Efficient variant of above.
   * (StringBuilder may be reused)
   *
   * \param sb StringBuilder that will store result
   * \param start Link to start with
   */
  inline void GetQualifiedName(util::tStringBuilder& sb, const tLink* start) const
  {
    GetQualifiedName(sb, start, true);
  }

  /*!
   * (for convenience)
   * \return Registry of the one and only RuntimeEnvironment - Structure changing operations need to be synchronized on this object!
   * (Only lock runtime for minimal periods of time!)
   */
  util::tMutexLockOrder& GetRegistryLock() const;

  /*!
   * (for convenience)
   * \return The one and only RuntimeEnvironment
   */
  tRuntimeEnvironment* GetRuntime() const;

  /*!
   * Initialize this framework element and all framework elements in sub tree that were created by this thread
   * and weren't initialized already.
   *
   * This must be called prior to using framework elements - and in order to them being published.
   */
  void Init();

  /*!
   * Initializes all unitialized framework elements created by this thread
   */
  static void InitAll();

  /*!
   * Is Runtime element a child of the specified Runtime element?
   * (also considers links)
   *
   * \param re Possible parent of this Runtime element
   * \return Answer
   */
  inline bool IsChildOf(tFrameworkElement* re) const
  {
    return IsChildOf(re, false);
  }

  /*!
   * Is Runtime element a child of the specified Runtime element?
   * (also considers links)
   *
   * \param re Possible parent of this Runtime element
   * \param ignore_delete_flag Perform check even if delete flag is already set on object (deprecated in C++ - except of directly calling on runtime change)
   * \return Answer
   */
  bool IsChildOf(tFrameworkElement* re, bool ignore_delete_flag) const;

  /*!
   * \return true before element is officially declared as being initialized
   */
  inline bool IsConstructing() const
  {
    return !GetFlag(tCoreFlags::cREADY);
  }

  /*!
   * \return Has framework element been deleted? - dangerous if you actually encounter this in C++...
   */
  inline bool IsDeleted() const
  {
    return (flags & tCoreFlags::cDELETED) > 0;
  }

  /*!
   * \return true after the element has been initialized - equivalent to isReady()
   */
  inline bool IsInitialized() const
  {
    return IsReady();
  }

  /*!
   * \return Is element a port?
   */
  inline bool IsPort() const
  {
    return (const_flags & tCoreFlags::cIS_PORT) > 0;
  }

  /*!
   * \return Is framework element ready/fully initialized and not yet deleted?
   */
  inline bool IsReady() const
  {
    return (flags & tCoreFlags::cREADY) > 0;
  }

  /*!
   * Can this framework element be locked after the specified one has been locked?
   *
   * \param fe Specified other framework element
   * \return Answer
   */
  inline bool LockAfter(const tFrameworkElement* fe) const
  {
    return obj_mutex.ValidAfter(fe->obj_mutex);
  }

  /*!
   * Deletes element and all child elements
   */
  inline void ManagedDelete()
  {
    ManagedDelete(NULL);
  }

  /*!
   * Are name of this element and String 'other' identical?
   * (result is identical to getName().equals(other); but more efficient in C++)
   *
   * \param other Other String
   * \return Result
   */
  bool NameEquals(const util::tString& other) const;

  /*!
   * Helper for Debugging: Prints structure below this framework element to console
   */
  inline void PrintStructure()
  {
    PrintStructure(rrlib::logging::eLL_USER);
  }

  /*!
   * Helper for Debugging: Prints structure below this framework element to log domain
   *
   * \param ll Loglevel with which to print
   */
  void PrintStructure(rrlib::logging::tLogLevel ll);

  /*!
   * \param name New Port name
   * (only valid/possible before, element is initialized)
   */
  void SetName(const util::tString& name);

  // for efficient streaming of fully-qualified framework element name
  void StreamQualifiedName(std::ostream& output) const
  {
    if (!GetFlag(tCoreFlags::cIS_RUNTIME))
    {
      StreamQualifiedParent(output);
    }
    output << GetCName();
  }

  void StreamQualifiedParent(std::ostream& output) const
  {
    const tFrameworkElement* parent = GetParent();
    if (parent != NULL && (!parent->GetFlag(tCoreFlags::cIS_RUNTIME)))
    {
      parent->StreamQualifiedParent(output);
      output << parent->GetCName();
      output << "/";
    }
  }

  virtual const util::tString ToString() const
  {
    return GetName();
  }

  /*!
   * Write name of link number i to Output stream
   *
   * \param os OutputStream
   * \param i Link Number (0 is primary link/name)
   */
  void WriteName(rrlib::serialization::tOutputStream& os, int i) const;

public:

  /*!
   * \author Max Reichardt
   *
   * Used to iterate over a framework element's children.
   */
  class tChildIterator : public util::tObject
  {
  private:

    // next element to check (in array)
    tFrameworkElement::tLink* const * next_elem;

    // last element in array
    tFrameworkElement::tLink* const * last;

    /*! Relevant flags */
    uint flags;

    /*! Expected result when ANDing with flags */
    uint result;

  protected:

    /*! FrameworkElement that is currently iterated over */
    const tFrameworkElement* cur_parent;

  public:

    tChildIterator(const tFrameworkElement* parent);

    /*!
     * \param parent Framework element over whose child to iterate
     * \param flags Flags that children must have in order to be considered
     */
    tChildIterator(const tFrameworkElement* parent, uint flags_);

    /*!
     * \param parent Framework element over whose child to iterate
     * \param flags Relevant flags
     * \param result Result that ANDing flags with flags must bring (allows specifying that certain flags should not be considered)
     */
    tChildIterator(const tFrameworkElement* parent, uint flags_, uint result_);

    /*!
     * \param parent Framework element over whose child to iterate
     * \param flags Relevant flags
     * \param result Result that ANDing flags with flags must bring (allows specifying that certain flags should not be considered)
     * \param include_non_ready Include children that are not fully initialized yet?
     */
    tChildIterator(const tFrameworkElement* parent, uint flags_, uint result_, bool include_non_ready);

    /*!
     * \return Next child - or null if there are no more children left
     */
    tFrameworkElement* Next();

    /*!
     * \return Next child that is a port - or null if there are no more children left
     */
    tAbstractPort* NextPort();

    /*!
     * Use iterator again on same framework element
     */
    inline void Reset()
    {
      Reset(cur_parent);
    }

    /*!
     * Use Iterator for different framework element
     * (or same and reset)
     *
     * \param parent Framework element over whose child to iterate
     */
    inline void Reset(const tFrameworkElement* parent)
    {
      Reset(parent, 0, 0);
    }

    /*!
     * Use Iterator for different framework element
     * (or same and reset)
     *
     * \param parent Framework element over whose child to iterate
     * \param flags Flags that children must have in order to be considered
     */
    inline void Reset(const tFrameworkElement* parent, uint flags_)
    {
      Reset(parent, flags_, flags_);
    }

    /*!
     * Use Iterator for different framework element
     * (or same and reset)
     *
     * \param parent Framework element over whose child to iterate
     * \param flags Relevant flags
     * \param result Result that ANDing flags with flags must bring (allows specifying that certain flags should not be considered)
     */
    inline void Reset(const tFrameworkElement* parent, uint flags_, uint result_)
    {
      Reset(parent, flags_, result_, false);
    }

    /*!
     * Use Iterator for different framework element
     * (or same and reset)
     *
     * \param parent Framework element over whose child to iterate
     * \param flags Relevant flags
     * \param result Result that ANDing flags with flags must bring (allows specifying that certain flags should not be considered)
     * \param include_non_ready Include children that are not fully initialized yet?
     */
    void Reset(const tFrameworkElement* parent, uint flags_, uint result_, bool include_non_ready);

  };

};

} // namespace finroc
} // namespace core

namespace finroc
{
namespace core
{
inline std::ostream& operator << (std::ostream& output, const tFrameworkElement* lu)
{
  lu->StreamQualifiedName(output);
  output << " (" << ((void*)lu) << ")";
  return output;
}
inline std::ostream& operator << (std::ostream& output, const tFrameworkElement& lu)
{
  output << (&lu);
  return output;
}

} // namespace finroc
} // namespace core

#endif // core__tFrameworkElement_h__
