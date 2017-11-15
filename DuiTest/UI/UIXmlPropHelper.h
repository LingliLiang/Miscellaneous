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
		std::vector<CString> vecValues; //适应节点下的所有数据节点
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
		BOOL b_auto; //Reserve - 没有设置vecXmlNode时,自动填写 
		BOOL b_attribute; //将countNode作为属性还是节点使用

		//true 为该集启用UpdateCollData控制,对应的ctls控件得到数据更新,
		//需要SetCollUpdate设置当前数据位置,只更新设置的数据项
		//false 时遍历所有数据项,但ctls控件得不到更新
		BOOL b_update; 
		int bind_size; //Reserve - 绑定节点的大小
		int adapt_base; //适应节点基数
		CString mainNode; // 数据集主节点 ex: Node1/Node2/mainNode,相对于MainProp位置
		CString countNode; // 数据集个数属性节点,受b_attribute影响
		CString adaptNode; //数据集适应节点 ex: Node%d
		std::vector<CControlUI*> ctls; //必设,一组控件控制CollectionValue集,第一个为关键标示符
		std::vector<CString> vecXmlNode; //类似mainNode,适应节点下的所有数据节点,空时指向adaptNode节点
		std::vector<CollectionValue> collValues;
		CollectionInfo()
			:change_index(0),b_auto(0),b_attribute(0),b_update(0),bind_size(0),adapt_base(0)
		{::memset(&value,0,sizeof(ValueByte));}

	};
	
	CXmlPropDataHelper(CPaintManagerUI*);
	
	/**
	** 描述: 指定读取节点的路径,有多个节点时,按照从根到子的节点设置 - 支持集合读取
	**/
	void PushMainProp(CString xmlNode);

	BOOL LoadXMLBuffer(LPTSTR& lpszXMLBuffer);

	/**
	** 描述: 保存所有节点到内存 - 支持集合保存
	**/
	BOOL SaveXMLBuffer(LPTSTR& lpszXMLBuffer);

	BOOL LoadXMLFile(LPTSTR lpszXmlFile);

	BOOL SaveXMLFile(LPTSTR lpszXmlFile);

	/**
	** 描述: 映射XML Node的值到DUI控件
	** 当前节点下xmlTreeNode的值:
	** "/" 使用反斜杠作为节点分割符
	** 1, 当该值为空时,使用控件的名称作为节点, 获取数据
	** 2, 当格式为 "/Node1",使用Node1作为节点, 获取数据
	** 3, 当格式为 "Node1/Node2/" 即有反斜杠在末尾时, 根据Node的层次加控件的名称节点,获取数据
	** 4,当格式为 "Node1/Node2" 直接根据Node的层次节点, 获取数据
	**/
	void UDE_Control(CString strCtlName,CString xmlTreeNode);

	void UDE_Control(CControlUI* pCtl,CString xmlTreeNode);

	void UDE_Control_Coll(CollectionInfo& datas, UDECollectionCallBack* pCallBack, void* pFuncGet = NULL,  void* pFuncSet = NULL);

	//更改数据,仅当index为现有数据项量时为添加操作
	BOOL SetCollData(std::vector<CString>& values,CControlUI* pCtl,int index);

	BOOL GetCollData(std::vector<CString>& values,CControlUI* pCtl,int index);

	int GetCollSize(CControlUI* pCtl);

	//pCtl - 关键标示符,以定位数据集, index - 需要更新的数据项位置
	BOOL SetCollUpdate(CControlUI* pCtl,int index);

	/**
	** 描述: 更新集合数据
	** 1: XmlCollInfo - b_update = true, ctls 设置控件数量==vecXmlNode节点数
	** 使用SetCollUpdate设置下标[0~](不是从adapt_base开始),即可更新对应数据项到控件上.UDECollectionCallBack只通知当前项数据
	**2: XmlCollInfo - b_update = false, ctls 设置控件数量至少一个,vecXmlNode不设
	**SetCollUpdate无效.UDECollectionCallBack通知所有项数据
	**/
	virtual BOOL UpdateCollData(BOOL bGet = FALSE);

	/**
	** 描述: 按照ch字串分割字符strSource
	**/
	static void SplitCString(CString strSource, LPCTSTR ch, std::vector<CString>& vecString);

	/**
	** 描述: xml操作函数
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