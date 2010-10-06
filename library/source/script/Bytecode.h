/*
 * Ion-Engine
 * Copyright Canio Massimo Tristano <massimo.tristano@gmail.com>
 * All rights reserved.
 */


#ifndef ION_BYTECODE_H
#define	ION_BYTECODE_H

#include "OpCode.h"

#include <iostream>
#include <sstream>

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
         BytecodeWriter & operator<< (int data);
         BytecodeWriter & operator<< (unsigned int data);
         BytecodeWriter & operator<< (double data);
         BytecodeWriter & operator<< (OpCode data);
         BytecodeWriter & operator<< (const std::string& data);
         BytecodeWriter & operator<< (bool data);

      private:
         std::vector<char>& mOutput;
      };

      class BytecodeReader {
      public:
         BytecodeReader (std::vector<char>& output);
         size_t getCursorPosition () const {
            return mPosition;
         }

         void setCursorPosition (index_t index);

         bool continues () const;
         void print (std::ostream& outStream);



         BytecodeReader & operator>> (char& data);
         BytecodeReader & operator>> (unsigned char& data);
         BytecodeReader & operator>> (int& data);
         BytecodeReader & operator>> (unsigned int& data);
         BytecodeReader & operator>> (double& data);
         BytecodeReader & operator>> (OpCode& data);
         BytecodeReader & operator>> (std::string& data);
         BytecodeReader & operator>> (bool& data);

      private:
         size_t mPosition;
         std::vector<char>& mOutput;
      };

   }
}
#endif	/* ION_BYTECODE_H */

