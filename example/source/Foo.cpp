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

#include "Foo.h"
#include "Boo.h"

#include <iostream>

using namespace std;

Foo::Foo (const string& name, size_t childrenCount) {
	mName = name;
	for (size_t i = 0; i < childrenCount; i++)		
		mChildren.push_back(new Boo(this, i));
	cout << "Foo " << mName << " created.\n";
}

Foo::~Foo() {
	for (size_t i = 0; i < mChildren.size(); i++)
		delete mChildren[i];
	cout << "Foo " << mName << " deleted.\n";
}

void Foo::sayHello() const {
	cout << "Foo " << mName << " says hello!\n";
}

Boo* Foo::createBoo() {
	mChildren.push_back(new Boo(this, mChildren.size()));
	return mChildren.back();
}
