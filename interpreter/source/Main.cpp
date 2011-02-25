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

#include <IonScript/IonScript.h>
#include <fstream>
#include <iostream>
#include <exception>

using namespace std;
using namespace ionscript;

int main(int argc, char** argv) {
    bool opTree = false;
    string filename = "";

    for (int i = 1; i < argc; i++) {
        if (argv[i][0] == '-') {
            switch (argv[i][1]) {
                case 't': // Print the tree 
                    opTree = true;
                    break;
            }
        } else
            filename = string(argv[i]);
    }
    
    try { 
        SyntaxTree tree;
        vector<char> bytecode;
        ifstream ifs(filename.c_str());

        VirtualMachine vm;

        vm.compile(ifs, bytecode, tree);

        if (opTree)
            tree.dump(std::cout);

        vm.run(&bytecode[0]);

    } catch (std::exception &e) {
        std::cerr << e.what() << "\n";
    }
}
