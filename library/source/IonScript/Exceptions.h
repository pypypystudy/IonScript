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

#ifndef ION_SCRIPT_EXCEPTIONS_H
#define	ION_SCRIPT_EXCEPTIONS_H

#include <sstream>
#include <exception>
#include <cstdio>

#include "Lexer.h"

namespace ionscript {

   class IonScriptException : public std::exception {
   public:
      IonScriptException() throw () { }
      IonScriptException(const std::string& what) throw () : mWhat(what) { }
      virtual ~IonScriptException() throw () { }
      const char* what() const throw () {
         return mWhat.c_str();
      }
   protected:
      std::string mWhat;
   };

   class BadStreamException : public IonScriptException {
   public:
      BadStreamException() throw () : IonScriptException("Given stream is not good for reading.") { }
   };

   class CompileException : public IonScriptException {
   public:
      CompileException(size_t lineNumber, size_t column) throw () {
         mMessageBuilder << "At line " << lineNumber << ":" << column << ": ";
      }
      CompileException(const CompileException& orig) throw () {
         mMessageBuilder.str(mMessageBuilder.str());
         mWhat = orig.mWhat;
      }
      ~CompileException() throw () { }
      const char* what() const throw () {
         return mMessageBuilder.str().c_str();
      }
      CompileException & operator=(const CompileException& orig) {
         mMessageBuilder.str(mMessageBuilder.str());
         mWhat = orig.mWhat;
         return *this;
      }
   protected:
      std::stringstream mMessageBuilder;
   };

   class LexicalError : public CompileException {
   public:
      LexicalError(size_t lineNumber, size_t column, char c) throw () : CompileException(lineNumber, column) {
         if (c == EOF)
            mMessageBuilder << "unexpected end-of-file found.";
         else
            mMessageBuilder << "unexpected character \'" << c << "\' found.";
      }
   };

   class SyntaxError : public CompileException {
   public:
      SyntaxError(size_t lineNumber, size_t column, Lexer::TokenType tokenType, const std::string& tokenString) throw () : CompileException(lineNumber, column) {
         mMessageBuilder << "unexpected ";
         switch (tokenType) {
            case Lexer::T_EOS:
               mMessageBuilder << "end of stream";
               break;
            case Lexer::T_NEWLINE:
               mMessageBuilder << "newline";
               break;
            case Lexer::T_NUMBER:
               mMessageBuilder << "number " << tokenString;
               break;
            case Lexer::T_STRING:
               mMessageBuilder << "string " << tokenString;
               break;
            default:
               mMessageBuilder << "token \"" << tokenString << "\"";
         }
         mMessageBuilder << ".";
      }
   };

   /**
    * Error thrown when the program is correct syntatically but presents wants to do something incorrect or senseless.
    * @param line
    * @param error message.
    */
   class SemanticError : public CompileException {
   public:
      SemanticError(size_t lineNumber, size_t column, const std::string& error) throw () : CompileException(lineNumber, column) {
         mMessageBuilder << error << ".";
      }
   };

   /**
    * Error that occurs during execution of bytecode and cannot be statically detected.
    */
   class RuntimeError : public IonScriptException {
   public:
      RuntimeError(const std::string& error) throw () : IonScriptException("RuntimeError: " + error) { }
   };

   class UndefinedGlobalVariableException : public IonScriptException {
   public:
      UndefinedGlobalVariableException(const std::string& key) throw () : IonScriptException("Global variable " + key + " undefined.") { }
   };

}
#endif	/* ION_SCRIPT_EXCEPTIONS_H */

