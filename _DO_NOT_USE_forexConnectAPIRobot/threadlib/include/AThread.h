/* Copyright 2011 Forex Capital Markets LLC

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use these files except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
*/

#pragma once

#if !defined(WIN32) && !defined(PTHREADS)
#define PTHREADS
#endif

#ifdef PTHREADS
#include <pthread.h>
#endif

#include "Mutex.h"

#ifndef INFINITE
#define INFINITE        0xFFFFFFFF    // Infinite timeout
#endif
#define THREAD_OBJECT_HAS_DELETED 0xFFFFFFFE
namespace threading
{

class AThread
{
 public:

    /** Class constructor

    Construct class, not start an associated new thread.
    New thread starts from run() method.

    */
    AThread();
    virtual ~AThread();

    virtual bool start();

    virtual bool join(unsigned long dwWaitMilliseconds = INFINITE);

    void requestStop()
    { 
        m_isStopRequested = true; 
    }

    bool isStopRequested() const
    { 
        return m_isStopRequested; 
    }

    bool isRunning() const
    {
        Mutex::Lock lock(m_hLock);
#ifdef PTHREADS
        return mRunning;
#else
        return mThreadID != 0; 
#endif
    }
    /** Check the current threa or not*/
    bool isCurrentThread();
protected:

    void resetRunning()
    {
#ifdef PTHREADS
    mRunning = false;
#else
    mThreadID = 0;
#endif
    }

    virtual int run(void *) = 0;
#ifdef PTHREADS
    static void *threadRunner(void *);
#else
    static unsigned int WINAPI threadRunner(void *);
#endif

#ifdef PTHREADS
    pthread_t m_oThread;
    bool mRunning;
#else
    unsigned int mThreadID;
    HANDLE m_hThread;
#endif  
    
    mutable Mutex m_hLock;
    volatile bool m_isStopRequested;
};

} //namespace
