#pragma once

#include <limits>
#include <string>

/**
* Global instance Cache:
* attch local instance to Global atom table, get() intance whatever you want in process;
**/
template <typename T>
class CParasiteHost
{
public:
	CParasiteHost(T* p, const TCHAR* perfixString /*max size 110*/,bool freeptr = false)
		:p_(p),perfix_(perfixString),bFreePtr_(freeptr)
	{
		perfixFormat_ = perfixString;
		perfixFormat_.append(_T("_HOST_%p"));
		identity_.resize(128,0);
		_stprintf_s(&identity_[0],127,perfixFormat_.c_str(),p_);
		ClearAtomStrings();
		atom_ = ::GlobalAddAtom(identity_.c_str());
	}

	~CParasiteHost()
	{
		::GlobalDeleteAtom(atom_);
		if(bFreePtr_ && p_) delete p_;
	}

	T* get()
	{
		return p_;
	}

	T* get2()
	{
		T* p = 0;
		std::basic_string<TCHAR> instptr(128,0);
		if(::GlobalGetAtomName(atom_,&instptr[0],127))
		{
			int pos = instptr.rfind(_T("_HOST_"));
			if(pos != -1)
			{
				instptr.erase(0,pos+6);
				_stscanf_s(&instptr[0],_T("%p"),&p);
			}
		}
		return static_cast<T*>(p);
	}

	static T* get3(const TCHAR* perfixString)
	{
		T* p = 0;
		std::basic_string<TCHAR> perfix(perfixString);
		std::basic_string<TCHAR> instptr(128,0);
		perfix.append(_T("_HOST_%p"));
		TCHAR buf[128] = {0};
		_stprintf_s(buf,perfix.c_str(),p);
		size_t trueLen = _tcslen(buf);
		perfix.erase(perfix.length()-2,2);
		for(ATOM it = MAXINTATOM; it < 0xFFFF; it++)
		{
			if(::GlobalGetAtomName(it,buf,127))
			{
				if(_tcslen(buf) != trueLen) continue;
				instptr = buf;
				buf[perfix.length()] = _T('\0');
				if(perfix.compare(buf) == 0)
				{
					size_t pos = instptr.rfind(_T("_HOST_"));
					if(pos != std::string::npos)
					{
						instptr.erase(0,pos+6);
						_stscanf_s(&instptr[0],_T("%p"),&p);
					}
					break;
				}
			}
		}
		return p;
	}

protected:
	void ClearAtomStrings()
	{
		//clear existed string atom
		TCHAR buf[128] = {0};
		void* p=0;
		_stprintf_s(buf,perfixFormat_.c_str(),p);
		trueLen_ = _tcslen(buf);
		for(ATOM it = MAXINTATOM; it < 0xFFFF; it++)
		{
			if(::GlobalGetAtomName(it,buf,127))
			{
				if(_tcslen(buf) != trueLen_) continue;
				buf[perfix_.length()] = _T('\0');
				if(perfix_.compare(buf) == 0)
				{
					::GlobalDeleteAtom(it);
				}
			}
		}
	}
private:
	//not copyable
	CParasiteHost();
	CParasiteHost(const CParasiteHost&);
    CParasiteHost& operator = (const CParasiteHost&);

	std::basic_string<TCHAR> perfix_; //perfix string
	std::basic_string<TCHAR> perfixFormat_; //string format like perfix_%p, for scanf ect.
	std::basic_string<TCHAR> identity_;// identity string max size 127
	unsigned int trueLen_; //length of string in atom table
	bool bFreePtr_;
	ATOM atom_; // host atom
	T* p_; //host ptr
};


/**
* Global instance obtain:
* in process dll etc.;
**/
template <typename T>
class CWorm
{
public:
	CWorm(const TCHAR* perfixString)
		:p_(0),perfix_(perfixString),bCache(false)
	{
		perfixFormat_ = perfixString;
		perfixFormat_.append(_T("_HOST_%p"));
	}
	T* get()
	{
		if(p_) return p_;
		if(!bCache) FindAtomStrings();
		return static_cast<T*>(p_);
	}
protected:
	void FindAtomStrings()
	{
		std::basic_string<TCHAR> instptr(128,0);
		TCHAR buf[128] = {0};
		void* p=0;
		_stprintf_s(buf,perfixFormat_.c_str(),p);
		size_t trueLen = _tcslen(buf);
		for(ATOM it = MAXINTATOM; it < 0xFFFF; it++)
		{
			if(::GlobalGetAtomName(it,buf,127))
			{
				if(_tcslen(buf) != trueLen) continue;
				instptr = buf;
				buf[perfix_.length()] = _T('\0');
				if(perfix_.compare(buf) == 0)
				{
					size_t pos = instptr.rfind(_T("_HOST_"));
					if(pos != std::string::npos)
					{
						instptr.erase(0,pos+6);
						_stscanf_s(&instptr[0],_T("%p"),&p_);
					}
					bCache = true;
					break;
				}
			}
		}
	}
private:
	//not copyable
	CWorm();
	CWorm(const CWorm&);
    CWorm& operator = (const CWorm&);

	std::basic_string<TCHAR> perfix_; //perfix string
	std::basic_string<TCHAR> perfixFormat_; //string format like perfix_%p, for scanf ect.
	bool bCache;
	T* p_; // ptr
};

/**
Usage:
Setup global var - 
CParasiteHost<your class type> somename(class ptr , string of unique );

Get  your class instance - 
CWorm<your class type> inst(string of unique);
inst get();
**/