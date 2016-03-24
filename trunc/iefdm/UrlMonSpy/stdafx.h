/*
  Free Download Manager Copyright (c) 2003-2016 FreeDownloadManager.ORG
*/

#pragma once

#define WIN32_LEAN_AND_MEAN             

#define _WIN32_WINNT _WIN32_WINNT_WINXP
#define _WIN32_IE _WIN32_IE_IE70

#include <windows.h>
#include <DbgHelp.h>
#include <UrlMon.h>
#include <ObjBase.h>
#include <comdef.h>
#include <atlbase.h>
#include <assert.h>

_COM_SMARTPTR_TYPEDEF(IInternetProtocolEx, __uuidof(IInternetProtocolEx));
_COM_SMARTPTR_TYPEDEF(IUri, __uuidof(IUri));

#include <vector>
#include <string>
#include <memory>
#include <functional>
using namespace std;

#include "../../common/vms_sifdm_cl/base.h"

#ifdef _DEBUG
#define SCL_ENABLE
#endif

#include "../../Include.Add/vmsSourceCodeLogger.h"

