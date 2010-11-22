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

#ifndef ION_SCRIPT_TIMER_H
#define	ION_SCRIPT_TIMER_H

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

#endif	/* ION_SCRIPT_TIMER_H */

