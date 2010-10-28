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
#include "core/plugin/tConstructorCreateModuleAction.h"

#ifndef CORE__PLUGIN__TCONSTRUCTORCREATEMODULEACTIONIMPL_H
#define CORE__PLUGIN__TCONSTRUCTORCREATEMODULEACTIONIMPL_H

namespace finroc
{
namespace core
{

#define CONSTRUCTORCREATEMODULEACTIONIMPL(param...) \
  class tConstructorCreateModuleActionImpl<MODULE, param> : public tConstructorCreateModuleActionBase<param> { \
  public:\
    typedef tConstructorCreateModuleActionBase<param> base;\
    tConstructorCreateModuleActionImpl(const util::tString& group, const util::tString& type, const util::tString& param_names) : base(group, type, param_names) {}\
    tConstructorCreateModuleActionImpl(const util::tString& group, const util::tString& type, util::tTypedClass<MODULE> mc, const util::tString& param_names) : base(group, type, param_names) {}\
    virtual tFrameworkElement* CreateModule(const util::tString& name, tFrameworkElement* parent, tConstructorParameters* p) const {

template <typename MODULE, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9, typename P10, typename P11, typename P12>
class tConstructorCreateModuleActionImpl : public tConstructorCreateModuleActionBase<P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12>
{
public:
  typedef tConstructorCreateModuleActionBase<P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12> base;
  tConstructorCreateModuleActionImpl(const util::tString& group, const util::tString& type, const util::tString& param_names) : base(group, type, param_names) {}
  tConstructorCreateModuleActionImpl(const util::tString& group, const util::tString& type, util::tTypedClass<MODULE> mc, const util::tString& param_names) : base(group, type, param_names) {}
  virtual tFrameworkElement* CreateModule(const util::tString& name, tFrameworkElement* parent, tConstructorParameters* p) const
  {
    return new MODULE(name, parent, base::GetP1(p), base::GetP2(p), base::GetP3(p), base::GetP4(p), base::GetP5(p), base::GetP6(p), base::GetP7(p), base::GetP8(p), base::GetP9(p), base::GetP10(p), base::GetP11(p), base::GetP12(p));
  }
};

template <typename MODULE, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9, typename P10, typename P11>
CONSTRUCTORCREATEMODULEACTIONIMPL(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, tEmpty)
return new MODULE(name, parent, base::GetP1(p), base::GetP2(p), base::GetP3(p), base::GetP4(p), base::GetP5(p), base::GetP6(p), base::GetP7(p), base::GetP8(p), base::GetP9(p), base::GetP10(p), base::GetP11(p));
}
};

template <typename MODULE, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9, typename P10>
CONSTRUCTORCREATEMODULEACTIONIMPL(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, tEmpty, tEmpty)
return new MODULE(name, parent, base::GetP1(p), base::GetP2(p), base::GetP3(p), base::GetP4(p), base::GetP5(p), base::GetP6(p), base::GetP7(p), base::GetP8(p), base::GetP9(p), base::GetP10(p));
}
};

template <typename MODULE, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9>
CONSTRUCTORCREATEMODULEACTIONIMPL(P1, P2, P3, P4, P5, P6, P7, P8, P9, tEmpty, tEmpty, tEmpty)
return new MODULE(name, parent, base::GetP1(p), base::GetP2(p), base::GetP3(p), base::GetP4(p), base::GetP5(p), base::GetP6(p), base::GetP7(p), base::GetP8(p), base::GetP9(p));
       }
       };

       template <typename MODULE, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8>
       CONSTRUCTORCREATEMODULEACTIONIMPL(P1, P2, P3, P4, P5, P6, P7, P8, tEmpty, tEmpty, tEmpty, tEmpty)
       return new MODULE(name, parent, base::GetP1(p), base::GetP2(p), base::GetP3(p), base::GetP4(p), base::GetP5(p), base::GetP6(p), base::GetP7(p), base::GetP8(p));
              }
              };

              template <typename MODULE, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7>
              CONSTRUCTORCREATEMODULEACTIONIMPL(P1, P2, P3, P4, P5, P6, P7, tEmpty, tEmpty, tEmpty, tEmpty, tEmpty)
              return new MODULE(name, parent, base::GetP1(p), base::GetP2(p), base::GetP3(p), base::GetP4(p), base::GetP5(p), base::GetP6(p), base::GetP7(p));
                     }
                     };

                     template <typename MODULE, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6>
                     CONSTRUCTORCREATEMODULEACTIONIMPL(P1, P2, P3, P4, P5, P6, tEmpty, tEmpty, tEmpty, tEmpty, tEmpty, tEmpty)
                     return new MODULE(name, parent, base::GetP1(p), base::GetP2(p), base::GetP3(p), base::GetP4(p), base::GetP5(p), base::GetP6(p));
                            }
                            };

                            template <typename MODULE, typename P1, typename P2, typename P3, typename P4, typename P5>
                            CONSTRUCTORCREATEMODULEACTIONIMPL(P1, P2, P3, P4, P5, tEmpty, tEmpty, tEmpty, tEmpty, tEmpty, tEmpty, tEmpty)
                            return new MODULE(name, parent, base::GetP1(p), base::GetP2(p), base::GetP3(p), base::GetP4(p), base::GetP5(p));
                                   }
                                   };

                                   template <typename MODULE, typename P1, typename P2, typename P3, typename P4>
                                   CONSTRUCTORCREATEMODULEACTIONIMPL(P1, P2, P3, P4, tEmpty, tEmpty, tEmpty, tEmpty, tEmpty, tEmpty, tEmpty, tEmpty)
                                   return new MODULE(name, parent, base::GetP1(p), base::GetP2(p), base::GetP3(p), base::GetP4(p));
                                          }
                                          };

                                          template <typename MODULE, typename P1, typename P2, typename P3>
                                          CONSTRUCTORCREATEMODULEACTIONIMPL(P1, P2, P3, tEmpty, tEmpty, tEmpty, tEmpty, tEmpty, tEmpty, tEmpty, tEmpty, tEmpty)
                                          return new MODULE(name, parent, base::GetP1(p), base::GetP2(p), base::GetP3(p));
                                                 }
                                                 };

                                                 template <typename MODULE, typename P1, typename P2>
                                                 CONSTRUCTORCREATEMODULEACTIONIMPL(P1, P2, tEmpty, tEmpty, tEmpty, tEmpty, tEmpty, tEmpty, tEmpty, tEmpty, tEmpty, tEmpty)
                                                 return new MODULE(name, parent, base::GetP1(p), base::GetP2(p));
                                                        }
                                                        };

                                                        template <typename MODULE, typename P1>
                                                        CONSTRUCTORCREATEMODULEACTIONIMPL(P1, tEmpty, tEmpty, tEmpty, tEmpty, tEmpty, tEmpty, tEmpty, tEmpty, tEmpty, tEmpty, tEmpty)
                                                        return new MODULE(name, parent, base::GetP1(p));
                                                               }
                                                               };

                                                               template < typename MODULE, typename P1 = tEmpty, typename P2 = tEmpty, typename P3 = tEmpty, typename P4 = tEmpty, typename P5 = tEmpty, typename P6 = tEmpty, typename P7 = tEmpty, typename P8 = tEmpty, typename P9 = tEmpty, typename P10 = tEmpty, typename P11 = tEmpty, typename P12 = tEmpty >
                                                               class tConstructorCreateModuleAction : public tConstructorCreateModuleActionImpl<MODULE, P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12>
{
public:
  typedef tConstructorCreateModuleActionImpl<MODULE, P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12> base;
  \
  tConstructorCreateModuleAction(const util::tString& group, const util::tString& type, const util::tString& param_names) : base(group, type, param_names) {}\
  tConstructorCreateModuleAction(const util::tString& group, const util::tString& type, util::tTypedClass<MODULE> mc, const util::tString& param_names) : base(group, type, param_names) {}\
};

} // namespace finroc
} // namespace core

#endif // CORE__PLUGIN__TCONSTRUCTORCREATEMODULEACTIONIMPL_H
