/*
 * Ion-Engine
 * Copyright Canio Massimo Tristano <massimo.tristano@gmail.com>
 * All rights reserved.
 */

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

         /* Non Terminals */
         void block (SyntaxTree& tree, bool insideFunction);
         void statement (SyntaxTree& tree, bool insideFunction);
         void function (SyntaxTree& tree);
         void ifblock (SyntaxTree& tree, bool insideFunction);
         void elseblock (SyntaxTree& tree, bool insideFunction);
         void whileblock (SyntaxTree& tree, bool insideFunction);
         void forblock (SyntaxTree& tree, bool insideFunction);
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

