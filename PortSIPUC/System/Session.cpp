
#include "StdAfx.h"
#include "Session.h"
#include "Main/MainWnd.h"


Session::Session()
		:mSessionId(0)
		,mCallState(false)
		,mRecvCallState(false)
		,mConference(false)
		,mHoldState(false)
		,mSecond1(0)
		,mSecond2(0)
		,mMinute1(0)
		,mMinute2(0)
		,mHour1(0)
		,mHour2(0)
		,mHasVideo(false)
		,mSendVideoState(false)
		,mOriginCallSessionId(-1)
		,mIsReferCall(false)
		,m_nID(0)
{

}




Session::~Session()
{

}




void Session::reset()
{
	mSessionId = 0;
	mCallState = false;
	mRecvCallState = false;
	mConference = false;
	mHoldState = false;
	mSecond1 = 0;
	mSecond2 = 0;
	mMinute1 = 0;
	mMinute2 = 0;
	mHour1 = 0;
	mHour2 = 0;

	mHasVideo = false;

	mSendVideoState = false;
	mOriginCallSessionId = -1;
	mIsReferCall = false;
	m_bRecord	= false;
	
	m_strFileName = "";
	m_strFilePath = "";
	m_oSessionInfo.reset();
}




bool Session::getCallState()
{
	return mCallState;
}


void Session::setCallState(bool state)
{
	mCallState = state;
}


void Session::setRecvCallState(bool state)
{
	mRecvCallState = state;
}



bool Session::getRecvCallState()
{
	return mRecvCallState;
}



void Session::setPhoneNumber(const CString & number)
{
	mPhoneNumber = number;
}



void Session::setTextLine1(const CString & text)
{
	mLineText1 = text;
}


void Session::setTextLine2(const CString & text)
{
	mLineText2 = text;
}


void Session::setTextLine3(const CString & text)
{
	mLineText3 = text;
}



long Session::getSessionId()
{
	return mSessionId;
}



void Session::setSessionId(long id)
{
	mSessionId = id;
}



CString Session::getPhoneNumber()
{
	return mPhoneNumber;
}





CString Session::getTextLine1()
{
	return mLineText1;
}


CString Session::getTextLine2()
{
	return mLineText2;
}


CString Session::getTextLine3()
{
	return mLineText3;
}



void Session::setHoldState(bool state)
{
	mHoldState = state;
}


bool Session::getHoldState()
{
	return mHoldState;
}


void Session::setConferenceState(bool state)
{
	mConference = state;
}



bool Session::getConferenceState()
{
	return mConference;
}



void Session::resetCallTimer()
{
	mSecond1 = mSecond2 = 0;
	mMinute1 = mMinute2 = 0;
	mHour1 = mHour2 = 0;
}


CString Session::refreshCallTimer()
{
	WCHAR temp1[8] = { 0 };

	++mSecond1;
	if (mSecond1 <= 9) 
	{
		_itow(mSecond2, temp1, 10);
		_itow(mSecond1, temp1+1, 10);
	}
	else
	{
		mSecond1 = 0;
		++mSecond2;
		_itow(mSecond2, temp1, 10);
		_itow(mSecond1, temp1+1, 10);
	}


	if (mSecond2 >= 6) 
	{
		wcscpy(temp1, L"00");
		mSecond1 = mSecond2 = 0;
		++mMinute1;
	}


	WCHAR temp2[4] = { 0 };
	if (mMinute1 <= 9) 
	{
		_itow(mMinute2, temp2, 10);
		_itow(mMinute1, temp2+1, 10);
	}
	else
	{
		mMinute1 = 0;
		++mMinute2;
		_itow(mMinute2, temp2, 10);
		_itow(mMinute1, temp2+1, 10);
	}

	if (mMinute2 >= 6) 
	{
		wcscpy(temp1, L"00");
		wcscpy(temp2, L"00");
		mSecond1 = mSecond2 = 0;
		mMinute1 = mMinute2 = 0;

		++mHour1;
	}

	WCHAR temp3[4] = { 0 };
	if (mHour1 <= 9) 
	{
		_itow(mHour2, temp3, 10);
		_itow(mHour1, temp3+1, 10);
	}
	else
	{
		mHour1 = 0;
		++mHour2;
		_itow(mHour2, temp3, 10);
		_itow(mHour1, temp3+1, 10);
	}
	//TO MATCH LAGUAGE
	CString info  = L"";
	info += temp3;
	info += L":";
	info += temp2;
	info += L":";
	info += temp1;

	mLineText3 = info;

	return info;
}



void Session::setSendVideoState(bool state)
{
	mSendVideoState = state;
}


bool Session::getSendVideoState()
{
	return mSendVideoState;
}




//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////


SessionInfo::SessionInfo()
			:m_lCallEndTime(0)
			,m_lCallStartTime(0)
			,mCallType(call_out)
			,mState(false)
{

}


SessionInfo::~SessionInfo()
{

}




void SessionInfo::startCall()
{
	m_lCallStartTime = CTime::GetCurrentTime();
	mState = true;
}


void SessionInfo::endCall()
{
	m_lCallEndTime = CTime::GetCurrentTime();
}

CString SessionInfo::getCallStartTime()
{
	return convertCallTime(true);
}


CString SessionInfo::getCallEndTime()
{
	return convertCallTime(false);
}


CString SessionInfo::convertCallTime(bool Start)
{
	CString CallTime;

	USES_CONVERSION;

	if (Start == true) 
	{
		CallTime = m_lCallStartTime.Format(L"%Y-%m-%d %H:%M:%S %p");
	
	}
	else
	{
		CallTime = m_lCallEndTime.Format(L"%Y-%m-%d %H:%M:%S %p");

	}

	return CallTime;
}



void SessionInfo::startMediaStream()
{
	m_dwMediaStartTime = timeGetTime();
}


void SessionInfo::endMediaStream()
{
	m_dwMediaEndTime = timeGetTime();
}



CString SessionInfo::getMediaSessionTime()
{
	return converMediaSessionTime();
}


CString SessionInfo::converMediaSessionTime()
{
	CString ret = L"00:00:00";

	if (m_dwMediaStartTime == 0)
	{
		return ret;
	}


	DWORD dwSecond = (m_dwMediaEndTime - m_dwMediaStartTime) / 1000;

	int nHour = dwSecond / 3600;
	int nMin  = ( dwSecond - nHour * 3600 ) / 60;
	int nSec  = dwSecond - nHour * 3600 - nMin * 60;

	WCHAR szTime[100] = { 0 };
	wsprintf(szTime, L"%02d:%02d:%02d", nHour, nMin, nSec);

	ret = szTime;

	return ret;	
}




void SessionInfo::reset()
{
	m_dwMediaEndTime = 0;
	m_dwMediaStartTime = 0;

	m_lCallEndTime = 0;
	m_lCallStartTime = 0;

	mCallType = call_out;
	mState = false;

	mCaller = L"";
	mCallee = L"";
	mRemoteUserName = L"";
}