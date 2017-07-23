#pragma once

#include <directui\Core\UIManager.h>
#include "TouchUtils.h"
#include <map>
#include <memory>
namespace DirectUI {
	class CFontLoader
	{
	public:
		CFontLoader()
		{
			LPCTSTR path = _T("\\skin\\font");

			TCHAR pszAppData[MAX_PATH] = {0};

			//default confige
			::GetModuleFileName(NULL, pszAppData, MAX_PATH);
			PathRemoveFileSpec(pszAppData);
			_tcscat(pszAppData,path);
			//StringCbCat(pszAppData,MAX_PATH,path);
			CString strpath = pszAppData;
			strpath+=_T("\\*.*");
			{
				WIN32_FIND_DATA fileData;
				HANDLE hFind = FindFirstFile(strpath, &fileData);
				if (hFind != INVALID_HANDLE_VALUE)
				{
					do 
					{
						if (_tcsicmp(fileData.cFileName, _T(".")) == 0 || _tcsicmp(fileData.cFileName, _T("..")) == 0)
						{
							continue;
						}
						else if (fileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
						{
							continue;
						}
						else
						{
							CString data(pszAppData);
							data +=_T("\\");
							data +=fileData.cFileName;
							LoadFromFile(data.GetBuffer());
						}
					} while (FindNextFile(hFind, &fileData));
				}
				::FindClose(hFind);
			}

		}
		~CFontLoader()
		{
			for(auto iter = m_hFontLoaded.begin(); m_hFontLoaded.end() != iter;++iter)
			{
				if(*iter != NULL || *iter != INVALID_HANDLE_VALUE)
					RemoveFontMemResourceEx(*iter);
			}
		}
	private:
		void LoadFromFile(LPCTSTR lpFileName)
		{
			HANDLE hFile = CreateFile(lpFileName,GENERIC_READ,FILE_SHARE_READ,0,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,0);
			if(hFile != INVALID_HANDLE_VALUE)
			{
				DWORD cdSize = ::GetFileSize(hFile,0);
				BYTE* pBuffer = new BYTE[cdSize];
				::ZeroMemory(pBuffer,cdSize);
				DWORD cdRead = 0;
				if(::ReadFile(hFile,pBuffer,cdSize,&cdRead,0))
				{
					assert(cdSize == cdRead);
					AddFont(pBuffer,cdSize);
				}
				delete [] pBuffer;
				CloseHandle(hFile);
			}
		}
		//file buffer & size of byte
		//return: count of font installed
		DWORD AddFont(void* pvData, DWORD cbSize){
			HANDLE hFontLoad = NULL;
			DWORD nFontsInstalled = 0;
			hFontLoad = AddFontMemResourceEx(pvData,cbSize,0,&nFontsInstalled);
			if(hFontLoad != INVALID_HANDLE_VALUE)
			{
				m_hFontLoaded.push_back(hFontLoad);
			}
			return nFontsInstalled;
		}

		std::vector<HANDLE> m_hFontLoaded;
	};

	//TEventUI 的wKeyState标明是非是主触控点
	typedef enum EVENTTYPE_TOUCH_UI
	{
		UIEVENT__TOUCHBEGIN = UIEVENT__LAST,
		//在主触控点的UIEVENT_TAP_DOWN消息
		//总是比UIEVENT_BUTTONDOWN快一步出现
		UIEVENT_TAP_DOWN,
		UIEVENT_TAP_UP,
		UIEVENT_TAP,
		UIEVENT_DBTAP,
		UIEVENT_PAN,
		UIEVENT_ROTAE,
		UIEVENT_ZOOM,
		UIEVENT_PRESSHOLD,
		UIEVENT__TOUCHEND
	};

	class CTouchManagerUI
		: public CPaintManagerUI,
		IHandleTouchInput
	{
	public:
		CTouchManagerUI(void);
		~CTouchManagerUI(void);

		bool IsMultiTouchMode() const;
		void EnableMultiTouch(bool bEnable);


		bool MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lRes);
	public:
		virtual bool HandleTouchInput(__in POINT ptScreen, PTOUCHINPUT pIn);
		virtual bool HandleGestureInput(PGESTUREINFO pIn);
		std::unique_ptr<TouchUtils> m_uniTouchUtils;
	protected:
		bool TouchMessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lRes);

		struct TouchInfo
		{
			CControlUI* pControl;
			POINT oldTouchPt;
			TouchInfo():pControl(NULL)
			{
				ZeroMemory(&oldTouchPt,sizeof(POINT));
			}
		};
		typedef DWORD TOUCHID;
		std::map<TOUCHID,TouchInfo> m_mapTouchFocus;
		BOOL m_bMultiTouch;
		CFontLoader m_load;
	};
}