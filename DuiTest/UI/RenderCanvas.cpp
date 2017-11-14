#include "StdAfx.h"
#include "RenderCanvas.h"

CRenderCanvas::CRenderCanvas(void)
	:m_hParent(NULL),m_ppPtr(NULL),m_bPopup(FALSE),m_hRgnCurrent(NULL)
{
	m_hRgnCurrent = ::CreateRectRgn(0, 0, 0, 0);
}

CRenderCanvas::~CRenderCanvas(void)
{
	if(m_hRgnCurrent){
		DeleteObject(m_hRgnCurrent);
	}
}

LPCTSTR CRenderCanvas::GetWindowClassName() const
{
	return _T("RenderCanvas");
}

UINT CRenderCanvas::GetClassStyle() const
{
	return CS_DBLCLKS;
}

void CRenderCanvas::OnFinalMessage(HWND hWnd)
{
	m_hWnd = NULL;
	if(m_ppPtr && *m_ppPtr) *m_ppPtr = NULL;
	OutputDebugString(L"CRenderCanvas::OnFinalMessage(HWND hWnd)\n");
	delete this;
}

void CRenderCanvas::SetFianlEmptyPtr(LPVOID* ppPtr)
{
	m_ppPtr = ppPtr;
}

BOOL CRenderCanvas::CreateCanvas(HWND hParent, BOOL childstyle, BOOL visible, int cx , int cy )
{
	CRect rcParent;
	::GetClientRect(hParent, &rcParent);
	m_bPopup = FALSE;
	auto style = UI_WNDSTYLE_CHILD;
	if(!visible) style &= ~WS_VISIBLE;
	if(!childstyle){
		style &= ~WS_CHILD;
		style |= WS_POPUP;
		m_bPopup = TRUE;
	}
	Create(hParent, GetWindowClassName(), style,WS_EX_NOACTIVATE|WS_EX_NOPARENTNOTIFY);
	//::SetClassLongPtr(m_hWnd,GCLP_HBRBACKGROUND,(LONG_PTR)GetStockObject(BLACK_BRUSH));
	if(cy < 0 && cx < 0)
	{
		::SetWindowPos(m_hWnd, NULL, 0, 0, rcParent.GetWidth(), rcParent.GetHeight(), SWP_FRAMECHANGED);
	}
	else
		::SetWindowPos(m_hWnd, NULL, 0, 0, cx, cy, SWP_FRAMECHANGED);
	m_hParent = hParent;
	return TRUE;
}

BOOL CRenderCanvas::ClipCanvasArea(const CRect rect)
{
	if(!m_hWnd) return FALSE; 
	HRGN hRgn = ::CreateRectRgn(rect.left,rect.top, rect.right, rect.bottom);
	::CombineRgn(m_hRgnCurrent, m_hRgnCurrent, hRgn, RGN_XOR);
	HRGN hRgnWnd =  ::CreateRectRgn(0, 0, 0, 0);
	::CombineRgn(hRgnWnd, m_hRgnCurrent, NULL, RGN_COPY); //拷贝当前到窗口区域
	::SetWindowRgn(m_hWnd, hRgnWnd, TRUE); //不需要delete hRgnWnd,系统处理
	DeleteObject(hRgn);  
	return TRUE;
}

BOOL CRenderCanvas::MakeCanvasArea(const std::vector<CRect> &vecRect)
{
	if(!m_hWnd) return FALSE; 
	ResetCurrentRgn();
	RECT rcClient;
	::GetClientRect(m_hWnd, &rcClient);
	HRGN hRgnWnd = ::CreateRectRgn(0, 0, 0, 0);
	{
		for(size_t index = 0; index <vecRect.size(); ++index)
		{
			HRGN hRgn = ::CreateRectRgn(vecRect[index].left,vecRect[index].top,
				vecRect[index].right, vecRect[index].bottom);
			::CombineRgn(m_hRgnCurrent, m_hRgnCurrent, hRgn, RGN_OR);
			DeleteObject(hRgn);  
		}
	}
	::CombineRgn(hRgnWnd, m_hRgnCurrent, NULL, RGN_COPY);
	::SetWindowRgn(m_hWnd, hRgnWnd, TRUE);
	return TRUE;
}

BOOL CRenderCanvas::AddCanvasArea(const CRect rect)
{
	if(!m_hWnd) return FALSE; 
	//CRect rcClient = GetClientRect();
	//HRGN hRgnMain = CreateRectRgn(0, 0, rcClient.GetWidth(), rcClient.GetHeight());
	//int re = ::GetWindowRgn(m_hWnd, hRgnMain);
	HRGN hRgn = ::CreateRectRgn(rect.left,rect.top, rect.right, rect.bottom);
	::CombineRgn(m_hRgnCurrent, m_hRgnCurrent, hRgn, RGN_OR);

	HRGN hRgnWnd =  ::CreateRectRgn(0, 0, 0, 0);
	::CombineRgn(hRgnWnd, m_hRgnCurrent, NULL, RGN_COPY);

	::SetWindowRgn(m_hWnd, hRgnWnd, TRUE);
	DeleteObject(hRgn);  
	return TRUE;
}

void CRenderCanvas::EmptyCanvasArea()
{
	ResetCurrentRgn();
	HRGN hRgnWnd = ::CreateRectRgn(0, 0, 0, 0);
	::SetWindowRgn(m_hWnd, hRgnWnd, TRUE);
}

BOOL CRenderCanvas::IsPopupWindows()
{
	return m_bPopup;
}

CRect  CRenderCanvas::GetClientRect()
{
	RECT rcClient;
	::GetClientRect(m_hWnd, &rcClient);
	return rcClient;
}

LRESULT CRenderCanvas::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if(m_PreHandleMessage.size())
	{
		BOOL bHandle = FALSE;
		LRESULT lres = 0;
		for(auto iter = m_PreHandleMessage.begin(); m_PreHandleMessage.end() != iter; ++iter)
		{
			lres = (*iter)(uMsg, wParam, lParam, bHandle);
			if(bHandle) return lres;
		}
	}
	return CWindowWnd::HandleMessage(uMsg, wParam, lParam);
}

void CRenderCanvas::ResetCurrentRgn(){
	if(m_hRgnCurrent){
		DeleteObject(m_hRgnCurrent);
		m_hRgnCurrent = ::CreateRectRgn(0, 0, 0, 0);;
	}
}

void CRenderCanvas::SetIntersectionRect(CRect rc)
{
	if(!rc.IsNull()){
		m_rcIntersection = rc;
	}
}

void CRenderCanvas::AreaScrollWithIntersection(int x, int y)
{
	if(!m_hWnd || (x==0 && y==0)) return ; 
	CRect& rc = m_rcIntersection;
	if(!rc.IsNull())
	{
		rc.Offset(x,y);
		HRGN hRgn = ::CreateRectRgn(rc.left,rc.top, rc.right, rc.bottom);
		int rel = ::CombineRgn(hRgn, hRgn, m_hRgnCurrent, RGN_AND);
		if(rel == ERROR)
		{}
		::SetWindowRgn(m_hWnd, hRgn, TRUE);
	}
	CRect rcWnd;
	::GetWindowRect(m_hWnd, &rcWnd);
	CPoint pt(rcWnd.left,rcWnd.top);
	if(m_bPopup){
	}
	else
	{
		::ScreenToClient(m_hParent, &pt);
	}
	pt.x -= x;
	pt.y -= y;
	::SetWindowPos(m_hWnd, NULL, pt.x, pt.y, 0, 0, SWP_NOSENDCHANGING | SWP_NOSIZE | SWP_NOACTIVATE);
}

void CRenderCanvas::AreaScrollReset()
{

}