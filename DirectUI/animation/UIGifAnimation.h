#ifndef __UIGIFANIMATION_H__
#define __UIGIFANIMATION_H__

namespace DirectUI 
{
	class DirectUI_API IGifAnimationCallback
	{
	public:
		virtual void OnFrame() = 0;
	};

	class DirectUI_API CGifAnimation
	{
	public:
		#pragma pack(1)

		enum GIFBlockTypes
		{
			BLOCK_UNKNOWN,
			BLOCK_APPEXT,
			BLOCK_COMMEXT,
			BLOCK_CONTROLEXT,
			BLOCK_PLAINTEXT,
			BLOCK_IMAGE,
			BLOCK_TRAILER
		};

		enum ControlExtValues
		{
			GCX_PACKED_DISPOSAL, 
			GCX_PACKED_USERINPUT,
			GCX_PACKED_TRANSPCOLOR
		};

		enum LSDPackedValues 
		{
			LSD_PACKED_GLOBALCT,
			LSD_PACKED_CRESOLUTION,
			LSD_PACKED_SORT,
			LSD_PACKED_GLOBALCTSIZE
		};

		enum IDPackedValues 
		{
			ID_PACKED_LOCALCT,
			ID_PACKED_INTERLACE,
			ID_PACKED_SORT,
			ID_PACKED_LOCALCTSIZE
		};

		struct TGIFHeader
		{
			char   m_cSignature[3]; //"GIF����ͷ"
			char   m_cVersion[3];   //�汾 "87a" or "89a"
		};

		struct TGIFLSDescriptor
		{
			WORD          m_wWidth;         
			WORD          m_wHeight;      
			unsigned char m_cPacked;       
			unsigned char m_cBkIndex;       
			unsigned char m_cPixelAspect;  
			inline int    GetPackedValue(enum LSDPackedValues Value);
		};

	    struct TGIFAppExtension 
		{
			unsigned char m_cExtIntroducer;     
			unsigned char m_cExtLabel;         
			unsigned char m_cBlockSize;         
			char          m_cAppIdentifier[8];  
			char          m_cAppAuth[3];  
		};

		struct TGIFControlExt 
		{
			unsigned char m_cExtIntroducer; // (0x21)
			unsigned char m_cControlLabel;  // (0xF9)
			unsigned char m_cBlockSize;     // fixed value of 4
			unsigned char m_cPacked;        // packed field
			WORD          m_wDelayTime;     // delay time
			unsigned char m_cTColorIndex;   // transparent color index
			unsigned char m_cBlockTerm;     // block terminator (0x00)
			inline int    GetPackedValue(enum ControlExtValues Value);
		};

		struct TGIFCommentExt 
		{
			unsigned char m_cExtIntroducer; // extension introducer (0x21)
			unsigned char m_cCommentLabel;  // comment extension label (0xFE)
		};

		struct TGIFPlainTextExt
		{
			unsigned char m_cExtIntroducer;     // extension introducer (0x21)
			unsigned char m_cPlainTextLabel;    // text extension label (0x01)
			unsigned char m_cBlockSize;         // fixed value of 12
			WORD          m_wLeftPos;           // text grid left position
			WORD          m_wTopPos;            // text grid top position
			WORD          m_wGridWidth;         // text grid width
			WORD          m_wGridHeight;        // text grid height
			unsigned char m_cCellWidth;         // character cell width
			unsigned char m_cCellHeight;        // character cell height
			unsigned char m_cFgColor;           // text foreground color index
			unsigned char m_cBkColor;           // text background color index
		};

		struct TGIFImageDescriptor // image descriptor block
		{
			unsigned char m_cImageSeparator;    // image separator byte (0x2C)
			WORD          m_wLeftPos;           // image left position
			WORD          m_wTopPos;            // image top position
			WORD          m_wWidth;             // image width
			WORD          m_wHeight;            // image height
			unsigned char m_cPacked;            // packed field
			inline int    GetPackedValue(enum IDPackedValues Value);
		};
		#pragma pack()
		struct TFrame 
		{
			TImageInfo* m_pImage;       // pointer to one frame image
			SIZE        m_frameSize;
			SIZE        m_frameOffset;
			UINT        m_nDelay;       // delay (in 1/100s of a second)
			UINT        m_nDisposal;    // disposal method
		};
		typedef std::vector<TFrame>     VTFRAME;

	public:
		CGifAnimation(IGifAnimationCallback *pCallback);
		~CGifAnimation();

		const TImageInfo* GetCurImage();
		const TImageInfo* LoadGIF(LPCTSTR bitmap, LPCTSTR type = NULL, DWORD mask = 0);
		const TImageInfo* LoadGIF(const LPBYTE pData, DWORD dwSize, DWORD mask = 0);
		const TImageInfo* LoadGIFFirstFrame(const LPBYTE pData, DWORD dwSize, DWORD mask = 0);
		SIZE GetSize() const;
		size_t GetFrameCount() const;
		COLORREF GetBkColor() const;
		void SetCurrentFrame(long nFrame);

		bool Play();   
		void Stop(); 
		void UnLoad();
		bool IsPlaying() const;
    
	protected:
		int GetNextBlockLen() const;
		BOOL SkipNextBlock();
		BOOL SkipNextGraphicBlock();
		void ResetDataPointer();
		enum GIFBlockTypes GetNextBlock() const;
		UINT GetSubBlocksLen(UINT nStartingOffset) const;
		LPBYTE GetNextGraphicBlock(UINT* pBlockLen, UINT* pDelay, SIZE* pBlockSize, SIZE* pBlockOffset, UINT* pDisposal);

	#ifdef GIF_TRACING
		void EnumGIFBlocks();
		void WriteDataOnDisk(CString szFileName, HGLOBAL hData, DWORD dwSize);
	#endif // GIF_TRACING

		void ClearGifData();

	private:
		unsigned char*			m_pRawData;           // ����ʱ�����ݴ涯���ļ����� ����������������Ч
		UINT					m_nDataSize;          // GIF�ļ���С
		TGIFHeader*				m_pGIFHeader;         // GIF�ļ�ͷ
		TGIFLSDescriptor*		m_pGIFLSDescriptor;   // �߼���Ļ��ʶ��
		UINT					m_nGlobalCTSize;      // ȫ����ɫ�б��С
		SIZE					m_PictureSize;        // ͼ��ߴ�
		COLORREF				m_clrBackground;      // ����ɫ
		volatile long			m_nCurrFrame;         // ��ǰ֡����ֵ
		UINT					m_nCurrOffset;        // ���ȡƫ����
		VTFRAME*				m_pvFrames;           // ֡����

		HANDLE					m_hThread;
		HANDLE					m_hDrawEvent;         // ��ͼ�¼� ����ǰ֡�Ѿ�����ȡ���ڻ��� ����ֹ������Ⱦ�ٶ�����������֡��
		volatile bool			m_bExitThread;
		IGifAnimationCallback*	m_pCallback;

		DWORD ThreadAnimation();
		static DWORD __stdcall _ThreadAnimation(LPVOID pParam);

		unsigned char*			m_pGifData;           // ΪLoadGIFFirstFrameʹ�ã�����gif����
		UINT					m_nGifSize;          // ΪLoadGIFFirstFrameʹ�ã���¼GIF�ļ���С
		BOOL					m_bOnlyFirstFrame;	//ΪLoadGIFFirstFrameʹ�ã���ǰֻload��1֡��Ҫ���ŵ�ʱ���load���е�
		UINT					m_nGifCount;//ΪLoadGIFFirstFrameʹ��,��¼֡��
		UINT					m_nGifMask;//ΪLoadGIFFirstFrameʹ��,��¼mask
	};

	class DirectUI_API CGifAnimationUI : public IGifAnimationCallback
	{
	public:
		virtual ~CGifAnimationUI();

		bool Play(LPCTSTR bitmap);
		void Stop(LPCTSTR bitmap);

		/**
		 * ��ͼ��������ǿ�� ���ԭ��CControlUI::DrawImage �� CRenderEngine::DrawImageString����
		 * Note: ��PaintXXImage����
		 */
		bool DrawImageEx(HDC hDC, CPaintManagerUI* pManager, const RECT& rcPaint, LPCTSTR pStrImage);
		bool DrawImageEx(HDC hDC, CPaintManagerUI* pManager, const RECT& rc, const RECT& rcPaint, LPCTSTR pStrImage, 
			LPCTSTR pStrModify = NULL);
		/**
		 * ��ͼ��������ǿ�� ���ԭ��DuiLib::DrawImage����
		 * Note: �����ж��ļ����͡���ʼ����ȡGIF���ݵ�
		 */
		bool CGifAnimationUI::DrawAniImage(HDC hDC, CPaintManagerUI* pManager, const RECT& rc, const RECT& rcPaint, const CUIString& strImageName,\
			const CUIString& strImageResType, DWORD dwMask);
		bool DrawAniImage(HDC hDC, CPaintManagerUI* pManager, const RECT& rc, const RECT& rcPaint, const CUIString& strImageName, \
			const CUIString& strImageResType, RECT rcItem, RECT rcBmpPart, RECT rcCorner, DWORD dwMask, BYTE bFade, \
			bool bHole, bool bTiledX, bool bTiledY);

 		const TImageInfo* AddAniImage(LPCTSTR bitmap, LPBYTE pData, DWORD dwSize, DWORD mask = 0);
  
	protected:
		const TImageInfo* GetAniImage(LPCTSTR bitmap);
		const TImageInfo* AddAniImage(LPCTSTR bitmap, LPCTSTR type = NULL, DWORD mask = 0);
   
	protected:
		CStdStringPtrMap    m_mAniHash;
	};

	class DirectUI_API CLabelEx : public CLabelUI, public CGifAnimationUI
	{
	public:
		CLabelEx();
		~CLabelEx(){};

		LPCTSTR GetClass() const;
		void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);
		void DoEvent(TEventUI& event);

		void PaintBkImage(HDC hDC);
		void OnFrame();
		void SetNotify(DWORD dwIndex, INotifyUI* pNotify);
		bool IsPlaying();
		bool IsMousePlayEnabled(){return m_bMousePlayEnabled;}
		void SetMousePlayEnabled(bool bEnable){m_bMousePlayEnabled=bEnable;}
	protected:
		bool		m_bMousePlayEnabled;//�������ȥʱ�Զ����ţ��Ƴ���ֹͣ����
		bool		m_bPlaying;
		DWORD		m_dwIndex;
		INotifyUI*	m_pNotify;
	};
}

#endif //__UIGIFANIMATION_H__