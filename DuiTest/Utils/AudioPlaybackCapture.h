#pragma once

#include <mmreg.h>

namespace AudioLoopBack
{
	enum CaptureStatus{
		CAPTURE_START,
		CAPTURE_STOP,
		CAPTURE_ERROR
	};

	class IAudioCaptureEvent
	{
	public:
		virtual void OnCaptureStart(DWORD dwInterval) = 0;
		virtual void OnCaptureStop() = 0;
		virtual void OnCaptureStatus(DWORD status, LPVOID data) = 0;
		virtual void OnAdjustCaptureFormat(const WAVEFORMATEX* pFormat) = 0;
		virtual void OnCaptureData(LPBYTE pData, int nDataLen) = 0;
	};

	class CCaptureImpl;

	class CAudioPlaybackCapture
	{
	public:
		CAudioPlaybackCapture(void);
		~CAudioPlaybackCapture(void);

	public:

		BOOL Initialize(IAudioCaptureEvent* pHandler);
		void UnInit();

		BOOL Start();
		void Stop();

		BOOL IsInited() const;
		BOOL IsCapturing() const;

	private:
		CAudioPlaybackCapture(const CAudioPlaybackCapture&);
		CAudioPlaybackCapture& operator = (const CAudioPlaybackCapture&);

	private:
		std::unique_ptr<CCaptureImpl> m_pImpl;
	};

}