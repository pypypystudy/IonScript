/*
 * Ion-Engine
 * Copyright Canio Massimo Tristano <massimo.tristano@gmail.com>
 * All rights reserved.
 */

#ifndef ION_SCRIPTTYPEDEFS_H
#define	ION_SCRIPTTYPEDEFS_H

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

#endif	/* ION_SCRIPTTYPEDEFS_H */

