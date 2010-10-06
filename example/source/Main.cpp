/*
 * Sandbox
 * Copyright Canio Massimo Tristano <massimo.tristano@gmail.com>
 * All rights reserved.
 */
#include <Timer.h>

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
      cout << "Error(" << errno << ") opening " << dir << endl;
      return errno;
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
   if (error)
      cout << "\tSome errors occurred." << endl;
}
