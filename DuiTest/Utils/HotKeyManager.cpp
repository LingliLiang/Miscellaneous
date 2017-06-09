#include "stdafx.h"
#include "HotKeyManager.h"
#include  "VKeyDefines.h"
#include <algorithm>
#ifndef KEY_SHIFT
#define KEY_SHIFT 0x01
#define KEY_CTRL 0x02
#define KEY_ALT 0x04
#define KEY_WIN 0x08
#endif


////////////////////////////////////////////////////////////////////////////////////
COMDELEGATE::CCommonDelegate::CCommonDelegate(void * pO, void * pF)
	:m_pO(pO), m_pF(pF)
{
}

COMDELEGATE::CCommonDelegate::CCommonDelegate(const CCommonDelegate & cdg)
	: m_pO(cdg.m_pO), m_pF(cdg.m_pF)
{
}

COMDELEGATE::CCommonDelegate::~CCommonDelegate()
{
}

bool COMDELEGATE::CCommonDelegate::operator()(void * param)
{
	return Invoke(param);
}

bool COMDELEGATE::CCommonDelegate::operator==(const CCommonDelegate & cdg) const
{
	return  m_pO == cdg.m_pO && m_pF == cdg.m_pF;
}

void * COMDELEGATE::CCommonDelegate::GetO()
{
	return m_pO;
}

void * COMDELEGATE::CCommonDelegate::GetF()
{
	return m_pF;
}

COMDELEGATE::CEvent::~CEvent()
{
	ITER_DG it = m_lDelegates.begin();
	while (it != m_lDelegates.end())
	{
		if (*it) delete (*it);
		it++;
	}
}

COMDELEGATE::CEvent::operator bool()
{
	return m_lDelegates.size() > 0;
}

void COMDELEGATE::CEvent::operator=(FnType pFn)
{
	(*this) += CreateDelegate<CObjNull,CObjNull>(pFn);
}

void COMDELEGATE::CEvent::operator+=(const CCommonDelegate & cgd)
{
	ITER_DG it = m_lDelegates.begin();
	while (it != m_lDelegates.end())
	{
		if ((*it) && cgd == (*(*it))) return;
		it++;
	}
	m_lDelegates.push_back(cgd.newCopy());
}

void COMDELEGATE::CEvent::operator+=(CCommonDelegate * cgd)
{
	m_lDelegates.push_back(cgd);
}

void COMDELEGATE::CEvent::operator+=(FnType pFn)
{
	(*this) += CreateDelegate<CObjNull,CObjNull>(pFn);
}

void COMDELEGATE::CEvent::operator-=(const CCommonDelegate & cgd)
{
	CCommonDelegate* pObject = NULL;
	ITER_DG it = m_lDelegates.begin();
	while (it != m_lDelegates.end())
	{
		if ((*it) && cgd == (*(*it)))
		{
			m_lDelegates.remove(*it);
			break;
		}
		it++;
	}
	if (pObject)
	{
		delete pObject;
		pObject = NULL;
	}
}

void COMDELEGATE::CEvent::operator-=(CCommonDelegate * cgd)
{
	m_lDelegates.remove(cgd);
}

void COMDELEGATE::CEvent::operator-=(FnType pFn)
{
	(*this) -= CreateDelegate<CObjNull,CObjNull>(pFn);
}

bool COMDELEGATE::CEvent::operator()(void * param)
{
	ITER_DG it = m_lDelegates.begin();
	while (it != m_lDelegates.end())
	{
		if ((*it))
		{
			if (!(*it)->operator()(param))
				return false;
		}
		it++;
	}
	return true;
}

////////////////////////////////////////////////////////////////////////////////////

unsigned long ACCELCTL::CUniqueIdPool::GetId()
{
	std::map<unsigned long,bool>::iterator iter = m_mPool.end();
	bool bTemp = false;
	if(m_nNewRelease != 0)
	{
		m_mPool[m_nNewRelease] = true;
		return m_nNewRelease;
	}
	iter = std::find_if(m_mPool.begin(),m_mPool.end(),ACCELCTL::CMapValueFinder<unsigned long,bool>(bTemp));
	if(iter != m_mPool.end())
	{
		iter->second = true;
		return iter->first;
	}
	std::pair<unsigned long,bool> addpair = std::make_pair((unsigned long)(m_nBase+m_mPool.size()),true);
	m_mPool.insert(addpair);
	return addpair.first;
}

void ACCELCTL::CUniqueIdPool::ReleaseId(unsigned long nId)
{
	std::map<unsigned long,bool>::iterator iter = m_mPool.end();
	iter = m_mPool.find(nId);
	if(iter != m_mPool.end())
	{
		iter->second = false;
		m_nNewRelease = nId;
	}
}

void ACCELCTL::CUniqueIdPool::SetBase(unsigned long nBase)
{
	if(nBase < 1) nBase = 1;
	m_nBase = nBase;
}


////////////////////////////////////////////////////////////////////////////////////
//RegisterHotKey()
//idHotKey   =   (int)   wParam;   //该参数在设置系统级的热键有用,一般不予使用
//fuModifiers   =   (UINT)   LOWORD(lParam);     //热键的辅助按键
//uVirtKey   =   (UINT)   HIWORD(lParam);   //热键的键值   

CHotKeyManager::CHotKeyManager( )
{
	m_bCheckHWND = true;
	m_bEnable = true;
	do {
	ATOM cc = GlobalFindAtom(_T("HotKeyManager"));
	if (cc == 0)
		break;
	::GlobalDeleteAtom(cc);
	} while (1);
	m_nHotKeyId = GlobalAddAtom(_T("HotKeyManager")) - 0xC000;
}

CHotKeyManager::~CHotKeyManager(void)
{
	if(GlobalFindAtom(_T("HotKeyManager")))
		::GlobalDeleteAtom((ATOM)(m_nHotKeyId+ 0xC000));
	if (IsRegisted())
		ReleaseHotKey();
}

void CHotKeyManager::AttachWnd(HWND hAttachWnd)
{
	assert(::IsWindow(hAttachWnd));
	m_hWnd = hAttachWnd;
	if(m_vHotKeyProc.size() > 0 || m_vKeyCodes.size() > 0)
	{
		ReleaseHotKey();
	}
}

void CHotKeyManager::AddHotKey(DWORD keyCode, bool(*pFn)(void*))
{
	if (keyCode == 0)
		return;
	ParserCode(keyCode);
	std::vector<DWORD>::iterator iter;
	iter = find(m_vKeyCodes.begin(), m_vKeyCodes.end(), keyCode);
	if (iter != m_vKeyCodes.end())
		return;
	COMDELEGATE::CEvent* pESrc = new COMDELEGATE::CEvent();
	*pESrc += COMDELEGATE::CreateDelegate<COMDELEGATE::CObjNull,COMDELEGATE::CObjNull>(pFn);
	for (int i = 0; i < m_vKeyCodes.size();i++)
	{
		if (0 == m_vKeyCodes[i])
		{
			m_vKeyCodes[i] = keyCode;
			m_vHotKeyProc[i] = pESrc;
			RegisterHotKey(m_hWnd, m_nHotKeyId + i, HIWORD(keyCode), LOWORD(keyCode));
			return;
		}
	}
	m_vKeyCodes.push_back(keyCode);
	m_vHotKeyProc.push_back(pESrc);
	RegisterHotKey(m_hWnd, m_nHotKeyId + (int)m_vKeyCodes.size() - 1, HIWORD(keyCode), LOWORD(keyCode));
}

void CHotKeyManager::AddHotKey(DWORD key, DWORD ctl, bool (*pFn)(void*))
{
	AddHotKey( MAKELONG(key,ctl), pFn );
}

 LRESULT CHotKeyManager::OnHotKey(UINT uMsg, WPARAM wParam, LPARAM lParam)
 {
	 if(!m_bEnable) return S_FALSE;
	 if(m_bCheckHWND && ::GetActiveWindow() != m_hWnd) return S_FALSE;
	
	 if(uMsg == WM_HOTKEY)
	 {
		 // LOWORD(lParam); //modifier (alt,ctrl,shift,win)
		 // HIWORD(lParam); //key
#if(WINVER >= 0x0601)
		 DWORD keyCode = MAKELONG(HIWORD(lParam) ,LOWORD(lParam)| MOD_NOREPEAT);
#else
		 DWORD keyCode = MAKELONG(HIWORD(lParam) ,LOWORD(lParam));
#endif /* WINVER >= 0x0601 */
		 for(int i = 0; i< m_vKeyCodes.size();i++)
		 {
			 if (m_vKeyCodes[i] == 0) continue;
			 if(keyCode == m_vKeyCodes[i])
			 {
				 //dothings
				 if(m_vHotKeyProc.size() <= i)
					 break;
				 if(m_vHotKeyProc[i])
					(*m_vHotKeyProc[i])((void*)0);
				 break;
			 }
		 }
	 }
	 return S_OK;
 }

void CHotKeyManager::ReleaseHotKey()
{
	for(int i=0;i<m_vHotKeyProc.size();i++)
	{
		if (m_vHotKeyProc[i])
		{
			delete m_vHotKeyProc[i];
			m_vHotKeyProc[i] = NULL;
			UnregisterHotKey(m_hWnd, m_nHotKeyId + i);
		}
	}
	m_vHotKeyProc.clear();
	m_vKeyCodes.clear();
}

void CHotKeyManager::ParserCode(DWORD& dwKey)
{
	//MOD_ALT/MOD_CONTROL/MOD_NOREPEAT/MOD_SHIFT/MOD_WIN
	WORD wLow=LOWORD(dwKey);
	WORD wHigh=HIWORD(dwKey);
	WORD newHigh = 0;
#if(WINVER >= 0x0601)
	newHigh = MOD_NOREPEAT;
#endif /* WINVER >= 0x0601 */
	CUIString strText;
	if (wHigh&KEY_SHIFT) {
		newHigh |= MOD_SHIFT;
	}
	if (wHigh&KEY_CTRL) {
		newHigh |= MOD_CONTROL;
	}
	if (wHigh&KEY_ALT) {
		newHigh |= MOD_ALT;
	}
	if (wHigh&MOD_WIN) {
		newHigh |= MOD_WIN;
	}
	dwKey = MAKELONG(wLow,newHigh);
}

bool CHotKeyManager::IsRegisted()
{
	return !m_vKeyCodes.empty();
}

bool CHotKeyManager::ModifyHotKey(DWORD keyOld, DWORD keyNew)
{
	if (keyOld == keyNew)
		return false;
	if (keyOld == 0 || keyNew == 0)
		return false;
	std::vector<DWORD>::iterator iter;
	ParserCode(keyNew);
	iter = find(m_vKeyCodes.begin(), m_vKeyCodes.end(), keyNew);
	if (iter != m_vKeyCodes.end())
		return false;
	ParserCode(keyOld);
	for (int i = 0; i< m_vKeyCodes.size();i++)
	{
		if (keyOld == m_vKeyCodes[i])
		{
			 m_vKeyCodes[i] = keyNew;
			UnregisterHotKey(m_hWnd, m_nHotKeyId + i);
			RegisterHotKey(m_hWnd, m_nHotKeyId + i, HIWORD(keyNew), LOWORD(keyNew));
			return true;
		}
	}
	return false;
}

bool CHotKeyManager::ModifyHotKey(DWORD keyCode, bool(*pFn)(void *))
{
	if (keyCode == 0)
		return false;
	ParserCode(keyCode);
	COMDELEGATE::CEvent* pESrc = new COMDELEGATE::CEvent();
	*pESrc += COMDELEGATE::CreateDelegate<COMDELEGATE::CObjNull,COMDELEGATE::CObjNull>(pFn);
	for (int i = 0; i < m_vKeyCodes.size();i++)
	{
		if (keyCode == m_vKeyCodes[i])
		{
			delete m_vHotKeyProc[i];
			m_vHotKeyProc[i] = pESrc;
			return true;
		}
	}
	return false;
}

bool CHotKeyManager::DeleteHotKey(DWORD keyCode)
{
	ParserCode(keyCode);
	for (int i = 0; i< m_vKeyCodes.size();i++)
	{
		if (keyCode == m_vKeyCodes[i])
		{
			m_vKeyCodes[i] = 0;
			delete m_vHotKeyProc[i];
			m_vHotKeyProc[i] = NULL;
			UnregisterHotKey(m_hWnd, m_nHotKeyId + i);
			break;
		}
	}
	return false;
}

////////////////////////////////////////////////////////////////////////////////////

LRESULT CAccelManager::TranslateAccelerator(MSG *pMsg)
{
	if (m_hAccel != NULL)
	{
		if (::TranslateAccelerator(pMsg->hwnd, m_hAccel, pMsg))
			return S_OK;
	}
	return S_FALSE;
}

ACCEL CAccelManager::ParserAccel(DWORD & dwKey,int nIdOffs)
{
	WORD wLow = LOWORD(dwKey);
	WORD wHigh = HIWORD(dwKey);
	WORD newHigh = FVIRTKEY;
	CUIString strText;
	if (wHigh&KEY_SHIFT) {
		newHigh |= FSHIFT;
	}
	if (wHigh&KEY_CTRL) {
		newHigh |= FCONTROL;
	}
	if (wHigh&KEY_ALT) {
		newHigh |= FALT;
	}
#ifndef _MAC
	ACCEL accel = { (BYTE)newHigh, wLow, (WORD)(m_nAccelId + nIdOffs) };
#else
	ACCEL accel = { newHigh, wLow, (DWORD)(m_nAccelId + nIdOffs) };
#endif
	return accel; 
}

void CAccelManager::ParserAccel(DWORD & dwKey)
{
	WORD wLow = LOWORD(dwKey);
	WORD wHigh = HIWORD(dwKey);
	WORD newHigh = FVIRTKEY;
	CUIString strText;
	if (wHigh&KEY_SHIFT) {
		newHigh |= FSHIFT;
	}
	if (wHigh&KEY_CTRL) {
		newHigh |= FCONTROL;
	}
	if (wHigh&KEY_ALT) {
		newHigh |= FALT;
	}
	dwKey = MAKELONG(wLow,newHigh);
}

void CAccelManager::AddAccelerKey(DWORD keyCode, bool(*pFn)(void *))
{
	if(keyCode==0) return;
	std::map<DWORD,ACCELCTL::CEPair>::iterator iter;
	iter = m_vKeyCodes.find(keyCode);
	if(iter != m_vKeyCodes.end())
		return;
	if(m_hAccel)
	{
		DestroyAcceleratorTable(m_hAccel);
		m_hAccel = NULL;
	}
	ACCEL accel = ParserAccel(keyCode,(int)m_IdPool.GetId());
	COMDELEGATE::CEvent* pESrc = new COMDELEGATE::CEvent();
	*pESrc += COMDELEGATE::CreateDelegate<COMDELEGATE::CObjNull,COMDELEGATE::CObjNull>(pFn);
	ACCELCTL::CEPair cepair = {accel.cmd,pESrc};
	m_vKeyCodes.insert(std::map<DWORD,ACCELCTL::CEPair>::value_type(keyCode,cepair));
	m_vAccelTable.push_back(accel);
	m_hAccel = CreateAcceleratorTable(&m_vAccelTable[0], (int)m_vAccelTable.size());
}

void CAccelManager::AddAccelerKey(DWORD key, DWORD ctl, bool(*pFn)(void *))
{
	AddAccelerKey(MAKELONG(key, ctl), pFn);
}

CAccelManager::CAccelManager(HWND hWnd)
	:m_hWnd(hWnd)
{
	m_nAccelId = 0x9C40;
	m_hAccel = NULL;
}

CAccelManager::~CAccelManager(void)
{
}

void CAccelManager::ClearAccelertors()
{
	DestroyAcceleratorTable(m_hAccel);
	m_hAccel = NULL;
	std::map<DWORD,ACCELCTL::CEPair>::iterator iter = m_vKeyCodes.begin();
	while(iter != m_vKeyCodes.end())
	{
		if(iter->second.event_)
		{
			delete iter->second.event_;
			iter->second.event_ = NULL;
		}
		iter++;
	}
	m_IdPool.Clear();
	m_vKeyCodes.clear();
	m_vAccelTable.clear();
}

LRESULT CAccelManager::OnCommandKey(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch(uMsg)
	{
	case WM_COMMAND:
		{
			WORD hCode = HIWORD(wParam);//通知码
			WORD lCode = LOWORD(wParam);//控件ID
			//if(hCode == 0) return;
			std::map<DWORD,ACCELCTL::CEPair>::iterator iter = m_vKeyCodes.end();
			ACCELCTL::CEPair cepair = {lCode,NULL};
			iter = std::find_if(m_vKeyCodes.begin(),m_vKeyCodes.end(),ACCELCTL::CAccelMapValueFinder(cepair,true));
			if(iter != m_vKeyCodes.end() && iter->second.event_)
			{
				(*(iter->second.event_))((void*)0);
			}
		}
		break;
	case WM_SYSCOMMAND:
		break;
default:
	break;
	}
	return 0;
}

bool CAccelManager::ModifyAccelerKey(DWORD keyCode, bool(*pFn)(void *))
{
	std::map<DWORD,ACCELCTL::CEPair>::iterator iter;
	iter = m_vKeyCodes.find(keyCode);
	if(iter != m_vKeyCodes.end())
		return false;
	delete iter->second.event_ ;
	COMDELEGATE::CEvent* pESrc = new COMDELEGATE::CEvent();
	*pESrc += COMDELEGATE::CreateDelegate<COMDELEGATE::CObjNull,COMDELEGATE::CObjNull>(pFn);
	iter->second.event_ = pESrc;
	return true;
}

bool CAccelManager::ModifyAccelerKey(DWORD keyOld, DWORD keyNew)
{
	if(keyNew == keyOld || keyOld == 0 || keyNew == 0) return false;
	ParserAccel(keyNew);
	WORD wLow = LOWORD(keyNew);
	WORD wHigh = HIWORD(keyNew);
	std::vector<ACCEL>::iterator iter =  m_vAccelTable.begin();
	while(iter != m_vAccelTable.end())
	{
		if(iter->fVirt == wHigh && iter->key == wLow)
		{
			return false;
		}
		iter++;
	}
	ParserAccel(keyOld);
	wLow = LOWORD(keyOld);
	wHigh = HIWORD(keyOld);
	iter =  m_vAccelTable.begin();
	while(iter != m_vAccelTable.end())
	{
		if(iter->fVirt == wHigh && iter->key == wLow)
		{
#ifndef _MAC
			iter->fVirt = (BYTE)HIWORD(keyNew);
#else
			iter->fVirt = HIWORD(keyNew);
#endif
			iter->key = LOWORD(keyNew);
			DestroyAcceleratorTable(m_hAccel);
			m_hAccel = CreateAcceleratorTable(&m_vAccelTable[0], (int)m_vAccelTable.size());
			return true;
		}
		iter++;
	}
	return false;
}

bool CAccelManager::DeleteAccelerKey(DWORD keyCode)
{
	if(keyCode == 0) return false;
	ParserAccel(keyCode);
	WORD wLow = LOWORD(keyCode);
	WORD wHigh = HIWORD(keyCode);
	std::vector<ACCEL>::iterator iter =  m_vAccelTable.begin();
	while(iter != m_vAccelTable.end())
	{
		if(iter->fVirt == wHigh && iter->key == wLow)
		{
			m_IdPool.ReleaseId(iter->cmd - m_nAccelId);
			m_vAccelTable.erase(iter);
			DestroyAcceleratorTable(m_hAccel);
			m_hAccel = CreateAcceleratorTable(&m_vAccelTable[0], (int)m_vAccelTable.size());
			return true;
		}
		iter++;
	}
	return false;
}