#include "StdAfx.h"
//#define _CRTDBG_MAP_ALLOC
#include<crtdbg.h>

#ifdef _DEBUG
	//#include <vld.h>
#endif

#include "TestWin.h"

int APIENTRY _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	HRESULT Hr = ::OleInitialize(NULL);
	if( FAILED(Hr) ) return 0;
	CPaintManagerUI::SetInstance(hInstance);
	std::auto_ptr<CTestWin> pWin(new CTestWin);
	pWin->Create(NULL,_T("TestWin"),UI_WNDSTYLE_FRAME,UI_WNDSTYLE_EX_FRAME);
	pWin->CenterWindow();
	pWin->ShowWindow();
	CPaintManagerUI::MessageLoop();
	CPaintManagerUI::Term();
	::OleUninitialize();
	//_CrtDumpMemoryLeaks();
	return 0;
}

		