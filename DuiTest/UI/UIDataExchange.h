#ifndef __UIDATAEXCHANGE_H__
#define __UIDATAEXCHANGE_H__
#pragma once

class UDEConstraintCallBack
{
public:
	virtual void ConstraintSetData(CString strCtlName, CString& strData) = 0;//Memory to UI
	virtual void ConstraintGetData(CString strCtlName, CString& strData) = 0;//UI to Memory
};

/**
* ����: �ṩ��DUI���ݽ����Ĺ���
*֧��UI������:
*CheckBoxUI/OptionUI->ѡ��/��ѡ��
*EditUI ->����
*ComboUI->USERData
*/
class CUIDataExchange
{

public:
	typedef void (*pFuncConstraint)(CString strCtlName, CString& strData);
	//���EditUI��Լ��
	enum ConstraintValue
	{
		//�ڴ����ݼ򵥵�Լ���趨
		NON = 0x0, //û�в���
		ADD = 0x01, // +
		SUB = 0x02, // -
		MUL = 0x04, //*
		DIV = 0x08,  //  /
		ZFIX = 0x10, // ���0,��Ϊָ��ֵ
		/**
		*����Լ����ֵΪ�û��Զ����������
		*�����̳�UDEConstraintCallBack
		*��ͨ����Ҳ���ṩConstraintSetData/ConstraintGetData����
		**/
		FUNC = 0x20, // ��ͨ������
		OFUNC = 0x40, // �ຯ����
		END
	};


	CUIDataExchange(CPaintManagerUI*);

	/**
	** ����: ӳ��DUI�ؼ����ڲ��洢,�ÿؼ���������������
	**/
	void UDE_Control(CControlUI* pCtl);

	void UDE_Control(CString strCtlName);

	/**
	** ����: �����ڴ����ݵ�UI��Լ������
	** value: ʹ��value�ڵ�ֵ����Լ��
	**  strLinkProp: ���ӵ�������,������ʹ��value����Լ��
	**/
	void UDE_Constraint(CControlUI* pCtl, ConstraintValue opt, int value, CString strLinkProp = _T(""));

	void UDE_Constraint(CString strCtlName, ConstraintValue opt, int value, CString strLinkProp = _T(""));

	/**
	** ����: �Բ���UI��Լ������,ע:��UDE_Constraint����, ����Լ�����EditUI,ControlUI��text���
	** pCallBack: �ຯ����,pFuncGet/pFuncSet ����ΪNULL
	**  pFuncGet: ��ͨ������,��ȡ����,pCallBack����ΪNULL
	**  pFuncSet: ��ͨ������,��������,pCallBack����ΪNULL
	**/
	void UDE_Constraint_Func(CControlUI* pCtl, UDEConstraintCallBack* pCallBack, void* pFuncGet = NULL,  void* pFuncSet = NULL);

	void UDE_Constraint_Func(CString strCtlName, UDEConstraintCallBack* pCallBack, void* pFuncGet = NULL,  void* pFuncSet = NULL);

	/**
	** ����: ����UI�����ݽ�������
	** bGet:
	**  TRUE: ��UI��ȡ����
	**  FALSE: ���޸ĵ�����Update��UI
	**/
	virtual BOOL UpdateData(BOOL bGet = FALSE);

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
	** ����: ������а�Edit����Control�ؼ���text
	**/
	virtual void ClearText();
protected:
	CPaintManagerUI* m_pPaintManager;
private:
	struct DataLeve
	{
		CControlUI* pCtl;
		CString cur;
		ConstraintValue opt;
		union ValueByte
		{
			int vInt;
			CString* pvLink;
			UDEConstraintCallBack* pCall;
			pFuncConstraint pFunc[2];
		} value;
		BOOL bLink; //ʹ��link����ʱע��ؼ����Ƶ�����,��link�Ŀؼ������ڱ��ؼ�֮ǰ����,��alpha����˳��
		DataLeve():pCtl(0),opt(NON),bLink(0){::memset(&value,0,sizeof(ValueByte));}
	};
	typedef std::map<CString,struct DataLeve>::iterator ExDataIter;
	std::map<CString,struct DataLeve> mapExData;
private:
	void UpdateCombo(bool bGetData, ExDataIter& iter);
	void UpdateEdit(bool bGetData, ExDataIter& iter);
	void UpdateCheckbox(bool bGetData, ExDataIter& iter);
	void UpdateControl(bool bGetData, ExDataIter& iter);

	void PassConstraint(bool bGetData, ExDataIter& iter);
};

class UDECollectionCallBack;

class CXmlPropDataHelper : public  CUIDataExchange
{
public:
	struct  CollectionValue
	{
		int index;
		std::vector<CString> vecValues; //��Ӧ�ڵ��µ��������ݽڵ�
		CollectionValue()
			:index(0)
		{}
	};	

	typedef void (*pFuncCollection)(std::vector<CControlUI*>, int countofCtl,CollectionValue&);

	/**
	* CollectionInfo
	**/
	struct CollectionInfo
	{
		friend class CXmlPropDataHelper;
	private:
		union ValueByte
		{
			UDECollectionCallBack* pCall;
			pFuncCollection pFunc[2];
		} value;
		unsigned int change_index;
	public:
		BOOL b_auto; //Reserve - û������vecXmlNodeʱ,�Զ���д 
		BOOL b_attribute; //��countNode��Ϊ���Ի��ǽڵ�ʹ��

		//true Ϊ�ü�����UpdateCollData����,��Ӧ��ctls�ؼ��õ����ݸ���,
		//��ҪSetCollUpdate���õ�ǰ����λ��,ֻ�������õ�������
		//false ʱ��������������,��ctls�ؼ��ò�������
		BOOL b_update; 
		int bind_size; //Reserve - �󶨽ڵ�Ĵ�С
		int adapt_base; //��Ӧ�ڵ����
		CString mainNode; // ���ݼ����ڵ� ex: Node1/Node2/mainNode,�����MainPropλ��
		CString countNode; // ���ݼ��������Խڵ�,��b_attributeӰ��
		CString adaptNode; //���ݼ���Ӧ�ڵ� ex: Node%d
		std::vector<CControlUI*> ctls; //����,һ��ؼ�����CollectionValue��,��һ��Ϊ�ؼ���ʾ��
		std::vector<CString> vecXmlNode; //����mainNode,��Ӧ�ڵ��µ��������ݽڵ�,��ʱָ��adaptNode�ڵ�
		std::vector<CollectionValue> collValues;
		CollectionInfo()
			:change_index(0),b_auto(0),b_attribute(0),b_update(0),bind_size(0),adapt_base(0)
		{::memset(&value,0,sizeof(ValueByte));}

	};
	
	CXmlPropDataHelper(CPaintManagerUI*);
	
	/**
	** ����: ָ����ȡ�ڵ��·��,�ж���ڵ�ʱ,���մӸ����ӵĽڵ����� - ֧�ּ��϶�ȡ
	**/
	void PushMainProp(CString xmlNode);

	BOOL LoadXMLBuffer(LPTSTR& lpszXMLBuffer);

	/**
	** ����: �������нڵ㵽�ڴ� - ֧�ּ��ϱ���
	**/
	BOOL SaveXMLBuffer(LPTSTR& lpszXMLBuffer);

	BOOL LoadXMLFile(LPTSTR lpszXmlFile);

	BOOL SaveXMLFile(LPTSTR lpszXmlFile);

	/**
	** ����: ӳ��XML Node��ֵ��DUI�ؼ�
	** ��ǰ�ڵ���xmlTreeNode��ֵ:
	** "/" ʹ�÷�б����Ϊ�ڵ�ָ��
	** 1, ����ֵΪ��ʱ,ʹ�ÿؼ���������Ϊ�ڵ�, ��ȡ����
	** 2, ����ʽΪ "/Node1",ʹ��Node1��Ϊ�ڵ�, ��ȡ����
	** 3, ����ʽΪ "Node1/Node2/" ���з�б����ĩβʱ, ����Node�Ĳ�μӿؼ������ƽڵ�,��ȡ����
	** 4,����ʽΪ "Node1/Node2" ֱ�Ӹ���Node�Ĳ�νڵ�, ��ȡ����
	**/
	void UDE_Control(CString strCtlName,CString xmlTreeNode);

	void UDE_Control(CControlUI* pCtl,CString xmlTreeNode);

	void UDE_Control_Coll(CollectionInfo& datas, UDECollectionCallBack* pCallBack, void* pFuncGet = NULL,  void* pFuncSet = NULL);

	//��������,����indexΪ������������ʱΪ��Ӳ���
	BOOL SetCollData(std::vector<CString>& values,CControlUI* pCtl,int index);

	BOOL GetCollData(std::vector<CString>& values,CControlUI* pCtl,int index);

	int GetCollSize(CControlUI* pCtl);

	//pCtl - �ؼ���ʾ��,�Զ�λ���ݼ�, index - ��Ҫ���µ�������λ��
	BOOL SetCollUpdate(CControlUI* pCtl,int index);

	/**
	** ����: ���¼�������
	** 1: XmlCollInfo - b_update = true, ctls ���ÿؼ�����==vecXmlNode�ڵ���
	** ʹ��SetCollUpdate�����±�[0~](���Ǵ�adapt_base��ʼ),���ɸ��¶�Ӧ������ؼ���.UDECollectionCallBackֻ֪ͨ��ǰ������
	**2: XmlCollInfo - b_update = false, ctls ���ÿؼ���������һ��,vecXmlNode����
	**SetCollUpdate��Ч.UDECollectionCallBack֪ͨ����������
	**/
	virtual BOOL UpdateCollData(BOOL bGet = FALSE);

	/**
	** ����: ����ch�ִ��ָ��ַ�strSource
	**/
	static void SplitCString(CString strSource, LPCTSTR ch, std::vector<CString>& vecString);

	/**
	** ����: xml��������
	**/
	static bool InPropElem(class CXmlFile* xmlFile, std::vector<CString>& vecXmlNode, bool last_into);
	static void OutPropElem(CXmlFile* xmlFile, std::vector<CString>& vecXmlNode, bool last_out);
	static void InAddPropElem(CXmlFile* xmlFile, std::vector<CString>& vecXmlNode);
	static void OutAddPropElem(CXmlFile* xmlFile, std::vector<CString>& vecXmlNode);
private:
	BOOL LoadXml(CXmlFile& xmlFile);
	BOOL SaveXml(CXmlFile& xmlFile);
	void XmlUpdateCombo(bool bGetData,CControlUI* pCtl, CString& value);
	void XmlUpdateEdit(bool bGetData,CControlUI* pCtl, CString& value);
	void XmlUpdateCheckbox(bool bGetData,CControlUI* pCtl, CString& value);
	void XmlUpdateControl(bool bGetData,CControlUI* pCtl, CString& value);
private:
	typedef std::map<CString,CString>::iterator XmlPropIter;
	std::map<CString,CString> mapXmlProp;
	std::vector<CString> vecXmlMain;
	CString m_tempXml;

	typedef std::map<CControlUI*,CollectionInfo>::iterator XmlCollPropIter;
	std::map<CControlUI*,CollectionInfo> mapXmlCollProp;
};

typedef CXmlPropDataHelper::CollectionValue XmlCollValue;
typedef CXmlPropDataHelper::CollectionInfo XmlCollInfo;

class UDECollectionCallBack
{
public:
	virtual void CollectionSetData(std::vector<CControlUI*> ctls, int countofCtl, XmlCollValue& data) = 0;//Memory to UI
	virtual void CollectionGetData(std::vector<CControlUI*> ctls, int countofCtl, XmlCollValue& data) = 0;//UI to Memory
};


#endif //__UIDATAEXCHANGE_H__