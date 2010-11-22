/***************************************************************************
 * IonScript *
 * Copyright (C) 2010 by Canio Massimo Tristano *
 * *
 * This program is free software; you can redistribute it and/or modify *
 * it under the terms of the GNU General Public License as published by *
 * the Free Software Foundation; either version 2 of the License, or *
 * (at your option) any later version. *
 * *
 * This program is distributed in the hope that it will be useful, *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the *
 * GNU General Public License for more details. *
 * *
 * As a special exception, if other files instantiate templates or use *
 * macros or inline functions from this file, or you compile this file *
 * and link it with other works to produce a work based on this file, *
 * this file does not by itself cause the resulting work to be covered *
 * by the GNU General Public License. However the source code for this *
 * file must still be made available in accordance with the GNU General *
 * Public License. This exception does not invalidate any other reasons *
 * why a work based on this file might be covered by the GNU General *
 * Public License. *
 * *
 * You should have received a copy of the GNU General Public License *
 * along with this program; if not, see <http://www.gnu.org/licenses/> *
 ***************************************************************************/

#ifndef ION_SCRIPT_FUNCTIONCALLMANAGER_H
#define	ION_SCRIPT_FUNCTIONCALLMANAGER_H

#include "Typedefs.h"
#include "Value.h"
#include "VirtualMachine.h"

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
            return mArgumentsCount;
         }
         /**
          * Gets a specific argument from the arguments list.
          * @param index index of the argument (0 is the first).
          * @return the argument.
          * @remark the given index is not checked to be within the arguments range.
          */
         inline const Value& getArgument (size_t index) const {
            return mpArguments[index];
         }
         /**
          * @return a reference to the VM that made this call.
          */
         inline VirtualMachine& getVM () const {
            return mVM;
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
         FunctionCallManager (VirtualMachine &vm, FunctionID functionID, const Value* pArguments, size_t argumentsCount);

         VirtualMachine& mVM;
         FunctionID mFunctionID;
         const Value* mpArguments;
         size_t mArgumentsCount;
      };

   }
}

#endif	/* ION_SCRIPT_FUNCTIONCALLMANAGER_H */

