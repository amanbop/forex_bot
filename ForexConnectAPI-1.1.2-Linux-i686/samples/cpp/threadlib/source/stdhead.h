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

#ifdef WIN32

#ifndef _WIN32_WINNT        // Allow use of features specific to Windows XP or later.                   
#define _WIN32_WINNT 0x0501 // Change this to the appropriate value to target other versions of Windows.
#endif                      

#ifndef PTHREADS
#define WIN32_LEAN_AND_MEAN     // Exclude rarely-used stuff from Windows headers
#include <windows.h>

#endif

#else

#define PTHREADS
#define PTHREADS_MUTEX

#endif
