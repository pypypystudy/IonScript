#ifndef FOO_H
#define FOO_H

#include "IonScript/IonScript.h"

#include <string>
#include <vector>

class Boo;

class Foo : public ion::script::IManageableObject {
public:
   Foo (const std::string& name, size_t childrenCount);
   virtual ~Foo ();
   const std::string& getName () const {
      return mName;
   }

   void sayHello () const;
   Boo* getBoo (size_t index) {
      return mChildren[index];
   }

   Boo* createBoo ();

private:
   std::string mName;
   std::vector<Boo*> mChildren;
};

#endif
