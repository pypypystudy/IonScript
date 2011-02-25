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

#include <vector>
#include <stdlib.h>

#include "Typedefs.h"
#include <cassert>

#include "Bytecode.h"

using namespace std;
using namespace ionscript;

BytecodeWriter::BytecodeWriter(std::vector<char>& output) : mOutput(output) { }

void BytecodeWriter::set(size_t offset, unsigned int value) {
   assert(offset + 4 <= mOutput.size());
   mOutput[offset] = (char) (value >> 24);
   mOutput[offset + 1] = (char) ((value << 8) >> 24);
   mOutput[offset + 2] = (char) ((value << 16) >> 24);
   mOutput[offset + 3] = (char) ((value << 24) >> 24);
}

void BytecodeWriter::set(size_t offset, small_size_t value) {
   mOutput[offset] = value;
}

BytecodeWriter & BytecodeWriter::operator<<(char data) {
   mOutput.push_back(data);
   return *this;
}

BytecodeWriter & BytecodeWriter::operator<<(small_size_t data) {
   mOutput.push_back(data);
   return *this;
}

BytecodeWriter & BytecodeWriter::operator<<(int32_t data) {
   mOutput.push_back((char) (data >> 24));
   mOutput.push_back((char) ((data << 8) >> 24));
   mOutput.push_back((char) ((data << 16) >> 24));
   mOutput.push_back((char) ((data << 24) >> 24));

   return *this;
}

BytecodeWriter & BytecodeWriter::operator<<(uint32_t data) {
   mOutput.push_back((char) (data >> 24));
   mOutput.push_back((char) ((data << 8) >> 24));
   mOutput.push_back((char) ((data << 16) >> 24));
   mOutput.push_back((char) ((data << 24) >> 24));

   return *this;
}

BytecodeWriter & BytecodeWriter::operator<<(double data) {

   union {
      double d;
      char c[sizeof (double) ];
   } temp;

   temp.d = data;
   for (size_t i = 0; i < sizeof (double); i++) {

      mOutput.push_back(temp.c[i]);
   }

   return *this;
}

BytecodeWriter & BytecodeWriter::operator<<(OpCode data) {
   mOutput.push_back(data);

   return *this;
}

BytecodeWriter & BytecodeWriter::operator<<(const std::string& data) {
   std::string::const_iterator it = data.begin();
   for (; it != data.end(); ++it)
      mOutput.push_back(*it);
   mOutput.push_back('\0');

   return *this;
}

BytecodeWriter & BytecodeWriter::operator<<(bool data) {
   mOutput.push_back((data) ? 1 : 0);

   return *this;
}


//

BytecodeReader::BytecodeReader(char* output) {
   mOutput = output;
   mPosition = sizeof (unsigned int) * 2;
   *this >> mSize;
   mPosition = 0;
}

bool BytecodeReader::continues() const {
   return mPosition < mSize;
}

void BytecodeReader::print(std::ostream& outStream) {
   size_t line = 0;
   mPosition = 0;

   unsigned int magicNumber, version;
   size_t size;
   *this >> magicNumber >> version >> size;
   outStream << "IonScript Bytecode\nVersion: " << version << "\nSize: " << size << "\nInstructions:\n";

   while (mPosition < mSize) {
      cout << mPosition << ". ";

      OpCode op;
      location_t loc1, loc2, loc3;
      (*this) >> op;

      switch (op) {
         case OP_NOP:
            outStream << "nop";
            break;

         case OP_REG:
         {
            small_size_t uchar;
            (*this) >> uchar;
            outStream << "reg " << (unsigned int) uchar;
            break;
         }

         case OP_PUSH: // push
            outStream << "push";
            break;

         case OP_PUSH_VAL: //push.val
         {
            location_t loc;
            (*this) >> loc;
            outStream << "push.val " << (int) loc;
            break;
         }

         case OP_POP: // pop
            outStream << "pop";
            break;

         case OP_POP_N:// pop.n
         {
            small_size_t n;
            (*this) >> n;
            outStream << "pop.n " << (int) n;
            break;
         }

         case OP_POP_TO:
         { // pop.to
            location_t loc;
            (*this) >> loc;
            outStream << "pop.to " << (int) loc;
            break;
         }

            //         case OP_PUSH_I: // store.i  <integer>
            //         {
            //            int integer;
            //            (*this) >> integer;
            //            outStream << "store.i " << integer;
            //            break;
            //         }

         case OP_PUSH_N:
         {
            double decimal;
            (*this) >> decimal;
            outStream << "push.d " << decimal;
            break;
         }

         case OP_PUSH_S:
         {
            string str;
            (*this) >> str;
            outStream << "push.s \"" << str << "\"";
            break;
         }

         case OP_PUSH_B:
         {
            bool b;
            (*this) >> b;
            outStream << "push.b " << ((b) ? "true" : "false");
            break;
         }

         case OP_STORE_AT_NIL:
         {
            location_t loc;
            (*this) >> loc;
            outStream << "store_at.nil " << (int) loc;
            break;
         }

         case OP_STORE_AT_F:
         {
            index_t index;
            small_size_t nRegisters, nArguments;
            (*this) >> loc1 >> index >> nArguments >> nRegisters;
            outStream << "store_at.f " << (int) loc1 << ", " << index << ", " << (int) nArguments << ", " << (int) nRegisters;
            break;
         }

         case OP_MOVE: // move     <target location>, <source location>
            (*this) >> loc1;
            (*this) >> loc2;
            outStream << "move " << (int) loc1 << ", " << (int) loc2;
            break;

         case OP_ADD: // add      <target location>: <first addend location>: <second addend location>
            (*this) >> loc1 >> loc2 >> loc3;
            outStream << "add " << (int) loc1 << ", " << (int) loc2 << ", " << (int) loc3;
            break;

         case OP_SUB: // sub      <target location>: <first addend location>: <second addend location>
            (*this) >> loc1 >> loc2 >> loc3;
            outStream << "sub " << (int) loc1 << ", " << (int) loc2 << ", " << (int) loc3;
            break;

         case OP_MUL: // mul      <target location>, <first factor location>, <second factor location>< "move " << loc1 << ", " << loc2;
            (*this) >> loc1 >> loc2 >> loc3;
            outStream << "mul " << (int) loc1 << ", " << (int) loc2 << ", " << (int) loc3;
            break;

         case OP_DIV: // mul      <target location>, <first factor location>, <second factor location>< "move " << loc1 << ", " << loc2;
            (*this) >> loc1 >> loc2 >> loc3;
            outStream << "div " << (int) loc1 << ", " << (int) loc2 << ", " << (int) loc3;
            break;

         case OP_JUMP:
         {
            index_t index;
            (*this) >> index;
            outStream << "jump " << index;
            break;
         }
         case OP_JUMP_COND:
         {
            location_t loc;
            index_t index;
            (*this) >> loc >> index;
            outStream << "jump.cond " << (int) loc << ", " << index;
            break;
         }
         case OP_RETURN_NIL:
            outStream << "ret.nil";
            break;

         case OP_RETURN:
         {
            location_t loc;
            (*this) >> loc;
            outStream << "ret " << (int) loc;

            break;
         }
         case OP_PCALL_SF_G:
         {
            location_t function;
            (*this) >> function;
            outStream << "pcall_sf.g " << (int) function;
            break;
         }
         case OP_PCALL_SF_L:
         {
            location_t function;
            (*this) >> function;
            outStream << "pcall_sf.l " << (int) function;
            break;
         }
         case OP_CALL_SF_G:
         {
            location_t function;
            small_size_t nArguments;
            (*this) >> function >> nArguments;
            outStream << "call_sf.g " << (int) function << ", " << (int) nArguments;
            break;
         }
         case OP_CALL_SF_L:
         {
            location_t function;
            small_size_t nArguments;
            (*this) >> function >> nArguments;
            outStream << "call_sf.l " << (int) function << ", " << (int) nArguments;
            break;
         }
         case OP_CALL_HF:
         {
            HostFunctionGroupID hfID;
            FunctionID cID;
            small_size_t nArguments;
            (*this) >> hfID >> cID >> nArguments;
            outStream << "call_hf " << hfID << ", " << (int) cID << ", " << (int) nArguments;
            break;
         }

         case OP_NOT:
         {
            location_t loc1, loc2;
            (*this) >> loc1 >> loc2;
            outStream << "not " << (int) loc1 << ", " << (int) loc2;
            break;
         }

         case OP_AND:
         {
            location_t loc1, loc2, loc3;
            (*this) >> loc1 >> loc2 >> loc3;
            outStream << "and " << (int) loc1 << ", " << (int) loc2 << ", " << (int) loc3;
            break;
         }

         case OP_OR:
         {
            location_t loc1, loc2, loc3;
            (*this) >> loc1 >> loc2 >> loc3;
            outStream << "or " << (int) loc1 << ", " << (int) loc2 << ", " << (int) loc3;
            break;
         }

         case OP_EQ:
         {
            location_t loc1, loc2, loc3;
            (*this) >> loc1 >> loc2 >> loc3;
            outStream << "eq " << (int) loc1 << ", " << (int) loc2 << ", " << (int) loc3;
            break;
         }

         case OP_NEQ:
         {
            location_t loc1, loc2, loc3;
            (*this) >> loc1 >> loc2 >> loc3;
            outStream << "neq " << (int) loc1 << ", " << (int) loc2 << ", " << (int) loc3;
            break;
         }

         case OP_GR:
         {
            location_t loc1, loc2, loc3;
            (*this) >> loc1 >> loc2 >> loc3;
            outStream << "gr " << (int) loc1 << ", " << (int) loc2 << ", " << (int) loc3;
            break;
         }

         case OP_GRE:
         {
            location_t loc1, loc2, loc3;
            (*this) >> loc1 >> loc2 >> loc3;
            outStream << "gr " << (int) loc1 << ", " << (int) loc2 << ", " << (int) loc3;
            break;
         }

         case OP_LS:
         {
            location_t loc1, loc2, loc3;
            (*this) >> loc1 >> loc2 >> loc3;
            outStream << "ls " << (int) loc1 << ", " << (int) loc2 << ", " << (int) loc3;
            break;
         }

         case OP_LSE:
         {
            location_t loc1, loc2, loc3;
            (*this) >> loc1 >> loc2 >> loc3;
            outStream << "lse " << (int) loc1 << ", " << (int) loc2 << ", " << (int) loc3;

            break;
         }

         case OP_LIST_NEW:
         {
            location_t loc;
            (*this) >> loc;
            outStream << "list.new " << (int) loc;
            break;
         }

         case OP_DICTIONARY_NEW:
         {
            location_t loc1;
            (*this) >> loc1;
            outStream << "dict.new " << (int) loc1;
            break;
         }

         case OP_LIST_ADD:
         {
            location_t loc1, loc2;
            (*this) >> loc1 >> loc2;
            outStream << "list.add " << (int) loc1 << ", " << (int) loc2;
            break;
         }
         case OP_DICTIONARY_ADD:
         {
            location_t loc1, loc2, loc3;
            (*this) >> loc1 >> loc2 >> loc3;
            outStream << "dict.add " << (int) loc1 << ", " << (int) loc2 << ", " << (int) loc3;
            break;
         }

         case OP_GET:
         {
            location_t loc1, loc2, loc3;
            (*this) >> loc1 >> loc2 >> loc3;
            outStream << "get " << (int) loc1 << ", " << (int) loc2 << ", " << (int) loc3;
            break;
         }
         case OP_SET:
         {
            location_t loc1, loc2, loc3;
            (*this) >> loc1 >> loc2 >> loc3;
            outStream << "set " << (int) loc1 << ", " << (int) loc2 << ", " << (int) loc3;
            break;
         }
         default:
            break;
      }
      outStream << "\n";
      ++line;
   }
}

void BytecodeReader::setCursorPosition(index_t index) {
   mPosition = index;
}

BytecodeReader & BytecodeReader::operator>>(char& data) {
   data = mOutput[mPosition++];

   return *this;
}

BytecodeReader & BytecodeReader::operator>>(small_size_t& data) {
   data = mOutput[mPosition++];

   return *this;
}

BytecodeReader & BytecodeReader::operator>>(int32_t& data) {
   char* c = &mOutput[mPosition];
   data = (c[0] << 24) + ((c[1] & 0xFF) << 16) + ((c[2] & 0xFF) << 8) + (c[3] & 0xFF);
   mPosition += 4;
   return *this;
}

BytecodeReader & BytecodeReader::operator>>(uint32_t& data) {
   char* c = &mOutput[mPosition];
   data = (c[0] << 24) + ((c[1] & 0xFF) << 16) + ((c[2] & 0xFF) << 8) + (c[3] & 0xFF);
   mPosition += 4;
   return *this;
}

BytecodeReader & BytecodeReader::operator>>(double& data) {

   union {
      double d;
      char c[sizeof (double) ];
   } temp;

   for (size_t i = 0; i < sizeof (double); i++) {
      temp.c[i] = mOutput[mPosition++];
   }
   data = temp.d;

   return *this;

   return *this;
}

BytecodeReader & BytecodeReader::operator>>(OpCode& data) {
   data = (OpCode) (mOutput[mPosition++]);

   return *this;
}

BytecodeReader & BytecodeReader::operator>>(std::string& data) {
   data = "";
   char c;
   while ((c = mOutput[mPosition++]) != '\0')
      data += c;

   return *this;
}

BytecodeReader & BytecodeReader::operator>>(bool& data) {
   data = (mOutput[mPosition++]) != 0;
   return *this;
}
