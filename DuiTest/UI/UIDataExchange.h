#pragma once
#include <map>
#include <functional>

/**
* 描述: 提供与DUI数据交换的功能
*通过GetClass判断控件类型,支持UI的属性:
*CheckBoxUI/OptionUI->选中/非选中 ,值为 "1"/"0"
*EditUI/RichEditUI/ControlUI ->文字
*ComboUI->USERData
*/
class CUIDataExchange
{

public:
	
	//约束函数
	typedef void (*pFuncConstraint)(int get_form_ui /*1 = UI to Memory 0 = Memory to UI*/, \
		CControlUI* pCtl/*当前控件*/, CString strCtlName/*控件名称*/, CString& strData/*当前存储的控件数据,根据需要修改*/);
#define FuncConstraint void(int,CControlUI*,CString,CString&)
	enum ConstraintValue
	{
		//内存数据简单的约束设定
		//运算类型主要针对Text
		NON = 0x0, //没有操作
		ADD = 0x01, // +
		SUB = 0x02, // -
		MUL = 0x04, //*
		DIV = 0x08,  //  /
		ZFIX = 0x10, // 如果数据值0,设为指定值
		/**
		*下面约束键值为用户自定义操作设置
		*根据get_form_ui判断数据走向
		*使用UDE_Constraint_Func绑定回调函数
		**/
		FUNC = 0x20, // 普通函数绑定
		OFUNC = 0x40, // 类函数绑定
		END
	};

#define FuncUpdate void(CControlUI*, CString&) //外部update函数 

public:
	/**
	** 描述: 传入容器并且不为空
	**/
	CUIDataExchange(CContainerUI*);

	/**
	** 描述: 映射DUI控件到内部存储,该控件必须有名称属性,并且名称唯一
	**/
	void UDE_Control(CString strCtlName);

	/**
	** 描述: 创建内存数据到UI的约束条件,主要针对控件Text属性,一些简单的单步数学运算
	** value: 使用value内的值进行约束
	** 复杂的约束请使用绑定函数解决问题
	** 不能接受FUNC以下的枚举值
	**/
	void UDE_Constraint(CString strCtlName, ConstraintValue opt, LONG value);

	/**
	** 描述: 自操作UI的约束条件,制定控件的行为,注:与UDE_Constraint互斥
	** pFunc: 函数绑定,传入符合FuncConstraint规则的类或普通函数
	** pthis: 类函数绑定,pthis必须不为NULL
	**/
	template <typename FnType, typename O /*类类型,不使用类函数绑定时设置一些普通类型如void*/>
	void UDE_Constraint_Func(CString strCtlName, FnType pFunc, O* pthis = NULL);
	
	/**
	** 描述: 对没有定义的控件数据获取设置函数,进行函数注册.具体原有支持的控件在类注释上查看
	** strClass: 控件类的GetClass名称
	** getFormUI/setToUI: 函数绑定,传入符合FuncUpdate规则的类或普通函数
	** pthis: 类函数绑定,pthis必须不为NULL
	** note: 有覆盖作用,后设置的函数会覆盖同类型的原有函数.Update总是在Constraint之前调用.注意的处理这些关系.
	**/
	template <typename FnType, typename O /*类类型,不使用类函数绑定时设置一些普通类型如void*/>
	void UDE_Update_Func(CString strClass, FnType getFormUI, FnType setToUI, O* pthis = NULL);


	/**
	** 描述: 进行UI的数据交换操作
	** bGet:
	**  TRUE: 从UI获取数据
	**  FALSE: 将修改的数据Update到UI
	**/
	virtual BOOL UpdateData(BOOL bGetFromUI = FALSE);

	/**
	** 描述: 根据控件获取数据
	**/
	CString GetData(CControlUI* pCtl);

	/**
	** 描述: 根据控件名称获取数据
	**/
	CString GetData(CString strCtlName);

	/**
	** 描述: 根据控件名称设置数据值
	**/
	BOOL SetData(CControlUI* pCtl, CString strData);

	/**
	** 描述: 根据控件名称设置数据值
	**/
	BOOL SetData(CString strCtlName, CString strData);

	/**
	** 描述: 禁用和启用所有绑定的控件
	**/
	virtual void SetEnabled(BOOL bEnable);

	/**
	** 描述: 清空绑定控件EditUI/RichEditUI/ControlUI的text
	**/
	virtual void ClearText();

protected:
	CContainerUI* m_pRoot;
	CPaintManagerUI& m_pPaintManager;
protected:

	struct DataLeve
	{
		CControlUI* pCtl;
		CString cur;
		CString cur_;//cache of cur,when updata to ui
		ConstraintValue opt;
		union ValueByte
		{
			LONG vLong;
		} value;
		std::function<FuncConstraint> pfun;
		DataLeve() :pCtl(0), opt(NON) { ::memset(&value, 0, sizeof(ValueByte)); }
	};

	typedef std::map<CString, DataLeve>::iterator ExDataIter;

	struct _tagFuncUpdate
	{
		std::function<FuncUpdate> get;
		std::function<FuncUpdate> set;
		_tagFuncUpdate(){}
		_tagFuncUpdate(std::function<FuncUpdate> g, std::function<FuncUpdate> s) :get(g), set(s) {}
	};
	std::map<CString, DataLeve> mapExData;
	std::map<CString, _tagFuncUpdate> mapFuncReg;

private:
	void InitControlUpdataFunc();
	void UpdateGetCombo(CControlUI* pCtrl, CString& cur);
	void UpdateSetCombo(CControlUI* pCtrl, CString& cur);
	void UpdateGetEdit(CControlUI* pCtrl, CString& cur);
	void UpdateSetEdit(CControlUI* pCtrl, CString& cur);
	void UpdateGetCheckbox(CControlUI* pCtrl, CString& cur);
	void UpdateSetCheckbox(CControlUI* pCtrl, CString& cur);

	inline void PassConstraint(BOOL bGetFromUI, ExDataIter& iter);
};