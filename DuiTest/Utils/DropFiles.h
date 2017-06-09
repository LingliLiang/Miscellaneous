#pragma once

#include <shlobj.h>
#include <ShellAPI.h>


struct IDropClient {
	virtual void OnDropFiles(std::vector<CString>& slFiles, DWORD dropEffect) PURE;
    virtual DWORD OnDropAccept(std::vector<CString>& slFiles, DWORD dwKeyState, CPoint point) PURE;
};

class DropTargetHandler 
{
	class DropTargetImpl : public IDropTarget
	{
	public:
		DropTargetImpl() : m_cRef(1),m_pHandler(0){}
		~DropTargetImpl() {}

		STDMETHODIMP QueryInterface(REFIID riid, void **ppv)
		{
			if (riid == IID_IUnknown || riid == IID_IDropTarget) {
				*ppv = static_cast<IUnknown*>(this);
				AddRef();
				return S_OK;
			}
			*ppv = NULL;
			return E_NOINTERFACE;
		}

		STDMETHODIMP_(ULONG) AddRef()
		{
			return InterlockedIncrement(&m_cRef);
		}

		STDMETHODIMP_(ULONG) Release()
		{
			LONG cRef = InterlockedDecrement(&m_cRef);
			if (cRef == 0) delete this;
			return cRef;
		}

		STDMETHODIMP DragEnter(IDataObject *pdto,
			DWORD grfKeyState, POINTL ptl, DWORD *pdwEffect);

		STDMETHODIMP DragOver(DWORD grfKeyState,
			POINTL ptl, DWORD *pdwEffect);

		STDMETHODIMP DragLeave();

		STDMETHODIMP Drop(IDataObject *pdto, DWORD grfKeyState,
			POINTL ptl, DWORD *pdwEffect);

		STDMETHODIMP AttchHandler(DropTargetHandler* pHandler)
		{
			m_pHandler = pHandler;
			return S_OK;
		}
	private:
		LONG m_cRef;
		DropTargetHandler* m_pHandler;
	};
	friend class DropTargetImpl;
public:
	DropTargetHandler();
	~DropTargetHandler() {}
	bool DragAccept(IDropClient* pClient, HWND hWnd);
	bool	DragReject(); // clear before window close.
protected:
	void AcceptFromDataObject(IDataObject *pdto,DWORD dropEffect);

private:
	HWND hTarget;
	IDropClient* m_pClient;
	DWORD m_effectLast;
	CComPtr<DropTargetImpl> m_spDropTarget;
	CComPtr<IDropTargetHelper> m_pDropHelper;
};

