#include "stdafx.h"
#include "AudioPlaybackCaputre.h"
#include <process.h>

#include <mmdeviceapi.h>
#include <Audioclient.h>
#include <avrt.h>

#pragma comment (lib,"Avrt.lib")

namespace AudioLoopBack
{

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
		HWND m_hWndMessage;
		HANDLE m_hEventStarted;
		HANDLE m_hEventStop;
		HANDLE m_hThreadCapture;

		BOOL m_bInited;

		IAudioCaptureEvent* m_pEventHandler;

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

			//NotifyWaveFormat(hWndMessage, pwfx);
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

			//NotifyStatus(hWndMessage, CAPTURE_START, lTimeBetweenFires);
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
					//NotifyStatus(hWndMessage, CAPTURE_ERROR);
					break;
				}

				hr = pAudioCaptureClient->GetNextPacketSize(&nNextPacketSize);
				if(FAILED(hr))
				{
					//NotifyStatus(hWndMessage, CAPTURE_ERROR);
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
					//NotifyStatus(hWndMessage, CAPTURE_ERROR);
					break;
				}

				if (0 != nNumFramesToRead)
				{
					//NotifyData(hWndMessage, pData, nNumFramesToRead * pwfx->nBlockAlign);
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
				//NotifyStatus(hWndMessage, CAPTURE_STOP);
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

	CAudioPlaybackCaputre::CAudioPlaybackCaputre(void)
		:m_pImpl(new CCaptureImpl)
	{

	}

	CAudioPlaybackCaputre::~CAudioPlaybackCaputre(void)
	{
	}

	BOOL CAudioPlaybackCaputre::Initialize(IAudioCaptureEvent* pHandler)
	{
		if(m_pImpl)
			return m_pImpl->Initialize(pHandler);
		return FALSE;
	}

	void CAudioPlaybackCaputre::UnInit()
	{
		if(m_pImpl)
			m_pImpl->UnInit();
	}

	BOOL CAudioPlaybackCaputre::Start()
	{
		if(m_pImpl)
			return m_pImpl->Start();
		return FALSE;
	}

	void CAudioPlaybackCaputre::Stop()
	{
		if(m_pImpl)
			m_pImpl->Stop();
	}

	BOOL CAudioPlaybackCaputre::IsInited() const
	{
		if(m_pImpl)
			return m_pImpl->IsInited();
		return FALSE;
	}

	BOOL CAudioPlaybackCaputre::IsCapturing() const
	{
		if(m_pImpl)
			return m_pImpl->IsCapturing();
		return FALSE;
	}

}//namespace AudioLoopBack