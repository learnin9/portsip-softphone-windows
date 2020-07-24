#pragma  once
#include"system/CriticalSection.h"
#include "BaseThread.h"
class CConferenceStatusMgr
{
public:
	CConferenceStatusMgr();
	~CConferenceStatusMgr();

	void AddConferenceNumber(std::string strConferenceNumber);
	void UnSubConferenceNumber(std::string strConferenceNumber);
	bool UnSubAllConference();
protected:
	std::map<std::string,long > m_mapConferenceID2SubStatus;
private:
};