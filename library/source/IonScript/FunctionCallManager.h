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

#ifndef ION_SCRIPT_FUNCTIONCALLMANAGER_H
#define	ION_SCRIPT_FUNCTIONCALLMANAGER_H

#include "Typedefs.h"
#include "Value.h"
#include "VirtualMachine.h"

#include <vector>
#include <typeinfo>

namespace ionscript {

   /**
    * Presents a convenient way to check and access function call data like the function ID and passed arguments and provides some functionalities to manipulate
    * the VirtualMachine that calls the function.
    */
   class FunctionCallManager {
      friend class VirtualMachine;

   public:
      virtual ~FunctionCallManager();
      /**
       * @return the actual called function ID.
       */
      inline FunctionID getFunctionID() const {
         return mFunctionID;
      }
      /**
       * @return the number of arguments passed to the function.
       */
      inline size_t getArgumentsCount() const {
         return mArgumentsCount;
      }
      /**
       * Gets a specific argument from the arguments list.
       * @param index index of the argument (0 is the first).
       * @return the argument.
       * @remark the given index is not checked to be within the arguments range.
       */
      inline const Value& getArgument(size_t index) const {
         return mpArguments[index];
      }
      /**
       * @return a reference to the VM that made this call.
       */
      inline VirtualMachine& getVM() const {
         return mVM;
      }
      /**
       * Asserts that a specified argument at position index has an allowed type. It returns silently if the assertion succeeds.
       * @param index index of the argument to check in the vector of passed arguments.
       * @param type the allowed types. You can specify multiple allowed types using the | operator (i.e. Value::TYPE_STRING | VALUE::TYPE_NUMBER).
       * @throw a RuntimeError if assertion fails.
       */
      void assertArgumentType(size_t index, int type) const;
      /**
       * Asserts that:
       *    1) The the specified argument is a TYPE_OBJECT.
       *    2) The object pointed by the value type corresponds to the one given.
       * @param index index of the argument to check in the vector of passed arguments.
       * @param info desired object type. Use typeid(TypeName).
       * @throw a RuntimeError if assertion fails.
       */
      void assertArgumentObjectType(size_t index, const std::type_info& info) const;
      /**
       * The called function returns nil and the execution immediately continues when the host function group returns.
       */
      void returnNil() const;
      /**
       * The called function returns a number and the execution immediately continues when the host function group returns.
       * @param value number to return.
       */
      void returnNumber(double value) const;
      /**
       * The called function returns a boolean value and the execution immediately continues when the host function group returns.
       * @param value boolean value to return.
       */
      void returnBoolean(bool value) const;
      /**
       * The called function returns a string and the execution immediately continues when the host function group returns.
       * @param value string to return.
       */
      void returnString(const std::string& value) const;
      /**
       * The called function returns an object.
       * @param object reference to the object to be returned.
       * @param managed whether it should be managed or not.
       */
      template <typename T>
      void returnObject(T* pObject, bool managed = false) const {
         returnValue(Value(pObject, managed));
      }
      /**
       * The called function returns a certain Value and the execution immediately continues when the host function group returns.
       * @param value Value to return.
       */
      void returnValue(const Value& value) const;

   private:
      /**
       * Only VirtualMachine creates new FunctionCallManager when the script calls a host function.
       */
      FunctionCallManager(VirtualMachine &vm, FunctionID functionID, const Value* pArguments, size_t argumentsCount);

      VirtualMachine& mVM;
      FunctionID mFunctionID;
      const Value* mpArguments;
      size_t mArgumentsCount;
   };

}

#endif	/* ION_SCRIPT_FUNCTIONCALLMANAGER_H */

