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
#include "rrlib/finroc_core_utils/tJCBase.h"

#ifndef CORE__PARAMETER__TPARAMETERINFO_H
#define CORE__PARAMETER__TPARAMETERINFO_H

#include "core/buffers/tCoreInput.h"
#include "core/buffers/tCoreOutput.h"
#include "core/tFinrocAnnotation.h"

namespace finroc
{
namespace core
{
class tDataType;

/*!
 * \author Max Reichardt
 *
 * Annotates ports that are a parameter
 * and provides respective functionality
 */
class tParameterInfo : public tFinrocAnnotation
{
private:

  /*! Place in Configuration tree, this parameter is configured from (nodes are separated with dots) */
  util::tString config_entry;

public:

  /*! Data Type */
  static tDataType* cTYPE;

  /*! Log domain */
  RRLIB_LOG_CREATE_NAMED_DOMAIN(edge_log, "parameter");

  tParameterInfo() :
      config_entry()
  {}

  virtual void Deserialize(tCoreInput& is)
  {
    SetConfigEntry(is.ReadString());
  }

  virtual void Deserialize(const util::tString& s)
  {
    SetConfigEntry(s);
  }

  /*!
   * \return Place in Configuration tree, this parameter is configured from (nodes are separated with dots)
   */
  inline util::tString GetConfigEntry()
  {
    return config_entry;
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

  virtual void Serialize(tCoreOutput& os) const
  {
    os.WriteString(config_entry);
  }

  virtual util::tString Serialize() const
  {
    return config_entry;
  }

  /*!
   * (loads value from configuration file, if is exists
   *
   * \param config_entry New Place in Configuration tree, this parameter is configured from (nodes are separated with dots)
   */
  void SetConfigEntry(const util::tString& config_entry_);

};

} // namespace finroc
} // namespace core

#endif // CORE__PARAMETER__TPARAMETERINFO_H
