#pragma once

#include <Tpcshrd.h>
#include "TouchSink.h"
// Manipulation implementation file
//#include <manipulations_i.c>

class IHandleTouchInput
{
public:
	virtual bool HandleTouchInput(__in POINT ptScreen, PTOUCHINPUT pIn) = 0;
	virtual bool HandleGestureInput(PGESTUREINFO pIn) = 0;
};

class TouchUtils
{
	IHandleTouchInput* m_pHandleInput;
public:
	static const unsigned int MOUSEEVENTF_FROMTOUCH = 0xff515700;
	RECT RcThreashold;

	TouchUtils()
		:m_pHandleInput(NULL)
	{
		ZeroMemory(&RcThreashold,sizeof(RcThreashold));
	}

	/// Multi Touch
	/// https://msdn.microsoft.com/library/dd940543.aspx

	bool SupportMultiTouch()
	{
		int ret = ::GetSystemMetrics(SM_DIGITIZER);
		//The device supports multiple sources of digitizer input.
		if(ret & NID_MULTI_INPUT)
			return true;
		return false;
	}

	int GetMaxMultiTouch()
	{
		return ::GetSystemMetrics(SM_MAXIMUMTOUCHES);
	}

	bool IsFromTouch()  
	{  
		LPARAM p = GetMessageExtraInfo();  
		return MOUSEEVENTF_FROMTOUCH == (p & MOUSEEVENTF_FROMTOUCH);  
	} 

	void SetupTouchHandle(IHandleTouchInput* handle)
	{
		m_pHandleInput = handle;
	}


	/// http://msdn.microsoft.com/library/dd353241.aspx
	void SetupGestureControl(HWND hWnd)
	{
		GESTURECONFIG gtconfigs[] = {
			{0,GC_ALLGESTURES,0}
			//,{GID_ZOOM,GC_ZOOM,0}
		};
		BOOL bSucceed = SetGestureConfig(hWnd,NULL,_countof(gtconfigs),gtconfigs,sizeof(GESTURECONFIG));
	}

	////http://msdn.microsoft.com/library/dd353242.aspx
	bool DecodeGesture(WPARAM wParam, LPARAM lParam)
	{
		HGESTUREINFO hgi = (HGESTUREINFO)lParam;
		bool handled = false;  
		GESTUREINFO gi = {0};  
		gi.cbSize = sizeof(gi);  
		if(GetGestureInfo(hgi,&gi))
		{
			if(m_pHandleInput) handled = m_pHandleInput->HandleGestureInput(&gi);
		}
		else{
			handled = false;
			DWORD dwErr = GetLastError();
		}
		if (handled)  
			CloseGestureInfoHandle(hgi);  
		return handled;  
	}

	void FlicksEnable(HWND hWnd, BOOL b)
	{
		const DWORD dwHwndTabletProperty = 
			//TABLET_DISABLE_PRESSANDHOLD | // disables press and hold (right-click) gesture
			//TABLET_DISABLE_PENTAPFEEDBACK | // disables UI feedback on pen up (waves)
			//TABLET_DISABLE_PENBARRELFEEDBACK | // disables UI feedback on pen button down (circle)
			TABLET_DISABLE_FLICKS; // disables pen flicks (back, forward, drag down, drag up)
		ATOM atom = ::GlobalAddAtom(MICROSOFT_TABLETPENSERVICE_PROPERTY);  
		SetProp(hWnd, MICROSOFT_TABLETPENSERVICE_PROPERTY, reinterpret_cast<HANDLE>(dwHwndTabletProperty));
		::GlobalDeleteAtom(atom);
	}

	void d(HWND hWnd)
	{
		RemoveProp(hWnd, MICROSOFT_TABLETPENSERVICE_PROPERTY);
	}

	void SetupTouchControl(HWND hWnd)
	{
		ULONG ulFlag = 0;
		if(!::IsTouchWindow(hWnd,&ulFlag))
		{
			ulFlag = TWF_WANTPALM | TWF_FINETOUCH;
			BOOL ret = ::RegisterTouchWindow(hWnd,ulFlag);
		}
		HRESULT hr = CoInitialize(0);
		hr = CoCreateInstance(CLSID_ManipulationProcessor,
			NULL,
			CLSCTX_INPROC_SERVER,
			IID_IUnknown,
			(VOID**)(&m_pIManipProc)
			);
		hr = CoCreateInstance(CLSID_InertiaProcessor,
			NULL,
			CLSCTX_INPROC_SERVER,
			IID_IUnknown,
			(VOID**)(&m_pIInertProc)
			);
		assert(m_pIManipProc);
		assert(m_pIInertProc);
		m_pManipulationEventSink.Release();
		m_pManipulationEventSink.Attach(new CManipulationEventSink(m_pIManipProc,m_pIInertProc, hWnd));
	}

	void DestoryTouchControl(HWND hWnd)
	{
		::UnregisterTouchWindow(hWnd);
		m_pManipulationEventSink->FreeConnect();
		m_pManipulationEventSink.Release();
		m_pIInertProc.Release();
		m_pIManipProc.Release();
		::CoUninitialize();
	}

	bool DecodeTouch(WPARAM wParam, LPARAM lParam)
	{
		// dwID - 接触点标识符，用于区分特定触控输入与其他输入
		//dwFlags - 一组位标志，用于指定接触点的状态
		//接触点的 X 和 Y 坐标（基本上是每个接触点的位置）centa-pixel
		//dwTime - 事件的时间步长，以毫秒为单位
		//dwMask - 一组位标志，用于指定结构中的哪些可选字段包含有效值
		HTOUCHINPUT hthi = (HTOUCHINPUT)lParam;
		unsigned int inputs = (unsigned int)wParam;
		TOUCHINPUT* tins = new  TOUCHINPUT[inputs];

		//设置标识获取更多信息
		for (auto index = 0U; index < inputs; ++index)
		{
			TOUCHINPUT& tin = tins[index];
			tin.dwMask = TOUCHINPUTMASKF_CONTACTAREA | TOUCHINPUTMASKF_EXTRAINFO | TOUCHINPUTMASKF_TIMEFROMSYSTEM;
		}
		bool handled = true;  
		if(GetTouchInputInfo(hthi, inputs, tins, sizeof(TOUCHINPUT)))
		{
			for(auto index = 0U; index < inputs; ++index)
			{
				TOUCHINPUT& tin = tins[index];
				POINT ptScreen = {TOUCH_COORD_TO_PIXEL(tin.x),TOUCH_COORD_TO_PIXEL(tin.y)};
				if (tin.dwFlags & TOUCHEVENTF_DOWN) {
					m_pIManipProc->ProcessDown(tin.dwID, static_cast<FLOAT>(tin.x), static_cast<FLOAT>(tin.y));
					handled = true;
				}
				if (tin.dwFlags & TOUCHEVENTF_UP) {
					m_pIManipProc->ProcessUp(tin.dwID, static_cast<FLOAT>(tin.x), static_cast<FLOAT>(tin.y));
					handled = true;
				}
				if (tin.dwFlags & TOUCHEVENTF_MOVE) {
					m_pIManipProc->ProcessMove(tin.dwID, static_cast<FLOAT>(tin.x), static_cast<FLOAT>(tin.y));
					handled = true;
				}
				//if(m_pHandleInput) handled = m_pHandleInput->HandleTouchInput(ptScreen,&tin);
			}
		}
		else  
		{  
			handled = false;  
		}  
		if (handled)  
			CloseTouchInputHandle(hthi);  
		delete [] tins;
		return handled;  
	}

	///THIS IS LEGACY TOUCH
	/// WM_TOUCH handle functions, mutually exclusive WM_GESTURE
	LRESULT CALLBACK TouchWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandle)
	{
		bHandle = 0;
		LRESULT res = 0;
		switch(uMsg)
		{
		case WM_GESTURENOTIFY:
			SetupGestureControl(hWnd);
			break;
			///http://msdn.microsoft.com/en-us/library/bb969148.aspx
		case WM_TABLET_QUERYSYSTEMGESTURESTATUS:
			{
				POINT pt = {GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
				if (::PtInRect(&RcThreashold,pt)){
					// no flicks in the region specified by the threashold
					res =  TABLET_DISABLE_FLICKS;
					bHandle = 1;
				}
			}
			break;
		case WM_GESTURE:
			res = DecodeGesture(wParam, lParam) ? 1 : 0;
			bHandle = 1;
			break;
		case WM_TOUCH:	
			//如果应用程序处理此消息，则它应返回 0。
			res = DecodeTouch(wParam, lParam) ? 0 : 0;
			bHandle = 1;
			break;
		case WM_TIMER:       
			if (m_pIInertProc && wParam == TIMER_INERTPROC){
				BOOL b;       
				m_pIInertProc->Process(&b); 
				bHandle = 1;
			}
			break;
		default:
			break;
		}
		return res;
	}


	///// Smart Pointer to a global reference of a manipulation processor, event sink
	CComPtr<IManipulationProcessor> m_pIManipProc;
	CComPtr<IInertiaProcessor> m_pIInertProc;

	//// Set up a variable to point to the manipulation event sink implementation class    
	CComPtr<CManipulationEventSink> m_pManipulationEventSink;

	///Pointer Input Message
	///https://msdn.microsoft.com/en-us/library/hh454877(v=vs.85).aspx
};
