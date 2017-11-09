#include "StdAfx.h"
#include "RenderCanvas.h"

namespace VCasterUI
{

	CRenderCanvas::CRenderCanvas(void)
		:m_bPopup(FALSE)
	{

	}

	CRenderCanvas::~CRenderCanvas(void)
	{
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
		//::SetClassLongPtr(m_hWnd,GCLP_HBRBACKGROUND,(LONG)GetStockObject(BLACK_BRUSH));
		if(cy < 0 && cx < 0)
		{
			::SetWindowPos(m_hWnd, NULL, 0, 0, rcParent.GetWidth(), rcParent.GetHeight(), SWP_FRAMECHANGED);
		}
		else
			::SetWindowPos(m_hWnd, NULL, 0, 0, cx, cy, SWP_FRAMECHANGED);

		return TRUE;
	}

	BOOL CRenderCanvas::ClipCanvasArea(const CRect rect)
	{
		if(!m_hWnd) return FALSE; 
		CRect rcClient = GetClientRect();
		HRGN hRgnMain = CreateRectRgn(0, 0, rcClient.GetWidth(), rcClient.GetHeight());
		int re = ::GetWindowRgn(m_hWnd, hRgnMain);
		HRGN hRgn = ::CreateRectRgn(rect.left,rect.top, rect.right, rect.bottom);
		::CombineRgn(hRgnMain, hRgnMain, hRgn, RGN_XOR);
		::SetWindowRgn(m_hWnd, hRgnMain, TRUE);
		DeleteObject(hRgn);  
		DeleteObject(hRgnMain);
		return TRUE;
	}

	BOOL CRenderCanvas::MakeCanvasArea(const std::vector<CRect> &vecRect)
	{
		if(!m_hWnd) return FALSE; 
		RECT rcClient;
		::GetClientRect(m_hWnd, &rcClient);
		HRGN hRgnMain = ::CreateRectRgn(0, 0, 0, 0);
		{
			for(size_t index = 0; index <vecRect.size(); ++index)
			{
				HRGN hRgn = ::CreateRectRgn(vecRect[index].left,vecRect[index].top,
					vecRect[index].right, vecRect[index].bottom);
				::CombineRgn(hRgnMain, hRgnMain, hRgn, RGN_OR);
				DeleteObject(hRgn);  
			}
		}
		::SetWindowRgn(m_hWnd, hRgnMain, TRUE);
		DeleteObject(hRgnMain);
		return TRUE;
	}

	BOOL CRenderCanvas::AddCanvasArea(const CRect rect)
	{
		if(!m_hWnd) return FALSE; 
		CRect rcClient = GetClientRect();
		HRGN hRgnMain = CreateRectRgn(0, 0, rcClient.GetWidth(), rcClient.GetHeight());
		int re = ::GetWindowRgn(m_hWnd, hRgnMain);
		HRGN hRgn = ::CreateRectRgn(rect.left,rect.top, rect.right, rect.bottom);
		::CombineRgn(hRgnMain, hRgnMain, hRgn, RGN_OR);
		::SetWindowRgn(m_hWnd, hRgnMain, TRUE);
		DeleteObject(hRgn);  
		DeleteObject(hRgnMain);
		return TRUE;
	}

	void CRenderCanvas::EmptyCanvasArea()
	{
		HRGN hRgnMain = ::CreateRectRgn(0, 0, 0, 0);
		::SetWindowRgn(m_hWnd, hRgnMain, TRUE);
		DeleteObject(hRgnMain);
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

} //namespace VCasterUI
