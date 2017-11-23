#pragma once

#include <tchar.h>
#include <Windows.h>
/// <summary>
/// 为应用程序的WebBrowser默认使用IE内核进行更改
/// </summary>
class CIEVersion
{
public:
	/// <summary>
	/// IE WebBrowser内核设置
	/// </summary>
	void BrowserEmulationSet();

	/// <summary>
	/// IE版本号
	/// </summary>
	/// <returns></returns>
	int IeVersion();

	/// <summary>
	/// 根据IE版本号 返回Emulation值
	/// </summary>
	/// <param name="ieVersion"></param>
	/// <returns></returns>
	int IeVersionEmulation(int ieVersion);
};