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

#ifndef BOO_H
#define BOO_H

#include <iostream>

class Foo;

class Boo {
public:
   Boo(const Foo* pFoo, int ID) {
      mID = ID;
      mpFoo = pFoo;
   }
   virtual ~Boo() {
      std::cout << "Boo " << mID << " of Foo " << mpFoo->getName() << " deleted.\n";
   }
   void sayHello() const {
      std::cout << "Hello from Boo " << mID << " of Foo " << mpFoo->getName() << ".\n";
   }

private:
   const Foo* mpFoo;
   int mID;
};

#endif
