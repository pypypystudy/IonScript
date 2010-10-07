/*
 * Ion-Engine
 * Copyright Canio Massimo Tristano <massimo.tristano@gmail.com>
 * All rights reserved.
 */

#include "Compiler.h"
#include "SyntaxTree.h"
#include "OpCode.h"
#include "Bytecode.h"
#include "Parser.h"

#include <map>

using namespace std;
using namespace ion::script;

Compiler::Compiler (const HostFunctionsMap& hostFunctionsMap) : mHostFunctionsMap (hostFunctionsMap) { }

void Compiler::compile (const SyntaxTree& tree, BytecodeWriter& output) {
   mNamesStack.clear();
   mScriptFunctionsLocations.clear();

   mActivationFramePointer.push(0);
   mnRequiredRegisters.push(0);
   mDeclareOnly.push(false);
   mVariableDeclarationAllowed.push(false);

   // Set a temporary op for registers preallocaiton
   output << OP_REG << mnRequiredRegisters.top();

   // We're ready to go!
   compile(tree, output, -1);

   output.set(1, mnRequiredRegisters.top());
}

//

int Compiler::compile (const SyntaxTree& tree, BytecodeWriter& output, location_t target) {

   switch (tree.type) {
      case SyntaxTree::TYPE_BLOCK:
      {
         mnDeclaredValues.push(0);

         for (list<SyntaxTree*>::const_iterator it = tree.getChildren().begin();
            it != tree.getChildren().end();
            ++it)
            compile(**it, output, target);

         for (size_t i = 0; i < mnDeclaredValues.top(); ++i)
            mNamesStack.pop_back();

         while (mnDeclaredValues.top() > 0) {
            if (mnDeclaredValues.top() == 1) {
               output << OP_POP;
               break;
            } else {
               size_t count = min((int) mnDeclaredValues.top(), 256);
               mnDeclaredValues.top() -= count;
               output << OP_POP_N << (small_size_t) count;
            }
         }


         return 0;
      }

      case SyntaxTree::TYPE_IF:
      {
         list<SyntaxTree*>::const_iterator it = tree.getChildren().begin();
         const SyntaxTree* pConditionTree = *it;

         target = compile(*pConditionTree, output, target);
         output << OP_JUMP_COND << target;
         size_t jumpIndex = output.getSize();
         output << (index_t) 0;

         ++it;
         const SyntaxTree* pIfBlock = *it;
         compile(*pIfBlock, output, target);

         ++it;
         if (it != tree.getChildren().end()) {//there's an if block
            output << OP_JUMP;
            size_t afterElseJumpIndex = output.getSize();
            output << (index_t) 0;

            output.set(jumpIndex, (index_t) output.getSize());

            compile(**it, output, target);

            output.set(afterElseJumpIndex, (index_t) output.getSize());

         } else
            output.set(jumpIndex, (index_t) output.getSize());
         break;
      }

      case SyntaxTree::TYPE_WHILE:
      {
         list<SyntaxTree*>::const_iterator it = tree.getChildren().begin();

         mDeclareOnly.push(true);
         compile(**it, output, target);
         mDeclareOnly.pop();

         index_t beginning = output.getSize();

         location_t result = compile(**it, output, target);

         output << OP_JUMP_COND << result;
         index_t jumpIndex = output.getSize();
         output << (index_t) 0;

         ++it; //block
         vector<index_t> continues;
         mContinues.push(&continues);

         compile(**it, output, target);

         output << OP_JUMP << beginning;

         output.set(jumpIndex, (index_t) output.getSize());

         for (size_t i = 0; i < continues.size(); i++)
            output.set(continues[i], beginning);

         mContinues.pop();

         break;
      }

      case SyntaxTree::TYPE_FOR:
      {
         list<SyntaxTree*>::const_iterator it = tree.getChildren().begin();

         // Assignement
         compile(**it, output, target);

         // Condition
         ++it;
         const SyntaxTree& conditionTree = **it;
         ++it; //increment
         const SyntaxTree& incrementTree = **it;
         ++it; // Block
         const SyntaxTree& blockTree = **it;

         mDeclareOnly.push(true);
         compile(conditionTree, output, target);
         compile(incrementTree, output, target);
         mDeclareOnly.pop();

         index_t beginning = output.getSize();

         location_t result = compile(conditionTree, output, target);

         output << OP_JUMP_COND << result;
         index_t jumpIndex = output.getSize();
         output << (index_t) 0;

         vector<index_t> continues;
         mContinues.push(&continues);
         compile(blockTree, output, target);

         index_t incrementIndex = output.getSize();
         compile(incrementTree, output, target);

         output << OP_JUMP << beginning;

         output.set(jumpIndex, (index_t) output.getSize());

         for (size_t i = 0; i < continues.size(); i++)
            output.set(continues[i], incrementIndex);

         mContinues.pop();
         break;
      }

      case SyntaxTree::TYPE_CONTINUE:
         output << OP_JUMP;
         mContinues.top()->push_back(output.getSize());
         output << (index_t) 0;
         return target;

      case SyntaxTree::TYPE_FUNCTION_DEF:
      {
         location_t loc;
         if (!findLocalName(tree.str, loc)) {
            ++mnDeclaredValues.top();
            mNamesStack.push_back(tree.str);
            output << OP_PUSH;
            loc = mNamesStack.size() - 1 - mActivationFramePointer.top();
         }
         mScriptFunctionsLocations[tree.str] = loc;

         output << OP_STORE_AT_F << loc;
         size_t storeIndex = output.getSize();
         output << (index_t) 0; //temporary

         output << (small_size_t) (tree.getChildren().size() - 1);

         size_t regIndex = output.getSize();
         output << (small_size_t) 0;

         output << OP_JUMP;
         size_t jumpPos = output.getSize();
         output << (index_t) 0; //temporary

         output.set(storeIndex, (index_t) output.getSize());

         mActivationFramePointer.push(mNamesStack.size());
         mnRequiredRegisters.push(0);

         std::list<SyntaxTree*>::const_iterator it;
         for (it = tree.getChildren().begin(); it != tree.getChildren().end(); it++) {
            if ((*it)->type == SyntaxTree::TYPE_ARGUMENT)
               mNamesStack.push_back((*it)->str);
            else {// BLOCK
               compile(**it, output, target);
               output.set(regIndex, mnRequiredRegisters.top());
            }
         }

         // return nil (Rollback)
         output << OP_RETURN_NIL;

         output.set(jumpPos, (index_t) output.getSize());

         while (mNamesStack.size() > mActivationFramePointer.top())
            mNamesStack.pop_back();

         mnRequiredRegisters.pop();
         mActivationFramePointer.pop();

         return loc;
      }

      case SyntaxTree::TYPE_RETURN:
      {
         if (tree.hasChildren()) {
            location_t result = compile(*tree.left(), output, target);
            output << OP_RETURN << result;
            return result;
         } else {
            output << OP_RETURN_NIL;
            return target;
         }
      }

      case SyntaxTree::TYPE_FUNCTION_CALL:
      { // Lookup for the callable in the names stack
         OpCode callOp = OP_CALL_SF_G;
         location_t loc = 0; // useless initialization
         HostFunctionGroupID hfgID = 0;
         FunctionID fID = 0;

         if (findLocalName(tree.str, loc))
            callOp = OP_CALL_SF_L;
         else {
            map<string, location_t>::const_iterator sfit = mScriptFunctionsLocations.find(tree.str);
            if (sfit == mScriptFunctionsLocations.end()) {
               HostFunctionsMap::const_iterator hfit = mHostFunctionsMap.find(tree.str);
               if (hfit != mHostFunctionsMap.end()) {
                  callOp = OP_CALL_HF;

                  const FunctionInfo& info = hfit->second;

                  hfgID = info.hfgID;
                  fID = info.fID;

                  // Make sure that the number of arguments falls within the range between minArgumentsCount and maxArgumentsCount.
                  if ((int) tree.getChildren().size() < info.minArgumentsCount ||
                     (info.maxArgumentsCount != -1 && (int) tree.getChildren().size() > info.maxArgumentsCount)) {
                     stringstream ss;
                     ss << "wrong number of arguments given. ";
                     if (info.maxArgumentsCount == -1)
                        ss << "It must be at least " << info.minArgumentsCount;
                     else if (info.maxArgumentsCount == info.minArgumentsCount)
                        ss << "It must be exactly " << info.minArgumentsCount;
                     else
                        ss << "It must be between " << info.minArgumentsCount << " and " << info.maxArgumentsCount;
                     ss << " while it is " << tree.getChildren().size() << ".";
                     error(tree.sourceLineNumber, ss.str());
                  }

               } else
                  error(tree.sourceLineNumber, "Could not found function \"" + tree.str + "\".");
            } else
               loc = sfit->second;
         }

         std::list<SyntaxTree*>::const_iterator it;
         for (it = tree.getChildren().begin(); it != tree.getChildren().end(); it++) {

            location_t regLoc = target;
            if (regLoc >= 0)
               regLoc = -1;

            location_t result = compile(**it, output, regLoc);

            if (result < 0)
               mnRequiredRegisters.top() = max((int) mnRequiredRegisters.top(), (int) -regLoc);

            output << OP_PUSH_ARG << result;
         }

         if (callOp == OP_CALL_HF)
            output << callOp << hfgID << fID << (small_size_t) tree.getChildren().size();
         else
            output << callOp << loc << (small_size_t) tree.getChildren().size();

         mnRequiredRegisters.top() = max((int) -target, (int) mnRequiredRegisters.top());

         output << OP_POP_TO << target;

         return target;
      }

      case SyntaxTree::TYPE_NIL:
         output << OP_STORE_AT_NIL << target;
         return target;

      case SyntaxTree::TYPE_NUMBER:
      {
         location_t loc = 0;
         stringstream s;
         s << tree.number;
         if (findLocalName(s.str(), loc))
            return loc;
         else {
            ++mnDeclaredValues.top();
            mNamesStack.push_back(s.str());
            output << OP_PUSH;
            output << OP_STORE_N << tree.number;
            return mNamesStack.size() - 1 - mActivationFramePointer.top();
         }
      }

      case SyntaxTree::TYPE_STRING:
      {
         location_t loc = 0;
         if (findLocalName(tree.str, loc))
            return loc;
         else {
            ++mnDeclaredValues.top();
            mNamesStack.push_back(tree.str);
            output << OP_PUSH;
            output << OP_STORE_S << tree.str;
            return mNamesStack.size() - 1 - mActivationFramePointer.top();
         }
      }

      case SyntaxTree::TYPE_VARIABLE:
      {
         location_t loc = 0;
         if (findLocalName(tree.str, loc))
            return loc;
         else {
            if (!mVariableDeclarationAllowed.top())
               error(tree.sourceLineNumber, "undefined variable \"" + tree.str + "\".");

            ++mnDeclaredValues.top();
            mNamesStack.push_back(tree.str);
            output << OP_PUSH;
            return mNamesStack.size() - 1 - mActivationFramePointer.top();
         }
      }

      case SyntaxTree::TYPE_LIST:
      {
         output << OP_LIST_NEW << target;
         std::list<SyntaxTree*>::const_iterator it;
         location_t reg = (target < 0) ? target - 1 : -1;
         for (it = tree.getChildren().begin(); it != tree.getChildren().end(); it++) {
            location_t result = compile(**it, output, reg);
            output << OP_LIST_ADD << target << result;

            if (result < 0)
               max((int) -result, (int) mnRequiredRegisters.top());
         }

         return target;
      }

      case SyntaxTree::TYPE_DICTIONARY:
      {
         output << OP_DICTIONARY_NEW << target;
         std::list<SyntaxTree*>::const_iterator it;

         for (it = tree.getChildren().begin(); it != tree.getChildren().end(); it++) {

            location_t reg = (target < 0) ? target - 1 : -1;

            location_t index = compile(*(*it)->left(), output, reg);

            if (reg < 0 && index == reg) {
               mnRequiredRegisters.top() = max((int) -reg, (int) mnRequiredRegisters.top());
               --reg;
            }
            location_t value = compile(*(*it)->right(), output, reg);

            if (reg < 0 && value == reg)
               mnRequiredRegisters.top() = max((int) -reg, (int) mnRequiredRegisters.top());

            mnRequiredRegisters.top() = max((int) mnRequiredRegisters.top(), (int) -reg);

            output << OP_DICTIONARY_ADD << target << index << value;
         }

         return target;
      }

      case SyntaxTree::TYPE_CONTAINER_ELEMENT:
      {
         location_t reg = (target < 0) ? target : -1;
         location_t listLoc = compile(*tree.left(), output, reg);
         if (listLoc == reg)
            reg--;
         location_t indexLoc = compile(*tree.right(), output, reg);

         output << OP_GET << target << listLoc << indexLoc;
         return target;
      }

      case SyntaxTree::TYPE_ASSIGNEMENT:
      {
         if (tree.left()->type != SyntaxTree::TYPE_VARIABLE && tree.left()->type != SyntaxTree::TYPE_CONTAINER_ELEMENT)
            error(tree.sourceLineNumber, "In an assignement, left value must be a varible or a container element.");

         location_t listLoc = 0, indexLoc = 0, result = 0;

         if (tree.left()->type == SyntaxTree::TYPE_CONTAINER_ELEMENT) {
            location_t reg = (target < 0) ? target : -1;
            listLoc = compile(*tree.left()->left(), output, reg);
            if (listLoc == reg) reg--;
            indexLoc = compile(*tree.left()->right(), output, reg);

         } else {
            mVariableDeclarationAllowed.push(true);
            target = compile(*tree.left(), output, target);
            mVariableDeclarationAllowed.pop();
         }

         result = compile(*tree.right(), output, target);

         if (!mDeclareOnly.top()) {
            if (tree.left()->type == SyntaxTree::TYPE_CONTAINER_ELEMENT)
               output << OP_SET << result << listLoc << indexLoc;
            if (result != target)
               output << OP_MOVE << target << result; // move <target>, <result>
         }
         return target;
      }

      case SyntaxTree::TYPE_SUM:
         compileExpressionNodeChildren(tree, output, target, OP_ADD);
         return target;

      case SyntaxTree::TYPE_DIFFERENCE:
         compileExpressionNodeChildren(tree, output, target, OP_SUB);
         return target;

      case SyntaxTree::TYPE_PRODUCT:
         compileExpressionNodeChildren(tree, output, target, OP_MUL);
         return target;

      case SyntaxTree::TYPE_DIVISION:
         compileExpressionNodeChildren(tree, output, target, OP_DIV);
         return target;

      case SyntaxTree::TYPE_BOOLEAN:
      {
         location_t loc;
         string s = ((tree.boolean) ? "true" : "false");
         if (findLocalName(s, loc))
            return loc;
         else {
            // well, it hasn't been found. Let's add it to the stack.
            ++mnDeclaredValues.top();
            mNamesStack.push_back(s);
            output << OP_PUSH;
            output << OP_STORE_B << tree.boolean;
            return mNamesStack.size() - 1 - mActivationFramePointer.top();

         }
      }

      case SyntaxTree::TYPE_NOT:
      {
         location_t reg, left;

         reg = (target < 0) ? target : -1;
         left = compile(*tree.left(), output, reg);

         if (reg < 0 && left == reg)
            mnRequiredRegisters.top() = max((int) -reg, (int) mnRequiredRegisters.top());

         output << OP_NOT << target << left;
         return target;
      }

      case SyntaxTree::TYPE_AND:
         compileExpressionNodeChildren(tree, output, target, OP_AND);
         return target;

      case SyntaxTree::TYPE_OR:
         compileExpressionNodeChildren(tree, output, target, OP_OR);
         return target;

      case SyntaxTree::TYPE_EQUALS:
         checkComparisonConsistency(tree);
         compileExpressionNodeChildren(tree, output, target, OP_EQ);
         return target;

      case SyntaxTree::TYPE_NOT_EQUALS:
         checkComparisonConsistency(tree);
         compileExpressionNodeChildren(tree, output, target, OP_NEQ);
         return target;

      case SyntaxTree::TYPE_GREATER:
         checkComparisonConsistency(tree);
         compileExpressionNodeChildren(tree, output, target, OP_GR);
         return target;

      case SyntaxTree::TYPE_GREATER_EQUALS:
         checkComparisonConsistency(tree);
         compileExpressionNodeChildren(tree, output, target, OP_GRE);
         return target;

      case SyntaxTree::TYPE_LESSER:
         checkComparisonConsistency(tree);
         compileExpressionNodeChildren(tree, output, target, OP_LS);
         return target;

      case SyntaxTree::TYPE_LESSER_EQUALS:
         checkComparisonConsistency(tree);
         compileExpressionNodeChildren(tree, output, target, OP_LSE);
         return target;


      default:
         error(tree.sourceLineNumber, "unsupported feature.");
   }
   return target;
}

void Compiler::error (size_t line, const std::string & error) const {
   throw SemanticError(line, error);
}

void Compiler::compileExpressionNodeChildren (const SyntaxTree& node, BytecodeWriter& output, location_t target, OpCode op) {
   location_t reg, left, right;

   reg = (target < 0) ? target : -1;
   left = compile(*node.left(), output, reg);

   if (reg < 0 && left == reg) {
      mnRequiredRegisters.top() = max((int) -reg, (int) mnRequiredRegisters.top());
      --reg;
   }

   right = compile(*node.right(), output, reg);

   if (reg < 0 && right == reg)
      mnRequiredRegisters.top() = max((int) -reg, (int) mnRequiredRegisters.top());

   if (target < 0)
      mnRequiredRegisters.top() = max((int) -target, (int) mnRequiredRegisters.top());

   if (!mDeclareOnly.top())
      output << op << target << left << right;
}

bool Compiler::findLocalName (const std::string& name, location_t & outLocation) const {
   size_t start = mActivationFramePointer.top();
   for (size_t i = start; i < mNamesStack.size(); ++i)
      if (mNamesStack[i] == name) {
         outLocation = i - start;
         return true;
      }
   return false;
}


void Compiler::checkComparisonConsistency (const SyntaxTree& tree) const {
   if (tree.left()->type == SyntaxTree::TYPE_NIL ||
      tree.right()->type == SyntaxTree::TYPE_NIL)
      error(tree.sourceLineNumber, "cannot compare disequality of a nil value.");
   else if (tree.left()->type == SyntaxTree::TYPE_BOOLEAN ||
      tree.right()->type == SyntaxTree::TYPE_BOOLEAN)
      error(tree.sourceLineNumber, "cannot compare disequality of a boolean.");
   else if ((tree.left()->type == SyntaxTree::TYPE_NUMBER ||
      tree.left()->type == SyntaxTree::TYPE_STRING) && (
      tree.right()->type == SyntaxTree::TYPE_NUMBER ||
      tree.right()->type == SyntaxTree::TYPE_STRING) &&
      tree.left()->type != tree.right()->type)
      error(tree.sourceLineNumber, "disequality involves operands of different type.");
}