/*
    PortSIP VoIP SDK
    Copyright (C) 2008 PortSIP Solutions, Inc.
   
    support@portsip.com

    Visit us at http://www.portsip.com
*/



#ifndef PANGOLIN_SESSION_hxx
#define PANGOLIN_SESSION_hxx


#include "atltime.h"
class SessionInfo
{
public:
	SessionInfo();
	virtual ~SessionInfo();

	typedef enum{ call_in = 1, call_out, call_in_miss, call_out_miss} CALL_TYPE;

public:

	void startCall();
	void endCall();

	CString getCallStartTime();
	CString getCallEndTime();
	CString convertCallTime(bool Start);



	void startMediaStream();
	void endMediaStream();
	CString getMediaSessionTime();

	CString converMediaSessionTime();

	CALL_TYPE getCallType() { return mCallType; }
	void setCallType(CALL_TYPE type) { mCallType = type; }


	CString  getCallee() { return mCallee; }
	void setCallee(CString callee) { mCallee = callee; }


	CString getCaller() { return mCaller; }
	void setCaller(CString caller) { mCaller = caller; }

	bool getState() { return mState; }
	void setState(bool State) { mState = State; }


	void reset();

protected:

private:

	DWORD m_dwMediaStartTime;
	DWORD m_dwMediaEndTime;

	CTime m_lCallStartTime;
	CTime m_lCallEndTime;

	CString mCaller;
	CString mCallee;
	CString mRemoteUserName;

	CALL_TYPE mCallType;
	bool mState;

};


class Session
{
public:
	Session();
	virtual ~Session();



public:
	bool getCallState();
	void setCallState(bool state);

	bool isReferCall() { return mIsReferCall; }
	long getOriginCallSessionId() { return mOriginCallSessionId; }
	void setReferCall(bool referCall, long originCallSessionId)
	{
		mIsReferCall = referCall;
		mOriginCallSessionId = originCallSessionId;
	}

	bool getRecvCallState();
	void setRecvCallState(bool state);

	void setPhoneNumber(const CString & number);
	CString getPhoneNumber();

	void setTextLine1(const CString & text);
	void setTextLine2(const CString & text);
	void setTextLine3(const CString & text);

	CString getTextLine1();
	CString getTextLine2();
	CString getTextLine3();

	long getSessionId();
	void setSessionId(long id);

	void reset();


	bool hasVideo() { return mHasVideo; }
	void setHasVideo(bool video) { mHasVideo = video; }


	bool getHoldState();
	void setHoldState(bool state);

	bool getConferenceState();
	void setConferenceState(bool state);


	void resetCallTimer();
	CString refreshCallTimer();
	
	void setSendVideoState(bool state);
	bool getSendVideoState();

	void setPrimaryKey(int nIndex) { m_nID = nIndex; }
	int  getPrimaryKey() { return m_nID; }

public:

	SessionInfo m_oSessionInfo;


protected:


private:


private:
	bool mIsReferCall;
	long mOriginCallSessionId;


	LONG mSessionId;
	bool mHasVideo;

	CString mLineText1;
	CString mLineText2;
	CString mLineText3;

	bool mCallState;
	bool mRecvCallState;

	bool mConference;
	bool mHoldState;

	CString mPhoneNumber;

	int mSecond1;
	int mSecond2;

	int mMinute1;
	int mMinute2;

	int mHour1;
	int mHour2;

	bool mSendVideoState;
	//数据库中主键ID
	int m_nID;
public:
	//是否开启录音
	bool m_bRecord;
	//录音文件路径
	std::string  m_strFileName;
	std::string  m_strFilePath;
};



#endif
