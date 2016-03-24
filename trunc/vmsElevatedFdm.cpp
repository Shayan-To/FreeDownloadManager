/*
  Free Download Manager Copyright (c) 2003-2016 FreeDownloadManager.ORG
*/

#include "stdafx.h"
#include "vmsElevatedFdm.h"
#include "DlgElevateRequired.h"

vmsElevatedFdm::vmsElevatedFdm(void)
	: m_bInstallIeIntegrationPerformed(false)
{
}

vmsElevatedFdm::~vmsElevatedFdm(void)
{
}

bool vmsElevatedFdm::Run(LPCTSTR ptszAdditionalArgs, bool bWaitForComplete, bool bShowPreUacDlg)
{
	TCHAR tsz [MAX_PATH];
	GetModuleFileName (NULL, tsz, _countof (tsz));
	
	SHELLEXECUTEINFO sei;
	ZeroMemory (&sei, sizeof (sei));
	sei.cbSize = sizeof (sei);
	sei.fMask = SEE_MASK_FLAG_NO_UI | SEE_MASK_NOCLOSEPROCESS;
	sei.lpVerb = bShowPreUacDlg ? _T("runas") : _T("open");
	sei.lpFile = tsz;
	tstring tstr = _T ("-runelevated");
	if (ptszAdditionalArgs && *ptszAdditionalArgs)
	{
		tstr += ' ';
		tstr += ptszAdditionalArgs;
	}
	sei.lpParameters = tstr.c_str ();
	sei.nShow = SW_HIDE;
	
	bool bOK = ShellExecuteEx (&sei) != FALSE;

	if (bOK && bWaitForComplete)
		WaitForSingleObject (sei.hProcess, INFINITE);

	if (bOK)
		CloseHandle (sei.hProcess);

	return bOK;
}

vmsElevatedFdm& vmsElevatedFdm::o(void)
{
	static vmsElevatedFdm _o;
	return _o;
}

bool vmsElevatedFdm::InstallIeIntegration(bool bShowPreUacDlg)
{
	if (m_bInstallIeIntegrationPerformed)
		return false;

	if (bShowPreUacDlg)
	{
		CDlgElevateRequired dlg;
		dlg.m_strMsg = LS (L_IE_INTEGRATION_REQ_ADMIN);
		if (IDOK != dlg.DoModal ())
			return false;
	}

	if (Run (_T ("-ie")))
		m_bInstallIeIntegrationPerformed = true;

	return m_bInstallIeIntegrationPerformed;
}

bool vmsElevatedFdm::InstallChromeIntegration(bool bShowPreUacDlg)
{
	if (bShowPreUacDlg)
	{
		wchar_t wsz [10000];
		wsprintf (wsz, LS (L_ADM_RIGHTS_REQ_TO_INTEGRATE_INTO), L"Google Chrome");
		CDlgElevateRequired dlg;
		dlg.m_strMsg = wsz;
		if (IDOK != dlg.DoModal())
			return false;
	}

	return Run(_T("-chrome"), true, bShowPreUacDlg);
}

bool vmsElevatedFdm::InstallIntegration (const std::vector <vmsKnownBrowsers::Browser> &vBrowsers, bool bInstall, bool bShowPreUacDlg)
{
	if (bShowPreUacDlg)
	{
		CDlgElevateRequired dlg;
		dlg.m_strMsg = LS (L_ELREQ_MONITORING);
		if (_DlgMgr.DoModal (&dlg) != IDOK)
			return false;
	}

	tstring tstrArgs = bInstall ? _T ("-installIntegration=") : _T ("-deinstallIntegration=");
	bool bNeedComma = false;
	for (size_t i = 0; i < vBrowsers.size (); i++, bNeedComma = true)
	{
		TCHAR tsz [40];
		_itot (vBrowsers [i], tsz, 10);
		if (bNeedComma)
			tstrArgs += ',';
		tstrArgs += tsz;
	}

	return Run (tstrArgs.c_str (), true);
}