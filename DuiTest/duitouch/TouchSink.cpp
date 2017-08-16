#include "stdafx.h"
#include "TouchSink.h"

CManipulationEventSink::CManipulationEventSink(IManipulationProcessor *manip, IInertiaProcessor *pInert, HWND hWnd)
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

	//Advise.
	hr = m_pConnPoint->Advise(this, &dwCookie);

	m_pInert = pInert;
	fExtrapolating=FALSE;
}


CManipulationEventSink::CManipulationEventSink(IInertiaProcessor *pInert, HWND hWnd)
{
	m_hWnd = hWnd;

	m_pInert = pInert;
	//Set initial ref count to 1.
	m_cRefCount = 1;

	fExtrapolating=TRUE;

	m_cStartedEventCount = 0;
	m_cDeltaEventCount = 0;
	m_cCompletedEventCount = 0;

	HRESULT hr = S_OK;

	//Get the container with the connection points.
	IConnectionPointContainer* spConnectionContainer;

	hr = pInert->QueryInterface(
		IID_IConnectionPointContainer, 
		(LPVOID*) &spConnectionContainer
		);
	if (spConnectionContainer == NULL){
		// something went wrong, try to gracefully quit        
	}

	//Get a connection point.
	hr = spConnectionContainer->FindConnectionPoint(__uuidof(_IManipulationEvents), &m_pConnPoint);
	if (m_pConnPoint == NULL){
		// something went wrong, try to gracefully quit
	}

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

void CManipulationEventSink::FreeConnect()
{
	HRESULT hr = S_OK;
	if(m_pConnPoint)
	{
		hr = m_pConnPoint->Unadvise(dwCookie);
	}
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


	// set origins in manipulation processor
	m_pInert->put_InitialOriginX(x);
	m_pInert->put_InitialOriginY(y);

	RECT screenRect;

	HWND desktop = GetDesktopWindow();
	GetClientRect(desktop, &screenRect);

	// physics settings
	// deceleration is units per square millisecond
	m_pInert->put_DesiredDeceleration(.1f);

	// set the boundaries        
	screenRect.left-= 1024;
	m_pInert->put_BoundaryLeft  ( static_cast<float>(screenRect.left   * 100));
	m_pInert->put_BoundaryTop   ( static_cast<float>(screenRect.top    * 100));
	m_pInert->put_BoundaryRight ( static_cast<float>(screenRect.right  * 100));
	m_pInert->put_BoundaryBottom( static_cast<float>(screenRect.bottom * 100));


	// Elastic boundaries - I set these to 90% of the screen 
	// so... 5% at left, 95% right, 5% top,  95% bottom
	// Values are whole numbers because units are in centipixels
	m_pInert->put_ElasticMarginLeft  (static_cast<float>(screenRect.left   * 5));
	m_pInert->put_ElasticMarginTop   (static_cast<float>(screenRect.top    * 5));
	m_pInert->put_ElasticMarginRight (static_cast<float>(screenRect.right  * 95));
	m_pInert->put_ElasticMarginBottom(static_cast<float>(screenRect.bottom * 95));
	::OutputDebugStringA("CManipulationEventSink::ManipulationStarted\n");

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
	if (fExtrapolating){
		::OutputDebugStringA("CManipulationEventSink::ManipulationCompleted KillTimer\n");
		//Inertia Complete, stop the timer used for processing      
		KillTimer(m_hWnd,TIMER_INERTPROC);        
	}else{ 
		// setup velocities for inertia processor
		float vX, vY, vA = 0.0f;
		m_pManip->GetVelocityX(&vX);
		m_pManip->GetVelocityY(&vY);
		m_pManip->GetAngularVelocity(&vA);

		// complete any previous processing
		m_pInert->Complete();

		// Reset sets the  initial timestamp
		m_pInert->Reset();

		// 
		m_pInert->put_InitialVelocityX(vX);
		m_pInert->put_InitialVelocityY(vY);        

		m_pInert->put_InitialOriginX(x);
		m_pInert->put_InitialOriginY(y);

		::OutputDebugStringA("CManipulationEventSink::ManipulationCompleted SetTimer\n");

		// Start a timer
		SetTimer(m_hWnd,TIMER_INERTPROC, 50, 0);        
	}
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
//
////Smart Pointer to a global reference of a manipulation processor, event sink
//CComPtr<IManipulationProcessor> g_pIManipProc;
//
//void init() {
//	HRESULT hr = CoInitialize(0);
//
//	hr = CoCreateInstance(CLSID_ManipulationProcessor,
//		NULL,
//		CLSCTX_INPROC_SERVER,
//		IID_IUnknown,
//		(VOID**)(&g_pIManipProc)
//		);
//
//}

//// Set up a variable to point to the manipulation event sink implementation class    
//CComPtr<CManipulationEventSink> g_pManipulationEventSink(new CManipulationEventSink(g_pIManipProc, 0));

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
