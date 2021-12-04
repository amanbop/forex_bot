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

#include "stdhead.h"
#include "../include/AThread.h"
#ifndef PTHREADS
#include <process.h>    /* _beginthread, _endthread */
#endif

using namespace threading;

//------------------------------------------------------------------------------------------
AThread::AThread() 
        : m_isStopRequested(false) 
{
#ifndef PTHREADS
    m_hThread = 0;
#endif
    resetRunning();
}

bool AThread::start()
{
    if (isRunning())
        return true;
    {
    Mutex::Lock d(m_hLock);
#ifdef PTHREADS
    pthread_create(&m_oThread, NULL, &threadRunner, this);
    mRunning = true;
#else
    if (m_hThread)
        ::CloseHandle(m_hThread);

    m_hThread = (HANDLE)_beginthreadex(NULL, 0, threadRunner, this,
        0, &mThreadID);
    if (m_hThread == (void *)-1L)
    {
        m_hThread = 0;
        resetRunning();
        return false;
    }
#endif
    }
    return true;
}

//------------------------------------------------------------------------------------------
AThread::~AThread()
{
    if (!join(30000))
#ifdef PTHREADS
    #ifdef ANDROID
        ;
    //TODO:
    #else
        pthread_cancel(m_oThread);
    #endif
#else
        {
            Mutex::Lock d(m_hLock);
            if (m_hThread)
            {
                ::SuspendThread(m_hThread);
                ::CloseHandle(m_hThread);
                m_hThread = 0;
                resetRunning();
            }
        }
#endif
}

bool AThread::join(unsigned long dwWaitMilliseconds /*= INFINITE*/)
{
#ifdef PTHREADS
    {
        Mutex::Lock d(m_hLock);        
        // Thread not started
        if (!mRunning)
            return true;
        requestStop();
        if (pthread_equal(m_oThread, pthread_self()))
            return true;
    }

    void *pStatus = NULL;
    pthread_join(m_oThread, &pStatus);
    // TODO: Timed join
    return true;
#else 
    {
        Mutex::Lock d(m_hLock);        
        // Thread not started
        if (mThreadID == 0)
            return true;
        requestStop();
        DWORD dwExitCode = 0;
        
        if (!GetExitCodeThread(m_hThread, &dwExitCode))
            return true;
        if (dwExitCode != STILL_ACTIVE) 
            return true; // thread already terminated, so nothing to join.

        if (mThreadID == ::GetCurrentThreadId())
            return true;
    }

    bool bRes = (::WaitForSingleObject(m_hThread, dwWaitMilliseconds) == WAIT_OBJECT_0);
    if (!bRes)
        return false;
    else
    {
        Mutex::Lock d(m_hLock);

        if (m_hThread)
        {
            ::CloseHandle(m_hThread);
            m_hThread = 0;
        }
        mThreadID = 0;
    }
    return true;
#endif
}

/** Check the current threa or not*/
bool AThread::isCurrentThread()
{
#ifdef PTHREADS
    Mutex::Lock d(m_hLock);        
    // Thread not started
    return pthread_equal(m_oThread, pthread_self());
#else
    Mutex::Lock d(m_hLock);        
    return mThreadID == ::GetCurrentThreadId();
#endif
}


#ifdef PTHREADS
void *AThread::threadRunner(void *pPtr)
#else
unsigned int WINAPI AThread::threadRunner(void *pPtr)
#endif
{
    AThread *pObj = (AThread*)pPtr;
    int res = pObj->run(pPtr);
#ifdef PTHREADS
    #ifdef ANDROID
    //TODO:
    #else
    pthread_testcancel();
    #endif
#endif
    if (res != (int)THREAD_OBJECT_HAS_DELETED) // May be better to modify 'int run()' to 'usigned int run()'
    {
        Mutex::Lock d(pObj->m_hLock);
        pObj->m_isStopRequested = false;
        pObj->resetRunning();       
    }
    return 0;
}

