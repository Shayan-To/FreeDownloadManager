/*
  Free Download Manager Copyright (c) 2003-2016 FreeDownloadManager.ORG
*/

#include "stdafx.h"
#include "fdm.h"
#include "vmsAppTipsMgr.h"
#include "mfchelp.h"
#include "vmsStringParser.h"
#include "vmsAppUtil.h"
#include "vmsSecurity.h"
#include "FdmApp.h"
#include "MainFrm.h"
#include "vmsLogger.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

vmsAppSmallTipsMgr::vmsAppSmallTipsMgr()
{
	m_nCurrentTip = -1;
}

vmsAppSmallTipsMgr::~vmsAppSmallTipsMgr()
{

}

tstring vmsAppSmallTipsMgr::getTip()
{
	vmsAUTOLOCKSECTION (m_csTipAcc);
	if (m_vTips.empty ())
		return _T ("");
	if (m_nCurrentTip == -1)
		SetupCurrentTipIndex ();
	return m_vTips [m_nCurrentTip].c_str ();
}

vmsAppSmallTipsMgr& vmsAppSmallTipsMgr::o()
{
	static vmsAppSmallTipsMgr o;
	
	
	static bool _bLoaded = false;
	if (!_bLoaded)
	{
		_bLoaded = true;
		o.Load ();
	}
	return o;
}

std::unique_ptr <CStdioFile> mfc_open_text_file (const tstring& name)
{
	
	auto file = open_text_file (name);
	std::unique_ptr <CStdioFile> result;
	if (file)
		result.reset (new CStdioFile (file));
	return result;
}

void vmsAppSmallTipsMgr::Load_imp(LPCTSTR ptszFile, std::vector <tstring> &vTips, bool bIgnoreServiceInformation)
{
	vTips.clear ();

	try
	{
		auto file = mfc_open_text_file (ptszFile);
		if (!file)
			return;

		CString strLine;
		tstring tip;
		bool b1stLine = true;
		bool bForceShow = false;
		int iForceTipIndex = -1;

		while (file->ReadString (strLine) && strLine.IsEmpty () == FALSE)
		{
			if (b1stLine)
			{
				if (strLine.GetLength () >= 3 && BYTE (strLine [0]) == 0xEF && BYTE (strLine [1]) == 0xBB && BYTE (strLine [2]) == 0xBF)
					strLine.Delete (0, 3); 
				b1stLine = false;
				if (strLine.IsEmpty ())
					continue;
			}

			if (strLine [0] == ';')
				continue; 

#ifndef UNICODE
			vmsUtf8ToAscii (strLine.GetBuffer (strLine.GetLength ()));
			strLine.ReleaseBuffer ();
			tip = (LPCTSTR)strLine;
#endif

			if (_tcsnicmp (tip.c_str(), _T ("ForceShow"), 9) == 0)
			{
				if (bIgnoreServiceInformation)
					continue;

				LPCTSTR ptsz = tip.c_str();
				ptsz += 9;
				vmsStringParser::SkipWhiteChars (ptsz);
				if (*ptsz == '=')
				{
					ptsz++;
					vmsStringParser::SkipWhiteChars (ptsz);
					tstring tstr;
					vmsStringParser::GetWord (ptsz, tstr);
					if (!tstr.empty ())
					{
						CString strOld = _App.SmallTips_ForceShow ();
						if (strOld.CompareNoCase (tstr.c_str ()))
						{
							_App.SmallTips_Show (TRUE);
							_App.SmallTips_ForceShow (tstr.c_str ());
							bForceShow = true;
						}
					}
				}

				continue;
			}
			else if (_tcsnicmp (tip.c_str(), _T ("ForceTipIndex"), 13) == 0)
			{
				if (bIgnoreServiceInformation)
					continue;

				LPCTSTR ptsz = tip.c_str();
				ptsz += 13;
				vmsStringParser::SkipWhiteChars (ptsz);
				if (*ptsz == '=')
				{
					ptsz++;
					vmsStringParser::SkipWhiteChars (ptsz);
					tstring tstr;
					vmsStringParser::GetWord (ptsz, tstr);
					if (!tstr.empty ())
						iForceTipIndex = _ttoi (tstr.c_str ());
				}

				continue;
			}

			vTips.push_back (tip);
		}

		if (!bIgnoreServiceInformation && bForceShow && iForceTipIndex != -1)
		{
			_App.SmallTips_CurrentTip (iForceTipIndex);
			FILETIME ft; ZeroMemory (&ft, sizeof (ft));
			_App.SmallTips_LastTime (ft);
		}

	}
	catch (const std::exception& ex)
	{
		ASSERT (FALSE);
		vmsLogger::WriteLog(_T("vmsAppSmallTipsMgr::Load_imp ") + tstringFromString(ex.what()));
	}
	catch (...)	{}
}

void vmsAppSmallTipsMgr::Load()
{
	vmsAUTOLOCKSECTION (m_csTipAcc);

	m_vTips.clear ();
	std::vector <tstring> vEngTips;

	Load_imp (fsGetDataFilePath (_T("tips.dat")), m_vTips, false);
	Load_imp (fsGetProgramFilePath (_T("tips.dat")), vEngTips, !m_vTips.empty ());	

	if (vEngTips.size () > m_vTips.size ())
	{
		for (size_t i = m_vTips.size (); i < vEngTips.size (); i++)
			m_vTips.push_back (vEngTips [i]);
	}
}

bool vmsAppSmallTipsMgr::UpdateTipsFile()
{
	CString strUrl;
	CString strLng = _LngMgr.GetLngFileNameWoExt (_LngMgr.GetCurLng ());
	strUrl.Format (_T("%stips/%s.dat"), (LPCTSTR)_App.Update_URL (), (LPCTSTR)strLng);
	CString strFile = fsGetDataFilePath (_T("tips.bak"));
	fsInternetResult ir = vmsAppUtil::UrlDownloadToFile (strUrl, strFile);
	if (ir != IR_SUCCESS && strLng.CompareNoCase (_T("eng")) != 0)
	{
		strUrl = _App.Update_URL ();
		strUrl += _T("tips/eng.dat");
		ir = vmsAppUtil::UrlDownloadToFile (strUrl, strFile);
	}
	if (ir != IR_SUCCESS)
		return false;
	CString strKeyFile = ((CFdmApp*)AfxGetApp ())->m_strAppPath;
	strKeyFile += _T("sigkey.dat");
	if (GetFileAttributes (strKeyFile) == DWORD (-1))
		strKeyFile = fsGetProgramFilePath (_T("sigkey.dat"));
	if (false == vmsSecurity::VerifySign (strFile, strKeyFile))
	{
		DeleteFile (strFile);
		return false;
	}
	if (false == vmsSecurity::ExtractFileFromSignedFile (strFile, fsGetDataFilePath (_T("tips.dat"))))
	{
		ASSERT (false);
		return false; 
	}
	DeleteFile (strFile);
	BOOL b = _App.SmallTips_Show ();
	EnterCriticalSection (m_csTipAcc);
	Load ();
	SetupCurrentTipIndex ();
	LeaveCriticalSection (m_csTipAcc);
	if (b == FALSE && _App.SmallTips_Show ())
		((CMainFrame*)AfxGetApp ()->m_pMainWnd)->ApplyShowSmallTipsSetting ();
	return true;
}

bool vmsAppSmallTipsMgr::isDisabledTipIndex (int nIndex) const
{
	return nIndex == 1 || nIndex == 3;
}

void vmsAppSmallTipsMgr::SetupCurrentTipIndex()
{
	int nCurrentTip = _App.SmallTips_CurrentTip ();
	FILETIME ft = _App.SmallTips_LastTime ();
	FILETIME ftCurrent; SYSTEMTIME st;
	GetLocalTime (&st); SystemTimeToFileTime (&st, &ftCurrent);
	if (ft.dwHighDateTime == 0)
	{
		_App.SmallTips_LastTime (ftCurrent);
	}
	else
	{
		int nDelta = fsGetTimeDelta (&ftCurrent, &ft);
		if (nDelta > 24*3600)
		{
			nCurrentTip++;
			_App.SmallTips_CurrentTip (nCurrentTip);
			_App.SmallTips_LastTime (ftCurrent);
		}
	}
	if (nCurrentTip < 0 || nCurrentTip >= (int)m_vTips.size ())
	{
		nCurrentTip = 0;
		_App.SmallTips_CurrentTip (nCurrentTip);
	}
	m_nCurrentTip = nCurrentTip;
	if (isDisabledTipIndex (m_nCurrentTip))
	{
		while (isDisabledTipIndex (++m_nCurrentTip) && m_nCurrentTip != nCurrentTip)
		{
			if (m_nCurrentTip >= m_vTips.size ())
				m_nCurrentTip = -1;
		}
		assert (m_nCurrentTip != nCurrentTip);
		_App.SmallTips_CurrentTip (m_nCurrentTip);
	}
}
