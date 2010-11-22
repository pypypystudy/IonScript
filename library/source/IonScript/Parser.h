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

#ifndef ION_SCRIPT_PARSER_H
#define ION_SCRIPT_PARSER_H

#include "Exceptions.h"
#include "Lexer.h"

#include <fstream>
#include <string>
#include <vector>

namespace ion {
   namespace script {

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
}

#endif /* ION_SCRIPT_PARSER_H */