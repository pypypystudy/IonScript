/*
 * Ion-Engine
 * Copyright Canio Massimo Tristano <massimo.tristano@gmail.com>
 * All rights reserved.
 */

#ifndef ION_TIMER_H
#define	ION_TIMER_H

#ifdef WIN
#include <windows.h>
#else
#ifdef LINUX
#include <sys/time.h>
#else
#error "Timer in this platform is not implemented yet."
#endif
#endif


      /**
       * A high precision timer.
       */
      class Timer {
      public:
         Timer () {
#ifdef WIN
            QueryPerformanceFrequency(&mFreq);
            reset();
#endif
         }

         virtual ~Timer () { }
         
         double getDuration () const {
#ifdef WIN
            LARGE_INTEGER now;
            QueryPerformanceCounter(&now);
            return (double) (now.QuadPart - mStartTime.QuadPart) / mFreq.QuadPart;
#endif
#ifdef LINUX
				struct timeval now;
				gettimeofday(&now,0);
				return (double)(now.tv_sec - mStartTime.tv_sec) + (double)(now.tv_usec - mStartTime.tv_usec) / 1000000.0;
#endif
         }
         
         void reset () {
         	#ifdef WIN
            QueryPerformanceCounter(&mStartTime);
            #endif
            #ifdef LINUX
            gettimeofday(&mStartTime, 0);
            #endif
         }

      private:
#ifdef WIN
         LARGE_INTEGER mFreq;
         LARGE_INTEGER mStartTime;
#endif
#ifdef LINUX
			struct timeval mStartTime;
#endif
      };

#endif	/* ION_TIMER_H */

