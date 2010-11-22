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

#include "FunctionCallManager.h"

#include "Exceptions.h"
#include "VirtualMachine.h"

#include <sstream>
#include <vector>

using namespace std;
using namespace ion::script;

FunctionCallManager::FunctionCallManager (VirtualMachine& vm, FunctionID functionID, const Value* pArguments, size_t argumentsCount)
: mVM (vm), mFunctionID (functionID), mpArguments (pArguments), mArgumentsCount (argumentsCount) { }

FunctionCallManager::~FunctionCallManager () { }

void FunctionCallManager::assertArgumentType (size_t index, int type) const {
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

void FunctionCallManager::assertArgumentObjectType (size_t index, const std::type_info& info) const {
   assertArgumentType(0, Value::TYPE_OBJECT);
   if (!getArgument(index).checkObjectType(info)) {
      stringstream ss;
      ss << "unexpected object type passed as argument at position " << index << ". Expected was " << info.name() << " while received is " << getArgument(index).getObjectTypeName() << ".";
      throw RuntimeError(ss.str());
   }
}

void FunctionCallManager::returnNil () const {
   mVM.returnValue(Value());
}

void FunctionCallManager::returnBoolean (bool value) const {
   mVM.returnValue(Value(value));
}

void FunctionCallManager::returnNumber (double value) const {
   mVM.returnValue(Value(value));
}

void FunctionCallManager::returnString (const std::string& value) const {
   mVM.returnValue(Value(value));
}

void FunctionCallManager::returnValue (const Value& value) const {
   mVM.returnValue(value);
}
