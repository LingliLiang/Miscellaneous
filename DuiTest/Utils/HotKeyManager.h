#ifndef __HOTKEYMANAGER_H__
#define __HOTKEYMANAGER_H__

#pragma once
#include <list>
#include <map>
/**
Provide simple delegate.
Author : liang
Date: 2016.6.25
**/
namespace COMDELEGATE
{
	class CObjNull {};

	class  CCommonDelegate
	{
	public:
		CCommonDelegate(void* pO, void* pF);
		CCommonDelegate(const CCommonDelegate& cdg);
		virtual ~CCommonDelegate();
		bool operator() (void* param);
		bool operator== (const CCommonDelegate& cdg) const;
		virtual CCommonDelegate* newCopy() const = 0;
	protected:

		void* GetO();
		void* GetF();
		virtual bool Invoke(void* param) = 0;
	private:
		void* m_pO; //object
		void* m_pF; //function
	};

	template <class O = CObjNull, class T = CObjNull>
	class CDelegate : public CCommonDelegate
	{
		typedef bool(*Fn)(void*);
		typedef bool(T::*FnO)(void*);
	public:
		CDelegate(O* pObj,FnO pFno) : CCommonDelegate(pObj, NULL), m_pFo(pFno){ }
		CDelegate(Fn pFn) : CCommonDelegate(NULL, pFn), m_pFo(NULL) { }
		CDelegate(const CDelegate& cdg) : CCommonDelegate(cdg) { this->m_pFo = cdg.m_pFo;}
		virtual CDelegate* newCopy() const { return new CDelegate(*this); };
	protected:
		virtual bool Invoke(void* param)
		{
			O* pObject = (O*)GetO();
			if (pObject == NULL)
			{
				Fn pFn = (Fn)GetF();
				if (pFn)
					return (*pFn)(param);
				else
					return false;
			}
			else
			{
				if(m_pFo)
					return (pObject->*m_pFo)(param);
				else
					return false;
			}
			return false;
		}
	private:
		FnO m_pFo;
	};

	template <class O , class T >
	CDelegate<O, T> CreateDelegate(O* pObject, bool (T::*pFn)(void*))
	{
		return CDelegate<O, T>(pObject, pFn);
	}

	template <class O  , class T >
	CDelegate<O, T> CreateDelegate(bool (*pFn)(void*))
	{
		return CDelegate<O, T>(pFn);
	}

	class CEvent
	{
		typedef bool(*FnType)(void*);
		typedef std::list<CCommonDelegate*>::iterator ITER_DG;
	public:
		~CEvent();
		operator bool();
		void operator = (FnType pFn);
		void operator+= (const CCommonDelegate& cgd); 
		void operator+= (CCommonDelegate* cgd); 
		void operator+= (FnType pFn);
		void operator-= (const CCommonDelegate& cgd);
		void operator-= (CCommonDelegate* cgd);
		void operator-= (FnType pFn);
		bool operator() (void* param);

	protected:
		std::list<CCommonDelegate*> m_lDelegates;
	};
}

/**
Provide hotkey control,for A window class.
Author : liang 
Date: 2016.4.15
modify Date: 2016.6.26
context: add modify key control
**/
class  CHotKeyManager
{
public:
	CHotKeyManager();
	~CHotKeyManager(void);

	/**
	Attach window, set hotkey control to it. The handle of window must vailed.
	**/
	void AttachWnd(HWND hAttachWnd);
	/**
	HotKey detects function,must call this on window's message handle function.
	**/
	LRESULT OnHotKey(UINT uMsg, WPARAM wParam, LPARAM lParam);
	/**
	Add hotkey to detect. it's for class member function. 
	keyCode: hotkey , Hight bits for Modifiers like CTRL... ; Low bits for Key, if Key is letter ,it must in ranger [A,Z].
	pThis: The class point;
	pFn: The class member funtion to execution when keys press down.
	**/
	template <class O, class T> 
	void AddHotKey(DWORD keyCode,O* pThis, bool (T::*pFn)(void*));
	/**
	Add hotkey to detect. it's for class member function. 
	key: if Key is letter ,it must in ranger [A,Z].
	ctl: Modifiers like CTRL...
	pThis: The class point;
	pFn: The class member funtion to execution when keys press down.
	**/
	template <class O, class T> 
	void AddHotKey(DWORD key, DWORD ctl,O* pThis, bool (T::*pFn)(void*));
	/**
	Add hotkey to detect. it's for normal function. 
	pFn: The normal funtion to execution when keys press down.
	**/
	void AddHotKey(DWORD keyCode, bool (*pFn)(void*));
	/**
	Add hotkey to detect. it's for normal function. 
	pFn: The normal funtion to execution when keys press down.
	**/
	void AddHotKey(DWORD key, DWORD ctl, bool (*pFn)(void*));
	/**
	ReleaseHotKey and clear up things. Wranging : Must call this when program exitting.
	and don't call this on (added Event)  function, 
	AttachWnd is call this by default.
	**/
	void ReleaseHotKey();
	/**
	have key Registed
	**/
	bool IsRegisted();
	/**
	Modify key code
	**/
	bool ModifyHotKey(DWORD keyOld, DWORD keyNew);
	/**
	Modify key function
	**/
	bool ModifyHotKey(DWORD keyCode, bool(*pFn)(void*));
	/**
	Modify key function
	**/
	template <class O, class T>
	bool ModifyHotKey(DWORD keyCode, O* pThis, bool (T::*pFn)(void*));
	/**
	Delete key control
	**/
	bool DeleteHotKey(DWORD keyCode);
	/**
	Check is control's win
	**/
	void SetCheckHWND(bool bCheck = true) { m_bCheckHWND = bCheck; }
	bool GetCheckHWND() { return m_bCheckHWND; }
	/**
	enable/disable control
	**/
	void SetEnable(bool bEnable = true) { m_bEnable = bEnable; }
	bool GetEnable() { return m_bEnable; }

protected:
	/**
	Parser code to adapt system's hotkey control.
	**/
	void ParserCode(DWORD &dwKey);	


private:
	HWND m_hWnd; // window handle to detect.
	unsigned int m_nHotKeyId; //unique id for  register hotkey.
	std::vector<DWORD> m_vKeyCodes; // hotkey array.
	std::vector<COMDELEGATE::CEvent*>  m_vHotKeyProc; // evnet funtion array for hotkey.
	bool m_bEnable;
	bool m_bCheckHWND;
};

namespace ACCELCTL{
	typedef struct Cmd_Event
	{
		UINT cmd_;
		COMDELEGATE::CEvent* event_;
	}CEPair;

	class ITranslateAccelerator
	{
	public:
		virtual LRESULT TranslateAccelerator(MSG *pMsg) = 0;
	};
	/**
	Unique Id manager pool
	**/
	class CUniqueIdPool
	{
	public:
		CUniqueIdPool():m_nBase(1),m_nNewRelease(0){}
		~CUniqueIdPool(){}
		unsigned long GetId();
		void ReleaseId(unsigned long nId);
		void SetBase(unsigned long nBase);
		void Clear(){m_mPool.clear();m_nBase=1;m_nNewRelease=0;}
	private:
		unsigned long m_nBase;
		unsigned long m_nNewRelease;
		std::map<unsigned long,bool> m_mPool;
	};

	/**
	template map value finder use for find_if()
	**/
	template <typename OKey, typename TValue>
	class CMapValueFinder{
	public:
		CMapValueFinder(TValue& value):m_value_(value){};

		bool operator ()(typename const std::map<OKey,TValue>::value_type &pair)
       {
			   return pair.second == m_value_;
       }

		private:
			TValue& m_value_;
	};

	/**
	accel map value finder use for find_if()
	**/
	class CAccelMapValueFinder
	{
	public:
		CAccelMapValueFinder(CEPair & cepair,bool cmd=true):m_cepair(cepair),m_cmd(cmd){};

		bool operator ()(const std::map<DWORD,CEPair>::value_type &pair)
       {
		   if(m_cmd)
			   return pair.second.cmd_ == m_cepair.cmd_;
		   else
			   return pair.second.event_ == m_cepair.event_;
       }
	private:
		bool m_cmd;
		CEPair& m_cepair;
	};
}

/**
Provide Accelerators control, for A window class.
Author : liang
date : 2016.6.25
**/
class  CAccelManager : public ACCELCTL::ITranslateAccelerator
{
public:
	CAccelManager(HWND hWnd);
	~CAccelManager(void);

	/**
	Accel detects function,must call this on window's message handle function.
	**/
	LRESULT OnCommandKey(UINT uMsg, WPARAM wParam, LPARAM lParam);

	template <class O, class T>
	void AddAccelerKey(DWORD keyCode, O* pThis, bool (T::*pFn)(void*));

	template <class O, class T>
	void AddAccelerKey(DWORD key, DWORD ctl, O* pThis, bool (T::*pFn)(void*));

	void AddAccelerKey(DWORD keyCode, bool(*pFn)(void*));

	void AddAccelerKey(DWORD key, DWORD ctl, bool(*pFn)(void*));

	/**
	ClearAccelertors and clear up things. Wranging : Must call this when program exitting.
	**/
	void ClearAccelertors();
	/**
	have key Registed
	**/
	bool IsRegisted() {return !m_vKeyCodes.empty();}

	//////////////////////////////////
	/**
	use dui need call:
	m_PaintManager.AddTranslateAccelerator(reinterpret_cast<DirectUI::ITranslateAccelerator*>(this)); //on init
	m_PaintManager.RemoveTranslateAccelerator(reinterpret_cast<DirectUI::ITranslateAccelerator*>(this)); //on exit
	use normal app:
	call this on messageloop, before TranslateMessage
	**/
	virtual LRESULT TranslateAccelerator(MSG *pMsg);

	template <class O, class T>
	bool ModifyAccelerKey(DWORD keyCode, O * pThis, bool(T::* pFn)(void *));

	bool ModifyAccelerKey(DWORD keyCode, bool(*pFn)(void *));

	bool ModifyAccelerKey(DWORD keyOld, DWORD keyNew);

	bool DeleteAccelerKey(DWORD keyCode);

protected:
	/**
	Parser code to create system's ACCEL control.
	**/
	ACCEL ParserAccel(DWORD &dwKey,int nIdOffs);
	void ParserAccel(DWORD &dwKey);
private:
	HWND m_hWnd; // window handle to detect.
	unsigned int m_nAccelId; //unique id for Acceler.
	std::map<DWORD,ACCELCTL::CEPair> m_vKeyCodes; //keycode & cmd ID & CEvent
	std::vector<ACCEL> m_vAccelTable; // Acceler array.
	HACCEL m_hAccel; //handle of Accel
	ACCELCTL::CUniqueIdPool m_IdPool; //id pool manager ids
};

template <class O, class T>
void CHotKeyManager::AddHotKey(DWORD keyCode,O* pThis, bool (T::*pFn)(void*))
{
	if (keyCode == 0)
		return;
	ParserCode(keyCode);
	std::vector<DWORD>::iterator iter;
	iter = find(m_vKeyCodes.begin(), m_vKeyCodes.end(), keyCode);
	if (iter != m_vKeyCodes.end())
		return;
	COMDELEGATE::CEvent* pESrc = new COMDELEGATE::CEvent;
	*pESrc += COMDELEGATE::CreateDelegate<O,T>(pThis,pFn);
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
	RegisterHotKey( m_hWnd, m_nHotKeyId+ (int)m_vKeyCodes.size() - 1, HIWORD(keyCode), LOWORD(keyCode) );
}

template <class O, class T>
void CHotKeyManager::AddHotKey(DWORD key, DWORD ctl,O* pThis, bool (T::*pFn)(void*))
{
	AddHotKey( MAKELONG(key,ctl), pThis, pFn );
}

template <class O, class T>
bool CHotKeyManager::ModifyHotKey(DWORD keyCode, O * pThis, bool(T::* pFn)(void *))
{
	if (keyCode == 0)
		return false;
	ParserCode(keyCode);
	COMDELEGATE::CEvent* pESrc = new COMDELEGATE::CEvent;
	*pESrc += COMDELEGATE::CreateDelegate<O,T>(pThis, pFn);
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

template <class O, class T>
void CAccelManager::AddAccelerKey(DWORD keyCode, O* pThis, bool (T::*pFn)(void*))
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
	*pESrc += COMDELEGATE::CreateDelegate<O,T>(pThis,pFn);
	ACCELCTL::CEPair cepair = {accel.cmd,pESrc};
	m_vKeyCodes.insert(std::map<DWORD,ACCELCTL::CEPair>::value_type(keyCode,cepair));
	m_vAccelTable.push_back(accel);
	m_hAccel = CreateAcceleratorTable(&m_vAccelTable[0], (int)m_vAccelTable.size());
}

template <class O, class T>
void CAccelManager::AddAccelerKey(DWORD key, DWORD ctl, O* pThis, bool (T::*pFn)(void*))
{
	AddAccelerKey(MAKELONG(key,ctl), pThis, pFn );
}

template <class O, class T>
bool CAccelManager::ModifyAccelerKey(DWORD keyCode, O * pThis, bool(T::* pFn)(void *))
{
	std::map<DWORD,ACCELCTL::CEPair>::iterator iter;
	iter = m_vKeyCodes.find(keyCode);
	if(iter != m_vKeyCodes.end())
		return false;
	delete iter->second.event_ ;
	COMDELEGATE::CEvent* pESrc = new COMDELEGATE::CEvent();
	*pESrc += COMDELEGATE::CreateDelegate<O,T>(pThis,pFn);
	iter->second.event_ = pESrc;
	return true;
}

#endif //__HOTKEYMANAGER_H__
