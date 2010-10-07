/***************************************************************************
 *   IonScript                                                             *
 *   Copyright (C) 2010 by Canio Massimo Tristano                          *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   As a special exception, if other files instantiate templates or use   *
 *   macros or inline functions from this file, or you compile this file   *
 *   and link it with other works to produce a work based on this file,    *
 *   this file does not by itself cause the resulting work to be covered   *
 *   by the GNU General Public License. However the source code for this   *
 *   file must still be made available in accordance with the GNU General  *
 *   Public License. This exception does not invalidate any other reasons  *
 *   why a work based on this file might be covered by the GNU General     *
 *   Public License.                                                       *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, see <http://www.gnu.org/licenses/>   *
 ***************************************************************************/

#ifndef ION_SCRIPT_TYPEDEFS_H
#define	ION_SCRIPT_TYPEDEFS_H

#include <map>
#include <string>
#include <vector>

namespace ion {
   namespace script {

      class Value;
      class VirtualMachine;
      class Compiler;
      class FunctionCallManager;
      class SyntaxTree;
      class Parser;
      class Lexer;
      class BytecodeReader;
      class BytecodeWriter;
      struct ValueComp;

      typedef std::vector<Value> List;
      typedef std::map<Value, Value, ValueComp> Dictionary;

      typedef size_t HostFunctionGroupID;
      typedef unsigned char FunctionID;

      struct FunctionInfo {
         HostFunctionGroupID hfgID;
         FunctionID fID;
         int minArgumentsCount;
         int maxArgumentsCount;
      };
      typedef std::map<std::string, FunctionInfo> HostFunctionsMap;
      typedef void (*HostFunction)(const FunctionCallManager&);
   }
}

#endif	/* ION_SCRIPT_TYPEDEFS_H */

