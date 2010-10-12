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

#include "VirtualMachine.h"
#include "Exceptions.h"
#include "Parser.h"
#include "SyntaxTree.h"
#include "OpCode.h"
#include "Bytecode.h"
#include "Compiler.h"

#include <vector>
#include <map>

using namespace ion::script;
using namespace std;

enum {
   BFID_PRINT,
   BFID_POST,
   BFID_GET,
   BFID_LEN,
   BFID_APPEND,
   BFID_REMOVE,
   BFID_ASSERT,
   BFID_DUMP,
   BFID_STR,
   BFID_JOIN,
};

VirtualMachine::VirtualMachine () : mpProgram (0) {
   HostFunctionGroupID hfgID = registerHostFunctionGroup(builtinsGroup);
   setFunction("print", hfgID, BFID_PRINT, 0, -1);
   setFunction("post", hfgID, BFID_POST, 1);
   setFunction("get", hfgID, BFID_GET, 1);
   setFunction("len", hfgID, BFID_LEN, 1);
   setFunction("append", hfgID, BFID_APPEND, 2);
   setFunction("remove", hfgID, BFID_REMOVE, 2);
   setFunction("assert", hfgID, BFID_ASSERT, 1, 2);
   setFunction("dump", hfgID, BFID_DUMP); // no arguments
   setFunction("str", hfgID, BFID_STR, 1);
   setFunction("join", hfgID, BFID_JOIN, 2, -1);
}

VirtualMachine::~VirtualMachine () {
   if (mpProgram)
      delete mpProgram;
}

HostFunctionGroupID VirtualMachine::registerHostFunctionGroup (HostFunction function) {
   mHostFunctionGroups.push_back(function);
   return mHostFunctionGroups.size() - 1;
}

void VirtualMachine::setFunction (const std::string& name, HostFunctionGroupID hostFunctionID, FunctionID functionID, int minArgumentsCount, int maxArgumentsCount) {
   if (maxArgumentsCount == -2 || (maxArgumentsCount != -1 && maxArgumentsCount < minArgumentsCount))
      maxArgumentsCount = minArgumentsCount;

   FunctionInfo info;
   info.hfgID = hostFunctionID;
   info.fID = functionID;
   info.minArgumentsCount = minArgumentsCount;
   info.maxArgumentsCount = maxArgumentsCount;

   mHostFunctionsMap[name] = info;
}

void VirtualMachine::post (const std::string& name, const Value& value) {
   mGlobalVariables[name] = value;
}

bool VirtualMachine::hasGlobalVariable (const std::string& name) const {
   return mGlobalVariables.find(name) != mGlobalVariables.end();
}

void VirtualMachine::undefineVariable (const std::string& name) {
   mGlobalVariables.erase(name);
}

Value& VirtualMachine::get (const std::string& name) {
   map<string, Value>::iterator it = mGlobalVariables.find(name);
   if (it == mGlobalVariables.end())
      throw UndefinedGlobalVariableException(name);
   else
      return it->second;
}

void VirtualMachine::compile (std::istream& source, std::vector<char>& output) {
   SyntaxTree tree;
   compile(source, output, tree);
}

void VirtualMachine::compile (std::istream& source, std::vector<char>& output, SyntaxTree& tree) {
   Parser parser(source);
   parser.parse(tree);
   BytecodeWriter writer(output);
   Compiler compiler(mHostFunctionsMap);
   compiler.compile(tree, writer);
}

void VirtualMachine::run (std::vector<char>& program) {
   if (mpProgram)
      delete mpProgram;

   mpProgram = new BytecodeReader(program);

   mValuesStack.clear();
   // Preallocate alues
   mValuesStack.reserve(40);

   mIndicesStack.clear();
   mArgsVector.clear();
   mArgsVector.reserve(10);

   mIndicesStack.push_back(0); // Stack pointer

   mRunning = true;

   while (mpProgram->continues() && mRunning)
      executeInstruction();

   mRunning = false;
}

void VirtualMachine::dump (std::ostream& output) {
   output << "Values-Stack:\n";
   for (size_t i = 0; i < mValuesStack.size(); ++i)
      output << "   " << i << ", " << (long) i - (long) mIndicesStack.back() << ") " << mValuesStack[i].toString() << "\n";

   output << "Indices-Stack:\n";
   for (size_t i = 0; i < mIndicesStack.size(); ++i)
      output << "   " << i << ") " << mIndicesStack[i] << "\n";

   output << "Arguments-Vector:\n";
   for (size_t i = 0; i < mArgsVector.size(); ++i)
      output << "   " << i << ") " << mArgsVector[i].toString() << "\n";
}
//

void VirtualMachine::executeInstruction () {
   OpCode op;
   *mpProgram >> op;

   switch (op) {
      case OP_REG:
      {
         small_size_t ucval;
         *mpProgram >> ucval;
         for (size_t i = 0; i < (size_t) ucval; i++)
            mValuesStack.push_back(Value());
         mIndicesStack.back() += ucval;
         return;
      }


      case OP_CALL_SF_L:
      case OP_CALL_SF_G:
      {
         location_t functionLoc;
         small_size_t nArguments;
         *mpProgram >> functionLoc >> nArguments;

         Value functionValue;
         if (op == OP_CALL_SF_G)
            functionValue = mValuesStack[mIndicesStack[0] + functionLoc]; // global
         else
            functionValue = mValuesStack[mIndicesStack.back() + functionLoc]; //local

         if (functionValue.mType == Value::TYPE_SCRIPT_FUNCTION) {
            // Check whether the required number of arguments corresponds to the one given.
            if (functionValue.mnArguments != nArguments) {
               stringstream ss;
               ss << "wrong number of arguments given (" << (int) nArguments << " instead of " << (int) functionValue.mnArguments << ").";
               error(ss.str());
            }
            // Push return index
            mIndicesStack.push_back(mpProgram->getCursorPosition());
            // Push stack size
            mIndicesStack.push_back(mValuesStack.size());
            // Create registers
            for (size_t i = 0; i < functionValue.mnFunctionRegisters; i++)
               mValuesStack.push_back(Value());
            // Push activation frame index
            mIndicesStack.push_back(mValuesStack.size());
            // Push arguments
            for (size_t i = mArgsVector.size() - nArguments; i < mArgsVector.size(); i++)
               mValuesStack.push_back(mArgsVector[i]);
            // Remove arguments
            for (size_t i = 0; i < nArguments; i++)
               mArgsVector.pop_back();
            // Finally set the current IP
            mpProgram->setCursorPosition(functionValue.mFunctionIndex);
         } else
            throw RuntimeError("object " + functionValue.toString() + " is not callable.");
         return;
      }


      case OP_CALL_HF:
      {
         HostFunctionGroupID hfgID;
         FunctionID fID;
         small_size_t nArguments;
         *mpProgram >> hfgID >> fID >> nArguments;

         FunctionCallManager manager(*this, fID, &mArgsVector[mArgsVector.size() - nArguments], nArguments);

         // Pause the machine
         mRunning = false;

         // Set the number of arguments
         mHostFunctionArgumentsCount = nArguments;

         // Call the host function group.
         mHostFunctionGroups[hfgID](manager);
         break;
      }

      case OP_RETURN_NIL:
      {
         while (mValuesStack.size() > mIndicesStack[mIndicesStack.size() - 2])
            mValuesStack.pop_back();

         mValuesStack.push_back(Value());

         mpProgram->setCursorPosition(mIndicesStack[mIndicesStack.size() - 3]);

         mIndicesStack.pop_back();
         mIndicesStack.pop_back();
         mIndicesStack.pop_back();

         return;
      }

      case OP_RETURN:
      {
         location_t loc;
         *mpProgram >> loc;

         Value returnValue = mValuesStack[mIndicesStack.back() + loc];

         while (mValuesStack.size() > mIndicesStack[mIndicesStack.size() - 2])
            mValuesStack.pop_back();

         mValuesStack.push_back(returnValue);

         mpProgram->setCursorPosition(mIndicesStack[mIndicesStack.size() - 3]);

         mIndicesStack.pop_back();
         mIndicesStack.pop_back();
         mIndicesStack.pop_back();

         return;
      }

      case OP_PUSH:
         mValuesStack.push_back(Value());
         return;

      case OP_POP:
         mValuesStack.pop_back();
         return;

      case OP_POP_N:
      {
         small_size_t n;
         *mpProgram >> n;
         for (int i = 0; i < n; i++)
            mValuesStack.pop_back();
         return;
      }

      case OP_POP_TO:
      {
         location_t loc;
         *mpProgram >> loc;
         mValuesStack[mIndicesStack.back() + loc] = mValuesStack.back();
         mValuesStack.pop_back();
         return;
      }

      case OP_PUSH_ARG:
      {
         location_t loc;
         *mpProgram >> loc;
         mArgsVector.push_back(mValuesStack[mIndicesStack.back() + loc]);
         return;
      }

      case OP_STORE_I:
      {
         int ival;
         *mpProgram >> ival;
         mValuesStack.back() = ival;
         return;
      }

      case OP_STORE_N:
      {
         double dval;
         *mpProgram >> dval;
         mValuesStack.back() = dval;
         return;
      }

      case OP_STORE_S:
      {
         string str;
         *mpProgram >> str;
         mValuesStack.back() = str;
         return;
      }

      case OP_STORE_B:
      {
         bool b;
         *mpProgram >> b;
         mValuesStack.back() = b;
         break;
      }

      case OP_STORE_AT_NIL:
      {
         location_t loc;
         *mpProgram >> loc;
         mValuesStack[mIndicesStack.back() + loc].setNil();
         break;
      }

      case OP_STORE_AT_F:
      {
         index_t index;
         location_t loc;
         small_size_t nArguments, nRegisters;
         *mpProgram >> loc >> index >> nArguments >> nRegisters;
         mValuesStack[mIndicesStack.back() + loc].setFunctionValue(index, nArguments, nRegisters);
         return;
      }

      case OP_LIST_NEW:
      {
         location_t listLoc;
         *mpProgram >> listLoc;
         mValuesStack[mIndicesStack.back() + listLoc].setEmptyList();
         return;
      }

      case OP_LIST_ADD:
      {
         location_t listLoc, indexLoc;
         *mpProgram >> listLoc >> indexLoc;
         mValuesStack[mIndicesStack.back() + listLoc].getList().push_back(mValuesStack[mIndicesStack.back() + indexLoc]);
         return;
      }

      case OP_DICTIONARY_NEW:
      {
         location_t dictLoc;
         *mpProgram >> dictLoc;
         mValuesStack[mIndicesStack.back() + dictLoc].setEmptyDictionary();
         return;
      }

      case OP_DICTIONARY_ADD:
      {
         location_t dictLoc, keyLoc, valueLoc;
         *mpProgram >> dictLoc >> keyLoc >> valueLoc;
         mValuesStack[mIndicesStack.back() + dictLoc].getDictionary()[ mValuesStack[mIndicesStack.back() + keyLoc]] = mValuesStack[mIndicesStack.back() + valueLoc];
         return;
      }

      case OP_GET:
      {
         location_t targetLoc, contLoc, indexLoc;
         *mpProgram >> targetLoc >> contLoc >> indexLoc;
         Value& cont = mValuesStack[mIndicesStack.back() + contLoc];

         cont.assertType(Value::TYPE_LIST | Value::TYPE_DICTIONARY);

         if (cont.isList()) {

            mValuesStack[mIndicesStack.back() + indexLoc].assertIsPositiveInteger();

            size_t index = static_cast<size_t> (mValuesStack[mIndicesStack.back() + indexLoc].getNumber());

            if (index >= cont.getList().size())
               throw RuntimeError("index out of list boundaries.");

            mValuesStack[mIndicesStack.back() + targetLoc] = cont.getListElement(index);

         } else {

            Dictionary::const_iterator it = cont.getDictionary().find(mValuesStack[mIndicesStack.back() + indexLoc]);
            if (it == cont.getDictionary().end())
               throw RuntimeError("key not found in dictionary.");
            else
               mValuesStack[mIndicesStack.back() + targetLoc] = it->second;
         }

         return;
      }

      case OP_SET:
      {
         location_t valueLoc, contLoc, indexLoc;
         *mpProgram >> valueLoc >> contLoc >> indexLoc;
         Value& cont = mValuesStack[mIndicesStack.back() + contLoc];

         cont.assertType(Value::TYPE_LIST | Value::TYPE_DICTIONARY);

         if (cont.isList()) {

            mValuesStack[mIndicesStack.back() + indexLoc].assertIsPositiveInteger();

            size_t index = static_cast<size_t> (mValuesStack[mIndicesStack.back() + indexLoc].getNumber());

            if (index >= cont.getList().size())
               throw RuntimeError("index out of list boundaries.");

            cont.getList()[index] = mValuesStack[mIndicesStack.back() + valueLoc];

         } else
            cont.getDictionary()[mValuesStack[mIndicesStack.back() + indexLoc]] = mValuesStack[mIndicesStack.back() + valueLoc];

         return;
      }



      case OP_MOVE:
      {
         location_t loc1, loc2;
         *mpProgram >> loc1 >> loc2;
         mValuesStack[mIndicesStack.back() + loc1] = mValuesStack[mIndicesStack.back() + loc2];
         return;
      }

      case OP_ADD:
      {
         location_t loc1, loc2, loc3;
         *mpProgram >> loc1 >> loc2 >> loc3;
         mValuesStack[mIndicesStack.back() + loc1] = mValuesStack[mIndicesStack.back() + loc2] + mValuesStack[mIndicesStack.back() + loc3];
         return;
      }
      case OP_SUB:
      {
         location_t loc1, loc2, loc3;
         *mpProgram >> loc1 >> loc2 >> loc3;
         mValuesStack[mIndicesStack.back() + loc1] = mValuesStack[mIndicesStack.back() + loc2] - mValuesStack[mIndicesStack.back() + loc3];
         return;
      }

      case OP_MUL:
      {
         location_t loc1, loc2, loc3;
         *mpProgram >> loc1 >> loc2 >> loc3;
         mValuesStack[mIndicesStack.back() + loc1] = mValuesStack[mIndicesStack.back() + loc2] * mValuesStack[mIndicesStack.back() + loc3];
         return;
      }

      case OP_DIV:
      {
         location_t loc1, loc2, loc3;
         *mpProgram >> loc1 >> loc2 >> loc3;
         mValuesStack[mIndicesStack.back() + loc1] = mValuesStack[mIndicesStack.back() + loc2] / mValuesStack[mIndicesStack.back() + loc3];
         return;
      }

      case OP_JUMP:
      {
         index_t index;
         *mpProgram >> index;
         mpProgram->setCursorPosition(index);
         return;
      }
      case OP_JUMP_COND:
      {
         location_t loc;
         index_t index;
         *mpProgram >> loc >> index;
         if (!mValuesStack[mIndicesStack.back() + loc].toBoolean())
            mpProgram->setCursorPosition(index);
         return;
      }

      case OP_NOT:
      {
         location_t loc1, loc2;
         *mpProgram >> loc1 >> loc2;
         mValuesStack[mIndicesStack.back() + loc1] = !mValuesStack[mIndicesStack.back() + loc2 ];
         return;
      }

      case OP_AND:
      {
         location_t loc1, loc2, loc3;
         *mpProgram >> loc1 >> loc2 >> loc3;
         mValuesStack[mIndicesStack.back() + loc1] = mValuesStack[mIndicesStack.back() + loc2 ] && mValuesStack[mIndicesStack.back() + loc3 ];
         return;
      }

      case OP_OR:
      {
         location_t loc1, loc2, loc3;
         *mpProgram >> loc1 >> loc2 >> loc3;
         mValuesStack[mIndicesStack.back() + loc1] = mValuesStack[mIndicesStack.back() + loc2 ] || mValuesStack[mIndicesStack.back() + loc3 ];
         return;
      }

      case OP_EQ:
      {
         location_t loc1, loc2, loc3;
         *mpProgram >> loc1 >> loc2 >> loc3;
         mValuesStack[mIndicesStack.back() + loc1] = mValuesStack[mIndicesStack.back() + loc2 ] == mValuesStack[mIndicesStack.back() + loc3 ];
         return;
      }

      case OP_NEQ:
      {
         location_t loc1, loc2, loc3;
         *mpProgram >> loc1 >> loc2 >> loc3;
         mValuesStack[mIndicesStack.back() + loc1] = mValuesStack[mIndicesStack.back() + loc2 ] != mValuesStack[mIndicesStack.back() + loc3 ];
         return;
      }

      case OP_GR:
      {
         location_t loc1, loc2, loc3;
         *mpProgram >> loc1 >> loc2 >> loc3;
         mValuesStack[mIndicesStack.back() + loc1] = mValuesStack[mIndicesStack.back() + loc2 ] > mValuesStack[mIndicesStack.back() + loc3 ];
         return;
      }

      case OP_GRE:
      {
         location_t loc1, loc2, loc3;
         *mpProgram >> loc1 >> loc2 >> loc3;
         mValuesStack[mIndicesStack.back() + loc1] = mValuesStack[mIndicesStack.back() + loc2 ] >= mValuesStack[mIndicesStack.back() + loc3 ];
         return;
      }

      case OP_LS:
      {
         location_t loc1, loc2, loc3;
         *mpProgram >> loc1 >> loc2 >> loc3;
         mValuesStack[mIndicesStack.back() + loc1] = mValuesStack[mIndicesStack.back() + loc2 ] < mValuesStack[mIndicesStack.back() + loc3 ];
         return;
      }

      case OP_LSE:
      {
         location_t loc1, loc2, loc3;
         *mpProgram >> loc1 >> loc2 >> loc3;
         mValuesStack[mIndicesStack.back() + loc1] = mValuesStack[mIndicesStack.back() + loc2 ] <= mValuesStack[mIndicesStack.back() + loc3 ];
         return;
      }

      default:
         error("Unsupported op-code.");
         return;
   }
}

void VirtualMachine::error (const std::string& message) const {
   throw RuntimeError(message);
}

void VirtualMachine::returnValue (const Value& value) {
   mValuesStack.push_back(value);
   mRunning = true;
   mArgsVector.resize(mArgsVector.size() - mHostFunctionArgumentsCount);
}

void VirtualMachine::builtinsGroup (const FunctionCallManager& manager) {
   switch (manager.getFunctionID()) {
      case BFID_PRINT:
         for (size_t i = 0; i < manager.getArgumentsCount(); i++) {
            cout << manager.getArgument(i).toString();
            if (i != manager.getArgumentsCount() - 1)
               cout << " ";
         }
         cout << endl;
         manager.returnNil();
         return;

      case BFID_POST:
         manager.assertArgumentType(0, Value::TYPE_STRING);
         manager.mVM.mGlobalVariables[manager.getArgument(0).getString()] = manager.getArgument(1);
         manager.returnValue(manager.getArgument(1));
         return;

      case BFID_GET:
      {
         manager.assertArgumentType(0, Value::TYPE_STRING);
         map<string, Value>::iterator it = manager.mVM.mGlobalVariables.find(manager.getArgument(0).getString());
         if (it == manager.mVM.mGlobalVariables.end())
            manager.returnNil();
         else
            manager.returnValue(it->second);
         return;
      }

      case BFID_LEN:
         manager.assertArgumentType(0, Value::TYPE_STRING | Value::TYPE_LIST | Value::TYPE_DICTIONARY);
         switch (manager.getArgument(0).getType()) {
            case Value::TYPE_STRING:
               manager.returnNumber(manager.getArgument(0).getString().size());
               return;
            case Value::TYPE_LIST:
               manager.returnNumber(manager.getArgument(0).getList().size());
               return;
            case Value::TYPE_DICTIONARY:
               manager.returnNumber(manager.getArgument(0).getDictionary().size());
               return;
            default: // Never executed
               return;
         }


      case BFID_APPEND:
         manager.assertArgumentType(0, Value::TYPE_LIST);
         manager.getArgument(0).getList().push_back(manager.getArgument(1));
         manager.returnValue(manager.getArgument(0));
         return;

      case BFID_REMOVE:
         manager.assertArgumentType(0, Value::TYPE_LIST);
         manager.assertArgumentType(1, Value::TYPE_NUMBER);
         if (!manager.getArgument(1).isInteger() || manager.getArgument(1).getNumber() < 0)
            throw RuntimeError("the index of the element to remove must be a positive integer.");
         manager.getArgument(0).getList().erase(manager.getArgument(0).getList().begin() + static_cast<size_t> (manager.getArgument(1).getNumber()));
         manager.returnValue(manager.getArgument(0));
         return;

      case BFID_ASSERT:
         if (!manager.getArgument(0).toBoolean()) {
            if (manager.getArgumentsCount() == 2) {
               manager.assertArgumentType(1, Value::TYPE_STRING);
               throw RuntimeError("user assertion failed: " + manager.getArgument(1).getString() + ".");
            } else
               throw RuntimeError("user assertion failed.");
         }
         manager.returnNil();
         return;

      case BFID_DUMP:
         manager.mVM.dump(std::cout);
         manager.returnNil();
         return;

      case BFID_STR:
         manager.returnString(manager.getArgument(0).toString());
         return;

      case BFID_JOIN:
      {
         manager.assertArgumentType(0, Value::TYPE_STRING);
         string result = "";
         const string& separator = manager.getArgument(0).getString();
         if (manager.getArgument(1).isList()) {
            List& list = manager.getArgument(1).getList();
            for (size_t i = 0; i < list.size(); i++) {
               result += list[i].toString();
               if (i != list.size() - 1)
                  result += separator;
            }
         } else {
            for (size_t i = 1; i < manager.getArgumentsCount(); i++) {
               result += manager.getArgument(i).toString();
               if (i != manager.getArgumentsCount() - 1)
                  result += separator;
            }
         }
         manager.returnString(result);
         return;
      }

      default:
         manager.returnNil();
   }
}