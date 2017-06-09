#include "StdAfx.h"
#include "NetworkCardInfo.h"
#include <Iphlpapi.h>
#pragma comment(lib,"Iphlpapi.lib") //��Ҫ���Iphlpapi.lib��
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
	//PIP_ADAPTER_INFO�ṹ��ָ��洢����������Ϣ
	PIP_ADAPTER_INFO pIpAdapterInfoOrg = nullptr;
    PIP_ADAPTER_INFO pIpAdapterInfo = new IP_ADAPTER_INFO();
    //�õ��ṹ���С,����GetAdaptersInfo����
    unsigned long stSize = sizeof(IP_ADAPTER_INFO);
    //����GetAdaptersInfo����,���pIpAdapterInfoָ�����;����stSize��������һ��������Ҳ��һ�������
    int nRel = GetAdaptersInfo(pIpAdapterInfo,&stSize);

    if (ERROR_BUFFER_OVERFLOW==nRel)
    {
        //����������ص���ERROR_BUFFER_OVERFLOW
		//��˵��GetAdaptersInfo�������ݵ��ڴ�ռ䲻��,ͬʱ�䴫��stSize,��ʾ��Ҫ�Ŀռ��С
        //��Ҳ��˵��ΪʲôstSize����һ��������Ҳ��һ�������
        //�ͷ�ԭ�����ڴ�ռ�
        delete pIpAdapterInfo;
        //���������ڴ�ռ������洢����������Ϣ
        pIpAdapterInfo = (PIP_ADAPTER_INFO)new BYTE[stSize];
        //�ٴε���GetAdaptersInfo����,���pIpAdapterInfoָ�����
        nRel=GetAdaptersInfo(pIpAdapterInfo,&stSize);    
		bOverFlow = true;
    }
	pIpAdapterInfoOrg = pIpAdapterInfo;
    if (ERROR_SUCCESS==nRel)
    {
        //���������Ϣ
		//�����ж�����,���ͨ��ѭ��ȥ�ж�
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

			//���������ж�IP,���ͨ��ѭ��ȥ�ж�
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
    //�ͷ��ڴ�ռ�
    if (pIpAdapterInfoOrg)
    {
		if(bOverFlow)
			delete (BYTE*)pIpAdapterInfoOrg;
		else
			delete pIpAdapterInfoOrg;
    }
}