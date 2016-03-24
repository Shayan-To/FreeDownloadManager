/*
  Free Download Manager Copyright (c) 2003-2016 FreeDownloadManager.ORG
*/

#if !defined(AFX_VMSMAGNETEXTENSION_H__027AF6AC_9BCC_4ED2_BF22_2168E5F6E080__INCLUDED_)
#define AFX_VMSMAGNETEXTENSION_H__027AF6AC_9BCC_4ED2_BF22_2168E5F6E080__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif 

class vmsMagnetExtension  
{
public:
	struct AssociationParameters
	{
		tstring tstrShellOpenCmdLine;
		bool bUrlProtocolSpecified;
		tstring tstrUrlProtocol;
	};
public:
	static BOOL CheckAccessRights ();
	
	static BOOL AssociateWith (const AssociationParameters& ap);
	static AssociationParameters GetCurrentAssociation();
	static BOOL IsAssociatedWithUs();
	
	static BOOL IsAssociationExist();
	
	static BOOL Associate();

	vmsMagnetExtension();
	virtual ~vmsMagnetExtension();

public:
	
	static fsString get_ShellOpenCommandLine();
};

#endif 
