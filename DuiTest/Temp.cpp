#include "stdafx.h"
#include "Temp.h"
#define SKINFILE  _T("temp.xml");
#define SKINFOLDER _T("skin");
LPCTSTR CTemp::GetWindowClassName() const
{
	return _T("TempUI");
}

CUIString CTemp::GetSkinFile()
{
	return SKINFILE;
}

CUIString CTemp::GetSkinFolder()
{
	return SKINFOLDER;
}