/*
  Free Download Manager Copyright (c) 2003-2016 FreeDownloadManager.ORG
*/

#if !defined(AFX_VMSDATAFILE_H__2D3A0382_A38B_44AA_963E_6B7E3B3024DB__INCLUDED_)
#define AFX_VMSDATAFILE_H__2D3A0382_A38B_44AA_963E_6B7E3B3024DB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif 

#include "tree.h"
#include "common/vmsFile.h"

#define FDM_DATA_FILE_CURRENT_VERSION	(1)

#define FDM_DATA_FILE_SIG L"FDM Data File  "

struct vmsDataFileHeader
{
	WCHAR wszSig [sizeof (FDM_DATA_FILE_SIG) / sizeof(WCHAR)];
	WORD wVer;

	vmsDataFileHeader ()
	{
		memset(wszSig, 0, sizeof (FDM_DATA_FILE_SIG));
		_tcscpy (wszSig, FDM_DATA_FILE_SIG);
		wVer = FDM_DATA_FILE_CURRENT_VERSION;
	}
};

enum vmsVariantValueType
{
	VVT_EMPTY,			
	VVT_INT,			
	VVT_DOUBLE,			
	VVT_INT64,			
	VVT_ASTRING,		
	VVT_LPBYTE,			
};

struct vmsVariantValue
{
protected:
	vmsVariantValueType enType; 
	union {
		int iVal;			
		double fVal;		
		__int64 i64Val;		
		LPTSTR pszVal;		
		struct {
			LPBYTE pbVal;	
			UINT nByteBufferSize;	
		};
	};

public:

	vmsVariantValue () {enType = VVT_EMPTY;}
	vmsVariantValue (const vmsVariantValue& vt) {enType = VVT_EMPTY; *this = vt;}
	
	~vmsVariantValue () {clear ();}

	vmsVariantValueType type () {return enType;};
	bool empty () {return type () == VVT_EMPTY;};

	operator int () const {ASSERT (enType == VVT_INT); return iVal;}
	operator double () const {ASSERT (enType == VVT_DOUBLE); return fVal;}
	operator __int64 () const {ASSERT (enType == VVT_INT64); return i64Val;}
	operator LPCTSTR () const {ASSERT (enType == VVT_ASTRING); return pszVal;}
	operator const LPBYTE () const {ASSERT (enType == VVT_LPBYTE); return pbVal;}
	operator LPBYTE () {ASSERT (enType == VVT_LPBYTE); return pbVal;}
	UINT bytebuffersize () {return nByteBufferSize;}

	void set (int i) {clear (); enType = VVT_INT; iVal = i;}
	void set (__int64 i) {clear (); enType = VVT_INT64; i64Val = i;}
	void set (double f) {clear (); enType = VVT_DOUBLE; fVal = f;}
	void set (LPCTSTR psz) {
		clear (); enType = VVT_ASTRING; pszVal = new TCHAR [(_tcslen (psz) + 1) * sizeof(TCHAR)];
		_tcscpy (pszVal, psz);
	}
	void set (const LPBYTE pb, UINT nSize) {
		clear (); enType = VVT_LPBYTE; pbVal = new BYTE [nSize]; 
		CopyMemory (pbVal, pb, nSize);
		nByteBufferSize = nSize;
	}

	int operator = (int i) {set (i); return i;}
	__int64 operator = (__int64 i) {set (i); return i;}
	double operator = (double f) {set (f); return f;}
	LPCTSTR operator = (LPCTSTR psz) {set (psz); return psz;}

	void clear () {
		switch (enType) 
		{
		case VVT_EMPTY:
		case VVT_INT:
		case VVT_DOUBLE:
		case VVT_INT64: break;

		case VVT_LPBYTE: delete [] pbVal; break;
		case VVT_ASTRING: delete [] pszVal; break;

		default: ASSERT (false);	
		}
		enType = VVT_EMPTY;
	}

	vmsVariantValue& operator = (const vmsVariantValue& vt) {
		switch (vt.enType)
		{
		case VVT_EMPTY: clear (); break;
		case VVT_INT: set ((int)vt); break;
		case VVT_DOUBLE: set ((double)vt); break;
		case VVT_INT64: set ((__int64)vt); break;
		case VVT_ASTRING: set ((LPCTSTR)vt); break;
		case VVT_LPBYTE: set ((const LPBYTE)vt, vt.nByteBufferSize); break;
		default: ASSERT (false); 
		}
		return *this;
	}
};

struct vmsDataFileItem
{
	fsString strName;	
	vmsVariantValue vt;	
};

typedef fs::ListTree <vmsDataFileItem> *LPDATAFILETREE;

typedef vmsVariantValue DATAFILEITEM, *LPDATAFILEITEM;

class vmsDataFile
{
public:
	bool get_Value (LPCTSTR pszSection, LPCTSTR pszValueName, LPBYTE& pbValue, UINT& nValueSize);
	bool get_Value (LPCTSTR pszSection, LPCTSTR pszValueName, LPCTSTR& strValue);
	bool get_Value (LPCTSTR pszSection, LPCTSTR pszValueName, double& fValue);
	bool get_Value (LPCTSTR pszSection, LPCTSTR pszValueName, __int64& i64Value);
	bool get_Value (LPCTSTR pszSection, LPCTSTR pszValueName, int& iValue);
	void set_Value(LPCTSTR pszSection, LPCTSTR pszValueName, LPBYTE pbValue, UINT nValueSize);
	void set_Value(LPCTSTR pszSection, LPCTSTR pszValueName, LPCTSTR pszValue);
	void set_Value(LPCTSTR pszSection, LPCTSTR pszValueName, double fValue);
	void set_Value(LPCTSTR pszSection, LPCTSTR pszValueName, __int64 i64Value);
	void set_Value (LPCTSTR pszSection, LPCTSTR pszValueName, int iValue);
	
	LPDATAFILEITEM CreateItem(LPCTSTR pszSection, LPCTSTR pszItemName);
	LPDATAFILEITEM CreateItem (LPDATAFILETREE pSection, LPCTSTR pszItemName);
	LPDATAFILETREE CreateSection (LPCTSTR pszSection, LPDATAFILETREE ptRoot = NULL);
	
	void LoadFromFile (HANDLE hFile);
	void LoadFromFile_old (HANDLE hFile);
	void SaveToFile (HANDLE hFile);
	void SaveToBuffer(LPBYTE& pbtCurrentPos, LPBYTE pbtBuffer, DWORD dwBufferSize, DWORD* pdwRequiredSize);
	void LoadFromBuffer(LPBYTE& pbtCurrentPos, LPBYTE pbtBuffer, DWORD dwBufferSizeSize);

	vmsDataFile();
	virtual ~vmsDataFile();

protected:
	void LoadFromFile_old(vmsFDM::vmsFile& file, vmsDataFileItem &item);
	void LoadFromFile(vmsFDM::vmsFile& file, vmsDataFileItem &item);
	void LoadFromFile(vmsFDM::vmsFile& file, LPDATAFILETREE ptRoot);
	void LoadFromFile_old(vmsFDM::vmsFile& file, LPDATAFILETREE ptRoot);
	void SaveToFile(vmsFDM::vmsFile& file, vmsDataFileItem& item);
	void SaveToFile(vmsFDM::vmsFile& file, LPDATAFILETREE ptRoot);
	
	
	
	
	LPDATAFILETREE FindItem (LPCTSTR pszSection, LPCTSTR pszValueName, LPDATAFILETREE ptRoot = NULL);
	void SaveToBuffer(LPBYTE& pbtCurrentPos, LPBYTE pbtBuffer, DWORD dwBufferSize, DWORD* pdwRequiredSize, vmsDataFileItem &item);
	void SaveToBuffer(LPBYTE& pbtCurrentPos, LPBYTE pbtBuffer, DWORD dwBufferSize, DWORD* pdwRequiredSize, LPDATAFILETREE ptRoot);
	bool LoadFromBuffer(LPBYTE& pbtCurrentPos, LPBYTE pbtBuffer, DWORD dwBufferSizeSize, LPDATAFILETREE ptRoot);
	bool LoadFromBuffer(LPBYTE& pbtCurrentPos, LPBYTE pbtBuffer, DWORD dwBufferSizeSize, vmsDataFileItem &item);

	
	fs::ListTree <vmsDataFileItem>::ListTreePtr m_tData;
};

#endif 
