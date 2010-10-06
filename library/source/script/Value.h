/*
 * Ion-Engine
 * Copyright Canio Massimo Tristano <massimo.tristano@gmail.com>
 * All rights reserved.
 */

#ifndef ION_VALUE_H
#define	ION_VALUE_H

#include "Exceptions.h"
#include "Typedefs.h"
#include "OpCode.h"

#include <string>
#include <ostream>
#include <typeinfo>
#include <cstring>
#include <vector>

namespace ion {
   namespace script {

      /**
       * All managed objects must implement this very trivial interface since C++ does not to delete from void pointers.
       */
      class IManageableObject {
      public:
         virtual ~IManageableObject () { }
      };

      /**
       * This class represents the dynamic type script variables have. It automatically adapts to incoming types and manages memory automatically.
       */
      class Value {
         friend class VirtualMachine;

      public:

         /**
          * The eight types a value can be.
          */
         enum Type {
            TYPE_NIL = 1,
            TYPE_BOOLEAN = 2,
            TYPE_NUMBER = 4,
            TYPE_STRING = 8,
            TYPE_SCRIPT_FUNCTION = 16,
            TYPE_OBJECT = 32,
            TYPE_LIST = 64,
            TYPE_DICTIONARY = 128
         };

         /**
          * Constructs an empty value initially set to nil.
          */
         Value ();
         /**
          * Copies the original value into this one.
          * @param original the original value.
          */
         Value (const Value & original);
         /**
          * Creates a new Value from a const char* string.
          */
         explicit Value (const char * value);
         /**
          * Creates a new Value from a string.
          */
         explicit Value (const std::string & value);
         /**
          * Creates a new Value from an integer.
          */
         explicit Value (int value);
         /**
          * Creates a new Value from a double.
          */
         explicit Value (double value);
         /**
          * Creates a new Value from a boolean.
          */
         explicit Value (bool value);
         /**
          * Creates a new Value of type TYPE_OBJECT that embeds a user defined object.
          * @param pObject a pointer to the object.
          * @param managed true if you want the Value to automatically manage object deletion when no values are referencing the object, false if you want to handle
          *    the deletion manually or you simply don't want the script engine to delete the object as it is owned by some other user object.
          */
         template <typename T>
         explicit Value (T* pObject, bool managed = false) {
            mType = TYPE_OBJECT;
            if (managed)
               mpReferenceCount = new int(1);
            else
               mpReferenceCount = 0;

            mObjectPointer = (void*) pObject;
            mObjectTypeName = typeid (T).name();
         }
         /**
          * Deconstructor.
          */
         ~Value ();
         /**
          * @return true if this Value is nil (TYPE_NIL).
          */
         inline bool isNil () const {
            return mType == TYPE_NIL;
         }
         /**
          * @return true if this Value is a boolean (TYPE_BOOLEAN).
          */
         inline bool isBoolean () const {
            return mType == TYPE_BOOLEAN;
         }
         /**
          * @return true if this Value is a number (TYPE_NUMBER).
          */
         inline bool isNumber () const {
            return mType == TYPE_NUMBER;
         }
         /**
          * @return true if this Value is a string (TYPE_STRING).
          */
         inline bool isString () const {
            return mType == TYPE_STRING;
         }
         /**
          * @return true if this Value is a script function (TYPE_SCRIPT_FUNCTION).
          */
         inline bool isScriptFunction () const {
            return mType == TYPE_SCRIPT_FUNCTION;
         }
         /**
          * @return true if this Value is an user object (TYPE_OBJECT).
          */
         inline bool isObject () const {
            return mType == TYPE_OBJECT;
         }
         /**
          * @return true if this Value is a user object (TYPE_OBJECT) and it is managed.
          */
         inline bool isManagedObject () const {
            return mType == TYPE_OBJECT && mpReferenceCount != 0;
         }
         /**
          * @return true if this Value is an integer number.
          */
         inline bool isInteger () const {
            return mType == TYPE_NUMBER && ((int) mNumber) == mNumber;
         }
         /**
          * @return true if this Value is a positive integer number.
          */
         inline bool isPositiveInteger () const {
            return isInteger() && mNumber >= 0;
         }
         /**
          * @return true if this Value is a list (TYPE_LIST).
          */
         inline bool isList () const {
            return mType == TYPE_LIST;
         }
         /**
          * @return true if this Value is dictionary (TYPE_LIST).
          */
         inline bool isDictonary () const {
            return mType == TYPE_DICTIONARY;
         }
         /**
          * Asserts this Value has type as specified. It returns silently if assertion succeeds.
          * @param type desired type. You can combine different accepted types with the | operator.
          * @throw RuntimeError if assertion fails.
          */
         void assertType (int type) const;
         /**
          * Asserts this Value is an integer. It returns silently if assertion succeeds.
          * @throw RuntimeError if assertion fails.
          */
         inline void assertIsInteger () const {
            if (!isInteger())
               throw RuntimeError("value type assertion failed: expected integer.");
         }
         /**
          * Asserts this Value is a positive integer type as specified. It returns silently if assertion succeeds.
          * @throw RuntimeError if assertion fails.
          */
         void assertIsPositiveInteger () const {
            if (!isPositiveInteger())
               throw RuntimeError("value type assertion failed: expected positive integer.");
         }
         /**
          * @return this value type.
          */
         inline Type getType () const {
            return mType;
         }
         /**
          * @return the contained boolean value.
          * @remark it does not check type for efficiency. Behaviour is unknown and definitely incorrect if this Value is not a TYPE_BOOLEAN.
          */
         inline bool getBoolean () const {
            return mBoolean;
         }
         /**
          * @return the contained number value.
          * @remark it does not check type for efficiency. Behaviour is unknown and definitely incorrect if this Value is not a TYPE_NUMBER.
          */
         inline double getNumber () const {
            return mNumber;
         }
         /**
          * @return the contained string value.
          * @remark it does not check type for efficiency. Behaviour is unknown and definitely incorrect if this Value is not a TYPE_STRING.
          */
         inline const std::string & getString () const {
            return *reinterpret_cast<std::string*> (mObjectPointer);
         }
         //         /**
         //          * @return the contained  function first instruction index.
         //          * @remark it does not check type for efficiency. Behaviour is unknown and definitely incorrect if this Value is not a TYPE_SCRIPT_FUNCTION.
         //          */
         //         inline index_t getFunctionStartIndex () const {
         //            return mFunctionIndex;
         //         }
         /**
          * @return the contained object pointer value if the requested type is correct, 0 otherwise.
          * @remark it does not check type for efficiency. Behaviour is unknown and definitely incorrect if this Value is not a TYPE_OBJECT.
          */
         template<typename T >
         T * getObject () const {
            if (checkObjectType(typeid (T)))
               return reinterpret_cast<T*> (mObjectPointer);
            else
               return 0;
         }
         /**
          * @return the object type name string.
          * @remarks it does not check type for efficiency. Behaviour is unknown and definitely incorrect if this Value is not a TYPE_OBJECT.
          */
         std::string getObjectTypeName () const {
            return std::string(mObjectTypeName);
         }
         /**
          * Checks that the user object type corresponds to the one specified.
          * @param type desired user object type. Use typeid().
          * @return true if the two types correspond.
          * @remark it does not check type for efficiency. Behaviour is unknown and definitely incorrect if this Value is not a TYPE_OBJECT.
          */
         inline bool checkObjectType (const std::type_info & type) const {
            return strcmp(mObjectTypeName, type.name()) == 0;
         }
         /**
          * @return the contained list value.
          * @remark it does not check type for efficiency. Behaviour is unknown and definitely incorrect if this Value is not a TYPE_LIST.
          */
         inline List & getList () const {
            return *reinterpret_cast<List*> (mObjectPointer);
         }
         /**
          * @return the contained list value element at specified index.
          * @param index index of the element.
          * @remark it does not check type for efficiency. Behaviour is unknown and definitely incorrect if this Value is not a TYPE_LIST.
          */
         inline Value & getListElement (size_t index) const {
            return reinterpret_cast<List*> (mObjectPointer)->at(index);
         }
         /**
          * @return the contained dictionary value.
          * @remark it does not check type for efficiency. Behaviour is unknown and definitely incorrect if this Value is not a TYPE_DICTIONARY.
          */
         inline Dictionary & getDictionary () const {
            return *reinterpret_cast<Dictionary*> (mObjectPointer);
         }
         /**
          * @return the contained dictionary value element at specified key.
          * @param key key of the element.
          * @remark it does not check type for efficiency. Behaviour is unknown and definitely incorrect if this Value is not a TYPE_DICTIONARY.
          */
         Value & getDictionaryElement (const Value & key) const;

         /**
          * Sets this value as nil.
          */
         void setNil ();
         /**
          * Sets this value to a script-function.
          * @param functionIndex first function instruction index.
          * @param nArguments accepted number of arguments.
          * @param nRegisters number of registers to allocate.
          */
         void setFunctionValue (index_t functionIndex, unsigned char nArguments, unsigned char nRegisters);
         /**
          * Sets this value to a new list.
          * @return the newly created list.
          */
         List & setEmptyList ();
         /**
          * Sets this value to a specified list.
          * @param pList pointer to the list to assign.
          * @remark list object memory is automatically managed! By passing a new list pointer to this method, list ownership is gained by the scripting system, hence
          *    don't need to and shouldn't delete the list from your code.
          */
         void setList (List * pList);
         /**
          * Sets this value to a new dictionary.
          * @return the newly created dictionary.
          */
         Dictionary & setEmptyDictionary ();
         /**
          * Sets this value to a specified dictionary.
          * @param pDictionary pointer to the dictionary to assign.
          * @remark dictionary object memory is automatically managed! By passing a new dictionary pointer to this method, list ownership is gained by the scripting system, hence
          *    don't need to and shouldn't delete the dictionary from your code.
          */
         void setDictionary (Dictionary * pDictionary);
         /**
          * @return a boolean representation of this value.
          * @remark true values are:
          *    1) The True boolean.
          *    2) Non-zero numbers.
          *    3) Non-empty strings.
          *    4) Objects.
          *    5) Non-empty string.
          *    6) Non-empty dictionaries.
          *    7) Functions.
          */
         bool toBoolean () const;
         /**
          * @return a string representation of this value.
          */
         std::string toString () const;

         Value & operator= (const Value & original);
         Value & operator= (int original);
         Value & operator= (double original);
         Value & operator= (const std::string & original);
         Value & operator= (bool original);

         Value operator+ (const Value & original);
         Value operator- (const Value & original);
         Value operator* (const Value & original);
         Value operator/ (const Value & original);


         bool operator! () const;
         bool operator&& (const Value & right) const;
         bool operator|| (const Value & right) const;

         bool operator== (const Value & right) const;
         bool operator!= (const Value & right) const;
         bool operator< (const Value & right) const;
         bool operator> (const Value & right) const;
         bool operator<= (const Value & right) const;
         bool operator>= (const Value & right) const;

         /**
          * @return returns the string name of the specified type.
          */
         static const std::string & getTypeName (Type type);

      private:
         Type mType;

         union {
            bool mBoolean;
            double mNumber;

            struct {
               int* mpReferenceCount;
               void* mObjectPointer;
               const char* mObjectTypeName;
            };

            struct {
               unsigned char mnFunctionRegisters;
               unsigned char mnArguments;
               index_t mFunctionIndex;
            };

         };

         /**
          * Manages memory and deletes the pointed object if necessary.
          */
         void cleanup ();
         /**
          * Operation is not valid.
          */
         void throwOperationError (const std::string& operation, Type firstValueType, Type secondValueType) const throw ( RuntimeError);
      };

      struct ValueComp {
         bool operator() (const Value& lhs, const Value & rhs) const {
            return lhs.toString() < rhs.toString();
         }
      };
   }
}
#endif	/* ION_VALUE_H */

