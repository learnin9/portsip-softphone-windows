#pragma  once
#include "vlc/vlc.h"
#include "vlc/libvlc.h"

#define MAX_URL_LENGTH 500
/************************************************************************/
/*          VLC version  £º3.0.1									    */
/************************************************************************/

enum PlayerState {
	STATE_PREPARE,
	STATE_PLAY,
	STATE_PAUSE
};
#define NORMAL_VOLUME   100

class  CAVPlayer;
class  CAVPlayer
{
public:
    CAVPlayer(void);
    ~CAVPlayer(void);

    bool    Init(ENUM_PLAYER_TYPE enumPlayerType); 
	ENUM_PLAYER_TYPE GetPlayType() {
		return m_enumPlayerType;
	};

	PlayerState      GetPlayState() {
		return m_enumPlayerState;
	}
	HWND  GetPlayHwnd();
	HWLWND GetPlayWLHwnd();
	std::string GetPlayFilePath() { return m_strFilePath; }
    // bool    Play(std::string strFilePath);  
	bool    PlayByHwnd(std::string strFilePath, HWND hWnd);
	bool    PlayByWLWnd(std::string strFilePath, IUI::HWLWND hWnd,int nVolumeSet,int nPosSet);
    bool    PlayNext();                    
    bool    PlayPre();                      
    void    Stop (bool SendMsg = true);                        
    void    Pause();                       
	void    OnlyPause();					
	void    OnContinue();					
	bool    IsPlay();						
    bool    VolumeSet(size_t nVol);         
	void    MuteVol(bool bMute);						
	int     GetVolume();				
    bool    VolumeIncrease();               
    bool    VolumeReduce();               
    bool    SeekTo(size_t nPos);           
    bool    Forward();                      
    bool    Backward();                    
    void    Close();                        
    bool    SetHwnd(HWND ulHwndDisplay, HWND hProcessMsgWnd);                      
    void    StreamSeek( int nPos);                        
	CIUIString GetTemplateTime(std::string strFilePath,int& nSecond);   //cstring 00:00:00
	static void CALLBACK TimerProcPlay(HWND hwnd, UINT uMsg, UINT idEvent, DWORD dwTime);
	void  RefreshProgress();
	bool GetIsInit();
	Slider* GetBindSliderHandle();
	Button* GetBindPlay();
	void SetCallBack();
	void SendMSGPlayEnd(int progress, CIUIString temlateString);
	void SendMSGTimeChange(LPWSTR lpTimeInfo);
	void SendMSGPosChange(int nPos);
public:
	void    Clear();
	libvlc_instance_t *		m_pLibVLC_Instance;
	libvlc_media_player_t *	m_pLibVLC_mp;
	libvlc_media_t *		m_pLibVLC_m;
	PlayerState				m_enumPlayerState;
	ENUM_PLAYER_TYPE		m_enumPlayerType;
	HWND					m_hScreen;
	Slider *				m_pSlider;
	HWLWND					m_hStaDuration;
	Button*					m_hPlay;
	HWND					m_hPlayWnd;
	HWND					m_hProcessMsgWnd;
	HWLWND                  m_hWlProcesMsgWnd;
	bool                    m_bInit;
	bool					m_bTruePlayMode;
	bool                   m_bSendMSG;
	std::string			   m_strFilePath;
	//
};
