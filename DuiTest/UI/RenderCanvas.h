#ifndef __RENDERCANVAS_H__
#define __RENDERCANVAS_H__

class CRenderCanvas : public CWindowWnd
{
public:
	CRenderCanvas(void);
	virtual ~CRenderCanvas(void);

	virtual LPCTSTR GetWindowClassName() const;
	virtual UINT GetClassStyle() const;

	virtual void OnFinalMessage(HWND hWnd);

	//设置窗口在释放时应该置空的指针, 必须为非局部变量
	void SetFianlEmptyPtr(LPVOID* ppPtr);

	//childstyle: CHILD or POPUP style
	BOOL CreateCanvas(HWND hParent, BOOL childstyle, BOOL visible, int cx = -1, int cy = -1);

	//再当前区域中裁剪传入的区域
	BOOL ClipCanvasArea(const CRect rect);

	//传入有效的区域集合
	BOOL MakeCanvasArea(const std::vector<CRect> &vecRect);

	BOOL AddCanvasArea(const CRect rect);

	//清空区域, 区域为RECT(0,0,0,0)
	void EmptyCanvasArea();

	BOOL IsPopupWindows();

	//获取窗口客户区
	CRect GetClientRect();

	virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);


	template <typename O, typename FnClassType>
	void ChainToMessageHandle(O* pthis, FnClassType msghandle)
	{
		if(pthis)
		{
			m_PreHandleMessage.push_back(std::bind(msghandle, pthis, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
		}
	}

	/*窗口区域用于移动裁剪, 该矩形left/top为相对于窗口左顶点的坐标*/
	void SetIntersectionRect(CRect rc);

	//首次移动时会记录当前的位置信息移动裁剪区域, 值为 +向下/右 -向左上
	void AreaScrollWithIntersection(int x, int y);

	//重设窗口移动,
	void AreaScrollReset();

protected:
	void ResetCurrentRgn();

private:
	HWND m_hParent;
	LPVOID* m_ppPtr; //存储非局部变量指针
	BOOL m_bPopup;
	HRGN m_hRgnCurrent;
	CRect m_rcIntersection; //窗口交集裁剪
	std::vector<std::function<LRESULT (UINT, WPARAM, LPARAM, BOOL&)> > m_PreHandleMessage;
};


#endif //__RENDERCANVAS_H__
