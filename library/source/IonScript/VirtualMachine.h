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

#ifndef ION_SCRIPT_VIRTUAL_MACHINE_H
#define ION_SCRIPT_VIRTUAL_MACHINE_H

#include "Typedefs.h"
#include "Value.h"
#include "FunctionCallManager.h"

#include <iostream>
#include <istream>
#include <stack>
#include <map>
#include <list>

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

         enum State {
            STATE_FINISHED,
            STATE_RUNNING,
            STATE_WAITING_FOR_RETURN,
            STATE_PAUSED,
         };

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
          * @return the actual VirtualMachine state.
          */
         State getState () const {
            return mState;
         }
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
         void setFunction (const std::string& functionName, HostFunctionGroupID hostFunctionGroupID, FunctionID functionID, int minArgumentsCount = 0, int maxArgumentsCount = -2);
         /**
          * Gets a global variable value.
          * @param name the global variable name.
          * @return the global variable value if found.
          * @throw ItemNotFoundException<std::string>
          */
         Value& get (const std::string& name);
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
         void run (char* program);
         /**
          * Compiles given source code and immediately runs it.
          * @param source input source stream containing the source code.
          */
         void compileAndRun (const std::string& filename);
         /**
          * Compiles given source code and immediately runs it.
          * @param source input source stream containing the source code.
          */
         void compileAndRun (std::istream& source);
         /**
          * Pauses the execution of the current script. You can continue execution at any time later by calling goOn().
          */
         void pause();
         /**
          * If the VM is in pause state, this method makes it running again.
          */
         void goOn();
         /**
          * Calls given script functions with given arguments.
          * @remark The VM must already have loaded the bytecode.
          */
         Value callScriptFunction (const Value& function, const Value& argument);
         /**
          * Calls given script functions with given arguments.
          * @remark The VM must already have loaded the bytecode.
          */
         Value callScriptFunction (const Value& function, const Value& argument1, const Value& argument2);
         /**
          * Calls given script functions with given arguments.
          * @remark The VM must already have loaded the bytecode.
          */
         Value callScriptFunction (const Value& function, const Value** arguments, size_t argumentsCount);
         /**
          * Dumps the actual status information about its memory to target output stream.
          * @param output where to print the output.
          */
         void dump (std::ostream& output = std::cout);

      private:
         /** Actual VM state. */
         State mState;
         /** List of registered host function groups. */
         std::vector<HostFunction> mHostFunctionGroups;
         /** Map of registered host-script functions. */
         HostFunctionsMap mHostFunctionsMap;
         /** Map of global variables. */
         std::map<std::string, Value> mGlobalVariables;
         /** The actual program bytecode reader. */
         BytecodeReader* mpProgram;
         /** The stack containing all values. */
         std::vector<Value> mValues;

         /** Convenient data-structure for function calls activation frames management.*/
         struct ActivationRecord {
            index_t returnIndex;
            size_t stackSize;
            location_t firstVariableLocation;
            ActivationRecord() : returnIndex(0), stackSize(0), firstVariableLocation(0) { }
            ActivationRecord(index_t returnIndex, size_t stackSize, location_t firstVariableLocation) :
            returnIndex(returnIndex), stackSize(stackSize), firstVariableLocation(firstVariableLocation) { }
         };
         /** Stack of all the activation frames */
         std::list<ActivationRecord> mActivations;
         /** The number of arguments of the just called host function. NOTE: the VM always calls one HF at a time so there's no possibility for nested HF calls. */
         size_t mHostFunctionArgumentsCount;
         /**
          * Executes a single instruction.
          */
         void executeInstruction ();
         /**
          * Auxiliary function that returns the local value at given location.
          */
         inline Value& getLocalValue (location_t loc) {
            return mValues[mActivations.back().firstVariableLocation + loc];
         }
         /**
          * Throws a RuntimeError with given message.
          * @param message message of the error.
          */
         void error (const std::string& message) const;
         /**
          * This method is called by FunctionCallManager when the user wants to conclude the function call returning a certain value.
          * The value is pushed onto the value stack, the VM is unpaused and the proper number of arguments is removed from the
          * arguments stack.
          * @param value the value to return.
          */
         void returnValue (const Value& value);
         /**
          * The function group containing built-in functions (print, len, append, ...)
          */
         static void builtinsGroup (const FunctionCallManager& manager);
      };
   }
}

#endif // ION_SCRIPT_VIRTUAL_MACHINE_H