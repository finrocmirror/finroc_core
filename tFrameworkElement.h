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
#include <array>

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
 *
 * A framework element can be locked with
 *   util::tLock lock(<framework_element>)
 *
 * That's why it inherits from tMutexLockOrder.
 * This also defines the lock order in which framework elements can be locked.
 * Generally the framework element tree is locked from root to leaves.
 * So children's lock level needs to be larger than their parent's.
 *
 * tMutexLockOrder's secondary component is the element's unique handle in local runtime environment.
 * ("normal" elements have negative handle, while ports have positive ones)
 */
class tFrameworkElement : public tAnnotatable, public rrlib::thread::tRecursiveMutex
{
public:

  /*!
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
    tFrameworkElement& outer_class;

    /*! Name of Framework Element - in link context */
    util::tString name;

    /*! Parent - Element in which this link was inserted */
    tFrameworkElement* parent;

    /*! Next link for this framework element (=> singly-linked list) */
    tFrameworkElement::tLink* next;

  public:

    tLink(tFrameworkElement& outer_class) :
      outer_class(outer_class),
      name(),
      parent(NULL),
      next(NULL)
    {}

    /*!
     * \return Element that this link points to
     */
    inline tFrameworkElement& GetChild() const
    {
      return outer_class;
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
      return this == &(GetChild().primary);
    }

  };

  /*! Iterator filter that accepts all elements */
  struct tIteratorFilterNone
  {
    static bool Accept(const tFrameworkElement& fe)
    {
      return true;
    }
  };

  /*! Iterator filter that accepts only ports */
  struct tIteratorFilterPorts
  {
    static bool Accept(const tFrameworkElement& fe)
    {
      return fe.IsPort();
    }
  };

  // Iterator types

  template <typename ELEMENT, typename FILTER, bool INCLUDE_LINKS = true>
  class tChildIteratorImpl;
  class tSubElementIterator;
  typedef tChildIteratorImpl<tFrameworkElement, tIteratorFilterNone> tChildIterator;
  typedef tChildIteratorImpl<tAbstractPort, tIteratorFilterPorts> tChildPortIterator;

private:

  friend class util::tGarbageDeleter;
  friend class tRuntimeEnvironment;
  friend class tFinstructableGroup;

  /*! Primary link to framework element - the place at which it actually is in FrameworkElement tree - contains name etc. */
  tLink primary;

protected:

  /*! Uid of thread that created this framework element */
  const int64 creater_thread_uid;

  // Splitting flags up might allow compiler optimizations??

  /*! Constant Flags - see CoreFlags */
  const uint const_flags;

  /*! Variable Flags - see CoreFlags; Functions modifying this must acquire simple_mutex */
  uint flags;

  /*! children - may contain null entries (for efficient thread-safe unsynchronized iteration) */
  util::tSafeConcurrentlyIterableList<tLink*, rrlib::thread::tNoMutex> children;

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
  void AddChild(tLink& child);

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
   * \param sb Buffer storing result
   * \param l Link to continue with
   * \param abort_at_link_root Abort when an alternative link root is reached?
   */
  static void GetNameHelper(util::tString& sb, const tLink& l, bool abort_at_link_root);

  /*!
   * Very efficient implementation of above.
   * (StringBuilder may be reused)
   *
   * \param sb Buffer that will store result
   * \param start Link to start with
   * \param force_full_link Return full link from root (even if object has shorter globally unique link?)
   * \return Is this a globally unique link?
   */
  bool GetQualifiedName(util::tString& sb, const tLink& start, bool force_full_link) const;

  /*!
   * Very efficient implementation of above.
   * (StringBuilder may be reused)
   *
   * \param sb Buffer that will store result
   * \param start Link to start with
   * \param force_full_link Return full link from root (even if object has shorter globally unique link?)
   * \return Is this a globally unique link?
   */
  bool GetQualifiedNameImpl(util::tString& sb, const tLink& start, bool force_full_link) const;

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
    return rrlib::thread::tThread::CurrentThreadId() == creater_thread_uid;
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
  const rrlib::thread::tRecursiveMutex& RuntimeLockHelper() const;

protected:

  virtual ~tFrameworkElement();

  /*!
   * Checks if specified link can be added to parent.
   * Aborts program if this is not the case.
   *
   * (Check: Framework elements may only have multiple children with the same name
   *  if explicitly allowed in runtime settings.)
   */
  void CheckForNameClash(const tLink& link);

  /*!
   * Helper for above
   *
   * \param name (relative) Qualified name
   * \param name_index Current index in string
   * \param only_globally_unique_children Only return child with globally unique link?
   * \param cLink root
   * \return Framework element - or null if non-existent
   */
  tFrameworkElement* GetChildElement(const util::tString& name, int name_index, bool only_globally_unique_children, tFrameworkElement& root);

  /*!
   * Create link to this framework element
   *
   * \param parent Parent framework element
   * \param link_name name of link
   */
  virtual void Link(tFrameworkElement& parent, const util::tString& link_name);

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
  void PublishUpdatedEdgeInfo(int8 change_type, tAbstractPort& target);

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
  explicit tFrameworkElement(tFrameworkElement* parent = NULL, const util::tString& name = "", uint flags = tCoreFlags::cALLOWS_CHILDREN, int lock_order = -1);

  /*!
   * Add Child to framework element
   * (It will be initialized as soon as this framework element is - or instantly if this has already happened)
   *
   * \param fe Framework element to add (must not have been initialized already - structure is fixes in this case)
   */
  inline void AddChild(tFrameworkElement& fe)
  {
    AddChild(fe.primary);
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
   * \return An iterator to iterate over this node's child ports. Initially points to the first port.
   *
   * Typically used in this way (fe is a tFrameworkElement reference):
   *
   *   for (auto it = fe.ChildPortsBegin(); it < fe.ChildPortsEnd(); ++it)
   *   {
   *     if (it->IsReady())
   *     {
   *       ...
   *     }
   *   }
   */
  inline tChildPortIterator ChildPortsBegin() const
  {
    return tChildPortIterator(*children.GetIterable());
  }

  /*!
   * \return An iterator to iterate over this node's child ports pointing to the past-the-end port.
   */
  inline tChildPortIterator ChildPortsEnd() const
  {
    return tChildPortIterator();
  }

  /*!
   * \return An iterator to iterate over this node's children. Initially points to the first element.
   *
   * Typically used in this way (fe is a tFrameworkElement reference):
   *
   *   for (auto it = fe.ChildrenBegin(); it < fe.ChildrenEnd(); ++it)
   *   {
   *     if (it->IsReady())
   *     {
   *       ...
   *     }
   *   }
   */
  inline tChildIterator ChildrenBegin() const
  {
    return tChildIterator(*children.GetIterable());
  }

  /*!
   * \return An iterator to iterate over this node's children pointing to the past-the-end element.
   */
  inline tChildIterator ChildrenEnd() const
  {
    return tChildIterator();
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
    return GetSecondary();
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
    return GetPrimary();
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
    util::tString sb;
    GetQualifiedLink(sb);
    return sb;
  }

  /*!
   * Efficient variant of above.
   * (StringBuilder may be reused)
   *
   * \param sb Buffer that will store result
   * \return Is this link globally unique?
   */
  inline bool GetQualifiedLink(util::tString& sb) const
  {
    return GetQualifiedLink(sb, primary);
  }

  /*!
   * Efficient variant of above.
   * (StringBuilder may be reused)
   *
   * \param sb Buffer that will store result
   * \param link_index Index of link to start with
   * \return Is this link globally unique?
   */
  inline bool GetQualifiedLink(util::tString& sb, size_t link_index) const
  {
    const tLink* link = GetLink(link_index);
    assert(link);
    return GetQualifiedLink(sb, *link);
  }

  /*!
   * Efficient variant of above.
   * (StringBuilder may be reused)
   *
   * \param sb Buffer that will store result
   * \param start Link to start with
   * \return Is this link globally unique?
   */
  inline bool GetQualifiedLink(util::tString& sb, const tLink& start) const
  {
    return GetQualifiedName(sb, start, false);
  }

  /*!
   * (Use StringBuilder version if efficiency or real-time is an issue)
   * \return Concatenation of parent names and this element's name
   */
  inline util::tString GetQualifiedName() const
  {
    util::tString sb;
    GetQualifiedName(sb);
    return sb;
  }

  /*!
   * Efficient variant of above.
   * (StringBuilder may be reused)
   *
   * \param sb Buffer that will store result
   */
  inline void GetQualifiedName(util::tString& sb) const
  {
    GetQualifiedName(sb, primary);
  }

  /*!
   * Efficient variant of above.
   * (StringBuilder may be reused)
   *
   * \param sb Buffer that will store result
   * \param link_index Index of link to start with
   */
  inline void GetQualifiedName(util::tString& sb, size_t link_index) const
  {
    const tLink* link = GetLink(link_index);
    assert(link);
    GetQualifiedName(sb, *link);
  }

  /*!
   * Efficient variant of above.
   * (StringBuilder may be reused)
   *
   * \param sb Buffer that will store result
   * \param start Link to start with
   */
  inline void GetQualifiedName(util::tString& sb, const tLink& start) const
  {
    GetQualifiedName(sb, start, true);
  }

  /*!
   * (for convenience)
   * \return Registry of the one and only RuntimeEnvironment - Structure changing operations need to be synchronized on this object!
   * (Only lock runtime for minimal periods of time!)
   */
  const rrlib::thread::tRecursiveMutex& GetRegistryLock() const;

  /*!
   * (for convenience)
   * \return The one and only RuntimeEnvironment
   */
  tRuntimeEnvironment& GetRuntime() const;

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
  inline bool IsChildOf(const tFrameworkElement& re) const
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
  bool IsChildOf(const tFrameworkElement& re, bool ignore_delete_flag) const;

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
  inline bool LockAfter(const tFrameworkElement& fe) const
  {
    return ValidAfter(fe);
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
  void PrintStructure();

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

  /*!
   * \param include_this Include this element (root of the subtree) in iteration?
   * \return An iterator to iterate over each element of the complete subtree below this framework element. Initially points to the first direct child.
   *
   * Typically used in this way (fe is a tFrameworkElement reference):
   *
   *   for (auto it = fe.SubElementsBegin(); it < fe.SubElementsEnd(); ++it)
   *   {
   *     if (it->IsReady())
   *     {
   *       ...
   *     }
   *   }
   */
  inline tSubElementIterator SubElementsBegin(bool include_this = false)
  {
    return tSubElementIterator(*this, include_this);
  }

  /*!
   * \return An iterator to iterate over each element of the complete subtree below this framework element pointing to the past-the-end element.
   */
  inline tSubElementIterator SubElementsEnd() const
  {
    return tSubElementIterator();
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


  /*!
   * Used to iterate over a framework element's children (Input Operator)
   */
  template <typename ELEMENT, typename FILTER, bool INCLUDE_LINKS>
  class tChildIteratorImpl : public std::iterator<std::forward_iterator_tag, ELEMENT, size_t>
  {
  private:

    friend class tFrameworkElement;
    typedef std::iterator<std::forward_iterator_tag, ELEMENT, size_t> tBase;

    /*! current element - NULL if last entry was passed */
    tFrameworkElement::tLink* const * current_element;

    /*! last element in array */
    tFrameworkElement::tLink* const * last;

    /*!
     * \param array Array to iterate over
     */
    inline tChildIteratorImpl(util::tArrayWrapper<tLink*>& array) :
      current_element(array.GetPointer() == NULL ? NULL : array.GetPointer() - 1),
      last((array.GetPointer() + array.Size()) - 1)
    {
      operator++();
    }

  public:

    /*!
     * Constructor for end iterator
     */
    inline tChildIteratorImpl() : current_element(NULL), last(NULL) {}

    // Operators needed for C++ Input Iterator

    inline typename tBase::reference operator*() const
    {
      assert(current_element);
      return static_cast<ELEMENT&>((*current_element)->GetChild());
    }
    inline typename tBase::pointer operator->() const
    {
      return &(operator*());
    }

    inline tChildIteratorImpl& operator++()
    {
      if (current_element)
      {
        current_element++;
        while (current_element <= last)
        {
          tFrameworkElement::tLink* current_link = *current_element;
          if (current_link && FILTER::Accept((*current_element)->GetChild()) && (INCLUDE_LINKS || current_link->IsPrimaryLink()))
          {
            return *this;
          }
          current_element++;
        }

        current_element = NULL;
      }
      return *this;
    }
    inline tChildIteratorImpl operator ++ (int)
    {
      tChildIteratorImpl temp(*this);
      operator++();
      return temp;
    }

    inline const bool operator == (const tChildIteratorImpl &other) const
    {
      return current_element == other.current_element;
    }
    inline const bool operator != (const tChildIteratorImpl &other) const
    {
      return !(*this == other);
    }
  };


  /*!
   * Used to iterate over each element of the complete subtree below a framework element (Input Operator)
   * Ignores links.
   */
  class tSubElementIterator : public std::iterator<std::forward_iterator_tag, tFrameworkElement, size_t>
  {
  private:

    friend class tFrameworkElement;
    typedef tChildIteratorImpl<tFrameworkElement, tIteratorFilterNone, false> tNoLinkChildIterator;

    /*! Maximum tree depth the iterator implementation can handle */
    const static size_t cMAX_TREE_DEPTH = 100;

    /*! Array with current iterator in each tree level */
    std::array<tNoLinkChildIterator, cMAX_TREE_DEPTH> iterator_stack;

    /*! Current depth in tree */
    size_t current_depth;

    /*! Set, if currently at root element */
    tFrameworkElement* at_root;

    /*!
     * \param framework_element Framework element over whose subtree to iterate
     * \param include_root Include root element in iteration
     */
    inline tSubElementIterator(tFrameworkElement& framework_element, bool include_root) :
      current_depth(0),
      at_root(include_root ? &framework_element : NULL)
    {
      if (!include_root)
      {
        // first element
        iterator_stack[0] = tNoLinkChildIterator(*framework_element.children.GetIterable());
        if (iterator_stack[0] != tNoLinkChildIterator())
        {
          current_depth = 1;
        }
      }
    }

  public:

    /*!
     * Constructor for end iterator
     */
    inline tSubElementIterator() : current_depth(0), at_root(NULL) {}

    // Operators needed for C++ Input Iterator

    inline reference operator*() const
    {
      return at_root ? *at_root : *iterator_stack[current_depth - 1];
    }
    inline pointer operator->() const
    {
      return &(operator*());
    }

    inline tSubElementIterator& operator++()
    {
      if (current_depth || at_root)
      {
        tFrameworkElement& current_element = **this;
        at_root = NULL;
        iterator_stack[current_depth] = tNoLinkChildIterator(*current_element.children.GetIterable());
        if (iterator_stack[current_depth] == tNoLinkChildIterator())
        {
          iterator_stack[current_depth - 1]++;
          while (iterator_stack[current_depth - 1] == tNoLinkChildIterator())
          {
            current_depth--;
            if (current_depth == 0)
            {
              return *this; // end
            }
            iterator_stack[current_depth - 1]++;
          }
        }
        else
        {
          current_depth++;
        }
      }
      return *this;
    }

    inline tSubElementIterator operator ++ (int)
    {
      tSubElementIterator temp(*this);
      operator++();
      return temp;
    }

    inline const bool operator == (const tSubElementIterator &other) const
    {
      return current_depth == other.current_depth && at_root == other.at_root && std::equal(other.iterator_stack.begin(), other.iterator_stack.end(), iterator_stack.begin());
    }
    inline const bool operator != (const tSubElementIterator &other) const
    {
      return !(*this == other);
    }
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
