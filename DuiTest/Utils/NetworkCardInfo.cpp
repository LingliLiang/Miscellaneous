#include "StdAfx.h"
#include "NetworkCardInfo.h"
#include <Iphlpapi.h>
#pragma comment(lib,"Iphlpapi.lib") //需要添加Iphlpapi.lib库
#include <WS2tcpip.h>
CNetworkCardInfo::CNetworkCardInfo(void)
{
	m_currentIpIndex = 0;
	GetAllNetworkCardInfo();
}

CNetworkCardInfo::~CNetworkCardInfo(void)
{
}

CNetworkCardInfo::pairIpsMac CNetworkCardInfo::GetCurIpMacInfo()
{
	if(m_vecCardInfo.size()<= m_currentIpIndex) m_currentIpIndex = 0;
	return m_vecCardInfo[m_currentIpIndex];
}

std::vector<CNetworkCardInfo::pairIpsMac> CNetworkCardInfo::GetIpsMacInfo()
{
	return m_vecCardInfo;
}

void CNetworkCardInfo::GetAllNetworkCardInfo()
{
	bool bOverFlow = false;
	//PIP_ADAPTER_INFO结构体指针存储本机网卡信息
	PIP_ADAPTER_INFO pIpAdapterInfoOrg = nullptr;
    PIP_ADAPTER_INFO pIpAdapterInfo = new IP_ADAPTER_INFO();
    //得到结构体大小,用于GetAdaptersInfo参数
    unsigned long stSize = sizeof(IP_ADAPTER_INFO);
    //调用GetAdaptersInfo函数,填充pIpAdapterInfo指针变量;其中stSize参数既是一个输入量也是一个输出量
    int nRel = GetAdaptersInfo(pIpAdapterInfo,&stSize);

    if (ERROR_BUFFER_OVERFLOW==nRel)
    {
        //如果函数返回的是ERROR_BUFFER_OVERFLOW
		//则说明GetAdaptersInfo参数传递的内存空间不够,同时其传出stSize,表示需要的空间大小
        //这也是说明为什么stSize既是一个输入量也是一个输出量
        //释放原来的内存空间
        delete pIpAdapterInfo;
        //重新申请内存空间用来存储所有网卡信息
        pIpAdapterInfo = (PIP_ADAPTER_INFO)new BYTE[stSize];
        //再次调用GetAdaptersInfo函数,填充pIpAdapterInfo指针变量
        nRel=GetAdaptersInfo(pIpAdapterInfo,&stSize);    
		bOverFlow = true;
    }
	pIpAdapterInfoOrg = pIpAdapterInfo;
    if (ERROR_SUCCESS==nRel)
    {
        //输出网卡信息
		//可能有多网卡,因此通过循环去判断
		while (pIpAdapterInfo)
		{
			BasicStr outMac;
			string tempMac;
			char buf[255] = { 0 };
			pIpAdapterInfo->Description;MIB_IF_TYPE_ETHERNET;
			pIpAdapterInfo->Type;
			sprintf_s(buf, "%02x-%02x-%02x-%02x-%02x-%02x",
				pIpAdapterInfo->Address[0], pIpAdapterInfo->Address[1],
				pIpAdapterInfo->Address[2], pIpAdapterInfo->Address[3],
				pIpAdapterInfo->Address[4], pIpAdapterInfo->Address[5]);
			tempMac = buf;
#ifdef UNICODE
			outMac = CA2W(tempMac.c_str());
#else
			outMac = tempMac;
#endif

			VecBasicStr outIps;

			//可能网卡有多IP,因此通过循环去判断
			IP_ADDR_STRING *pIpAddrString =&(pIpAdapterInfo->IpAddressList);
			do 
			{
#ifdef UNICODE
				outIps.push_back((LPTSTR)CA2W(pIpAddrString->IpAddress.String));
#else
				outIps.push_back(pIpAddrString->IpAddress.String);
#endif
				pIpAddrString=pIpAddrString->Next;
			} while (pIpAddrString);

			string strGwIpAddr = pIpAdapterInfo->GatewayList.IpAddress.String;
			if (strGwIpAddr.compare("0.0.0.0") != 0)
			{
				m_currentIpIndex = m_vecCardInfo.size();
			}
			m_vecCardInfo.push_back(std::make_pair(outIps, outMac));
			pIpAdapterInfo = pIpAdapterInfo->Next;
		}
    }
    //释放内存空间
    if (pIpAdapterInfoOrg)
    {
		if(bOverFlow)
			delete (BYTE*)pIpAdapterInfoOrg;
		else
			delete pIpAdapterInfoOrg;
    }
}