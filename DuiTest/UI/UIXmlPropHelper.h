#pragma once
#include <map>
#include <functional>
#include "UIDataExchange.h"
//#include "G:\msys64\home\boost-1_64\include\boost\archive\basic_archive.hpp"
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
	void XmlUpdateCombo(bool bGetFromUI,CControlUI* pCtl, CString& value);
	void XmlUpdateEdit(bool bGetFromUI,CControlUI* pCtl, CString& value);
	void XmlUpdateCheckbox(bool bGetFromUI,CControlUI* pCtl, CString& value);
	void XmlUpdateControl(bool bGetFromUI,CControlUI* pCtl, CString& value);
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