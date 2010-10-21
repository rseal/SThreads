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
#ifndef STHREAD_H
#define STHREAD_H

#include <iostream>
#include <time.h>
#include <errno.h>
#include <pthread.h>
#include <sthread/Lock.hpp>
#include <map>

namespace sthread{
   enum { nsec, usec, msec, sec};
   const static long ONE_E0=1L;
   const static long ONE_E3=1000L;
   const static long ONE_E6=1000000L;
   const static long ONE_E9=1000000000L;
};

class SThread
{
   std::map< long, double > unitMap_;

   public:

      SThread() {  
         pthread_mutex_init(&mutex_,NULL);
         pthread_attr_init(&attr_);
         pthread_cond_init(&condition_, NULL);

         unitMap_[ sthread::nsec ] = sthread::ONE_E9;
         unitMap_[ sthread::usec ] = sthread::ONE_E6;
         unitMap_[ sthread::msec ] = sthread::ONE_E3;
         unitMap_[ sthread::sec  ] = sthread::ONE_E0;
      }

      virtual ~SThread(){
      }

      virtual void Run()=0;

      static void* Init(void* _this){
         SThread* p_object = reinterpret_cast<SThread*>(_this);
         p_object->Run();
         return NULL;
      }

      void Start(){
         int status=-1;
         status = pthread_create(&p_sthread_, NULL, Init, this);
         if(status < 0)
            std::cerr << "STHREAD: thread creation failed" << std::endl;
      }    

      void Wait(){ pthread_join(p_sthread_, NULL);}

      void Detach(){pthread_detach(p_sthread_);}

      void Destroy(){
         int status;
         pthread_exit(reinterpret_cast<void*>(&status));
      }

      void Lock(pthread_mutex_t& mutex){
         pthread_mutex_lock(&mutex);
      }

      void Unlock(pthread_mutex_t& mutex){
         pthread_mutex_unlock(&mutex);
      }

      void Pause(){
         ScopedPThreadLock Lock(mutex_);
         pthread_cond_wait(&condition_, &mutex_);
      }

      void Wake(){ pthread_cond_signal(&condition_);}

      void Sleep ( int type = sthread::usec, long value = sthread::ONE_E3 ) {

         double time = value;
         double multiplier = unitMap_.find( type )->second;

         fTime_.tv_sec = 0;
         fTime_.tv_nsec = 0;

         while( time >= multiplier ){ 
            time -= multiplier;
            fTime_.tv_sec += 1;
         }

         fTime_.tv_nsec = sthread::ONE_E9*time/multiplier;

         ScopedPThreadLock lock ( mutex_ );
         nanosleep(&fTime_, NULL);
      }

      void Priority(int _value);
      void SetCondition(int _value);

   protected:
      pthread_t p_sthread_;
      pthread_mutex_t mutex_;
      pthread_attr_t attr_;
      pthread_cond_t condition_;
      timespec cTime_;
      timespec fTime_;
};

#endif
