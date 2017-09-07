#pragma once

#include <mmreg.h>

namespace AudioLoopBack
{

	class IAudioCaptureEvent
	{
	public:
		virtual void OnCatpureStart(DWORD dwInterval) = 0;
		virtual void OnCaptureStop() = 0;

		virtual void OnAdjustCaptureFormat(WAVEFORMATEX* pFormat) = 0;
		virtual void OnCatpureData(LPBYTE pData, int nDataLen) = 0;
	};

	class CCaptureImpl;

	class CAudioPlaybackCaputre
	{
	public:
		CAudioPlaybackCaputre(void);
		~CAudioPlaybackCaputre(void);

	public:

		BOOL Initialize(IAudioCaptureEvent* pHandler);
		void UnInit();

		BOOL Start();
		void Stop();

		BOOL IsInited() const;
		BOOL IsCapturing() const;

	private:
		CAudioPlaybackCaputre(const CAudioPlaybackCaputre&);
		CAudioPlaybackCaputre& operator = (const CAudioPlaybackCaputre&);

	private:
		std::unique_ptr<CCaptureImpl> m_pImpl;
	};

}