/*
 * Ion-Engine
 * Copyright Canio Massimo Tristano <massimo.tristano@gmail.com>
 * All rights reserved.
 */

#ifndef ION_LEXER_H
#define	ION_LEXER_H

#include "Exceptions.h"

#include <istream>
#include <stack>

namespace ion {
   namespace script {

      /**
       * A Lexer tokenizes a given stream of characters by means of regular expressions. It is implemented with a Finite State Machine. This class is used by the Parser to tokenize the given input stream.
       */
      class Lexer {
      public:

         typedef enum {
            T_AND,
            T_ASSIGNEMENT,
            T_ASTERISK,
            T_ASTERISK_ASSIGNEMENT,
            T_COLON,
            T_COMMA,
            T_DECIMAL,
            T_DEF,
            T_DOT,
            T_ELSE,
            T_END,
            T_EOS,
            T_EQUALS,
            T_FALSE,
            T_FOR,
            T_GREATER,
            T_GREATER_EQUALS,
            T_IDENTIFIER,
            T_IF,
            T_IN,
            T_INTEGER,
            T_LEFT_CURLY_BRACKET,
            T_LEFT_ROUND_BRACKET,
            T_LEFT_SQUARE_BRACKET,
            T_LESSER,
            T_LESSER_EQUALS,
            T_MINUS,
            T_MINUS_ASSIGNEMENT,
            T_NEWLINE,
            T_NIL,
            T_NOT,
            T_NOTEQUALS,
            T_OR,
            T_PLUS,
            T_PLUS_ASSIGNEMENT,
            T_RETURN,
            T_RIGHT_CURLY_BRACKET,
            T_RIGHT_ROUND_BRACKET,
            T_RIGHT_SQUARE_BRACKET,
            T_SEMICOLON,
            T_SLASH,
            T_SLASH_ASSIGNEMENT,
            T_STRING,
            T_TO,
            T_TRUE,
            T_WHILE,
         } TokenType;

         /**
          * Constructs a new Lexer.
          * @param source input stream to tokenize.
          */
         Lexer (std::istream& source);
         virtual ~Lexer ();
         /**
          * Elaborates the input stream as much as it can until it recognizes the longest token possible, then returns its type.
          * @return the token type.
          */
         TokenType nextToken ();
         /**
          * @return current source line index.
          */
         inline size_t getLine () const {
            return mLine;
         }
         /**
          * @return the actual token integer value.
          */
         inline int getIntegerValue () const {
            return mIntegerValue;
         }
         /**
          * @return the actual token decimal value.
          */
         inline double getDecimalValue () const {
            return mDecimalValue;
         }
         /**
          * @return the actual token string.
          */
         inline const std::string& getString () const {
            return mText;
         }

      private:
         std::istream& mSource;

         int mIntegerValue;
         double mDecimalValue;
         std::string mText;

         int mLine;
         inline static bool isIdentifier (char c) {
            return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_' || (c >= '0' && c <= '9');
         }
      };
   }
}
#endif	/* ION_LEXER_H */

