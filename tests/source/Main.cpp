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
 
#include "Timer.h"

#include <IonScript/IonScript.h>

#include <sys/types.h>
#include <dirent.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <vector>

using namespace std;
using namespace ionscript;

int getdir (string dir, vector<string> &files) {
   DIR *dp;
   struct dirent *dirp;
   if ((dp = opendir(dir.c_str())) == NULL) {
      cout << "Error opening " << dir << endl;
      return -1;
   }

   while ((dirp = readdir(dp)) != NULL) {
      string file = string(dirp->d_name);
      if (file.find(".is") != string::npos)
         files.push_back(file);
   }
   closedir(dp);
   return 0;
}

void centerstring (const char* s, size_t linesize = 80) {
   int l = strlen(s);
   int pos = (int) ((linesize - l) / 2);
   for (int i = 0; i < pos; i++)
      cout << " ";
   cout << s;
   if (strlen(s) % 2) ++pos;
   for (int i = 0; i < pos; i++)
      cout << " ";
}

int main (int argc, char** argv) {
   Timer total;
   total.reset();

   vector<string> files;
   getdir("scripts", files);

   Timer timer;
   VirtualMachine vm;

   double compileDuration, execDuration;
   bool error = false;
   for (size_t i = 0; i < files.size(); i++) {
      try {
         vector<char> bytecode;
         ifstream ifs(("scripts/" + files[i]).c_str());

         cout << "********************************************************************************\n";
         cout << "***";
         centerstring(files[i].c_str(), 74);
         cout << "***\n";
         cout << "********************************************************************************\n";
         cout << ">> Compiling " << "...";
         timer.reset();
         vm.compile(ifs, bytecode);
         compileDuration = timer.getDuration() * 1000;
         cout << "done! (size: " << bytecode.size() << " bytes)" << endl;

//         BytecodeReader r(&bytecode[0]);
//         r.print(std::cout);

         cout << ">> Executing..." << endl;
         timer.reset();
         vm.run(&bytecode[0]);
         execDuration = timer.getDuration() * 1000;
         cout << ">> Terminated.\n\tCompilation duration: " << compileDuration << " ms\n\tExecution duration: " << execDuration << " ms.\n";
         cout << "\n";

      } catch (exception & e) {
         cout << endl << "XXX " << e.what() << endl;
         error = true;
      }
   }

   cout << ">> Total time: " << (int) (total.getDuration()*1000) << " ms." << endl;
   if (error) {
      cout << "\tSome errors occurred." << endl;
      return -1;
   }

}
