#include "stdafx.h"
#include "BrowserEmulationSet.h"

/// <summary>
/// 为应用程序的WebBrowser默认使用IE内核进行更改
/// </summary>

/// <summary>
/// IE WebBrowser内核设置
/// </summary>
void CIEVersion::BrowserEmulationSet()
{
	DWORD type = REG_DWORD;
	TCHAR szProcessPath[255] = { 0 };
	DWORD dwInfoSize = 255 * sizeof(TCHAR);
	::GetModuleFileName(NULL, szProcessPath, dwInfoSize);
	//当前程序名称
	CString strProcessName = PathFindFileName(szProcessPath);
	
	HKEY hKey = NULL;
	LPCTSTR pSubKey = _T("SOFTWARE\\Microsoft\\Internet Explorer\\MAIN\\FeatureControl\\FEATURE_BROWSER_EMULATION");
	//IE注册表信息
	long ret = 0;
	ret = RegOpenKeyEx(HKEY_CURRENT_USER,
		pSubKey, 0, KEY_READ | KEY_WRITE,
		&hKey);
	if(ret == ERROR_FILE_NOT_FOUND)
	{
		ret = ::RegCreateKey(HKEY_CURRENT_USER,pSubKey,&hKey);
		if(ret == ERROR_SUCCESS)
		{
		}
	}
	if (ret == ERROR_SUCCESS)
	{

		LONG ret = RegQueryValueEx(hKey, strProcessName.GetBuffer(), NULL, &type, NULL, NULL);
		if (ret != ERROR_SUCCESS)
		{
			DWORD version = IeVersionEmulation(IeVersion());
			::RegSetValueEx(hKey, strProcessName.GetBuffer(), NULL, REG_DWORD, (BYTE*)&version, sizeof(DWORD));
		}
	}
	RegCloseKey(hKey);
}

/// <summary>
/// IE版本号
/// </summary>
/// <returns></returns>
int CIEVersion::IeVersion()
{
	//IE版本号
	int version = 11;
	HKEY hKey = NULL;
	LPCTSTR pSubKey = _T("SOFTWARE\\Microsoft\\Internet Explorer");

	if (RegOpenKeyEx(HKEY_LOCAL_MACHINE,
		pSubKey, 0, KEY_READ,
		&hKey) == ERROR_SUCCESS)
	{
		DWORD type = REG_SZ;
		TCHAR KeyInfo[255] = { 0 };
		DWORD dwInfoSize = 255 * sizeof(TCHAR);
		//更新的IE版本
		do{
			LONG ret = RegQueryValueEx(hKey, _T("svcVersion"), NULL, &type, (LPBYTE)KeyInfo, &dwInfoSize);
			if (ret == ERROR_SUCCESS)
			{
				_stscanf_s(KeyInfo, _T("%d"), &version);
				break;
			}
			//默认版本
			ret = RegQueryValueEx(hKey, _T("Version"), NULL, &type, (LPBYTE)KeyInfo, &dwInfoSize);
			if (ret == ERROR_SUCCESS)
			{
				_stscanf_s(KeyInfo,_T("%d"), &version);
			}
		}while(0);
	}
	RegCloseKey(hKey);
	return version;
}

/// <summary>
/// 根据IE版本号 返回Emulation值
/// </summary>
/// <param name="ieVersion"></param>
/// <returns></returns>
int CIEVersion::IeVersionEmulation(int ieVersion)
{
	//IE7 7000 (0x1B58)
	if (ieVersion < 8)
	{
		return 0;
	}
	if (ieVersion == 8)
	{
		return 0x1F40;//8000 (0x1F40)、8888 (0x22B8)
	}
	if (ieVersion == 9)
	{
		return 0x2328;//9000 (0x2328)、9999 (0x270F)
	}
	else if (ieVersion == 10)
	{
		return 0x02710;//10000 (0x02710)、10001 (0x2711)
	}
	else if (ieVersion == 11)
	{
		return 0x2AF8;//11000 (0x2AF8)、11001 (0x2AF9
	}
	return 0;
}