#pragma once

#include <MsHTML.h>
#include <ExDispid.h>
#include <atlcomcli.h>
#include <MsHtmdid.h>

class IDocumentEvents2
{
	//dispinterface HTMLDocumentEvents2
public:
	virtual VARIANT_BOOL onhelp(IHTMLEventObj* pEvtObj){return VARIANT_TRUE;}
	virtual VARIANT_BOOL onclick(__in IHTMLEventObj* pEvtObj){return VARIANT_TRUE;}
	virtual VARIANT_BOOL ondblclick(__in IHTMLEventObj* pEvtObj){return VARIANT_TRUE;}
	virtual void onkeydown(__in IHTMLEventObj* pEvtObj){}
	virtual void onkeyup(__in IHTMLEventObj* pEvtObj){}
	virtual VARIANT_BOOL onkeypress(__in IHTMLEventObj* pEvtObj){return VARIANT_TRUE;}
	virtual void onmousedown(__in IHTMLEventObj* pEvtObj){}
	virtual void onmousemove(__in IHTMLEventObj* pEvtObj){}
	virtual void onmouseup(__in IHTMLEventObj* pEvtObj){}
	virtual void onmouseout(__in IHTMLEventObj* pEvtObj){}
	virtual void onmouseover(__in IHTMLEventObj* pEvtObj){}
	virtual void onreadystatechange(__in IHTMLEventObj* pEvtObj){}
	virtual VARIANT_BOOL onbeforeupdate(__in IHTMLEventObj* pEvtObj){return VARIANT_TRUE;}
	virtual void onafterupdate(__in IHTMLEventObj* pEvtObj){}
	virtual VARIANT_BOOL onrowexit(__in IHTMLEventObj* pEvtObj){return VARIANT_TRUE;}
	virtual void onrowenter(__in IHTMLEventObj* pEvtObj){}
	virtual VARIANT_BOOL ondragstart(__in IHTMLEventObj* pEvtObj){return VARIANT_TRUE;}
	virtual VARIANT_BOOL onselectstart(__in IHTMLEventObj* pEvtObj){return VARIANT_TRUE;}
	virtual VARIANT_BOOL onerrorupdate(__in IHTMLEventObj* pEvtObj){return VARIANT_TRUE;}
	virtual VARIANT_BOOL oncontextmenu(__in IHTMLEventObj* pEvtObj){return VARIANT_TRUE;}
	virtual VARIANT_BOOL onstop(__in IHTMLEventObj* pEvtObj){return VARIANT_TRUE;}
	virtual void onrowsdelete(__in IHTMLEventObj* pEvtObj){}
	virtual void onrowsinserted(__in IHTMLEventObj* pEvtObj){}
	virtual void oncellchange(__in IHTMLEventObj* pEvtObj){}
	virtual void onpropertychange(__in IHTMLEventObj* pEvtObj){}
	virtual void ondatasetchanged(__in IHTMLEventObj* pEvtObj){}
	virtual void ondataavailable(__in IHTMLEventObj* pEvtObj){}
	virtual void ondatasetcomplete(__in IHTMLEventObj* pEvtObj){}
	virtual void onbeforeeditfocus(__in IHTMLEventObj* pEvtObj){}
	virtual void onselectionchange(__in IHTMLEventObj* pEvtObj){}
	virtual VARIANT_BOOL oncontrolselect(__in IHTMLEventObj* pEvtObj){return VARIANT_TRUE;}
	virtual VARIANT_BOOL onmousewheel(__in IHTMLEventObj* pEvtObj){return VARIANT_TRUE;}
	virtual void onfocusin(__in IHTMLEventObj* pEvtObj){}
	virtual void onfocusout(__in IHTMLEventObj* pEvtObj){}
	virtual void onactivate(__in IHTMLEventObj* pEvtObj){}
	virtual void ondeactivate(__in IHTMLEventObj* pEvtObj){}
	virtual VARIANT_BOOL onbeforeactivate(__in IHTMLEventObj* pEvtObj){return VARIANT_TRUE;}
	virtual VARIANT_BOOL onbeforedeactivate(__in IHTMLEventObj* pEvtObj){return VARIANT_TRUE;}
};

class  CHTMLDocEvent :
	public HTMLDocumentEvents2 
{
public:
	CHTMLDocEvent();

	// IUnknown
	STDMETHOD_(ULONG,AddRef)()
	{
		InterlockedIncrement(&m_dwRef); 
		ATLTRACE(_T("CHTMLDocEvent add ref %d \n"), m_dwRef);
		return m_dwRef;
	}
	STDMETHOD_(ULONG,Release)()
	{
		ULONG ulRefCount = InterlockedDecrement(&m_dwRef);
		ATLTRACE(_T("CHTMLDocEvent rel ref %d \n"), m_dwRef);		
		if(ulRefCount == 0)
			delete this;
		return ulRefCount; 
	}
	STDMETHOD(QueryInterface)(REFIID riid, LPVOID *ppvObject)
	{
		*ppvObject = NULL;

		if (riid == IID_IDispatch)
			*ppvObject = static_cast<IDispatch*>(this);
		///*else if (riid == DIID_HTMLElementEvents2)
		//	*ppvObject = static_cast<HTMLDocumentEvents2*>(this);*/
		if( *ppvObject != NULL )
			AddRef();
		return *ppvObject == NULL ? E_NOINTERFACE : S_OK;
	}

	// IDispatch
	virtual HRESULT STDMETHODCALLTYPE GetTypeInfoCount( __RPC__out UINT *pctinfo )
	{
		*pctinfo = 0;
		return E_NOTIMPL;
	}
	virtual HRESULT STDMETHODCALLTYPE GetTypeInfo( UINT iTInfo, LCID lcid, __RPC__deref_out_opt ITypeInfo **ppTInfo )
	{
		return E_NOTIMPL;
	}
	virtual HRESULT STDMETHODCALLTYPE GetIDsOfNames( __RPC__in REFIID riid, __RPC__in_ecount_full(cNames ) LPOLESTR *rgszNames, UINT cNames, LCID lcid, __RPC__out_ecount_full(cNames) DISPID *rgDispId)

	{
		return E_NOTIMPL;
	}
	virtual HRESULT STDMETHODCALLTYPE Invoke( DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS *pDispParams, VARIANT *pVarResult, EXCEPINFO *pExcepInfo, UINT *puArgErr );
private:
	LONG m_dwRef;


	IDocumentEvents2* pHandler;
public:
	void DocumentComplete(CComQIPtr<IHTMLDocument2> pDoc);
	void ConnectEvents(CComPtr<IHTMLElement> pElem);

	//dispinterface HTMLDocumentEvents2
	const static int DISPID_HTMLELEMENTEVENTS2_onhelp = 0x8001000a;
	VARIANT_BOOL onhelp(IHTMLEventObj* pEvtObj);
	const static int DISPID_HTMLELEMENTEVENTS2_onclick = 0xfffffda8;
	VARIANT_BOOL onclick(__in IHTMLEventObj* pEvtObj);
	const static int DISPID_HTMLELEMENTEVENTS2_ondblclick = 0xfffffda7;
	VARIANT_BOOL ondblclick(__in IHTMLEventObj* pEvtObj);
	const static int DISPID_HTMLELEMENTEVENTS2_onkeydown = 0xfffffda6;
	void onkeydown(__in IHTMLEventObj* pEvtObj);
	const static int DISPID_HTMLELEMENTEVENTS2_onkeyup = 0xfffffda4;
	void onkeyup(__in IHTMLEventObj* pEvtObj);
	const static int DISPID_HTMLELEMENTEVENTS2_onkeypress = 0xfffffda5;
	VARIANT_BOOL onkeypress(__in IHTMLEventObj* pEvtObj);
	const static int DISPID_HTMLELEMENTEVENTS2_onmousedown = 0xfffffda3;
	void onmousedown(__in IHTMLEventObj* pEvtObj);
	const static int DISPID_HTMLELEMENTEVENTS2_onmousemove = 0xfffffda2;
	void onmousemove(__in IHTMLEventObj* pEvtObj);
	const static int DISPID_HTMLELEMENTEVENTS2_onmouseup = 0xfffffda1;
	void onmouseup(__in IHTMLEventObj* pEvtObj);
	const static int DISPID_HTMLELEMENTEVENTS2_onmouseout = 0x80010009;
	void onmouseout(__in IHTMLEventObj* pEvtObj);
	const static int DISPID_HTMLELEMENTEVENTS2_onmouseover = 0x80010008;
	void onmouseover(__in IHTMLEventObj* pEvtObj);
	const static int DISPID_HTMLELEMENTEVENTS2_onreadystatechange = 0xfffffd9f;
	void onreadystatechange(__in IHTMLEventObj* pEvtObj);
	const static int DISPID_HTMLELEMENTEVENTS2_onbeforeupdate = 0x80010004;
	VARIANT_BOOL onbeforeupdate(__in IHTMLEventObj* pEvtObj);
	const static int DISPID_HTMLELEMENTEVENTS2_onafterupdate = 0x80010005;
	void onafterupdate(__in IHTMLEventObj* pEvtObj);
	const static int DISPID_HTMLELEMENTEVENTS2_onrowexit = 0x80010006;
	VARIANT_BOOL onrowexit(__in IHTMLEventObj* pEvtObj);
	const static int DISPID_HTMLELEMENTEVENTS2_onrowenter = 0x80010007;
	void onrowenter(__in IHTMLEventObj* pEvtObj);
	const static int DISPID_HTMLELEMENTEVENTS2_ondragstart = 0x8001000b;
	VARIANT_BOOL ondragstart(__in IHTMLEventObj* pEvtObj);
	const static int DISPID_HTMLELEMENTEVENTS2_onselectstart = 0x8001000c;
	VARIANT_BOOL onselectstart(__in IHTMLEventObj* pEvtObj);
	const static int DISPID_HTMLELEMENTEVENTS2_onerrorupdate = 0x8001000d;
	VARIANT_BOOL onerrorupdate(__in IHTMLEventObj* pEvtObj);
	const static int DISPID_HTMLELEMENTEVENTS2_oncontextmenu = 0x000003ff;
	VARIANT_BOOL oncontextmenu(__in IHTMLEventObj* pEvtObj);
	const static int DISPID_HTMLELEMENTEVENTS2_onstop = 0x00000402;
	VARIANT_BOOL onstop(__in IHTMLEventObj* pEvtObj);
	const static int DISPID_HTMLELEMENTEVENTS2_onrowsdelete = 0x80010020;
	void onrowsdelete(__in IHTMLEventObj* pEvtObj);
	const static int DISPID_HTMLELEMENTEVENTS2_onrowsinserted = 0x80010021;
	void onrowsinserted(__in IHTMLEventObj* pEvtObj);
	const static int DISPID_HTMLELEMENTEVENTS2_oncellchange = 0x80010022;
	void oncellchange(__in IHTMLEventObj* pEvtObj);
	const static int DISPID_HTMLELEMENTEVENTS2_onpropertychange = 0x80010013;
	void onpropertychange(__in IHTMLEventObj* pEvtObj);
	const static int DISPID_HTMLELEMENTEVENTS2_ondatasetchanged = 0x8001000e;
	void ondatasetchanged(__in IHTMLEventObj* pEvtObj);
	const static int DISPID_HTMLELEMENTEVENTS2_ondataavailable = 0x8001000f;
	void ondataavailable(__in IHTMLEventObj* pEvtObj);
	const static int DISPID_HTMLELEMENTEVENTS2_ondatasetcomplete = 0x80010010;
	void ondatasetcomplete(__in IHTMLEventObj* pEvtObj);
	const static int DISPID_HTMLELEMENTEVENTS2_onbeforeeditfocus = 0x00000403;
	void onbeforeeditfocus(__in IHTMLEventObj* pEvtObj);
	const static int DISPID_HTMLELEMENTEVENTS2_onselectionchange = 0x0000040d;
	void onselectionchange(__in IHTMLEventObj* pEvtObj);
	const static int DISPID_HTMLELEMENTEVENTS2_oncontrolselect = 0x0000040c;
	VARIANT_BOOL oncontrolselect(__in IHTMLEventObj* pEvtObj);
	const static int DISPID_HTMLELEMENTEVENTS2_onmousewheel = 0x00000409;
	VARIANT_BOOL onmousewheel(__in IHTMLEventObj* pEvtObj);
	const static int DISPID_HTMLELEMENTEVENTS2_onfocusin = 0x00000418;
	void onfocusin(__in IHTMLEventObj* pEvtObj);
	const static int DISPID_HTMLELEMENTEVENTS2_onfocusout = 0x00000419;
	void onfocusout(__in IHTMLEventObj* pEvtObj);
	const static int DISPID_HTMLELEMENTEVENTS2_onactivate = 0x00000414;
	void onactivate(__in IHTMLEventObj* pEvtObj);
	const static int DISPID_HTMLELEMENTEVENTS2_ondeactivate = 0x00000415;
	void ondeactivate(__in IHTMLEventObj* pEvtObj);
	const static int DISPID_HTMLELEMENTEVENTS2_onbeforeactivate = 0x00000417;
	VARIANT_BOOL onbeforeactivate(__in IHTMLEventObj* pEvtObj);
	const static int DISPID_HTMLELEMENTEVENTS2_onbeforedeactivate = 0x0000040a;
	VARIANT_BOOL onbeforedeactivate(__in IHTMLEventObj* pEvtObj);
};