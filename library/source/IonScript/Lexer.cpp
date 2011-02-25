/*******************************************************************************
 * IonScript                                                                   *
 * (c) Canio Massimo Tristano <massimo.tristano@gmail.com>                     *
 *                                                                             *
 * This software is provided 'as-is', without any express or implied           *
 * warranty. In no event will the authors be held liable for any damages       *
 * arising from the use of this software.                                      *
 *                                                                             *
 * Permission is granted to anyone to use this software for any purpose,       *
 * including commercial applications, and to alter it and redistribute it      *
 * freely, subject to the following restrictions:                              *
 *                                                                             *
 * . The origin of this software must not be misrepresented; you must not      *
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

#include "Lexer.h"
#include "Exceptions.h"

#include <cstdlib>

#define EOF (-1)

using namespace ionscript;

Lexer::Lexer() { }

Lexer::Lexer(std::istream& source) {
   setSource(source);
}

Lexer::~Lexer() { }

void Lexer::setSource(std::istream& source) {
   mpSource = &source;
   mLineNumber = 1;
   mTokenNumber = 0;
   if (!mpSource->good() && !mpSource->eof())
      throw BadStreamException();
   getNextChar();
   mColumn = 0;
}

Lexer::TokenType Lexer::nextToken() {
   mTokenString = "";

   if (!mpSource->good() && !mpSource->eof())
      throw BadStreamException();

   while (mpSource->good() && mCurrChar != EOF) {
      switch (mCurrChar) {
            // Ignore whitespaces and EOF
         case EOF:
         case ' ':
         case '\t':
         case '\r':
            break;

            // Newlines
         case '\n':
            ++mLineNumber;
            mCurrChar = 0;
            getNextChar();
            mColumn = 0;
            return T_NEWLINE;

            // Numbers
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
         {
            bool decimal = false;
            while (true) {
               addAndGetNextChar();
               if (mCurrChar == '.') {
                  if (decimal)
                     error();
                  else
                     decimal = true;
                  continue;
               }
               if ((mCurrChar < '0' || mCurrChar > '9')) {
                  mTokenNumber = atof(mTokenString.c_str());
                  return T_NUMBER;
               }
            }
         }
            // Strings
         case '\"':
         case '\'':
         {
            char terminator = mCurrChar;
            getNextChar();
            while (true) {
               addAndGetNextChar();
               if (mCurrChar == terminator) {
                  getNextChar();
                  return T_STRING;
               } else if (mCurrChar == '\\') {
                  getNextChar();
                  mCurrChar = getEscapeCharacter();
               }
            }
         }
         case '^':
         {
            getNextChar();
            if (scanIdentifier(NONE) == NONE)
               error();
            else
               return T_STRING;
         }

            // Operations
         case '+':
         {
            addAndGetNextChar();
            if (mCurrChar == '=') {
               addAndGetNextChar();
               return T_PLUS_ASSIGNEMENT;
            } else
               return T_PLUS;
         }
         case '-':
         {
            addAndGetNextChar();
            if (mCurrChar == '=') {
               addAndGetNextChar();
               return T_MINUS_ASSIGNEMENT;
            } else
               return T_MINUS;
         }
         case '*':
         {
            addAndGetNextChar();
            if (mCurrChar == '=') {
               addAndGetNextChar();
               return T_ASTERISK_ASSIGNEMENT;
            } else
               return T_ASTERISK;
         }
         case '/':
         {
            getNextChar();
            switch (mCurrChar) {
               case '=':
                  getNextChar();
                  mTokenString = "/=";
                  return T_SLASH_ASSIGNEMENT;
               case '/': // one line comment
                  while (mCurrChar != '\n')
                     getNextChar();
                  mpSource->unget();
                  break;
               case '*': // block comment
               {
                  unsigned int count = 1;
                  while (count > 0) {
                     getNextChar();
                     switch (mCurrChar) {
                        case '/':
                           getNextChar();
                           if (mCurrChar == '*')
                              ++count;
                           break;
                        case '*':
                           getNextChar();
                           if (mCurrChar == '/')
                              --count;
                           break;
                        case EOF:
                           error();
                     }
                  }
                  break;
               }
               default:
                  mTokenString = "/";
                  return T_SLASH;
            }
            break;
         }
         case '=':
         {
            addAndGetNextChar();
            if (mCurrChar == '=') {
               addAndGetNextChar();
               return T_EQUALS;
            } else
               return T_ASSIGNEMENT;
         }
         case '!':
         {
            addAndGetNextChar();
            if (mCurrChar == '=') {
               addAndGetNextChar();
               return T_NOTEQUALS;
            } else
               error(); // no ! alone
         }
         case '<':
         {
            addAndGetNextChar();
            if (mCurrChar == '=') {
               addAndGetNextChar();
               return T_LESSER_EQUALS;
            } else
               return T_LESSER;
         }
         case '>':
         {
            addAndGetNextChar();
            if (mCurrChar == '=') {
               addAndGetNextChar();
               return T_GREATER_EQUALS;
            } else
               return T_GREATER;
         }
         case '\\':
         {
            getNextChar();
            if (mCurrChar == '\n') {
               mLineNumber++;
               mCurrChar = 0;
               getNextChar();
               continue; // ignore this and restart
            } else
               error();
         }
         case '(':
            addAndGetNextChar();
            return T_LEFT_ROUND_BRACKET;
         case ')':
            addAndGetNextChar();
            return T_RIGHT_ROUND_BRACKET;
         case '[':
            addAndGetNextChar();
            return T_LEFT_SQUARE_BRACKET;
         case ']':
            addAndGetNextChar();
            return T_RIGHT_SQUARE_BRACKET;
         case '{':
            addAndGetNextChar();
            return T_LEFT_CURLY_BRACKET;
         case '}':
            addAndGetNextChar();
            return T_RIGHT_CURLY_BRACKET;
         case '.':
            addAndGetNextChar();
            return T_DOT;
         case ',':
            addAndGetNextChar();
            return T_COMMA;
         case ':':
            addAndGetNextChar();
            return T_COLON;
         case ';':
            addAndGetNextChar();
            return T_SEMICOLON;

         case 'a': // and
         {
            addAndGetNextChar();
            if (mCurrChar == 'n') {
               addAndGetNextChar();
               if (mCurrChar == 'd') {
                  addAndGetNextChar();
                  return scanIdentifier(T_AND);
               }
            }
            return scanIdentifier(T_IDENTIFIER);
         }
         case 'b': // break
         {
            addAndGetNextChar();
            if (mCurrChar == 'r') {
               addAndGetNextChar();
               if (mCurrChar == 'e') {
                  addAndGetNextChar();
                  if (mCurrChar == 'a') {
                     addAndGetNextChar();
                     if (mCurrChar == 'k') {
                        addAndGetNextChar();
                        return scanIdentifier(T_BREAK);
                     }
                  }
               }
            }
            return scanIdentifier(T_IDENTIFIER);
         }
         case 'c': // continue
         {
            addAndGetNextChar();
            if (mCurrChar == 'o') {
               addAndGetNextChar();
               if (mCurrChar == 'n') {
                  addAndGetNextChar();
                  if (mCurrChar == 't') {
                     addAndGetNextChar();
                     if (mCurrChar == 'i') {
                        addAndGetNextChar();
                        if (mCurrChar == 'n') {
                           addAndGetNextChar();
                           if (mCurrChar == 'u') {
                              addAndGetNextChar();
                              if (mCurrChar == 'e') {
                                 addAndGetNextChar();
                                 return scanIdentifier(T_CONTINUE);
                              }
                           }
                        }
                     }
                  }
               }
            }
            return scanIdentifier(T_IDENTIFIER);
         }
         case 'd': // def
         {
            addAndGetNextChar();
            if (mCurrChar == 'e') {
               addAndGetNextChar();
               if (mCurrChar == 'f') {
                  addAndGetNextChar();
                  return scanIdentifier(T_DEF);
               }
            }
            return scanIdentifier(T_IDENTIFIER);
         }
         case 'e': // else end
         {
            addAndGetNextChar();
            switch (mCurrChar) {
               case 'l':
                  addAndGetNextChar();
                  if (mCurrChar == 's') {
                     addAndGetNextChar();
                     if (mCurrChar == 'e') {
                        addAndGetNextChar();
                        return scanIdentifier(T_ELSE);
                     }
                  }
                  break;
               case 'n':
                  addAndGetNextChar();
                  if (mCurrChar == 'd') {
                     addAndGetNextChar();
                     return scanIdentifier(T_END);
                  }
                  break;
            }
            return scanIdentifier(T_IDENTIFIER);
         }
         case 'f': // for false
         {
            addAndGetNextChar();
            switch (mCurrChar) {
               case 'a':
                  addAndGetNextChar();
                  if (mCurrChar == 'l') {
                     addAndGetNextChar();
                     if (mCurrChar == 's') {
                        addAndGetNextChar();
                        if (mCurrChar == 'e') {
                           addAndGetNextChar();
                           return scanIdentifier(T_FALSE);
                        }
                     }
                  }
                  break;
               case 'o':
                  addAndGetNextChar();
                  if (mCurrChar == 'r') {
                     addAndGetNextChar();
                     return scanIdentifier(T_FOR);
                  }
                  break;
            }
            return scanIdentifier(T_IDENTIFIER);
         }
         case 'i': // if in
         {
            addAndGetNextChar();
            if (mCurrChar == 'f') {
               addAndGetNextChar();
               return scanIdentifier(T_IF);
            }
            return scanIdentifier(T_IDENTIFIER);
         }
         case 'n': // not new
         {
            addAndGetNextChar();
            switch (mCurrChar) {
               case 'o':
                  addAndGetNextChar();
                  if (mCurrChar == 't') {
                     addAndGetNextChar();
                     return scanIdentifier(T_NOT);
                  }
                  break;
               case 'e':
                  addAndGetNextChar();
                  if (mCurrChar == 'w') {
                     addAndGetNextChar();
                     return scanIdentifier(T_NEW);
                  }
                  break;
            }
            return scanIdentifier(T_IDENTIFIER);
         }
         case 'o': // or
         {
            addAndGetNextChar();
            if (mCurrChar == 'r') {
               addAndGetNextChar();
               return scanIdentifier(T_OR);
            }
            return scanIdentifier(T_IDENTIFIER);
         }
         case 'r': //return
         {
            addAndGetNextChar();
            if (mCurrChar == 'e') {
               addAndGetNextChar();
               if (mCurrChar == 't') {
                  addAndGetNextChar();
                  if (mCurrChar == 'u') {
                     addAndGetNextChar();
                     if (mCurrChar == 'r') {
                        addAndGetNextChar();
                        if (mCurrChar == 'n') {
                           addAndGetNextChar();
                           return scanIdentifier(T_RETURN);
                        }
                     }
                  }
               }
            }
            return scanIdentifier(T_IDENTIFIER);
         }
         case 't': // true
         {
            addAndGetNextChar();
            if (mCurrChar == 'r') {
               addAndGetNextChar();
               if (mCurrChar == 'u') {
                  addAndGetNextChar();
                  if (mCurrChar == 'e') {
                     addAndGetNextChar();
                     return scanIdentifier(T_TRUE);
                  }
               }
            }
            return scanIdentifier(T_IDENTIFIER);
         }
         case 'w': // while
         {
            addAndGetNextChar();
            if (mCurrChar == 'h') {
               addAndGetNextChar();
               if (mCurrChar == 'i') {
                  addAndGetNextChar();
                  if (mCurrChar == 'l') {
                     addAndGetNextChar();
                     if (mCurrChar == 'e') {
                        addAndGetNextChar();
                        return scanIdentifier(T_WHILE);
                     }
                  }
               }
            }
            return scanIdentifier(T_IDENTIFIER);
         }
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
            addAndGetNextChar();
            return scanIdentifier(T_IDENTIFIER);
            break;

         default:
            error();
      }
      getNextChar();
   }
   return T_EOS;
}

void Lexer::error() const {
   throw LexicalError(mLineNumber, mColumn, mCurrChar);
}

char Lexer::getEscapeCharacter() const {
   switch (mCurrChar) {
      case 'n':return '\n';
      case 't':return '\t';
      case '\\':
      case '\'':
      case '\"':
         return mCurrChar;
      default:
         error();
   }
   return 0;
}