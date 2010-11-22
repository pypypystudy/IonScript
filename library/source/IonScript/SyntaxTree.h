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

#ifndef ION_SCRIPT_SYNTAXTREE_H
#define	ION_SCRIPT_SYNTAXTREE_H

#include <list>
#include <string>
#include <iostream>

namespace ion {
   namespace script {

      /**
       * A syntax tree contains the whole program structure and definition in a tree-like abstraction for easy manipulation. The SyntaxTree allows simplification
       * of certain structures that can be computed statically (imagine 1 + 1 = 2) ensuring both bytecode additional size and slower performance. The SyntaxTree
       * is the output of the parsification process operated by the Parser.
       */
      class SyntaxTree {
      public:

         enum Type {
            TYPE_AND,
            TYPE_ARGUMENT,
            TYPE_ASSIGNEMENT,
            TYPE_BLOCK,
            TYPE_BOOLEAN,
            TYPE_BREAK,
            TYPE_CONTAINER_ELEMENT,
            TYPE_CONTINUE,
            TYPE_DICTIONARY,
            TYPE_DIFFERENCE,
            TYPE_DIVISION,
            TYPE_EQUALS,
            TYPE_FOR,
            TYPE_FUNCTION_CALL,
            TYPE_FUNCTION_DEF,
            TYPE_GREATER,
            TYPE_GREATER_EQUALS,
            TYPE_IF,
            TYPE_LESSER,
            TYPE_LESSER_EQUALS,
            TYPE_LIST,
            TYPE_NEGATION,
            TYPE_NIL,
            TYPE_NOT,
            TYPE_NOT_EQUALS,
            TYPE_NUMBER,
            TYPE_OR,
            TYPE_PAIR,
            TYPE_PRODUCT,
            TYPE_RETURN,
            TYPE_STRING,
            TYPE_SUM,
            TYPE_UNKNOWN,
            TYPE_VARIABLE,
            TYPE_WHILE,
         } type;

         std::string str;

         union {
            bool boolean;
            double number;
         };

         size_t sourceLineNumber;
         std::string sourceLine;

         SyntaxTree ();
         virtual ~SyntaxTree ();
         inline SyntaxTree* getParent () const {
            return mpParent;
         }
         inline bool hasChildren () const {
            return mChildren.size() > 0;
         }
         inline const std::list<SyntaxTree*>& getChildren () const {
            return mChildren;
         }
         SyntaxTree* left () const {
            return mChildren.front();
         }
         SyntaxTree* right () const {
            return mChildren.back();
         }
         bool isInteger () const {
            return type == TYPE_NUMBER && (int) number == number;
         }
         SyntaxTree* createChild ();
         SyntaxTree* copyOnNewChild ();
         void replaceByChild (SyntaxTree& pChild);
         void copyTo (SyntaxTree& tree) const;
         void remove ();
         void simplify ();
         void dump (std::ostream & targetStream = std::cout, const std::string& spacing = "") const;

      private:
         std::list<SyntaxTree*> mChildren;
         SyntaxTree* mpParent;

         void deleteChildren ();

         static void convertToBoolean (SyntaxTree& tree);
      };
   }
}

#endif	/* ION_SCRIPT_SYNTAXTREE_H */

