//
// Copyright (C) 2007 Mateusz Pusz
//
// This file is part of freettcn (Free TTCN) library.

// This library is free software; you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation; either version 2.1 of the License, or
// (at your option) any later version.

// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.

// You should have received a copy of the GNU Lesser General Public License
// along with this library; if not, write to the Free Software
// Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA


/**
 * @file   triTimerId.h
 * @author Mateusz Pusz
 * @date   Tue Apr 24 21:05:33 2007
 * 
 * @brief  
 * 
 * 
 */

#ifndef __TRI_TIMER_ID_H__
#define __TRI_TIMER_ID_H__

#include <freettcn/etsi/tri.h>
#include <freettcn/tools/nonAssignable.h>
#include <memory>


namespace freettcn {

  using namespace ORG_ETSI_TTCN3_TRI;

  class CTriTimerId : CNonAssignable, public ORG_ETSI_TTCN3_TRI::TriTimerId {
    static Tinteger _nextId;
    std::shared_ptr<const Tstring> _name;
    const Tinteger _id;
    CTriTimerId(const CTriTimerId &) = default;
  public:
    CTriTimerId(const Tstring &name): _name(new Tstring(name)), _id(_nextId++) {}
    CTriTimerId(CTriTimerId &&) = default;
    ~CTriTimerId() = default;
#ifdef ISSUE_0005949
    TriTimerId *clone() const override                { return new CTriTimerId(*this); }
#else
    TriTimerId *cloneTimerId() override const         { return new CTriTimerId(*this); }
#endif
      
    const Tstring &getTimerName() const override      { return *_name; }
    void setTimerName(const Tstring &pName) override  { _name.reset(new Tstring(pName)); }
    const Tinteger getTId() const override            { return _id; }
      
    Tboolean operator==(const TriTimerId &tmid) const override  { return _id == tmid.getTId(); }
    Tboolean operator<(const TriTimerId &tmid) const override   { return _id < tmid.getTId(); }
  };
    
} // namespace freettcn

#endif /* __TRI_TIMER_ID_H__ */
