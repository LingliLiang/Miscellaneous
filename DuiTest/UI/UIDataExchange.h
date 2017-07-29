#pragma once
#include <map>
#include <functional>

/**
* ����: �ṩ��DUI���ݽ����Ĺ���
*ͨ��GetClass�жϿؼ�����,֧��UI������:
*CheckBoxUI/OptionUI->ѡ��/��ѡ�� ,ֵΪ "1"/"0"
*EditUI/RichEditUI/ControlUI ->����
*ComboUI->USERData
*/
class CUIDataExchange
{

public:
	
	//Լ������
	typedef void (*pFuncConstraint)(int get_form_ui /*1 = UI to Memory 0 = Memory to UI*/, \
		CControlUI* pCtl/*��ǰ�ؼ�*/, CString strCtlName/*�ؼ�����*/, CString& strData/*��ǰ�洢�Ŀؼ�����,������Ҫ�޸�*/);
#define FuncConstraint void(int,CControlUI*,CString,CString&)
	enum ConstraintValue
	{
		//�ڴ����ݼ򵥵�Լ���趨
		//����������Ҫ���Text
		NON = 0x0, //û�в���
		ADD = 0x01, // +
		SUB = 0x02, // -
		MUL = 0x04, //*
		DIV = 0x08,  //  /
		ZFIX = 0x10, // �������ֵ0,��Ϊָ��ֵ
		/**
		*����Լ����ֵΪ�û��Զ����������
		*����get_form_ui�ж���������
		*ʹ��UDE_Constraint_Func�󶨻ص�����
		**/
		FUNC = 0x20, // ��ͨ������
		OFUNC = 0x40, // �ຯ����
		END
	};

#define FuncUpdate void(CControlUI*, CString&) //�ⲿupdate���� 

public:
	/**
	** ����: �����������Ҳ�Ϊ��
	**/
	CUIDataExchange(CContainerUI*);

	/**
	** ����: ӳ��DUI�ؼ����ڲ��洢,�ÿؼ���������������,��������Ψһ
	**/
	void UDE_Control(CString strCtlName);

	/**
	** ����: �����ڴ����ݵ�UI��Լ������,��Ҫ��Կؼ�Text����,һЩ�򵥵ĵ�����ѧ����
	** value: ʹ��value�ڵ�ֵ����Լ��
	** ���ӵ�Լ����ʹ�ð󶨺����������
	** ���ܽ���FUNC���µ�ö��ֵ
	**/
	void UDE_Constraint(CString strCtlName, ConstraintValue opt, LONG value);

	/**
	** ����: �Բ���UI��Լ������,�ƶ��ؼ�����Ϊ,ע:��UDE_Constraint����
	** pFunc: ������,�������FuncConstraint����������ͨ����
	** pthis: �ຯ����,pthis���벻ΪNULL
	**/
	template <typename FnType, typename O /*������,��ʹ���ຯ����ʱ����һЩ��ͨ������void*/>
	void UDE_Constraint_Func(CString strCtlName, FnType pFunc, O* pthis = NULL);
	
	/**
	** ����: ��û�ж���Ŀؼ����ݻ�ȡ���ú���,���к���ע��.����ԭ��֧�ֵĿؼ�����ע���ϲ鿴
	** strClass: �ؼ����GetClass����
	** getFormUI/setToUI: ������,�������FuncUpdate����������ͨ����
	** pthis: �ຯ����,pthis���벻ΪNULL
	** note: �и�������,�����õĺ����Ḳ��ͬ���͵�ԭ�к���.Update������Constraint֮ǰ����.ע��Ĵ�����Щ��ϵ.
	**/
	template <typename FnType, typename O /*������,��ʹ���ຯ����ʱ����һЩ��ͨ������void*/>
	void UDE_Update_Func(CString strClass, FnType getFormUI, FnType setToUI, O* pthis = NULL);


	/**
	** ����: ����UI�����ݽ�������
	** bGet:
	**  TRUE: ��UI��ȡ����
	**  FALSE: ���޸ĵ�����Update��UI
	**/
	virtual BOOL UpdateData(BOOL bGetFromUI = FALSE);

	/**
	** ����: ���ݿؼ���ȡ����
	**/
	CString GetData(CControlUI* pCtl);

	/**
	** ����: ���ݿؼ����ƻ�ȡ����
	**/
	CString GetData(CString strCtlName);

	/**
	** ����: ���ݿؼ�������������ֵ
	**/
	BOOL SetData(CControlUI* pCtl, CString strData);

	/**
	** ����: ���ݿؼ�������������ֵ
	**/
	BOOL SetData(CString strCtlName, CString strData);

	/**
	** ����: ���ú��������а󶨵Ŀؼ�
	**/
	virtual void SetEnabled(BOOL bEnable);

	/**
	** ����: ��հ󶨿ؼ�EditUI/RichEditUI/ControlUI��text
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