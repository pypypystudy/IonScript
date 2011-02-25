/*******************************************************************************
 * IonScript                                                                   *
 * (c) 2010-2011 Canio Massimo Tristano <massimo.tristano@gmail.com>           *
 *                                                                             *
 * This software is provided 'as-is', without any express or implied           *
 * warranty. In no event will the authors be held liable for any damages       *
 * arising from the use of this software.                                      *
 *                                                                             *
 * Permission is granted to anyone to use this software for any purpose,       *
 * including commercial applications, and to alter it and redistribute it      *
 * freely, subject to the following restrictions:                              *
 *                                                                             *
 * 1. The origin of this software must not be misrepresented; you must not     *
 * claim that you wrote the original software. If you use this software        *
 * in a product, an acknowledgment in the product documentation would be       *
 * appreciated but is not required.                                            *
 *                                                                             *
 * 2. Altered source versions must be plainly marked as such, and must not be  *
 * misrepresented as being the original software.                              *
 *                                                                             *
 * 3. This notice may not be removed or altered from any source                *
 * distribution.                                                               *
 ******************************************************************************/

#ifndef ION_SCRIPT_TYPEDEFS_H
#define	ION_SCRIPT_TYPEDEFS_H

#include <map>
#include <string>
#include <vector>

namespace ionscript {

   const static unsigned int kMagicNumber = 193687;
   const static unsigned int kVersion = 1;

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

#endif	/* ION_SCRIPT_TYPEDEFS_H */

