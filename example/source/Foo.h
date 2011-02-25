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

#ifndef FOO_H
#define FOO_H

#include "IonScript/IonScript.h"

#include <string>
#include <vector>

class Boo;

class Foo : public ionscript::IManageableObject {
public:
   Foo(const std::string& name, size_t childrenCount);
   virtual ~Foo();
   const std::string& getName() const {
      return mName;
   }

   void sayHello() const;
   Boo* getBoo(size_t index) {
      return mChildren[index];
   }

   Boo* createBoo();

private:
   std::string mName;
   std::vector<Boo*> mChildren;
};

#endif
