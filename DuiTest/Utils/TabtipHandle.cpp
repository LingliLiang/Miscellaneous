#include "stdafx.h"
#include "TabtipHandle.h"
#include <initguid.h>
#include <Objbase.h>
#include <Shobjidl.h>


////C:\Program Files\Common Files\microsoft shared\ink\tiptsf.dll   win10 
#include "tiptsf.tlh" 
using namespace PenInputPanelLib;

//1700 (VisualStudio 2012)
#if defined(_MSC_VER) && (_MSC_VER >= 1700)
#else
//declare in Shobjidl.h (Windows 8 [desktop apps only] ) 

// D5120AA3-46BA-44C5-822D-CA8092C1FC72
DEFINE_GUID(CLSID_FrameworkInputPane,
	0xD5120AA3, 0x46BA, 0x44C5, 0x82, 0x2D, 0xCA, 0x80, 0x92, 0xC1, 0xFC, 0x72);

// 226C537B-1E76-4D9E-A760-33DB29922F18
DEFINE_GUID(IID_IFrameworkInputPaneHandler,
	0x226C537B, 0x1E76, 0x4D9E, 0xA7, 0x60, 0x33, 0xDB, 0x29, 0x92, 0x2F, 0x18);

struct IFrameworkInputPaneHandler : public IUnknown
{
public:
	virtual HRESULT STDMETHODCALLTYPE Showing( 
		/* [in] */ __RPC__in RECT *prcInputPaneScreenLocation,
		/* [in] */ BOOL fEnsureFocusedElementInView) = 0;

	virtual HRESULT STDMETHODCALLTYPE Hiding( 
		/* [in] */ BOOL fEnsureFocusedElementInView) = 0;

};

// 5752238B-24F0-495A-82F1-2FD593056796
DEFINE_GUID(IID_IFrameworkInputPane,
	0x5752238B, 0x24F0, 0x495A, 0x82, 0xF1, 0x2F, 0xD5, 0x93, 0x05, 0x67, 0x96);

struct IFrameworkInputPane : public IUnknown
{
	virtual HRESULT STDMETHODCALLTYPE Advise( 
		/* [in] */ __RPC__in_opt IUnknown *pWindow,
		/* [in] */ __RPC__in_opt IFrameworkInputPaneHandler *pHandler,
		/* [out] */ __RPC__out DWORD *pdwCookie) = 0;

	virtual HRESULT STDMETHODCALLTYPE AdviseWithHWND( 
		/* [in] */ __RPC__in HWND hwnd,
		/* [in] */ __RPC__in_opt IFrameworkInputPaneHandler *pHandler,
		/* [out] */ __RPC__out DWORD *pdwCookie) = 0;

	virtual HRESULT STDMETHODCALLTYPE Unadvise( 
		/* [in] */ DWORD dwCookie) = 0;

	virtual HRESULT STDMETHODCALLTYPE Location( 
		/* [out] */ __RPC__out RECT *prcInputPaneScreenLocation) = 0;
};

#endif

//1910 (VisualStudio 2017) no idea, it's  declared the interface in vs2017
#if defined(_MSC_VER) && (_MSC_VER >= 1910)
#else
//win10 hide com interface declare, see more goto
//https://stackoverflow.com/questions/38774139/show-touch-keyboard-tabtip-exe-in-windows-10-anniversary-edition/40921638#40921638

// 4ce576fa-83dc-4F88-951c-9d0782b4e376
DEFINE_GUID(CLSID_UIHostNoLaunch,
	0x4CE576FA, 0x83DC, 0x4f88, 0x95, 0x1C, 0x9D, 0x07, 0x82, 0xB4, 0xE3, 0x76);

// 37c994e7_432b_4834_a2f7_dce1f13b834b
DEFINE_GUID(IID_ITipInvocation,
	0x37c994e7, 0x432b, 0x4834, 0xa2, 0xf7, 0xdc, 0xe1, 0xf1, 0x3b, 0x83, 0x4b);

struct ITipInvocation : IUnknown
{
	virtual HRESULT STDMETHODCALLTYPE Toggle(HWND wnd) = 0;
};
#endif


class Observer : public IFrameworkInputPaneHandler {
public:
	Observer() : ref_count_(0) {
	}
	virtual HRESULT STDMETHODCALLTYPE QueryInterface( 
		REFIID riid,
		void **ppvObject) {
			if (IsEqualIID(riid, IID_IUnknown)) {
				*ppvObject = static_cast<IUnknown*>(this);
			} else if (IsEqualIID(riid, IID_IFrameworkInputPaneHandler)) {
				*ppvObject = static_cast<IUnknown*>(this);
			} else {
				return E_NOINTERFACE;
			}
			AddRef();
			return S_OK;
	}

	virtual ULONG STDMETHODCALLTYPE AddRef() {
		return InterlockedIncrement(&ref_count_);
	}

	virtual ULONG STDMETHODCALLTYPE Release() {
		const LONG count = InterlockedDecrement(&ref_count_);
		if (count == 0) {
			delete this;
		}
		return count;
	}

	virtual HRESULT STDMETHODCALLTYPE Showing(
		RECT *prcInputPaneScreenLocation,
		BOOL fEnsureFocusedElementInView) {
			OutputDebugStringW(L"Showing");
			ATLTRACE(_T("RECT %d >> %d >> %d >> %d\n"),
				prcInputPaneScreenLocation->left,
				prcInputPaneScreenLocation->top,
				prcInputPaneScreenLocation->right,
				prcInputPaneScreenLocation->bottom
			);
			//TabtipKeyboardYPos(100);
			ATLTRACE(_T("%d fEnsureFocusedElementInView!\n"), fEnsureFocusedElementInView);
			return S_OK;
	}

	virtual HRESULT STDMETHODCALLTYPE Hiding(
		BOOL fEnsureFocusedElementInView) {
			OutputDebugStringW(L"Hiding\n");
			ATLTRACE(_T("%d fEnsureFocusedElementInView!\n"), fEnsureFocusedElementInView);
			return S_OK;
	}

private:
	volatile LONG ref_count_;
};

static const int desk_dpi = GetDeviceCaps(::GetWindowDC(::GetDesktopWindow()), LOGPIXELSY);

static HWND g_hTabtip = ::FindWindow(_TEXT("IPTip_Main_Window"), NULL);
static const int g_Threshold = 50; //ms
static BOOL g_bTabtipVisible = FALSE;
static DWORD g_tickCount = 0;

DWORD g_inputPaneCookie = 0;
CComPtr<IFrameworkInputPaneHandler> handler(new Observer());
CComPtr<IFrameworkInputPane> g_FrameworkinputPane;

void AdviseinputPane(HWND hWnd)
{
	HRESULT hr = S_OK;
	if (g_FrameworkinputPane)
	{
		if (g_inputPaneCookie)
		{
			UnAdviseinputPane();
		}
		hr = g_FrameworkinputPane->AdviseWithHWND(hWnd, handler, &g_inputPaneCookie);
		ATLTRACE(_T("%x Advise inputPane!\n"), hr);
	}
}

void UnAdviseinputPane()
{
	HRESULT hr = S_OK;
	if (g_inputPaneCookie && g_FrameworkinputPane)
	{
		hr = g_FrameworkinputPane->Unadvise(g_inputPaneCookie);
		g_inputPaneCookie = 0;
		ATLTRACE(_T("%x UnAdvise inputPane!\n"), hr);
	}
}

class IInputPaneClear : public IUnknown
{
public:
	IInputPaneClear()
		:ref_count_(0)
	{
		HRESULT hr = S_OK;
		hr = CoInitialize(NULL);
		if (!g_FrameworkinputPane)
		{
			hr = CoCreateInstance(CLSID_FrameworkInputPane, 0, CLSCTX_ALL, IID_IFrameworkInputPane, (LPVOID*)&g_FrameworkinputPane);
			if (SUCCEEDED(hr))
			{
				ATLTRACE(_T("g_FrameworkinputPane Setup!\n"));
			}
		}
	}
	~IInputPaneClear()
	{
		if(g_FrameworkinputPane)
			ATLTRACE(_T("g_FrameworkinputPane clear!\n"));
		UnAdviseinputPane();
		CoUninitialize();
	}

	virtual HRESULT STDMETHODCALLTYPE QueryInterface(
		REFIID riid,
		void **ppvObject) {
		if (IsEqualIID(riid, IID_IUnknown)) {
			*ppvObject = static_cast<IUnknown*>(this);
		}
		else {
			return E_NOINTERFACE;
		}
		AddRef();
		return S_OK;
	}

	virtual ULONG STDMETHODCALLTYPE AddRef() {
		return InterlockedIncrement(&ref_count_);
	}

	virtual ULONG STDMETHODCALLTYPE Release() {
		const LONG count = InterlockedDecrement(&ref_count_);
		if (count == 0) {
			delete this;
		}
		return count;
	}
private:
	volatile LONG ref_count_;
};

CComPtr<IInputPaneClear> g_inputPaneClear(new IInputPaneClear);

///Windows 8 [desktop apps only] Shobjidl.h
BOOL __cdecl IsVirtualKeyboardVisible()
{
	BOOL    bRet = FALSE;
	RECT    InputPaneScreenLocation = { 0, 0, 0, 0 };
	IFrameworkInputPane* IinputPane = NULL;
	HRESULT hr = S_OK;
	__try
	{
		//hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);

		if (SUCCEEDED(hr))
		{
			// http://msdn.microsoft.com/en-us/library/windows/desktop/hh706967(v=vs.85).aspx
			hr = CoCreateInstance(CLSID_FrameworkInputPane, 0, CLSCTX_ALL, IID_IFrameworkInputPane, (LPVOID*)&IinputPane);
			if(SUCCEEDED(hr))
			{
				IinputPane->Location(&InputPaneScreenLocation);

				if (InputPaneScreenLocation.bottom == 0 && InputPaneScreenLocation.left == 0 &&
					InputPaneScreenLocation.right == 0 && InputPaneScreenLocation.top == 0)
				{
					bRet = FALSE;// VKB is not visible
				}
				else
				{
					bRet = TRUE;// VKB is visible
				}
			}
		}

	}   // try
	__finally
	{
		if(IinputPane) IinputPane->Release();
		//CoUninitialize();
	}

	return bRet;
}

HWND TabtipWnd()
{
	return g_hTabtip;
}

BOOL IsTabtipVisible()
{
	g_hTabtip = ::FindWindow(_TEXT("IPTip_Main_Window"), NULL);
	auto win_style = GetWindowLongPtr(g_hTabtip, GWL_STYLE);
	
	//Keyboard is not visible - when "IPTIP_Main_Window" is NOT present
	if (g_hTabtip == 0 || win_style == 0)
	{
		return FALSE;
	}

	//Keyboard is not visible - when "IPTIP_Main_Window" is present but disabled
	if (win_style & WS_DISABLED)
	{
		return FALSE;
	}
	//Keyboard is not visible - when "IPTIP_Main_Window" is present but NOT disabled and NOT visible
	if (!(win_style & WS_VISIBLE) && !(win_style & WS_DISABLED) )
	{
		return FALSE;
	}
	//Keyboard is Visible - when "IPTIP_Main_Window" is present, NOT disabled and IS visible
	if ((win_style & WS_VISIBLE) && !(win_style & WS_DISABLED))
	{
		return TRUE;
	}
	return FALSE;
}

int TabtipVisible(BOOL bShow)
{
	HRESULT hr = S_OK;
	ITipInvocation* tip = NULL;
	//BOOL vkv = IsVirtualKeyboardVisible();
	BOOL vkv = IsTabtipVisible();
	//ATLTRACE(_T("TabtipVisible %d  To %d\n"),vkv,bShow);
	if(g_Threshold >= ::GetTickCount() - g_tickCount)
	{
		//Reenter to quick
		//ATLTRACE(_T("ReEnter g_bTabtipVisible %d ,now %d To %d\n"),g_bTabtipVisible,vkv,bShow);
		 if(g_bTabtipVisible == bShow) return 0;
	}
	else if(bShow == vkv) return 0;
	__try
	{
		//hr = CoInitialize(0);
		hr = CoCreateInstance(CLSID_UIHostNoLaunch, 0, CLSCTX_INPROC_HANDLER | CLSCTX_LOCAL_SERVER,
			IID_ITipInvocation, (void**)&tip);
		if(SUCCEEDED(hr))
		{
			tip->Toggle(GetDesktopWindow());
			g_bTabtipVisible = bShow;
			IDispatch* disp = NULL;
			hr = tip->QueryInterface(IID_IDispatch,(void**)&disp);
			if(SUCCEEDED(hr))
			{
				::OutputDebugString(L"ITipInvocation is IDispatch!\n");
				disp->Release();
			}
		}
		else
		{
			//ATLTRACE(_T("0x%x CreateInstance Failed\n"),hr);
		}
	}
	__finally
	{
		if(tip) tip->Release();
		g_tickCount = ::GetTickCount();
		//CoUninitialize();
	}
	return 0;
}

void TabtipTurnOff()
{
	g_hTabtip = ::FindWindow(_TEXT("IPTip_Main_Window"), NULL);
	::PostMessage(g_hTabtip, WM_SYSCOMMAND, SC_CLOSE, 0);
}


///HKEY_CURRENT_USER\SOFTWARE\Microsoft\TabletTip\1.7
void GetTabletTipKey(LPCTSTR key, DWORD& kVaule)
{
	HKEY hTabletTip = NULL;
	LSTATUS lres = 0;
	DWORD keyType = REG_DWORD;
	DWORD dwInfoSize = sizeof(kVaule);

	lres = RegOpenKeyEx(HKEY_CURRENT_USER, _T("SOFTWARE\\Microsoft\\TabletTip\\1.7"), 0, KEY_READ, &hTabletTip);
	if (lres == ERROR_SUCCESS)
	{
		lres = RegQueryValueEx(hTabletTip, NULL, NULL, &keyType, (BYTE*)&kVaule, &dwInfoSize);
		if (lres == ERROR_SUCCESS)
		{

		}
	}
	RegCloseKey(hTabletTip);
}

void SetTabletTipKey(LPCTSTR key, DWORD kVaule)
{
	HKEY hTabletTip = NULL;
	LSTATUS lres = 0;

	lres = RegOpenKeyEx(HKEY_CURRENT_USER, _T("SOFTWARE\\Microsoft\\TabletTip\\1.7"), 0, KEY_WRITE, &hTabletTip);
	if (lres == ERROR_SUCCESS)
	{
		lres = RegSetKeyValue(hTabletTip, NULL, key, REG_DWORD, (BYTE*)&kVaule, sizeof(kVaule));
		if (lres == ERROR_SUCCESS)
		{

		}
	}
	RegCloseKey(hTabletTip);

}

//EdgeTargetDockedState REG_DWORD 1- docked 0- undock
void TabtipDock(BOOL bDock)
{
	SetTabletTipKey(_T("EdgeTargetDockedState"),bDock);
}

//EnableCompatibilityKeyboard REG_DWORD 1/0
void TabtipCompatibilityKeyboard(BOOL bEnable)
{
	SetTabletTipKey(_T("EnableCompatibilityKeyboard"),bEnable);
}

//EnableDesktopModeAutoInvoke REG_DWORD 1- auto Show keyboard (focus in edit control)
void TabtipDesktopAutoShow(BOOL bEnable)
{
	SetTabletTipKey(_T("EnableDesktopModeAutoInvoke"), bEnable);
}

//EnableKeyAudioFeedback REG_DWORD 1/0
void TabtipAudioFeedback(BOOL bEnable)
{
	SetTabletTipKey(_T("EnableKeyAudioFeedback"), bEnable);
}

//PasswordSecurity 5
void TabtipPasswordSecurity(DWORD dwSecurityLevel)
{
	SetTabletTipKey(_T("PasswordSecurity"), dwSecurityLevel);
}

//lang, ingore fo now
//KeyboardEmojiCategoryPreference REG_DWORD ?
//KeyboardEmojiMFU REG_BINARY ?
//KeyboardEmojiMFUV2 REG_BINARY ?
void TabtipEmoji()
{}

//KeyboardLayoutPreference REG_DWORD [0~3] 0-QWERTY 1-thumb keyboard 2-handwriting 3-advance keyboard
void TabtipLayoutPreference(DWORD dwPreference/* = 0*/)
{
	SetTabletTipKey(_T("KeyboardLayoutPreference"), dwPreference);
}

// no effect
//OneHandedKeyboardRelativeXPositionOnScreen Decimal [0~100000]
void TabtipOneHandedKeyboardXPos(DWORD dwPos)
{
	dwPos = dwPos * desk_dpi;
	SetTabletTipKey(_T("OneHandedKeyboardRelativeXPositionOnScreen"), dwPos);
}

// no effect
//OneHandedKeyboardRelativeYPositionOnScreen Decimal [0~100000]
void TabtipOneHandedKeyboardYPos(DWORD dwPos)
{
	dwPos = dwPos * desk_dpi;
	SetTabletTipKey(_T("OneHandedKeyboardRelativeYPositionOnScreen"), dwPos);
}

//OptimizedKeyboardRelativeXPositionOnScreen  Decimal [0~100000] 1 pixel = screen dpi (default 96)
void TabtipKeyboardXPos(DWORD dwPos)
{
	dwPos = dwPos * desk_dpi;
	SetTabletTipKey(_T("OptimizedKeyboardRelativeXPositionOnScreen"), dwPos);
}

//OptimizedKeyboardRelativeYPositionOnScreen  Decimal [0~100000]
void TabtipKeyboardYPos(DWORD dwPos)
{
	dwPos = dwPos * desk_dpi;
	SetTabletTipKey(_T("OptimizedKeyboardRelativeYPositionOnScreen"), dwPos);
}

//SplitKeyboardRelativeYPositionOnScreen Decimal [0~100000] thumb keyboard Y rePos
// _in dwSize:
void TabtipThumbKeyboardYPos(DWORD dwPos)
{
	dwPos = dwPos * desk_dpi;
	SetTabletTipKey(_T("SplitKeyboardRelativeYPositionOnScreen"), dwPos);
}

//restart to take effect
//ThumbKeyboardSizePreference [0~2] size of Keyboard 0-small 1- middle 2-big
void TabtipThumbKeyboardSize(DWORD dwSize)
{
	SetTabletTipKey(_T("ThumbKeyboardSizePreference"), dwSize);
}

//restart to take effect
//TipbandDesiredVisibility 1/0 taskbar icon visible
void TabtipTaskbarVisibility(BOOL bVisible)
{
	SetTabletTipKey(_T("TipbandDesiredVisibility"), bVisible);
}

//TicDockedPreference [1] ?
void TabtipTicDocked(DWORD dw)
{}

//TotalFinalizedCountForSIUF [3] ?
void TabtipTotalFinalizedCountForSIUF(DWORD dw)
{}
