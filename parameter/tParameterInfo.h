/**
 * You received this file as part of an advanced experimental
 * robotics framework prototype ('finroc')
 *
 * Copyright (C) 2010 Max Reichardt,
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

#ifndef core__parameter__tParameterInfo_h__
#define core__parameter__tParameterInfo_h__

#include "rrlib/finroc_core_utils/definitions.h"

#include "rrlib/serialization/tDataTypeBase.h"
#include "rrlib/serialization/tOutputStream.h"
#include "rrlib/serialization/tStringOutputStream.h"
#include "core/tFinrocAnnotation.h"

namespace rrlib
{
namespace serialization
{
class tInputStream;
} // namespace rrlib
} // namespace serialization

namespace finroc
{
namespace core
{
/*!
 * \author Max Reichardt
 *
 * Annotates ports that are a parameter
 * and provides respective functionality
 */
class tParameterInfo : public tFinrocAnnotation
{
private:

  /*!
   * Place in Configuration tree, this parameter is configured from (nodes are separated with '/')
   * (starting with '/' => absolute link - otherwise relative)
   */
  util::tString config_entry;

  /*! Was config entry set from finstruct? */
  bool entry_set_from_finstruct;

  /*!
   * Command line option to set this parameter
   * (set by outer-most finstructable group)
   */
  util::tString command_line_option;

  /*!
   * Default value set in finstruct (optional)
   * (set by finstructable group responsible for connecting this parameter to attribute tree)
   */
  util::tString finstruct_default;

public:

  /*! Data Type */
  static rrlib::serialization::tDataTypeBase cTYPE;

  /*! Log domain */
  RRLIB_LOG_CREATE_NAMED_DOMAIN(edge_log, "parameter");

protected:

  virtual void AnnotatedObjectInitialized();

public:

  tParameterInfo();

  virtual void Deserialize(rrlib::serialization::tInputStream& is);

  virtual void Deserialize(const rrlib::xml2::tXMLNode& node);

  void Deserialize(const rrlib::xml2::tXMLNode& node, bool finstruct_context, bool include_commmand_line);

  /*!
   * \return Command line option to set this parameter
   * (set by outer-most finstructable group)
   */
  util::tString GetCommandLineOption()
  {
    return command_line_option;
  }

  /*!
   * \return Place in Configuration tree, this parameter is configured from (nodes are separated with dots)
   */
  inline util::tString GetConfigEntry()
  {
    return config_entry;
  }

  /*!
   * \return Default value set in finstruct (optional)
   * (set by finstructable group responsible for connecting this parameter to attribute tree)
   */
  util::tString GetFinstructDefault()
  {
    return finstruct_default;
  }

  /*!
   * \return Does parameter have any non-default info relevant for finstructed group?
   */
  bool HasNonDefaultFinstructInfo()
  {
    return (config_entry.Length() > 0 && entry_set_from_finstruct) || command_line_option.Length() > 0 || finstruct_default.Length() > 0;
  }

  /*!
   * \return Is config entry set from finstruct/xml?
   */
  bool IsConfigEntrySetFromFinstruct()
  {
    return entry_set_from_finstruct;
  }

  /*!
   * load value from configuration file
   */
  inline void LoadValue()
  {
    LoadValue(false);
  }

  /*!
   * load value from configuration file
   *
   * \param ignore ready flag?
   */
  void LoadValue(bool ignore_ready);

  /*!
   * save value to configuration file
   * (if value equals default value and entry does not exist, no entry is written to file)
   */
  void SaveValue();

  virtual void Serialize(rrlib::serialization::tOutputStream& os) const;

  virtual void Serialize(rrlib::xml2::tXMLNode& node) const;

  void Serialize(rrlib::xml2::tXMLNode& node, bool finstruct_context, bool include_command_line) const;

  /*!
   * \param commandLineOption Command line option to set this parameter
   * (set by outer-most finstructable group)
   */
  void SetCommandLineOption(const util::tString& command_line_option)
  {
    this->command_line_option = command_line_option;
  }

  /*!
   * (loads value from configuration file, if is exists
   *
   * \param config_entry New Place in Configuration tree, this parameter is configured from (nodes are separated with dots)
   * \param finstruct_set Is config entry set from finstruct?
   */
  void SetConfigEntry(const util::tString& config_entry_, bool finstruct_set = false);

  /*!
   * \param finstructDefault Default value set in finstruct.
   * (set by finstructable group responsible for connecting this parameter to attribute tree)
   */
  void SetFinstructDefault(const util::tString& finstruct_default)
  {
    this->finstruct_default = finstruct_default;
  }
};

} // namespace finroc
} // namespace core

#include "rrlib/serialization/tDataType.h"

extern template class ::rrlib::serialization::tDataType<finroc::core::tParameterInfo>;

#endif // core__parameter__tParameterInfo_h__
