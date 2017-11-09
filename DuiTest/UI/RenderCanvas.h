#ifndef __VCASTER_RENDERCANVAS_H__
#define __VCASTER_RENDERCANVAS_H__

namespace VCasterUI
{

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

		/**/
		void SetIntersectionRect();
	private:
		LPVOID* m_ppPtr; //存储非局部变量指针
		BOOL m_bPopup;
		CRect m_rcIntersection; //窗口交集裁剪
		std::vector<std::function<LRESULT (UINT, WPARAM, LPARAM, BOOL&)> > m_PreHandleMessage;
	};

} //namespace __VCASTER_RENDERCANVAS_H__


#endif //__VCASTER_PVWUI_H__
