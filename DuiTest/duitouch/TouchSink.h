#pragma once
// Manipulation Header Files
#include <comdef.h>
#include <manipulations.h>
#include <ocidl.h>


///https://msdn.microsoft.com/zh-cn/library/windows/desktop/dd371594(v=vs.85).aspx

#define TIMER_INERTPROC 0xFFFFFFFC

class CManipulationEventSink : _IManipulationEvents
{
public:
	CManipulationEventSink(IManipulationProcessor *manip, IInertiaProcessor *pInert, HWND hWnd);
	CManipulationEventSink(IInertiaProcessor *pInert, HWND hWnd);

	int GetStartedEventCount();
	int GetDeltaEventCount();
	int GetCompletedEventCount();
	double CManipulationEventSink::GetX();
	double CManipulationEventSink::GetY();

	void FreeConnect();

	~CManipulationEventSink();

	//////////////////////////////
	// IManipulationEvents methods
	//////////////////////////////
	virtual HRESULT STDMETHODCALLTYPE ManipulationStarted(
		/* [in] */ FLOAT x,
		/* [in] */ FLOAT y);

	virtual HRESULT STDMETHODCALLTYPE ManipulationDelta(
		/* [in] */ FLOAT x,
		/* [in] */ FLOAT y,
		/* [in] */ FLOAT translationDeltaX,
		/* [in] */ FLOAT translationDeltaY,
		/* [in] */ FLOAT scaleDelta,
		/* [in] */ FLOAT expansionDelta,
		/* [in] */ FLOAT rotationDelta,
		/* [in] */ FLOAT cumulativeTranslationX,
		/* [in] */ FLOAT cumulativeTranslationY,
		/* [in] */ FLOAT cumulativeScale,
		/* [in] */ FLOAT cumulativeExpansion,
		/* [in] */ FLOAT cumulativeRotation);

	virtual HRESULT STDMETHODCALLTYPE ManipulationCompleted(
		/* [in] */ FLOAT x,
		/* [in] */ FLOAT y,
		/* [in] */ FLOAT cumulativeTranslationX,
		/* [in] */ FLOAT cumulativeTranslationY,
		/* [in] */ FLOAT cumulativeScale,
		/* [in] */ FLOAT cumulativeExpansion,
		/* [in] */ FLOAT cumulativeRotation);

	////////////////////////////////////////////////////////////
	// IUnknown methods
	////////////////////////////////////////////////////////////
	STDMETHOD_(ULONG, AddRef)(void);
	STDMETHOD_(ULONG, Release)(void);
	STDMETHOD(QueryInterface)(REFIID riid, LPVOID *ppvObj);

private:
	double m_fX;
	double m_fY;

	int m_cRefCount;
	int m_cStartedEventCount;
	int m_cDeltaEventCount;
	int m_cCompletedEventCount;

	IManipulationProcessor* m_pManip;

	IConnectionPointContainer* m_pConPointContainer;
	IConnectionPoint* m_pConnPoint;

	DWORD dwCookie;

	HWND m_hWnd;

	IInertiaProcessor*      m_pInert;
	BOOL fExtrapolating;
};
