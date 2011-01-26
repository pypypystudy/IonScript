/***************************************************************************
 *   IonScript                                                             *
 *   Copyright (C) 2010 by Canio Massimo Tristano                          *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   As a special exception, if other files instantiate templates or use   *
 *   macros or inline functions from this file, or you compile this file   *
 *   and link it with other works to produce a work based on this file,    *
 *   this file does not by itself cause the resulting work to be covered   *
 *   by the GNU General Public License. However the source code for this   *
 *   file must still be made available in accordance with the GNU General  *
 *   Public License. This exception does not invalidate any other reasons  *
 *   why a work based on this file might be covered by the GNU General     *
 *   Public License.                                                       *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, see <http://www.gnu.org/licenses/>   *
 ***************************************************************************/

#include <IonScript/IonScript.h>
#include <fstream>
#include <iostream>
#include <exception>

using namespace std;
using namespace ion::script;

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
