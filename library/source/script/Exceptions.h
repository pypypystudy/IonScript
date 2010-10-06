/*
 * Ion-Engine
 * Copyright Canio Massimo Tristano <massimo.tristano@gmail.com>
 * All rights reserved.
 */


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

