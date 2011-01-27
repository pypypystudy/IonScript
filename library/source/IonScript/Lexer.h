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

#ifndef ION_SCRIPT_LEXER_H
#define	ION_SCRIPT_LEXER_H

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
                T_BREAK,
                T_COLON,
                T_COMMA,
                T_CONTINUE,
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
                T_NEW,
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
            Lexer(std::istream& source);
            virtual ~Lexer();
            /**
             * Elaborates the input stream as much as it can until it recognizes the longest token possible, then returns its type.
             * @return the token type.
             */
            TokenType nextToken();

            /**
             * @return current source line index.
             */
            inline size_t getLine() const {
                return mLine;
            }

            /**
             * @return the actual token integer value.
             */
            inline int getIntegerValue() const {
                return mIntegerValue;
            }

            /**
             * @return the actual token decimal value.
             */
            inline double getDecimalValue() const {
                return mDecimalValue;
            }

            /**
             * @return the actual token string.
             */
            inline const std::string& getString() const {
                return mText;
            }

        private:
            std::istream& mSource;

            int mIntegerValue;
            double mDecimalValue;
            std::string mText;

            int mLine;

            inline static bool isIdentifier(char c) {
                return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_' || (c >= '0' && c <= '9');
            }
        };
    }
}
#endif	/* ION_SCRIPT_LEXER_H */

