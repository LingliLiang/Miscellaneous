#include "stdafx.h"
#include "AudioPlaybackCapture.h"
#include <process.h>

#include <mmdeviceapi.h>
#include <Audioclient.h>
#include <avrt.h>

#pragma comment (lib,"Avrt.lib")

namespace AudioLoopBack
{

#define WM_STATUS		WM_USER+1000
#define WM_WAVEFORMAT			WM_USER+1001
#define WM_WAVEDATA			WM_USER+1002

	namespace {

		void NotifyWaveFormat(HWND hWnd, PWAVEFORMATEX* ppwfx)
		{
			::SendMessage(hWnd, WM_WAVEFORMAT, 0, (LPARAM)(*ppwfx));
		}

		void NotifyWaveData(HWND hWnd, LPBYTE pData, int nDataLen)
		{
			::SendMessage(hWnd, WM_WAVEDATA, (WPARAM)pData, nDataLen);
		}

		void NotifyStatus(HWND hWnd, DWORD dwStatus, LPVOID UserData)
		{
			::SendMessage(hWnd, WM_STATUS, (WPARAM)dwStatus, (LPARAM)UserData);
		}

	}// unname namespace



	class CCaptureImpl
	{
	public:
		CCaptureImpl();
		~CCaptureImpl();

		BOOL Initialize(IAudioCaptureEvent* pHandler);
		void UnInit();

		BOOL Start();
		void Stop();

		BOOL IsInited() const;
		BOOL IsCapturing() const;

		IAudioCaptureEvent* GetEventHandler() const	{ return m_pEventHandler;}
		void OnThreadEnd();

	private:
		IMMDevice* GetMMDevice();
		CComPtr<IMMDevice> m_pDevice;

	private:
		static unsigned int WINAPI ThreadCaptureProc(LPVOID param);
		unsigned int AudioCaptureProc();
		HANDLE m_hEventStarted;
		HANDLE m_hEventStop;
		HANDLE m_hThreadCapture;

		BOOL m_bInited;

		IAudioCaptureEvent* m_pEventHandler;

		///////
		HANDLE m_hThreadWnd;
		HWND m_hMsgWnd;
		static unsigned int WINAPI ThreadWndProc(LPVOID param);
		static LRESULT WINAPI WindowProc(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam);

	};

	CCaptureImpl::CCaptureImpl()
	{}

	CCaptureImpl::~CCaptureImpl()
	{
		if(m_bInited) UnInit();
	}

	BOOL CCaptureImpl::Initialize(IAudioCaptureEvent* pHandler)
	{
		if(m_bInited) return TRUE;

		m_pEventHandler = pHandler;
		
		do{
			m_pDevice.Attach(GetMMDevice());
			if(m_pDevice == NULL) break;

			m_hEventStop = CreateEvent(NULL, TRUE, FALSE, NULL);
			if(m_hEventStop == NULL) break;

			m_hEventStarted = CreateEvent(NULL, TRUE, FALSE, NULL);
			if(m_hEventStarted == NULL) break;

			m_bInited = TRUE;

		}while(FALSE);

		if(!m_bInited) UnInit();

		return m_bInited;
	}

	void CCaptureImpl::UnInit()
	{
		m_pEventHandler = NULL;
		m_pDevice = NULL;

		if(m_hEventStop != NULL)
		{
			CloseHandle(m_hEventStop);
			m_hEventStop = NULL;
		}

		if(m_hEventStarted != NULL)
		{
			CloseHandle(m_hEventStarted);
			m_hEventStarted = NULL;
		}

		m_bInited = FALSE;
	}

	BOOL CCaptureImpl::Start()
	{
		if(!m_bInited) return FALSE;
		if(m_hThreadCapture != NULL) return TRUE;

		m_hThreadCapture = (HANDLE)_beginthreadex(NULL, 0, &ThreadCaptureProc, this, 0, NULL);
		if(m_hThreadCapture == NULL) return FALSE;

		m_hThreadWnd = (HANDLE)_beginthreadex(NULL, 0, &ThreadWndProc, this, 0, NULL);
		if(m_hThreadWnd == NULL) return FALSE;

		HANDLE arWaits[2] = {m_hEventStarted, m_hThreadCapture};
		DWORD dwWaitResult = WaitForMultipleObjects(sizeof(arWaits)/sizeof(arWaits[0]), arWaits, FALSE, INFINITE);
		if(dwWaitResult != WAIT_OBJECT_0)
		{
			Stop();
			return FALSE;
		}

		return TRUE;
	}

	void CCaptureImpl::Stop()
	{
		if(!m_bInited) return;

		if(m_hEventStop != NULL
			&& m_hThreadCapture != NULL)
		{
			SetEvent(m_hEventStop);
			OnThreadEnd();
		}
		if(m_hThreadWnd && m_hMsgWnd)
		{
			::SendMessage(m_hMsgWnd,WM_CLOSE,0,0);
			::CloseHandle(m_hThreadWnd);
			m_hMsgWnd = NULL;
			m_hThreadWnd = NULL;
		}
	}

	BOOL CCaptureImpl::IsInited() const
	{
		return m_bInited;
	}

	BOOL CCaptureImpl::IsCapturing() const
	{
		return m_hThreadCapture != NULL;
	}

	void CCaptureImpl::OnThreadEnd()
	{
		if(m_hThreadCapture != NULL)
		{
			CloseHandle(m_hThreadCapture);
			m_hThreadCapture = NULL;
		}
	}

	unsigned int  WINAPI CCaptureImpl::ThreadCaptureProc(LPVOID param)
	{
		::CoInitialize(NULL);
		CCaptureImpl* pThis = static_cast<CCaptureImpl*>(param);
		unsigned int nRet = 0;
		if(pThis)
		{
			nRet = pThis->AudioCaptureProc();
		}
		::CoUninitialize();
		return nRet;
	}

	BOOL AdjustFormatTo16Bits(WAVEFORMATEX *pwfx)
	{
		BOOL bRet(FALSE);

		if(pwfx->wFormatTag == WAVE_FORMAT_IEEE_FLOAT)
		{
			pwfx->wFormatTag = WAVE_FORMAT_PCM;
			pwfx->wBitsPerSample = 16;
			pwfx->nBlockAlign = pwfx->nChannels * pwfx->wBitsPerSample / 8;
			pwfx->nAvgBytesPerSec = pwfx->nBlockAlign * pwfx->nSamplesPerSec;

			bRet = TRUE;
		}
		else if(pwfx->wFormatTag == WAVE_FORMAT_EXTENSIBLE)
		{
			PWAVEFORMATEXTENSIBLE pEx = reinterpret_cast<PWAVEFORMATEXTENSIBLE>(pwfx);
			if (IsEqualGUID(KSDATAFORMAT_SUBTYPE_IEEE_FLOAT, pEx->SubFormat))
			{
				pEx->SubFormat = KSDATAFORMAT_SUBTYPE_PCM;
				pEx->Samples.wValidBitsPerSample = 16;
				pwfx->wBitsPerSample = 16;
				pwfx->nBlockAlign = pwfx->nChannels * pwfx->wBitsPerSample / 8;
				pwfx->nAvgBytesPerSec = pwfx->nBlockAlign * pwfx->nSamplesPerSec;

				bRet = TRUE;
			} 
		}

		return bRet;
	}

	unsigned int CCaptureImpl::AudioCaptureProc()
	{
		HRESULT hr;
		IAudioClient *pAudioClient = NULL;
		WAVEFORMATEX *pwfx = NULL;
		REFERENCE_TIME hnsDefaultDevicePeriod(0);
		HANDLE hTimerWakeUp = NULL; 
		IAudioCaptureClient *pAudioCaptureClient = NULL;
		DWORD nTaskIndex = 0;
		HANDLE hTask = NULL;
		BOOL bStarted(FALSE);
		do 
		{
			hr = m_pDevice->Activate(__uuidof(IAudioClient), CLSCTX_ALL, NULL, (void**)&pAudioClient);
			if(FAILED(hr)) break;

			hr = pAudioClient->GetDevicePeriod(&hnsDefaultDevicePeriod, NULL);
			if(FAILED(hr)) break;

			hr = pAudioClient->GetMixFormat(&pwfx);
			if (FAILED(hr)) break;

			if(!AdjustFormatTo16Bits(pwfx)) break;

			hTimerWakeUp = CreateWaitableTimer(NULL, FALSE, NULL);
			if(hTimerWakeUp == NULL) break;

			SetEvent(m_hEventStarted);

			NotifyWaveFormat(m_hMsgWnd, &pwfx);

			hr = pAudioClient->Initialize(AUDCLNT_SHAREMODE_SHARED, AUDCLNT_STREAMFLAGS_LOOPBACK, 0, 0, pwfx, 0);
			if(FAILED(hr)) break;

			hr = pAudioClient->GetService(__uuidof(IAudioCaptureClient), (void**)&pAudioCaptureClient);
			if(FAILED(hr)) break;

			hTask = AvSetMmThreadCharacteristics(L"Capture", &nTaskIndex);
			if (NULL == hTask) break;

			LARGE_INTEGER liFirstFire;
			liFirstFire.QuadPart = -hnsDefaultDevicePeriod / 2; // negative means relative time
			LONG lTimeBetweenFires = (LONG)hnsDefaultDevicePeriod / 2 / (10 * 1000); // convert to milliseconds

			BOOL bOK = SetWaitableTimer(hTimerWakeUp,&liFirstFire,lTimeBetweenFires,NULL, NULL, FALSE);
			if(!bOK) break;

			hr = pAudioClient->Start();
			if(FAILED(hr)) break;

			NotifyStatus(m_hMsgWnd, CAPTURE_START, (LPVOID)lTimeBetweenFires);
			bStarted = TRUE;

			HANDLE waitArray[2] = { m_hEventStop, hTimerWakeUp };
			DWORD dwWaitResult;
			UINT32 nNextPacketSize(0);
			BYTE *pData = NULL;
			UINT32 nNumFramesToRead;
			DWORD dwFlags;
			while(TRUE)
			{
				dwWaitResult = WaitForMultipleObjects(sizeof(waitArray)/sizeof(waitArray[0]), waitArray, FALSE, INFINITE);
				if(WAIT_OBJECT_0 == dwWaitResult) break;

				if (WAIT_OBJECT_0 + 1 != dwWaitResult)
				{
					NotifyStatus(m_hMsgWnd, CAPTURE_ERROR, 0);
					break;
				}

				hr = pAudioCaptureClient->GetNextPacketSize(&nNextPacketSize);
				if(FAILED(hr))
				{
					NotifyStatus(m_hMsgWnd, CAPTURE_ERROR, 0);
					break;
				}

				if (nNextPacketSize == 0) continue;

				hr = pAudioCaptureClient->GetBuffer(
					&pData,
					&nNumFramesToRead,
					&dwFlags,
					NULL,
					NULL
					);
				if(FAILED(hr))
				{
					NotifyStatus(m_hMsgWnd, CAPTURE_ERROR, 0);
					break;
				}

				if (0 != nNumFramesToRead)
				{
					NotifyWaveData(m_hMsgWnd, pData, nNumFramesToRead * pwfx->nBlockAlign);
				}

				pAudioCaptureClient->ReleaseBuffer(nNumFramesToRead);
			}

		}while(FALSE);

		if(hTask != NULL)
		{
			AvRevertMmThreadCharacteristics(hTask);
			hTask = NULL;
		}

		if(pAudioCaptureClient != NULL)
		{
			pAudioCaptureClient->Release();
			pAudioCaptureClient = NULL;
		}

		if(pwfx != NULL)
		{
			CoTaskMemFree(pwfx);
			pwfx = NULL;
		}

		if(hTimerWakeUp != NULL)
		{
			CancelWaitableTimer(hTimerWakeUp);
			CloseHandle(hTimerWakeUp);
			hTimerWakeUp = NULL;
		}

		if(pAudioClient != NULL)
		{
			if(bStarted)
			{
				pAudioClient->Stop();
				NotifyStatus(m_hMsgWnd, CAPTURE_STOP, 0);
			}

			pAudioClient->Release();
			pAudioClient = NULL;
		}

		return 0;
	}

	IMMDevice* CCaptureImpl::GetMMDevice()
	{
		IMMDevice* pDevice = NULL;
		IMMDeviceEnumerator *pMMDeviceEnumerator = NULL;
		HRESULT hr = CoCreateInstance(
			__uuidof(MMDeviceEnumerator), NULL, CLSCTX_ALL, 
			__uuidof(IMMDeviceEnumerator),
			(void**)&pMMDeviceEnumerator);
		if(FAILED(hr)) return NULL;

		hr = pMMDeviceEnumerator->GetDefaultAudioEndpoint(eRender, eConsole, &pDevice);
		pMMDeviceEnumerator->Release();
		return pDevice;
	}

	///---------------------------------------------------------------------

	unsigned int  WINAPI CCaptureImpl::ThreadWndProc(LPVOID param)
	{
		CCaptureImpl* pThis = static_cast<CCaptureImpl*>(param);
		TCHAR szClassName[] = _T("AudioLoopBack-CCaptureImplWindow");
		WNDCLASSEX wndClass;
		wndClass.cbSize = sizeof(wndClass);
		wndClass.style = CS_HREDRAW | CS_VREDRAW;
		wndClass.lpfnWndProc = WindowProc;
		wndClass.cbClsExtra = 0;
		wndClass.cbWndExtra = 0;
		wndClass.hInstance = GetModuleHandle(0);
		wndClass.hIcon = LoadIcon(NULL,IDI_APPLICATION);
		wndClass.hCursor = LoadCursor(NULL,IDC_ARROW);
		wndClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
		wndClass.lpszMenuName = NULL;
		wndClass.lpszClassName = szClassName;
		wndClass.hIconSm = NULL;

		ATOM atom = RegisterClassEx(&wndClass);
		if(0 == atom)
		{
			if(GetLastError() != ERROR_CLASS_ALREADY_EXISTS)
			{
				return 0;
			}
		}
		pThis->m_hMsgWnd = CreateWindowEx(0,szClassName,_T(""),WS_OVERLAPPEDWINDOW,CW_USEDEFAULT,CW_USEDEFAULT,
			CW_USEDEFAULT,CW_USEDEFAULT,HWND_MESSAGE,NULL,NULL,pThis);
		if(pThis->m_hMsgWnd == NULL)
		{
			return 0;
		}
		MSG msg;
		while(GetMessage(&msg,NULL,0,0))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		return 0;
	}

	LRESULT WINAPI CCaptureImpl::WindowProc(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
	{
		CCaptureImpl* pThis = NULL;
		pThis = reinterpret_cast<CCaptureImpl*>(::GetWindowLongPtr(hWnd, GWLP_USERDATA));
		switch(uMsg)
		{
		case WM_DESTROY:
			{
				if(pThis){
					::SetWindowLongPtr(pThis->m_hMsgWnd, GWLP_USERDATA, 0L);
				}
				::PostThreadMessage(::GetCurrentThreadId(),WM_QUIT,0,0);
			}
		case WM_CREATE:
			{
				LPCREATESTRUCT lpcs = reinterpret_cast<LPCREATESTRUCT>(lParam);
				if(lpcs)
				{
					pThis = static_cast<CCaptureImpl*>(lpcs->lpCreateParams);
					::SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LPARAM>(pThis));
				}
			}
		}
		if(pThis)
		{
			auto & pEventHandler = pThis->m_pEventHandler;
			if(pEventHandler)
			{
				if( uMsg == WM_STATUS)
				{
					if(wParam == CAPTURE_START)
						pEventHandler->OnCaptureStart((DWORD)lParam);
					else if(wParam == CAPTURE_STOP)
						pEventHandler->OnCaptureStop();
					else
						pEventHandler->OnCaptureStatus((DWORD)wParam, (LPVOID)lParam);
				}
				else if(uMsg == WM_WAVEDATA)
				{
					pEventHandler->OnCaptureData(reinterpret_cast<LPBYTE>(wParam), (int)lParam);
				}
				else if(uMsg == WM_WAVEFORMAT)
				{
					pEventHandler->OnAdjustCaptureFormat(reinterpret_cast<PWAVEFORMATEX>(lParam));
				}
			}
		}
		return DefWindowProc(hWnd,uMsg,wParam,lParam);
	}

	///---------------------------------------------------------------------

	CAudioPlaybackCapture::CAudioPlaybackCapture(void)
		:m_pImpl(new CCaptureImpl)
	{

	}

	CAudioPlaybackCapture::~CAudioPlaybackCapture(void)
	{
	}

	BOOL CAudioPlaybackCapture::Initialize(IAudioCaptureEvent* pHandler)
	{
		if(m_pImpl)
			return m_pImpl->Initialize(pHandler);
		return FALSE;
	}

	void CAudioPlaybackCapture::UnInit()
	{
		if(m_pImpl)
			m_pImpl->UnInit();
	}

	BOOL CAudioPlaybackCapture::Start()
	{
		if(m_pImpl)
			return m_pImpl->Start();
		return FALSE;
	}

	void CAudioPlaybackCapture::Stop()
	{
		if(m_pImpl)
			m_pImpl->Stop();
	}

	BOOL CAudioPlaybackCapture::IsInited() const
	{
		if(m_pImpl)
			return m_pImpl->IsInited();
		return FALSE;
	}

	BOOL CAudioPlaybackCapture::IsCapturing() const
	{
		if(m_pImpl)
			return m_pImpl->IsCapturing();
		return FALSE;
	}

}//namespace AudioLoopBack