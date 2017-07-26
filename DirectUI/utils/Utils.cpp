#include "stdafx.h"
#include "Utils.h"

namespace DirectUI
{

	/////////////////////////////////////////////////////////////////////////////////////
	//
	//

	CPoint::CPoint()
	{
		x = y = 0;
	}

	CPoint::CPoint(const POINT& src)
	{
		x = src.x;
		y = src.y;
	}

	CPoint::CPoint(int _x, int _y)
	{
		x = _x;
		y = _y;
	}

	CPoint::CPoint(LPARAM lParam)
	{
		x = GET_X_LPARAM(lParam);
		y = GET_Y_LPARAM(lParam);
	}


	/////////////////////////////////////////////////////////////////////////////////////
	//
	//

	CSize::CSize()
	{
		cx = cy = 0;
	}

	CSize::CSize(const SIZE& src)
	{
		cx = src.cx;
		cy = src.cy;
	}

	CSize::CSize(const RECT rc)
	{
		cx = rc.right - rc.left;
		cy = rc.bottom - rc.top;
	}

	CSize::CSize(int _cx, int _cy)
	{
		cx = _cx;
		cy = _cy;
	}


	/////////////////////////////////////////////////////////////////////////////////////
	//
	//

	CRect::CRect()
	{
		left = top = right = bottom = 0;
	}

	CRect::CRect(const RECT& srcRect)
	{
		::CopyRect(this, &srcRect);
	}

	CRect::CRect(LPCRECT lpSrcRect)
	{
		::CopyRect(this, lpSrcRect);
	}

	CRect::CRect(int nLeft, int nTop, int nRight, int nBottom)
	{
		left = nLeft;
		top = nTop;
		right = nRight;
		bottom = nBottom;
	}

	int CRect::GetWidth() const
	{
		return right - left;
	}

	int CRect::GetHeight() const
	{
		return bottom - top;
	}

	void CRect::Empty()
	{
		::SetRectEmpty(this);
	}

	bool CRect::IsNull() const
	{
		return (left == 0 && right == 0 && top == 0 && bottom == 0); 
	}

	void CRect::Join(const RECT& rc)
	{
		if (rc.left < left) left = rc.left;
		if (rc.top < top) top = rc.top;
		if (rc.right > right) right = rc.right;
		if (rc.bottom > bottom) bottom = rc.bottom;
	}

	void CRect::ResetOffset()
	{
		::OffsetRect(this, -left, -top);
	}

	void CRect::Normalize()
	{
		if (left > right) { int iTemp = left; left = right; right = iTemp; }
		if (top > bottom) { int iTemp = top; top = bottom; bottom = iTemp; }
	}

	void CRect::Offset(int cx, int cy)
	{
		::OffsetRect(this, cx, cy);
	}

	void CRect::Inflate(int cx, int cy)
	{
		::InflateRect(this, cx, cy);
	}

	void CRect::Inflate(LPCRECT lpRect)
	{
		left -= lpRect->left;
		top -= lpRect->top;
		right += lpRect->right;
		bottom += lpRect->bottom;
	}

	void CRect::Deflate(int cx, int cy)
	{
		::InflateRect(this, -cx, -cy);
	}

	void CRect::MoveToXY(int cx, int cy)
	{
		int nWidth = right - left;
		int nHeight = bottom - top;

		left = cx;
		top = cy;
		right = left + nWidth;
		bottom = top + nHeight;
	}

	void CRect::Deflate(LPCRECT lpRect)
	{
		left += lpRect->left;
		top += lpRect->top;
		right -= lpRect->right;
		bottom -= lpRect->bottom;
	}

	void CRect::Union(CRect& rc)
	{
		::UnionRect(this, this, &rc);
	}

	void CRect::SetRect(int x1, int y1, int x2, int y2)
	{
		::SetRect(this, x1, y1, x2, y2);
	}

	void CRect::SetRect(POINT topLeft, POINT bottomRight)
	{
		::SetRect(this, topLeft.x, topLeft.y, bottomRight.x, bottomRight.y);
	}

	void CRect::SetRectEmpty()
	{
		::SetRectEmpty(this);
	}

	void CRect::CopyRect(LPCRECT lpSrcRect)
	{
		::CopyRect(this, lpSrcRect);
	}

	BOOL CRect::EqualRect(LPCRECT lpRect) const
	{
		return ::EqualRect(this, lpRect);
	}

	CRect CRect::operator +(LPCRECT lpRect) const
	{
		CRect rect(this);
		rect.Inflate(lpRect);
		return rect;
	}

	CRect CRect::operator -(LPCRECT lpRect) const
	{
		CRect rect(this);
		rect.Deflate(lpRect);
		return rect;
	}

	void CRect::operator =(const RECT& srcRect)
	{
		::CopyRect(this, &srcRect);
	}

	BOOL CRect::operator ==(const RECT& rect) const
	{
		return ::EqualRect(this, &rect);
	}

	BOOL CRect::operator !=(const RECT& rect) const
	{
		return !::EqualRect(this, &rect);
	}

	/////////////////////////////////////////////////////////////////////////////////////
	//
	//
	class CArrayAutoLock {
	public:
		CArrayAutoLock(LPCRITICAL_SECTION pSec){m_pSec=pSec;if (m_pSec) ::EnterCriticalSection(m_pSec);}
		~CArrayAutoLock() {if (m_pSec) {::LeaveCriticalSection(m_pSec);}}
	protected:
		LPCRITICAL_SECTION m_pSec;
	};

	CStdPtrArray::CStdPtrArray(int iPreallocSize) : m_ppVoid(NULL), m_nCount(0), m_nAllocated(iPreallocSize),m_pCriticalSection(NULL)
	{
		m_pCriticalSection = new CRITICAL_SECTION;

		if (m_pCriticalSection)
			InitializeCriticalSection(m_pCriticalSection);

		ASSERT(iPreallocSize >= 0);
		if (iPreallocSize > 0)
		{
			m_ppVoid = static_cast<LPVOID*>(malloc(iPreallocSize * sizeof(LPVOID)));
		}
	}

	CStdPtrArray::CStdPtrArray(const CStdPtrArray& src) : m_ppVoid(NULL), m_nCount(0), m_nAllocated(0)
	{
		m_pCriticalSection = new CRITICAL_SECTION;

		if (m_pCriticalSection)
			InitializeCriticalSection(m_pCriticalSection);
		for (int i=0; i<src.GetSize(); i++)
		{
			Add(src.GetAt(i));
		}
		if (m_pCriticalSection)
		{
			DeleteCriticalSection(m_pCriticalSection);
			delete m_pCriticalSection;
			m_pCriticalSection=NULL;
		}
	}

	CStdPtrArray::~CStdPtrArray()
	{
		if (m_ppVoid)
		{
			free(m_ppVoid);
		}
		if (m_pCriticalSection)
		{
			DeleteCriticalSection(m_pCriticalSection);
			delete m_pCriticalSection;
			m_pCriticalSection=NULL;
		}
	}

	void CStdPtrArray::Empty()
	{
		CArrayAutoLock lk(m_pCriticalSection);
		if (m_ppVoid)
		{
			free(m_ppVoid);
		}

		m_ppVoid = NULL;
		m_nCount = m_nAllocated = 0;
	}

	void CStdPtrArray::Resize(int iSize)
	{
		CArrayAutoLock lk(m_pCriticalSection);
		Empty();
		m_ppVoid = static_cast<LPVOID*>(malloc(iSize * sizeof(LPVOID)));
		::ZeroMemory(m_ppVoid, iSize * sizeof(LPVOID));
		m_nAllocated = iSize;
		m_nCount = iSize;
	}

	bool CStdPtrArray::IsEmpty() const
	{
		return m_nCount == 0;
	}

	bool CStdPtrArray::Add(LPVOID pData)
	{
		CArrayAutoLock lk(m_pCriticalSection);
		if (++m_nCount >= m_nAllocated) 
		{
			int nAllocated = m_nAllocated * 2;
			if (nAllocated == 0) nAllocated = 11;
			LPVOID* ppVoid = static_cast<LPVOID*>(realloc(m_ppVoid, nAllocated * sizeof(LPVOID)));
			if (ppVoid) 
			{
				m_nAllocated = nAllocated;
				m_ppVoid = ppVoid;
			}
			else 
			{
				--m_nCount;
				return false;
			}
		}

		m_ppVoid[m_nCount - 1] = pData;
		return true;
	}

	bool CStdPtrArray::InsertAt(int nIndex, LPVOID pData)
	{
		if (nIndex == m_nCount) return Add(pData);
		CArrayAutoLock lk(m_pCriticalSection);
		if (nIndex < 0 || nIndex > m_nCount) return false;
		if (++m_nCount >= m_nAllocated) 
		{
			int nAllocated = m_nAllocated * 2;
			if (nAllocated == 0) nAllocated = 11;
			LPVOID* ppVoid = static_cast<LPVOID*>(realloc(m_ppVoid, nAllocated * sizeof(LPVOID)));
			if (ppVoid)
			{
				m_nAllocated = nAllocated;
				m_ppVoid = ppVoid;
			}
			else 
			{
				--m_nCount;
				return false;
			}
		}

		memmove(&m_ppVoid[nIndex + 1], &m_ppVoid[nIndex], (m_nCount - nIndex - 1) * sizeof(LPVOID));
		m_ppVoid[nIndex] = pData;
		return true;
	}

	bool CStdPtrArray::SetAt(int nIndex, LPVOID pData)
	{
		CArrayAutoLock lk(m_pCriticalSection);
		if (nIndex < 0 || nIndex >= m_nCount) return false;
		m_ppVoid[nIndex] = pData;
		return true;
	}

	bool CStdPtrArray::Remove(int nIndex)
	{
		CArrayAutoLock lk(m_pCriticalSection);
		if (nIndex < 0 || nIndex >= m_nCount) return false;
		if (nIndex < --m_nCount)
		{
			::CopyMemory(m_ppVoid + nIndex, m_ppVoid + nIndex + 1, (m_nCount - nIndex) * sizeof(LPVOID));
		}
		return true;
	}

	bool CStdPtrArray::Move(int nSrcIndex, int nDesIndex)
	{
		CArrayAutoLock lk(m_pCriticalSection);
		if (nSrcIndex<0 || nDesIndex<0 || nSrcIndex>=m_nCount || nDesIndex>=m_nCount || nSrcIndex==nDesIndex)
		{
			return false;
		}

		LPVOID pVoid = m_ppVoid[nSrcIndex];

		if (nSrcIndex>nDesIndex)
		{
			//Move Up
			memmove(&m_ppVoid[nDesIndex+1], &m_ppVoid[nDesIndex], (nSrcIndex-nDesIndex)*sizeof(LPVOID));
			m_ppVoid[nDesIndex] = pVoid;
		}
		else
		{
			//Move Down
			memmove(&m_ppVoid[nSrcIndex], &m_ppVoid[nSrcIndex+1], (nDesIndex-nSrcIndex)*sizeof(LPVOID));
			m_ppVoid[nDesIndex] = pVoid;
		}
		return true;
	}

	int CStdPtrArray::Find(LPVOID pData) const
	{
		CArrayAutoLock lk(m_pCriticalSection);
		for (int i = 0; i < m_nCount; i++)
		{
			if (m_ppVoid[i] == pData) 
				return i;
		}

		return -1;
	}

	int CStdPtrArray::GetSize() const
	{
		return m_nCount;
	}

	LPVOID* CStdPtrArray::GetData()
	{
		return m_ppVoid;
	}

	LPVOID CStdPtrArray::GetAt(int nIndex) const
	{
		CArrayAutoLock lk(m_pCriticalSection);
		if (nIndex < 0 || nIndex >= m_nCount) return NULL;
		return m_ppVoid[nIndex];
	}

	LPVOID CStdPtrArray::operator[] (int nIndex) const
	{
		CArrayAutoLock lk(m_pCriticalSection);
		ASSERT(nIndex >= 0 && nIndex < m_nCount);
		return m_ppVoid[nIndex];
	}


	/////////////////////////////////////////////////////////////////////////////////////
	//
	//

	CStdValArray::CStdValArray(int iElementSize, int iPreallocSize /*= 0*/) : 
	m_pVoid(NULL), 
		m_nCount(0), 
		m_iElementSize(iElementSize), 
		m_nAllocated(iPreallocSize)
	{
		ASSERT(iElementSize>0);
		ASSERT(iPreallocSize>=0);
		if( iPreallocSize > 0 ) m_pVoid = static_cast<LPBYTE>(malloc(iPreallocSize * m_iElementSize));
	}

	CStdValArray::~CStdValArray()
	{
		if( m_pVoid != NULL ) free(m_pVoid);
	}

	void CStdValArray::Empty()
	{   
		m_nCount = 0;  // NOTE: We keep the memory in place
	}

	bool CStdValArray::IsEmpty() const
	{
		return m_nCount == 0;
	}

	bool CStdValArray::Add(LPCVOID pData)
	{
		if( ++m_nCount >= m_nAllocated) {
			int nAllocated = m_nAllocated * 2;
			if( nAllocated == 0 ) nAllocated = 11;
			LPBYTE pVoid = static_cast<LPBYTE>(realloc(m_pVoid, nAllocated * m_iElementSize));
			if( pVoid != NULL ) {
				m_nAllocated = nAllocated;
				m_pVoid = pVoid;
			}
			else {
				--m_nCount;
				return false;
			}
		}
		::CopyMemory(m_pVoid + ((m_nCount - 1) * m_iElementSize), pData, m_iElementSize);
		return true;
	}

	bool CStdValArray::Remove(int nIndex)
	{
		if( nIndex < 0 || nIndex >= m_nCount ) return false;
		if( nIndex < --m_nCount ) ::CopyMemory(m_pVoid + (nIndex * m_iElementSize), m_pVoid + ((nIndex + 1) * m_iElementSize), (m_nCount - nIndex) * m_iElementSize);
		return true;
	}

	int CStdValArray::GetSize() const
	{
		return m_nCount;
	}

	LPVOID CStdValArray::GetData()
	{
		return static_cast<LPVOID>(m_pVoid);
	}

	LPVOID CStdValArray::GetAt(int nIndex) const
	{
		if( nIndex < 0 || nIndex >= m_nCount ) return NULL;
		return m_pVoid + (nIndex * m_iElementSize);
	}

	LPVOID CStdValArray::operator[] (int nIndex) const
	{
		ASSERT(nIndex>=0 && nIndex<m_nCount);
		return m_pVoid + (nIndex * m_iElementSize);
	}


	/////////////////////////////////////////////////////////////////////////////////////
	//
	//

	CUIString::CUIString() : m_pstr(m_szBuffer)
	{
		m_szBuffer[0] = '\0';
	}

	CUIString::CUIString(const TCHAR ch) : m_pstr(m_szBuffer)
	{
		m_szBuffer[0] = ch;
		m_szBuffer[1] = '\0';
	}

	CUIString::CUIString(LPCTSTR lpsz, int nLen) : m_pstr(m_szBuffer)
	{      
		ASSERT(!::IsBadStringPtr(lpsz,-1) || lpsz==NULL);
		m_szBuffer[0] = '\0';
		Assign(lpsz, nLen);
	}

	CUIString::CUIString(const CUIString& src) : m_pstr(m_szBuffer)
	{
		m_szBuffer[0] = '\0';
		Assign(src.m_pstr);
	}

	CUIString::~CUIString()
	{
		if( m_pstr != m_szBuffer ) free(m_pstr);
	}

	int CUIString::GetLength() const
	{ 
		return (int) _tcslen(m_pstr); 
	}

	CUIString::operator LPCTSTR() const 
	{ 
		return m_pstr; 
	}

	void CUIString::Append(LPCTSTR pstr)
	{
		int nNewLength = GetLength() + (int) _tcslen(pstr);
		if( nNewLength >= MAX_LOCAL_STRING_LEN ) {
			if( m_pstr == m_szBuffer ) {
				m_pstr = static_cast<LPTSTR>(malloc((nNewLength + 1) * sizeof(TCHAR)));
				_tcscpy(m_pstr, m_szBuffer);
				_tcscat(m_pstr, pstr);
			}
			else {
				m_pstr = static_cast<LPTSTR>(realloc(m_pstr, (nNewLength + 1) * sizeof(TCHAR)));
				_tcscat(m_pstr, pstr);
			}
		}
		else {
			if( m_pstr != m_szBuffer ) {
				free(m_pstr);
				m_pstr = m_szBuffer;
			}
			_tcscat(m_szBuffer, pstr);
		}
	}

	void CUIString::Assign(LPCTSTR pstr, int cchMax)
	{
		if( pstr == NULL ) pstr = _T("");
		cchMax = (cchMax < 0 ? (int) _tcslen(pstr) : cchMax);
		if( cchMax < MAX_LOCAL_STRING_LEN ) {
			if( m_pstr != m_szBuffer ) {
				free(m_pstr);
				m_pstr = m_szBuffer;
			}
		}
		else if( cchMax > GetLength() || m_pstr == m_szBuffer ) {
			if( m_pstr == m_szBuffer ) m_pstr = NULL;
			m_pstr = static_cast<LPTSTR>(realloc(m_pstr, (cchMax + 1) * sizeof(TCHAR)));
		}
		_tcsncpy(m_pstr, pstr, cchMax);
		m_pstr[cchMax] = '\0';
	}

	bool CUIString::IsEmpty() const 
	{ 
		return m_pstr[0] == '\0'; 
	}

	void CUIString::Empty() 
	{ 
		if( m_pstr != m_szBuffer ) free(m_pstr);
		m_pstr = m_szBuffer;
		m_szBuffer[0] = '\0'; 
	}

	LPCTSTR CUIString::GetData() const
	{
		return m_pstr;
	}

	TCHAR CUIString::GetAt(int nIndex) const
	{
		return m_pstr[nIndex];
	}

	TCHAR CUIString::operator[] (int nIndex) const
	{ 
		return m_pstr[nIndex];
	}   

	const CUIString& CUIString::operator=(const CUIString& src)
	{      
		Assign(src);
		return *this;
	}

	const CUIString& CUIString::operator=(LPCTSTR lpStr)
	{      
		if ( lpStr )
		{
			ASSERT(!::IsBadStringPtr(lpStr,-1));
			Assign(lpStr);
		}
		else
		{
			Empty();
		}
		return *this;
	}

#ifdef _UNICODE

	const CUIString& CUIString::operator=(LPCSTR lpStr)
	{
		if ( lpStr )
		{
			ASSERT(!::IsBadStringPtrA(lpStr,-1));
			int cchStr = (int) strlen(lpStr) + 1;
			LPWSTR pwstr = (LPWSTR) _alloca(cchStr);
			if( pwstr != NULL ) ::MultiByteToWideChar(::GetACP(), 0, lpStr, -1, pwstr, cchStr) ;
			Assign(pwstr);
		}
		else
		{
			Empty();
		}
		return *this;
	}

	const CUIString& CUIString::operator+=(LPCSTR lpStr)
	{
		if ( lpStr )
		{
			ASSERT(!::IsBadStringPtrA(lpStr,-1));
			int cchStr = (int) strlen(lpStr) + 1;
			LPWSTR pwstr = (LPWSTR) _alloca(cchStr);
			if( pwstr != NULL ) ::MultiByteToWideChar(::GetACP(), 0, lpStr, -1, pwstr, cchStr) ;
			Append(pwstr);
		}
		
		return *this;
	}

#else

	const CUIString& CUIString::operator=(LPCWSTR lpwStr)
	{      
		if ( lpwStr )
		{
			ASSERT(!::IsBadStringPtrW(lpwStr,-1));
			int cchStr = ((int) wcslen(lpwStr) * 2) + 1;
			LPSTR pstr = (LPSTR) _alloca(cchStr);
			if( pstr != NULL ) ::WideCharToMultiByte(::GetACP(), 0, lpwStr, -1, pstr, cchStr, NULL, NULL);
			Assign(pstr);
		}
		else
		{
			Empty();
		}
		
		return *this;
	}

	const CUIString& CUIString::operator+=(LPCWSTR lpwStr)
	{
		if ( lpwStr )
		{
			ASSERT(!::IsBadStringPtrW(lpwStr,-1));
			int cchStr = ((int) wcslen(lpwStr) * 2) + 1;
			LPSTR pstr = (LPSTR) _alloca(cchStr);
			if( pstr != NULL ) ::WideCharToMultiByte(::GetACP(), 0, lpwStr, -1, pstr, cchStr, NULL, NULL);
			Append(pstr);
		}
		
		return *this;
	}

#endif // _UNICODE

	const CUIString& CUIString::operator=(const TCHAR ch)
	{
		Empty();
		m_szBuffer[0] = ch;
		m_szBuffer[1] = '\0';
		return *this;
	}

	CUIString CUIString::operator+(const CUIString& src) const
	{
		CUIString sTemp = *this;
		sTemp.Append(src);
		return sTemp;
	}

	CUIString CUIString::operator+(LPCTSTR lpStr) const
	{
		if ( lpStr )
		{
			ASSERT(!::IsBadStringPtr(lpStr,-1));
			CUIString sTemp = *this;
			sTemp.Append(lpStr);
			return sTemp;
		}

		return *this;
	}

	const CUIString& CUIString::operator+=(const CUIString& src)
	{      
		Append(src);
		return *this;
	}

	const CUIString& CUIString::operator+=(LPCTSTR lpStr)
	{      
		if ( lpStr )
		{
			ASSERT(!::IsBadStringPtr(lpStr,-1));
			Append(lpStr);
		}
		
		return *this;
	}

	const CUIString& CUIString::operator+=(const TCHAR ch)
	{      
		TCHAR str[] = { ch, '\0' };
		Append(str);
		return *this;
	}

	bool CUIString::operator == (LPCTSTR str) const { return (Compare(str) == 0); };
	bool CUIString::operator != (LPCTSTR str) const { return (Compare(str) != 0); };
	bool CUIString::operator <= (LPCTSTR str) const { return (Compare(str) <= 0); };
	bool CUIString::operator <  (LPCTSTR str) const { return (Compare(str) <  0); };
	bool CUIString::operator >= (LPCTSTR str) const { return (Compare(str) >= 0); };
	bool CUIString::operator >  (LPCTSTR str) const { return (Compare(str) >  0); };

	void CUIString::SetAt(int nIndex, TCHAR ch)
	{
		ASSERT(nIndex>=0 && nIndex<GetLength());
		m_pstr[nIndex] = ch;
	}

	int CUIString::Compare(LPCTSTR lpsz) const 
	{ 
		return _tcscmp(m_pstr, lpsz); 
	}

	int CUIString::CompareNoCase(LPCTSTR lpsz) const 
	{ 
		return _tcsicmp(m_pstr, lpsz); 
	}

	void CUIString::MakeUpper() 
	{ 
		_tcsupr(m_pstr); 
	}

	void CUIString::MakeLower() 
	{ 
		_tcslwr(m_pstr); 
	}

	CUIString CUIString::Left(int iLength) const
	{
		if( iLength < 0 ) iLength = 0;
		if( iLength > GetLength() ) iLength = GetLength();
		return CUIString(m_pstr, iLength);
	}

	CUIString CUIString::Mid(int iPos, int iLength) const
	{
		if( iLength < 0 ) iLength = GetLength() - iPos;
		if( iPos + iLength > GetLength() ) iLength = GetLength() - iPos;
		if( iLength <= 0 ) return CUIString();
		return CUIString(m_pstr + iPos, iLength);
	}

	CUIString CUIString::Right(int iLength) const
	{
		int iPos = GetLength() - iLength;
		if( iPos < 0 ) {
			iPos = 0;
			iLength = GetLength();
		}
		return CUIString(m_pstr + iPos, iLength);
	}

	int CUIString::Find(TCHAR ch, int iPos /*= 0*/) const
	{
		ASSERT(iPos>=0 && iPos<=GetLength());
		if( iPos != 0 && (iPos < 0 || iPos >= GetLength()) ) return -1;
		LPCTSTR p = _tcschr(m_pstr + iPos, ch);
		if( p == NULL ) return -1;
		return (int)(p - m_pstr);
	}

	int CUIString::Find(LPCTSTR pstrSub, int iPos /*= 0*/) const
	{
		ASSERT(!::IsBadStringPtr(pstrSub,-1));
		ASSERT(iPos>=0 && iPos<=GetLength());
		if( iPos != 0 && (iPos < 0 || iPos > GetLength()) ) return -1;
		LPCTSTR p = _tcsstr(m_pstr + iPos, pstrSub);
		if( p == NULL ) return -1;
		return (int)(p - m_pstr);
	}

	int CUIString::ReverseFind(TCHAR ch) const
	{
		LPCTSTR p = _tcsrchr(m_pstr, ch);
		if( p == NULL ) return -1;
		return (int)(p - m_pstr);
	}

	int CUIString::Replace(LPCTSTR pstrFrom, LPCTSTR pstrTo)
	{
		CUIString sTemp;
		int nCount = 0;
		int iPos = Find(pstrFrom);
		if( iPos < 0 ) return 0;
		int cchFrom = (int) _tcslen(pstrFrom);
		int cchTo = (int) _tcslen(pstrTo);
		while( iPos >= 0 ) {
			sTemp = Left(iPos);
			sTemp += pstrTo;
			sTemp += Mid(iPos + cchFrom);
			Assign(sTemp);
			iPos = Find(pstrFrom, iPos + cchTo);
			nCount++;
		}
		return nCount;
	}

	int CUIString::Format(LPCTSTR pstrFormat, ...)
	{
        int nLen = 0;
		LPTSTR szSprintf = NULL;
		va_list argList = NULL;
		va_start(argList, pstrFormat);

        nLen = ::_vsntprintf(NULL, 0, pstrFormat, argList);
        szSprintf = (TCHAR*)malloc((nLen + 1) * sizeof(TCHAR));
        ZeroMemory(szSprintf, (nLen + 1) * sizeof(TCHAR));

		int nRet = ::_vsntprintf(szSprintf, nLen + 1, pstrFormat, argList);
		va_end(argList);
		Assign(szSprintf);
        free(szSprintf);
		return nRet;
	}

	int CUIString::SmallFormat(LPCTSTR pstrFormat, ...)
	{
		CUIString sFormat = pstrFormat;
		TCHAR szBuffer[64] = { 0 };
		va_list argList;
		va_start(argList, pstrFormat);

		int iRet = ::wvsprintf(szBuffer, sFormat, argList);
		va_end(argList);
		Assign(szBuffer);
		return iRet;
	}

	bool CUIString::LoadString(int nID)
	{
		HMODULE hd=::GetModuleHandle(NULL);
		if (hd==NULL)
			return false;
		TCHAR szb[256];
		ZeroMemory(szb,256*sizeof(TCHAR));
		int nLen=::LoadString(hd, nID,szb,256);
		if (nLen<=0)
			return false;
		if (nLen<256) {
			Assign(szb);
			return true;
		}
		LPTSTR sz=(TCHAR*)malloc((nLen+1)*sizeof(TCHAR));
		ZeroMemory(sz, (nLen + 1) * sizeof(TCHAR));
		::LoadString(hd, nID,sz,nLen+1);
		Assign(sz);
		free(sz);
		return true;
	}

	/////////////////////////////////////////////////////////////////////////////
	//
	//

	static UINT HashKey(LPCTSTR Key)
	{
		UINT i = 0;
		SIZE_T len = _tcslen(Key);
		while( len-- > 0 ) i = (i << 5) + i + Key[len];
		return i;
	}

	static UINT HashKey(const CUIString& Key)
	{
		return HashKey((LPCTSTR)Key);
	};

	CStdStringPtrMap::CStdStringPtrMap(int nSize) : m_nCount(0)
	{
		if( nSize < 16 ) nSize = 16;
		m_nBuckets = nSize;
		m_aT = new TITEM*[nSize];
		memset(m_aT, 0, nSize * sizeof(TITEM*));
	}

	CStdStringPtrMap::~CStdStringPtrMap()
	{
		if( m_aT ) {
			int len = m_nBuckets;
			while( len-- ) {
				TITEM* pItem = m_aT[len];
				while( pItem ) {
					TITEM* pKill = pItem;
					pItem = pItem->pNext;
					delete pKill;
				}
			}
			delete [] m_aT;
			m_aT = NULL;
		}
	}

	void CStdStringPtrMap::RemoveAll()
	{
		this->Resize(m_nBuckets);
	}

	void CStdStringPtrMap::Resize(int nSize)
	{
		if( m_aT ) {
			int len = m_nBuckets;
			while( len-- ) {
				TITEM* pItem = m_aT[len];
				while( pItem ) {
					TITEM* pKill = pItem;
					pItem = pItem->pNext;
					delete pKill;
				}
			}
			delete [] m_aT;
			m_aT = NULL;
		}

		if( nSize < 0 ) nSize = 0;
		if( nSize > 0 ) {
			m_aT = new TITEM*[nSize];
			memset(m_aT, 0, nSize * sizeof(TITEM*));
		} 
		m_nBuckets = nSize;
		m_nCount = 0;
	}

	LPVOID CStdStringPtrMap::Find(LPCTSTR key, bool optimize) const
	{
		if( m_nBuckets == 0 || GetSize() == 0 ) return NULL;

		UINT slot = HashKey(key) % m_nBuckets;
		for (TITEM* pItem = m_aT[slot]; pItem; pItem = pItem->pNext) 
		{
			if (pItem->Key == key)
			{
				if (optimize && pItem != m_aT[slot])
				{
					if (pItem->pNext)
					{
						pItem->pNext->pPrev = pItem->pPrev;
					}
					pItem->pPrev->pNext = pItem->pNext;
					pItem->pPrev = NULL;
					pItem->pNext = m_aT[slot];
					pItem->pNext->pPrev = pItem;
					//��item�ƶ�������ͷ��
					m_aT[slot] = pItem;
				}
				return pItem->Data;
			}        
		}

		return NULL;
	}

	bool CStdStringPtrMap::Insert(LPCTSTR key, LPVOID pData)
	{
		if (m_nBuckets == 0) return false;
		if (Find(key)) return false;

		// Add first in bucket
		UINT slot = HashKey(key) % m_nBuckets;
		TITEM* pItem = new TITEM;
		pItem->Key = key;
		pItem->Data = pData;
		pItem->pPrev = NULL;
		pItem->pNext = m_aT[slot];
		if (pItem->pNext)
			pItem->pNext->pPrev = pItem;
		m_aT[slot] = pItem;
		m_nCount++;
		return true;
	}

	LPVOID CStdStringPtrMap::Set(LPCTSTR key, LPVOID pData)
	{
		if( m_nBuckets == 0 ) return pData;

		if (GetSize()>0) {
			UINT slot = HashKey(key) % m_nBuckets;
			// Modify existing item
			for( TITEM* pItem = m_aT[slot]; pItem; pItem = pItem->pNext ) {
				if( pItem->Key == key ) {
					LPVOID pOldData = pItem->Data;
					pItem->Data = pData;
					return pOldData;
				}
			}
		}

		Insert(key, pData);
		return NULL;
	}

	bool CStdStringPtrMap::Remove(LPCTSTR key)
	{
		if( m_nBuckets == 0 || GetSize() == 0 ) return false;

		UINT slot = HashKey(key) % m_nBuckets;
		TITEM** ppItem = &m_aT[slot];
		while (*ppItem)
		{
			if ((*ppItem)->Key == key) 
			{
				TITEM* pKill = *ppItem;
				*ppItem = (*ppItem)->pNext;
				if (*ppItem)
					(*ppItem)->pPrev = pKill->pPrev;
				delete pKill;
				m_nCount--;
				return true;
			}
			ppItem = &((*ppItem)->pNext);
		}

		return false;
	}

	int CStdStringPtrMap::GetSize() const
	{
#if 0//def _DEBUG
		int nCount = 0;
		int len = m_nBuckets;
		while( len-- ) {
			for( const TITEM* pItem = m_aT[len]; pItem; pItem = pItem->pNext ) nCount++;
		}
		ASSERT(m_nCount==nCount);
#endif
		return m_nCount;
	}

	LPCTSTR CStdStringPtrMap::GetAt(int nIndex) const
	{
		if( m_nBuckets == 0 || GetSize() == 0 ) return false;

		int pos = 0;
		int len = m_nBuckets;
		while( len-- ) {
			for( TITEM* pItem = m_aT[len]; pItem; pItem = pItem->pNext ) {
				if( pos++ == nIndex ) {
					return pItem->Key.GetData();
				}
			}
		}

		return NULL;
	}

	LPCTSTR CStdStringPtrMap::operator[] (int nIndex) const
	{
		return GetAt(nIndex);
	}


	/////////////////////////////////////////////////////////////////////////////////////
	//
	//

	CWaitCursor::CWaitCursor()
	{
		m_hOrigCursor = ::SetCursor(::LoadCursor(NULL, IDC_WAIT));
	}

	CWaitCursor::~CWaitCursor()
	{
		::SetCursor(m_hOrigCursor);
	}

} // namespace DirectUI
