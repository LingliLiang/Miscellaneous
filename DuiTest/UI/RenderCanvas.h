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

	//���ô������ͷ�ʱӦ���ÿյ�ָ��, ����Ϊ�Ǿֲ�����
	void SetFianlEmptyPtr(LPVOID* ppPtr);

	//childstyle: CHILD or POPUP style
	BOOL CreateCanvas(HWND hParent, BOOL childstyle, BOOL visible, int cx = -1, int cy = -1);

	//�ٵ�ǰ�����вü����������
	BOOL ClipCanvasArea(const CRect rect);

	//������Ч�����򼯺�
	BOOL MakeCanvasArea(const std::vector<CRect> &vecRect);

	BOOL AddCanvasArea(const CRect rect);

	//�������, ����ΪRECT(0,0,0,0)
	void EmptyCanvasArea();

	BOOL IsPopupWindows();

	//��ȡ���ڿͻ���
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

	/*�������������ƶ��ü�, �þ���left/topΪ����ڴ����󶥵������*/
	void SetIntersectionRect(CRect rc);

	//�״��ƶ�ʱ���¼��ǰ��λ����Ϣ�ƶ��ü�����, ֵΪ +����/�� -������
	void AreaScrollWithIntersection(int x, int y);

	//���贰���ƶ�,
	void AreaScrollReset();

protected:
	void ResetCurrentRgn();

private:
	HWND m_hParent;
	LPVOID* m_ppPtr; //�洢�Ǿֲ�����ָ��
	BOOL m_bPopup;
	HRGN m_hRgnCurrent;
	CRect m_rcIntersection; //���ڽ����ü�
	std::vector<std::function<LRESULT (UINT, WPARAM, LPARAM, BOOL&)> > m_PreHandleMessage;
};


#endif //__RENDERCANVAS_H__
