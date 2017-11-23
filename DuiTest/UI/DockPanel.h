#ifndef __DOCKPANEL_H__
#define __DOCKPANEL_H__

#pragma once
//#include "directui\core\uicontainer.h"
//#include "directui\layout\UIHorizontalLayout.h"
//#include "DirectUI\utils\UIBaseWnd.h"

#define  DUI_CTR_DOCKPANEL                       (_T("DockPanel"))
#define  DUI_CTR_DOCKER                       (_T("Docker"))
#define  DUI_CTR_DOCKTITLE                      (_T("DockTitle"))
#define  DUI_CTR_DOCKTITLEITEM                      (_T("DockTitleItem"))
#define  DUI_CTR_DOCKTITLETEXT                     (_T("DockTitleText"))

#define WM_INACCEPT WM_USER+0x0f0
#define WM_LEAVEACCEPT WM_USER+0x0f1
#define DUI_MSGTYPE_DOCK_WM  (_T("dock_windows_message"))

class CDockTitleTextUI : public CLabelUI
{
public:
	CDockTitleTextUI();

	LPCTSTR GetClass() const
	{
		return _T("DockTitleTextUI");
	}

	LPVOID GetInterface(LPCTSTR pstrName)
	{
		if (_tcscmp(pstrName, _T("CDockTitleText")) == 0) return static_cast<CDockTitleTextUI*>(this);
		return CLabelUI::GetInterface(pstrName);
	}

	UINT GetEstimaLength();

	SIZE EstimateSize(SIZE szAvailable);

	void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);

	/**
	** if it's Vertical Text ,then turn to west or east
	**/
	void SetVerticalWestText(bool west = true);


	void SetSelectedTextColor(DWORD dwTextColor);

	DWORD GetSelectedTextColor();

	void SetHotTextColor(DWORD dwColor);

	DWORD GetHotTextColor() const;

	void SetPushedTextColor(DWORD dwColor);

	DWORD GetPushedTextColor() const;

	void SetFocusedTextColor(DWORD dwColor);

	DWORD GetFocusedTextColor() const;

	void SetTextState(UINT state);

	void ParseTextState(DWORD &dwTextColor);

	void DoPaint(HDC hDC, const RECT& rcPaint);

	void PaintText(HDC hDC);

	void DrawItemText(HDC hDC, CPaintManagerUI* pManager, RECT& rc, LPCTSTR pstrText, DWORD dwTextColor, int iFont, UINT uStyle);
protected:

	DWORD m_dwSelectedTextColor;
	DWORD m_dwHotTextColor;
	DWORD m_dwPushedTextColor;
	DWORD m_dwFocusedTextColor;
private:
	bool m_bVerticalWestText;
	unsigned int m_nEstimaLength;
	UINT m_nTextState;
};

class CDockTitleUI;
class CDockTitleItemUI : public CContainerUI
{
	enum BTN_IMGS { BTN_IMG_NORMAL = 0, BTN_IMG_HOT, BTN_IMG_PUSHED, BTN_IMG_DISABLE, BTN_IMG_FOCUS, BTN_IMG_FORCE, BTN_IMG_HOTFORCE, };
public:

	CDockTitleItemUI();
	~CDockTitleItemUI();

	virtual LPCTSTR GetClass() const
	{
		return _T("DockTitleItemUI");
	}

	virtual LPVOID GetInterface(LPCTSTR pstrName)
	{
		if (_tcscmp(pstrName, _T("DockTitleItem")) == 0) return static_cast<CDockTitleItemUI*>(this);
		return CContainerUI::GetInterface(pstrName);
	}

	virtual UINT GetControlFlags() const
	{
		return UIFLAG_SETCURSOR;
	}

	virtual void SetManager(CPaintManagerUI* pManager, CControlUI* pParent, bool bInit);

	bool Activate();

	void SetEnabled(bool bEnable);

	void ChangeNormalImageToOther(BTN_IMGS nStatus);

	void SetPos(RECT rc);

	void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);

	void SetOwer(CDockTitleUI* pOwer);

	void SetText(LPCTSTR pstrText);

	void CalculateSize();

	/*******************Attributes********************/
	LPCTSTR GetGroup() const;
	void SetGroup(LPCTSTR pStrGroupName);
	bool IsSelected() const;
	void Selected(bool bSelected);
	void SetSelectedBkColor(DWORD dwBkColor);
	DWORD GetSelectBkColor();
	void SetHotBkColor(DWORD dwColor);
	DWORD GetHotBkColor() const;
	//images 
	LPCTSTR GetSelectedImage();
	void SetSelectedImage(LPCTSTR pStrImage);
	LPCTSTR GetSelectedHotImage();
	void SetSelectedHotImage(LPCTSTR pStrImage);
	LPCTSTR GetSelectedPushedImage();
	void SetSelectedPushedImage(LPCTSTR pStrImage);
	LPCTSTR GetForeImage();
	void SetForeImage(LPCTSTR pStrImage);
	LPCTSTR GetHotForeImage();
	void SetHotForeImage(LPCTSTR pStrImage);
	void SetForePushedImage(LPCTSTR pStrImage);
	LPCTSTR GetForePushedImage();
	LPCTSTR GetNormalImage();
	void SetNormalImage(LPCTSTR pStrImage);
	LPCTSTR GetHotImage();
	void SetHotImage(LPCTSTR pStrImage);
	LPCTSTR GetPushedImage();
	void SetPushedImage(LPCTSTR pStrImage);
	LPCTSTR GetFocusedImage();
	void SetFocusedImage(LPCTSTR pStrImage);
	LPCTSTR GetDisabledImage();
	void SetDisabledImage(LPCTSTR pStrImage);

	CUIString GetListItemAttr(){return m_strListItemAttr;}

	bool GetMoveable();
	void SetMoveable(bool moveable);
	bool GetFloatItem();
	void SetFloatItem(bool floatItem);

	//event
	void DoEvent(TEventUI& event);

	//paints
	void PaintStatusImage(HDC hDC);

	bool DrawImage(HDC hDC, LPCTSTR pStrImage, LPCTSTR pStrModify = NULL);
protected:
	bool m_bSelected;
	CUIString m_sGroupName;
	UINT m_uButtonState;

	//color
	DWORD m_dwSelectedBkColor;
	DWORD m_dwHotBkColor;
	//images
	CUIString m_sSelectedImage;
	CUIString m_sSelectedHotImage;
	CUIString m_sSelectedPushedImage;

	CUIString m_sForeImage;
	CUIString m_sHotForeImage;
	CUIString m_sPushedForeImage;

	CUIString m_sNormalImage;
	CUIString m_sHotImage;
	CUIString m_sPushedImage;

	CUIString m_sFocusedImage;
	CUIString m_sDisabledImage;

	CUIString m_sFirstNormalImage;

	int m_nNowNormalID;

	bool m_bMoveable;
	bool m_bFloatItem;
private:
	CDockTitleTextUI* m_pText;
	CLabelUI* m_pIcon; //icon label
	CButtonUI* m_pClose; //close button
	CDockTitleUI* m_pDockItemOwer;
	CUIString m_strListItemAttr; // arrt in TitleListWnd
};


class CTitleListWnd : public CWindowWnd
{
public:
    void Init(CDockTitleUI* pOwner);
    LPCTSTR GetWindowClassName() const;
    void OnFinalMessage(HWND hWnd);

    LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);

    void Scroll(int dx, int dy);

#if(_WIN32_WINNT >= 0x0501)
	virtual UINT GetClassStyle() const;
#endif

public:
    CPaintManagerUI m_pm;
    CDockTitleUI* m_pOwner;
    CVerticalLayoutUI* m_pLayout;
	bool m_bScrollBar;
};
class CDockPanelUI;
class CDockTitleUI : public CContainerUI
{
	friend class CTitleListWnd;
public:
	enum LayoutMethod { Vertical_Method, Horizontal_Method };
	enum FloatMethod { DBLCLICK_FLOAT, DRAG_FLOAT };

	CDockTitleUI();

	LPCTSTR GetClass() const
	{
		return _T("DockTitleUI");
	}

	LPVOID GetInterface(LPCTSTR pstrName)
	{
		if (_tcscmp(pstrName, _T("DockTitle")) == 0) return static_cast<CDockTitleUI*>(this);
		return CContainerUI::GetInterface(pstrName);
	}

	bool Add(CControlUI* pControl);

	bool AddAt(CControlUI* pControl, int nIndex);

	void SetLayoutMethod(LayoutMethod method = Horizontal_Method);

	void SetTitleLength(int length);

	void SetOwer(CDockPanelUI* pPanel);

	void SelectTitle(CDockTitleItemUI* pSelect);

	void FloatWindow(FloatMethod method = DBLCLICK_FLOAT);

	void MoveTitleItem(bool left);

	int GetTitleItemIndex();

	int GetTitleItemIndex(CControlUI* pControl);

	void SetPos(RECT rc);
	void DoEvent(TEventUI& event_);
	bool DoItemEvent(void* pevent_);

	void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);
	void SetItemsAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);


	bool GetMove()
	{
		return m_bMoveItem;
	}
	void SetMove(bool bMoveItem)
	{
		m_bMoveItem = bMoveItem;
	}
	bool GetCalculateSize()
	{
		return m_bCalcsize;
	}
	void SetCalculateSize(bool bCalcsize);
	bool IsHorizontal() {
		CUIString strClass;
		if(m_pItemLayout)
			strClass = m_pItemLayout->GetClass();
		return strClass == _T("HorizontalLayoutUI") ? true : false;
	}
	CContainerUI* GetItemsLayout();
	CDockTitleItemUI* GetTitleItemAt(int index);
	int GetTitleItemCount();
	void FloatTitleItemOut(int index);
	void FloatTitleItemOut(CDockTitleItemUI* pItem);
	void RemoveItemFormGroup(CDockTitleItemUI* pItem);

	void SetItemView(bool bView);
	bool GetItemView();
	void SetListViewSize(SIZE size){m_szView = size;}
	SIZE GetListViewSize(){return m_szView;}
private:
	UINT m_nTitle; // define title's width/height
	bool m_bMoveItem;
	bool m_bCalcsize;
	CContainerUI* m_pItemLayout;
	CDockTitleItemUI* m_pSelected;
	CUIString m_sUniGroupName;
	CButtonUI* m_pMiniBtn;
	CUIString m_strMiniAttr;
	bool m_incView; // using list view title when m_incView is false, else using click inc button
	COptionUI* m_pListItemOpt;
	CTitleListWnd* m_pListWindow;
	SIZE m_szView;
	CUIString m_strListOptAttr;

	CDockPanelUI* m_pTitleOwer;
};

class CDockPanelUI : public CContainerUI
{
	friend class CDockerUI;
public:
	typedef enum Panel_State {HLOD=0x0,HIDE=0x2,MINI=0x4,MINIPOP=0x8} PANELSTATE;//the state of panel 
	// titlesize  arr minbtn
	void SetMini(bool mini)
	{
		m_bMini = mini;
		if(this->GetTitlePositionHor())
		{
			if(!m_bMini)
				this->SetFixedHeight(m_nOldFixed);
			else{
				m_nOldFixed = this->GetFixedHeight();
				this->SetFixedHeight(m_titleFixedHeight);
			}
		}
		else
		{
			if(!m_bMini)
				this->SetFixedWidth(m_nOldFixed);
			else{
				m_nOldFixed = this->GetFixedWidth();
				this->SetFixedWidth(m_titleFixedHeight);
			}
		}

	}
	bool GetMini(){return m_bMini;}
	//visible
	void SetHide(bool hide)
	{
		this->SetVisible(hide);
	}
	//min floatdock pos arr -minbtn
	void SetPanelState(UINT state)
	{m_nPanelState = state;}

	CDockPanelUI(void);
	virtual ~CDockPanelUI(void);

	LPCTSTR GetClass() const;
	LPVOID GetInterface(LPCTSTR pstrName);
	static CControlUI* CreateControl(LPCTSTR pstrClass);
	void DoEvent(TEventUI& event_);

	int GetDockerCount() const;
	bool Add(CControlUI* pControl);

	CDockerUI* GetDockerAt(int nIndex) const;
	int GetDockerIndex(CControlUI* pControl) const;
	bool SetDockerIndex(CControlUI* pControl, int nIndex);

	bool AddDocker(CControlUI* pControl);
	bool AddDockerAt(CControlUI* pControl, int nIndex, bool reIn = false);
	bool RemoveDocker(CControlUI* pControl);
	bool RemoveDockerAt(int nIndex);
	bool FloatDockerOut(CControlUI* pControl);
	bool FloatDockerOut(int nIndex);
	void RemoveAllDocker();

	void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);
	void SetDockersAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);
	void SetTitlePosition(TCHAR position);
	TCHAR GetTitlePosition();
	bool GetTitlePositionHor();
	void SetTitleFixed(int  titlefixed);
	int GetTitleFixed();
	void SetRestore(bool restore);
	bool GetRestore();
	std::vector<UINT> GetWinLayoutIndex();

	void SetParentWindow(CWindowUI* pWin);
	void SetParentWindow(INotifyUI* pWin);
	INotifyUI* GetParentNotify();

	CUIString GetUniGroup() const;
	void SetUniGroup(LPCTSTR pStrGroupName);

	CContainerUI* GetDockerLayout(){return m_pDockerLayout;}
	CDockTitleUI* GetTitleLayout(){return m_pTitleLayout;}

	void MoveDocker(int src, int to);

	void FloatDocker(bool bMousePt);

	CRect GetDockingArea(); //screen area

	void AcceptDocking(bool bAccept); //not user call function
	bool GetAcceptDocking(){return m_bAcceptDocking;}
	void DoPaint(HDC hDC, const RECT& rcPaint);
	void PaintDockingState(HDC hDC);

	void SelectDocker(int nIndexOld,int nIndexNew);

	CDockerUI* GetCurrentDocker();

	void SetFloatColor(DWORD dwColor){m_dwFloatColor = dwColor;}
	DWORD GetFloatBkColor(){return m_dwFloatColor;}
	void SetWinStyle(DWORD dwStyle, DWORD dwExStyle = 0);
	std::pair<DWORD,DWORD> GetWinStyle();
private:

	void ReArrangeItems();

private:
	CStdPtrArray m_arrDockPtr; //dockerui array
	TCHAR m_titlePosition; // EWSN

	bool m_restore; //return in panel or clear close
	bool m_bAcceptDocking; //是否接受停靠
	bool m_bDockable; //是否允许停靠和弹出

	union  IParent{
		IDialogBuilderCallback* m_pParentWin;
		INotifyUI* m_pParentNotify;
		CWindowWnd* m_pParentWnd;
	} m_pParent;

	//subtitle arrtu时
	int m_titleFixedHeight; //the height of docker's title

	//this is not CContainerUI ,but it's child class.
	CDockTitleUI* m_pTitleLayout;
	CContainerUI* m_pDockerLayout;

	//custom windows style 
	CUIString m_winXml;
	std::vector<UINT> m_nLayoutIndex; // the insert index of docker in window
	DWORD m_dwWinStyle;
	DWORD m_dwWinExStyle;
	DWORD m_dwFloatColor;

	CUIString m_sUniGroup;

	UINT m_nPanelState;
	UINT m_nOldFixed; //old width/height
	bool m_bMini;

};

class CDockerUI : public CContainerUI
{
	template<class TLayout , class OSrcThis > friend  class CDockerWindow;
public:

	CDockerUI();
	virtual ~CDockerUI();

	LPCTSTR GetClass() const;
	LPVOID GetInterface(LPCTSTR pstrName);

	void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);

	void InitXml(LPCTSTR pstrXml); // custom layout by xml file, option;

	CUIString GetXml();

	bool ReInPanel(bool bClose = false);

	void SetIndexOfDocker(int index)
	{
		if(index < 0)
			index = 0;
		m_index = index;
	}
	int GetPostionOfDocker(){return m_index;}

	void SetOwner(CDockPanelUI* pDock)
	{
		m_pParentDock = pDock;
	}

	CDockPanelUI* GetOwner() { return m_pParentDock;}

	HWND GetOwnerWnd() { return m_pParentWnd;}

	void SetBuilderCallback(IDialogBuilderCallback* pIDBcallback);

	void SetTitleItem(CDockTitleItemUI* pTitleItem);
	CDockTitleItemUI* GetTitleItem( );

	LPCTSTR GetDockGroup() const;
	void SetDockGroup(LPCTSTR pStrGroupName);
	void SetClearClose(bool bClear);
	bool GetClearClose();

	void CloseClearUpDocker();

	void DoEvent(TEventUI& event);

	void SetDisableMaxMin(BOOL bdis){m_bDisableMaxMin = bdis;}
	BOOL GetDisableMaxMin(){return m_bDisableMaxMin;}
	DWORD GetSubStyle(){return m_dwStyleSub;}
	DWORD GetSubExStyle(){return m_dwExStyleSub;}
	DWORD GetAddStyle(){return m_dwStylePlus;}
	DWORD GetAddExStyle(){return m_dwExStylePlus;}

private:
	CDockTitleItemUI* m_pTitleItem;
	bool m_clearClose; //hit close button action close or 

	/**
	** xml file format:
	**<?xml version="1.0" encoding="utf-8"?><Window ><Something/></Window>
	**/
	CUIString m_strXml;  //custom CDockerUI use xml
	bool m_useXml; // switch  use xml file
	CUIString m_tilteimgXml; //backimage of float windows's title bar; DockPanel must set xmlfile and it has control name 'docker_title'


	UINT m_index; //the index in the Panel

	IDialogBuilderCallback* m_pIDBcallback;

	CDockPanelUI* m_pParentDock; //this was Docked when not null 

	HWND m_pParentWnd; // current dock windows, can change when float, docked would be null


	CUIString m_sDockGroup;

	UINT m_bDisableMaxMin;

	DWORD m_dwStylePlus;
	DWORD m_dwExStylePlus;
	DWORD m_dwStyleSub;
	DWORD m_dwExStyleSub;
};


/**
** CDockManager 定位为静态，控制所有Float窗口的恢复到原来的DockPanel
**/
class CDockManager
{
public:
	CDockManager();
	~CDockManager();

	static void AddDockPanelToWatch(CDockPanelUI* panel);
	static void RemoveDockPanel(CDockPanelUI* panel);
	static void Docking(HWND hWin,CDockerUI* pDocker);
	static void StopDocking();
	static CDockPanelUI* GetDockingPanel();
	static bool IsDocking();
	static void ClearDockingPanel();
	static void SendInMessage();
	static void SendLeaveMessage();
	static POINT GetClientCursorPos(HWND hClient)
	{
		POINT pt = {0};
		::GetCursorPos(&pt);
		::ScreenToClient(hClient,&pt);
		return pt;
	}
protected:
	static DWORD WINAPI DockingThread(LPVOID lpbDocking);
private:
	static bool m_bDocking;
	static std::vector<CDockPanelUI*> m_vPanels;
	static HWND m_hDockWin;
	static CDockPanelUI* m_pDockingPanel;
	static CDockerUI* m_pDocker;
};


class CDockBuilderCallback: public IDialogBuilderCallback
{
public:
	CControlUI* CreateControl(LPCTSTR pstrClass)
	{
		if(_tcsicmp(pstrClass, DUI_CTR_DOCKPANEL) == 0)
		{
			return new CDockPanelUI;
		}
		else if(_tcsicmp(pstrClass, DUI_CTR_DOCKER) == 0)
		{
			return new CDockerUI;
		}
		else if(_tcsicmp(pstrClass, DUI_CTR_DOCKTITLE) == 0)
		{
			return new CDockTitleUI;
		}
		else if(_tcsicmp(pstrClass, DUI_CTR_DOCKTITLEITEM) == 0)
		{
			return new CDockTitleItemUI;
		}
		else if(_tcsicmp(pstrClass, DUI_CTR_DOCKTITLETEXT) == 0)
		{
			return new CDockTitleTextUI;
		}
		return NULL;
	}
};

template<class TLayout , class OSrcThis >
class CDockerWindow :public CWindowUI 
{
	friend class CDockPanelUI;
	/**
	**  向下支持FloatWindow:
	** 为了解决资源,自定义图片资源不显示在新窗口而设计
	** 需要用户自己编写复制资源函数传人调用
	** 现在的DockerWindow:
	** 使用UseParentResource获取父类窗口的自定义资源(主要是image),故可以不用设置init proc
	**修改GetImageEx添加父PaintManager支持
	**对于关闭窗口后资源反馈会原来的添加CPaintManagerUI函数
	**ReMoveAllImagesToAother设置图片资源,ResourceFeedBack中实现,故可以不用设置clear up proc
	**/
	typedef DWORD (OSrcThis::*iProc)(_In_ CPaintManagerUI*); //init proc
	typedef DWORD (OSrcThis::*cProc)(_In_ CPaintManagerUI*); //last clear up proc
public:
	CDockerWindow(void);
	~CDockerWindow(void);
	HWND GetHWND() const{return __super::GetHWND();}
	void MoveWithMousePt();
protected:
	virtual CUIString GetSkinFolder(){return _T("skin\\default");}
	virtual CUIString GetSkinFile(){return _T("");}
	UINT GetClassStyle() const{return CS_DBLCLKS;}
	virtual LPCTSTR GetWindowClassName(void) const {return _T("DockerWindow") ;}
	virtual void InitWindow();
	virtual void Notify(TNotifyUI& msg);
	virtual void OnFinalMessage(HWND hWnd){
		delete this;
	}
	virtual LRESULT OnGetMinMaxInfo(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual LRESULT OnClose(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled);

	virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);

	void ResourceFeedBack();
	bool PtInTitle(WPARAM wParam, LPARAM lParam)
	{
		POINT pt; pt.x = GET_X_LPARAM(lParam); pt.y = GET_Y_LPARAM(lParam);
		::ScreenToClient(*this, &pt);
		CRect rcCaption;
		::GetClientRect(*this,&rcCaption);
		rcCaption.bottom = m_PaintManager.GetCaptionRect().bottom;
		rcCaption.Deflate(&m_PaintManager.GetSizeBox());
		return ::PtInRect(&rcCaption,pt)?true:false;
	}
	inline void DockToPanel()
	{
		if(!m_pDockerLayout) return;
		if(!m_pDockerLayout->GetClearClose())
		{
			m_pDockerLayout->ReInPanel(true);
		}
		else
		{
			m_pDockerLayout->CloseClearUpDocker();
		}
		Close();
		return;
	}
	
	void InitProc()
	{	
		//call first
		if(m_pSrcThis != NULL && m_iProc != NULL)
		{
			(m_pSrcThis->*m_iProc)(&m_PaintManager);
		}
	}
	void CallBackProc()
	{
		//call back
		if(m_pSrcThis != NULL && m_cProc != NULL)
		{
			(m_pSrcThis->*m_cProc)(&m_PaintManager);
		}
	}
public:
	/**
	** common layout support, pop UI in common way.
	** pLayout : add to current window
	** pSrcPm: the old layout window's PaintManager
	** winXml : the custom windows ui using xml file, NULL is use defualt window ui.
	** pSrcThis : the old layout window's pointer
	** cproc : call back function incase have custom resources need set back.
	**iproc : init function to init custom resources for this window
	**/
	void Init(TLayout* pLayout, CPaintManagerUI* pSrcPm, DWORD dwStyle = WS_OVERLAPPEDWINDOW,
		DWORD dwExStyle = 0, LPCTSTR winXml = NULL,
		OSrcThis* pSrcThis = NULL , cProc cproc = NULL ,iProc iproc = NULL)
	{
		if(pLayout == NULL  || pSrcPm == NULL)
			return;
		m_orgLayout = pLayout;
		m_orgParentLayout = (CContainerUI*)(m_orgLayout->GetParent());
		if(m_orgParentLayout == NULL) return;
		m_orgLayoutIndex = m_orgParentLayout->GetItemIndex(m_orgLayout);
		m_pSrcPM = pSrcPm;
		m_pSrcThis = pSrcThis;
		m_iProc = iproc;
		m_cProc = cproc;
		m_winXml = winXml;
		m_bDocker = false;
		Create(pSrcPm->GetPaintWindow(), GetWindowClassName(), dwStyle , dwExStyle/*WS_EX_TOPMOST*/,0,0,0,0);
	}
	/**
	** DockerUI functions support, pop ui form CDockPanelUI
	**/
	void Init(CPaintManagerUI* pSrcPm, CDockerUI* pDocker, DWORD dwStyle = WS_OVERLAPPEDWINDOW,
		DWORD dwExStyle = 0, LPCTSTR winXml = NULL,
		OSrcThis* pSrcThis = NULL , cProc cproc = NULL ,iProc iproc = NULL)
	{	
		if(pDocker == NULL  || pSrcPm == NULL)
			return;
		m_pDockerLayout = pDocker;
		m_pSrcPM = pSrcPm;
		m_pSrcThis = pSrcThis;
		m_iProc = iproc;
		m_cProc = cproc;
		m_winXml = winXml;
		m_bDocker = true;
		Create(pSrcPm->GetPaintWindow(), pDocker->GetText().GetData(), dwStyle , dwExStyle/*WS_EX_TOPMOST*/,0, 0, 0, 0);
	}

	void SetInitProc(iProc proc){if(proc) m_iProc = proc;}
	void SetCallBackProc(cProc proc){if(proc) m_cProc = proc;}

	void SetCustomInsertIndexs(std::vector<int> layoutIndexs)
	{
		m_layoutIndexs = layoutIndexs;
	}

protected:
	CContainerUI* m_vLayout;
	CUIString m_winXml;

	bool m_bFullScreen;
	/**
	** common layout support
	**/
	TLayout* m_orgLayout;
	CContainerUI* m_orgParentLayout;
	UINT m_orgLayoutIndex;

	CDockerUI* m_pDockerLayout;
	bool m_bDocker;

	CPaintManagerUI* m_pSrcPM;
	OSrcThis* m_pSrcThis;
	INotifyUI* m_pNotifySrc; // from Docker
	iProc m_iProc;
	cProc m_cProc;
};

template<class TLayout , class OSrcThis>
CDockerWindow<TLayout , OSrcThis>::CDockerWindow(void)
	:m_bFullScreen(false),
	m_orgLayout(NULL),
	m_orgParentLayout(NULL),
	m_vLayout(NULL),
	m_pSrcPM(NULL),
	m_pSrcThis(NULL),
	m_pNotifySrc(NULL),
	m_pDockerLayout(NULL)
{
	m_iProc = NULL;
	m_cProc = NULL;
}

template<class TLayout , class OSrcThis>
CDockerWindow<TLayout , OSrcThis>::~CDockerWindow(void)
{
}

template<class TLayout , class OSrcThis>
void CDockerWindow<TLayout , OSrcThis>::InitWindow()
{
	InitProc();
	if(!m_pDockerLayout->m_tilteimgXml.IsEmpty())
	{
		CControlUI* pControl = m_PaintManager.FindControl(_T("docker_title"));
		if(pControl != NULL)
		{
			pControl->SetBkImage(m_pDockerLayout->m_tilteimgXml.GetData());
		}
	}
	if(!m_pDockerLayout->GetText().IsEmpty())
	{
		CControlUI* pControl = m_PaintManager.FindControl(_T("docker_title"));
		if(pControl != NULL)
		{
			pControl->SetText(m_pDockerLayout->GetText().GetData());
		}
	}
	if(m_pDockerLayout->GetDisableMaxMin())
	{
		m_PaintManager.SetSizeBox(CRect(0,0,0,0));
	}
}

template<class TLayout , class OSrcThis>
void CDockerWindow<TLayout , OSrcThis>::Notify(TNotifyUI& msg)
{
	if(_tcscmp(msg.sType ,DUI_MSGTYPE_CLICK) == 0)
	{
		if(_tcscmp(msg.pSender->GetName() ,_T("close")) == 0)
		{
			DockToPanel();
			return;
		}
	}
	if(_tcscmp(msg.sType ,DUI_MSGTYPE_SELECTCHANGED) == 0)
	{
		if(_tcscmp(msg.pSender->GetName() ,_T("pin")) == 0)
		{
			if(((COptionUI*)msg.pSender)->IsSelected())
			{
				::SetWindowPos(m_hWnd,HWND_TOPMOST, 0, 0, 0, 0,SWP_NOSIZE | SWP_NOMOVE);
			}
			else
			{
				::SetWindowPos(m_hWnd,HWND_NOTOPMOST,0, 0, 0, 0,SWP_NOSIZE | SWP_NOMOVE);
			}
		}
		else if(_tcscmp(msg.pSender->GetName() ,_T("fullscr")) == 0)
		{
			static CRect winRect;
			if(m_bFullScreen = !m_bFullScreen) //赋值再判段
			{
				int   nScreenWidth = GetSystemMetrics(SM_CXSCREEN);
				int   nScreenHeight = GetSystemMetrics(SM_CYSCREEN);
				m_PaintManager.SetCaptionRect(CRect(0,0,0,0));
				m_PaintManager.SetSizeBox(CRect(0,0,0,0));
				::GetWindowRect(m_hWnd,&winRect);
				::SetWindowPos(m_hWnd,HWND_TOPMOST, 0, 0, nScreenWidth, nScreenHeight,0);
			}
			else
			{
				m_PaintManager.SetCaptionRect(CRect(0,0,0,40));
				m_PaintManager.SetSizeBox(CRect(4,4,4,4));
				::SetWindowPos(m_hWnd,HWND_NOTOPMOST, winRect.left, winRect.top, winRect.GetWidth(), winRect.GetHeight(),0);
			}
		}
	}
	else if(_tcscmp(msg.sType , DUI_MSGTYPE_WINDOWINIT)==0)
	{
		return; //拦截初始化消息
	}

	if(m_pNotifySrc) // 发送Notify消息给源Manager
	{
		m_pNotifySrc->Notify(msg);
		//m_pSrcPM->SendNotify(msg); //not used
	}
}

template<class TLayout , class OSrcThis>
void CDockerWindow<TLayout , OSrcThis>::MoveWithMousePt()
{
	CPoint ptScreenMouse;
	::GetCursorPos(&ptScreenMouse);
	::SetWindowPos(m_hWnd, NULL, ptScreenMouse.x-120, ptScreenMouse.y-20, 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_SHOWWINDOW);
	::PostMessage(m_hWnd,WM_NCLBUTTONDOWN, HTCAPTION, MAKELPARAM(ptScreenMouse.x, ptScreenMouse.y));
}

template<class TLayout , class OSrcThis>
LRESULT CDockerWindow<TLayout , OSrcThis>::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	CControlUI* pRoot = NULL;
	LONG styleValue = ::GetWindowLong(*this, GWL_STYLE);
	styleValue &= ~WS_CAPTION;
	::SetWindowLong(*this, GWL_STYLE, styleValue | WS_CLIPSIBLINGS /*| WS_CLIPCHILDREN*/);

	RECT rcClient;
	::GetClientRect(*this, &rcClient);
	::SetWindowPos(*this, NULL, rcClient.left, rcClient.top, rcClient.right - rcClient.left,
		rcClient.bottom - rcClient.top, SWP_FRAMECHANGED);

	m_PaintManager.Init(m_hWnd);
	m_PaintManager.AddPreMessageFilter(this);
	m_PaintManager.UseParentResource(m_pSrcPM); //set parent resource, iProc could ignore
	SIZE minInfo = {160,100};
	SIZE initSize = {1280,720};
	//build root
	CDialogBuilder builder;
	CControlUI* pNode = builder.Create(m_winXml.GetData(), NULL, m_pSrcThis, &m_PaintManager);
	if(m_orgLayout) //old common layout pop
	{
		RECT rcItem = m_orgLayout->GetPos();
		initSize.cx = rcItem.right - rcItem.left;
		initSize.cy = rcItem.bottom - rcItem.top;
		m_pDockerLayout = new CDockerUI;
		m_pDockerLayout->SetOwner(NULL);
		m_orgParentLayout->RemoveNotDestroy(m_orgLayout);
		m_pDockerLayout->Add(m_orgLayout);
	}
	else //new DockUI layout pop
	{ 
		//override window style
		DWORD modifyStyle = 0;
		LONG styleValue = ::GetWindowLong(*this, GWL_STYLE);
		modifyStyle = m_pDockerLayout->GetAddStyle();
		if(modifyStyle) styleValue &= modifyStyle;
		modifyStyle = m_pDockerLayout->GetSubStyle();
		if(modifyStyle) styleValue &= ~modifyStyle;
		::SetWindowLong(*this, GWL_STYLE, styleValue);

		LONG exstyleValue = ::GetWindowLong(*this, GWL_EXSTYLE);
		modifyStyle = m_pDockerLayout->GetAddExStyle();
		if(modifyStyle) exstyleValue &= modifyStyle;
		modifyStyle = m_pDockerLayout->GetSubExStyle();
		if(modifyStyle) exstyleValue &= ~modifyStyle;
		::SetWindowLong(*this, GWL_EXSTYLE, exstyleValue);

		RECT rcItem = m_pDockerLayout->GetPos();
		initSize.cx = rcItem.right - rcItem.left;
		initSize.cy = rcItem.bottom - rcItem.top;
		m_pNotifySrc = m_pDockerLayout->GetOwner()->GetParentNotify();
		m_pDockerLayout->GetOwner()->FloatDockerOut(m_pDockerLayout);
		m_pDockerLayout->m_pParentWnd = this->m_hWnd;
	}

	if(pNode != NULL) //using xml window 
	{
		std::vector<UINT> layoutIndexs = m_pDockerLayout->GetOwner()->GetWinLayoutIndex();
		pRoot = pNode;
		CContainerUI* pContainer = (CContainerUI*)pRoot;
		m_PaintManager.AttachDialog(pRoot);
		int vsize = (int)layoutIndexs.size();
		int it = 0;
		if(vsize == 0)
		{
			pContainer->Add(m_pDockerLayout);
		}
		else
		{
			for (it = 0; it < vsize-1; it++)
			{
				pContainer = (CContainerUI*)pContainer->GetItemAt(layoutIndexs[it]);
			}
			pContainer->AddAt(m_pDockerLayout, layoutIndexs[it]);
		}
		m_pDockerLayout->SetVisible(true);
	}
	else
	{
		RECT rcSizeBox = { 2,2,2,2 };
		RECT rcCaption = { 0,0,0,30 };
		//defualt window init
		CVerticalLayoutUI* pVer = new CVerticalLayoutUI;
		pVer->SetBkColor(0xff282828);
		pVer->SetInset(CRect(2,2,2,2));
		pVer->SetBorderSize(2);
		pVer->SetBorderColor(0xff161616);
		CHorizontalLayoutUI* pHor = new CHorizontalLayoutUI;
		pHor->SetBkColor(0xff2c3d5a);
		pHor->SetFixedHeight(30);
		CLabelUI* pLabel = new CLabelUI;
		pLabel->SetAttribute(_T("endellipsis"),_T("true"));
		pLabel->SetText(m_pDockerLayout->GetText().GetData());
		pLabel->SetFixedWidth(120);
		pLabel->SetTextPadding(CRect(10,0,0,0));
		CButtonUI* pButton = new CButtonUI;
		pButton->SetFixedWidth(30);
		pButton->SetName(_T("close"));
		pButton->SetText(_T("×"));
		pButton->SetTextColor(0xAAAAAA);
		pButton->SetHotTextColor(0x808080);
		pButton->SetPushedTextColor(0x606060);
		pHor->Add(pLabel);
		pHor->Add(new CControlUI);
		pHor->Add(pButton);
		pVer->Add(pHor);
		pVer->Add(m_pDockerLayout);
		m_pDockerLayout->SetVisible(true);
		pRoot = pVer;
		
		//init PaintManager
		m_PaintManager.SetSizeBox(rcSizeBox);
		m_PaintManager.SetCaptionRect(rcCaption);
		m_PaintManager.SetRoundCorner(2,2);
		initSize.cy += 30; //title
		//m_pDockerLayout->SetBkColor(0xffeeeeee);
		m_PaintManager.SetInitSize(initSize.cx,initSize.cy);
		m_PaintManager.SetMinInfo(minInfo.cx, minInfo.cy);
		ASSERT(pRoot);
		m_PaintManager.AttachDialog(pRoot);
	}

	/*ASSERT(pRoot);
	m_PaintManager.AttachDialog(pRoot);*/
	m_PaintManager.AddNotifier(this);
	m_PaintManager.UsedVirtualWnd(true);
	m_PaintManager.SetBackgroundTransparent(TRUE);
	InitWindow();
	bHandled = FALSE;
	return 0;
}

template<class TLayout , class OSrcThis>
LRESULT CDockerWindow<TLayout , OSrcThis>::OnClose(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
	CallBackProc();
	ResourceFeedBack();
	if(m_orgParentLayout != NULL)
	{
		m_pDockerLayout->RemoveNotDestroy(m_orgLayout);
		m_orgParentLayout->AddAt(m_orgLayout,m_orgLayoutIndex);
	}
	m_pDockerLayout = NULL;
	bHandled = FALSE;
	return 0;
}

template<class TLayout , class OSrcThis>
LRESULT CDockerWindow<TLayout , OSrcThis>::OnGetMinMaxInfo(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	LPMINMAXINFO lpMMI = (LPMINMAXINFO) lParam;

	MONITORINFO oMonitor = {};
	oMonitor.cbSize = sizeof(oMonitor);
	::GetMonitorInfo(::MonitorFromWindow(*this, MONITOR_DEFAULTTONEAREST), &oMonitor);
	CRect rcWork = oMonitor.rcWork;
	CRect rcMonitor = oMonitor.rcMonitor;
	rcWork.Offset(-oMonitor.rcMonitor.left, -oMonitor.rcMonitor.top);

	// 计算最大化时，正确的原点坐标
	//Work screen
	lpMMI->ptMaxPosition.x	= rcWork.left;
	lpMMI->ptMaxPosition.y	= rcWork.top;

	if(m_bFullScreen)
	{
		//full screen
		lpMMI->ptMaxSize.x = rcMonitor.GetWidth();
		lpMMI->ptMaxSize.y = rcMonitor.GetHeight();
		lpMMI->ptMaxTrackSize.x = rcMonitor.GetWidth();
		lpMMI->ptMaxTrackSize.y = rcMonitor.GetHeight();
	}
	else
	{
		lpMMI->ptMaxSize.x = rcWork.GetWidth();
		lpMMI->ptMaxSize.y = rcWork.GetHeight();
		lpMMI->ptMaxTrackSize.x = rcWork.GetWidth();
		lpMMI->ptMaxTrackSize.y = rcWork.GetHeight();
	}

	lpMMI->ptMinTrackSize.x = m_PaintManager.GetMinInfo().cx;
	lpMMI->ptMinTrackSize.y = m_PaintManager.GetMinInfo().cy;
	bHandled = TRUE;
	return 0;
}

template<class TLayout , class OSrcThis>
LRESULT CDockerWindow<TLayout , OSrcThis>::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if(uMsg == WM_SYSKEYDOWN && wParam == VK_F4)
	{
		DockToPanel(); //主窗口关闭时自动恢复到界面
		return true;
	}
	if(m_pDockerLayout)
	{
		if(uMsg == WM_NCLBUTTONDOWN && PtInTitle(wParam,lParam))
		{
			CDockManager::Docking(m_hWnd,m_pDockerLayout);
		}
	}
	if(CDockManager::IsDocking() && uMsg == WM_NCMOUSEMOVE)
	{
		CDockManager::StopDocking();
		if(m_pDockerLayout->ReInPanel()){
			Close();
		}
		CDockManager::ClearDockingPanel();
	}
	if(uMsg == WM_INACCEPT )
	{
		m_PaintManager.SetTransparent(100);
	}
	else if(uMsg == WM_LEAVEACCEPT)
	{
		m_PaintManager.SetTransparent(255);
	}
	else if(uMsg ==WM_DESTROY)
	{
		DockToPanel(); //主窗口关闭时自动恢复到界面
	}
	if(m_pSrcPM && m_pNotifySrc)
	{
		if((WM_KEYFIRST <= uMsg && uMsg <=  WM_KEYLAST) ||
			(WM_MOUSEFIRST <= uMsg && uMsg <=  WM_MOUSELAST) ||
			(WM_NCMOUSEMOVE <= uMsg && uMsg <=  WM_NCMBUTTONDBLCLK) ||
			(WM_CTLCOLORMSGBOX <= uMsg && uMsg <=  WM_CTLCOLORSTATIC) ||
			WM_SYSCOMMAND || WM_COMMAND || WM_CREATE
			)
		{
			CPoint ptMousePos;
			::GetCursorPos(&ptMousePos);
			::ScreenToClient(m_hWnd,&ptMousePos);
			TNotifyUI msg;
			msg.sType = DUI_MSGTYPE_DOCK_WM;
			msg.pSender = m_pDockerLayout;
			msg.sVirtualWnd.Format(_T("%x"),uMsg);
			msg.wParam = wParam;
			msg.lParam = lParam;
			msg.ptMouse = ptMousePos;
			msg.dwTimestamp = ::GetTickCount();
			m_pNotifySrc->Notify(msg);
			if(msg.wParam != 0)
				return (LRESULT)msg.wParam;
			//::PostMessage(m_pSrcPM->GetPaintWindow(),uMsg,wParam,lParam);
		}
	}
	return __super::HandleMessage(uMsg,wParam,lParam);
}

template<class TLayout , class OSrcThis>
void CDockerWindow<TLayout , OSrcThis>::ResourceFeedBack()
{
	if(m_pSrcPM !=NULL)
	{
		m_PaintManager.ReMoveAllImagesToAother(m_pSrcPM);
	}
}

template <class T , class O>
HWND DockerWindow(T* pLayoutSrc, CPaintManagerUI* pSrcPm, O* pSrcThis =NULL , 
	DWORD (O::*CallFirstProc)(_In_ CPaintManagerUI*) = NULL ,
	DWORD (O::*CallBackProc)(_In_ CPaintManagerUI*) = NULL)
{
	CDockerWindow<T,O> *pFloatw = new CDockerWindow<T,O>;
	pFloatw->Init(pLayoutSrc,pSrcPm);
	pFloatw->CenterWindow();
	pFloatw->ShowWindow();
	return pFloatw->GetHWND();
}

#endif  //__DOCKPANEL_H__
