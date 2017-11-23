#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN  
#include <windows.h>

#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS 

#include <atlbase.h>
#include <atlstr.h>

#ifndef TRACE
#define TRACE ATLTRACE
#endif

#ifdef VLDINC
	#include <vld.h>
#endif

#include "directui\UIlib.h"
using namespace DirectUI;



#ifndef SAFE_DELETE_ARRAY
#define SAFE_DELETE_ARRAY(x) if (x){delete [] x; x=NULL;}
#endif

#ifndef SAFE_DELETE
#define SAFE_DELETE(x) if (x){delete x; x=NULL;}
#endif

#define WM_MOVINGNOTIFY WM_USER+0x8000