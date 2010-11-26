#include "Foo.h"
#include "Boo.h"

#include <IonScript/IonScript.h>
#include <iostream>
#include <fstream>

using namespace std;
using namespace ion::script;

enum {
   CID_FOO_NEW,
   CID_FOO_AND_BOO_SAY_HELLO,
   CID_FOO_GET_BOO,
   CID_FOO_CREATE_BOO,
};

static void moduleFunctions (const FunctionCallManager& manager) {
   switch (manager.getFunctionID()) {
      case CID_FOO_NEW:
         manager.returnObject(
            new Foo(manager.getArgument(0).getStringSafely(), manager.getArgument(1).getPositiveIntegerSafely()),
            true);
         return;

      case CID_FOO_AND_BOO_SAY_HELLO:
      {
         if (manager.getArgument(0).checkObjectType(typeid (Foo)))
            manager.getArgument(0).getObject<Foo > ()->sayHello();
         else {
            manager.assertArgumentObjectType(0, typeid (Boo));
            manager.getArgument(0).getObject<Boo > ()->sayHello();
         }
         break;
      }

      case CID_FOO_GET_BOO:
         manager.returnObject(
            manager.getArgument(0).getObjectSafely<Foo > ()->getBoo(manager.getArgument(1).getIntegerSafely()),
            false);
         return;

      case CID_FOO_CREATE_BOO:
         manager.returnObject(
            manager.getArgument(0).getObjectSafely<Foo > ()->createBoo(),
            false);
         return;
   }
   manager.returnNil();
}

int main (int argc, char** argv) {
   try {
      ifstream ifs("script.is");

      VirtualMachine vm;

      HostFunctionGroupID hfgID = vm.registerHostFunctionGroup(moduleFunctions);
      vm.setFunction("new_Foo", hfgID, CID_FOO_NEW, 2);
      vm.setFunction("sayHello", hfgID, CID_FOO_AND_BOO_SAY_HELLO, 1);
      vm.setFunction("getBoo", hfgID, CID_FOO_GET_BOO, 2);
      vm.setFunction("createBoo", hfgID, CID_FOO_CREATE_BOO, 1);


      vm.compileAndRun(ifs);
      
   } catch (exception& e) {
      cerr << e.what() << endl;
      return -1;
   }
   return 0;
}

