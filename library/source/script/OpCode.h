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

#ifndef ION_SCRIPT_OPCODE_H
#define	ION_SCRIPT_OPCODE_H


namespace ion {
   namespace script {

      typedef char location_t;
      typedef unsigned int index_t;
      typedef unsigned char small_size_t;

      /**
       * Operation Code supported by the VirtualMachine.
       */
      enum OpCode {
         /**
          * No operation is done.
          */
         OP_NOP,

         /**
          * reg <small_size_t: register_count>
          * Creates <register_count> registers.
          */
         OP_REG,

         /**
          * push
          * Pushes an empty value into the value-stack.
          */
         OP_PUSH,

         /**
          * push.val <location_t: target>
          * Pushes value at <target> onto the value-stack.
          */
         OP_PUSH_VAL,

         /**
          * pop
          * Removes the most recent Value from the value-stack.
          */
         OP_POP,

         /**
          * pop.n <small_size_t: count>
          * Pops <count> values from the value-stack.
          */
         OP_POP_N,

         /**
          * pop.to <location_t: target>
          * Copies the most recent Value on the value-stack to <target>, then it pops.
          */
         OP_POP_TO,

         /**
          * store.i <int: value>
          * Sets the most recent Value on the value-stack as the integer <value>.
          */
         OP_STORE_I,

         /**
          * store.n <double: value>
          * Sets the most recent Value on the value-stack as the double <value>.
          */
         OP_STORE_N,

         /**
          * store.s <string: value>
          * Sets the most recent Value on the value-stack as the string <value>.
          */
         OP_STORE_S,

         /**
          * store.b <bool: value>
          * Sets the most recent Value on the value-stack as the boolean <value>.
          */
         OP_STORE_B,

         /**
          * store_at.nil <location_t: target>
          * Sets the value <target> as nil.
          */
         OP_STORE_AT_NIL, // store_at.nil <location>

         /**
          * store_at.f <location_t: target>, <index_t: ip>, <small_size_t: registers_count>
          * Sets a function value at location <target> with first instruction index <ip> and required registers number <registers_count>.
          */
         OP_STORE_AT_F,

         /**
          * move <location_t: target>, <location_t: source>
          * Copies value at location <source> into <target>.
          */
         OP_MOVE,

         /**
          * add <location_t: target>, <location_t: first>, <location_t: second>
          * Adds value at location <first> with value at location <second> and puts the result in <target>.
          */
         OP_ADD,

         /**
          * sub <location_t: target>, <location_t: first>, <location_t: second>
          * Subtracts value at location <first> by value at location <second> and puts the result in <target>.
          */
         OP_SUB,

         /**
          * mul <location_t: target>, <location_t: first>, <location_t: second>
          * Multiplies value at location <first> with value at location <second> and puts the result in <target>.
          */
         OP_MUL,

         /**
          * div <location_t: target>, <location_t: first>, <location_t: second>
          * Divides value at location <first> by value at location <second> and puts the result in <target>.
          */
         OP_DIV,

         /**
          * not <location_t: target>, <location_t: source>
          * Logically negates value a location <source> and puts the result in <target>
          */
         OP_NOT,

         /**
          * and <location_t: target>, <location_t: first>, <location_t: second>
          * Logically ANDs value at location <first> with value at location <second> and puts the result in <target>.
          */
         OP_AND,

         /**
          * or <location_t: target>, <location_t: first>, <location_t: second>
          * Logically ORs value at location <first> by value at location <second> and puts the result in <target>.
          */
         OP_OR,

         /**
          * eq <location_t: target>, <location_t: first>, <location_t: second>
          * Puts the result of the "==" operation between <first> and <second> and puts the result in <target>
          */
         OP_EQ,

         /**
          * neq <location_t: target>, <location_t: first>, <location_t: second>
          * Puts the result of the "!=" operation between <first> and <second> and puts the result in <target>
          */
         OP_NEQ,

         /**
          * gr <location_t: target>, <location_t: first>, <location_t: second>
          * Puts the result of the ">" operation between <first> and <second> and puts the result in <target>
          */
         OP_GR,

         /**
          * gre <location_t: target>, <location_t: first>, <location_t: second>
          * Puts the result of the ">=" operation between <first> and <second> and puts the result in <target>
          */
         OP_GRE,

         /**
          * ls <location_t: target>, <location_t: first>, <location_t: second>
          * Puts the result of the "<" operation between <first> and <second> and puts the result in <target>
          */
         OP_LS,

         /**
          * lse < location_t : target>, <location_t : first>, <location_t : second>
          * Puts the result of the "<=" operation between <first> and <second> and puts the result in <target>
          */
         OP_LSE,

         /**
          * jump <index_t: index>
          * Sets the Instruction Pointer to index.
          */
         OP_JUMP,

         /**
          * jump.cond <location_t: condition>, <index_t: index>
          * Sets the Instruction Pointer to index if value at <condition> is false.
          */
         OP_JUMP_COND,

         /**
          * ret.nil
          * Destroyes the funciton-call structure, restores the IP to the previous index and pushes nil into the value-stack.
          */
         OP_RETURN_NIL,

         /**
          * ret.nil <location:t target>
          * Destroyes the funciton-call structure, restores the IP to the previous index and pushes value at <location> into the value-stack.
          */
         OP_RETURN,
         /**
          * pcall_sf.g <location:t target>
          * Prepares the call of the global function at <target>. It simply pushes the necessary registers onto the value-stack.
          */
         OP_PCALL_SF_G,
         /**
          * pcall_sf.l <location:t target>
          * Prepares the call of the local function at <target>. It simply pushes the necessary registers onto the value-stack.
          */
         OP_PCALL_SF_L,
         /**
          * call_sf.g <location_t: function>, <small_size_t: arguments_count>
          * Calls the function GLOBALLY located at <function> with <argument_count> arguments pushed previously.
          */
         OP_CALL_SF_G,

         /**
          * call_sf.l <location_t: function>, <small_size_t: arguments_count>
          * Calls the function LOCALLY located at <function> with <argument_count> arguments pushed previously.
          */
         OP_CALL_SF_L,

         /**
          * call_hf <HostFunctionGroupID: hfgID>, <FunctionID: fID>, <small_size_t: arguments_count>
          * Calls the host function group <hfgID> passing the function ID <fID> with <arguments_count> arguments pushed previously.
          */
         OP_CALL_HF,

         /**
          * list.new <location_t: target>
          * Sets the value at location <target> as a new list.
          */
         OP_LIST_NEW,

         /**
          * list.add <location_t: list>, <location_t: value>
          * Adds the value at location <value> to the list at location <list>
          */
         OP_LIST_ADD,

         /**
          * dict.new <location_t: target>
          * Sets the value at location <target> as a new dictionary.
          */
         OP_DICTIONARY_NEW,

         /**
          * dict.add <location_t: dict>, <location_t: key>, <location_t: value>
          * Adds the value at location <value> to the dictionary at location <dict> with key-value at location <key>.
          */
         OP_DICTIONARY_ADD,

         /**
          * get <location_t: target>, <location_t: cont>, <location_t: index>
          * Sets the value contained in container at location <cont> with index/key <index> into the value at location <target>
          */
         OP_GET,

         /**
          * get <location_t: target>, <location_t: cont>, <location_t: index>
          * Sets the value at location <target> to value with index/key at <index> within the container at location <cont>.
          */
         OP_SET,
      };
   }
}
#endif	/* ION_SCRIPT_OPCODE_H */

