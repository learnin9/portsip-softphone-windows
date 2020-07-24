#pragma once

#include "PortGo.h"
#include "Main/MsgHistoryWnd.h"
#pragma execution_character_set("utf-8")
#define OLE_ALONE_MSG			1
#define GETPROP_VEDIO_HWND	_T("GETPROP_VEDIO_HWND")
#define GETPROP_OLETYPE		_T("GETPROP_OLETYPE")
#define GETPROP_CONTACT		_T("GETPROP_CONTACT")
#define GETPROP_CONTACT_FACE _T("GETPROP_CONTACT_FACE")
#define GETPROP_PAGE		_T("GETPROP_PAGE")
#define GETPROP_VOICE_MAIL  _T("GETPROP_VOICE_MAIL")
#define GETPROP_RECORD_FILE _T("GETPROP_RECORD_FILE")
#define GETPROP_UNIQUE		_T("GETPROP_UNIQUE")
#define GETPROP_FILE_INFO   _T("GETPROP_FILE_INFO")
#define GETPROP_RECORD_ID   _T("GETPROP_RECORD_ID")
//#define GETPROP_EMOJI_FILENAME _T("GETPROP_EMOJI_FILENAME")

#define IDT_TRAY_FLICKER	USER_TIMER_MAXIMUM - 100
#define IDT_PLAY_TIME       USER_TIMER_MAXIMUM - 99
#define IDT_SESSION_TIMER   USER_TIMER_MAXIMUM - 98

struct EMOJI_INFO
{
	int nStartPos;
	int nEndPos;
	std::string strFind;
};
namespace PortGo
{
	extern CMainWnd *g_pMainFrame;
	extern CMsgHistoryWnd *g_pMsgHistoryWnd;
#define WM_ADDED_CONTACT		WM_CUSTOM + 1
#define WM_DELETED_CONTACT		WM_CUSTOM + 2

	// 
	// #define WM_NEW_FRIENDS_SAVE		WM_CUSTOM + 20
	//
	// 	
	// #define WM_SET_CONTACT			WM_CUSTOM + 21
	//
	// 	
	// #define WM_DBCLICK_CONTACT		WM_CUSTOM + 22
	//
	// 
	// #define WM_START_VIDEO			WM_CUSTOM + 23
	//
	// 	
	// #define WM_START_AUDIO			WM_CUSTOM + 24
	//
	// 	
	// #define WM_START_CHAT			WM_CUSTOM + 25
	//
	// 	
	// #define WM_SWITCH_TO_PROFILE	WM_CUSTOM + 26
	//
	// 
	// #define WM_SWITCH_TO_PHONE_KEYBOARD	WM_CUSTOM + 27
	//
	// 
	// #define WM_SHOW_CHAT_LIST		WM_CUSTOM + 28
	//
	//
	// #define WM_SHOW_CHAT_MESSAGE	WM_CUSTOM + 29
	//
	// 
	// #define WM_NEW_FRIEND			WM_CUSTOM + 30
	//
	//
	// #define WM_HANGUP				WM_CUSTOM + 31
	//
	// 
	// #define VHM_SWITCH_VIDEO		WM_CUSTOM + 32
	//
	// 
	// #define WM_LAYOUT				WM_CUSTOM + 33
	//
	//
	// #define WM_POPUP_VIDEO			WM_CUSTOM + 34
	//
	//
	// #define WM_NEED_DELETE_RECENT_NODE		WM_CUSTOM + 35
	//
	// 
	// #define WM_NEED_READD_RECENT_NODE		WM_CUSTOM + 36
	//
	// 
	// #define IUI_VIDEO_VOICE_HOST_NAME _T("IUI_VIDEO_VOICE_HOST_NAME")
	//
	// #ifndef WM_IDLEUPDATECMDUI
	// #define WM_IDLEUPDATECMDUI  0x0363  // wParam == bDisableIfNoHandler
	// #endif
//#define WM_SWITCH_TO_SINGLE_CONTACT     WM_CUSTOM  + 39
#define WM_SWITCH_TO_CONTACT			WM_CUSTOM + 40
#define WM_EXIT_FULL_SCREEN_VIDEO		WM_CUSTOM + 41
#define WM_SELECTED_EMOTICON			WM_CUSTOM + 42
#define WM_TRAYNOTIFY					WM_CUSTOM + 43
#define WM_TOGGLE_VIDEO					WM_CUSTOM + 44
#define WM_HOST_WINDOWPOSCHANGED		WM_CUSTOM + 45
	class CContact
	{
	public:
		CContact();
		virtual ~CContact();

	public:
		int SetUID(int nUID);
		int GetUID() const;

		int SetFace(LPCTSTR lpszImage);
		CIUIString GetFace() const;

		int SetUserName(LPCTSTR lpszUserName);
		CIUIString GetUserName() const;

		int SetSIPNumber(LPCTSTR lpszSIPNumber);
		CIUIString GetSIPNumber() const;

		int SetMobileNumber(LPCTSTR lpszMobileNumber);
		CIUIString GetMobileNumber() const;

		int SetWorkNumber(LPCTSTR lpszWorkNumber);
		CIUIString GetWorkNumber() const;

		int SetHomeNumber(LPCTSTR lpszHomeNumber);
		CIUIString GetHomeNumber() const;

		int SetEmail(LPCTSTR lpszEmail);
		CIUIString GetEmail() const;

		int SetAddress(LPCTSTR lpszAddress);
		CIUIString GetAddress() const;

		int SetRemark(LPCTSTR lpszRemark);
		CIUIString GetRemark() const;

		void SetUserData(DWORD_PTR dwData);
		DWORD_PTR GetUserData() const;

	protected:
		int m_nUserID; 
		CIUIString m_strFace; 
		CIUIString m_strUserName;
		CIUIString m_strSIPNumber;
		CIUIString m_strMobileNumber;
		CIUIString m_strWorkNumber;
		CIUIString m_strHomeNumber;
		CIUIString m_strEmail;
		CIUIString m_strAddress;
		CIUIString m_strRemark;
		EOnlineState m_eOnlineState;	
		DWORD_PTR m_dwUserData;
	};
	int FindContact(LPCTSTR lpszFilter, __out std::vector<CContactDB *> *pvContacts);
	HTREEITEM FindContactFromTree(HWLWND hTree, HTREEITEM hParentItem, int nUID);
	int SwitchToContact(TAG_RECENT *pContact, CHAT_TAB enumChatTab);
	int SwitchToContactNoBlock(TAG_RECENT *pContact, CHAT_TAB enumChatTab);
	CIUIString	FormatEmojiText(CIUIString strPureFileName);

	ITEM_POS     CompareItemPosByFirstItem(TreeView *pTreeView, HTREEITEM itemX, HTREEITEM itemY);
	/************************************************************************/
	/*                            pic and text                              */
	/************************************************************************/
	int InsertHistoryMessage(HWLWND hIMHistory, int cp, LPCTSTR lpszFace);
	int InsertInputImageOle(HWLWND hReInput, LPCTSTR lpszImage, int cp, HWLWND &hOleWnd);
	int InsertHistoryImageOle(HWLWND hIMHistory, BOOL bSelf, LPCTSTR lpszImagePath,LPCTSTR lpszImageName, HWLWND &hOleHwnd, wstring strContactName, int nMsgID, INSERT_HISTORY_MODE enumHistoryMode, LPCTSTR lpszFace, LPCTSTR lpszSenderID);
	/************************************************************************/
	/*                               file                                   */
	/************************************************************************/
	// cp: -1. add to text end
	int InsertInputFileOle(HWLWND hReInput, LPCTSTR lpszFile, LPCTSTR lpszFilePath, int cp, HWLWND &hOleWnd, wstring wstrSender=L"");
	int InsertHistoryFileOle(HWLWND hIMHistory, BOOL bSelf, LPCTSTR lpszFile, LPCTSTR lpszFilePath,HWLWND &hOleWnd,wstring strContactName,int nMsgID , INSERT_HISTORY_MODE enumHistoryMode, LPCTSTR lpszFace, LPCTSTR lpszSenderID);
	
	/************************************************************************/
	/*                                video                                  */
	/************************************************************************/
	int InsertInputVideoOle(HWLWND hReInput, LPCTSTR lpszVideo, LPCTSTR lpszFilePath, LPCTSTR lpszTotalPlayTime, HWLWND &hOleControlPannel, wstring strContactName);
	int InsertHistoryVideoOle(HWLWND hIMHistory, BOOL bSelf, LPCTSTR lpszVideo,LPCTSTR lpszFilePath, LPCTSTR lpszTotalPlayTime ,HWLWND &hOleWnd,void **pVideoCanvasHandler, wstring strContactName, int nMsgID,INSERT_HISTORY_MODE enumHistoryMode, LPCTSTR lpszFace,LPCTSTR lpszSenderID);
	
	/************************************************************************/
	/*                                 audio                                 */
	/************************************************************************/
	int InsertInputAudioOle(HWLWND hReInput, LPCTSTR lpszAudio, LPCTSTR lpszFilePath, LPCTSTR lpszTotalPlayTime, HWLWND &hOleControlPannel, wstring strContactName);
	int InsertHistoryAudioOle(HWLWND hIMHistory, BOOL bSelf, LPCTSTR lpszAudio, LPCTSTR lpszFilePath, LPCTSTR lpszTotalPlayTime ,HWLWND &hOleControlPannel, wstring strContactName, int nMsgID, INSERT_HISTORY_MODE enumHistoryMode, LPCTSTR lpszFace,  LPCTSTR lpszSenderID);
	int PopupVideoWnd(HBITMAP pBitmap, HWND hParent, CIUIString strFilePath, CIUIString strTotalPlayTime, \
		CIUIString strFileName);
	void PopupLoginLoadWnd(HWND hParent);
	int ClosePopupLoginLoadWnd();
	HWND PopupNoPreviewVideoWnd(HWND hParent);
	int PopupInfoWnd(LPCTSTR lpszTitle, LPCTSTR lpszInfo, HWND hParent, WND_INFO_LEVEL enumWndInfoLevel,int nAutoCloseTime = 10000);
	int PopupTrayMsgWnd(HWND hParent);
	int PopupMsgHistoryWnd(CContactDB		*pContact, HWND hParent);
	int LayoutMsgHistoryWnd();
	int FlickerTrayIcon(HICON hIcon,bool bEnable);
	void ProcessShake();

	static unsigned int _ShakeThread(void* pParam);
	void ShakeWnd(int nShakeWndNum, CMainWnd* pShakeWnd );
	
	void FormatItemContact(CIUIString &strName, CIUIString &strHeadImage, CIUIString &strNumber, CContactDB *pContact);
	enum GROUP_FACE_PART
	{
		GFP_FULL = 0,		// 
		GFP_LEFT = 1,		//
		GFP_TOP_LEFT,		//
		GFP_TOP_RIGHT,		//
		GFP_RIGHT,			// 
		GFP_LEFT_BOTTOM,	// 
		GFP_RIGHT_BOTTOM,	//
		GFP_BOTTOM			// 
	};
	int DrawFace(
		HDC hDC,
		LPCRECT lprcTarget,
		Gdiplus::TextureBrush *ptBrush);
	int DrawFace(
		HDC hDC,
		LPCRECT lprcTarget,
		LPCTSTR lpszFace);
	int DrawGroupFace2(
		HDC hDC,
		LPCRECT lprcTarget,
		LPCTSTR lpszFace1,
		LPCTSTR lpszFace2,
		LPCTSTR lpszName1,
		LPCTSTR lpszName2);
	int DrawGroupFace3(
		HDC hDC,
		LPCRECT lprcTarget,
		LPCTSTR lpszFace1,
		LPCTSTR lpszFace2,
		LPCTSTR lpszFace3,
		LPCTSTR lpszName1,
		LPCTSTR lpszName2,
		LPCTSTR lpszName3);
	int DrawGroupFace4(
		HDC hDC,
		LPCRECT lprcTarget,
		LPCTSTR lpszFace1,
		LPCTSTR lpszFace2,
		LPCTSTR lpszFace3,
		LPCTSTR lpszFace4,
		LPCTSTR lpszName1,
		LPCTSTR lpszName2,
		LPCTSTR lpszName3,
		LPCTSTR lpszName4);
	int DrawGroupFace(HDC hDC, LPCRECT lprcTarget, TAG_RECENT *pContact);
	extern HWND g_hMainFrame;
	extern bool g_bIsShaking;
}
