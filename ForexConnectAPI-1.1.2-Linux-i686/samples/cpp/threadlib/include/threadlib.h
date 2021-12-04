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

#ifndef WIN32

#ifndef PTHREADS
#define PTHREADS
#endif

#ifndef PTHREADS_MUTEX
#define PTHREADS_MUTEX
#endif

#endif

#include "Interlocked.h"
#include "winEmul.h"
#include "AThread.h"
#include "Mutex.h"
