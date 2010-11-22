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
