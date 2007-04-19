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


#include "module.h"

freettcn::CQualifiedName::CQualifiedName(String moduleName, String objectName, void *aux)
{
  _name.moduleName = moduleName;
  _name.objectName = objectName;
  _name.aux = aux;
}

const QualifiedName &freettcn::CQualifiedName::Get() const
{
  return _name;
}



freettcn::CModule::CModule(const CQualifiedName &id) :
  _id(id)
{
}

const TciModuleIdType &freettcn::CModule::Id() const
{
  return _id.Get();
}
