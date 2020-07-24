#include "StdAfx.h"
#include "AVPlayer.h"
#include "System/SystemEx.h"
CAVPlayer* gAVPlayer = NULL;
int        gTimerHandle = 0;
CAVPlayer::CAVPlayer(void)    
{
   
	gAVPlayer = this;
	m_hPlay = NULL;
	m_bInit = false;
	m_hPlayWnd = NULL;
	m_hProcessMsgWnd = NULL;
	m_bTruePlayMode = false;
	m_bSendMSG = false;
	Clear();
	m_strFilePath = "";
}

CAVPlayer::~CAVPlayer(void)
{
    Close();
	libvlc_release(m_pLibVLC_Instance);
}

void    CAVPlayer::Clear()
{
	m_enumPlayerState = STATE_PREPARE;
	m_hScreen = NULL;
	m_pLibVLC_Instance = NULL;
	m_pLibVLC_m = NULL;
	m_pLibVLC_mp = NULL;

	if (m_hPlay)
		m_hPlay->SetCheck(false);
	m_hPlay = NULL;
	
	m_bInit = false;
}

void    CAVPlayer::Close()
{ 
    Clear();
}

bool    CAVPlayer::Init( ENUM_PLAYER_TYPE enumPlayerType)
{
	if (m_bInit)
	{
		LOG4_INFO("Inited error /in player");
		Stop();
		//
		//delete m_pLibVLC_Instance;
	}
	m_pLibVLC_Instance = libvlc_new(0, NULL);
	m_enumPlayerType = enumPlayerType;
	m_bInit = true;
    return true;
}

bool    CAVPlayer::SetHwnd(HWND ulHwndDisplay,HWND hProcessMsgWnd)
{
	m_hProcessMsgWnd = hProcessMsgWnd;
	if(m_enumPlayerType==ENUM_AUDIO)
		return true;
    if(ulHwndDisplay)
    {
		m_hScreen =(HWND) ulHwndDisplay;
        return  true;
    }
    return true;
}
bool CAVPlayer::GetIsInit()
{
	return m_bInit;
}

Slider* CAVPlayer::GetBindSliderHandle()
{
	return m_pSlider;
}
Button* CAVPlayer::GetBindPlay()
{
	return m_hPlay;
}
HWND  CAVPlayer::GetPlayHwnd()
{
	return m_hPlayWnd;
}
HWLWND CAVPlayer::GetPlayWLHwnd()
{
	return m_hWlProcesMsgWnd;
}
bool     CAVPlayer::PlayByHwnd(std::string strFilePath, HWND hWnd)
{
	m_strFilePath = strFilePath;
	m_hPlayWnd = hWnd;
	m_bTruePlayMode = true;
	if (strFilePath.empty()||hWnd==NULL)
	{
		CIUIString strInfo;
		strInfo.Format(L"%s", L"Play The File Error,File Path Is empty ");
		CSystemEx::GetInstance()->GetMainDlg()->CreatInfoWnd(L"ERROR", strInfo, NULL);
		return false;
	}
	//std::wstring  strFileTemp = PortUtility::string2WString(strFilePath);
	std::string strURL = strFilePath;// PortUtility::Unicode2Utf8_ND(strFileTemp);
	const char *chaURL = strURL.c_str();

	if (strcmp(chaURL, "") == 0) {
		LOG4_INFO("ERROR PLAY URL");
		return false;
	}

	if (m_enumPlayerState == STATE_PLAY) {
		Stop();
		Init(m_enumPlayerType);
	}
	if (gTimerHandle != 0)
	{
		::KillTimer(NULL, gTimerHandle);
		CIUIString strCreateTimer;
		strCreateTimer.Format(L"%s%d\n", L"delete timer ID :", gTimerHandle);
		OutputDebugString(strCreateTimer);
		gTimerHandle = 0;
	}

	m_pLibVLC_m = libvlc_media_new_path(m_pLibVLC_Instance, chaURL);
	m_pLibVLC_mp = libvlc_media_player_new_from_media(m_pLibVLC_m);
	libvlc_media_release(m_pLibVLC_m);
	if (m_hScreen != NULL)
	{
		libvlc_media_player_set_hwnd(m_pLibVLC_mp, m_hScreen);
	}
	int  nRet = libvlc_media_player_play(m_pLibVLC_mp);
	if (nRet == 0)
	{
		m_enumPlayerState = STATE_PLAY;
		gTimerHandle = ::SetTimer(NULL, IDT_PLAY_TIME, 500, TimerProcPlay);
		CIUIString strCreateTimer;
		strCreateTimer.Format(L"%s%d\n", L"create timer ID :", gTimerHandle);
		OutputDebugString(strCreateTimer);
		return true;
	}
	else
	{
	}
	
	return false;
}
bool     CAVPlayer::PlayByWLWnd(std::string strFilePath, IUI::HWLWND  hWnd, int nVolumeSet, int nPosSet)
{
	m_strFilePath = strFilePath;
	m_hWlProcesMsgWnd = hWnd;
	m_bTruePlayMode = false;
	if (strFilePath.empty() || hWnd == NULL)
	{
		CIUIString strInfo;
		strInfo.Format(L"%s", L"Play The File Error,File Path Is empty ");
		CSystemEx::GetInstance()->GetMainDlg()->CreatInfoWnd(L"ERROR", strInfo, NULL);
		return false;
	}
	std::wstring  strFileTemp = PortUtility::string2WString(strFilePath);
	std::string strURL = PortUtility::Unicode2Utf8_ND(strFileTemp);
	const char *chaURL = strURL.c_str();

	if (strcmp(chaURL, "") == 0) {
		LOG4_INFO("ERROR PLAY URL");
		return false;
	}

	if (m_enumPlayerState == STATE_PLAY) {
		Stop();
		Init(m_enumPlayerType);
	}
	if (gTimerHandle != 0)
	{
		::KillTimer(NULL, gTimerHandle);
		CIUIString strCreateTimer;
		strCreateTimer.Format(L"%s%d\n", L"delete timer ID :", gTimerHandle);
		OutputDebugString(strCreateTimer);
		gTimerHandle = 0;
	}

	m_pLibVLC_m = libvlc_media_new_path(m_pLibVLC_Instance, chaURL);
	m_pLibVLC_mp = libvlc_media_player_new_from_media(m_pLibVLC_m);
	libvlc_media_release(m_pLibVLC_m);
	if (m_hScreen != NULL)
	{
		libvlc_media_player_set_hwnd(m_pLibVLC_mp, m_hScreen);
	}
	VolumeSet(nVolumeSet);
	int  nRet = libvlc_media_player_play(m_pLibVLC_mp);
	if (nRet == 0)
	{
		m_enumPlayerState = STATE_PLAY;
		gTimerHandle = ::SetTimer(NULL, IDT_PLAY_TIME, 500, TimerProcPlay);
		CIUIString strCreateTimer;
		strCreateTimer.Format(L"%s%d\n", L"create timer ID :", gTimerHandle);
		OutputDebugString(strCreateTimer);
		return true;
	}
	else
	{
	}

	return false;
}
void CALLBACK CAVPlayer::TimerProcPlay(HWND hwnd, UINT uMsg, UINT idEvent, DWORD dwTime)
{
	if (gTimerHandle == idEvent)
	{//
		if (gAVPlayer!=NULL&&  gAVPlayer->m_pLibVLC_mp != NULL)
		{
			//CSystemEx::GetInstance()->ProcessTimer(idEvent, uMsg);
			gAVPlayer->RefreshProgress();
		}

		if (gAVPlayer != NULL)
		{
			if (gAVPlayer->m_pLibVLC_mp == NULL)
			{
				gAVPlayer->Stop();
				return;
			}
			//Stop in the end
			if (libvlc_media_player_get_state(gAVPlayer->m_pLibVLC_mp) == libvlc_Ended)
			{
				//CIUIString strInfo;
				//strInfo.Format(L"%s", L"Play The File Error ");
				//CSystemEx::GetInstance()->GetMainDlg()->CreatInfoWnd(L"ERROR", strInfo,NULL);
				gAVPlayer->Stop();
			}	
		}
	}
	else
	{
		
	}
	
}

CIUIString CAVPlayer::GetTemplateTime(std::string strFilePath,int & nSecond)
{
	libvlc_instance_t * pLocalInstance =NULL;
	int curtime;
	int duration;
	int tns, thh, tmm, tss;
	CIUIString   durationstr = L"";;
	std::wstring  strFileTemp = PortUtility::string2WString(strFilePath);
	std::string strURL = PortUtility::Unicode2Utf8_ND(strFileTemp);
	const char *chaURL = strURL.c_str();

	libvlc_media_player_t *	pLibVLCMediaPlayer;
	libvlc_media_t *		pLibVLCMedia;
	const char * const vlc_args[] = {
	"--no-audio","--no-video"
	};


	pLocalInstance = libvlc_new(sizeof(vlc_args) / sizeof(vlc_args[0]), vlc_args);
	pLibVLCMedia = libvlc_media_new_path(pLocalInstance, chaURL);
	pLibVLCMediaPlayer = libvlc_media_player_new_from_media(pLibVLCMedia);
	libvlc_media_release(pLibVLCMedia);
	int nRet = libvlc_audio_set_volume(pLibVLCMediaPlayer, 0);
	//libvlc_media_player_set_hwnd(pLibVLCMediaPlayer, NULL);
	nRet = libvlc_media_player_play(pLibVLCMediaPlayer);
	//libvlc_media_player_stop(pLibVLCMediaPlayer);
	//libvlc_media_parse(pLibVLCMedia);
	//libvlc_media_release(pLibVLCMedia);
	/************************************************************************/
	/* libvlc_media_player_get_length()£¬libvlc_video_get_width()£¬libvlc_video_get_height()
	needs sleep ,may be the lib play need a little time
	*/
	/************************************************************************/
	Sleep(1000);
	if (/*nRet == 0*/pLibVLCMediaPlayer!=NULL)
	{
		duration = libvlc_media_player_get_length(pLibVLCMediaPlayer);
		if (duration != 0)
		{
			tns = duration / 1000;
			nSecond = tns;
			thh = tns / 3600;
		
			tmm = (tns % 3600) / 60;
			tss = (tns % 60);
			if (thh > 0)
			{
				durationstr.Format(_T("%02d:%02d:%02d"), thh, tmm, tss);
			}
			else
			{
				durationstr.Format(_T("%02d:%02d"), tmm, tss);
			}
		}
		else
		{
			nSecond = 0;
			durationstr.Format(L"00:00");
		}

		if (pLibVLCMediaPlayer != NULL) {
			libvlc_media_player_release(pLibVLCMediaPlayer);
		}
	}
	libvlc_release(pLocalInstance);
	return durationstr;
}

void   CAVPlayer::RefreshProgress()
{
	CIUIString  curtimestr, durationstr;
	int curtime;
	int duration;
	int tns, thh, tmm, tss;
	int progress;
	CIUIString temlateString;
	//ms
	curtime = libvlc_media_player_get_time(m_pLibVLC_mp);
	if (curtime != 0) {
		//change to second
		tns = curtime / 1000;
		thh = tns / 3600;
		tmm = (tns % 3600) / 60;
		tss = (tns % 60);
		curtimestr.Format(_T("%02d:%02d"), tmm, tss);
	}
	duration = libvlc_media_player_get_length(m_pLibVLC_mp);
	if (duration != 0) {
		tns = duration / 1000;
		thh = tns / 3600;
		tmm = (tns % 3600) / 60;
		tss = (tns % 60);
		durationstr.Format(_T("%02d:%02d"), tmm, tss);
		if (curtimestr.Compare(_T("00:04"))==0)
		{
			printf("");
		}
		progress = int (float((float)curtime * 100 / (float)duration));
		temlateString = curtimestr + L"/" + durationstr;

		SendMSGPosChange(progress);
		SendMSGTimeChange(temlateString.GetBuffer(temlateString.GetLength()));
	}
}
bool    CAVPlayer::PlayNext()
{

    return  true;
}

bool    CAVPlayer::PlayPre()
{

    return  true;
}
void CAVPlayer::SendMSGPlayEnd(int progress, CIUIString temlateString)
{
	
	if (m_bTruePlayMode)
	{
		if (m_hProcessMsgWnd != NULL)
		{
			if (IsWindow(m_hProcessMsgWnd))
			{
				::SendMessage(m_hProcessMsgWnd, WM_PLAY_END, progress, (LPARAM)temlateString.GetBuffer(temlateString.GetLength()));
			}
		}
	}
	else
	{
		if (m_hWlProcesMsgWnd!=NULL)
		{
			if (IUI::IsWindow(m_hWlProcesMsgWnd))
			{
				IUI::SendMessage(m_hWlProcesMsgWnd, WM_PLAY_END, progress, (LPARAM)temlateString.GetBuffer(temlateString.GetLength()));
			}
		}
	}
}

void CAVPlayer::SendMSGTimeChange(LPWSTR lpTimeInfo)
{
	if (m_bTruePlayMode)
	{
		if (m_hProcessMsgWnd != NULL)
		{
			if (IsWindow(m_hProcessMsgWnd))
			{
				::SendMessage(m_hProcessMsgWnd, WM_PLAY_TIME_CHANGED, (WPARAM)lpTimeInfo, 0);
			}
		}
	}
	else
	{
		if (m_hWlProcesMsgWnd != NULL)
		{
			if (IUI::IsWindow(m_hWlProcesMsgWnd))
			{
				IUI::SendMessage(m_hWlProcesMsgWnd, WM_PLAY_TIME_CHANGED, (WPARAM)lpTimeInfo, 0);
			}
		}
	}
}
void CAVPlayer::SendMSGPosChange(int nPos)
{

	if (m_bTruePlayMode)
	{
		if (m_hProcessMsgWnd != NULL)
		{
			if (IsWindow(m_hProcessMsgWnd))
			{
				::SendMessage(m_hProcessMsgWnd, WM_PLAY_POS_CHANGED, nPos, 0);
			}
		}
	}
	else
	{
		if (m_hWlProcesMsgWnd != NULL)
		{
			if (IUI::IsWindow(m_hWlProcesMsgWnd))
			{
				IUI::SendMessage(m_hWlProcesMsgWnd, WM_PLAY_POS_CHANGED, nPos, 0);
			}
		}
	}
}
void    CAVPlayer::Stop(bool bSendMsg)
{
	if (m_pLibVLC_mp != NULL) {
		CIUIString  curtimestr, durationstr;
		int curtime;
		int duration;
		int tns, thh, tmm, tss;
		int progress;
		CIUIString temlateString;
		//ms
		duration = libvlc_media_player_get_length(m_pLibVLC_mp);
		if (duration != 0) {
			tns = duration / 1000;
			thh = tns / 3600;
			tmm = (tns % 3600) / 60;
			tss = (tns % 60);
			durationstr.Format(_T("%02d:%02d"), tmm, tss);
			progress = int(float((float)duration * 100 / (float)duration));

			temlateString = durationstr + L"/" + durationstr;

			CIUIString strPos;
			strPos.Format(L"progess:%d time:%s\n", progress, temlateString);
			OutputDebugString(strPos);
		}
		else
		{
			progress = 100;
			temlateString = L"";
		}
		if (bSendMsg)
		{
			SendMSGPlayEnd(progress, temlateString);
		}
		

		libvlc_media_player_stop(m_pLibVLC_mp);
		libvlc_media_player_release(m_pLibVLC_mp);
	}
	if (gTimerHandle != 0)
	{
		::KillTimer(NULL, gTimerHandle);
		CIUIString strCreateTimer;
		strCreateTimer.Format(L"%s%d\n", L"delete timer ID :", gTimerHandle);
		OutputDebugString(strCreateTimer);
		gTimerHandle = 0;
	}
	Clear();
	
	//Close();
}
bool CAVPlayer::IsPlay()
{
	bool bPlay = false;
	if (m_enumPlayerState == STATE_PLAY|| m_enumPlayerState == STATE_PAUSE)
	{
		bPlay = true;
	}
	return bPlay;
}
void    CAVPlayer::Pause()
{
	if (m_enumPlayerState == STATE_PLAY) {
		libvlc_media_player_set_pause(m_pLibVLC_mp, 1);
		m_enumPlayerState = STATE_PAUSE;
	}
	else if (m_enumPlayerState == STATE_PAUSE) {
		libvlc_media_player_set_pause(m_pLibVLC_mp, 0);
		m_enumPlayerState = STATE_PLAY;
	}
}


void    CAVPlayer::OnlyPause()
{
	if (m_enumPlayerState == STATE_PLAY)
	{
		libvlc_media_player_set_pause(m_pLibVLC_mp, 1);
		m_enumPlayerState = STATE_PAUSE;
	}
}
void    CAVPlayer::OnContinue()
{
	if (m_enumPlayerState == STATE_PAUSE)
	{
		libvlc_media_player_set_pause(m_pLibVLC_mp, 0);
		m_enumPlayerState = STATE_PLAY;
	}
}
void   CAVPlayer::MuteVol(bool bMute)
{
	if (IsPlay())
	{
		if (m_pLibVLC_mp!=NULL)
		{
			libvlc_audio_set_mute(m_pLibVLC_mp, (int)bMute);
		}
	}
}
bool    CAVPlayer::VolumeSet(size_t nVol)
{
	bool bRet = false;
	
	if (nVol>100)
	{
		nVol = 100;
	}
	if (nVol<0)
	{
		nVol = 0;
	}
	if (m_pLibVLC_mp != NULL)
	{
		int nRet = libvlc_audio_set_volume(m_pLibVLC_mp, nVol);
		if (nRet!=0)
		{
			bRet = false;
		}
		else
		{
			bRet = true;
		}
	}
    return  bRet;
}

int     CAVPlayer::GetVolume()
{
	int nVolume = 0;
	if (m_pLibVLC_mp!=NULL)
	{
		nVolume = libvlc_audio_get_volume(m_pLibVLC_mp);
	}
	return nVolume;
}
bool    CAVPlayer::VolumeIncrease()
{

    return  true;
}

bool    CAVPlayer::VolumeReduce()
{

    return  true;
}

bool    CAVPlayer::SeekTo(size_t nPos)
{

    return  true;
}

bool    CAVPlayer::Forward()
{

    return  true;
}

bool    CAVPlayer::Backward()
{

    return  true;
}

void CAVPlayer::StreamSeek(int nPos)
{
	if (nPos<0||nPos>100)
	{
		return;
	}
	float posf = (float)nPos / 100.0;
	if (m_pLibVLC_mp==NULL)
	{
		return;
	}
	libvlc_media_player_set_position(m_pLibVLC_mp, posf);
}