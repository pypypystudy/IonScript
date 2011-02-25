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

#include "SyntaxTree.h"
#include "Parser.h"

#include <iostream>
#include <cstring>

using namespace std;
using namespace ionscript;

SyntaxTree::SyntaxTree() : mpParent(0) { }

SyntaxTree::~SyntaxTree() {
   deleteChildren();
}

SyntaxTree* SyntaxTree::createChild() {
   SyntaxTree* tree = new SyntaxTree();
   tree->mpParent = this;
   tree->sourceLineNumber = sourceLineNumber;
   mChildren.push_back(tree);
   return tree;
}

SyntaxTree* SyntaxTree::copyOnNewChild() {
   SyntaxTree* tree = new SyntaxTree();

   tree->mpParent = this;
   tree->type = type;
   tree->mChildren = mChildren;
   tree->sourceLineNumber = sourceLineNumber;

   memcpy(&tree->number, &number, sizeof (double));

   tree->str = str;

   mChildren.clear();

   mChildren.push_back(tree);

   return tree;
}

void SyntaxTree::replaceByChild(SyntaxTree& child) {
   type = child.type;
   memcpy(&child.number, &number, sizeof (double));
   str = child.str;

   list<SyntaxTree*> tempList = child.mChildren;

   child.mChildren.clear();

   deleteChildren();

   mChildren = tempList;
}

void SyntaxTree::copyTo(SyntaxTree& tree) const {
   tree.type = type;
   memcpy(&tree.number, &number, sizeof (double));
   tree.str = str;
   tree.sourceLineNumber = sourceLineNumber;

   std::list<SyntaxTree*>::const_iterator it;
   for (it = mChildren.begin(); it != mChildren.end(); ++it)
      (*it)->copyTo(*tree.createChild());
}

void SyntaxTree::simplify() {
   // First simplify children
   std::list<SyntaxTree*>::iterator it;
   for (it = mChildren.begin(); it != mChildren.end(); ++it)
      (*it)->simplify();

   // Then simplify yourself
   SyntaxTree *pFirst;
   SyntaxTree *pSecond;

   it = mChildren.begin();
   pFirst = *(it++);
   pSecond = *(it++);

   switch (type) {
      case TYPE_NOT:
         convertToBoolean(*pFirst);
         switch (pFirst->type) {
            case TYPE_NOT:
               * this = *pFirst->left();
               pFirst->left()->mChildren.clear();
               delete pFirst;
               break;

            case TYPE_BOOLEAN:
               type = TYPE_BOOLEAN;
               boolean = !pFirst->boolean;
               deleteChildren();

            default:
               break;
         }
         break;

      case TYPE_AND:
         convertToBoolean(*pFirst);
         convertToBoolean(*pSecond);
         if (pFirst->type == TYPE_BOOLEAN && pSecond->type == TYPE_BOOLEAN) {
            type = TYPE_BOOLEAN;
            boolean = pFirst->boolean && pSecond->boolean;
            deleteChildren();
         }
         break;

      case TYPE_OR:
         convertToBoolean(*pFirst);
         convertToBoolean(*pSecond);
         if (pFirst->type == TYPE_BOOLEAN && pSecond->type == TYPE_BOOLEAN) {
            type = TYPE_BOOLEAN;
            boolean = pFirst->boolean || pSecond->boolean;
            deleteChildren();
         }
         break;


      case TYPE_NEGATION:
         switch (pFirst->type) {
            case TYPE_NUMBER:
               type = TYPE_NUMBER;
               number = -pFirst->number;
               deleteChildren();
               break;
            case TYPE_NEGATION:
               * this = *pFirst->left();
               pFirst->left()->mChildren.clear();
               delete pFirst;

            default:
               break;
         }
         return;


      case TYPE_SUM:
         if (pFirst->type == TYPE_NUMBER && pSecond->type == TYPE_NUMBER) {
            type = TYPE_NUMBER;
            number = pFirst->number + pSecond->number;
            deleteChildren();
         } else if (pFirst->type == TYPE_STRING && pSecond->type == TYPE_STRING) {
            type = TYPE_STRING;
            str = pFirst->str + pSecond->str;
            deleteChildren();
         }
         return;

      case TYPE_DIFFERENCE:
         if (pFirst->type == TYPE_NUMBER && pSecond->type == TYPE_NUMBER) {
            type = TYPE_NUMBER;
            number = pFirst->number - pSecond->number;
            deleteChildren();
         }
         return;

      case TYPE_PRODUCT:
         if (pFirst->type == TYPE_NUMBER && pSecond->type == TYPE_NUMBER) {
            type = TYPE_NUMBER;
            number = pFirst->number * pSecond->number;
            deleteChildren();
         } else if (pFirst->type == TYPE_STRING && pSecond->type == TYPE_NUMBER) {
            if (pSecond->number >= 0 && pSecond->isInteger()) {
               type = TYPE_STRING;
               str = "";
               for (size_t i = 0; i < (size_t) pSecond->number; i++)
                  str += pFirst->str;
               deleteChildren();
            }
         }
         return;

      case TYPE_DIVISION:
         if (pFirst->type == TYPE_NUMBER && pSecond->type == TYPE_NUMBER) {
            type = TYPE_NUMBER;
            number = pFirst->number / pSecond->number;
            deleteChildren();
         }
         return;

      case TYPE_EQUALS:
         switch (pFirst->type) {
            case TYPE_NIL:
               switch (pSecond->type) {
                  case TYPE_NIL:
                     type = TYPE_BOOLEAN;
                     boolean = true;
                     deleteChildren();
                     return;
                  case TYPE_NUMBER:
                  case TYPE_BOOLEAN:
                  case TYPE_STRING:
                     type = TYPE_BOOLEAN;
                     boolean = false;
                     deleteChildren();
                     return;
                  default:
                     return;
               }

            case TYPE_NUMBER:
               switch (pSecond->type) {
                  case TYPE_NUMBER:
                     type = TYPE_BOOLEAN;
                     if (pFirst->number == pSecond->number)
                        boolean = true;
                     else
                        boolean = false;
                     deleteChildren();
                     return;
                  case TYPE_NIL:
                  case TYPE_BOOLEAN:
                  case TYPE_STRING:
                     type = TYPE_BOOLEAN;
                     boolean = false;
                     deleteChildren();
                     return;
                  default:
                     return;
               }

            case TYPE_BOOLEAN:
               switch (pSecond->type) {
                  case TYPE_BOOLEAN:
                     type = TYPE_BOOLEAN;
                     if (pFirst->boolean == pSecond->boolean)
                        boolean = true;
                     else
                        boolean = false;
                     deleteChildren();
                     return;
                  case TYPE_NIL:
                  case TYPE_NUMBER:
                  case TYPE_STRING:
                     type = TYPE_BOOLEAN;
                     boolean = false;
                     deleteChildren();
                     return;
                  default:
                     return;
               }

            case TYPE_STRING:
               switch (pSecond->type) {
                  case TYPE_STRING:
                     type = TYPE_BOOLEAN;
                     if (pFirst->str == pSecond->str)
                        boolean = true;
                     else
                        boolean = false;
                     deleteChildren();
                     return;
                  case TYPE_NIL:
                  case TYPE_BOOLEAN:
                  case TYPE_NUMBER:
                     type = TYPE_BOOLEAN;
                     boolean = false;
                     deleteChildren();
                     return;
                  default:
                     return;
               }

            default:
               return;
         }
         return;

      case TYPE_NOT_EQUALS:
         switch (pFirst->type) {
            case TYPE_NIL:
               switch (pSecond->type) {
                  case TYPE_NIL:
                     type = TYPE_BOOLEAN;
                     boolean = false;
                     deleteChildren();
                     return;
                  case TYPE_NUMBER:
                  case TYPE_BOOLEAN:
                  case TYPE_STRING:
                     type = TYPE_BOOLEAN;
                     boolean = true;
                     deleteChildren();
                     return;
                  default:
                     return;
               }

            case TYPE_NUMBER:
               switch (pSecond->type) {
                  case TYPE_NUMBER:
                     type = TYPE_BOOLEAN;
                     if (pFirst->number != pSecond->number)
                        boolean = true;
                     else
                        boolean = false;
                     deleteChildren();
                     return;
                  case TYPE_NIL:
                  case TYPE_BOOLEAN:
                  case TYPE_STRING:
                     type = TYPE_BOOLEAN;
                     boolean = true;
                     deleteChildren();
                     return;
                  default:
                     return;
               }

            case TYPE_BOOLEAN:
               switch (pSecond->type) {
                  case TYPE_BOOLEAN:
                     type = TYPE_BOOLEAN;
                     if (pFirst->boolean != pSecond->boolean)
                        boolean = true;
                     else
                        boolean = false;
                     deleteChildren();
                     return;
                  case TYPE_NIL:
                  case TYPE_NUMBER:
                  case TYPE_STRING:
                     type = TYPE_BOOLEAN;
                     boolean = true;
                     deleteChildren();
                     return;
                  default:
                     return;
               }

            case TYPE_STRING:
               switch (pSecond->type) {
                  case TYPE_STRING:
                     type = TYPE_BOOLEAN;
                     if (pFirst->str != pSecond->str)
                        boolean = true;
                     else
                        boolean = false;
                     deleteChildren();
                     return;
                  case TYPE_NIL:
                  case TYPE_BOOLEAN:
                  case TYPE_NUMBER:
                     type = TYPE_BOOLEAN;
                     boolean = true;
                     deleteChildren();
                     return;
                  default:
                     return;
               }

            default:
               return;
         }
         return;

      case TYPE_GREATER:
         switch (pFirst->type) {
            case TYPE_NUMBER:
               switch (pSecond->type) {
                  case TYPE_NUMBER:
                     type = TYPE_BOOLEAN;
                     if (pFirst->number > pSecond->number)
                        boolean = true;
                     else
                        boolean = false;
                     deleteChildren();
                     return;
                  default:
                     return;
               }

            case TYPE_STRING:
               switch (pSecond->type) {
                  case TYPE_STRING:
                     type = TYPE_BOOLEAN;
                     if (pFirst->str > pSecond->str)
                        boolean = true;
                     else
                        boolean = false;
                     deleteChildren();
                     return;
                  default:
                     return;
               }

            default:
               return;
         }
         return;

      case TYPE_GREATER_EQUALS:
         switch (pFirst->type) {
            case TYPE_NUMBER:
               switch (pSecond->type) {
                  case TYPE_NUMBER:
                     type = TYPE_BOOLEAN;
                     if (pFirst->number >= pSecond->number)
                        boolean = true;
                     else
                        boolean = false;
                     deleteChildren();
                     return;
                  default:
                     return;
               }

            case TYPE_STRING:
               switch (pSecond->type) {
                  case TYPE_STRING:
                     type = TYPE_BOOLEAN;
                     if (pFirst->str >= pSecond->str)
                        boolean = true;
                     else
                        boolean = false;
                     deleteChildren();
                     return;
                  default:
                     return;
               }

            default:
               return;
         }
         return;

      case TYPE_LESSER:
         switch (pFirst->type) {
            case TYPE_NUMBER:
               switch (pSecond->type) {
                  case TYPE_NUMBER:
                     type = TYPE_BOOLEAN;
                     if (pFirst->number < pSecond->number)
                        boolean = true;
                     else
                        boolean = false;
                     deleteChildren();
                     return;
                  default:
                     return;
               }

            case TYPE_STRING:
               switch (pSecond->type) {
                  case TYPE_STRING:
                     type = TYPE_BOOLEAN;
                     if (pFirst->str < pSecond->str)
                        boolean = true;
                     else
                        boolean = false;
                     deleteChildren();
                     return;
                  default:
                     return;
               }

            default:
               return;
         }
         return;

      case TYPE_LESSER_EQUALS:
         switch (pFirst->type) {
            case TYPE_NUMBER:
               switch (pSecond->type) {
                  case TYPE_NUMBER:
                     type = TYPE_BOOLEAN;
                     if (pFirst->number <= pSecond->number)
                        boolean = true;
                     else
                        boolean = false;
                     deleteChildren();
                     return;
                  default:
                     return;
               }

            case TYPE_STRING:
               switch (pSecond->type) {
                  case TYPE_STRING:
                     type = TYPE_BOOLEAN;
                     if (pFirst->str <= pSecond->str)
                        boolean = true;
                     else
                        boolean = false;
                     deleteChildren();
                     return;
                  default:
                     return;
               }

            default:
               return;
         }
         return;

      case TYPE_IF:
         if (pFirst->type == TYPE_BOOLEAN) {
            if (pFirst->boolean == true)
               // Keep the true block
               replaceByChild(*pSecond);
            else if (it != mChildren.end()) {
               // Keep the false block
               SyntaxTree * pThird = *(it);
               replaceByChild(*pThird);
            }
         }
         return;

      case TYPE_WHILE:
         if (pFirst->type == TYPE_BOOLEAN) {
            if (pFirst->boolean == false) {
               deleteChildren();
               type = TYPE_BLOCK;
            }
         }
         return;

      case TYPE_FOR:
         if (pSecond->type == TYPE_BOOLEAN) {
            if (pSecond->boolean == false) {
               deleteChildren();
               type = TYPE_BLOCK;
            }
         }
         return;
      default:
         return;
   }
}

void SyntaxTree::dump(std::ostream& targetStream, const std::string& spacing) const {
   bool hs = false;
   list<SyntaxTree*>::const_iterator it;
   if (mpParent != 0)
      for (it = mpParent->mChildren.begin(); it != mpParent->mChildren.end(); ++it)
         if (this == *it) {
            if (++it != mpParent->mChildren.end())
               hs = true;
            break;
         }

   targetStream << spacing << ((hs) ? "├ " : "└ ");

   switch (type) {
      case SyntaxTree::TYPE_UNKNOWN:
         targetStream << "UNKNOWN";
         break;
      case SyntaxTree::TYPE_BLOCK:
         targetStream << "BLOCK:";
         break;
      case SyntaxTree::TYPE_NEGATION:
         targetStream << "NEG:";
         break;
      case SyntaxTree::TYPE_SUM:
         targetStream << '+';
         break;
      case SyntaxTree::TYPE_DIFFERENCE:
         targetStream << '-';
         break;
      case SyntaxTree::TYPE_PRODUCT:
         targetStream << '*';
         break;
      case SyntaxTree::TYPE_DIVISION:
         targetStream << '/';
         break;
      case SyntaxTree::TYPE_ASSIGNEMENT:
         targetStream << '=';
         break;
      case SyntaxTree::TYPE_VARIABLE:
         targetStream << "ID: " << str;
         break;
      case SyntaxTree::TYPE_FUNCTION_DEF:
         targetStream << "DEF: " << str;
         break;
      case SyntaxTree::TYPE_FUNCTION_CALL:
         targetStream << "CALL: " << str;
         break;
      case SyntaxTree::TYPE_NUMBER:
         targetStream << "NUM: " << number;
         break;
      case SyntaxTree::TYPE_STRING:
         targetStream << "STR: " << str;
         break;
      case SyntaxTree::TYPE_BOOLEAN:
         targetStream << "BOOL: " << ((boolean) ? "true" : "false");
         break;
      case SyntaxTree::TYPE_AND:
         targetStream << "AND";
         break;
      case SyntaxTree::TYPE_OR:
         targetStream << "OR";
         break;
      case SyntaxTree::TYPE_IF:
         targetStream << "IF";
         break;

      case TYPE_ARGUMENT:
         targetStream << "ARG: " << str;
         break;
      case TYPE_RETURN:
         targetStream << "RET";
         break;
      case TYPE_NOT:
         targetStream << "NOT";
         break;
      case TYPE_EQUALS:
         targetStream << "==";
         break;
      case TYPE_NOT_EQUALS:
         targetStream << "!=";
         break;
      case TYPE_GREATER:
         targetStream << ">";
         break;
      case TYPE_GREATER_EQUALS:
         targetStream << ">=";
         break;
      case TYPE_LESSER:
         targetStream << "<";
         break;
      case TYPE_LESSER_EQUALS:
         targetStream << "<=";
         break;
      case TYPE_NIL:
         targetStream << "NIL";
         break;
      case TYPE_WHILE:
         targetStream << "WHILE";
         break;
      case TYPE_FOR:
         targetStream << "FOR";
         break;

      default:
         targetStream << type;
         break;
   }
   if (getChildren().size() > 0) {
      list<SyntaxTree*>::const_iterator it;
      for (it = getChildren().begin(); it != getChildren().end(); ++it) {
         targetStream << "\n";
         (*it)->dump(targetStream, spacing + ((hs) ? "|" : " ") + "  ");
      }
   }
   if (spacing == "") targetStream << "\n";
}

//

void SyntaxTree::deleteChildren() {
   std::list<SyntaxTree*>::iterator it;
   for (it = mChildren.begin(); it != mChildren.end(); ++it)
      delete *it;
   mChildren.clear();
}

void SyntaxTree::convertToBoolean(SyntaxTree& tree) {
   switch (tree.type) {
      case TYPE_NIL:
         tree.type = TYPE_BOOLEAN;
         tree.boolean = false;
         break;

      case TYPE_NUMBER:
         tree.type = TYPE_BOOLEAN;
         tree.boolean = tree.number != 0.0;
         return;

      case TYPE_STRING:
         tree.type = TYPE_BOOLEAN;
         tree.boolean = tree.str != "";
         return;

      default:
         return;
   }
}
