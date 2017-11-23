#include "stdafx.h"
#include "BrowserEmulationSet.h"

/// <summary>
/// ΪӦ�ó����WebBrowserĬ��ʹ��IE�ں˽��и���
/// </summary>

/// <summary>
/// IE WebBrowser�ں�����
/// </summary>
void CIEVersion::BrowserEmulationSet()
{
	DWORD type = REG_DWORD;
	TCHAR szProcessPath[255] = { 0 };
	DWORD dwInfoSize = 255 * sizeof(TCHAR);
	::GetModuleFileName(NULL, szProcessPath, dwInfoSize);
	//��ǰ��������
	CString strProcessName = PathFindFileName(szProcessPath);
	
	HKEY hKey = NULL;
	LPCTSTR pSubKey = _T("SOFTWARE\\Microsoft\\Internet Explorer\\MAIN\\FeatureControl\\FEATURE_BROWSER_EMULATION");
	//IEע�����Ϣ
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
/// IE�汾��
/// </summary>
/// <returns></returns>
int CIEVersion::IeVersion()
{
	//IE�汾��
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
		//���µ�IE�汾
		do{
			LONG ret = RegQueryValueEx(hKey, _T("svcVersion"), NULL, &type, (LPBYTE)KeyInfo, &dwInfoSize);
			if (ret == ERROR_SUCCESS)
			{
				_stscanf_s(KeyInfo, _T("%d"), &version);
				break;
			}
			//Ĭ�ϰ汾
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
/// ����IE�汾�� ����Emulationֵ
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
		return 0x1F40;//8000 (0x1F40)��8888 (0x22B8)
	}
	if (ieVersion == 9)
	{
		return 0x2328;//9000 (0x2328)��9999 (0x270F)
	}
	else if (ieVersion == 10)
	{
		return 0x02710;//10000 (0x02710)��10001 (0x2711)
	}
	else if (ieVersion == 11)
	{
		return 0x2AF8;//11000 (0x2AF8)��11001 (0x2AF9
	}
	return 0;
}