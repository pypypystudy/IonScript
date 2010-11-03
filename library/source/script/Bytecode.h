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


#ifndef ION_SCRIPT_BYTECODE_H
#define	ION_SCRIPT_BYTECODE_H

#include "OpCode.h"

#include <iostream>
#include <sstream>
#include <vector>

namespace ion {

   namespace script {

      // NOTE: These two classes are temporary and are gonna be replaced by a 
      // memorystream class soon.

      class BytecodeWriter {
      public:
         BytecodeWriter (std::vector<char>& output);
         void set (size_t offset, unsigned int value);
         void set (size_t offset, unsigned char value);
         size_t getSize () const {
            return mOutput.size();
         }
         BytecodeWriter & operator<< (char data);
         BytecodeWriter & operator<< (unsigned char data);
         BytecodeWriter & operator<< (int32_t data);
         BytecodeWriter & operator<< (uint32_t data);
         BytecodeWriter & operator<< (double data);
         BytecodeWriter & operator<< (OpCode data);
         BytecodeWriter & operator<< (const std::string& data);
         BytecodeWriter & operator<< (bool data);

      private:
         std::vector<char>& mOutput;
      };

      class BytecodeReader {
      public:
         BytecodeReader (char* output);
         size_t getCursorPosition () const {
            return mPosition;
         }

         void setCursorPosition (index_t index);

         bool continues () const;
         void print (std::ostream& outStream);

         BytecodeReader & operator>> (char& data);
         BytecodeReader & operator>> (unsigned char& data);
         BytecodeReader & operator>> (int32_t& data);
         BytecodeReader & operator>> (uint32_t& data);
         BytecodeReader & operator>> (double& data);
         BytecodeReader & operator>> (OpCode& data);
         BytecodeReader & operator>> (std::string& data);
         BytecodeReader & operator>> (bool& data);

      private:
         size_t mPosition;
         size_t mSize;
         char* mOutput;
      };

   }
}
#endif	/* ION_SCRIPT_BYTECODE_H */

