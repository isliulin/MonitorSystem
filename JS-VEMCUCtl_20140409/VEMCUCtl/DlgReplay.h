#pragma once

#include "afxcmn.h"
#include "afxwin.h"
#include "SkinButton.h"
#include "DlgViewOut.h"
#include "BitmapSlider.h"
#include "WeatherDataWinShowInfo.h"
#include "DlgStationSelect.h"
#include "include/ListCtrlCl.h"
//////////////////////////////////////////////////////////////////////////
//回放RTSP状态
#define  REPLAY_RTSP_STATE_INIT                  0
#define  REPLAY_RTSP_STATE_OPTIONS         1
#define  REPLAY_RTSP_STATE_DESCRIBE        2
#define  REPLAY_RTSP_STATE_SETUP              3
#define  REPLAY_RTSP_STATE_PLAY                4
#define  REPLAY_RTSP_STATE_PAUSE             5
#define  REPLAY_RTSP_STATE_TEARDOWN    6

//////////////////////////////////////////////////////////////////////////
typedef struct _t_video_replay_download_info_
{
	BOOL bDownloadFlag;
	int     nDownloadType;
	char   szCameraName[256];
	char   szCameraCode[64];
	char   szStationName[256];
	char   szStationCode[64];
	char   szReplayStartTime[64];
	char   szReplayStopTime[64];
	char   szLinkageTime[64];
	char   szWeatherTime[64];
	char   szSubType[64];
	char   szVideoFileName[256];
	char   szInfoFileName[256];
	HANDLE hVideoSaveFile;
	HANDLE hInfoSaveFile;
	int	  nRtspID;
	int 	  nRecvBytes;
	int 	  nTotalBytes;
	int	  nDecodeTag;
	char   newSps[512];
	int      newSpsLen;
	T_HISTORY_WEATHER_DATA_WIN_INFO  weather_data_info;//气象信息
}VIDEO_REPLAY_DOWNLOAD_INFO;

//////////////////////////////////////////////////////////////////////////

// CDlgReplay 对话框
class CDlgReplay : public CDialog
{
	DECLARE_DYNAMIC(CDlgReplay)

public:
	CDlgReplay(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgReplay();

public:
	BOOL LoadSkin();
	BOOL InitControl();
	int StartRtspStack();// 启动rtsp库
	static void RecvRTP(int nid, void *data, int datalen, void *context);//接收播放数据
	static void SendRecvMsg(int msgtype, char* msg, int msglen, void* context);//发送和接收到RTSP信息

	bool BrowseFile(CString *strFileName);//打开文件查找
	void InitViewChannel();// 初始化窗口

	BOOL PlayRemoteVideo();
	BOOL StartPlayRemoteVideo();
	BOOL StopPlayRemoteVideo();
	BOOL PausePlayRemoteVideo();
	BOOL ResumePlayRemoteVideo();
	BOOL FastPlayRemoteVideo();
	BOOL SlowPlayRemoteVideo();
	BOOL PlaySetRemoteVideoPos(int nPos);

	//////////////////////////////////////////////////////////////////////////
	//通过平台回放视频
	BOOL PlayRemoteVideoByVideoPlatform();
	BOOL StartPlayRemoteVideoByVideoPlatform();
	BOOL StopPlayRemoteVideoByVideoPlatform();
	BOOL PausePlayRemoteVideoByVideoPlatform();
	BOOL ResumePlayRemoteVideoByVideoPlatform();
	BOOL FastPlayRemoteVideoByVideoPlatform();
	BOOL SlowPlayRemoteVideoByVideoPlatform();
	BOOL PlaySetRemoteVideoPosByVideoPlatform(int nPos);
	int      SwitchRemoteVideoTimeFromPosByVideoPlatform(int nPos,char *szStartTime,char *szStopTime);

	//通过直连前端设备回放视频
	BOOL PlayRemoteVideoByDirectDevice();
	BOOL StartPlayRemoteVideoByDirectDevice();
	BOOL StopPlayRemoteVideoByDirectDevice();
	BOOL PausePlayRemoteVideoByDirectDevice();
	BOOL ResumePlayRemoteVideoByDirectDevice();
	BOOL FastPlayRemoteVideoByDirectDevice();
	BOOL SlowPlayRemoteVideoByDirectDevice();
	BOOL PlaySetRemoteVideoPosByDirectDevice(int nPos);

	//////////////////////////////////////////////////////////////////////////
	BOOL VideoReplayDownloadByVideoPlatform();
	BOOL StartVideoReplayDownloadByVideoPlatform();
	BOOL StopVideoReplayDownloadByVideoPlatform();
	BOOL SaveVideoFileDataByVideoPlatform(void *pData,int nDataLen);
	BOOL GetVideoCameraCodeFromRtspUrlByVideoPlatform(char *szRtspUrl,char *szCameraCode);

	//////////////////////////////////////////////////////////////////////////
	BOOL VideoReplayDownloadByDirectDevice();
	BOOL StartVideoReplayDownloadByDirectDevice();
	BOOL StopVideoReplayDownloadByDirectDevice();

	//////////////////////////////////////////////////////////////////////////
	BOOL PlayLocalORRemoteVideo();
	BOOL PlayLocalFile();
	BOOL StartPlayLocalFile();
	BOOL StopPlayLocalFile();
	BOOL PausePlayLocalFile();
	BOOL ResumePlayLocalFile();
	BOOL FastPlayLocalFile();
	BOOL SlowPlayLocalFile();
	BOOL PlaySetLocalFilePos(float fPos);
	BOOL ModifyVideoStatus();

	//////////////////////////////////////////////////////////////////////////
	BOOL PlayLocalFileByVideoPlatform();
	BOOL StartPlayLocalFileByVideoPlatform();
	BOOL StopPlayLocalFileByVideoPlatform();
	BOOL PausePlayLocalFileByVideoPlatform();
	BOOL ResumePlayLocalFileByVideoPlatform();
	BOOL FastPlayLocalFileByVideoPlatform();
	BOOL SlowPlayLocalFileByVideoPlatform();
	BOOL PlaySetLocalFilePosByVideoPlatform(float fPos);
	BOOL ModifyVideoStatusByVideoPlatform();

	//////////////////////////////////////////////////////////////////////////
	BOOL PlayLocalFileByDirectDevice();
	BOOL StartPlayLocalFileByDirectDevice();
	BOOL StopPlayLocalFileByDirectDevice();
	BOOL PausePlayLocalFileByDirectDevice();
	BOOL ResumePlayLocalFileByDirectDevice();
	BOOL FastPlayLocalFileByDirectDevice();
	BOOL SlowPlayLocalFileByDirectDevice();
	BOOL PlaySetLocalFilePosByDirectDevice(float fPos);
	BOOL ModifyVideoStatusByDirectDevice();

	//////////////////////////////////////////////////////////////////////////
	BOOL SearchVideoPlatformHistoryVideo();
	BOOL SearchVideoPlatformHistoryVideoByVideoPlatform();
	BOOL SearchVideoPlatformHistoryVideoByDirectDevice();
	BOOL SearchD5000LinkageHistoryVideo();
	BOOL SearchD5000LinkageHistoryVideoByVideoPlatform();
	BOOL SearchD5000LinkageHistoryVideoByDirectDevice();
	BOOL SearchWeatherLinkageHistoryVideo();
	BOOL SearchWeatherLinkageHistoryVideoByVideoPlatform();
	BOOL SearchWeatherLinkageHistoryVideoByDirectDevice();

	BOOL ReadAndSetListInfo(char *szStation,char *szDevice,char *szType,char *szStartLinkageTime,char *szStopLinkageTime);
	BOOL ReadAndSetListCameraInfo(int nIndex,int nNodeId,char *szLinkageTime);

	BOOL ReadAndSetWeatherListInfo(char *szType,char *szStartLinkageTime,char *szStopLinkageTime);
	BOOL ReadAndSetWeatherListCameraInfo(int nIndex,int nNodeId,char *szLinkageTime);

	BOOL GetAndSetVideoFileRtspInfo(int nIndex);//通过视频平台，得到并设置回放文件信息
	BOOL GetAndSetDeviceVideoFileInfo(int nIndex);//通过直连前端，得到并设置前端回放文件信息
	BOOL SetLinkTimeLinePos(int nIndex);

	int      GetWeatherStationIdFromStationName(char *szStationName);
	BOOL StartRemoteVideoReplayHistoryWeatherInfo();
	BOOL StopRemoteVideoReplayHistoryWeatherInfo();

public:
	int m_nStationId;
	CDlgStationSelect* m_pDlgStationSelect;
	CDlgViewOut	m_ch;// 视频回放窗口
	CListCtrl m_ListCtrl_ReplayFile;// 查询结果文件列表
	CListCtrl m_listReplayRelation;
	CBitmapSlider m_SliderPlayPos;// 播放进度条
	CComboBox m_ComboRecordFileType;
	CComboBox m_ComboD5000DevSel;
	CComboBox m_ComboD5000StationSel;
	CComboBox m_ComboD500EventType;
	CComboBox m_ComboWeatherType;
	CComboBox m_ComboVideoAccessType;

	CSkinButton m_btnQuery;
	CSkinButton m_btnPlay;
	CSkinButton m_btnStop;
	CSkinButton m_btnFast;
	CSkinButton m_btnSlow;

	CSkinButton m_btnSelPlant;
	CSkinButton m_btnSelD5000;
	CSkinButton m_btnSelOP;
	CSkinButton m_btnSelWeather;
	CSkinButton m_btnAlarmReplay;

	CSkinButton m_btnLocalReplay;
	CSkinButton m_btnNodeSelect;
	
	CBrush m_BgBrush;// 背景色画刷
	CBrush m_Brush;
	CBrush m_StaticBKBrush;

public:
	CTime m_StartDay;// 查询条件，开始时间
	CTime m_StartTime;// 查询条件，开始时间
	CTime m_StopDay;// 查询条件，结束日期
	CTime m_StopTime;// 查询条件，结束时间

	CString m_strPlayState;// 当前的播放状态
	CString m_strPlaySpeed;
	
	_T_REPLAY_QUERY_INFO	m_Replay_Query_Info;// 回放资源信息

	//DVR和Camera信息
	char m_szDvrIP[32];
	int    m_nDvrPort;
	int    m_nCameraChannel;
	int    m_nDvrId;
	int    m_nDvrType;
	int    m_nRecordFileSize;
	char m_szUserName[64];
	char m_szUserPassword[32];
	char m_szDvrRecordStartTime[32];
	char m_szDvrRecordStopTime[32];
	
	//////////////////////////////////////////////////////////////////////////
public:
	IRtspClientAPI*	m_pRtspClientAPI;// RTSP接口
	char	m_RtspURL[256];// RTSP URL
	char	m_RtspIP[30];// RTSP IP
	int m_NowSpeed;// 当前播放速度
	int m_RemotePlayState;// 播放状态
	bool m_bPlayRemoteFlag;// 播放本地还是远端视频，true表示远端，false表示本地，
	int m_LocalPlayState;// 播放本地状态
	_T_NODE_INFO *m_pNodeInfo;// 列表节点信息
	_T_NODE_INFO m_tStationNode;//选择的变电站

	int m_SelectedPlant;		    // 0:表示本地，1:表示所属
	int m_nSearchVideoAccessType; //查询方式,0:表示通过南瑞平台,1:表示直连前端设备
	int m_nPlayVideoAccessType; //回放方式,0:表示通过南瑞平台,1:表示直连前端设备
	int m_nSpecialVideoAccessFlag;//0:表示不采用,1:表示查询和播放都通过南瑞平台,2表示查询和播放都通过直连前端设备

	CString m_SelectedName;
	char  	m_szSelectedNum[32];
	char  	m_szSelectedType[20];

	int m_nRtspMsgState;
	int m_nTableSelectIndex;//1表示选择D5000,2表示选择气象,3表示选择操作票,4表示选择视频平台,5表示告警回放
	int m_nPlaySliderWidth;

	char m_szWeatherStationName[256];
	char m_szWeatherStartTime[64];
	char m_szWeatherStopTime[64];

public:
// 对话框数据
	enum { IDD = IDD_DIALOG_REPLAY };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnBnClickedButtonQuery();
	afx_msg void OnBnClickedButtonLocalfile();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnNMDblclkListReplayfile(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedButtonPlay();
	afx_msg void OnBnClickedButtonStop();
	afx_msg void OnBnClickedButtonFast();
	afx_msg void OnBnClickedButtonSlow();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnBnClickedRadioLocalPlant();
	afx_msg void OnBnClickedRadioRemotePlant();
	afx_msg void OnBnClickedButtonRpPlantSelect();
	afx_msg void OnBnClickedButtonRpD5000Select();
	afx_msg void OnBnClickedButtonRpWeatherSelect();
	afx_msg void OnBnClickedButtonRpOpSelect();
	afx_msg void OnClose();
	afx_msg void OnBnClickedButtonSelectid();
	afx_msg void OnCbnSelchangeComboD5000StationSel();
	afx_msg void OnBnClickedButtonLocalReplay();
	afx_msg void OnNMRClickListReplayfile(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnMenuVideoReplayDownload();
	afx_msg void OnMenuVideoReplayPlay();
	LRESULT OnSelectDeviceNotifyMessageHandler(WPARAM wParam, LPARAM lParam);
	LRESULT OnSliderLButtonDownHandler(WPARAM wParam,LPARAM lParam);
	afx_msg void OnBnClickedButtonAlarmReplay();
	afx_msg void OnBnClickedButtonAlarmSelect();
	void ShowAlarmReplayControl(BOOL bFlag);
	void SearchAlarmReplayHistoryVideo();
	void SearchAndAddToListControl(int nStationId, char* strDevName, char* szYear,char* szStartTime,char* szEndTime);
	afx_msg void OnNMCustomdrawListReplayfile(NMHDR *pNMHDR, LRESULT *pResult);
	void UpdateAlarmReplayDeviceCombox(int nStationId);
};

//////////////////////////////////////////////////////////////////////////
extern CDlgReplay	*g_pDlgReplay;
extern VIDEO_REPLAY_DOWNLOAD_INFO g_VideoReplayDownloadInfo;