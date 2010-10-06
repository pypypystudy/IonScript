/*
 * Ion-Engine
 * Copyright Canio Massimo Tristano <massimo.tristano@gmail.com>
 * All rights reserved.
 */


#ifndef ION_OPCODE_H
#define	ION_OPCODE_H


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
          * push.arg <location_t: target>
          * Pushes value at <target> into the argument-stack.
          */
         OP_PUSH_ARG,

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
#endif	/* ION_OPCODE_H */

