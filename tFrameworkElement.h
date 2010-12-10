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
#include "rrlib/finroc_core_utils/tJCBase.h"

#ifndef CORE__TFRAMEWORKELEMENT_H
#define CORE__TFRAMEWORKELEMENT_H

#include "rrlib/finroc_core_utils/container/tSafeConcurrentlyIterableList.h"
#include "core/tCoreFlags.h"
#include "rrlib/finroc_core_utils/thread/sThreadUtil.h"
#include "core/port/tThreadLocalCache.h"
#include "core/tAnnotatable.h"

namespace finroc
{
namespace core
{
class tRuntimeEnvironment;
class tAbstractPort;
class tCreateModuleAction;
class tConstructorParameters;
class tCoreOutput;

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

    /*! Description of Framework Element - in link context */
    util::tString description;

    /*! Parent - Element in which this link was inserted */
    tFrameworkElement* parent;

    /*! Next link for this framework element (=> singly-linked list) */
    tFrameworkElement::tLink* next;

  public:

    tLink(tFrameworkElement* const outer_class_ptr_) :
        outer_class_ptr(outer_class_ptr_),
        description(),
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
     * \return Description of Framework Element - in link context
     */
    inline util::tString GetDescription() const
    {
      return description;
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

  friend class util::tGarbageCollector;
  friend class tChildIterator;
  friend class tRuntimeEnvironment;
private:

  friend class tLock;

  /*! Primary link to framework element - the place at which it actually is in FrameworkElement tree - contains description etc. */
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
  const int const_flags;

  /*! Variable Flags - see CoreFlags */
  int flags;

  /*! children - may contain null entries (for efficient thread-safe unsynchronized iteration) */
  util::tSafeConcurrentlyIterableList<tLink*> children;

public:

  /*! RuntimeElement as TreeNode (only available if set in RuntimeSettings) */
  //@JavaOnly private final DefaultMutableTreeNode treeNode;

  /*! State Constants */
  //public enum State { CONSTRUCTING, READY, DELETED };

  /*! Element's state */
  //protected volatile State state = State.CONSTRUCTING;

  /*! Type Constants */
  //public enum Type { STANDARD_ELEMENT, RUNTIME, PORT }

  /*!
   * Element's handle in local runtime environment
   * ("normal" elements have negative handle, while ports have positive ones)
   * Now stored in objMutex
   */
  //@Const protected final int handle;

  /*!
   * Defines lock order in which framework elements can be locked.
   * Generally the framework element tree is locked from root to leaves.
   * So children's lock level needs to be larger than their parent's.
   *
   * The secondary component is the element's unique handle in local runtime environment.
   * ("normal" elements have negative handle, while ports have positive ones)
   */
  mutable util::tMutexLockOrder obj_mutex;

  /*! This flag is set to true when the element has been initialized */
  //private boolean initialized;

  /*! This flag is set to true when element is deleted */
  //protected volatile boolean deleted;

  /*! Is RuntimeElement member of remote runtime; -1 = unknown, 0 = no, 1 = yes */
  //private int remote = -1;

  /*!
   * List with owned ports.
   *
   * Methods that want to prevent a current port value (that this RuntimeElement manages)
   * being outdated and reused while they are
   * asynchronously getting it should synchronize with this list.
   * Port methods do this.
   *
   * The owner module will synchronize on this list, every time it updates valuesBeforeAreUnused
   * variable of the ports.
   */
  //protected final SafeArrayList<Port<?>> ownedPorts;

  /*! Main Thread. This thread can write to the runtime element's ports without synchronization */
  //protected Thread mainThread;

  /*! Log domain for this class */
  RRLIB_LOG_CREATE_NAMED_DOMAIN(log_domain, "framework_elements");

  /*! Log domain for edges */
  RRLIB_LOG_CREATE_NAMED_DOMAIN(edge_log, "edges");

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
  static int GetLockOrder(int flags_, tFrameworkElement* parent, int lock_order);

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

  //  /**
  //   * Initialize this framework element and all framework elements in sub tree that were created by this thread
  //   * and weren't initialized already - provided that the parent element already is initialized.
  //   * (If parent is not initialized, nothing happens)
  //   *
  //   * Initializing must be done prior to using framework elements - and in order to them being published.
  //   */
  //  public void initIfParentIs() {
  //      FrameworkElement parent = getParent();
  //      synchronized(parent.children) {
  //          if (parent.isReady()) {
  //              init();
  //          }
  //      }
  //  }

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
   * Is Runtime element a child of the specified Runtime element?
   * (also considers links)
   *
   * \param re Possible parent of this Runtime element
   * \param ignore_delete_flag Perform check even if delete flag is already set on object (deprecated in C++!)
   * \return Answer
   */
  bool IsChildOf(tFrameworkElement* re, bool ignore_delete_flag) const;

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
  virtual void PrintStructure(int indent, rrlib::logging::tLogStream& output);

  //  /**
  //   * Called whenever an asynchronous call returns.
  //   * May be overriden/implemented by subclasses.
  //   * Default behaviour is throwing an Exception.
  //   * (Should only be called by framework-internal classes)
  //   *
  //   * \param pc Call Object containing various parameters
  //   */
  //  @InCppFile
  //  @Virtual public void handleCallReturn(AbstractCall pc) {
  //      throw new RuntimeException("This FrameworkElement cannot handle call returns");
  //  }

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
   * \param description_ Description of framework element (will be shown in browser etc.) - may not be null
   * \param parent_ Parent of framework element (only use non-initialized parent! otherwise null and addChild() later; meant only for convenience)
   * \param flags_ Any special flags for framework element
   * \param lock_order_ Custom value for lock order (needs to be larger than parent's) - negative indicates unused.
   */
  tFrameworkElement(tFrameworkElement* parent_ = NULL, const util::tString& description_ = "", int flags_ = tCoreFlags::cALLOWS_CHILDREN, int lock_order_ = -1);

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
   * Are description of this element and String 'other' identical?
   * (result is identical to getDescription().equals(other); but more efficient in C++)
   *
   * \param other Other String
   * \return Result
   */
  bool DescriptionEquals(const util::tString& other);

  /*!
   * \return Returns constant and non-constant flags
   */
  inline int GetAllFlags() const
  {
    return flags | const_flags;
  }

  /*!
   * \return RuntimeElement as TreeNode (only available if set in RuntimeSettings)
   */
  /*@JavaOnly public DefaultMutableTreeNode asTreeNode() {
      return treeNode;
  }*/

  /*!
   * \param manages_ports Is RuntimeElement responsible for releasing unused port values?
   */
  //@SuppressWarnings("unchecked")
  //public FrameworkElement(String description, boolean managesPorts, Thread mainThread) {
  //ownedPorts = managesPorts ? new SafeArrayList<Port<?>>() : null;
  //mainThread = mainThread == null ? (managesPorts ? Thread.currentThread() : null) : mainThread;
  //}

  //    /**
  //     * \return Tree Node representation of this runtime object
  //     */
  //    @JavaOnly protected DefaultMutableTreeNode createTreeNode() {
  //        return new DefaultMutableTreeNode(primary.description);
  //    }

  /*!
   *  same as below -
   *  except that we return a const char* in C++ - this way, no memory needs to be allocated
   */
  const char* GetCDescription() const;

  /*!
   * \param name Description
   * \return Returns first child with specified description - null if none exists
   */
  tFrameworkElement* GetChild(const util::tString& name) const;

  //  /**
  //   * Does element with specified qualified name exist?
  //   *
  //   * \param name (relative) Qualified name
  //   * \return Answer
  //   */
  //  public boolean elementExists(@Const @Ref String name) {
  //      return getChildElement(name, false) != null;
  //  }

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
   * \return Name/Description
   */
  const util::tString GetDescription() const;

  /*!
   * Is specified flag set?
   * for convenience - don't use in places with absolute maximum performance requirements (?)
   *
   * \param flag Flag to check
   * \return Answer
   */
  inline bool GetFlag(const int flag) const
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

  //  ///////////////////// real factory methods /////////////////////////
  //  //public Port<?>
  //
  //  protected NumberPort addNumberPort(boolean output, @Const @Ref String description, @CppDefault("CoreNumber::ZERO") @Const @Ref Number defaultValue, @CppDefault("NULL") Unit unit) {
  //      return addNumberPort(output ? PortFlags.OUTPUT_PORT : PortFlags.INPUT_PORT, description, defaultValue, unit);
  //  }
  //
  //  protected NumberPort addNumberPort(int flags, @Const @Ref String description, @CppDefault("CoreNumber::ZERO") @Const @Ref Number defaultValue, @CppDefault("NULL") Unit unit) {
  //      PortCreationInfo pci = new PortCreationInfo(description, this, flags);
  //      //PortDataCreationInfo.get().set(DataTypeRegister2.getDataTypeEntry(CoreNumberContainer.class), owner, prototype);
  //      //pci.defaultValue = new CoreNumberContainer(new CoreNumber2(defaultValue, unit));
  //      pci.unit = unit;
  //      //pci.parent = this;
  //      NumberPort np = new NumberPort(pci);
  //      np.getDefaultBuffer().setValue(defaultValue, unit != null ? unit : Unit.NO_UNIT);
  //      //addChild(np);
  //      return np;
  //  }
  //
  //  ///////////////////// convenience factory methods /////////////////////////
  //  @JavaOnly public NumberPort addNumberInputPort(@Const @Ref String description) {
  //      return addNumberInputPort(description, CoreNumber.ZERO, Unit.NO_UNIT);
  //  }
  //  @JavaOnly public NumberPort addNumberInputPort(@Const @Ref String description, @Const @Ref Number defaultValue) {
  //      return addNumberInputPort(description, defaultValue, Unit.NO_UNIT);
  //  }
  //  @JavaOnly public NumberPort addNumberInputPort(@Const @Ref String description, @Ptr Unit unit) {
  //      return addNumberInputPort(description, CoreNumber.ZERO, unit);
  //  }
  //  public NumberPort addNumberInputPort(@Const @Ref String description, @CppDefault("CoreNumber::ZERO") @Const @Ref Number defaultValue, @CppDefault("NULL") @Ptr Unit unit) {
  //      return addNumberPort(false, description, defaultValue, unit);
  //  }
  //  /*public NumberPort addNumberInputPort(String description, Number defaultValue, Unit unit, double min, double max) {
  //
  //  }*/
  //
  //  @JavaOnly public NumberPort addNumberOutputPort(@Const @Ref String description) {
  //      return addNumberOutputPort(description, CoreNumber.ZERO, Unit.NO_UNIT);
  //  }
  //  @JavaOnly public NumberPort addNumberOutputPort(@Const @Ref String description, @Const @Ref Number defaultValue) {
  //      return addNumberOutputPort(description, defaultValue, Unit.NO_UNIT);
  //  }
  //  @JavaOnly public NumberPort addNumberOutputPort(@Const @Ref String description, @Ptr Unit unit) {
  //      return addNumberOutputPort(description, CoreNumber.ZERO, unit);
  //  }
  //  public NumberPort addNumberOutputPort(@Const @Ref String description, @CppDefault("CoreNumber::ZERO") @Const @Ref Number defaultValue, @CppDefault("NULL") @Ptr Unit unit) {
  //      return addNumberPort(true, description, defaultValue, unit);
  //  }
  //
  //  public NumberPort addNumberProxyPort(boolean output, @Const @Ref String description, @CppDefault("CoreNumber::ZERO") @Const @Ref Number defaultValue, @CppDefault("NULL") @Ptr Unit unit) {
  //      return addNumberPort(output ? PortFlags.OUTPUT_PROXY : PortFlags.INPUT_PROXY, description, defaultValue, unit);
  //  }

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
  tFrameworkElement* GetParentWithFlags(int flags_);

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

  //  /**
  //   * Get all children with specified flags set/unset
  //   * (Don't store this array for longer than possible
  //   *
  //   * \param result Children will be copied to this array (if there are more, array will be filled completely)
  //   * \param checkFlags Flags to check
  //   * \param checkResult The result the check has to have in order to add child to result
  //   * \return Number of elements in result.
  //   */
  //  public @SizeT int getChildrenFlagged(@Ref FrameworkElement[] result, int checkFlags, int checkResult) {
  //
  //  }

  /*!
   * (Use StringBuilder version if efficiency or real-time is an issue)
   * \return Concatenation of parent descriptions and this element's description
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
   * (for convenience)
   * \return List with all thread local caches - Some cleanup methods require that this is locked
   * (Only lock runtime for minimal periods of time!)
   */
  util::tMutexLockOrder& GetThreadLocalCacheInfosLock() const;

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

  //  /**
  //   * \return Returns the element's uid, which is the concatenated description of it and all parents.
  //   * It is only valid as long as the structure and descriptions stay the same.
  //   */
  //  public String getUid() {
  //      return getUid(false, RuntimeEnvironment.class).toString();
  //  }
  //
  //  /**
  //   * \return Returns the element's uid, which is the concatenated description of it and all parents.
  //   * In case it is part of a remote runtime environment, the uid in the remote environment is returned.
  //   */
  //  public String getOriginalUid() {
  //      return getUid(false, core.Runtime.class).toString();
  //  }
  //
  //  /**
  //   * Optimized helper method for Uid creation
  //   *
  //   * \param includeSeparator Include separator at end?
  //   * \param upToParent Type of parent up to which uid is created
  //   * \return Returns the element's uid, which is the concatenated description of it and all parents.
  //   */
  //  protected StringBuilder getUid(boolean includeSeparator, Class<?> upToParent) {
  //      StringBuilder temp = null;
  //      if (upToParent.isAssignableFrom(getClass())) {
  //          return new StringBuilder();
  //      } else if (parent == null || upToParent.isAssignableFrom(parent.getClass())) {
  //          temp = new StringBuilder(description);
  //      } else {
  //          temp = parent.getUid(true, upToParent).append(description);
  //      }
  //      if (includeSeparator) {
  //          temp.append(getUidSeparator());
  //      }
  //      return temp;
  //  }

  //  /**
  //   * \return Is RuntimeElement member of remote runtime
  //   */
  //  public boolean isRemote() {
  //      if (remote >= 0) {
  //          return remote > 0;
  //      }
  //      if (this instanceof core.Runtime) {
  //          remote = (this instanceof RuntimeEnvironment) ? 0 : 1;
  //          return remote > 0;
  //      }
  //      remote = parent.isRemote() ? 1 : 0;
  //      return remote > 0;
  //  }
  //
  //  /**
  //   * \return Character Sequence that separates the UID after this class;
  //   */
  //  protected char getUidSeparator() {
  //      return '.';
  //  }
  //
  //  /**
  //   * \return Returns true when element is deleted
  //   */
  //  public boolean isDeleted() {
  //      return deleted;
  //  }
  //
  //  /**
  //   * Get all tasks from children... for thread migration... not a very nice way of doing it... but well
  //   */
  //  protected void collectTasks(List<Task> t) {
  //      List<FrameworkElement> l = getChildren();
  //      for (int i = 0; i < l.size(); i++) {
  //          FrameworkElement child = l.get(i);
  //          if (child != null) {
  //              child.collectTasks(t);
  //          }
  //      }
  //  }
  //
  //  /**
  //   * \return Parent
  //   */
  //  public FrameworkElement getParent() {
  //      return parent;
  //  }
  //
  //  /**
  //   * \return Umodifiable list of children. Can and should be used for fast
  //   * safe unsynchronized iteration. May contain null-entries.
  //   */
  //  public List<FrameworkElement> getChildren() {
  //      return children.getFastUnmodifiable();
  //  }
  //
  //  /**
  //   * \param childIndex Index
  //   * \return Child at specified index
  //   */
  //  public FrameworkElement getChildAt(int childIndex) {
  //      return children.get(childIndex);
  //  }
  //
  //  /**
  //   * Get Child with specified UID
  //   * (optimized so that no new Strings need to be created in relative mode)
  //   *
  //   * \param uid UID
  //   * \param b absolute UID? (rather than a relative one)
  //   * \return Child (null if does not exists)
  //   */
  //  public FrameworkElement getChild(String uid, boolean absolute) {
  //      if (absolute) {
  //          String myUid = getUid(true, RuntimeEnvironment.class).toString();
  //          if (!uid.startsWith(myUid)) {
  //              return uid.equals(getUid()) ? this : null;
  //          } else if (uid.length() == myUid.length()) {
  //              return this;
  //          }
  //          uid = uid.substring(myUid.length());  // cut off separator
  //      }
  //
  //      // uid now relative
  //      List<FrameworkElement> l = getChildren();
  //      for (int i = 0; i < l.size(); i++) {
  //          FrameworkElement child = l.get(i);
  //          if (child == null) {
  //              continue;
  //          }
  //          String childDesc = child.getDescription();
  //          if (uid.length() < childDesc.length()) {
  //              continue;
  //          } else if (uid.length() >= childDesc.length()) {
  //              if (uid.startsWith(childDesc)) {
  //                  if (uid.length() == childDesc.length()) {
  //                      return child;
  //                  } else if (uid.charAt(childDesc.length()) == child.getUidSeparator()){
  //                      return child.getChild(uid.substring(childDesc.length() + 1), false);
  //                  }
  //              }
  //          }
  //      }
  //      return null;
  //  }
  //
  //  /**
  //   * Get All children of the specified class
  //   *
  //   * \param childClass Class
  //   * \return List of children
  //   */
  //  public <T extends FrameworkElement> List<T> getAllChildren(Class<T> childClass) {
  //      List<T> result = new ArrayList<T>();
  //      getAllChildrenHelper(result, childClass);
  //      return result;
  //  }
  //
  //  /**
  //   * Recursive helper function to above function
  //   *
  //   * \param result List with results (only needs to be allocated once)
  //   * \param childClass Class
  //   */
  //  @SuppressWarnings("unchecked")
  //  private <T extends FrameworkElement> void getAllChildrenHelper(List<T> result, Class<T> childClass) {
  //      for (int i = 0, n = children.size(); i < n; i++) {
  //          FrameworkElement child = children.get(i);
  //          if (child == null) {
  //              continue;
  //          }
  //          if (childClass == null || childClass.isAssignableFrom(child.getClass())) {
  //              result.add((T)child);
  //          }
  //          child.getAllChildrenHelper(result, childClass);
  //      }
  //  }
  //
  //  /**
  //   * Serialize the runtime's uid to the specified output stream.
  //   * This is very efficient, since no new objects need to be allocated
  //   * to construct the uid
  //   *
  //   * \param oos Stream to serialize to.
  //   */
  //  public void serializeUid(CoreOutputStream oos) throws IOException  {
  //      serializeUid(oos, true);
  //  }
  //
  //  /**
  //   * Helper method for above
  //   *
  //   * \param oos Stream to serialize to.
  //   * \param firstCall Object the method was called on?
  //   */
  //  protected void serializeUid(CoreOutputStream oos, boolean firstCall) throws IOException {
  //      if (parent != null) {
  //          parent.serializeUid(oos, false);
  //      }
  //      oos.write8BitStringPart(description);
  //      if (firstCall) {
  //          oos.write8BitString(""); // end string
  //      } else {
  //          oos.writeByte(getUidSeparator());
  //      }
  //  }
  //
  //  /**
  //   * \return Methods that want to prevent a current port value (that this RuntimeElement manages)
  //   * being outdated and reused while they are
  //   * asynchronously getting it should synchronize with this object.
  //   */
  //  public Object getPortManagerSynchInstance() {
  //      return ownedPorts;
  //  }
  //
  //  /**
  //   * \return Is RuntimeElement responsible for releasing unused port values?
  //   */
  //  public boolean isPortManager() {
  //      return ownedPorts != null;
  //  }
  //
  //  /**
  //   * \param p Port that this RuntimeElement owns
  //   */
  //  protected void addOwnedPort(Port<?> p) {
  //      if (isPortManager()) {
  //          if (p.isOutputPort()) {
  //              p.setMainThread(getMainThread());
  //          }
  //          ownedPorts.add(p);
  //      } else {
  //          parent.addOwnedPort(p);
  //      }
  //  }
  //
  //  /**
  //   * \param p Port that this RuntimeElement does not own any longer
  //   */
  //  protected void removeOwnedPort(Port<?> p) {
  //      if (isPortManager()) {
  //          ownedPorts.remove(p);
  //      } else {
  //          parent.removeOwnedPort(p);
  //      }
  //  }
  //
  //  /**
  //   * \return Main Thread. This thread can write to the runtime element's ports without synchronization
  //   */
  //  public Thread getMainThread() {
  //      return mainThread;
  //  }
  //
  //  /**
  //   * \param Main Thread. This thread can write to the runtime element's ports without synchronization
  //   */
  //  public void setMainThread(Thread mainThread) {
  //      // update main thread of ports
  //      this.mainThread = mainThread;
  //      if (isPortManager()) {
  //          synchronized(ownedPorts) {
  //              for (int i = 0; i < ownedPorts.size(); i++) {
  //                  PortBase<?> p = ownedPorts.get(i);
  //                  if (p != null && p != this) {
  //                      ownedPorts.get(i).setMainThread(mainThread);
  //                  }
  //              }
  //          }
  //      }
  //  }

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
   * Releases all automatically acquired locks
   */
  inline void ReleaseAutoLocks()
  {
    tThreadLocalCache::GetFast()->ReleaseAllLocks();
  }

  /*!
   * \param description New Port description
   * (only valid/possible before, element is initialized)
   */
  void SetDescription(const util::tString& description);

  /*!
   * Mark element as finstructed
   * (should only be called by AdminServer and CreateModuleActions)
   *
   * \param create_action Action with which framework element was created
   * \param params Parameters that module was created with (may be null)
   */
  void SetFinstructed(tCreateModuleAction* create_action, tConstructorParameters* params);

  /*!
   * Called whenever a structure parameter on this framework element changed
   * (can be overridden to handle this event)
   */
  virtual void StructureParametersChanged()
  {
  }

  virtual const util::tString ToString() const
  {
    return GetDescription();
  }

  /*!
   * Write description of link number i to Output stream
   *
   * \param os OutputStream
   * \param i Link Number (0 is primary link/description)
   */
  void WriteDescription(tCoreOutput* os, int i) const;

  // for efficient streaming of fully-qualified framework element name
  void StreamQualifiedName(std::ostream& output) const
  {
    if (!GetFlag(tCoreFlags::cIS_RUNTIME))
    {
      StreamQualifiedParent(output);
    }
    output << GetCDescription();
  }

  void StreamQualifiedParent(std::ostream& output) const
  {
    const tFrameworkElement* parent = GetParent();
    if (parent != NULL && (!parent->GetFlag(tCoreFlags::cIS_RUNTIME)))
    {
      parent->StreamQualifiedParent(output);
      output << parent->GetCDescription();
      output << "/";
    }
  }

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

#endif // CORE__TFRAMEWORKELEMENT_H
