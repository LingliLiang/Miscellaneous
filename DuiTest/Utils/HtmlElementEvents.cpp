#include "StdAfx.h"
#include "HtmlElementEvents.h"
#include "ExDispID.h"

CHTMLDocEvent::CHTMLDocEvent()
	:m_dwRef(0),pHandler(NULL)
{

}


void CHTMLDocEvent::DocumentComplete(CComQIPtr<IHTMLDocument2> pDoc)
{
	if(!pDoc) return;

	CComPtr<IHTMLElementCollection> pElementColl;

	auto hr = pDoc->get_all(&pElementColl);
	if(SUCCEEDED(hr))
	{
		CComPtr<IDispatch> pInputDispatch;
		CComVariant varId(L"input");
		hr = pElementColl->tags(varId,&pInputDispatch);
		if(SUCCEEDED(hr))
		{
			CComPtr<IHTMLElementCollection> pInputColl;
			hr = pInputDispatch->QueryInterface(IID_IHTMLElementCollection,(LPVOID*)&pInputColl);
			if(SUCCEEDED(hr)){
				long nitems = 0;
				hr = pInputColl->get_length(&nitems);
				if(SUCCEEDED(hr)){
					for (long i=0;i<nitems;i++){
						CComVariant varIndex(i);
						CComPtr<IHTMLElement> pElem;
						CComPtr<IDispatch> pElemDispatch;
						hr = pInputColl->item( varIndex, varIndex, &pElemDispatch );
						if(FAILED(hr)) break;
						hr = pElemDispatch->QueryInterface(IID_IHTMLElement,(LPVOID*)&pElem);
						if(SUCCEEDED(hr))
						{
							ConnectEvents(pElem);
						}
					}
				}
			}//InputColl
		}//tags
	}

}


void CHTMLDocEvent::ConnectEvents(CComPtr<IHTMLElement> pElem)
{
	CComPtr<IConnectionPoint> pCp;
	CComPtr<IConnectionPointContainer> pCpc;
	DWORD dwCookie;
	//CComBSTR str;
	//CComBSTR strName;
	//pElem->get_id(&str);
	//pElem->get_tagName(&strName);
	//::OutputDebugStringW(str.m_str);
	//::OutputDebugStringA(" ");
	//::OutputDebugStringW(strName.m_str);
	CComPtr<IHTMLInputTextElement> pTextElement;
	auto hr = pElem->QueryInterface(IID_IHTMLInputTextElement,(LPVOID*)&pTextElement);
	if(FAILED(hr)) return;
	hr = pTextElement->QueryInterface(IID_IConnectionPointContainer,(LPVOID*)&pCpc);
	if(SUCCEEDED(hr))
	{
		/*HTMLInputTextElementEvents2*/
		//DIID_HTMLinputTextElementEvents2;
		hr = pCpc->FindConnectionPoint(DIID_HTMLInputTextElementEvents2,&pCp);
		if(SUCCEEDED(hr))
		{
			hr = pCp->Advise(this,&dwCookie);
			if(SUCCEEDED(hr))
			{}
		}
	}
}

HRESULT CHTMLDocEvent::Invoke( DISPID dispIdMember, 
	REFIID riid, LCID lcid, 
	WORD wFlags, 
	DISPPARAMS *pDispParams, 
	VARIANT *pVarResult, 
	EXCEPINFO *pExcepInfo, 
	UINT *puArgErr )
{
	VARIANT varlValue[1] = {};
	UINT cArg = 0;
	for(cArg = 0; cArg < pDispParams->cArgs; cArg++)
	{
		VariantInit(&varlValue[cArg]);
		VariantClear(&varlValue[cArg]);
		varlValue[cArg] = (pDispParams -> rgvarg)[cArg];
	}
	assert(cArg == 1);
	assert(varlValue[0].vt == VT_DISPATCH);
	IHTMLEventObj* pEvtObj = NULL;
	if(SUCCEEDED(varlValue[0].pdispVal->QueryInterface(IID_IHTMLEventObj,(LPVOID*)&pEvtObj)))
	{
	}
	else
		return S_OK;
	switch (dispIdMember)
	{
	case DISPID_HTMLELEMENTEVENTS2_onfocusin:
		onfocusin(pEvtObj);
		break;
	case DISPID_HTMLELEMENTEVENTS2_onfocusout:
		onfocusout(pEvtObj);
		break;
	default:
		break;
	}
	pEvtObj->Release();
	return S_OK;
}


VARIANT_BOOL CHTMLDocEvent::onhelp(IHTMLEventObj* pEvtObj){
	return pHandler ? VARIANT_FALSE : pHandler->onhelp(pEvtObj) ;
}

VARIANT_BOOL CHTMLDocEvent::onclick(__in IHTMLEventObj* pEvtObj){
	return pHandler ? VARIANT_FALSE : pHandler->onclick(pEvtObj) ;
}

VARIANT_BOOL CHTMLDocEvent::ondblclick(__in IHTMLEventObj* pEvtObj){
	return pHandler ? VARIANT_FALSE : pHandler->ondblclick(pEvtObj) ;
}

void CHTMLDocEvent::onkeydown(__in IHTMLEventObj* pEvtObj){
	if(pHandler)
	{
		pHandler->onkeydown(pEvtObj);
	}
}

void CHTMLDocEvent::onkeyup(__in IHTMLEventObj* pEvtObj){
	if(pHandler)
	{
		pHandler->onkeyup(pEvtObj);
	}
}

VARIANT_BOOL CHTMLDocEvent::onkeypress(__in IHTMLEventObj* pEvtObj){
	return pHandler ? VARIANT_FALSE : pHandler->onkeypress(pEvtObj) ;
}

void CHTMLDocEvent::onmousedown(__in IHTMLEventObj* pEvtObj){
	if(pHandler)
	{
		pHandler->onmousedown(pEvtObj);
	}
}

void CHTMLDocEvent::onmousemove(__in IHTMLEventObj* pEvtObj){
	if(pHandler)
	{
		pHandler->onmousemove(pEvtObj);
	}
}

void CHTMLDocEvent::onmouseup(__in IHTMLEventObj* pEvtObj){
	if(pHandler)
	{
		pHandler->onmouseup(pEvtObj);
	}
}

void CHTMLDocEvent::onmouseout(__in IHTMLEventObj* pEvtObj){
	if(pHandler)
	{
		pHandler->onmouseout(pEvtObj);
	}
}

void CHTMLDocEvent::onmouseover(__in IHTMLEventObj* pEvtObj){
	if(pHandler)
	{
		pHandler->onmouseover(pEvtObj);
	}
}

void CHTMLDocEvent::onreadystatechange(__in IHTMLEventObj* pEvtObj)
{
	if(pHandler)
	{
		pHandler->onreadystatechange(pEvtObj);
	}
}

VARIANT_BOOL CHTMLDocEvent::onbeforeupdate(__in IHTMLEventObj* pEvtObj){
	return pHandler ? VARIANT_FALSE : pHandler->onbeforeupdate(pEvtObj) ;
}

void CHTMLDocEvent::onafterupdate(__in IHTMLEventObj* pEvtObj){
	if(pHandler)
	{
		pHandler->onafterupdate(pEvtObj);
	}
}

VARIANT_BOOL CHTMLDocEvent::onrowexit(__in IHTMLEventObj* pEvtObj){
	return pHandler ? VARIANT_FALSE : pHandler->onrowexit(pEvtObj) ;
}

void CHTMLDocEvent::onrowenter(__in IHTMLEventObj* pEvtObj){
	if(pHandler)
	{
		pHandler->onrowenter(pEvtObj);
	}
}

VARIANT_BOOL CHTMLDocEvent::ondragstart(__in IHTMLEventObj* pEvtObj){
	return pHandler ? VARIANT_FALSE : pHandler->ondragstart(pEvtObj) ;
}

VARIANT_BOOL CHTMLDocEvent::onselectstart(__in IHTMLEventObj* pEvtObj){
	return pHandler ? VARIANT_FALSE : pHandler->onselectstart(pEvtObj) ;
}

VARIANT_BOOL CHTMLDocEvent::onerrorupdate(__in IHTMLEventObj* pEvtObj){
	return pHandler ? VARIANT_FALSE : pHandler->onerrorupdate(pEvtObj) ;
}

VARIANT_BOOL CHTMLDocEvent::oncontextmenu(__in IHTMLEventObj* pEvtObj){
	return pHandler ? VARIANT_FALSE : pHandler->oncontextmenu(pEvtObj) ;
}

VARIANT_BOOL CHTMLDocEvent::onstop(__in IHTMLEventObj* pEvtObj){
	return pHandler ? VARIANT_FALSE : pHandler->onstop(pEvtObj) ;
}

void CHTMLDocEvent::onrowsdelete(__in IHTMLEventObj* pEvtObj){
	if(pHandler)
	{
		pHandler->onrowsdelete(pEvtObj);
	}
}

void CHTMLDocEvent::onrowsinserted(__in IHTMLEventObj* pEvtObj){
	if(pHandler)
	{
		pHandler->onrowsinserted(pEvtObj);
	}
}

void CHTMLDocEvent::oncellchange(__in IHTMLEventObj* pEvtObj){
	if(pHandler)
	{
		pHandler->oncellchange(pEvtObj);
	}
}

void CHTMLDocEvent::onpropertychange(__in IHTMLEventObj* pEvtObj){
	if(pHandler)
	{
		pHandler->onpropertychange(pEvtObj);
	}
}

void CHTMLDocEvent::ondatasetchanged(__in IHTMLEventObj* pEvtObj){
	if(pHandler)
	{
		pHandler->ondatasetchanged(pEvtObj);
	}
}

void CHTMLDocEvent::ondataavailable(__in IHTMLEventObj* pEvtObj){
	if(pHandler)
	{
		pHandler->ondataavailable(pEvtObj);
	}
}

void CHTMLDocEvent::ondatasetcomplete(__in IHTMLEventObj* pEvtObj){
	if(pHandler)
	{
		pHandler->ondatasetcomplete(pEvtObj);
	}
}

void CHTMLDocEvent::onbeforeeditfocus(__in IHTMLEventObj* pEvtObj)
{
	if(pHandler)
	{
		pHandler->onbeforeeditfocus(pEvtObj);
	}
}

void CHTMLDocEvent::onselectionchange(__in IHTMLEventObj* pEvtObj){
	if(pHandler)
	{
		pHandler->onselectionchange(pEvtObj);
	}
}

VARIANT_BOOL CHTMLDocEvent::oncontrolselect(__in IHTMLEventObj* pEvtObj){
	return pHandler ? VARIANT_FALSE : pHandler->oncontrolselect(pEvtObj) ;
}

VARIANT_BOOL CHTMLDocEvent::onmousewheel(__in IHTMLEventObj* pEvtObj){
	return pHandler ? VARIANT_FALSE : pHandler->onmousewheel(pEvtObj) ;
}

void CHTMLDocEvent::onfocusin(__in IHTMLEventObj* pEvtObj){
	if(pHandler)
	{
		pHandler->onfocusin(pEvtObj);
	}
	::OutputDebugStringA("onfocusin\n");
	::TabtipVisible(TRUE);
}

void CHTMLDocEvent::onfocusout(__in IHTMLEventObj* pEvtObj){
	if(pHandler)
	{
		pHandler->onfocusout(pEvtObj);
	}
	::OutputDebugStringA("onfocusout\n");
	::TabtipVisible(FALSE);
}

void CHTMLDocEvent::onactivate(__in IHTMLEventObj* pEvtObj)
{
	if(pHandler)
	{
		pHandler->onactivate(pEvtObj);
	}
}

void CHTMLDocEvent::ondeactivate(__in IHTMLEventObj* pEvtObj){
	if(pHandler)
	{
		pHandler->ondeactivate(pEvtObj);
	}
}

VARIANT_BOOL CHTMLDocEvent::onbeforeactivate(__in IHTMLEventObj* pEvtObj){
	return pHandler ? VARIANT_FALSE : pHandler->onbeforeactivate(pEvtObj) ;
}

VARIANT_BOOL CHTMLDocEvent::onbeforedeactivate(__in IHTMLEventObj* pEvtObj){
	return pHandler ? VARIANT_FALSE : pHandler->onbeforedeactivate(pEvtObj) ;
}