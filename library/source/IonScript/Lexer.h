/*******************************************************************************
 * IonScript                                                                   *
 * (c) Canio Massimo Tristano <massimo.tristano@gmail.com>                     *
 *                                                                             *
 * This software is provided 'as-is', without any express or implied           *
 * warranty. In no event will the authors be held liable for any damages       *
 * arising from the use of this software.                                      *
 *                                                                             *
 * Permission is granted to anyone to use this software for any purpose,       *
 * including commercial applications, and to alter it and redistribute it      *
 * freely, subject to the following restrictions:                              *
 *                                                                             *
 * . The origin of this software must not be misrepresented; you must not      *
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

#ifndef IS_LEXER_H
#define	IS_LEXER_H

#include <istream>

namespace ionscript {
   class LexicalError;

   /**
    * A Lexer tokenizes a given stream of characters by means of regular
    * expressions. It is implemented with a Finite State Machine. This class is
    * used by the Parser to tokenize the given input stream.
    */
   class Lexer {
   public:

      enum TokenType {
         NONE, // Internal use only, it's not a token.
         T_AND,
         T_ASSIGNEMENT,
         T_ASTERISK,
         T_ASTERISK_ASSIGNEMENT,
         T_BREAK,
         T_COLON,
         T_COMMA,
         T_CONTINUE,
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
         T_LEFT_CURLY_BRACKET,
         T_LEFT_ROUND_BRACKET,
         T_LEFT_SQUARE_BRACKET,
         T_LESSER,
         T_LESSER_EQUALS,
         T_MINUS,
         T_MINUS_ASSIGNEMENT,
         T_NEW,
         T_NEWLINE,
         T_NIL,
         T_NOT,
         T_NOTEQUALS,
         T_NUMBER,
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
      };
   public:
      Lexer();
      /**
       * Constructs a new Lexer.
       * @param source input stream to tokenize.
       */
      Lexer(std::istream& source);
      virtual ~Lexer();
      /**
       * Sets the source to be scanned.
       */
      void setSource(std::istream& source);
      /**
       * Elaborates the input stream as much as it can until it recognizes the longest token possible, then returns its type.
       * @return the token type.
       */
      TokenType nextToken();
      /**
       * @return the currently scanned line number.
       */
      inline size_t getLine() const {
         return mLineNumber;
      }
      /**
       * @return the currently character position index from line beginning.
       */
      inline size_t getColumn() const {
         return mColumn - mTokenString.size();
      }
      /**
       * @return the current token as number.
       */
      inline double getTokenAsNumber() const {
         return mTokenNumber;
      }
      /**
       * @return the current token string.
       */
      inline const std::string& getString() const {
         return mTokenString;
      }

   private:
      /**
       * Source stream to read from.
       */
      std::istream* mpSource;
      /**
       * Currently read char.
       */
      char mCurrChar;
      /**
       * Current line number.
       */
      size_t mLineNumber;
      /**
       * Position index of current caracter from beginning of line.
       */
      size_t mColumn;
      /**
       * Current token integer value.
       */
      double mTokenNumber;
      /**
       * Current token string value.
       */
      std::string mTokenString;
      /**
       * Just a convenient and shorter helper function to throw the exception.
       */
      void error() const;
      /**
       * Get next character without
       */
      inline void getNextChar() {
         ++mColumn;
         mCurrChar = mpSource->get();
      }
      /**
       * Add current character to token string and get next character-
       */
      inline void addAndGetNextChar() {
         mTokenString += mCurrChar;
         ++mColumn;
         mCurrChar = mpSource->get();
      }
      /**
       * @return the corresponding excape character.
       */
      char getEscapeCharacter() const;
      /**
       * Scans input to read all the identifier or return immediately if the pending token is not an identifier.
       * @param incoming the token type at the time of calling the function.
       * @return the final token type.
       */
      inline TokenType scanIdentifier(TokenType incoming) {
         if (!((mCurrChar >= 'a' && mCurrChar <= 'z') || (mCurrChar >= 'A' && mCurrChar <= 'Z') || mCurrChar == '_' || (mCurrChar >= '0' && mCurrChar <= '9')))
            return incoming;
         do {
            addAndGetNextChar();
         } while ((mCurrChar >= 'a' && mCurrChar <= 'z') || (mCurrChar >= 'A' && mCurrChar <= 'Z') || mCurrChar == '_' || (mCurrChar >= '0' && mCurrChar <= '9'));
         return T_IDENTIFIER;
      }
   };

}
#endif	/* IS_LEXER_H */
