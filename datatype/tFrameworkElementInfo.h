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

#ifndef core__datatype__tFrameworkElementInfo_h__
#define core__datatype__tFrameworkElementInfo_h__

#include "rrlib/finroc_core_utils/definitions.h"
#include "rrlib/rtti/rtti.h"

#include "core/tRuntimeListener.h"
#include "core/tCoreFlags.h"
#include "core/port/tEdgeAggregator.h"

namespace finroc
{
namespace core
{
class tRemoteTypes;
class tFrameworkElement;

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
    /*! name */
    util::tString name;

    /*! parent handle */
    int parent;

    /*! additional flags to store (especially if parent or this is globally unique link) */
    uint extra_flags;
  };

  /*!
   * Infos regarding edges emerging from this element
   */
  struct tConnectionInfo
  {
    /*! Handle of destination port */
    int handle;

    /*! Was this edge finstructed? */
    bool finstructed;

    tConnectionInfo(int handle, bool finstructed) :
      handle(handle),
      finstructed(finstructed)
    {}
  };

private:

  /*! Infos about links to this port - currently in fixed array for efficiency reasons - 4 should be enough */
  tLinkInfo links[4];

  /*! Number of links */
  int8 link_count;

  /*! Handle in remote Runtime */
  int handle;

  /*! Type of port data */
  rrlib::rtti::tDataTypeBase type;

  /*! Port Flags */
  uint flags;

  /*! Strategy to use for this port - if it is destination port */
  int16 strategy;

  /*! Minimum network update interval */
  int16 min_net_update_time;

  /*! Stores outgoing connection destination ports - if this is a port */
  std::vector<tConnectionInfo> connections;

  /*! mask for non-ports and non-edge-aggregators */
  static const uint cPARENT_FLAGS_TO_STORE = tCoreFlags::cGLOBALLY_UNIQUE_LINK | tCoreFlags::cALTERNATE_LINK_ROOT | tCoreFlags::cEDGE_AGGREGATOR;

  /*! mask for non-ports and non-edge-aggregators */
  static const uint cEDGE_AGG_PARENT_FLAGS_TO_STORE = cPARENT_FLAGS_TO_STORE | tEdgeAggregator::cALL_EDGE_AGGREGATOR_FLAGS;

public:

  /*! EDGE_CHANGE Opcode */
  static const int8 cEDGE_CHANGE = tRuntimeListener::cREMOVE + 1;

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

  /*!
   * \param is Input Stream to deserialize from
   * \param type_lookup Remote type information to lookup type
   */
  void Deserialize(rrlib::serialization::tInputStream& is, tRemoteTypes& type_lookup);

  /*!
   * \param extra_flags all flags
   * \return Flags relevant for a remote parent framework element
   */
  static uint FilterParentFlags(uint extra_flags);

  /*!
   * Get outgoing connection's destination handles etc.
   *
   * \param copy_to List to copy result of get operation to
   */
  inline void GetConnections(std::vector<tConnectionInfo>& copy_to) const
  {
    copy_to = connections;
  }

  /*!
   * \return Type of port data
   */
  inline rrlib::rtti::tDataTypeBase GetDataType() const
  {
    return type;
  }

  /*!
   * \return the flags
   */
  inline uint GetFlags() const
  {
    return flags;
  }

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

  /*!
   * number of parents/links
   */
  inline size_t GetLinkCount() const
  {
    return link_count;
  }

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
   * \param stream Stream to serialize to
   * \param serialize_hierarchy Serialize hierarchy? (parents etc., otherwise elements will be serialized 'flat' with qualified names - typical for port-only filters)
   * \param serialize_connections Serialize connection info? (outgoing edges for every port)
   * \param send_tags Send framework element tags?
   * \param tmp Temporary string buffer
   * \param element_filter Element filtering function (only relevant if hierarchy is serialized)
   *
   * (call in runtime-registry synchronized context only)
   */
  static void SerializeFrameworkElement(tFrameworkElement& fe, int8 op_code, rrlib::serialization::tOutputStream& stream, bool serialize_hierarchy, bool serialize_connections, bool send_tags, std::string& tmp, const std::function<bool (const tFrameworkElement&)>& element_filter = tFrameworkElement::tIteratorFilterNone::Accept);

  virtual const util::tString ToString() const;

};

} // namespace finroc
} // namespace core

#endif // core__datatype__tFrameworkElementInfo_h__
