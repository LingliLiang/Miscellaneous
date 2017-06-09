#include "stdafx.h"
#include "DlgUtils.h"

#include <Shellapi.h>
#include <shlobj.h>
#include <CommDlg.h>

namespace DlgUtils
{

	bool InvalidNameString(CString & strName)
	{
		CString strTemp = strName;
		strTemp.Replace(_T(" "),_T(""));
		if(strTemp.IsEmpty())
			return true;
		TCHAR invalue[10] = { '\\' , '/' , ':' , '*' , '?' , '"' , '<' , '>' , '|' , '\0' };
		TCHAR* pvalue = invalue;
		while(*pvalue)
		{
			if(strName.Find(*pvalue) != -1)
				return true;
			pvalue++;
		}
		return false;
	}

	struct BrowseInfoParam
	{
		CString& vaildData;
	};

	int CALLBACK BrowseCallbackProc(HWND hwnd, UINT uMsg, LPARAM lParam, LPARAM dwData)
	{
		BrowseInfoParam* pbip =  reinterpret_cast<BrowseInfoParam*>(dwData);
		switch(uMsg)
		{
		case BFFM_INITIALIZED:
			{
			}
			break;
		case BFFM_IUNKNOWN:
			break;
		case BFFM_SELCHANGED:
			{
			}
			break;
		case BFFM_VALIDATEFAILED:
			{
				//Ä¿Â¼·Ç·¨
				pbip->vaildData.Format(_T("%s"),lParam);
				//::SendMessage(hwnd,BFFM_SETSTATUSTEXT,0,(LPARAM)(LPTSTR)(LPCTSTR)strTip);
			}
			return 0;
		}
		return 0;
	}

	bool DlgGetOpenFileName(CString& strFileFullPath,CString& strFileNameDotExt,
		LPCTSTR strFilter, HWND hWnd /*= NULL*/,
		CString strTitle, CString strDefExt /*= CString()*/, CString initPath /*= CString()*/)
	{
		bool ret = false;
		TCHAR szFile[MAX_PATH] = {0};
		TCHAR szFileTitle[MAX_PATH] = {0};
		TCHAR szInitialDir[MAX_PATH] = {0};
		if(!initPath.IsEmpty())
		{
			_tcscpy(szFile,initPath.GetBuffer());
		}
		OPENFILENAME ofn = {0};
		ofn.lStructSize = sizeof(OPENFILENAME);
		ofn.hwndOwner = hWnd;
		ofn.Flags = OFN_OVERWRITEPROMPT | OFN_EXPLORER;
		ofn.lpstrFilter = strFilter;
		ofn.lpstrFile = szFile;
		ofn.lpstrFileTitle = szFileTitle;
		if(!strTitle.IsEmpty())
			ofn.lpstrTitle = strTitle.GetBuffer();
		ofn.nMaxFile = MAX_PATH;
		ofn.nMaxFileTitle = MAX_PATH;
		if(ofn.lpstrFilter)
			ofn.nFilterIndex = 1;
		if(!strDefExt.IsEmpty())
			ofn.lpstrDefExt = strDefExt.GetBuffer();
		if(!SHGetSpecialFolderPath(hWnd,szInitialDir,CSIDL_DESKTOP,0))
			_tcscpy(szInitialDir,_T("C:\\"));
		ofn.lpstrInitialDir = szInitialDir;

		if(GetOpenFileName(&ofn))
		{
			strFileFullPath = ofn.lpstrFile;
			strFileNameDotExt = ofn.lpstrFileTitle;
			ret = true;
		}
		return ret;
	}

	bool DlgGetSaveFileName(CString& strFile, CString& strFileNameDotExt,
		LPCTSTR strFilter, HWND hWnd /*= NULL*/,
		CString strTitle, CString strDefExt /*= CString()*/, CString initPath /*= CString()*/)
	{
		bool ret = false;
		TCHAR szFile[MAX_PATH] = {0};
		TCHAR szFileTitle[MAX_PATH] = {0};
		TCHAR szInitialDir[MAX_PATH] = {0};
		if(!initPath.IsEmpty())
		{
			_tcscpy(szFile,initPath.GetBuffer());
		}
		OPENFILENAME ofn = {0};
		ofn.lStructSize = sizeof(OPENFILENAME);
		ofn.hwndOwner = hWnd;
		ofn.Flags = OFN_OVERWRITEPROMPT | OFN_EXPLORER;
		ofn.lpstrFilter = strFilter;
		ofn.lpstrFile = szFile;
		ofn.lpstrFileTitle = szFileTitle;
		if(!strTitle.IsEmpty())
			ofn.lpstrTitle = strTitle.GetBuffer();
		ofn.nMaxFile = MAX_PATH;
		ofn.nMaxFileTitle = MAX_PATH;
		if(ofn.lpstrFilter)
			ofn.nFilterIndex = 1;
		if(!strDefExt.IsEmpty())
			ofn.lpstrDefExt = strDefExt.GetBuffer();
		if(!SHGetSpecialFolderPath(hWnd,szInitialDir,CSIDL_DESKTOP,0))
			_tcscpy(szInitialDir,_T("C:\\"));
		ofn.lpstrInitialDir = szInitialDir;

		if(GetSaveFileName(&ofn))
		{
			strFile = ofn.lpstrFile;
			strFileNameDotExt = ofn.lpstrFileTitle;
			ret = true;
		}
		return ret;
	}

	bool DlgBrowseForFolder(
		CString& strPath, //
		CString& strEditCtx,
		CString strTitle,
		HWND hWnd ,
		BOOL UseEdit)
	{
		bool result = false;
		OleInitialize(NULL);

		CString vldata;
		BrowseInfoParam bip={vldata};
		BROWSEINFO bi = {0};
		TCHAR pszPath[MAX_PATH];
		memset(pszPath, 0, MAX_PATH*sizeof(TCHAR));
		TCHAR pszDisplayName[MAX_PATH];
		memset(pszDisplayName, 0, MAX_PATH*sizeof(TCHAR));

		LPITEMIDLIST pidl = NULL;
		bi.hwndOwner = hWnd;
		bi.pidlRoot = NULL;
		bi.pszDisplayName = pszDisplayName;
		bi.lpszTitle = strTitle.GetBuffer();
		bi.ulFlags = BIF_NEWDIALOGSTYLE |
			BIF_EDITBOX |
			BIF_RETURNONLYFSDIRS |
			BIF_STATUSTEXT|
			BIF_VALIDATE;
		
		bi.lpfn = BrowseCallbackProc;
		bi.lParam = reinterpret_cast<LPARAM>(&bip);
		bi.iImage = 0;

		pidl = SHBrowseForFolder(&bi);
		if (pidl == NULL)  
		{  
			return result;  
		}  
		if (SHGetPathFromIDList(pidl, pszPath))  
		{  
			CoTaskMemFree(pidl);
			strPath = pszPath;
			if(UseEdit)
			{
				if(!InvalidNameString(vldata))
				{
					strEditCtx = vldata;
				}
				else
				{
					strEditCtx = pszDisplayName;
					if(InvalidNameString(strEditCtx))
						strEditCtx = _T("New");
				}
			}
			
			result = true;
		}
		::OleUninitialize();
		return result;
	}

	bool DlgCopyReplace(CString strFileSrc, CString strFileDst)
	{
		int reCode = 0;
		/////SHell copy file
		SHFILEOPSTRUCT fileOp = {
			NULL, FO_COPY, strFileSrc.GetBuffer(), strFileDst.GetBuffer() ,0,
			FALSE, NULL, 0};
			//Returns zero if successful; otherwise nonzero. Applications normally should simply check for zero or nonzero. 
			reCode = SHFileOperation(&fileOp);
			return reCode==0;
	}

}