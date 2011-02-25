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
#ifndef ION_SCRIPT_PARSER_H
#define ION_SCRIPT_PARSER_H

#include "Exceptions.h"
#include "Lexer.h"

#include <fstream>
#include <string>
#include <vector>

namespace ionscript {
      class SyntaxTree;

      class Parser {
      public:
         Parser (std::istream &source);
         void parse (SyntaxTree& tree);

      private:
         Lexer mLexer;
         Lexer::TokenType mTokenType;

         void error () const;
         inline void nextToken () {
            mTokenType = mLexer.nextToken();
         }
         void expect (Lexer::TokenType token);
         bool accept (Lexer::TokenType token);
         void endOfStatement ();
         inline void stripNewLines () {
            while (accept(Lexer::T_NEWLINE))
               ;
         }

         enum ParserState {
            STATE_INSIDE_FUNCTION = 1,
            STATE_INSIDE_LOOP = 2,
         };

         /* Non Terminals */
         void block (SyntaxTree& tree, int state);
         void statement (SyntaxTree& tree, int state);
         void functionDefinition (SyntaxTree& tree);
         void ifblock (SyntaxTree& tree, int state);
         void elseblock (SyntaxTree& tree, int state);
         void whileblock (SyntaxTree& tree, int state);
         void forblock (SyntaxTree& tree, int state);
         void expression (SyntaxTree& tree);
         void andExpression (SyntaxTree& tree);
         void orExpression (SyntaxTree& tree);
         void comparisonExpression (SyntaxTree& tree);
         void mathExpression (SyntaxTree& tree);
         void term (SyntaxTree& tree);
         void implicitFunctionCall (SyntaxTree& tree);
         void dereference (SyntaxTree& tree);
         void factor (SyntaxTree& tree);
         void params (SyntaxTree& tree);
      };
}

#endif /* ION_SCRIPT_PARSER_H */
