/**
 * You received this file as part of an advanced experimental
 * robotics framework prototype ('finroc')
 *
 * Copyright (C) 2010-2011 Max Reichardt,
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

#ifndef core__parameter__tStaticParameterBase_h__
#define core__parameter__tStaticParameterBase_h__

#include "rrlib/finroc_core_utils/definitions.h"

#include "rrlib/rtti/tDataTypeBase.h"
#include "core/portdatabase/sSerializationHelper.h"
#include "core/port/std/tPortDataManager.h"
#include "core/port/cc/tCCPortDataManager.h"
#include "rrlib/serialization/tSerializable.h"

namespace rrlib
{
namespace serialization
{
class tInputStream;
} // namespace rrlib
} // namespace serialization

namespace rrlib
{
namespace xml2
{
class tXMLNode;
} // namespace rrlib
} // namespace xml2

namespace finroc
{
namespace core
{
class tFrameworkElement;
class tStaticParameterList;

/*!
 * \author Max Reichardt
 *
 * Static Parameter class
 * (Generic base class without template type)
 */
class tStaticParameterBase : util::tUncopyable
{
  friend class tStaticParameterList;
private:

  /*! Name of parameter */
  util::tString name;

  /*! DataType of parameter */
  rrlib::rtti::tDataTypeBase type;

  /*! Current parameter value (in CreateModuleAction-prototypes this is null) */
  std::unique_ptr<rrlib::rtti::tGenericObject> value;

  /*! Last parameter value (to detect whether value has changed) */
  std::unique_ptr<rrlib::rtti::tGenericObject> last_value;

  /*! Is current value enforced (typically hard-coded)? In this case, any config file entries or command line parameters are ignored */
  bool enforce_current_value;

  /*!
   * StaticParameterBase whose value buffer to use.
   * Typically, this is set to this.
   * However, it is possible to attach this parameter to another (outer) parameter.
   * In this case they share the same buffer: This parameter uses useValueOf.valPointer(), too.
   */
  tStaticParameterBase* use_value_of;

  /** List that this structure parameter is member of */
  tStaticParameterList* parent_list;

  /*! Index in parameter list */
  int list_index;

  /*!
   * Command line option to set this parameter
   * (set by finstructable group containing module with this parameter)
   */
  util::tString command_line_option;

  /*!
   * Name of outer parameter if parameter is configured by static parameter of finstructable group
   * (usually set by finstructable group containing module with this parameter)
   */
  util::tString outer_parameter_attachment;

  /*! Create outer parameter if it does not exist yet? (Otherwise an error message is displayed. Only true, when edited with finstruct.) */
  bool create_outer_parameter;

  /*!
   * Place in Configuration tree, this parameter is configured from (nodes are separated with '/')
   * (usually set by finstructable group containing module with this parameter)
   * (starting with '/' => absolute link - otherwise relative)
   */
  util::tString config_entry;

  /*! Was configEntry set by finstruct? */
  bool config_entry_set_by_finstruct;

  /*! Is this a proxy for other static parameters? (as used in finstructable groups) */
  bool structure_parameter_proxy;

  /*! List of attached parameters */
  util::tSimpleList<tStaticParameterBase*> attached_parameters;

private:

  /*!
   * Create buffer of specified type
   * (and delete old buffer)
   *
   * \param type Type
   */
  void CreateBuffer(rrlib::rtti::tDataTypeBase type_);

  /*!
   * Internal helper method to get parameter containing buffer we are using/sharing.
   *
   * \return Parameter containing buffer we are using/sharing.
   */
  tStaticParameterBase* GetParameterWithBuffer()
  {
    if (use_value_of == this)
    {
      return this;
    }
    return use_value_of->GetParameterWithBuffer();
  }

  /*!
   * Internal helper method to get parameter containing buffer we are using/sharing.
   *
   * \return Parameter containing buffer we are using/sharing.
   */
  const tStaticParameterBase* GetParameterWithBuffer() const
  {
    if (use_value_of == this)
    {
      return this;
    }
    return use_value_of->GetParameterWithBuffer();
  }

  /*!
   * \return Is this a remote parameter?
   */
  inline bool RemoteValue()
  {
    return false;
  }

  /*!
   * Set commandLineOption and configEntry.
   * Check if they changed and possibly load value.
   */
  void UpdateAndPossiblyLoad(const util::tString& command_line_option_tmp, const util::tString& configEntryTmp);

  /*!
   * Check whether change to outerParameterAttachment occured and perform any
   * changes required.
   */
  void UpdateOuterParameterAttachment();

public:

  /*!
   * \param name Name of parameter
   * \param type DataType of parameter
   * \param constructor_prototype Is this a CreteModuleActionPrototype (no buffer will be allocated)
   */
  tStaticParameterBase(const util::tString& name_, rrlib::rtti::tDataTypeBase type_, bool constructor_prototype, bool structure_parameter_proxy = false, const util::tString& config_entry = "");

  virtual ~tStaticParameterBase();

  /*!
   * Attach this static parameter to another one.
   * They will share the same value/buffer.
   *
   * \param other Other parameter to attach this one to. Use null or this to detach.
   */
  void AttachTo(tStaticParameterBase* other);

  /*!
   * (should be overridden by subclasses)
   * \return Deep copy of parameter (without value)
   */
  virtual tStaticParameterBase* DeepCopy()
  {
    throw util::tRuntimeException("Unsupported", CODE_LOCATION_MACRO);
  }

  void Deserialize(rrlib::serialization::tInputStream& is);

  void Deserialize(const rrlib::xml2::tXMLNode& node, bool finstructContext);

  /*!
   * \param result Result buffer for all attached parameters (including those from parameters this parameter is possibly (indirectly) attached to)
   */
  void GetAllAttachedParameters(util::tSimpleList<tStaticParameterBase*>& result);

  /*!
   * \return Place in Configuration tree, this parameter is configured from
   */
  util::tString GetConfigEntry()
  {
    return config_entry;
  }

  const char* GetLogDescription()
  {
    return name.GetCString();
  }

  /*!
   * \return Name of parameter
   */
  inline util::tString GetName()
  {
    return name;
  }

  /*!
   * \return List that this structure parameter is member of
   */
  tStaticParameterList* GetParentList()
  {
    return parent_list;
  }

  /*!
   * \return DataType of parameter
   */
  inline rrlib::rtti::tDataTypeBase GetType()
  {
    return type;
  }

  /*!
   * \return Has parameter changed since last call to "ResetChanged" (or creation).
   */
  bool HasChanged();

  /*!
   * \return Is this a proxy for other static parameters? (only used in finstructable groups)
   */
  bool IsStaticParameterProxy() const
  {
    return structure_parameter_proxy;
  }

  /*!
   * Load value (from any config file entries or command line or whereever)
   */
  void LoadValue();

  /*!
   * Reset "changed flag".
   * The current value will now be the one any new value is compared with when
   * checking whether value has changed.
   */
  void ResetChanged();

  void Serialize(rrlib::serialization::tOutputStream& os) const;

  void Serialize(rrlib::xml2::tXMLNode& node, bool finstruct_context) const;

  /*!
   * \return Value serialized as string (reverse operation to set)
   */
  inline util::tString SerializeValue()
  {
    return sSerializationHelper::TypedStringSerialize(type, ValPointer());
  }

  /*!
   * \param s serialized as string
   */
  virtual void Set(const util::tString& s);

  /*!
   * \param config_entry Place in Configuration tree, this parameter is configured from.
   * Immediately loads this value if parent module is initialized.
   */
  void SetConfigEntry(const util::tString& config_entry);

  /*!
   * \param outer_parameter_attachment Name of outer parameter of finstructable group to configure parameter with.
   * (set by finstructable group containing module with this parameter)
   * \param create_outer Create outer parameter if it does not exist yet?
   */
  void SetOuterParameterAttachment(const util::tString& outer_parameter_attachment, bool create_outer);

  /*!
   * (Internal helper function to make expressions shorter)
   *
   * \return value or ccValue, depending on data type
   */
  inline rrlib::rtti::tGenericObject* ValPointer() const
  {
    return GetParameterWithBuffer()->value.get();
  }

};

} // namespace finroc
} // namespace core

#endif // core__parameter__tStaticParameterBase_h__
