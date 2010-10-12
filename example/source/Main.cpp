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

#include "Timer.h"

#include <script/VirtualMachine.h>
#include <script/SyntaxTree.h>
#include <script/Bytecode.h>
#include <script/FunctionCallManager.h>

#include <sys/types.h>
#include <dirent.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <vector>

using namespace std;
using namespace ion::script;

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


         cout << ">> Executing..." << endl;
         timer.reset();
         vm.run(bytecode);
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
