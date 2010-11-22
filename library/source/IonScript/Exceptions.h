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

#ifndef ION_SCRIPT_EXCEPTIONS_H
#define	ION_SCRIPT_EXCEPTIONS_H

#include <sstream>
#include <exception>
#include <cstdio>

namespace ion {
   namespace script {

      class BadStreamException : public std::exception {
      public:
         BadStreamException (const std::string& message) throw () : mMessage (message) { }
         virtual ~BadStreamException () throw () { };
         const char* what () const throw () {
            return mMessage.c_str();
         }
      private:
         std::string mMessage;
      };

      /**
       * Error thrown when an unexpected token is found during scanning.
       */
      class LexicalErrorException : public std::exception {
      public:
         LexicalErrorException (int lineNumber, char unexpectedChar) throw () : mLineNumber (lineNumber), mUnexpectedChar (unexpectedChar) {
            std::stringstream ss;
            if (unexpectedChar == EOF)
               ss << "At line " << lineNumber << ": unexpected end-of-file found.";
            else
               ss << "At line " << lineNumber << ": unexpected character \'" << unexpectedChar << "\' found.";
            mMessage = ss.str();
         }
         virtual ~LexicalErrorException () throw () { }
         int getLineNumber () const throw () {
            return mLineNumber;
         }
         char getFoundChar () const throw () {
            return mUnexpectedChar;
         }
         const char* what () const throw () {
            return mMessage.c_str();
         }

      private:
         int mLineNumber;
         char mUnexpectedChar;
         std::string mMessage;
      };

      class SyntaxError : public std::exception {
      public:
         SyntaxError (size_t line, const std::string& whatIsUnexpected) throw () : mLine (line) {
            std::stringstream ss;
            ss << "Parsing error: at line " << line << ", unexpected " << whatIsUnexpected << ".";
            mMessage = ss.str();
         }
         virtual ~SyntaxError () throw () { }
         size_t getLine () const throw () {
            return mLine;
         }
         const char* what () const throw () {
            return mMessage.c_str();
         }
      private:
         size_t mLine;
         std::string mMessage;
      };

      /**
       * Error thrown when the program is correct syntatically but presents wants to do something incorrect or senseless.
       * @param line
       * @param error message.
       */
      class SemanticError : public std::exception {
      public:
         SemanticError (size_t line, const std::string& error) throw () : mLine (line) {
            std::stringstream ss;
            ss << "Compiling error: at line " << line << ", " << error << ".";
            mMessage = ss.str();
         }
         virtual ~SemanticError () throw () { }
         size_t getLine () const throw () {
            return mLine;
         }
         const char* what () const throw () {
            return mMessage.c_str();
         }
      private:
         std::string mMessage;
         size_t mLine;
      };

      /**
       * Error that occurs during execution of bytecode and cannot be statically detected.
       */
      class RuntimeError : public std::exception {
      public:
         RuntimeError (const std::string& error) throw () : mMessage ("Runtime error: " + error) { }
         virtual ~RuntimeError () throw () { }
         const char* what () const throw () {
            return mMessage.c_str();
         }
      private:
         std::string mMessage;
         size_t mLine;
      };

      class UndefinedGlobalVariableException : public std::exception {
      public:
         UndefinedGlobalVariableException (const std::string& key) throw () : mMessage ("Global variable " + key + " undefined.") { }
         virtual ~UndefinedGlobalVariableException () throw () { }
         const char* what () const throw () {
            return mMessage.c_str();
         }
      private:
         std::string mMessage;
      };
   }
}
#endif	/* ION_SCRIPT_EXCEPTIONS_H */

