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

#ifndef CORE__DATATYPE__TFRAMEWORKELEMENTINFO_H
#define CORE__DATATYPE__TFRAMEWORKELEMENTINFO_H

#include "finroc_core_utils/container/tSimpleList.h"
#include "core/tCoreFlags.h"
#include "core/port/tEdgeAggregator.h"
#include "core/tFrameworkElementTreeFilter.h"

namespace finroc
{
namespace core
{
class tDataType;
class tCoreInput;
class tRemoteTypes;
class tFrameworkElement;
class tCoreOutput;

/*!
 * \author Max Reichardt
 *
 * Framework element information to send to other runtime environments.
 */
class tFrameworkElementInfo : public util::tUncopyableObject
{
public:

  /*!
   * Infos regarding links to this element
   */
  struct tLinkInfo
  {
public:

    /*! name */
    util::tString name;

    /*! parent handle */
    int parent;

    /*! additional flags to store (especially if parent or this is globally unique link) */
    int extra_flags;

  };

private:

  /*! Infos about links to this port - currently in fixed array for efficiency reasons - 4 should be enough */
  tLinkInfo links[4];

  /*! Number of links */
  int8 link_count;

  /*! Handle in remote Runtime */
  int handle;

  /*! Type of port data */
  tDataType* type;

  /*! Port Flags */
  int flags;

  /*! Strategy to use for this port - if it is destination port */
  int16 strategy;

  /*! Minimum network update interval */
  int16 min_net_update_time;

  /*! Stores outgoing connection destination ports - if this is a port */
  util::tSimpleList<int> connections;

  /*! Register Data type */
  //@ConstPtr
  //public final static DataType TYPE = DataTypeRegister.getInstance().getDataType(FrameworkElementInfo.class);

  /*! mask for non-ports and non-edge-aggregators */
  static const int cPARENT_FLAGS_TO_STORE = tCoreFlags::cGLOBALLY_UNIQUE_LINK | tCoreFlags::cALTERNATE_LINK_ROOT | tCoreFlags::cEDGE_AGGREGATOR | tCoreFlags::cFINSTRUCTABLE_GROUP;

  /*! mask for non-ports and non-edge-aggregators */
  static const int cEDGE_AGG_PARENT_FLAGS_TO_STORE = cPARENT_FLAGS_TO_STORE | tEdgeAggregator::cALL_EDGE_AGGREGATOR_FLAGS;

public:

  /*! Op code: ADD CHANGE or DELETE */
  int8 op_code;

private:

  /*!
   * \return OpCode as string
   */
  util::tString GetOpCodeString() const;

  /*!
   * Reset info (for reuse)
   */
  void Reset();

public:

  tFrameworkElementInfo();

  //  /**
  //   * Serialize data type update information
  //   *
  //   * \param dt Data type
  //   * \param tp Packet to serialize to
  //   */
  //  public static void serializeDataType(DataType dt, CoreBuffer tp) {
  //      tp.writeByte(UPDATE_TIME); // write opcode (see base class)
  //      tp.writeInt(-1);
  //      tp.writeShort(dt.getUid());
  //      tp.writeInt(0);
  //      tp.writeInt(1);
  //      tp.writeByte((byte)0);
  //  }

  /*!
   * \param is Input Stream to deserialize from
   * \param type_lookup Remote type information to lookup type
   */
  void Deserialize(tCoreInput* is, tRemoteTypes& type_lookup);

  /*!
   * \param extra_flags all flags
   * \return Flags relevant for a remote parent framework element
   */
  static int FilterParentFlags(int extra_flags);

  /*!
   * Get outgoing connection's destination handles
   *
   * \param copy_to List to copy result of get operation to
   */
  inline void GetConnections(util::tSimpleList<int>& copy_to) const
  {
    copy_to.Clear();
    copy_to.AddAll(connections);
  }

  /*!
   * \return Type of port data
   */
  inline tDataType* GetDataType() const
  {
    return type;
  }

  /*!
   * \return the flags
   */
  inline int GetFlags() const
  {
    return flags;
  }

  //  @Override
  //  public Integer getKey() {
  //      return handle;
  //  }

  //  @Override
  //  public void handleChange(CoreByteBuffer buffer) {
  //      // actually not used... but never mind
  //      buffer.reset();
  //      byte command = buffer.readByte();
  //      int value = buffer.readInt();
  //      if (command == UPDATE_TIME) {
  //          minUpdateInterval = (short)value;
  //      } else if (command == flags) {
  //          flags = value;
  //      }
  //  }

  /*!
   * \return the index
   */
  inline int GetHandle() const
  {
    return handle;
  }

  /*!
   * \param index Link index
   * \return Information about links to this framework element
   */
  inline const tLinkInfo* GetLink(int index) const
  {
    return &(links[index]);
  }

  //
  //  /**
  //   * \return List with links for this port
  //   */
  //  @ConstMethod public @Const @Ref SimpleList<String> getLinks() {
  //      return links;
  //  }

  //  /**
  //   * \return List with links for this port
  //   */
  //  @ConstMethod public @Const @Ref SimpleList<Integer> getParents() {
  //      return parents;
  //  }

  /*!
   * number of parents/links
   */
  inline size_t GetLinkCount() const
  {
    return link_count;
  }

  //  /**
  //   * \param index Index of parent
  //   * \return Handle of parent
  //   */
  //  @ConstMethod public int getParentHandle(int index) {
  //      return (int)(parents.get(index) >> 8);
  //  }
  //
  //  /**
  //   * \param index Index of parent
  //   * \return Handle of parent
  //   */
  //  @ConstMethod public int getParentFlags(int index) {
  //      return links
  //  }

  /*!
   * \return Minimum network update interval
   */
  inline int16 GetMinNetUpdateInterval() const
  {
    return min_net_update_time;
  }

  /*!
   * \return Strategy to use if port is destination port
   */
  inline int16 GetStrategy() const
  {
    return strategy;
  }

  /*!
   * \return Is this information about remote port?
   */
  inline bool IsPort() const
  {
    return (flags & tCoreFlags::cIS_PORT) != 0;
  }

  /*!
   * Serialize framework element information to transaction packet
   *
   * \param fe Framework element to serialize info of
   * \param op_code Typically ADD, CHANGE or DELETE
   * \param tp Packet to serialize to
   * \param element_filter Element filter for client
   * \param tmp Temporary string buffer
   *
   * (call in runtime-registry synchronized context only)
   */
  static void SerializeFrameworkElement(tFrameworkElement* fe, int8 op_code_, tCoreOutput* tp, tFrameworkElementTreeFilter element_filter, util::tStringBuilder& tmp);

  virtual const util::tString ToString() const;

};

} // namespace finroc
} // namespace core

#endif // CORE__DATATYPE__TFRAMEWORKELEMENTINFO_H
