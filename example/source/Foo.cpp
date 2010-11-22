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
