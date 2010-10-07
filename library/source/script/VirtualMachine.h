/*
 * Ion-Engine
 * Copyright Canio Massimo Tristano <massimo.tristano@gmail.com>
 * All rights reserved.
 */

#ifndef ION_VIRTUAL_MACHINE_H
#define ION_VIRTUAL_MACHINE_H

#include "Typedefs.h"
#include "Value.h"
#include "FunctionCallManager.h"

#include <iostream>
#include <istream>
#include <stack>
#include <map>

namespace ion {
   namespace script {

      /**
       * This class provides all the scripting functionalities you need. It compiles source code from any input stream into bytecode which can be
       * executed in later stage. Other main functionalities are the possibility to register new host functions which can be called from the script
       * and post/get global values.
       */
      class VirtualMachine {
         friend class FunctionCallManager;

      public:
         /**
          * Constructs a new Virtual Machine.
          */
         VirtualMachine ();
         /**
          * Deconstructor.
          */
         virtual ~VirtualMachine ();
         /**
          * Makes your host function a function group which new callable script functions can be associated to.
          * @param function your host function that will handle desired function calls.
          * @return the host function group ID necessary to set new functions with setFunction().
          */
         HostFunctionGroupID registerHostFunctionGroup (HostFunction function);
         /**
          * Associates a function name string to a pair composed by the group ID and the actual function ID. After setting a new function, each newly compilated
          * source code will be able to call the new function.
          * @param functionName your new function name. Note that existent functions will be overwritten (so you can define your own "print" function ovverriding the built-in one).
          * @param hostFunctionGroupID  the function group that contains the function to be called.
          * @param functionID an user defined ID that will be passed by argument when ghe host function group is called to identify the very called function.
          * @param minArgumentsCount minimum number of accepted arguments (default is 0).
          * @param maxArgumentsCount maximum number of accepted arguments (default is -2 which means "same as min" therefore strictly min). NOTE: you can specify
          *       the value -1 for no upper limit of arguments.
          *    It's strongly suggested to have an enum for each host function group you may have that contains the IDs of the belonging functions.
          * @remark If maxArgumentsCount is lesser than minArgumentsCount and not equal to -1 it is set equal to minArgumentsCount.
          */
         void setFunction (const std::string& functionName, HostFunctionGroupID hostFunctionGroupID, FunctionID functionID,int minArgumentsCount = 0, int maxArgumentsCount = -2);
         /**
          * Sets a global variable by a new value.
          * @param name the global variable name.
          * @param value the new value to assign.
          */
         template <typename T>
         void post (const std::string& name, const T& value) {
            mGlobalVariables[name] = Value(value);
         }
         /**
          * Sets a global variable by a new value.
          * @param name the global variable name.
          * @param value the new value to assign.
          */
         void post (const std::string& name, const Value& value);
         /**
          * Checks whether the global variable has been defined.
          * @param name the global variable name.
          */
         bool hasGlobalVariable (const std::string& name) const;
         /**
          * Undefines a global variable.
          * @param name the global variable name.
          */
         void undefineVariable (const std::string& name);
         /**
          * Gets a global variable value.
          * @param name the global variable name.
          * @return the global variable value if found.
          * @throw ItemNotFoundException<std::string>
          */
         Value& get (const std::string& name);
         /**
          * Compiles input source code into executable bytecode.
          * @param source input source stream containing the source code.
          * @param output the target vector of bytes targeted to contain the resulting bytecode.
          */
         void compile (std::istream& source, std::vector<char>& output);
         /**
          * Compiles input source code into executable bytecode.
          * @param source input source stream containing the source code.
          * @param output the target vector of bytes targeted to contain the resulting bytecode.
          * @param tree an empty SyntaxTree that will be used for compilation.
          */
         void compile (std::istream& source, std::vector<char>& output, SyntaxTree& tree);
         /**
          * Executes given bytecode.
          * @param program the bytecode to be executed.
          */
         void run (std::vector<char>& program);
         /**
          * Dumps the actual status information about its memory to target output stream.
          * @param output where to print the output.
          */
         void dump (std::ostream& output = std::cout);

      private:
         /** List of registered host function groups. */
         std::vector<HostFunction> mHostFunctionGroups;
         /** Map of registered host-script functions. */
         HostFunctionsMap mHostFunctionsMap;
         /** Map of global variables. */
         std::map<std::string, Value> mGlobalVariables;
         /** The actual program bytecode reader. */
         BytecodeReader* mpProgram;
         /** The stack containing all values. */
         std::vector<Value> mValuesStack;
         /** The temporary stack containing arguments for the up calling function. */
         std::vector<Value> mArgsVector;
         /** The stack containing three indices per call.
          *    1) The return instruction index (instruction pointer).
          *    2) The stack size at call time.
          *    3) Activation frame start offset. Indicates the first local variable (before this index there must be nothing or registers).
          */
         std::vector<size_t> mIndicesStack;
         /** Is the VM running? */
         bool mRunning;

         /**
          * Executes a single instruction.
          */
         void executeInstruction ();
         /**
          * Throws a runtime exception.
          */
         void error (const std::string& message) const;
         /**
          * The function group containing built-in functions (print, len, append, ...)
          */
         static void builtinsGroup (const FunctionCallManager& manager);
      };
   }
}

#endif // ION_VIRTUAL_MACHINE_H