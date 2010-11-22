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
 
#include "Value.h"
#include "Exceptions.h"

#include <iostream>
#include <cstring>
#include <vector>
#include <typeinfo>
#include <map>

using namespace std;
using namespace ion::script;

Value::Value () : mType (TYPE_NIL) { }

Value::Value (const Value& original) {
   mType = TYPE_NIL;
   *this = original;
}

Value::Value (int value) : mType (TYPE_NUMBER), mNumber (value) { }

Value::Value (double value) : mType (TYPE_NUMBER), mNumber (value) { }

Value::Value (const char* value) : mType (TYPE_STRING), mpReferenceCount (new int(1)), mObjectPointer (new string (value)), mObjectTypeName (typeid (std::string).name ()) { }

Value::Value (const std::string& value) : mType (TYPE_STRING), mpReferenceCount (new int(1)), mObjectPointer (new string (value)), mObjectTypeName (typeid (std::string).name ()) { }

Value::Value (bool value) : mType (TYPE_BOOLEAN), mBoolean (value) { }

Value::~Value () {
   cleanup();
}

void Value::assertType (int type) const {
   if (!(mType & type)) {
      stringstream ss;

      ss << "value type assertion failed: value type is " << getTypeName(mType) << " while allowed ones are ";

      bool following = false;
      for (int i = 0; i < 7; i++) {
         int t = (1 << i);
         if (type & t) {
            if (following)
               ss << ", ";
            ss << getTypeName((Type) t);
            following = true;
         }
      }

      ss << ".";

      throw RuntimeError(ss.str());
   }
}

void Value::setNil () {
   cleanup();
   mType = TYPE_NIL;
}

void Value::setFunctionValue (index_t functionIndex, unsigned char nArguments, unsigned char nRegisters) {
   cleanup();
   mType = TYPE_SCRIPT_FUNCTION;
   this->mFunctionIndex = functionIndex;
   this->mnFunctionRegisters = nRegisters;
   this->mnArguments = nArguments;
}

List& Value::setEmptyList () {
   List* pTree = new List();
   setList(pTree);
   return *pTree;
}

void Value::setList (List* pList) {
   cleanup();
   mType = TYPE_LIST;
   mpReferenceCount = new int(1);
   mObjectPointer = pList;
   mObjectTypeName = typeid (List).name();
}

Dictionary& Value::setEmptyDictionary () {
   Dictionary* pDictionary = new Dictionary();
   setDictionary(pDictionary);
   return *pDictionary;
}

void Value::setDictionary (Dictionary* pDictionary) {
   cleanup();
   mType = TYPE_DICTIONARY;
   mpReferenceCount = new int(1);
   mObjectPointer = pDictionary;
   mObjectTypeName = typeid (Dictionary).name();
}

Value& Value::getDictionaryElement (const Value& value) const {
   Dictionary::iterator it = reinterpret_cast<Dictionary*> (mObjectPointer)->find(value);
   if (it == reinterpret_cast<Dictionary*> (mObjectPointer)->end())
      throw RuntimeError("key error, " + value.toString() + ".");
   else
      return it->second;
}

bool Value::toBoolean () const {
   switch (mType) {
      case TYPE_NIL:
         return false;

      case TYPE_BOOLEAN:
         return mBoolean;

      case TYPE_NUMBER:
         return mNumber != 0.0;

      case TYPE_STRING:
         return getString() != "";

      case TYPE_SCRIPT_FUNCTION:
         return true;

      case TYPE_OBJECT:
         return mObjectPointer != 0;

      case TYPE_LIST:
         return getList().size() > 0;

      case TYPE_DICTIONARY:
         return getDictionary().size() > 0;
   }
   return false;
}

std::string Value::toString () const {
   stringstream ss;
   switch (getType()) {
      case Value::TYPE_NIL:
         return "nil";
      case Value::TYPE_NUMBER:
         ss << getNumber();
         break;
      case Value::TYPE_STRING:
         ss << getString();
         break;
      case Value::TYPE_SCRIPT_FUNCTION:
         ss << "<function at " << mFunctionIndex << '>';
         break;
      case Value::TYPE_BOOLEAN:
         ss << ((getBoolean()) ? "true" : "false");
         break;
      case Value::TYPE_OBJECT:
         ss << "<" << ((mpReferenceCount != 0) ? "managed " : "") << "object " << mObjectTypeName << " at " << mObjectPointer << ">";
         break;

      case Value::TYPE_LIST:
         ss << '[';
         for (size_t i = 0; i < getList().size(); i++) {
            if (getList()[i].isString())
               ss << '"' << getList()[i].toString() << '"';
            else
               ss << getList()[i].toString();
            if (i != getList().size() - 1)
               ss << ", ";
         }
         ss << ']';
         break;

      case Value::TYPE_DICTIONARY:
      {
         ss << '{';
         Dictionary::const_iterator it;
         size_t i = 0;
         bool keyIsString, valueIsString;
         for (it = getDictionary().begin(); it != getDictionary().end(); it++) {
            keyIsString = it->first.isString();
            valueIsString = it->second.isString();
            ss << ((keyIsString) ? "\"" : "") << it->first.toString() << ((keyIsString) ? "\"" : "") << ":" << ((valueIsString) ? "\"" : "") << it->second.toString() << ((valueIsString) ? "\"" : "") << " ";
            if (i != getDictionary().size() - 1)
               ss << ", ";
            i++;
         }
         ss << '}';
         break;
      }

      default:
         ss << '-';
         break;
   }
   return ss.str();
}

Value & Value::operator= (const Value& original) {
   cleanup();
   mType = original.mType;

   switch (mType) {
      case TYPE_NIL:
         break;

      case TYPE_BOOLEAN:
         mBoolean = original.mBoolean;
         break;


      case TYPE_NUMBER:
         mNumber = original.mNumber;
         break;

      case TYPE_SCRIPT_FUNCTION:
         mFunctionIndex = original.mFunctionIndex;
         mnFunctionRegisters = original.mnFunctionRegisters;
         mnArguments = original.mnArguments;
         break;

      case TYPE_OBJECT:
      case TYPE_STRING:
      case TYPE_LIST:
      case TYPE_DICTIONARY:
         mObjectPointer = original.mObjectPointer;
         mObjectTypeName = original.mObjectTypeName;
         mpReferenceCount = original.mpReferenceCount;
         if (mpReferenceCount)
            (*mpReferenceCount)++;
         break;

      default:
         memcpy(&mNumber, &original.mNumber, sizeof (double));
         break;
   }
   return *this;
}

Value & Value::operator= (int original) {
   cleanup();
   mType = TYPE_NUMBER;
   mNumber = original;
   return *this;
}

Value & Value::operator= (double original) {
   cleanup();
   mType = TYPE_NUMBER;
   mNumber = original;
   return *this;
}

Value & Value::operator= (const std::string & original) {
   cleanup();
   mType = TYPE_STRING;
   mpReferenceCount = new int(1);
   mObjectPointer = new string(original);
   return *this;
}

Value & Value::operator= (bool original) {
   cleanup();
   mType = TYPE_BOOLEAN;
   mBoolean = original;
   return *this;
}

Value Value::operator+ (const Value & right) {

   if (right.mType == mType) {
      switch (mType) {
         case TYPE_NUMBER:
            return Value(mNumber + right.mNumber);

         case TYPE_STRING:
            return Value(getString() + right.getString());

         case TYPE_LIST:
         {
            Value v;
            v.setEmptyList();
            v.getList().reserve(getList().size() + right.getList().size());
            v.getList().insert(v.getList().end(), getList().begin(), getList().end());
            v.getList().insert(v.getList().end(), right.getList().begin(), right.getList().end());
            return v;
         }

         default:
            break;
      }
   }
   throwOperationError("sum", mType, right.mType);
   return *this; // it never arrives here
}

Value Value::operator- (const Value & right) {
   if (mType == TYPE_NUMBER && right.mType == TYPE_NUMBER)
      return Value(mNumber - right.mNumber);

   throwOperationError("subtract", mType, right.mType);
   return *this; // it never arrives here
}

Value Value::operator* (const Value & right) {
   if (mType == TYPE_NUMBER && right.mType == TYPE_NUMBER)
      return Value(mNumber * right.mNumber);

   else if (mType == TYPE_STRING && right.mType == TYPE_NUMBER) {
      if (!right.isInteger() || right.mNumber < 0)
         throw RuntimeError("multiplier number must be a positive integer.");
      else {
         string temp = "";
         for (size_t i = 0; i < (size_t) right.mNumber; ++i)
            temp += getString();
         return Value(temp);
      }

   } else if (mType == TYPE_LIST && right.mType == TYPE_NUMBER) {
      if (!right.isInteger() || right.mNumber < 0)
         throw RuntimeError("multiplier number must be a positive integer.");
      else {
         Value v;
         List& l = v.setEmptyList();
         l.reserve(getList().size() * right.mNumber);

         for (size_t i = 0; i < (size_t) right.mNumber; ++i)
            l.insert(l.end(), getList().begin(), getList().end());

         return v;
      }
   }

   throwOperationError("multiply", mType, right.mType);
   return *this; // it never arrives here
}

Value Value::operator/ (const Value & right) {
   if (mType == TYPE_NUMBER && right.mType == TYPE_NUMBER)
      return Value(mNumber / right.mNumber);
   throwOperationError("divide", mType, right.mType);
   return *this; // it never arrives here
}

bool Value::operator ! () const {
   return !toBoolean();
}

bool Value::operator && (const Value& right) const {
   return toBoolean() && right.toBoolean();
}

bool Value::operator || (const Value& right) const {
   return toBoolean() || right.toBoolean();
}

bool Value::operator== (const Value& right) const {
   if (mType != right.mType)
      return false;

   switch (mType) {
      case TYPE_NIL:
         return true;

      case TYPE_BOOLEAN:
         if (mBoolean == right.mBoolean)
            return true;
         else
            return false;

      case TYPE_NUMBER:
         if (mNumber == right.mNumber)
            return true;
         else
            return false;

      case TYPE_STRING:
         if (getString() == right.getString())
            return true;
         else
            return false;

      case TYPE_SCRIPT_FUNCTION:
         if (mFunctionIndex == right.mFunctionIndex)
            return true;
         else
            return false;

      case TYPE_OBJECT:
         return mObjectPointer == right.mObjectPointer;

      case TYPE_LIST:
         if (getList().size() != right.getList().size())
            return false;
         for (size_t i = 0; i < getList().size(); i++)
            if (getList()[i] != right.getList()[i])
               return false;
         return true;

      case TYPE_DICTIONARY:
         return getDictionary() == right.getDictionary();

      default:
         return false;
   }
}

bool Value::operator != (const Value& right) const {
   return !(*this == right);
}

bool Value::operator< (const Value& right) const {
   if (mType != right.mType)
      throwOperationError("compare disequality of", mType, right.mType);

   switch (mType) {
      case TYPE_NUMBER:
         return mNumber < right.mNumber;
      case TYPE_STRING:
         return getString() < right.getString();
      default:
         throwOperationError("compare disequality of", mType, right.mType);
   }

   return false;
}

bool Value::operator> (const Value& right) const {
   if (mType != right.mType)
      throwOperationError("compare disequality of", mType, right.mType);

   switch (mType) {
      case TYPE_NUMBER:
         return mNumber > right.mNumber;
      case TYPE_STRING:
         return getString() > right.getString();
      default:
         throwOperationError("compare disequality of", mType, right.mType);
   }

   return false;
}

bool Value::operator<= (const Value& right) const {
   if (mType != right.mType)
      throwOperationError("compare disequality of", mType, right.mType);

   switch (mType) {
      case TYPE_NUMBER:
         return mNumber <= right.mNumber;
      case TYPE_STRING:
         return getString() <= right.getString();
      default:
         throwOperationError("compare disequality of", mType, right.mType);
   }

   return false;
}

bool Value::operator>= (const Value& right) const {
   if (mType != right.mType)
      throwOperationError("compare disequality of", mType, right.mType);

   switch (mType) {
      case TYPE_NUMBER:
         return mNumber >= right.mNumber;
      case TYPE_STRING:
         return getString() >= right.getString();
      default:
         throwOperationError("compare disequality of", mType, right.mType);
   }

   return false;
}

const std::string& Value::getTypeName (Value::Type type) {
   static string nil = "Nil";
   static string boolean = "Boolean";
   static string number = "Number";
   static string stringstr = "String";
   static string function = "Script-Function";
   static string object = "Object";
   static string list = "List";
   static string dictionary = "Dictionary";
   static string invalid = "** invalid type **";

   switch (type) {
      case TYPE_NIL: return nil;
      case TYPE_BOOLEAN: return boolean;
      case TYPE_NUMBER: return number;
      case TYPE_STRING: return stringstr;
      case TYPE_SCRIPT_FUNCTION: return function;
      case TYPE_OBJECT: return object;
      case TYPE_LIST: return list;
      case TYPE_DICTIONARY: return dictionary;
   }
   return invalid;
}

//

void Value::cleanup () {
   if (mType == TYPE_STRING || mType == TYPE_LIST || mType == TYPE_DICTIONARY || (mType == TYPE_OBJECT && mpReferenceCount != 0)) {
      --(*mpReferenceCount);
      if (*mpReferenceCount <= 0) {
         delete mpReferenceCount;
         switch (mType) {
            case TYPE_STRING:
               delete reinterpret_cast<std::string*> (mObjectPointer);
               return;
            case TYPE_OBJECT:
               delete reinterpret_cast<IManageableObject*> (mObjectPointer);
               return;
            case TYPE_LIST:
               delete reinterpret_cast<List*> (mObjectPointer);
               return;
            case TYPE_DICTIONARY:
               delete reinterpret_cast<Dictionary*> (mObjectPointer);
               return;
            default:
               return;
         }
      }
   }
}

void Value::throwOperationError (const std::string& operation, Type firstValueType, Type secondValueType) const throw (RuntimeError) {
   throw RuntimeError("cannot " + operation + " a " + getTypeName(firstValueType) + " with a " + getTypeName(secondValueType) + ".");
}