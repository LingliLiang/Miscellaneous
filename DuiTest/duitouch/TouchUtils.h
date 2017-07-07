#pragma once

#include <Tpcshrd.h>

class IHandleTouchInput
{
public:
	virtual bool HandleTouchInput(__in POINT ptScreen, PTOUCHINPUT pIn) = 0;
	virtual bool HandleGestureInput(__in POINT ptScreen, PTOUCHINPUT pIn) = 0;
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
		bool handled = true;  
		GESTUREINFO gi = {0};  
		gi.cbSize = sizeof(gi);  
		if(GetGestureInfo(hgi,&gi))
		{
			switch (gi.dwID)  
			{  
			case GID_BEGIN: //ָʾ���ͱ����ѿ�ʼ
				break;
			case GID_END: //ָʾ���ͱ����ѽ���
				break;
			case GID_ZOOM: //ָʾ���ſ�ʼ�������ƶ�������ֹͣ����һ�� GID_ZOOM ������Ϣ��ʼ���ŵ����ᵼ���κ����š��ڶ��� GID_ZOOM ��������һ�� GID_ZOOM �а�����״̬��ص�����
				gi.ullArguments;//�������
				gi.ptsLocation; //��������
				break;  
			case GID_PAN: //ָʾƽ���ƶ���ƽ�ƿ�ʼ����һ�� GID_PAN ����ָʾƽ�ƿ�ʼ������ִ���κ�ƽ�ơ��ڳ��ֵڶ��� GID_PAN ������Ϣʱ��Ӧ�ó��򽫿�ʼƽ�ơ�
				gi.ullArguments;//�������
				gi.ptsLocation; //��ǰλ��
				break;  
			case GID_ROTATE: //ָʾ��ת�ƶ�����ת��ʼ����һ�� GID_ROTATE ������Ϣָʾ��ת�ƶ�����ת��ʼ�����������ת���ڶ��� GID_ROTATE ������Ϣ���������һ�� GID_ROTATE �а�����״̬��ص���ת������
				gi.ullArguments;//��ת�Ƕ�
				gi.ptsLocation; //��ת����
				break;  
			case GID_TWOFINGERTAP: // ָʾ˫ָ�������
				gi.ullArguments;//�������
				gi.ptsLocation; //��������
				break;  
			case GID_PRESSANDTAP: //ָʾ��ס���������
				break;  
			default:  
				handled = false;
				break;  
			}  
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
	}

	void DestoryTouchControl(HWND hWnd)
	{
		::UnregisterTouchWindow(hWnd);
	}

	bool DecodeTouch(WPARAM wParam, LPARAM lParam)
	{
		// dwID - �Ӵ����ʶ�������������ض�������������������
		//dwFlags - һ��λ��־������ָ���Ӵ����״̬
		//�Ӵ���� X �� Y ���꣨��������ÿ���Ӵ����λ�ã�centa-pixel
		//dwTime - �¼���ʱ�䲽�����Ժ���Ϊ��λ
		//dwMask - һ��λ��־������ָ���ṹ�е���Щ��ѡ�ֶΰ�����Чֵ
		HTOUCHINPUT hthi = (HTOUCHINPUT)lParam;
		unsigned int inputs = (unsigned int)wParam;
		TOUCHINPUT* tins = new  TOUCHINPUT[inputs];
		bool handled = true;  
		if(GetTouchInputInfo(hthi, inputs, tins, sizeof(TOUCHINPUT)))
		{
			for(auto index = 0U; index < inputs; ++index)
			{
				TOUCHINPUT& tin = tins[index];
				POINT ptScreen = {TOUCH_COORD_TO_PIXEL(tin.x),TOUCH_COORD_TO_PIXEL(tin.y)};
				if(m_pHandleInput) m_pHandleInput->HandleTouchInput(ptScreen,&tin);
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
			//SetGestureConfig
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
			//���Ӧ�ó��������Ϣ������Ӧ���� 0��
			res = DecodeTouch(wParam, lParam) ? 0 : 0;
			bHandle = 1;
			break;
		default:
			break;
		}
		return res;
	}

	///Pointer Input Message
	///https://msdn.microsoft.com/en-us/library/hh454877(v=vs.85).aspx
};
