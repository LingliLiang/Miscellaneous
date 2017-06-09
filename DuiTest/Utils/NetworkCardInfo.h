#pragma once

/**
获取网卡信息
**/
class CNetworkCardInfo
{
public:
	typedef std::basic_string<TCHAR> BasicStr;
	typedef std::vector<std::basic_string<TCHAR>> VecBasicStr;
	typedef std::pair<VecBasicStr, BasicStr> pairIpsMac;

	CNetworkCardInfo(void);
	~CNetworkCardInfo(void);
	std::vector<pairIpsMac> GetIpsMacInfo();
	pairIpsMac GetCurIpMacInfo();
private:
	void GetAllNetworkCardInfo();
	std::vector<pairIpsMac> m_vecCardInfo;
	size_t m_currentIpIndex;
};

