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

#ifndef ION_SCRIPT_TIMER_H
#define	ION_SCRIPT_TIMER_H

#ifdef WIN32
#include <windows.h>
#else
#ifdef __linux__
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
#ifdef WIN32
            QueryPerformanceFrequency(&mFreq);
            reset();
#endif
         }

         virtual ~Timer () { }
         
         double getDuration () const {
#ifdef WIN32
            LARGE_INTEGER now;
            QueryPerformanceCounter(&now);
            return (double) (now.QuadPart - mStartTime.QuadPart) / mFreq.QuadPart;
#endif
#ifdef __linux__
				struct timeval now;
				gettimeofday(&now,0);
				return (double)(now.tv_sec - mStartTime.tv_sec) + (double)(now.tv_usec - mStartTime.tv_usec) / 1000000.0;
#endif
         }
         
         void reset () {
         	#ifdef WIN32
            QueryPerformanceCounter(&mStartTime);
            #endif
            #ifdef __linux__
            gettimeofday(&mStartTime, 0);
            #endif
         }

      private:
#ifdef WIN32
         LARGE_INTEGER mFreq;
         LARGE_INTEGER mStartTime;
#endif
#ifdef __linux__
			struct timeval mStartTime;
#endif
      };

#endif	/* ION_SCRIPT_TIMER_H */

