/*
 * Ion-Engine
 * Copyright Canio Massimo Tristano <massimo.tristano@gmail.com>
 * All rights reserved.
 */

#include "Lexer.h"

#include <cstdlib>
#include <stack>

using namespace std;
using namespace ion::script;

Lexer::Lexer (std::istream& source) : mSource (source), mLine (1) { }

Lexer::~Lexer () { }

Lexer::TokenType Lexer::nextToken () {
   int state = 0;
   char c = 0;

   mText = "";

   if (!mSource.good() && !mSource.eof())
      throw BadStreamException("Given stream not good for reading.");
   
   while (mSource.good() && c != EOF) {
      c = mSource.get();

      // This is a very efficient state machine.
      // It's clear this code shouldn't be that readable without the state machine diagram.
      // This code must be efficient rather than beautiful or easy, so it has been implemented
      // in the most efficient way since the lexer is a static object which will never be
      // modified once completed and fully working.
      switch (state) {

            /*********************************************************************************************************/
            /* START STATE
             */
         case 0:
            switch (c) {
                  // Ignore whitespaces and EOF
               case EOF:
               case ' ':
               case '\t':
               case '\r':
                  break;

               case '\n':
                  ++mLine;
                  return T_NEWLINE;

                  // Start String
               case '\"':
                  state = 1;
                  break;

                  // Start Number
               case '0':
               case '1':
               case '2':
               case '3':
               case '4':
               case '5':
               case '6':
               case '7':
               case '8':
               case '9':
                  mText += c;
                  state = 2;
                  break;
                  // start BREAK
               case 'b':
                  mText += 'b';
                  state = 113;
                  break;
                  // start CONTINUE
               case 'c':
                  mText += 'c';
                  state = 118;
                  break;
                  // Start IF/IN/IDENTIFIER
               case 'i':
                  mText += 'i';
                  state = 5;
                  break;

                  // Start ELSE/END/IDENTIFIER
               case 'e':
                     mText += 'e';
                     state = 8;
                     break;

                  // Start NOT/NIL
               case 'n':
                  mText += 'n';
                  state = 14;
                  break;

                  // Start AND
               case 'a':
                  mText += 'a';
                  state = 17;
                  break;

                  // Start OR
               case 'o':
                  mText += 'o';
                  state = 20;
                  break;

                  // Start FOR/FALSE
               case 'f':
                  mText += 'f';
                  state = 26;
                  break;
                  
                  // Start TRUE
               case 't':
                  mText += 't';
                  state = 22;
                  break;

               case 'd':
                  mText += 'd';
                  state = 33;
                  break;

               case 'w':
                  mText += 'w';
                  state = 36;
                  break;

               case 'r':
                  mText += 'r';
                  state = 50;
                  break;

                  // Start IDENTIF
               case 'g':
               case 'h':
               case 'j':
               case 'k':
               case 'l':
               case 'm':
               case 'p':
               case 'q':
               case 's':
               case 'u':
               case 'v':
               case 'x':
               case 'y':
               case 'z':
               case 'A':
               case 'B':
               case 'C':
               case 'D':
               case 'E':
               case 'F':
               case 'G':
               case 'H':
               case 'I':
               case 'J':
               case 'K':
               case 'L':
               case 'M':
               case 'N':
               case 'O':
               case 'P':
               case 'Q':
               case 'R':
               case 'S':
               case 'T':
               case 'U':
               case 'V':
               case 'W':
               case 'X':
               case 'Y':
               case 'Z':
               case '_':
                  mText += c;
                  state = 4;
                  break;

               case '+':
                  mText += '+';
                  state = 109;
                  break;
               case '-':
                  mText += '-';
                  state = 110;
                  break;
               case '*':
                  mText += '*';
                  state = 111;
                  break;
               case '/':
                  mText += '/';
                  state = 105;
                  break;
               case '\\':
                  mText += '\\';
                  state = 112;
                  break;

               case '(':
                  mText += '(';
                  return T_LEFT_ROUND_BRACKET;
               case ')':
                  mText += ')';
                  return T_RIGHT_ROUND_BRACKET;
               case '[':
                  mText += '[';
                  return T_LEFT_SQUARE_BRACKET;
               case ']':
                  mText += ']';
                  return T_RIGHT_SQUARE_BRACKET;
               case '{':
                  mText += '{';
                  return T_LEFT_CURLY_BRACKET;
               case '}':
                  mText += '}';
                  return T_RIGHT_CURLY_BRACKET;
               case '.':
                  mText += '.';
                  return T_DOT;
               case ',':
                  mText += ',';
                  return T_COMMA;
               case ':':
                  mText += ':';
                  return T_COLON;
               case ';':
                  mText += ';';
                  return T_SEMICOLON;
               case '=':
                  mText += '=';
                  state = 101;
                  break;
               case '>':
                  mText += '>';
                  state = 103;
                  break;
               case '<':
                  mText += '<';
                  state = 104;
                  break;
               case '!':
                  mText += '!';
                  state = 102;
                  break;
                  
               default:
                  throw LexicalErrorException(mLine, c);
            }
            break;


            /*********************************************************************************************************/
            /* STRINGS
             */
         case 1: // String reading
            switch (c) {
               case '\"':
                  return T_STRING;
               case EOF:
                  // Hey, we haven't completed our string yet!
                  throw LexicalErrorException(mLine, EOF);
               default:
                  mText += c;
                  break;
            }
            break;

            /*********************************************************************************************************/
            /* NUMBERS
             */
         case 2: // Integer part
            switch (c) {
               case '.':
                  mText += c;
                  state = 3;
                  break;
               case '0':
               case '1':
               case '2':
               case '3':
               case '4':
               case '5':
               case '6':
               case '7':
               case '8':
               case '9':
                  mText += c;
                  break;
               default:
                  mSource.unget();
                  mIntegerValue = atoi(mText.c_str());
                  return T_INTEGER;
            }
            break;

         case 3: // Decimal part
            switch (c) {
               case '0':
               case '1':
               case '2':
               case '3':
               case '4':
               case '5':
               case '6':
               case '7':
               case '8':
               case '9':
                  state = 58;
                  mText += c;
                  break;
               default:
                  mSource.unget(); // remove the letter
                  mSource.unget(); // remove the dot
                  mText = mText.substr(0,mText.size()-1);
                  mIntegerValue = atoi(mText.c_str());
                  return T_INTEGER;
            }
            break;

         case 58:
            switch (c) {
               case '0':
               case '1':
               case '2':
               case '3':
               case '4':
               case '5':
               case '6':
               case '7':
               case '8':
               case '9':
                  mText += c;
                  break;
               default:
                  mSource.unget();
                  mDecimalValue = atof(mText.c_str());
                  return T_DECIMAL;
            }
            break;

            /*********************************************************************************************************/
            /* IDENTIFIER
             */
         case 4:
            if (isIdentifier(c)) {
               mText += c;
               state = 4;
            } else {
               mSource.unget();
               return T_IDENTIFIER;
            }
            break;

            /*********************************************************************************************************/
            /* IF / IN
             */
         case 5: // i
            if (c == 'f') {
               mText += 'f';
               state = 6;
            } else if (c == 'n') {
               mText += 'n';
               state = 7;
            } else if (isIdentifier(c)) {
               mText += c;
               state = 4;
            }// Identifier "i"
            else {
               mSource.unget();
               return T_IDENTIFIER;
            }
            break;

         case 6: // if
            if (isIdentifier(c)) {
               mText += c;
               state = 4;
            } else { // IF
               mSource.unget();
               return T_IF;
            }
            break;

         case 7: // in
            if (isIdentifier(c)) {
               mText += c;
               state = 4;
            } else {
               mSource.unget();
               return T_IN;
            }
            break;

            /*********************************************************************************************************/
            /* ELSE / END
             */
         case 8: // e
            if (c == 'l') {
               mText += c;
               state = 9;
            } else if (c == 'n') {
               mText += c;
               state = 12;
            } else if (isIdentifier(c)) {
               mText += c;
               state = 4;
            } else {
               mSource.unget();
               return T_IDENTIFIER;
            }
            break;

         case 9: // el
            if (c == 's') {
               mText += c;
               state = 10;
            } else if (isIdentifier(c)) {
               mText += c;
               state = 4;
            } else {
               mSource.unget();
               return T_IDENTIFIER;
            }
            break;

         case 10: // els
            if (c == 'e') {
               mText += c;
               state = 11;
            } else if (isIdentifier(c)) {
               mText += c;
               state = 4;
            } else {
               mSource.unget();
               return T_IDENTIFIER;
            }
            break;

         case 11: // else
            if (isIdentifier(c)) {
               mText += c;
               state = 4;
            } else {
               mSource.unget();
               return T_ELSE;
            }
            break;

         case 12: // en
            if (c == 'd') {
               mText += c;
               state = 13;
            } else if (isIdentifier(c)) {
               mText += c;
               state = 4;
            } else {
               mSource.unget();
               return T_IDENTIFIER;
            }
            break;

         case 13: // end
            if (isIdentifier(c)) {
               mText += c;
               state = 4;
            } else {
               mSource.unget();
               return T_END;
            }
            break;

            /*********************************************************************************************************/
            /* NOT / NIL
             */
         case 14:
            if (c == 'o') {
               mText += 'o';
               state = 15;
            } else if (c == 'i') {
               mText += 'i';
               state = 56;
            } else if (isIdentifier(c)) {
               mText += c;
               state = 4;
            } else {
               mSource.unget();
               return T_IDENTIFIER;
            }
            break;

         case 15:
            if (c == 't') {
               mText += 't';
               state = 16;
            } else if (isIdentifier(c)) {
               mText += c;
               state = 4;
            } else {
               mSource.unget();
               return T_IDENTIFIER;
            }
            break;

         case 16:
            if (isIdentifier(c)) {
               mText += c;
               state = 4;
            } else {
               mSource.unget();
               return T_NOT;
            }
            break;


            /*********************************************************************************************************/
            /* AND
             */
         case 17: // a
            if (c == 'n') {
               mText += 'n';
               state = 18;
            } else if (isIdentifier(c)) {
               mText += c;
               state = 4;
            } else {
               mSource.unget();
               return T_IDENTIFIER;
            }
            break;

         case 18: // an
            if (c == 'd') {
               mText += 'd';
               state = 19;
            } else if (isIdentifier(c)) {
               mText += c;
               state = 4;
            } else {
               mSource.unget();
               return T_IDENTIFIER;
            }
            break;

         case 19: // and
            if (isIdentifier(c)) {
               mText += c;
               state = 4;
            } else {
               mSource.unget();
               return T_AND;
            }
            break;

            /*********************************************************************************************************/
            /* OR
             */
         case 20: // o
            if (c == 'r') {
               mText += 'r';
               state = 21;
            } else if (isIdentifier(c)) {
               mText += c;
               state = 4;
            } else {
               mSource.unget();
               return T_IDENTIFIER;
            }
            break;

         case 21: // or
            if (isIdentifier(c)) {
               mText += c;
               state = 4;
            } else {
               mSource.unget();
               return T_OR;
            }
            break;

            /*********************************************************************************************************/
            /* TRUE
             */
         case 22: // t
            if (c == 'r') {
               mText += 'r';
               state = 23;
            } else if (c == 'o') {
               mText += 'o';
               state = 59;
            } else if (isIdentifier(c)) {
               mText += c;
               state = 4;
            } else {
               mSource.unget();
               return T_IDENTIFIER;
            }
            break;

         case 23: // tr
            if (c == 'u') {
               mText += 'u';
               state = 24;
            } else if (isIdentifier(c)) {
               mText += c;
               state = 4;
            } else {
               mSource.unget();
               return T_IDENTIFIER;
            }
            break;

         case 24: // tru
            if (c == 'e') {
               mText += 'e';
               state = 25;
            } else if (isIdentifier(c)) {
               mText += c;
               state = 4;
            } else {
               mSource.unget();
               return T_IDENTIFIER;
            }
            break;

         case 25: // true
            if (isIdentifier(c)) {
               mText += c;
               state = 4;
            } else {
               mSource.unget();
               return T_TRUE;
            }
            break;

            /*********************************************************************************************************/
            /* FOR/FALSE
             */
         case 26: // f
            if (c == 'a') {
               mText += 'a';
               state = 27;
            } else if (c == 'o') {
               mText += 'o';
               state = 31;
            } else if (isIdentifier(c)) {
               mText += c;
               state = 4;
            } else {
               mSource.unget();
               return T_IDENTIFIER;
            }
            break;

         case 27: // fa
            if (c == 'l') {
               mText += 'l';
               state = 28;
            } else if (isIdentifier(c)) {
               mText += c;
               state = 4;
            } else {
               mSource.unget();
               return T_IDENTIFIER;
            }
            break;

         case 28: // fal
            if (c == 's') {
               mText += 's';
               state = 29;
            } else if (isIdentifier(c)) {
               mText += c;
               state = 4;
            } else {
               mSource.unget();
               return T_IDENTIFIER;
            }
            break;

         case 29: // fals
            if (c == 'e') {
               mText += 'e';
               state = 30;
            } else if (isIdentifier(c)) {
               mText += c;
               state = 4;
            } else {
               mSource.unget();
               return T_IDENTIFIER;
            }
            break;

         case 30: // false
            if (isIdentifier(c)) {
               mText += c;
               state = 4;
            } else {
               mSource.unget();
               return T_FALSE;
            }
            break;

         case 31: // fo
            if (c == 'r') {
               mText += 'r';
               state = 32;
            } else if (isIdentifier(c)) {
               mText += c;
               state = 4;
            } else {
               mSource.unget();
               return T_IDENTIFIER;
            }
            break;

         case 32: // for
            if (isIdentifier(c)) {
               mText += c;
               state = 4;
            } else {
               mSource.unget();
               return T_FOR;
            }
            break;

            /*********************************************************************************************************/
            /* DEF
             */
         case 33: //d
            if (c == 'e') {
               mText += 'e';
               state = 34;
            } else if (isIdentifier(c)) {
               mText += c;
               state = 4;
            } else {
               mSource.unget();
               return T_IDENTIFIER;
            }
            break;
         case 34: //de
            if (c == 'f') {
               mText += 'f';
               state = 35;
            } else if (isIdentifier(c)) {
               mText += c;
               state = 4;
            } else {
               mSource.unget();
               return T_IDENTIFIER;
            }
            break;
         case 35:
            if (isIdentifier(c)) {
               mText += c;
               state = 4;
            } else {
               mSource.unget();
               return T_DEF;
            }
            break;
            /*********************************************************************************************************/
            /* WHILE
             */
         case 36: //w
            if (c == 'h') {
               mText += 'h';
               state = 37;
            } else if (isIdentifier(c)) {
               mText += c;
               state = 4;
            } else {
               mSource.unget();
               return T_IDENTIFIER;
            }
            break;
         case 37: //wh
            if (c == 'i') {
               mText += 'i';
               state = 38;
            } else if (isIdentifier(c)) {
               mText += c;
               state = 4;
            } else {
               mSource.unget();
               return T_IDENTIFIER;
            }
            break;
         case 38: //whi
            if (c == 'l') {
               mText += 'l';
               state = 39;
            } else if (isIdentifier(c)) {
               mText += c;
               state = 4;
            } else {
               mSource.unget();
               return T_IDENTIFIER;
            }
            break;
         case 39: //whil
            if (c == 'e') {
               mText += 'e';
               state = 40;
            } else if (isIdentifier(c)) {
               mText += c;
               state = 4;
            } else {
               mSource.unget();
               return T_IDENTIFIER;
            }
            break;
         case 40: //while
            if (isIdentifier(c)) {
               mText += c;
               state = 4;
            } else {
               mSource.unget();
               return T_WHILE;
            }
            break;

            /*********************************************************************************************************/
            /* RETURN
             */
         case 50: //r
            if (c == 'e') {
               mText += 'e';
               state = 51;
            } else if (isIdentifier(c)) {
               mText += c;
               state = 4;
            } else {
               mSource.unget();
               return T_IDENTIFIER;
            }
            break;
         case 51: //re
            if (c == 't') {
               mText += 't';
               state = 52;
            } else if (isIdentifier(c)) {
               mText += c;
               state = 4;
            } else {
               mSource.unget();
               return T_IDENTIFIER;
            }
            break;
         case 52: //ret
            if (c == 'u') {
               mText += 'u';
               state = 53;
            } else if (isIdentifier(c)) {
               mText += c;
               state = 4;
            } else {
               mSource.unget();
               return T_IDENTIFIER;
            }
            break;
         case 53: //retu
            if (c == 'r') {
               mText += 'r';
               state = 54;
            } else if (isIdentifier(c)) {
               mText += c;
               state = 4;
            } else {
               mSource.unget();
               return T_IDENTIFIER;
            }
            break;
         case 54: //retur
            if (c == 'n') {
               mText += 'n';
               state = 55;
            } else if (isIdentifier(c)) {
               mText += c;
               state = 4;
            } else {
               mSource.unget();
               return T_IDENTIFIER;
            }
            break;
         case 55: //return
            if (isIdentifier(c)) {
               mText += c;
               state = 4;
            } else {
               mSource.unget();
               return T_RETURN;
            }
            break;


            /*********************************************************************************************************/
            /* NIL
             */
         case 56: //ni
            if (c == 'l') {
               mText += 'l';
               state = 57;
            } else if (isIdentifier(c)) {
               mText += c;
               state = 4;
            } else {
               mSource.unget();
               return T_IDENTIFIER;
            }
            break;

         case 57: //nil
            if (isIdentifier(c)) {
               mText += c;
               state = 4;
            } else {
               mSource.unget();
               return T_NIL;
            }
            break;

            /*********************************************************************************************************/
            /* TO
             */
         case 59:
            if (isIdentifier(c)) {
               mText += c;
               state = 4;
            } else {
               mSource.unget();
               return T_TO;
            }
            break;


            /*********************************************************************************************************/
            /* ASSIGNEMENT / EQUALS / NOT / NOTEQUALS
             */
         case 101:
            if (c == '=') {
               mText += '=';
               return T_EQUALS;
            } else {
               mSource.unget();
               return T_ASSIGNEMENT;
            }
            break;
         case 102:
            if (c == '=') {
               mText += '=';
               return T_NOTEQUALS;
            } else { // error: no ! alone
               mSource.unget();
               throw LexicalErrorException(mLine, c);
            }

            /*********************************************************************************************************/
            /* <=, >=
             */
         case 103:
            if (c == '=') { // >=
               mText += '=';
               return T_GREATER_EQUALS;
            } else { // >
               mSource.unget();
               return T_GREATER;
            }
            break;
         case 104:
            if (c == '=') { // <=
               mText += '=';
               return T_LESSER_EQUALS;
            } else { // <
               mSource.unget();
               return T_LESSER;
            }

            /*********************************************************************************************************/
            /* /= , One line comment
             */
         case 105:
            if (c == '/') // //
               state = 106;
            else if (c == '=') {
               mText += '=';
               return T_SLASH_ASSIGNEMENT;
            } else if (c == '*')
               state = 107;
            else {
               mSource.unget();
               return T_SLASH;
            }
            break;

         case 106: // Skip the commento to the new line
            if (c == '\n' || c == EOF) {
               mSource.unget();
               mText = "";
               state = 0;
            }
            break;

         case 107: // /* ... */
            if (c == '*') {
               state = 108;
            }
            break;

         case 108:
            if (c == '/') {
               mText = "";
               state = 0;
            }
            break;

            /*********************************************************************************************************/
            /* += -= *=
             */
         case 109:
            if (c == '=') {
               mText += '=';
               return T_PLUS_ASSIGNEMENT;
            } else {
               mSource.unget();
               return T_PLUS;
            }
            break;
         case 110:
            if (c == '=') {
               mText += '=';
               return T_MINUS_ASSIGNEMENT;
            } else {
               mSource.unget();
               return T_MINUS;
            }
            break;
         case 111:
            if (c == '=') {
               mText += '=';
               return T_ASTERISK_ASSIGNEMENT;
            } else {
               mSource.unget();
               return T_ASTERISK;
            }
            break;
            /*********************************************************************************************************/
            /* \ and \n, ignore newline
             */
         case 112:
            if (c == '\n') {
               // simply ignore
               mLine++;
               mText = "";
               state = 0;
            } else {
               mSource.unget();
               throw LexicalErrorException(mLine, c);
            }
            break;
            /*********************************************************************************************************/
            /* break
             */
         case 113: //b
            if (c == 'r') {
               mText += 'r';
               state = 114;
            } else if (isIdentifier(c)) {
               mText += c;
               state = 4;
            } else {
               mSource.unget();
               return T_IDENTIFIER;
            }
            break;
         case 114: //br
            if (c == 'e') {
               mText += 'e';
               state = 115;
            } else if (isIdentifier(c)) {
               mText += c;
               state = 4;
            } else {
               mSource.unget();
               return T_IDENTIFIER;
            }
            break;
         case 115: //bre
            if (c == 'a') {
               mText += 'a';
               state = 116;
            } else if (isIdentifier(c)) {
               mText += c;
               state = 4;
            } else {
               mSource.unget();
               return T_IDENTIFIER;
            }
            break;
         case 116: //brea
            if (c == 'k') {
               mText += 'k';
               state = 117;
            } else if (isIdentifier(c)) {
               mText += c;
               state = 4;
            } else {
               mSource.unget();
               return T_IDENTIFIER;
            }
            break;
         case 117: //break
            if (isIdentifier(c)) {
               mText += c;
               state = 4;
            } else {
               mSource.unget();
               return T_BREAK;
            }
            break;

            /*********************************************************************************************************/
            /* continue
             */
         case 118: //c
            if (c == 'o') {
               mText += 'o';
               state = 119;
            } else if (isIdentifier(c)) {
               mText += c;
               state = 4;
            } else {
               mSource.unget();
               return T_IDENTIFIER;
            }
            break;
         case 119: //co
            if (c == 'n') {
               mText += 'n';
               state = 120;
            } else if (isIdentifier(c)) {
               mText += c;
               state = 4;
            } else {
               mSource.unget();
               return T_IDENTIFIER;
            }
            break;
         case 120: //con
            if (c == 't') {
               mText += 't';
               state = 121;
            } else if (isIdentifier(c)) {
               mText += c;
               state = 4;
            } else {
               mSource.unget();
               return T_IDENTIFIER;
            }
            break;
         case 121: //cont
            if (c == 'i') {
               mText += 'i';
               state = 122;
            } else if (isIdentifier(c)) {
               mText += c;
               state = 4;
            } else {
               mSource.unget();
               return T_IDENTIFIER;
            }
            break;
         case 122: //conti
            if (c == 'n') {
               mText += 'n';
               state = 123;
            } else if (isIdentifier(c)) {
               mText += c;
               state = 4;
            } else {
               mSource.unget();
               return T_IDENTIFIER;
            }
            break;
         case 123: //contin
            if (c == 'u') {
               mText += 'u';
               state = 124;
            } else if (isIdentifier(c)) {
               mText += c;
               state = 4;
            } else {
               mSource.unget();
               return T_IDENTIFIER;
            }
            break;
         case 124: //continu
            if (c == 'e') {
               mText += 'e';
               state = 125;
            } else if (isIdentifier(c)) {
               mText += c;
               state = 4;
            } else {
               mSource.unget();
               return T_IDENTIFIER;
            }
            break;
         case 125: //continue
            if (isIdentifier(c)) {
               mText += c;
               state = 4;
            } else {
               mSource.unget();
               return T_CONTINUE;
            }
            break;
      }
   }
   return T_EOS;
}
