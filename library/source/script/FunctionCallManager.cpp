/*
 * Ion-Engine
 * Copyright Canio Massimo Tristano <massimo.tristano@gmail.com>
 * All rights reserved.
 */

#include "FunctionCallManager.h"

#include "Exceptions.h"
#include "VirtualMachine.h"

#include <sstream>
#include <vector>

using namespace std;
using namespace ion::script;

FunctionCallManager::FunctionCallManager (VirtualMachine& vm, FunctionID functionCallID, const std::vector<Value>& arguments)
: mVM (vm), mFunctionID (functionCallID), mArguments (arguments) { }

FunctionCallManager::~FunctionCallManager () { }

void FunctionCallManager::assertArgumentType (size_t index, int type) const {
   if (!(mArguments[index].getType() & type)) {
      stringstream ss;

      ss << "wrong type of argument at position " << index << ". Its type is " << Value::getTypeName(mArguments[index].getType()) << " while allowed ones are ";

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
   if (!mArguments[index].checkObjectType(info)) {
      stringstream ss;
      ss << "unexpected object type passed as argument at position " << index << ". Expected was " << info.name() << " while received is " << mArguments[index].getObjectTypeName() << ".";
      throw RuntimeError(ss.str());
   }
}

void FunctionCallManager::returnNil () const {
   mVM.mValuesStack.push_back(Value());
   mVM.mRunning = true;
}

void FunctionCallManager::returnBoolean (bool value) const {
   mVM.mValuesStack.push_back(Value(value));
   mVM.mRunning = true;
}

void FunctionCallManager::returnNumber (double value) const {
   mVM.mValuesStack.push_back(Value(value));
   mVM.mRunning = true;
}

void FunctionCallManager::returnString (const std::string& value) const {
   mVM.mValuesStack.push_back(Value(value));
   mVM.mRunning = true;
}

void FunctionCallManager::returnValue (const Value& value) const {
   mVM.mValuesStack.push_back(value);
   mVM.mRunning = true;
}