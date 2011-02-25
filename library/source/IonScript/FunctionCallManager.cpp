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

#include "FunctionCallManager.h"

#include "Exceptions.h"
#include "VirtualMachine.h"

#include <sstream>
#include <vector>

using namespace std;
using namespace ionscript;

FunctionCallManager::FunctionCallManager(VirtualMachine& vm, FunctionID functionID, const Value* pArguments, size_t argumentsCount)
: mVM(vm), mFunctionID(functionID), mpArguments(pArguments), mArgumentsCount(argumentsCount) { }

FunctionCallManager::~FunctionCallManager() { }

void FunctionCallManager::assertArgumentType(size_t index, int type) const {
   if (!(getArgument(index).getType() & type)) {
      stringstream ss;

      ss << "wrong type of argument at position " << index << ". Its type is " << Value::getTypeName(getArgument(index).getType()) << " while allowed ones are ";

      bool following = false;
      for (int i = 0; i < 7; i++) {
         int t = (1 << i);
         if (type & t) {
            if (following)
               ss << ", ";
            ss << Value::getTypeName((Value::Type)t);
            following = true;
         }
      }

      ss << ".";

      throw RuntimeError(ss.str());
   }
}

void FunctionCallManager::assertArgumentObjectType(size_t index, const std::type_info& info) const {
   assertArgumentType(0, Value::TYPE_OBJECT);
   if (!getArgument(index).checkObjectType(info)) {
      stringstream ss;
      ss << "unexpected object type passed as argument at position " << index << ". Expected was " << info.name() << " while received is " << getArgument(index).getObjectTypeName() << ".";
      throw RuntimeError(ss.str());
   }
}

void FunctionCallManager::returnNil() const {
   mVM.returnValue(Value());
}

void FunctionCallManager::returnBoolean(bool value) const {
   mVM.returnValue(Value(value));
}

void FunctionCallManager::returnNumber(double value) const {
   mVM.returnValue(Value(value));
}

void FunctionCallManager::returnString(const std::string& value) const {
   mVM.returnValue(Value(value));
}

void FunctionCallManager::returnValue(const Value& value) const {
   mVM.returnValue(value);
}
