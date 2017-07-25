#include "stdafx.h"
#include "TouchSink.h"

CManipulationEventSink::CManipulationEventSink(IManipulationProcessor *manip, HWND hWnd)
{
	m_hWnd = hWnd;

	//Set initial ref count to 1.
	m_cRefCount = 1;

	m_pManip = manip;
	m_pManip->put_PivotRadius(-1);

	m_cStartedEventCount = 0;
	m_cDeltaEventCount = 0;
	m_cCompletedEventCount = 0;

	HRESULT hr = S_OK;

	//Get the container with the connection points.
	IConnectionPointContainer* spConnectionContainer;

	hr = manip->QueryInterface(
		IID_IConnectionPointContainer,
		(LPVOID*)&spConnectionContainer
		);
	//hr = manip->QueryInterface(&spConnectionContainer);
	if (spConnectionContainer == NULL) {
		// something went wrong, try to gracefully quit

	}

	//Get a connection point.
	hr = spConnectionContainer->FindConnectionPoint(__uuidof(_IManipulationEvents), &m_pConnPoint);
	if (m_pConnPoint == NULL) {
		// something went wrong, try to gracefully quit
	}

	DWORD dwCookie;

	//Advise.
	hr = m_pConnPoint->Advise(this, &dwCookie);
}

int CManipulationEventSink::GetStartedEventCount()
{
	return m_cStartedEventCount;
}

int CManipulationEventSink::GetDeltaEventCount()
{
	return m_cDeltaEventCount;
}

int CManipulationEventSink::GetCompletedEventCount()
{
	return m_cCompletedEventCount;
}

double CManipulationEventSink::GetX()
{
	return m_fX;
}

double CManipulationEventSink::GetY()
{
	return m_fY;
}

CManipulationEventSink::~CManipulationEventSink()
{
	//Cleanup.
}

///////////////////////////////////
//Implement IManipulationEvents
///////////////////////////////////

HRESULT STDMETHODCALLTYPE CManipulationEventSink::ManipulationStarted(
	/* [in] */ FLOAT x,
	/* [in] */ FLOAT y)
{
	m_cStartedEventCount++;

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CManipulationEventSink::ManipulationDelta(
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
	/* [in] */ FLOAT cumulativeRotation)
{
	m_cDeltaEventCount++;

	RECT rect;

	GetWindowRect(m_hWnd, &rect);

	int oldWidth = rect.right - rect.left;
	int oldHeight = rect.bottom - rect.top;

	// scale and translate the window size / position    
	MoveWindow(m_hWnd,                                                     // the window to move
		static_cast<int>(rect.left + (translationDeltaX / 100.0f)), // the x position
		static_cast<int>(rect.top + (translationDeltaY / 100.0f)),    // the y position
		static_cast<int>(oldWidth * scaleDelta),                    // width
		static_cast<int>(oldHeight * scaleDelta),                   // height
		TRUE);                                                      // redraw

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CManipulationEventSink::ManipulationCompleted(
	/* [in] */ FLOAT x,
	/* [in] */ FLOAT y,
	/* [in] */ FLOAT cumulativeTranslationX,
	/* [in] */ FLOAT cumulativeTranslationY,
	/* [in] */ FLOAT cumulativeScale,
	/* [in] */ FLOAT cumulativeExpansion,
	/* [in] */ FLOAT cumulativeRotation)
{
	m_cCompletedEventCount++;

	m_fX = x;
	m_fY = y;

	// place your code handler here to do any operations based on the manipulation   

	return S_OK;
}


/////////////////////////////////
//Implement IUnknown
/////////////////////////////////

ULONG CManipulationEventSink::AddRef(void)
{
	return ++m_cRefCount;
}

ULONG CManipulationEventSink::Release(void)
{
	m_cRefCount--;

	if (0 == m_cRefCount) {
		delete this;
		return 0;
	}

	return m_cRefCount;
}

HRESULT CManipulationEventSink::QueryInterface(REFIID riid, LPVOID *ppvObj)
{
	if (IID__IManipulationEvents == riid) {
		*ppvObj = (_IManipulationEvents *)(this); AddRef(); return S_OK;
	}
	else if (IID_IUnknown == riid) {
		*ppvObj = (IUnknown *)(this); AddRef(); return S_OK;
	}
	else {
		return E_NOINTERFACE;
	}
}


// Manipulation implementation file
#include <manipulations_i.c>

//Smart Pointer to a global reference of a manipulation processor, event sink
CComPtr<IManipulationProcessor> g_pIManipProc;

void init() {
	HRESULT hr = CoInitialize(0);

	hr = CoCreateInstance(CLSID_ManipulationProcessor,
		NULL,
		CLSCTX_INPROC_SERVER,
		IID_IUnknown,
		(VOID**)(&g_pIManipProc)
		);

}

// Set up a variable to point to the manipulation event sink implementation class    
CComPtr<CManipulationEventSink> g_pManipulationEventSink(new CManipulationEventSink(g_pIManipProc, 0));

//if (pInputs[i].dwFlags & TOUCHEVENTF_DOWN) {
//	g_pIManipProc->ProcessDown(pInputs[i].dwID, static_cast<FLOAT>(pInputs[i].x), static_cast<FLOAT>(pInputs[i].y));
//	bHandled = TRUE;
//}
//if (pInputs[i].dwFlags & TOUCHEVENTF_UP) {
//	g_pIManipProc->ProcessUp(pInputs[i].dwID, static_cast<FLOAT>(pInputs[i].x), static_cast<FLOAT>(pInputs[i].y));
//	bHandled = TRUE;
//}
//if (pInputs[i].dwFlags & TOUCHEVENTF_MOVE) {
//	g_pIManipProc->ProcessMove(pInputs[i].dwID, static_cast<FLOAT>(pInputs[i].x), static_cast<FLOAT>(pInputs[i].y));
//	bHandled = TRUE;
//}
