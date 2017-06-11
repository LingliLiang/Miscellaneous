#ifndef _FUNCTIONSLOT_H_
#define _FUNCTIONSLOT_H_
#pragma once
#include <vector>
#include <functional>

///////// don't bind virtual function


/**
* do not use this class
**/
template<typename FnType>
class CSlotBase
{
public:
	void Empty()
	{
		vecFuncs.clear();
	}
protected:
	CSlotBase() {}
	CSlotBase(const CSlotBase&) {}
	std::vector<std::function<FnType> > vecFuncs;
};

/**
* NAN Parama Function solt
**/
template<
	typename FnType = void(void)
>
class CFuncSlot :public CSlotBase<FnType>
{
public:
	template <typename O, typename OFnType>
	void Plug(O* pthis, OFnType pFn, bool first = false)
	{
		std::function<FnType> insert;
		if (first)
			vecFuncs.insert(vecFuncs.begin(), insert);
		else
			vecFuncs.push_back(insert);
		*(vecFuncs.rbegin()) = std::bind(pFn, pthis);
	}
	void Active()
	{
		for (auto iter = vecFuncs.begin();iter != vecFuncs.end(); ++iter)
		{
			(*iter)();
		}
	}
};

/**
* One Parama Function solt
**/
template<
	typename PM_1,
	typename FnType = void(PM_1)
>
class CFuncSlot_1 :public CSlotBase<FnType>
{
public:
	template <typename O, typename OFnType>
	void Plug(O* pthis, OFnType pFn, bool first = false)
	{
		std::function<FnType> insert;
		if (first)
			vecFuncs.insert(vecFuncs.begin(), insert);
		else
			vecFuncs.push_back(insert);
		*(vecFuncs.rbegin()) = std::bind(pFn, pthis, std::placeholders::_1);
	}
	void Active(PM_1 p1)
	{
		for (auto iter = vecFuncs.begin();iter != vecFuncs.end(); ++iter)
		{
			(*iter)(p1);
		}
	}
};

/**
* Two Parama Function solt
**/
template<
	typename PM_1,
	typename PM_2,
	typename FnType = void(PM_1, PM_2)
>
class CFuncSlot_2 :public CSlotBase<FnType>
{
public:
	template <typename O, typename OFnType>
	void Plug(O* pthis, OFnType pFn, bool first = false)
	{
		std::function<FnType> insert;
		if (first)
			vecFuncs.insert(vecFuncs.begin(), insert);
		else
			vecFuncs.push_back(insert);
		*(vecFuncs.rbegin()) = std::bind(pFn, pthis, std::placeholders::_1, std::placeholders::_2);
	}
	void Active(PM_1 p1, PM_2 p2)
	{
		for (auto iter = vecFuncs.begin();iter != vecFuncs.end(); ++iter)
		{
			(*iter)(p1, p2);
		}
	}
};

/**
* Three Parama Function solt
**/
template<
	typename PM_1,
	typename PM_2,
	typename PM_3,
	typename FnType = void(PM_1, PM_2, PM_3)
>
class CFuncSlot_3 :public CSlotBase<FnType>
{
public:
	template <typename O, typename OFnType>
	void Plug(O* pthis, OFnType pFn, bool first = false)
	{
		std::function<FnType> insert;
		if (first)
			vecFuncs.insert(vecFuncs.begin(), insert);
		else
			vecFuncs.push_back(insert);
		*(vecFuncs.rbegin()) = std::bind(pFn, pthis, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
	}
	void Active(PM_1 p1, PM_2 p2, PM_3 p3)
	{
		for (auto iter = vecFuncs.begin();iter != vecFuncs.end(); ++iter)
		{
			(*iter)(p1, p2, p3);
		}
	}
};

/**
* Four Parama Function solt
**/
template<
	typename PM_1,
	typename PM_2,
	typename PM_3,
	typename PM_4,
	typename FnType = void(PM_1, PM_2, PM_3, PM_4)
>
class CFuncSlot_4 :public CSlotBase<FnType>
{
public:
	template <typename O, typename OFnType>
	void Plug(O* pthis, OFnType pFn, bool first = false)
	{
		std::function<FnType> insert;
		if (first)
			vecFuncs.insert(vecFuncs.begin(), insert);
		else
			vecFuncs.push_back(insert);
		*(vecFuncs.rbegin()) = std::bind(pFn, pthis, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4);
	}
	void Active(PM_1 p1, PM_2 p2, PM_3 p3, PM_4 p4)
	{
		for (auto iter = vecFuncs.begin();iter != vecFuncs.end(); ++iter)
		{
			(*iter)(p1, p2, p3, p4);
		}
	}
};

#endif //_FUNCTIONSLOT_H_