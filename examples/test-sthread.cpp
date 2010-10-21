// Copyright (c) 2010 Ryan Seal <rlseal -at- gmail.com>
//
// This file is part of SThread Software.
//
// SThread is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//  
// SThread is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with SThread.  If not, see <http://www.gnu.org/licenses/>.
#include<iostream>
#include<sthread/Lock.hpp>
#include<sthread/SThread.hpp>

using namespace std;

//Simple demonstration of SThread
class Test : public SThread {
   string name_;
   int num_;
   Mutex mutex_;
   bool exit_;

   public:
   Test(std::string name, const int& num): name_(name), num_(num), exit_(false){}

   void Run(){
      while(!exit_) {
         mutex_.Lock();
         cout << num_ ;
         cout.flush();
         mutex_.Unlock();
         this->Sleep(sthread::usec, 100000);
      }
   }

   void SetNum(int num) { num_ = num; }
   void Exit() { exit_ = true;}
};

int main(int argc, char** argv)
{
   string input;
   //create two instances of class Test
   Test test1("thread1",1);
   Test test2("thread2",2);
   cout << "This simple example illustrates two threads, each containing unique integers, interacting with the scheduler." << endl;
   cout << "Please hit <g> to begin and <q> to quit" << endl;

   while(input[0] != 'g') { input.clear(); cin >> input;}

   //crank up both threads
   test1.Start();
   test2.Start();
   
   while(true){
      input.clear();
      cin >> input;
      //when user wants to stop, set exit_=true and jump out of each thread's Run() member
      if(input[0] == 'q'){ 
      test1.Exit(); 
      test2.Exit(); 
      break;
      }
   }

   //make sure threads have finished up before exiting program
   test2.Wait();
   test1.Wait();

   return 0;
};
