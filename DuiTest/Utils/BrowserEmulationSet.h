#pragma once

#include <tchar.h>
#include <Windows.h>
/// <summary>
/// ΪӦ�ó����WebBrowserĬ��ʹ��IE�ں˽��и���
/// </summary>
class CIEVersion
{
public:
	/// <summary>
	/// IE WebBrowser�ں�����
	/// </summary>
	void BrowserEmulationSet();

	/// <summary>
	/// IE�汾��
	/// </summary>
	/// <returns></returns>
	int IeVersion();

	/// <summary>
	/// ����IE�汾�� ����Emulationֵ
	/// </summary>
	/// <param name="ieVersion"></param>
	/// <returns></returns>
	int IeVersionEmulation(int ieVersion);
};