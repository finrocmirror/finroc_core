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
#include "core/datatype/tNumber.h"

#include "core/datatype/tUnit.h"

namespace finroc
{
namespace core
{
int8 tUnit::uid_counter = 0;
std::vector<tUnit*> tUnit::uid_lookup_table_temp;
std::vector<tUnit*> tUnit::unknown;
tUnit tUnit::cNO_UNIT(tUnit::unknown, "", 1);
std::vector<tUnit*> tUnit::length;
tUnit tUnit::nm(tUnit::length, "nm", 0.000000001);
tUnit tUnit::um(tUnit::length, "um", 0.000001);
tUnit tUnit::mm(tUnit::length, "mm", 0.001);
tUnit tUnit::cm(tUnit::length, "cm", 0.01);
tUnit tUnit::dm(tUnit::length, "dm", 0.1);
tUnit tUnit::m(tUnit::length, "m", 1);
tUnit tUnit::km(tUnit::length, "km", 1000);
std::vector<tUnit*> tUnit::speed;
tUnit tUnit::km_h(tUnit::speed, "km/h", 3.6);
tUnit tUnit::m_s(tUnit::speed, "m/s", 1);
std::vector<tUnit*> tUnit::weight;
tUnit tUnit::mg(tUnit::weight, "mg", 0.001);
tUnit tUnit::g(tUnit::weight, "g", 1);
tUnit tUnit::kg(tUnit::weight, "kg", 1000);
tUnit tUnit::t(tUnit::weight, "t", 1000000);
tUnit tUnit::mt(tUnit::weight, "mt", 1000000000000.0);
std::vector<tUnit*> tUnit::time;
tUnit tUnit::ns(tUnit::time, "ns", 0.000000001);
tUnit tUnit::us(tUnit::time, "us", 0.000001);
tUnit tUnit::ms(tUnit::time, "ms", 0.001);
tUnit tUnit::s(tUnit::time, "s", 1);
tUnit tUnit::min(tUnit::time, "min", 60);
tUnit tUnit::h(tUnit::time, "h", 3600);
tUnit tUnit::day(tUnit::time, "day", 86400);
std::vector<tUnit*> tUnit::angle;
tUnit tUnit::deg(tUnit::angle, "deg", 0.017453292);
tUnit tUnit::rad(tUnit::angle, "rad", 1);
std::vector<tUnit*> tUnit::frequency;
tUnit tUnit::Hz(tUnit::frequency, "Hz", 1);
std::vector<tUnit*> tUnit::screen;
tUnit tUnit::Pixel(tUnit::screen, "Pixel", 1);

tUnit::tUnit(std::vector<tUnit*>& group_, const util::tString& description_, double factor_) :
  factor(factor_),
  group(group_),
  description(description_),
  index(group_.size()),
  uid(uid_counter),
  factors(),
  is_aConstant(false)
{
  group_.push_back(this);
  uid_lookup_table_temp.push_back(this);
  uid_counter++;
}

tUnit::tUnit(const util::tString& description_, tUnit* u) :
  factor(u->factor),
  group(u->group),
  description(description_),
  index(u->index),
  uid(u->uid),
  factors(u->factors),
  is_aConstant(true)
{
}

void tUnit::CalculateFactors(std::vector<tUnit*>& units)
{
  for (size_t j = 0u; j < units.size(); j++)
  {
    tUnit* unit = units[j];
    unit->factors.reset(new util::tArrayWrapper<double>(units.size()));
    for (size_t i = 0u; i < units.size(); i++)
    {
      (*(unit->factors))[i] = unit->GetConversionFactor(units[i]);
    }
  }
}

double tUnit::ConvertTo(double value, tUnit* to_unit) const
{
  if (this == &(tUnit::cNO_UNIT) || to_unit == &(tUnit::cNO_UNIT))
  {
    return value;
  }
  return GetConversionFactor(to_unit) * value;
}

double tUnit::GetConversionFactor(tUnit* u) const
{
  if (ConvertibleTo(u))
  {
    return factor / u->factor;
  }
  throw util::tRuntimeException("Units cannot be converted.", CODE_LOCATION_MACRO);
}

tUnit* tUnit::GetUnit(const util::tString& unit_string)
{
  for (auto it = uid_lookup_table_temp.begin(); it != uid_lookup_table_temp.end(); ++it)
  {
    if (boost::equals((*it)->description, unit_string))
    {
      return (*it);
    }
  }
  return &(cNO_UNIT);
}

void tUnit::StaticInit()
{
  CalculateFactors(length);
  CalculateFactors(speed);
  CalculateFactors(weight);
  CalculateFactors(time);
  CalculateFactors(angle);

  // initialize uid lookup table
  //uidLookupTable = uidLookupTableTemp.toArray(new Unit[0]);
}

} // namespace finroc
} // namespace core

