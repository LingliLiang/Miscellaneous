#include "StdAfx.h"
#include "DropFiles.h"

//const CLIPFORMAT CF_URL = static_cast<CLIPFORMAT>(RegisterClipboardFormat(_T("UniformResourceLocator")));

static void GetDropFiles(std::vector<CString>& slFiles, IDataObject *pdto)
{
	slFiles.clear();
	FORMATETC fmte = { CF_HDROP, NULL, DVASPECT_CONTENT,
		-1, TYMED_HGLOBAL };
	STGMEDIUM stgm;
	if (SUCCEEDED(pdto->GetData(&fmte, &stgm))) {
		HDROP hdrop = reinterpret_cast<HDROP>(stgm.hGlobal);
		UINT count = 0;
		TCHAR filePath[MAX_PATH] = {0};
		count = DragQueryFile(hdrop, 0xFFFFFFFF, NULL, 0);
		if(count)
		{        
			for(UINT i=0; i<count; i++)
			{
				int pathLen = DragQueryFile(hdrop, i, filePath, sizeof(filePath));
				slFiles.push_back(CString(filePath));
			}
		}
		DragFinish(hdrop);
		ReleaseStgMedium(&stgm);
	}
}


void DropTargetHandler::AcceptFromDataObject(IDataObject *pdto,DWORD dropEffect)
{
	if (!hTarget) { /* need to create a new main window */ }
	std::vector<CString> slFiles;
	GetDropFiles(slFiles, pdto);
	if(m_pClient)
		m_pClient->OnDropFiles(slFiles,dropEffect);
}


bool DropTargetHandler::DragAccept(IDropClient* pClient,HWND hWnd)
{
	assert(hWnd);
	hTarget = hWnd;
	m_pClient = pClient;
	HRESULT hr = m_pDropHelper.CoCreateInstance(CLSID_DragDropHelper, nullptr, CLSCTX_INPROC_SERVER);
	m_spDropTarget.Attach(new DropTargetImpl());
	m_spDropTarget->AttchHandler(this);
	hr =  ::RegisterDragDrop(hTarget,m_spDropTarget);
	if(FAILED(hr))
	{
		m_spDropTarget.Release();
		return false;
	}
	return true;
}


bool	DropTargetHandler::DragReject()
{
	HRESULT hr = ::RevokeDragDrop(hTarget);
	m_pClient = NULL;
	hTarget = NULL;
	return true;
}


DropTargetHandler::DropTargetHandler()
	:m_pClient(NULL),hTarget(NULL),m_effectLast(0)
{
}


STDMETHODIMP DropTargetHandler::DropTargetImpl::DragEnter(IDataObject *pdto,
	DWORD grfKeyState, POINTL ptl, DWORD *pdwEffect)
{
	CPoint point(ptl.x,ptl.y);
	*pdwEffect &= DROPEFFECT_NONE;

	if(m_pHandler)
	{	
		if(m_pHandler->m_pClient)
		{
			std::vector<CString> slFiles;
			GetDropFiles(slFiles, pdto);
			*pdwEffect = m_pHandler->m_pClient->OnDropAccept(slFiles, grfKeyState, point);
			m_pHandler->m_effectLast = *pdwEffect;
		}
		m_pHandler->m_pDropHelper->DragEnter(m_pHandler->hTarget,pdto,&point,*pdwEffect);
	}
	return S_OK;
}


STDMETHODIMP DropTargetHandler::DropTargetImpl::DragOver(DWORD grfKeyState,
	POINTL ptl, DWORD *pdwEffect)
{
	CPoint point(ptl.x,ptl.y);
	if(m_pHandler)
	{
		*pdwEffect &= m_pHandler->m_effectLast;
		m_pHandler->m_pDropHelper->DragOver(&point,*pdwEffect);
	}
	return S_OK;
}


STDMETHODIMP DropTargetHandler::DropTargetImpl::DragLeave()
{
	if (m_pHandler) {
        m_pHandler->m_pDropHelper->DragLeave();
    }
	return S_OK;
}


STDMETHODIMP DropTargetHandler::DropTargetImpl::Drop(IDataObject *pdto, DWORD grfKeyState,
	POINTL ptl, DWORD *pdwEffect)
{
	CPoint point(ptl.x,ptl.y);
	if (m_pHandler) {
		m_pHandler->AcceptFromDataObject(pdto,*pdwEffect);
		m_pHandler->m_pDropHelper->Drop(pdto, &point, *pdwEffect);
	}
	return S_OK;
}