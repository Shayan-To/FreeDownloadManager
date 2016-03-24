/*
  Free Download Manager Copyright (c) 2003-2016 FreeDownloadManager.ORG
*/

#pragma once

#include <assert.h>
#include <string>
#include <vector>
#include <windows.h>
#include "../../common/vms_sifdm_cl/vmsCriticalSection.h"

class vmsHttpRedirectList
{
public:
	struct Redirect
	{
		std::wstring wstrUrl;
		std::wstring wstrOriginalUrl;
		DWORD dwTicksRegistered;
	};
public:
	vmsHttpRedirectList(void);
	virtual ~vmsHttpRedirectList(void);

protected:
	std::vector <Redirect> m_vRedirects;
	vmsCriticalSection m_csRedirects;
public:
	void addRedirect(const Redirect& obj);
protected:
	void RemoveTooOldItems(void);
public:
	int findRedirectIndex(LPCWSTR pwszUrl);
	const Redirect* getRedirect(int nIndex);
	void Lock(bool bLock);
	static vmsHttpRedirectList& o(void);
};

