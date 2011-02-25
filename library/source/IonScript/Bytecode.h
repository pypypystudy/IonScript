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

#ifndef ION_SCRIPT_BYTECODE_H
#define	ION_SCRIPT_BYTECODE_H

#include "OpCode.h"

#include <iostream>
#include <sstream>
#include <vector>
#include <stdint.h>

namespace ionscript {

   class BytecodeWriter {
   public:
      BytecodeWriter(std::vector<char>& output);
      void set(size_t offset, unsigned int value);
      void set(size_t offset, unsigned char value);
      size_t getSize() const {
         return mOutput.size();
      }
      BytecodeWriter & operator<<(char data);
      BytecodeWriter & operator<<(unsigned char data);
      BytecodeWriter & operator<<(int32_t data);
      BytecodeWriter & operator<<(uint32_t data);
      BytecodeWriter & operator<<(double data);
      BytecodeWriter & operator<<(OpCode data);
      BytecodeWriter & operator<<(const std::string& data);
      BytecodeWriter & operator<<(bool data);

   private:
      std::vector<char>& mOutput;
   };

   class BytecodeReader {
   public:
      BytecodeReader(char* output);
      size_t getCursorPosition() const {
         return mPosition;
      }

      void setCursorPosition(index_t index);

      bool continues() const;
      void print(std::ostream& outStream);

      BytecodeReader & operator>>(char& data);
      BytecodeReader & operator>>(unsigned char& data);
      BytecodeReader & operator>>(int32_t& data);
      BytecodeReader & operator>>(uint32_t& data);
      BytecodeReader & operator>>(double& data);
      BytecodeReader & operator>>(OpCode& data);
      BytecodeReader & operator>>(std::string& data);
      BytecodeReader & operator>>(bool& data);

   private:
      size_t mPosition;
      size_t mSize;
      char* mOutput;
   };
}
#endif	/* ION_SCRIPT_BYTECODE_H */

