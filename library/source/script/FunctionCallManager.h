/*
 * Ion-Engine
 * Copyright Canio Massimo Tristano <massimo.tristano@gmail.com>
 * All rights reserved.
 */

#ifndef ION_FUNCTIONCALLMANAGER_H
#define	ION_FUNCTIONCALLMANAGER_H

#include "Typedefs.h"

#include <vector>
#include <typeinfo>

namespace ion {
   namespace script {

      /**
       * Presents a convenient way to check and access function call data like the function ID and passed arguments and provides some functionalities to manipulate
       * the VirtualMachine that calls the function.
       */
      class FunctionCallManager {
         friend class VirtualMachine;

      public:
         virtual ~FunctionCallManager ();
         /**
          * @return the actual called function ID.
          */
         inline FunctionID getFunctionID () const {
            return mFunctionID;
         }
         /**
          * @return the number of arguments passed to the function.
          */
         inline size_t getArgumentsCount () const {
            return mArguments.size();
         }
         /**
          * Gets a specific argument from the arguments list.
          * @param index index of the argument (0 is the first).
          * @return the argument.
          */
         inline const Value& getArgument (size_t index) const {
            return mArguments[index];
         }
         /**
          * @return the whole vector of passed arguments.
          */
         inline const std::vector<Value>& getArguments () const {
            return mArguments;
         }
         /**
          * Asserts that a specified argument at position index has an allowed type. It returns silently if the assertion succeeds.
          * @param index index of the argument to check in the vector of passed arguments.
          * @param type the allowed types. You can specify multiple allowed types using the | operator (i.e. Value::TYPE_STRING | VALUE::TYPE_NUMBER).
          * @throw a RuntimeError if assertion fails.
          */
         void assertArgumentType (size_t index, int type) const;
         /**
          * Asserts that:
          *    1) The the specified argument is a TYPE_OBJECT.
          *    2) The object pointed by the value type corresponds to the one given.
          * @param index index of the argument to check in the vector of passed arguments.
          * @param info desired object type. Use typeid(TypeName).
          * @throw a RuntimeError if assertion fails.
          */
         void assertArgumentObjectType (size_t index, const std::type_info& info) const;

         /**
          * The called function returns nil and the execution immediately continues when the host function group returns.
          */
         void returnNil () const;
         /**
          * The called function returns a number and the execution immediately continues when the host function group returns.
          * @param value number to return.
          */
         void returnNumber (double value) const;
         /**
          * The called function returns a boolean value and the execution immediately continues when the host function group returns.
          * @param value boolean value to return.
          */
         void returnBoolean (bool value) const;
         /**
          * The called function returns a string and the execution immediately continues when the host function group returns.
          * @param value string to return.
          */
         void returnString (const std::string& value) const;
         /**
          * The called function returns a certain Value and the execution immediately continues when the host function group returns.
          * @param value Value to return.
          */
         void returnValue (const Value& value) const;

      private:
         /**
          * Only VirtualMachine creates new FunctionCallManager when the script calls a host function.
          */
         FunctionCallManager (VirtualMachine &vm, FunctionID functionID, const std::vector<Value>& arguments);

         VirtualMachine& mVM;
         FunctionID mFunctionID;
         const std::vector<Value> mArguments;
      };

   }
}

#endif	/* ION_FUNCTIONCALLMANAGER_H */

