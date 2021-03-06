// DlgPageServer.cpp : 实现文件
//

#include "stdafx.h"
#include "VEMCUCtl.h"
#include "DlgPageServer.h"
#include "VEMCUCtlDlg.h"
#include "CameraVideoOperate.h"
#include "SipCall.h"
#include "DlgPageViewSelect.h"
#include "DlgCameraStatus.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////////
_T_CAMERA_INFO g_CameraDvrInfo[512];
int g_nCameraDvrInfoCount = 0;

//////////////////////////////////////////////////////////////////////////
// CDlgPageServer 对话框

IMPLEMENT_DYNAMIC(CDlgPageServer, CPropertyPage)

CDlgPageServer::CDlgPageServer()
: CPropertyPage(CDlgPageServer::IDD)
, m_strSearch(_T(""))
{	
	m_pDragImage = NULL;
	m_bDragging = FALSE;
	m_preCh = -1;
	m_hSelectPageItem = NULL;
	m_DlgPreSet = NULL;
	m_DlgVideoParSet = NULL;
	m_ytLock = FALSE;
	m_bOnlyShowAnFang = FALSE;

	memset(g_CameraDvrInfo,0,sizeof(g_CameraDvrInfo));
	g_nCameraDvrInfoCount = 0;
}

CDlgPageServer::~CDlgPageServer()
{
	if (m_DlgPreSet != NULL)
	{
		m_DlgPreSet->DestroyWindow();
		delete m_DlgPreSet;
		m_DlgPreSet = NULL;
	}

	if (m_DlgVideoParSet != NULL)
	{
		m_DlgVideoParSet->DestroyWindow();
		delete m_DlgVideoParSet;
		m_DlgVideoParSet = NULL;
	}
}

void CDlgPageServer::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_SPEED, m_Combo_YTSpeed);
	DDX_Control(pDX, IDC_TREE_SERVER, m_trServer);
	DDX_Control(pDX, IDC_TREE_PRESET, m_trPreset);
	DDX_Control(pDX, IDC_TREE_TUNECYCLE, m_trTuneCycle);
	DDX_Control(pDX, IDC_YT_UP, m_btnUp);
	DDX_Control(pDX, IDC_YT_DOWN, m_btnDown);
	DDX_Control(pDX, IDC_YT_RIGHT, m_btnRight);
	DDX_Control(pDX, IDC_YT_LEFT, m_btnLeft);
	DDX_Control(pDX, IDC_YT_LOCK, m_btnLock);
	DDX_Control(pDX, IDC_YT_ZOOMOUT, m_btn_ZOOMOUT);
	DDX_Control(pDX, IDC_YT_ZOOMIN, m_btn_ZOOMIN);
	DDX_Control(pDX, IDC_YT_CALL, m_btnCall);
	DDX_Control(pDX, IDC_YT_SAVE, m_btnSave);
	DDX_Control(pDX, IDC_YT_DIAPHRAGM_LARGE, m_btDiaphragmL);
	DDX_Control(pDX, IDC_YT_DIAPHRAGM_SMALL, m_btDiaphragmS);
	DDX_Control(pDX, IDC_YT_RESURVE1, m_btnResurve1);
	DDX_Control(pDX, IDC_YT_RESURVE2, m_btnResurve2);
	DDX_Control(pDX, IDC_BUTTON_CHOOSE_PRESET, m_btnChoosePreset);
	DDX_Control(pDX, IDC_BUTTON_CHOOSEYT, m_btnChooseYT);
	DDX_Control(pDX, IDC_BUTTON_CHOOSEVIEW, m_btnChooseView);
	DDX_Control(pDX, IDC_STATIC_SPEED, m_titlespeed);
	DDX_Control(pDX, IDC_BUTTON_L_OPEN, m_btnDGOpen);
	DDX_Control(pDX, IDC_BUTTON_L_CLOSE, m_btnDGClose);
	DDX_Control(pDX, IDC_BUTTON_YS_OPEN, m_btnYSOpen);
	DDX_Control(pDX, IDC_BUTTON_YS_CLOSE, m_btnYSClose);
	DDX_Control(pDX, IDC_BUTTON_HW_OPEN, m_btnHWOpen);
	DDX_Control(pDX, IDC_BUTTON_HW_CLOSE, m_btnHWClose);
	DDX_Control(pDX, IDC_BUTTON_JR_OPEN, m_btnJROpen);
	DDX_Control(pDX, IDC_BUTTON_JR_CLOSE, m_btnJRClose);
	DDX_Control(pDX, IDC_BUTTON_PS_DEVICELIST, m_btnListDevice);
	DDX_Control(pDX, IDC_BUTTON_PS_RELATIONLIST, m_btnListRelation);
	DDX_Control(pDX, IDC_BUTTON_PS_CAMERALIST, m_btnListCamera);
	DDX_Control(pDX, IDC_BUTTON_PS_CYCLE_LIST, m_btnListCycle);
	DDX_Control(pDX, IDC_BUTTON_PS_FORM1, m_Form1);
	DDX_Control(pDX, IDC_BUTTON_PS_FORM4, m_Form4);
	DDX_Control(pDX, IDC_BUTTON_PS_FORM6, m_Form6);
	DDX_Control(pDX, IDC_BUTTON_PS_FORM9, m_Form9);
	DDX_Control(pDX, IDC_BUTTON_PS_FORM16, m_Form16);
	DDX_Control(pDX, IDC_LIST_PS_RELATION, m_RealRelationList);
	DDX_Control(pDX, IDC_BUTTON_RELATION_SHOW, m_btnRelationLargeShow);
	DDX_Control(pDX, IDC_BTN_SHOW_HIDE, m_btnShowHide);
	DDX_Control(pDX, IDC_LIST_PS_CAMERA_RELATION, m_RealCameraRelationList);
	DDX_Text(pDX, IDC_EDIT_SEARCH, m_strSearch);
	DDV_MaxChars(pDX, m_strSearch, 64);
	DDX_Control(pDX, IDC_BUTTON_SEARCH, m_btnSearch);
}

BEGIN_MESSAGE_MAP(CDlgPageServer, CPropertyPage)
	ON_WM_ERASEBKGND()
	ON_WM_SIZE()
	ON_NOTIFY(NM_CLICK, IDC_TREE_SERVER, &CDlgPageServer::OnNMClickTreeServer)
	ON_NOTIFY(NM_DBLCLK, IDC_TREE_SERVER, &CDlgPageServer::OnNMDblclkTreeServer)
	ON_NOTIFY(NM_RCLICK, IDC_TREE_SERVER, &CDlgPageServer::OnNMRClickTreeServer)
	ON_NOTIFY(NM_DBLCLK, IDC_TREE_PRESET, &CDlgPageServer::OnNMDblclkTreePreset)
	ON_BN_CLICKED(IDC_BUTTON_CHOOSEYT, &CDlgPageServer::OnBnClickedButtonChooseyt)
	ON_BN_CLICKED(IDC_BUTTON_CHOOSEVIEW, &CDlgPageServer::OnBnClickedButtonChooseview)
	ON_BN_CLICKED(IDC_BUTTON_CHOOSE_PRESET, &CDlgPageServer::OnBnClickedButtonChoosePreset)
	ON_COMMAND(ID_MENUITEM_REFLASHLIST, &CDlgPageServer::OnMenuitemReflashlist)
	ON_NOTIFY(TVN_BEGINDRAG, IDC_TREE_SERVER, OnTvnBegindragTreeServer)
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_BN_CLICKED(IDC_BUTTON_PS_DEVICELIST, &CDlgPageServer::OnBnClickedButtonPsDevicelist)
	ON_BN_CLICKED(IDC_BUTTON_PS_CAMERALIST, &CDlgPageServer::OnBnClickedButtonPsCameralist)
	ON_BN_CLICKED(IDC_BUTTON_PS_RELATIONLIST, &CDlgPageServer::OnBnClickedButtonPsRelationlist)
	ON_BN_CLICKED(IDC_BUTTON_RELATION_SHOW, &CDlgPageServer::OnBnClickedButtonRelationShow)
	ON_BN_CLICKED(IDC_BUTTON_PS_CYCLE_LIST, &CDlgPageServer::OnBnClickedButtonPsCycleList)
	ON_BN_CLICKED(IDC_BUTTON_SEARCH, &CDlgPageServer::OnBnClickedButtonSearch)
	ON_COMMAND_RANGE(1000, 2000, OnControlButton)
	ON_NOTIFY(NM_RCLICK, IDC_TREE_TUNECYCLE, &CDlgPageServer::OnNMRClickTreeTunecycle)
	ON_COMMAND(ID_MENU_START_TUNECYCLE, &CDlgPageServer::OnMenuStartTunecycle)
	ON_COMMAND(ID_MENU_STOP_TUNECYCLE, &CDlgPageServer::OnMenuStopTunecycle)
	ON_WM_DESTROY()
	ON_NOTIFY(NM_CLICK, IDC_LIST_PS_RELATION, &CDlgPageServer::OnNMClickListPsRelation)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_PS_RELATION, &CDlgPageServer::OnNMDblclkListPsRelation)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_PS_CAMERA_RELATION, &CDlgPageServer::OnNMDblclkListPsCameraRelation)
	ON_NOTIFY(NM_DBLCLK, IDC_TREE_TUNECYCLE, &CDlgPageServer::OnNMDblclkTreeTunecycle)
	ON_COMMAND(ID_MENUITEM_CANCEL_CAMERA_TAG, &CDlgPageServer::OnMenuitemCancelCameraTag)
	ON_COMMAND(ID_MENUITEM_TAG_CAMERA, &CDlgPageServer::OnMenuitemTagCamera)
	ON_NOTIFY(NM_RCLICK, IDC_TREE_PRESET, &CDlgPageServer::OnNMRClickTreePreset)
	ON_COMMAND(ID_MENUITEM_OPEN_STATION_CAMERA_VIDEO, &CDlgPageServer::OnMenuitemOpenStationCameraVideo)
	ON_COMMAND(ID_MENUITEM_TAG_CLASS, &CDlgPageServer::OnMenuitemTagClass)
	ON_WM_TIMER()
	ON_COMMAND(ID_MENUITEM_OPEN_STATION_CAMERA_VIDEO_AUTO, &CDlgPageServer::OnMenuitemOpenStationCameraVideoAuto)
	ON_COMMAND(ID__MENUITEM_VIEW_CAMERA_STATUS, &CDlgPageServer::OnMenuitemViewCameraStatus)
	ON_COMMAND(ID_MENUITEM_SET_ANFANG_CAMERA, &CDlgPageServer::OnMenuitemSetAnfangCamera)
	ON_COMMAND(ID_MENUITEM_CANCEL_ANFANG_CAMERA, &CDlgPageServer::OnMenuitemCancelAnfangCamera)
	ON_COMMAND(ID_MENUITEM_ONLY_SHOW_ANFANG_CAMERA, &CDlgPageServer::OnMenuitemOnlyShowAnfangCamera)
	ON_COMMAND(ID_MENUITEM_SHOW_ALL_CAMERA, &CDlgPageServer::OnMenuitemShowAllCamera)
END_MESSAGE_MAP()


// CDlgPageServer 消息处理程序

BOOL CDlgPageServer::OnEraseBkgnd(CDC* pDC)
{
	CRect   rect;
	GetClientRect(&rect);

	BITMAP   bm;
	CDC   dcMem;
	CBitmap*   pOldBitmap = NULL;

	dcMem.CreateCompatibleDC(pDC);   
	pOldBitmap = dcMem.SelectObject(&m_PageServerBmp);   

	pDC->BitBlt(
		0,
		0,
		rect.right-rect.left,
		rect.bottom-rect.top,
		&dcMem,
		0,
		0,
		SRCCOPY);

	if (GetDlgItem(IDC_BUTTON_CHOOSEYT)->IsWindowVisible() == 1)
	{
		dcMem.SelectObject(&m_PageServerBottomBmp);  
		m_PageServerBottomBmp.GetObject(sizeof(BITMAP),(LPVOID)&bm);   

		pDC->BitBlt(
			0,
			rect.bottom - 200,
			bm.bmWidth,
			bm.bmHeight,
			&dcMem,
			0,
			0,
			SRCCOPY);
	}

	if (GetDlgItem(IDC_STATIC_SPEED)->IsWindowVisible() == 1)
	{
		m_YTControlBGBmp.GetObject(sizeof(BITMAP),(LPVOID)&bm);   
		dcMem.SelectObject(&m_YTControlBGBmp);   

		pDC->BitBlt(
			rect.left + 4,
			rect.bottom - 200 + 24,
			bm.bmWidth,
			bm.bmHeight,
			&dcMem,
			0,
			0,
			SRCCOPY);
	}

	//右边
	dcMem.SelectObject(&m_RightBmp);   

	pDC->StretchBlt(rect.right-m_RightBm.bmWidth,rect.top,
		m_RightBm.bmWidth,rect.bottom-rect.top,
		&dcMem,
		0,0,
		m_RightBm.bmWidth,m_RightBm.bmHeight,
		SRCCOPY);

	dcMem.SelectObject(pOldBitmap);

	return true;
}

BOOL CDlgPageServer::PreTranslateMessage(MSG* pMsg)
{
	m_pPtool.RelayEvent(pMsg);

	if((pMsg->message == WM_KEYDOWN || pMsg->message == WM_KEYUP) 
		&& (pMsg->wParam == VK_ESCAPE || pMsg->wParam== VK_RETURN || pMsg->wParam == VK_SPACE))
		return TRUE;

	return CPropertyPage::PreTranslateMessage(pMsg);
}

BOOL CDlgPageServer::LoadSkin()
{
	// 云台控制速度控件初始化
	m_titlespeed.SetTransparent(TRUE);					//设置背景透明色 “云台速度”
	m_titlespeed.SetTextColor(RGB(41, 41, 53));		//设置字体颜色
	m_Combo_YTSpeed.SetCurSel(4);

	// 按钮加载图片
	m_btnUp.LoadBitmap(IDB_BITMAP_YT_UP, 0);
	m_btnDown.LoadBitmap(IDB_BITMAP_YT_DOWN, 0);
	m_btnRight.LoadBitmap(IDB_BITMAP_YT_RIGHT, 0);
	m_btnLeft.LoadBitmap(IDB_BITMAP_YT_LEFT, 0);
	m_btnLock.LoadBitmap(IDB_BITMAP_YT_CENTER, 0);
	m_btnLock.LoadBitmap(IDB_BITMAP_YT_CENTER_LOCK, 1);
	m_btnLock.SetIndex(0);
	m_btnLock.Invalidate(true);
	m_btn_ZOOMIN.LoadBitmap(IDB_BITMAP_ZOOMIN, 0);
	m_btn_ZOOMOUT.LoadBitmap(IDB_BITMAP_ZOOMOUT, 0);
	m_btDiaphragmL.LoadBitmap(IDB_BITMAP_YT_PRESETCALL, 0);
	m_btDiaphragmS.LoadBitmap(IDB_BITMAP_YT_PRESETSAVE, 0);
	m_btnResurve2.LoadBitmap(IDB_BITMAP_YT_RESURVE2, 0);
	m_btnChoosePreset.LoadBitmap(IDB_BITMAP_CHOOSEPRESET, 0);
	m_btnYSOpen.LoadBitmap(IDB_BITMAP_YS_OPEN, 0);
	m_btnYSClose.LoadBitmap(IDB_BITMAP_YS_CLOSE, 0);
	m_btnDGOpen.LoadBitmap(IDB_BITMAP_DG_OPEN, 0);
	m_btnDGClose.LoadBitmap(IDB_BITMAP_DG_CLOSE, 0);
	m_btnHWOpen.LoadBitmap(IDB_BITMAP_HW_OPEN, 0);
	m_btnHWClose.LoadBitmap(IDB_BITMAP_HW_CLOSE, 0);
	m_btnJROpen.LoadBitmap(IDB_BITMAP_JR_OPEN, 0);
	m_btnJRClose.LoadBitmap(IDB_BITMAP_JR_CLOSE, 0);
	m_btnRelationLargeShow.LoadBitmap(IDB_BITMAP_RELATIONINFO_LARGESHOW, 0);

	m_btnListDevice.LoadBitmap(IDB_BITMAP_PS_DEVICELIST_ON, 0);
	m_btnListDevice.LoadBitmap(IDB_BITMAP_PS_DEVICELIST_OFF, 1);
	m_btnListDevice.SetIndex(1);
	m_btnListDevice.Invalidate(TRUE);
	m_btnListRelation.LoadBitmap(IDB_BITMAP_PS_VIEWLIST_ON, 0);
	m_btnListRelation.LoadBitmap(IDB_BITMAP_PS_VIEWLIST_OFF, 1);
	m_btnListRelation.SetIndex(0);
	m_btnListRelation.Invalidate(TRUE);
	m_btnListCamera.LoadBitmap(IDB_BITMAP_PS_CAMERALIST_ON, 0);
	m_btnListCamera.LoadBitmap(IDB_BITMAP_PS_CAMERALIST_OFF, 1);
	m_btnListCamera.SetIndex(1);
	m_btnListCamera.Invalidate(TRUE);
	m_btnListCycle.LoadBitmap(IDB_BITMAP_PS_CYCLELIST_ON, 0);
	m_btnListCycle.LoadBitmap(IDB_BITMAP_PS_CYCLELIST_OFF, 1);
	m_btnListCycle.SetIndex(1);
	m_btnListCycle.Invalidate(TRUE);

	m_btnChooseYT.LoadBitmap(IDB_BITMAP_CHOOSEYT_ON, 0);
	m_btnChooseYT.LoadBitmap(IDB_BITMAP_CHOOSEYT_OFF, 1);
	m_btnChooseYT.SetIndex(0);
	m_btnChooseYT.Invalidate(TRUE);
	m_btnChooseView.LoadBitmap(IDB_BITMAP_CHOOSEVIEW_ON, 0);
	m_btnChooseView.LoadBitmap(IDB_BITMAP_CHOOSEVIEW_OFF, 1);
	m_btnChooseView.SetIndex(0);
	m_btnChooseView.Invalidate(TRUE);
	m_btnChoosePreset.ShowWindow(SW_HIDE);

	m_btnSearch.LoadBitmap(IDB_BITMAP_BTN_SEARCH_NODE_BK,0);
	m_btnSearch.SetIndex(0);
	m_btnSearch.Invalidate(TRUE);

	m_btnShowHide.LoadBitmap(IDB_BITMAP_SHOW_PAGESERVER,0);
	m_btnShowHide.LoadBitmap(IDB_BITMAP_HIDE_PAGE_SERVER,1);
	m_btnShowHide.SetIndex(1);
	m_btnShowHide.Invalidate(TRUE);

	// 树状列表加载图片
	CBitmap bmp;
	bmp.LoadBitmap(IDB_BITMAP_TREE);							//加载位图对象,树状列表节点图片
	m_ilTreeIcons.Create(16, 16, ILC_MASK|ILC_COLOR24, 0, 1);	//创建树状列表
	m_ilTreeIcons.Add(&bmp, RGB(0, 255, 0));

	m_trServer.SetImageList(&m_ilTreeIcons, TVSIL_NORMAL);		//树状列表初始化
	m_trServer.SetBkColor(RGB(239, 246, 253));					//设置树状列表背景色
	m_trServer.SetTextColor(RGB(36, 162, 146));
	m_trPreset.SetImageList(&m_ilTreeIcons, TVSIL_NORMAL);		//树状列表初始化
	m_trPreset.SetBkColor(RGB(239, 246, 253));					//设置树状列表背景色
	m_trPreset.SetTextColor(RGB(36, 162, 146));
	m_trTuneCycle.SetImageList(&m_ilTreeIcons, TVSIL_NORMAL);		//树状列表初始化
	m_trTuneCycle.SetBkColor(RGB(239, 246, 253));					//设置树状列表背景色
	m_trTuneCycle.SetTextColor(RGB(36, 162, 146));

	m_RealRelationList.SetBkColor(RGB(239, 246, 253));
	m_RealRelationList.SetTextBkColor(RGB(239, 246, 253));
	m_RealRelationList.SetTextColor(RGB(36, 162, 146));

	m_RealCameraRelationList.SetBkColor(RGB(239, 246, 253));
	m_RealCameraRelationList.SetTextBkColor(RGB(239, 246, 253));
	m_RealCameraRelationList.SetTextColor(RGB(36, 162, 146));

	//////////////////////////////////////////////////////////////////////////
	m_PageServerBmp.LoadBitmap(IDB_BITMAP_PAGESERVER);
	m_PageServerBottomBmp.LoadBitmap(IDB_BITMAP_PAGE_SERVER_BOTTOM);
	m_YTControlBGBmp.LoadBitmap(IDB_BITMAP_BG_YT);
	m_RightBmp.LoadBitmap(IDB_BITMAP_PAGESERVER_RIGHT_BK);

	m_RightBmp.GetObject(sizeof(BITMAP),(LPVOID)&m_RightBm);

	return TRUE;
}

BOOL CDlgPageServer::InitControl()
{
	// 加载按钮提示名称
	m_pPtool.Create(this);
	m_pPtool.Activate(TRUE);   
	m_pPtool.SetTipBkColor(RGB(255,255,255));//背景颜色
	m_pPtool.SetTipTextColor(RGB(100,100,100));//字体颜色
	m_pPtool.SetDelayTime(100);   //显示时间   
	m_pPtool.AddTool(GetDlgItem(IDC_YT_UP),"云台向上");
	m_pPtool.AddTool(GetDlgItem(IDC_YT_DOWN),"云台向下");
	m_pPtool.AddTool(GetDlgItem(IDC_YT_LEFT),"云台向左");
	m_pPtool.AddTool(GetDlgItem(IDC_YT_RIGHT),"云台向右");
	m_pPtool.AddTool(GetDlgItem(IDC_YT_ZOOMIN),"焦距缩小");
	m_pPtool.AddTool(GetDlgItem(IDC_YT_ZOOMOUT),"焦距放大");
	m_pPtool.AddTool(GetDlgItem(IDC_YT_SAVE),"变焦进");
	m_pPtool.AddTool(GetDlgItem(IDC_YT_CALL),"变焦远");
	m_pPtool.AddTool(GetDlgItem(IDC_YT_DIAPHRAGM_LARGE),"光圈大");
	m_pPtool.AddTool(GetDlgItem(IDC_YT_DIAPHRAGM_SMALL),"光圈小");
	m_pPtool.AddTool(GetDlgItem(IDC_YT_RESURVE1),"保留1");
	m_pPtool.AddTool(GetDlgItem(IDC_YT_RESURVE2),"保留2");

	// 加载预置位和参数设置界面
	CRect	rc;
	rc.top = 30;
	rc.bottom = 210;
	rc.left = 6;
	rc.right = 220;

	m_DlgPreSet = new CDlgPreSet();
	if (m_DlgPreSet != NULL)
	{
		m_DlgPreSet->Create(IDD_DIALOG_PRESET, this);
		m_DlgPreSet->MoveWindow(rc);
		m_DlgPreSet->ShowWindow(SW_HIDE);
	}

	m_DlgVideoParSet = new CDlgVideoParSet();
	if (m_DlgVideoParSet != NULL)
	{
		m_DlgVideoParSet->Create(IDD_DIALOG_VIDEOPARSET, this);
		m_DlgVideoParSet->MoveWindow(rc);
		m_DlgVideoParSet->ShowWindow(SW_HIDE);
	}

	GetDlgItem(IDC_TREE_SERVER)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_TREE_PRESET)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_LIST_PS_RELATION)->ShowWindow(SW_SHOW);

	m_Form1.SetIcon(IDI_ONEUP);
	m_Form1.SetTooltipText(_T("单画面"));
	m_Form4.SetIcon(IDI_FOURUP);
	m_Form4.SetTooltipText(_T("四画面"));
	m_Form6.SetIcon(IDI_SIXUP);
	m_Form6.SetTooltipText(_T("六画面"));
	m_Form9.SetIcon(IDI_NINEUP);
	m_Form9.SetTooltipText(_T("九画面"));
	m_Form16.SetIcon(IDI_SIXTYUP);
	m_Form16.SetTooltipText(_T("十六画面"));

	// 实时联动请求列表
	// 查询结果列表中需要显示的列包括了：开始时间、结束时间、摄像头名称、
	DWORD dwStyle;
	dwStyle = m_RealRelationList.GetStyle();
	dwStyle = LVS_EX_GRIDLINES |LVS_EX_FULLROWSELECT|LVS_SHOWSELALWAYS ;
	m_RealRelationList.SetExtendedStyle(dwStyle);
	m_RealRelationList.InsertColumn(0,"联动类型");
	m_RealRelationList.SetColumnWidth(0,80);
	m_RealRelationList.InsertColumn(1,"设备名称");
	m_RealRelationList.SetColumnWidth(1,120);
	m_RealRelationList.InsertColumn(2,"站点名称");
	m_RealRelationList.SetColumnWidth(2,120);
	m_RealRelationList.InsertColumn(3,"联动时间");
	m_RealRelationList.SetColumnWidth(3,100);
	m_RealRelationList.InsertColumn(4,"分类型");
	m_RealRelationList.SetColumnWidth(4,80);
	m_RealRelationList.InsertColumn(5,"屏号");
	m_RealRelationList.SetColumnWidth(5,60);
	m_RealRelationList.InsertColumn(6,"备注");
	m_RealRelationList.SetColumnWidth(6,100);
	m_RealRelationList.InsertColumn(7,"Value1");
	m_RealRelationList.SetColumnWidth(7,60);
	m_RealRelationList.InsertColumn(8,"Value2");
	m_RealRelationList.SetColumnWidth(8,60);
	m_RealRelationList.InsertColumn(9,"联动站点");
	m_RealRelationList.SetColumnWidth(9,100);
	m_RealRelationList.InsertColumn(10,"联动数量");
	m_RealRelationList.SetColumnWidth(10,80);
	m_nRelationListShow = 0;// 显示缩小的

	dwStyle = m_RealCameraRelationList.GetStyle();
	dwStyle = LVS_EX_GRIDLINES |LVS_EX_FULLROWSELECT|LVS_SHOWSELALWAYS ;
	m_RealCameraRelationList.SetExtendedStyle(dwStyle);
	m_RealCameraRelationList.InsertColumn(0,"摄像头名称");
	m_RealCameraRelationList.SetColumnWidth(0,200);
	m_RealCameraRelationList.InsertColumn(1,"摄像头号码");
	m_RealCameraRelationList.SetColumnWidth(1,200);
	m_RealCameraRelationList.InsertColumn(2,"解码标签");
	m_RealCameraRelationList.SetColumnWidth(2,150);
	m_RealCameraRelationList.InsertColumn(3,"站点名称");
	m_RealCameraRelationList.SetColumnWidth(3,200);

	//time_t now_time = time(NULL);

	//time_t history_time = now_time + 8*60*60 - 24*60*60;

	//tm* history_tm =  gmtime(&history_time);

	//char szLinkageTime[64] = {0};

	//sprintf_s(szLinkageTime,"%04d-%02d-02dT%02d:%02d:%02dZ",history_tm->tm_year+1900,history_tm->tm_mon+1,history_tm->tm_mday,history_tm->tm_hour,history_tm->tm_min,history_tm->tm_sec);

	//ReadAndSetRelationListInfo(szLinkageTime);

	return TRUE;
}

BOOL CDlgPageServer::InitTreeList()
{
	try
	{
		if (g_nUserResourceShowType == 1)//admin
		{
#if VM_QABD_CLIENT_VERSION
			InitCameraTreelistQABD();// 创建摄像头列表树
#else
			InitCameraTreelist();// 创建摄像头列表树
#endif
			InitPresetList();		  // 创建预置位列表树
		}
		else if (g_nUserResourceShowType == 2)//省检
		{
			InitCameraTreelistByCity();//创建摄像头列表树
			InitPresetListByCity();		    // 创建预置位列表树
		}
		else if (g_nUserResourceShowType == 3)//省调
		{
			InitCameraTreelistByVoltage();//创建摄像头列表树
			InitPresetListByVoltage();		  // 创建预置位列表树
		}

		InitTuneCycleList();	             // 创建轮巡列表树

		return TRUE;
	}
	catch(...)
	{

	}
	return FALSE;
}

//将相关控制按钮的消息发送到主对话框进行处理
void CDlgPageServer::OnControlButton(UINT nID)
{
	if (g_pMainDlg != NULL)
		g_pMainDlg->PostMessage(OM_CONTROLBUTTON, nID,VEM_CONTROL_BUTTON_MESSAGE_TYPE_PAGESERVER);
}

BOOL CDlgPageServer::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	// 加载皮肤
	LoadSkin();

	//初始化控件
	InitControl();

	// 默认选择云台控制
	OnBnClickedButtonChooseyt();

	// 默认选择联动列表显示
	//OnBnClickedButtonPsRelationlist();

	//默认选择设备显示
	//OnBnClickedButtonPsDevicelist();

	//默认选择摄像头列表显示
	OnBnClickedButtonPsCameralist();

	if (g_pMainDlg != NULL)
	{
		g_pMainDlg->PostMessage(OM_CONTROLBUTTON, IDC_BTN_SHOW_HIDE);
	}

	SetTimer(1,1000,NULL);

	return TRUE;
}

void CDlgPageServer::OnSize(UINT nType, int cx, int cy)
{
	CPropertyPage::OnSize(nType, cx, cy);

	//////////////////////////////////////////////////////////////////////////
	if (GetDlgItem(IDC_YT_ZOOMIN)->GetSafeHwnd())
	{
		RECT rc;		
		CWnd *pWnd;

		int pageTop = 300;

		//LIST布置
		rc.top = 331 - pageTop;
		rc.bottom = cy-6;
		rc.left = 6;
		rc.right = cx -26;

		pWnd = GetDlgItem(IDC_BTN_SHOW_HIDE);//显示按钮
		if (pWnd)
		{
			CRect rect;
			CRect win_rc;

			if (g_pMainDlg != NULL)
			{
				g_pMainDlg->GetWindowRect(&win_rc);

				rect.left=rc.right+16;			
				rect.right=rc.right+26;
				rect.top= (win_rc.top+win_rc.bottom)/2-50;
				rect.bottom=rect.top+100;

				ScreenToClient(&rect);

				pWnd->MoveWindow(rect);		
			}
		}

		pWnd = GetDlgItem(IDC_TREE_SERVER);     //摄像机树
		if(pWnd)//判断是否为空，因为对话框创建时会调用此函数，而当时控件还未创建
		{
			CRect rect;				

			rect.left=rc.left-3;			
			rect.right=rc.right+13;
			rect.top=rc.top;
			rect.bottom=rc.bottom - 218;

			pWnd->MoveWindow(rect);		
		}

		pWnd = GetDlgItem(IDC_TREE_PRESET);     //预置位树
		if(pWnd)//判断是否为空，因为对话框创建时会调用此函数，而当时控件还未创建
		{
			CRect rect;					

			rect.left=rc.left-3;			
			rect.right=rc.right+13;
			rect.top=rc.top;
			rect.bottom=rc.bottom - 218;

			pWnd->MoveWindow(rect);		
		}

		pWnd = GetDlgItem(IDC_TREE_TUNECYCLE);     //轮巡树
		if(pWnd)//判断是否为空，因为对话框创建时会调用此函数，而当时控件还未创建
		{
			CRect rect;					

			rect.left=rc.left-3;			
			rect.right=rc.right+13;
			rect.top=rc.top;
			rect.bottom=rc.bottom - 218;

			pWnd->MoveWindow(rect);		
		}

		pWnd = GetDlgItem(IDC_EDIT_SEARCH); //搜索编辑框
		if (pWnd)//判断是否为空，因为对话框创建时会调用此函数，而当时控件还未创建
		{
			CRect rect;					

			rect.left=rc.left-3;			
			rect.right=rect.left+186;
			rect.bottom=rc.bottom - 196;
			rect.top=rect.bottom-20;

			pWnd->MoveWindow(rect);	
		}

		pWnd = GetDlgItem(IDC_BUTTON_SEARCH); //搜索按钮
		if (pWnd)//判断是否为空，因为对话框创建时会调用此函数，而当时控件还未创建
		{
			CRect rect;					

			rect.left=rc.left+186;			
			rect.right=rc.right+13;
			rect.bottom=rc.bottom - 196;
			rect.top=rect.bottom-22;

			pWnd->MoveWindow(rect);	
		}

		pWnd = GetDlgItem(IDC_LIST_PS_RELATION);     //联动列表
		if(pWnd)//判断是否为空，因为对话框创建时会调用此函数，而当时控件还未创建
		{
			CRect rect;					

			rect.left=rc.left-3;			
			rect.right=rc.right-15;
			rect.top=rc.top;
			rect.bottom=(rc.top + rc.bottom - 38)/2 - 5;

			pWnd->MoveWindow(rect);		
		}

		pWnd = GetDlgItem(IDC_LIST_PS_CAMERA_RELATION);     //联动摄像头列表
		if(pWnd)//判断是否为空，因为对话框创建时会调用此函数，而当时控件还未创建
		{
			CRect rect;					

			rect.left=rc.left-3;			
			rect.right=rc.right-15;
			rect.top=(rc.top + rc.bottom - 38)/2 + 5;
			rect.bottom=rc.bottom - 38;

			pWnd->MoveWindow(rect);		
		}

		pWnd = GetDlgItem(IDC_BUTTON_RELATION_SHOW);     //联动显示按钮
		if(pWnd)//判断是否为空，因为对话框创建时会调用此函数，而当时控件还未创建
		{
			CRect rect;					

			rect.left=rc.right-15 + 1;			
			rect.right=rc.right+6;
			rect.top=rc.top;
			rect.bottom=rc.top + 40;

			pWnd->MoveWindow(rect);		
		}

		rc.top = 300 - pageTop;
		rc.bottom = rc.top + 31;
		rc.left = 0;
		//		rc.right = rc.left + 90;
		rc.right = rc.left + 66;

		pWnd = GetDlgItem(IDC_BUTTON_PS_RELATIONLIST);     //联动按钮
		if(pWnd)//判断是否为空，因为对话框创建时会调用此函数，而当时控件还未创建
		{
			pWnd->MoveWindow(&rc);
			pWnd->ShowWindow(SW_HIDE);
		}

		rc.left = 0;
		rc.right = rc.left + 90;
		pWnd = GetDlgItem(IDC_BUTTON_PS_DEVICELIST);     //设备按钮
		if(pWnd)//判断是否为空，因为对话框创建时会调用此函数，而当时控件还未创建
		{
			pWnd->MoveWindow(&rc);
		}

		rc.left = rc.right;
		rc.right = rc.left + 90;
		pWnd = GetDlgItem(IDC_BUTTON_PS_CAMERALIST);     //摄像机按钮
		if(pWnd)//判断是否为空，因为对话框创建时会调用此函数，而当时控件还未创建
		{
			pWnd->MoveWindow(&rc);
		}

		rc.left = rc.right;
		rc.right = rc.left + 90;
		pWnd = GetDlgItem(IDC_BUTTON_PS_CYCLE_LIST);     //轮巡按钮
		if(pWnd)//判断是否为空，因为对话框创建时会调用此函数，而当时控件还未创建
		{
			pWnd->MoveWindow(&rc);
		}

		GetClientRect(&rc);
		pWnd = GetDlgItem(IDC_BUTTON_CHOOSEYT);//云台控制按钮
		if (pWnd)
		{
			CRect rect;
			rect.left = rc.left + 2;
			rect.right = rect.left + 132;
			rect.top = rc.bottom - 200;
			rect.bottom = rect.top +24;

			pWnd->MoveWindow(rect);
		}

		pWnd = GetDlgItem(IDC_BUTTON_CHOOSEVIEW);//视频参数设置按钮
		if (pWnd)
		{
			CRect rect;
			rect.left = rc.left + 134;
			rect.right = rect.left + 132;
			rect.top = rc.bottom - 200;
			rect.bottom = rect.top +24;

			pWnd->MoveWindow(rect);

			// 视频参数窗口
			rect.left = rc.left;
			rect.right = rc.right;
			rect.top = rc.bottom - 200 + 24;
			rect.bottom = rc.bottom;

			m_DlgVideoParSet->MoveWindow(rect);
		}

		rc.top = rc.bottom - 200 + 24;
		rc.left = 4;

		// 云台控制界面位置
		pWnd = GetDlgItem(IDC_YT_ZOOMIN);//云镜控制-缩小
		if (pWnd)
		{
			CRect rect;

			rect.top = rc.top + 28;
			rect.bottom = rect.top +44;
			rect.left = rc.left + 150;
			rect.right = rect.left + 90;

			pWnd->MoveWindow(rect);
		}

		pWnd = GetDlgItem(IDC_YT_ZOOMOUT);//云镜控制-放大
		if (pWnd)
		{
			CRect rect;

			rect.top = rc.top + 40 + 28;
			rect.bottom = rect.top + 44;
			rect.left = rc.left + 150;
			rect.right = rect.left + 90;

			pWnd->MoveWindow(rect);
		}

		pWnd = GetDlgItem(IDC_YT_LEFT);//云镜控制-向左
		if (pWnd)
		{
			CRect rect;

			rect.top = rc.top + 45;
			rect.bottom = rect.top + 56;
			rect.left = rc.left + 13;
			rect.right = rect.left + 41;

			pWnd->MoveWindow(rect);
		}

		pWnd = GetDlgItem(IDC_YT_UP);//云镜控制-向上
		if (pWnd)
		{
			CRect rect;

			rect.top = rc.top + 14;
			rect.bottom = rect.top + 41;
			rect.left = rc.left + 45;
			rect.right = rect.left + 87;

			pWnd->MoveWindow(rect);
		}

		pWnd = GetDlgItem(IDC_YT_DOWN);//云镜控制-向下
		if (pWnd)
		{
			CRect rect;

			rect.top = rc.top + 85;
			rect.bottom = rect.top + 41;
			rect.left = rc.left + 45;
			rect.right = rect.left + 87;

			pWnd->MoveWindow(rect);
		}

		pWnd = GetDlgItem(IDC_YT_RIGHT);//云镜控制-向右
		if (pWnd)
		{
			CRect rect;

			rect.top = rc.top + 45;
			rect.bottom = rect.top + 87;
			rect.left = rc.left + 85;
			rect.right = rect.left + 41;

			pWnd->MoveWindow(rect);
		}

		pWnd = GetDlgItem(IDC_YT_LOCK);//云镜控制-锁定
		if (pWnd)
		{
			CRect rect;

			rect.top = rc.top + 36;
			rect.bottom = rect.top + 54;
			rect.left = rc.left + 38;
			rect.right = rect.left + 54;

			pWnd->MoveWindow(rect);
		}

		pWnd = GetDlgItem(IDC_YT_SAVE);//云镜控制-保存
		if (pWnd)
		{
			CRect rect;

			rect.top = rc.top + 8;
			rect.bottom = rect.top + 34;
			rect.left = rc.left + 145;
			rect.right = rect.left + 44;

			pWnd->MoveWindow(rect);
		}

		pWnd = GetDlgItem(IDC_YT_DIAPHRAGM_LARGE);
		if (pWnd)
		{
			CRect rect;

			rect.top = rc.top + 34 + 8;
			rect.bottom = rect.top + 32;
			rect.left = rc.left + 145;
			rect.right = rect.left + 24;

			pWnd->MoveWindow(rect);
		}

		pWnd = GetDlgItem(IDC_YT_RESURVE1);
		if (pWnd)
		{
			CRect rect;

			rect.top = rc.top + 34*2 + 8;
			rect.bottom = rect.top + 34;
			rect.left = rc.left + 145;
			rect.right = rect.left + 46;

			pWnd->MoveWindow(rect);
		}

		pWnd = GetDlgItem(IDC_YT_CALL);
		if (pWnd)
		{
			CRect rect;

			rect.top = rc.top + 8;
			rect.bottom = rect.top + 34;
			rect.left = rc.left + 240;
			rect.right = rect.left + 30;

			pWnd->MoveWindow(rect);
		}

		pWnd = GetDlgItem(IDC_YT_DIAPHRAGM_SMALL);
		if (pWnd)
		{
			CRect rect;

			rect.top = rc.top + 34 + 8;
			rect.bottom = rect.top + 32;
			rect.left = rc.left + 240;
			rect.right = rect.left + 44;

			pWnd->MoveWindow(rect);
		}
		pWnd = GetDlgItem(IDC_YT_RESURVE2);
		if (pWnd)
		{
			CRect rect;

			rect.top = rc.top + 34*2 + 8;
			rect.bottom = rect.top + 34;
			rect.left = rc.left + 240;
			rect.right = rect.left + 44;

			pWnd->MoveWindow(rect);
		}

		pWnd = GetDlgItem(IDC_STATIC_SPEED);//云台移动速度
		if (pWnd)
		{
			CRect rect;

			rect.top = rc.top + 34*3 + 8 + 4;
			rect.bottom = rect.top + 20;
			rect.left = rc.left + 10;
			rect.right = rect.left + 80;

			pWnd->MoveWindow(rect);
		}

		pWnd = GetDlgItem(IDC_COMBO_SPEED);//云台移动速度
		if (pWnd)
		{
			CRect rect;

			rect.top = rc.top + 34*3 + 8;
			rect.bottom = rect.top + 20;
			rect.left = rc.left + 20 + 60;
			rect.right = rect.left+80;

			pWnd->MoveWindow(rect);
		}

		pWnd = GetDlgItem(IDC_BUTTON_PS_FORM1);//一分屏
		if (pWnd)
		{
			CRect rect;

			rect.top = rc.bottom - 40;
			rect.bottom = rect.top + 34;
			rect.left = rc.left+10;
			rect.right = rect.left + 32;

			pWnd->MoveWindow(rect);
		}

		pWnd = GetDlgItem(IDC_BUTTON_PS_FORM4);//四分屏
		if (pWnd)
		{
			CRect rect;

			rect.top = rc.bottom - 40;
			rect.bottom = rect.top + 34;
			rect.left = rc.left +10+ 42;
			rect.right = rect.left + 32;

			pWnd->MoveWindow(rect);
		}

		pWnd = GetDlgItem(IDC_BUTTON_PS_FORM6);//六分屏
		if (pWnd)
		{
			CRect rect;

			rect.top = rc.bottom - 40;
			rect.bottom = rect.top + 34;
			rect.left = rc.left +10+84;
			rect.right = rect.left + 32;

			pWnd->MoveWindow(rect);
		}

		pWnd = GetDlgItem(IDC_BUTTON_PS_FORM9);//九分屏
		if (pWnd)
		{
			CRect rect;
			rect.top = rc.bottom - 40;
			rect.bottom = rect.top + 34;
			rect.left = rc.left +10+126;
			rect.right = rect.left + 32;
			pWnd->MoveWindow(rect);
		}

		pWnd = GetDlgItem(IDC_BUTTON_PS_FORM16);//十六分屏
		if (pWnd)
		{
			CRect rect;
			rect.top = rc.bottom - 40;
			rect.bottom = rect.top + 34;
			rect.left = rc.left +10+ 168;
			rect.right = rect.left + 32;
			pWnd->MoveWindow(rect);
		}
	}
}

void CDlgPageServer::OnBnClickedButtonChooseyt()
{
	GetDlgItem(IDC_YT_ZOOMIN)->ShowWindow(SW_SHOW);
	GetDlgItem(IDC_YT_ZOOMOUT)->ShowWindow(SW_SHOW);
	GetDlgItem(IDC_YT_LEFT)->ShowWindow(SW_SHOW);
	GetDlgItem(IDC_YT_UP)->ShowWindow(SW_SHOW);
	GetDlgItem(IDC_YT_DOWN)->ShowWindow(SW_SHOW);
	GetDlgItem(IDC_YT_RIGHT)->ShowWindow(SW_SHOW);
	GetDlgItem(IDC_YT_SAVE)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_YT_CALL)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_YT_DIAPHRAGM_LARGE)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_YT_DIAPHRAGM_SMALL)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_YT_RESURVE1)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_YT_RESURVE2)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_COMBO_SPEED)->ShowWindow(SW_SHOW);
	GetDlgItem(IDC_STATIC_SPEED)->ShowWindow(SW_SHOW);
	GetDlgItem(IDC_YT_LOCK)->ShowWindow(SW_SHOW);
	//	GetDlgItem(IDC_YT_LOCK)->ShowWindow(SW_HIDE);

	if (m_DlgPreSet != NULL)
	{
		m_DlgPreSet->ShowWindow(SW_HIDE);
	}
	
	if (m_DlgVideoParSet != NULL)
	{
		m_DlgVideoParSet->ShowWindow(SW_HIDE);
	}
	
	m_btnChooseYT.SetIndex(1);
	m_btnChooseYT.Invalidate(true);
	m_btnChooseView.SetIndex(0);
	m_btnChooseView.Invalidate(true);
}

void CDlgPageServer::OnBnClickedButtonChooseview()
{
	GetDlgItem(IDC_YT_ZOOMIN)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_YT_ZOOMOUT)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_YT_LEFT)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_YT_UP)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_YT_DOWN)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_YT_RIGHT)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_YT_SAVE)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_YT_CALL)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_YT_DIAPHRAGM_LARGE)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_YT_DIAPHRAGM_SMALL)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_YT_RESURVE1)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_YT_RESURVE2)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_COMBO_SPEED)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_STATIC_SPEED)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_YT_LOCK)->ShowWindow(SW_HIDE);

	m_DlgPreSet->ShowWindow(SW_HIDE);

	m_DlgVideoParSet->ShowWindow(SW_SHOW);
	// 	GetDlgItem(IDC_TREE_SERVER)->ShowWindow(SW_SHOW);
	// 	GetDlgItem(IDC_TREE_PRESET)->ShowWindow(SW_HIDE);
	m_btnChooseView.SetIndex(1);
	m_btnChooseView.Invalidate(true);
	m_btnChooseYT.SetIndex(0);
	m_btnChooseYT.Invalidate(true);
}

void CDlgPageServer::OnBnClickedButtonChoosePreset()
{
	GetDlgItem(IDC_YT_ZOOMIN)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_YT_ZOOMOUT)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_YT_LEFT)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_YT_UP)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_YT_DOWN)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_YT_RIGHT)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_YT_SAVE)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_YT_CALL)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_YT_DIAPHRAGM_LARGE)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_YT_DIAPHRAGM_SMALL)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_YT_RESURVE1)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_YT_RESURVE2)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_COMBO_SPEED)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_STATIC_SPEED)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_YT_LOCK)->ShowWindow(SW_HIDE);

	m_DlgPreSet->ShowWindow(SW_SHOW);
	m_DlgPreSet->DisplayPresetName();

	m_DlgVideoParSet->ShowWindow(SW_HIDE);
}

void CDlgPageServer::OnNMClickTreeServer(NMHDR *pNMHDR, LRESULT *pResult)
{
	if (!IsWindowVisible()) return;
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;

	HTREEITEM hItem = pNMTreeView->itemNew.hItem; //m_trServer.GetSelectedItem();

	m_hSelectPageItem = hItem;

	*pResult = 0;
}

void CDlgPageServer::SetPageServerItemInfo(HTREEITEM& hInsertItem,int nAnfang_flag,char* pCameraName,int camera_status,int streamless,int diagnosis_flag)
{
	char camera_name[80] = {0};
	if (nAnfang_flag==0)
	{
		//非安防
		sprintf_s(camera_name,64,"%s",pCameraName);
	}

	if(nAnfang_flag==1)
	{
		sprintf_s(camera_name,64,"[安防]%s",pCameraName);
	}

	if (camera_status == 0)
	{
		hInsertItem = m_trServer.InsertItem(camera_name, 4, 4, m_hSelectItem);
	}
	else if (camera_status == 1)
	{
		if (streamless == 1)
		{
			hInsertItem = m_trServer.InsertItem(camera_name, 4, 4, m_hSelectItem);
		}
		else
		{
			//无故障
			if (diagnosis_flag == 0)
			{
				hInsertItem = m_trServer.InsertItem(camera_name, 5, 5, m_hSelectItem);
			}
			//有故障
			else
			{
				hInsertItem = m_trServer.InsertItem(camera_name, 9, 9, m_hSelectItem);
			}
		}
	}
	else if (camera_status == 2)
	{
		if (streamless == 1)
		{
			hInsertItem = m_trServer.InsertItem(camera_name, 11, 11, m_hSelectItem);
		}
		else
		{
			hInsertItem = m_trServer.InsertItem(camera_name, 9, 9, m_hSelectItem);
		}
	}
	else if (camera_status == 3)
	{
		hInsertItem = m_trServer.InsertItem(camera_name, 11, 11, m_hSelectItem);
	}
	else
	{
		//其他异常状态
		hInsertItem = m_trServer.InsertItem(camera_name, 9, 9, m_hSelectItem);
	}
}

//双击树列表
void CDlgPageServer::OnNMDblclkTreeServer(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);

	try
	{
		CTreeCtrl *pList = (CTreeCtrl *)GetDlgItem(IDC_TREE_SERVER);	
		HTREEITEM hItem = pList->GetSelectedItem();

		char stationname[64] = {0};

		HTREEITEM hInsertItem = NULL;
		HTREEITEM hChildItem = NULL;
		HTREEITEM hParentItem = m_trServer.GetParentItem(hItem);

		if (hParentItem != NULL)
		{
			sprintf(stationname, "%s", m_trServer.GetItemText(hParentItem));
		}

		m_hSelectItem = hItem;
		m_trServer.SelectItem(hItem);
		if (hItem == NULL)
			return;

		if (g_pMainDlg == NULL)
			return;

		// 判断节点是何种类型节点
		m_pCameraNodeInfo = (_T_NODE_INFO *)(pList->GetItemData(hItem));

		if (m_pCameraNodeInfo == NULL)
			return;

		//如果类型为2，则加载该结点下的摄像头
		if (m_pCameraNodeInfo->node_type == 2)
		{
			if (m_trServer.ItemHasChildren(hItem))
				return;

			// 表示该节点为变电站，获取数据库中的信息，并将预置位信息插入到数据表中

			char sql_buf[1024] = {0};
			MYSQL_RES * res = NULL;
			MYSQL_ROW	row ;
			int camera_type = -1;
			int decode_tag = 0;
			int camera_status = 0;
			int streamless = 0;
			int anfang_flag = 0;
			char camera_name[64]={0};
			int diagnosis_flag = 0;

			// 读取当前视频平台数量及信息
			sprintf_s(sql_buf, "SELECT a.camera_id, a.camera_code, a.camera_name, a.camera_type,a.camera_decodetag,a.camera_status,a.streamless,b.anfang_flag as anfang_flag,  "
				" IFNULL(c.TotalResult,0) "
				" FROM ob_d5000_camera_station_%d a left join video_camera b on a.camera_code=b.camera_id  "
				" left join vaichannlscheckresults c on c.CameraCode=a.camera_code "
				" ORDER BY a.camera_id ASC", m_pCameraNodeInfo->node_id);

			if (!mysql_query(g_mySqlData, sql_buf))
			{
				res = mysql_store_result(g_mySqlData);

				while ( row = mysql_fetch_row( res ) )
				{
					camera_type = atoi(row[3]);
					if (camera_type <= 0||camera_type > 8)
						continue;

					decode_tag = atoi(row[4]);
					camera_status = atoi(row[5]);
					streamless = atoi(row[6]);
					anfang_flag = atoi(row[7]);
					sprintf_s(camera_name,64,"%s",row[2]);
					diagnosis_flag = atoi(row[8]);
					//如果设置了仅显示安防摄像机，遇到了没有安防标志的，则跳过
					if (m_bOnlyShowAnFang == TRUE && anfang_flag == 0)
					{
						continue;
					}

					//设置节点文本和图标
					SetPageServerItemInfo(hInsertItem,anfang_flag,camera_name,camera_status,streamless,diagnosis_flag);

					if (hInsertItem == NULL)
						continue;

					SetCameraNodeInfo(hInsertItem, row[2], row[1], m_pCameraNodeInfo->node_id,
						camera_status,
						streamless,
						0,
						0,
						decode_tag,
						3,
						m_pCameraNodeInfo->node_station);
				}
				mysql_free_result(res) ;
			}

			//设置摄像头DVR信息
			SetTreeServerStationCameraDvrInfo(m_hSelectItem);
		}

		//如果类型为3，则双击的就是摄像头，打开该摄像头
		else if (m_pCameraNodeInfo->node_type >= 3)
		{
			if (g_TuneCycle_MehtodInfo.bState == true)
			{
				MessageBox("当前处于轮巡状态","视频预览",MB_ICONWARNING);
				return;
			}

			if (g_VideoPageViewInfo.nStatus == AUTOVIDEO_PAGEVIEW_STATUS_RUNNING)
			{
				MessageBox("当前处于人工自动巡视状态","视频预览",MB_ICONWARNING);
				return;
			}

			// 表示该节点为摄像头,打开视频
			VIDEO_SIP_CALL_SERVER_INFO VideoSipCallServerInfo;
			memset(&VideoSipCallServerInfo,0,sizeof(VideoSipCallServerInfo));

			if (IsCameraVideoLinkByVideoPlatform(g_nClientVideoLinkType,m_pCameraNodeInfo->node_decodetag))//客户端视频连接方式
			{
				VideoSipCallServerInfo.nType = 1;//通过南瑞平台
				VideoSipCallServerInfo.nDecodeTag = m_pCameraNodeInfo->node_decodetag;
			}
			else
			{
				VideoSipCallServerInfo.nType = 2;//直接连接前端设备
				VideoSipCallServerInfo.nDecodeTag = m_pCameraNodeInfo->camera_info.dvr_info.dvr_type;
			}

			VideoSipCallServerInfo.nStatus = m_pCameraNodeInfo->node_status;
			VideoSipCallServerInfo.nStreamless = m_pCameraNodeInfo->node_streamless;
			strcpy_s(VideoSipCallServerInfo.szCode,sizeof(VideoSipCallServerInfo.szCode),m_pCameraNodeInfo->node_num);

			if (g_pMainDlg != NULL&&g_pMainDlg->m_iFocuseWindowID >= 0&&g_pMainDlg->m_iFocuseWindowID < g_pMainDlg->m_FormsNum)
			{
				VideoSipCallServerInfo.nScreenId = g_pMainDlg->m_iFocuseWindowID;
			}
			else
			{
				VideoSipCallServerInfo.nScreenId = 0;
			}

			strcpy_s(VideoSipCallServerInfo.szName,sizeof(VideoSipCallServerInfo.szName),m_pCameraNodeInfo->node_name);
			VideoSipCallServerInfo.hItem = m_hSelectItem;
			strcpy_s(VideoSipCallServerInfo.szStation,sizeof(VideoSipCallServerInfo.szStation),m_pCameraNodeInfo->node_station);
			memcpy(&VideoSipCallServerInfo.server_node_info,m_pCameraNodeInfo,sizeof(VideoSipCallServerInfo.server_node_info));

			int nOperate_type = WM_DEVICE_VIDEO_OPERATE_RESULT_NOTAG_ONLINE_VIDEO;
			int nOperate_result = WM_DEVICE_VIDEO_OPERATE_RESULT_NOTAG_ONLINE_VIDEO;

			if (m_pCameraNodeInfo->node_status == 0)
			{
				nOperate_type = WM_DEVICE_VIDEO_OPERATE_RESULT_NOTAG_OFFLINE_VIDEO;
				nOperate_result = WM_DEVICE_VIDEO_OPERATE_RESULT_NOTAG_OFFLINE_VIDEO;
			}
			else if (m_pCameraNodeInfo->node_status == 2)
			{
				nOperate_type = WM_DEVICE_VIDEO_OPERATE_RESULT_TAG_ONLINE_VIDEO;
				nOperate_result = WM_DEVICE_VIDEO_OPERATE_RESULT_TAG_ONLINE_VIDEO;
			}
			else if (m_pCameraNodeInfo->node_status == 3)
			{
				nOperate_type = WM_DEVICE_VIDEO_OPERATE_RESULT_TAG_OFFLINE_VIDEO;
				nOperate_result = WM_DEVICE_VIDEO_OPERATE_RESULT_TAG_OFFLINE_VIDEO;
			}



			WriteUserClientVideoOperateInfo(m_pCameraNodeInfo->node_num,m_pCameraNodeInfo->node_name,m_pCameraNodeInfo->node_station,nOperate_type,nOperate_result);

			g_ThreadVideoOperateNumberInfo.DeviceVideoInNumerAdd();

			g_pMainDlg->ThreadPoolDispatchTask(ThreadMakeCallCameraServer,(void *)&VideoSipCallServerInfo,sizeof(VideoSipCallServerInfo),2);

			return;
		}
	}
	catch(...)
	{

	}

	*pResult = 0;
}


void CDlgPageServer::OnNMDblclkTreePreset(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);

	try
	{
		CTreeCtrl *pList = (CTreeCtrl *)GetDlgItem(IDC_TREE_PRESET);	
		HTREEITEM hItem = pList->GetSelectedItem();

		char stationname[64] = {0};

		HTREEITEM hChildItem = NULL;
		HTREEITEM hParentItem = m_trPreset.GetParentItem(hItem);
		HTREEITEM hInsertItem = NULL;

		if (hParentItem != NULL)
		{
			sprintf(stationname, "%s", m_trPreset.GetItemText(hParentItem));
		}

		m_hSelectItem = hItem;
		m_trPreset.SelectItem(hItem);
		if (hItem == NULL)
			return;

		char sql_buf[1024] = {0};
		MYSQL_RES	*res = NULL ;
		MYSQL_ROW	row ;

		// 判断节点是何种类型节点
		m_pPresetNodeInfo = (_T_NODE_PRESET_INFO *)(pList->GetItemData(hItem));

		if (m_pPresetNodeInfo == NULL)
			return;

		if (m_pPresetNodeInfo->node_type == 2)//站点
		{
			if (m_trPreset.ItemHasChildren(hItem))
				return;

			hInsertItem = m_trPreset.InsertItem("区域", 7, 7, m_hSelectItem);
			SetPresetNodeInfo(hInsertItem, "区域", "00000", m_pPresetNodeInfo->node_id, NULL, 0,
				1,
				0,
				0,
				0,
				0,
				6,
				m_pPresetNodeInfo->node_station,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0
				);

			hInsertItem = m_trPreset.InsertItem("设备", 7, 7, m_hSelectItem);
			SetPresetNodeInfo(hInsertItem, "设备", "00000", m_pPresetNodeInfo->node_id, NULL, 0,
				1,
				0,
				0,
				0,
				0,
				7,
				m_pPresetNodeInfo->node_station,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0
				);
		}
		else if (m_pPresetNodeInfo->node_type == 3)//camera
		{
			if (g_TuneCycle_MehtodInfo.bState == true)
			{
				MessageBox("当前处于轮巡状态","视频预览",MB_ICONWARNING);
				return;
			}

			if (g_VideoPageViewInfo.nStatus == AUTOVIDEO_PAGEVIEW_STATUS_RUNNING)
			{
				MessageBox("当前处于人工自动巡视状态","视频预览",MB_ICONWARNING);
				return;
			}

			VIDEO_SIP_CALL_PRESET_INFO VideoSipCallPresetInfo;
			memset(&VideoSipCallPresetInfo,0,sizeof(VideoSipCallPresetInfo));

			if (IsCameraVideoLinkByVideoPlatform(g_nClientVideoLinkType,m_pPresetNodeInfo->node_decodetag))
			{
				VideoSipCallPresetInfo.nType = 1;
				VideoSipCallPresetInfo.nDecodeTag = m_pPresetNodeInfo->node_decodetag;
			}
			else
			{
				VideoSipCallPresetInfo.nType = 3;
				VideoSipCallPresetInfo.nDecodeTag = m_pPresetNodeInfo->camera_info.dvr_info.dvr_type;
			}

			VideoSipCallPresetInfo.nStatus = m_pPresetNodeInfo->node_status;
			VideoSipCallPresetInfo.nStreamless = m_pPresetNodeInfo->node_streamless;
			strcpy_s(VideoSipCallPresetInfo.szCode,sizeof(VideoSipCallPresetInfo.szCode),m_pPresetNodeInfo->node_num);

			if (g_pMainDlg != NULL&&g_pMainDlg->m_iFocuseWindowID >= 0&&g_pMainDlg->m_iFocuseWindowID < g_pMainDlg->m_FormsNum)
			{
				VideoSipCallPresetInfo.nScreenId = g_pMainDlg->m_iFocuseWindowID;
			}
			else
			{
				VideoSipCallPresetInfo.nScreenId = 0;
			}

			strcpy_s(VideoSipCallPresetInfo.szName,sizeof(VideoSipCallPresetInfo.szName),m_pPresetNodeInfo->node_name);
			VideoSipCallPresetInfo.hItem = m_hSelectItem;
			strcpy_s(VideoSipCallPresetInfo.szStation,sizeof(VideoSipCallPresetInfo.szStation),m_pPresetNodeInfo->node_station);
			memcpy(&VideoSipCallPresetInfo.preset_node_info,m_pPresetNodeInfo,sizeof(VideoSipCallPresetInfo.preset_node_info));

			int nOperate_type = WM_DEVICE_VIDEO_OPERATE_RESULT_NOTAG_ONLINE_VIDEO;
			int nOperate_result = WM_DEVICE_VIDEO_OPERATE_RESULT_NOTAG_ONLINE_VIDEO;

			if (m_pPresetNodeInfo->node_status == 0)
			{
				nOperate_type = WM_DEVICE_VIDEO_OPERATE_RESULT_NOTAG_OFFLINE_VIDEO;
				nOperate_result = WM_DEVICE_VIDEO_OPERATE_RESULT_NOTAG_OFFLINE_VIDEO;
			}
			else if (m_pPresetNodeInfo->node_status == 2)
			{
				nOperate_type = WM_DEVICE_VIDEO_OPERATE_RESULT_TAG_ONLINE_VIDEO;
				nOperate_result = WM_DEVICE_VIDEO_OPERATE_RESULT_TAG_ONLINE_VIDEO;
			}
			else if (m_pPresetNodeInfo->node_status == 3)
			{
				nOperate_type = WM_DEVICE_VIDEO_OPERATE_RESULT_TAG_OFFLINE_VIDEO;
				nOperate_result = WM_DEVICE_VIDEO_OPERATE_RESULT_TAG_OFFLINE_VIDEO;
			}

			WriteUserClientVideoOperateInfo(m_pPresetNodeInfo->node_num,m_pPresetNodeInfo->node_name,m_pPresetNodeInfo->node_station,nOperate_type,nOperate_result);

			g_pMainDlg->ThreadPoolDispatchTask(ThreadMakeCallCameraPreset,(void *)&VideoSipCallPresetInfo,sizeof(VideoSipCallPresetInfo),2);

			Sleep(100);
		}
		else if (m_pPresetNodeInfo->node_type == 5)//预置位
		{
			if (g_TuneCycle_MehtodInfo.bState == true)
			{
				MessageBox("当前处于轮巡状态","视频预览",MB_ICONWARNING);
				return;
			}

			if (g_VideoPageViewInfo.nStatus == AUTOVIDEO_PAGEVIEW_STATUS_RUNNING)
			{
				MessageBox("当前处于人工自动巡视状态","视频预览",MB_ICONWARNING);
				return;
			}

			// 表示该节点为摄像头，需要打开视频，并调用预置位
			VIDEO_SIP_CALL_PRESET_INFO VideoSipCallPresetInfo;
			memset(&VideoSipCallPresetInfo,0,sizeof(VideoSipCallPresetInfo));

			if (IsCameraVideoLinkByVideoPlatform(g_nClientVideoLinkType,m_pPresetNodeInfo->node_decodetag))
			{
				VideoSipCallPresetInfo.nType = 2;
				VideoSipCallPresetInfo.nDecodeTag = m_pPresetNodeInfo->node_decodetag;
			}
			else
			{
				VideoSipCallPresetInfo.nType = 4;
				VideoSipCallPresetInfo.nDecodeTag = m_pPresetNodeInfo->camera_info.dvr_info.dvr_type;
			}

			VideoSipCallPresetInfo.nStatus = m_pPresetNodeInfo->node_status;
			VideoSipCallPresetInfo.nStreamless = m_pPresetNodeInfo->node_streamless;

			strcpy_s(VideoSipCallPresetInfo.szCode,sizeof(VideoSipCallPresetInfo.szCode),m_pPresetNodeInfo->node_num);

			if (g_pMainDlg != NULL&&g_pMainDlg->m_iFocuseWindowID >= 0&&g_pMainDlg->m_iFocuseWindowID < g_pMainDlg->m_FormsNum)
			{
				VideoSipCallPresetInfo.nScreenId = g_pMainDlg->m_iFocuseWindowID;
			}
			else
			{
				VideoSipCallPresetInfo.nScreenId = 0;
			}

			strcpy_s(VideoSipCallPresetInfo.szName,sizeof(VideoSipCallPresetInfo.szName),m_pPresetNodeInfo->node_name);
			VideoSipCallPresetInfo.hItem = m_hSelectItem;
			strcpy_s(VideoSipCallPresetInfo.szStation,sizeof(VideoSipCallPresetInfo.szStation),m_pPresetNodeInfo->node_station);
			memcpy(&VideoSipCallPresetInfo.preset_node_info,m_pPresetNodeInfo,sizeof(VideoSipCallPresetInfo.preset_node_info));

			int nOperate_type = WM_DEVICE_VIDEO_OPERATE_RESULT_NOTAG_ONLINE_VIDEO;
			int nOperate_result = WM_DEVICE_VIDEO_OPERATE_RESULT_NOTAG_ONLINE_VIDEO;

			if (m_pPresetNodeInfo->node_status == 0)
			{
				nOperate_type = WM_DEVICE_VIDEO_OPERATE_RESULT_NOTAG_OFFLINE_VIDEO;
				nOperate_result = WM_DEVICE_VIDEO_OPERATE_RESULT_NOTAG_OFFLINE_VIDEO;
			}
			else if (m_pPresetNodeInfo->node_status == 2)
			{
				nOperate_type = WM_DEVICE_VIDEO_OPERATE_RESULT_TAG_ONLINE_VIDEO;
				nOperate_result = WM_DEVICE_VIDEO_OPERATE_RESULT_TAG_ONLINE_VIDEO;
			}
			else if (m_pPresetNodeInfo->node_status == 3)
			{
				nOperate_type = WM_DEVICE_VIDEO_OPERATE_RESULT_TAG_OFFLINE_VIDEO;
				nOperate_result = WM_DEVICE_VIDEO_OPERATE_RESULT_TAG_OFFLINE_VIDEO;
			}

			WriteUserClientVideoOperateInfo(m_pPresetNodeInfo->node_num,m_pPresetNodeInfo->node_name,m_pPresetNodeInfo->node_station,nOperate_type,nOperate_result);

			g_pMainDlg->ThreadPoolDispatchTask(ThreadMakeCallCameraPreset,(void *)&VideoSipCallPresetInfo,sizeof(VideoSipCallPresetInfo),2);

		}
		else if (m_pPresetNodeInfo->node_type == 6)//区域
		{
			if (m_trPreset.ItemHasChildren(hItem))
				return;

			// 场景预置位
			sprintf_s(sql_buf, "SELECT scene_id, camera_code, camera_name, camera_type, camera_preset_id, preset_name,scene_name "
				" FROM ob_d5000_scene_camerapreset where plant_station_id=%d ORDER BY scene_name ASC",m_pPresetNodeInfo->node_id);

			if (!mysql_query(g_mySqlData, sql_buf))
			{
				res = mysql_store_result(g_mySqlData);

				while ( row = mysql_fetch_row( res ) )
				{
					hInsertItem = m_trPreset.InsertItem(row[6], 5, 5, m_hSelectItem);
					SetPresetNodeInfo(hInsertItem, row[2], row[1], m_pPresetNodeInfo->node_id, row[5], atoi(row[4]),
						1,
						0,
						0,
						0,
						atoi(row[3]),
						5,
						m_pPresetNodeInfo->node_station,
						0,
						0,
						0,
						0,
						0,
						0,
						0,
						0
						);
				}
				mysql_free_result( res ) ;
				res = NULL;
			}

			//设置摄像头DVR信息
			SetTreePresetStationCameraDvrInfo(m_hSelectItem);

			////设置摄像头DVR信息
			//if(m_trPreset.ItemHasChildren(m_hSelectItem) != NULL)
			//{
			//	hInsertItem = m_trPreset.GetChildItem(m_hSelectItem);
			//	while(hInsertItem != NULL)
			//	{
			//		SetTreePresetCameraDvrInfo(hInsertItem);
			//		hInsertItem = m_trPreset.GetNextSiblingItem(hInsertItem);
			//	}
			//}
		}
		else if (m_pPresetNodeInfo->node_type == 7)//设备组
		{
			if (m_trPreset.ItemHasChildren(hItem))
				return;

			int nCount = 0;
			int nCameraStatus = 1;
			int nStreamless = 0;

			// 表示该节点为变电站的设备组，获取数据库中的信息，并将预置位信息插入到数据表中

			// 读取当前视频平台数量及信息，不读上级平台
			sprintf_s(sql_buf, "SELECT t1.camera_id, t1.camera_code, t1.camera_name, t1.camera_type, t1.camera_preset_id, t1.preset_name,t1.from_x_1,t1.from_y_1,t1.to_x_1,t1.to_y_1,t1.from_x_2,t1.from_y_2,t1.to_x_2,t1.to_y_2,t2.camera_status,t2.streamless "
				" FROM ob_d5000_camerapreset_station_%d AS t1,ob_d5000_camera_station_%d AS t2 WHERE t1.camera_code=t2.camera_code ORDER BY t1.preset_name ASC",m_pPresetNodeInfo->node_id,m_pPresetNodeInfo->node_id);

			if (!mysql_query(g_mySqlData, sql_buf))
			{
				res = mysql_store_result(g_mySqlData);

				while ( row = mysql_fetch_row( res ) )
				{
					nCameraStatus = atoi(row[14]);
					nStreamless = atoi(row[15]);

					if (nCameraStatus == 0)
					{
						hInsertItem = m_trPreset.InsertItem(row[5], 4, 4, m_hSelectItem);
					}
					else if (nCameraStatus == 1)
					{
						if (nStreamless == 1)
						{
							hInsertItem = m_trPreset.InsertItem(row[5], 4, 4, m_hSelectItem);
						}
						else
						{
							hInsertItem = m_trPreset.InsertItem(row[5], 5, 5, m_hSelectItem);
						}
					}
					else if (nCameraStatus == 2)
					{
						if (nStreamless == 1)
						{
							hInsertItem = m_trPreset.InsertItem(row[5], 11, 11, m_hSelectItem);
						}
						else
						{
							hInsertItem = m_trPreset.InsertItem(row[5], 9, 9, m_hSelectItem);
						}
					}
					else if (nCameraStatus == 3)
					{
						hInsertItem = m_trPreset.InsertItem(row[5], 11, 11, m_hSelectItem);
					}
					else
					{
						hInsertItem = m_trPreset.InsertItem(row[5], 5, 5, m_hSelectItem);
					}

					if (hInsertItem == NULL)
						continue;

					SetPresetNodeInfo(hInsertItem, row[2], row[1], m_pPresetNodeInfo->node_id, row[5], atoi(row[4]),
						nCameraStatus,
						nStreamless,
						0,
						0,
						atoi(row[3]),
						5,
						m_pPresetNodeInfo->node_station,
						atoi(row[6]),
						atoi(row[7]),
						atoi(row[8]),
						atoi(row[9]),
						atoi(row[10]),
						atoi(row[11]),
						atoi(row[12]),
						atoi(row[13])
						);

					nCount++;
				}
				mysql_free_result( res ) ;
				res = NULL;
			}

			if (nCount == 0)//没有找到预置位，再查找摄像头表
			{
				hParentItem = m_trPreset.GetParentItem(hItem);
				if (hParentItem == NULL)
					return;

				hParentItem = m_trPreset.GetParentItem(hParentItem);
				if (hParentItem == NULL)
					return;

				hParentItem = m_trPreset.GetParentItem(hParentItem);
				if (hParentItem == NULL)
					return;

				CString strItemName= m_trPreset.GetItemText(hParentItem);
				if (strItemName.Compare("输电") != 0)
					return;

				// 读取当前视频平台数量及信息
				sprintf_s(sql_buf, "SELECT camera_id, camera_code, camera_name, camera_type,camera_decodetag,camera_status,streamless "
					" FROM ob_d5000_camera_station_%d ORDER BY camera_id ASC", m_pPresetNodeInfo->node_id);

				if (!mysql_query(g_mySqlData, sql_buf))
				{
					res = mysql_store_result(g_mySqlData);

					while ( row = mysql_fetch_row( res ) )
					{
						nCameraStatus = atoi(row[5]);
						nStreamless = atoi(row[6]);

						if (nCameraStatus == 0)
						{
							hInsertItem = m_trPreset.InsertItem(row[2], 4, 4, m_hSelectItem);
						}
						else if (nCameraStatus == 1)
						{
							if (nStreamless == 1)
							{
								hInsertItem = m_trPreset.InsertItem(row[2], 4, 4, m_hSelectItem);
							}
							else
							{
								hInsertItem = m_trPreset.InsertItem(row[2], 5, 5, m_hSelectItem);
							}
						}
						else if (nCameraStatus == 2)
						{
							if (nStreamless == 1)
							{
								hInsertItem = m_trPreset.InsertItem(row[2], 11, 11, m_hSelectItem);
							}
							else
							{
								hInsertItem = m_trPreset.InsertItem(row[2], 9, 9, m_hSelectItem);
							}
						}
						else if (nCameraStatus == 3)
						{
							hInsertItem = m_trPreset.InsertItem(row[2], 11, 11, m_hSelectItem);
						}
						else
						{
							hInsertItem = m_trPreset.InsertItem(row[2], 5, 5, m_hSelectItem);
						}

						if (hInsertItem == NULL)
							continue;

						SetPresetNodeInfo(hInsertItem, row[2], row[1], m_pPresetNodeInfo->node_id,row[2],-1,
							nCameraStatus,
							nStreamless,
							0,
							0,
							atoi(row[4]),
							5,
							m_pPresetNodeInfo->node_station,
							0,
							0,
							0,
							0,
							0,
							0,
							0,
							0);
					}
					mysql_free_result( res ) ;
					res = NULL;
				}
			}

			//设置摄像头DVR信息
			SetTreePresetStationCameraDvrInfo(m_hSelectItem);

			////设置摄像头DVR信息
			//if(m_trPreset.ItemHasChildren(m_hSelectItem) != NULL)
			//{
			//	hInsertItem = m_trPreset.GetChildItem(m_hSelectItem);
			//	while(hInsertItem != NULL)
			//	{
			//		SetTreePresetCameraDvrInfo(hInsertItem);
			//		hInsertItem = m_trPreset.GetNextSiblingItem(hInsertItem);
			//	}
			//}
		}
	}
	catch(...)
	{

	}

	*pResult = 0;
}

void CDlgPageServer::OnNMRClickTreeServer(NMHDR *pNMHDR, LRESULT *pResult)
{
	POINT pt;
	::GetCursorPos(&pt);

	CWnd *pServerTreeWnd = GetDlgItem(IDC_TREE_SERVER);

	UINT hit_flag;
	HTREEITEM h_item = NULL;

	if(pServerTreeWnd != NULL&&pServerTreeWnd->IsWindowVisible())
	{
		m_trServer.ScreenToClient(&pt);

		// 查找点中的树结点项目
		h_item = m_trServer.HitTest(pt, &hit_flag);
		if (h_item == NULL)
			return;

		if ((hit_flag & TVHT_ONITEMICON) == 0 && (hit_flag & TVHT_ONITEMLABEL) == 0)
			return;

		m_trServer.SelectItem(h_item);
	}

	CMenu  popMenu;

	popMenu.LoadMenu(IDR_MENU_PAGESERVER_TREELIST);
	CMenu *pMenu = popMenu.GetSubMenu(0); 

	CPoint posMouse;
	GetCursorPos(&posMouse);

	pMenu->TrackPopupMenu(TPM_RIGHTBUTTON | TPM_RIGHTALIGN,  posMouse.x, posMouse.y, this); 
}

void CDlgPageServer::OnMenuitemReflashlist()
{
	ReadDBSystemInfo();// 树状列表初始化

	if (g_nUserResourceShowType == 1)
	{
#if VM_QABD_CLIENT_VERSION
		InitCameraTreelistQABD();// 创建摄像头列表树
#else
		InitCameraTreelist();// 创建摄像头列表树
#endif
		InitPresetList();		 // 创建预置位列表树
	}
	else if (g_nUserResourceShowType == 2)
	{
		InitCameraTreelistByCity();//创建摄像头列表树
		InitPresetListByCity();		 // 创建预置位列表树
	}
	else if (g_nUserResourceShowType == 3)
	{
		InitCameraTreelistByVoltage();//创建摄像头列表树
		InitPresetListByVoltage();		 // 创建预置位列表树
	}

	InitTuneCycleList();	 // 创建轮巡列表树
}

void CDlgPageServer::SetCameraNodeInfo(HTREEITEM hItem, char* ItemName, char* Itemnum, int StationOrCameraID,int ItemStatus,int Streamless,float ItemLongitude, float ItemLatitude, int ItemDecodeTag, int Itemtype, char* stationname)
{
	_T_NODE_INFO *pNodeInfo = NULL;

	try
	{
		pNodeInfo = new _T_NODE_INFO;
		memset(pNodeInfo, 0, sizeof(_T_NODE_INFO));

		sprintf (pNodeInfo->node_name, "%s", ItemName);
		sprintf(pNodeInfo->node_num, "%s", Itemnum);

		pNodeInfo->node_id = StationOrCameraID;
		pNodeInfo->node_status = ItemStatus;
		pNodeInfo->node_streamless = Streamless;
		pNodeInfo->node_longitude = ItemLongitude;
		pNodeInfo->node_latitude = ItemLatitude;
		pNodeInfo->node_type = Itemtype;

		if (ItemDecodeTag == 1)
		{
			ItemDecodeTag = 150;
		}

		if (Itemtype == 2)//站点
		{
			//记录第一个站点名称
			if (strlen(g_VideoPageViewInfo.szFirstStationName) == 0)
			{
				strcpy_s(g_VideoPageViewInfo.szFirstStationName,sizeof(g_VideoPageViewInfo.szFirstStationName)-1,ItemName);
			}
		}

		pNodeInfo->node_decodetag = ItemDecodeTag;
		sprintf(pNodeInfo->node_station, "%s", stationname);

		m_trServer.SetItemData(hItem, (DWORD_PTR)pNodeInfo);
	}
	catch(...)
	{

	}
}

void CDlgPageServer::SetPresetNodeInfo(HTREEITEM hItem, char* ItemName, char* Itemnum, int StationOrCameraID, char* PresetName, int PresetID, int ItemStatus,int Streamless,float ItemLongitude, float ItemLatitude, int ItemDecodeTag, int Itemtype, char* stationname,
									   int line1_from_x,int line1_from_y,int line1_to_x,int line1_to_y, int line2_from_x,int line2_from_y,int line2_to_x,int line2_to_y)
{
	_T_NODE_PRESET_INFO *pNodeInfo = NULL;

	try
	{
		pNodeInfo = new _T_NODE_PRESET_INFO;
		memset(pNodeInfo, 0, sizeof(_T_NODE_PRESET_INFO));

		sprintf (pNodeInfo->node_name, "%s", ItemName);
		sprintf(pNodeInfo->node_num, "%s", Itemnum);
		pNodeInfo->node_id = StationOrCameraID;
		sprintf(pNodeInfo->preset_name, "%s", PresetName);
		pNodeInfo->preset_id = PresetID;
		pNodeInfo->node_status = ItemStatus;
		pNodeInfo->node_streamless = Streamless;
		pNodeInfo->node_longitude = ItemLongitude;
		pNodeInfo->node_latitude = ItemLatitude;
		pNodeInfo->node_type = Itemtype;

		pNodeInfo->line1_from_x = line1_from_x;
		pNodeInfo->line1_from_y = line1_from_y;
		pNodeInfo->line1_to_x = line1_to_x;
		pNodeInfo->line1_to_y = line1_to_y;
		pNodeInfo->line2_from_x = line2_from_x;
		pNodeInfo->line2_from_y = line2_from_y;
		pNodeInfo->line2_to_x = line2_to_x;
		pNodeInfo->line2_to_y = line2_to_y;

		if (ItemDecodeTag == 1)
		{
			ItemDecodeTag = 150;
		}

		pNodeInfo->node_decodetag = ItemDecodeTag;
		sprintf(pNodeInfo->node_station, "%s", stationname);

		m_trPreset.SetItemData(hItem, (DWORD_PTR)pNodeInfo);
	}
	catch(...)
	{

	}
}

void CDlgPageServer::ReadDBSystemInfo()
{

	try
	{
		bool result = false;

		char sql_buf[1024] = {0};

		MYSQL_RES	* res ;
		MYSQL_ROW	row ;

		// 读取当前视频平台数量及信息，不读上级平台
		g_TotalSysNum = 0;
		sprintf_s(sql_buf, "SELECT system_id, system_name, cms_ip, cms_port, httpserver_ip, httpserver_port,"
			" system_status, up_system_id, local_domain FROM sm_d5000_plant_system where local_domain<>2 ORDER BY system_id");

		if (!mysql_query(g_mySqlData, sql_buf))
		{
			res = mysql_store_result(g_mySqlData);

			while ( row = mysql_fetch_row( res ) )
			{
				sprintf_s(g_SystemInfo[g_TotalSysNum].sysid, "%s", row[0]);
				sprintf_s(g_SystemInfo[g_TotalSysNum].sysname, "%s", row[1]);
				sprintf_s(g_SystemInfo[g_TotalSysNum].cms_ip, "%s", row[2]);
				g_SystemInfo[g_TotalSysNum].cms_port = atoi(row[3]);
				sprintf_s(g_SystemInfo[g_TotalSysNum].http_ip, "%s", row[4]);
				g_SystemInfo[g_TotalSysNum].http_port = atoi(row[5]);
				g_SystemInfo[g_TotalSysNum].sys_status = atoi(row[6]);
				sprintf_s(g_SystemInfo[g_TotalSysNum].upsysid, "%s", row[7]);
				g_SystemInfo[g_TotalSysNum].localdomain = atoi(row[8]);

				g_TotalSysNum ++;
			}
			mysql_free_result( res ) ;
		}

		// 读取变电站的节点信息
		g_nStationNode_Num = 0;
		sprintf_s(sql_buf, "SELECT node_id,parent_id,node_name,order_num,ywb_id,login_type,voltage_class FROM ob_d5000_node_relation ORDER by node_id ASC");
		if (!mysql_query(g_mySqlData, sql_buf))
		{
			res = mysql_store_result(g_mySqlData);

			while (row = mysql_fetch_row(res))
			{
				g_tStationNode_Info[g_nStationNode_Num].node_id = atoi(row[0]);
				g_tStationNode_Info[g_nStationNode_Num].parent_id = atoi(row[1]);
				sprintf_s(g_tStationNode_Info[g_nStationNode_Num].node_name, "%s", row[2]);
				g_tStationNode_Info[g_nStationNode_Num].order_num = atoi(row[3]);
				g_tStationNode_Info[g_nStationNode_Num].ywb_id = atoi(row[4]);
				g_tStationNode_Info[g_nStationNode_Num].login_type = atoi(row[5]);
				g_tStationNode_Info[g_nStationNode_Num].voltage_class = atoi(row[6]);

				g_nStationNode_Num ++;
				if (g_nStationNode_Num >= MAX_STATION_NODE_NUM)
				{
					MessageBox("系统最大支持的结点数为2000个，超过部分无法进行展示","热点视频监视");
					break;
				}
			}
			mysql_free_result(res) ;
		}

		// 读取变电站信息
		g_nStation_Num = 0;
		sprintf_s(sql_buf, "SELECT station_id,station_name_videoplant,station_code_videoplant,station_name_d5000,station_code_d5000,"
			" station_name_oms,station_code_oms,station_name_meteo_a,station_code_meteo_a,station_code_weather,station_code_typhoon,station_code_microclimate, station_video_status,voltage_class, node_id, node_yw_id,node_sd_yw_id,notes FROM ob_d5000_station WHERE voltage_class=500 ORDER BY order_num ASC");
		if (!mysql_query(g_mySqlData, sql_buf))
		{
			res = mysql_store_result(g_mySqlData);

			while (row = mysql_fetch_row(res))
			{
				if (g_nStation_Num >= MAX_STATION_NUM)
					break;

				g_tStation_Info[g_nStation_Num].station_id = atoi(row[0]);
				sprintf_s(g_tStation_Info[g_nStation_Num].station_name_videoplant, "%s", row[1]);
				sprintf_s(g_tStation_Info[g_nStation_Num].station_code_videoplant, "%s", row[2]);
				sprintf_s(g_tStation_Info[g_nStation_Num].station_name_d5000, "%s", row[3]);
				sprintf_s(g_tStation_Info[g_nStation_Num].station_code_d5000, "%s", row[4]);
				sprintf_s(g_tStation_Info[g_nStation_Num].station_name_oms, "%s", row[5]);
				sprintf_s(g_tStation_Info[g_nStation_Num].station_code_oms, "%s", row[6]);
				sprintf_s(g_tStation_Info[g_nStation_Num].station_name_meteo_a, "%s", row[7]);
				sprintf_s(g_tStation_Info[g_nStation_Num].station_code_meteo_a, "%s", row[8]);
				sprintf_s(g_tStation_Info[g_nStation_Num].station_code_weather,"%s",row[9]);
				sprintf_s(g_tStation_Info[g_nStation_Num].station_code_typhoon,"%s",row[10]);
				sprintf_s(g_tStation_Info[g_nStation_Num].station_code_microclimate,"%s",row[11]);
				g_tStation_Info[g_nStation_Num].station_video_status = atoi(row[12]);
				g_tStation_Info[g_nStation_Num].voltage_class = atoi(row[13]);
				g_tStation_Info[g_nStation_Num].node_id = atoi(row[14]);
				g_tStation_Info[g_nStation_Num].node_yw_id = atoi(row[15]);
				g_tStation_Info[g_nStation_Num].node_sd_yw_id = atoi(row[16]);
				sprintf_s(g_tStation_Info[g_nStation_Num].notes,"%s",row[17]);

				g_nStation_Num ++;
			}
			mysql_free_result(res) ;
		}

		sprintf_s(sql_buf, "SELECT station_id,station_name_videoplant,station_code_videoplant,station_name_d5000,station_code_d5000,"
			" station_name_oms,station_code_oms,station_name_meteo_a,station_code_meteo_a,station_code_weather,station_code_typhoon,station_code_microclimate, station_video_status,voltage_class, node_id, node_yw_id,node_sd_yw_id,notes FROM ob_d5000_station WHERE voltage_class=220 ORDER BY order_num ASC");
		if (!mysql_query(g_mySqlData, sql_buf))
		{
			res = mysql_store_result(g_mySqlData);

			while (row = mysql_fetch_row(res))
			{
				if (g_nStation_Num >= MAX_STATION_NUM)
					break;

				g_tStation_Info[g_nStation_Num].station_id = atoi(row[0]);
				sprintf_s(g_tStation_Info[g_nStation_Num].station_name_videoplant, "%s", row[1]);
				sprintf_s(g_tStation_Info[g_nStation_Num].station_code_videoplant, "%s", row[2]);
				sprintf_s(g_tStation_Info[g_nStation_Num].station_name_d5000, "%s", row[3]);
				sprintf_s(g_tStation_Info[g_nStation_Num].station_code_d5000, "%s", row[4]);
				sprintf_s(g_tStation_Info[g_nStation_Num].station_name_oms, "%s", row[5]);
				sprintf_s(g_tStation_Info[g_nStation_Num].station_code_oms, "%s", row[6]);
				sprintf_s(g_tStation_Info[g_nStation_Num].station_name_meteo_a, "%s", row[7]);
				sprintf_s(g_tStation_Info[g_nStation_Num].station_code_meteo_a, "%s", row[8]);
				sprintf_s(g_tStation_Info[g_nStation_Num].station_code_weather,"%s",row[9]);
				sprintf_s(g_tStation_Info[g_nStation_Num].station_code_typhoon,"%s",row[10]);
				sprintf_s(g_tStation_Info[g_nStation_Num].station_code_microclimate,"%s",row[11]);
				g_tStation_Info[g_nStation_Num].station_video_status = atoi(row[12]);
				g_tStation_Info[g_nStation_Num].voltage_class = atoi(row[13]);
				g_tStation_Info[g_nStation_Num].node_id = atoi(row[14]);
				g_tStation_Info[g_nStation_Num].node_yw_id = atoi(row[15]);
				g_tStation_Info[g_nStation_Num].node_sd_yw_id = atoi(row[16]);
				sprintf_s(g_tStation_Info[g_nStation_Num].notes,"%s",row[17]);

				g_nStation_Num ++;
			}
			mysql_free_result(res) ;
		}

		sprintf_s(sql_buf, "SELECT station_id,station_name_videoplant,station_code_videoplant,station_name_d5000,station_code_d5000,"
			" station_name_oms,station_code_oms,station_name_meteo_a,station_code_meteo_a,station_code_weather,station_code_typhoon,station_code_microclimate, station_video_status,voltage_class, node_id, node_yw_id,node_sd_yw_id,notes FROM ob_d5000_station WHERE voltage_class!=500 AND voltage_class!=220 ORDER BY order_num ASC");
		if (!mysql_query(g_mySqlData, sql_buf))
		{
			res = mysql_store_result(g_mySqlData);

			while (row = mysql_fetch_row(res))
			{
				if (g_nStation_Num >= MAX_STATION_NUM)
					break;

				g_tStation_Info[g_nStation_Num].station_id = atoi(row[0]);
				sprintf_s(g_tStation_Info[g_nStation_Num].station_name_videoplant, "%s", row[1]);
				sprintf_s(g_tStation_Info[g_nStation_Num].station_code_videoplant, "%s", row[2]);
				sprintf_s(g_tStation_Info[g_nStation_Num].station_name_d5000, "%s", row[3]);
				sprintf_s(g_tStation_Info[g_nStation_Num].station_code_d5000, "%s", row[4]);
				sprintf_s(g_tStation_Info[g_nStation_Num].station_name_oms, "%s", row[5]);
				sprintf_s(g_tStation_Info[g_nStation_Num].station_code_oms, "%s", row[6]);
				sprintf_s(g_tStation_Info[g_nStation_Num].station_name_meteo_a, "%s", row[7]);
				sprintf_s(g_tStation_Info[g_nStation_Num].station_code_meteo_a, "%s", row[8]);
				sprintf_s(g_tStation_Info[g_nStation_Num].station_code_weather,"%s",row[9]);
				sprintf_s(g_tStation_Info[g_nStation_Num].station_code_typhoon,"%s",row[10]);
				sprintf_s(g_tStation_Info[g_nStation_Num].station_code_microclimate,"%s",row[11]);
				g_tStation_Info[g_nStation_Num].station_video_status = atoi(row[12]);
				g_tStation_Info[g_nStation_Num].voltage_class = atoi(row[13]);
				g_tStation_Info[g_nStation_Num].node_id = atoi(row[14]);
				g_tStation_Info[g_nStation_Num].node_yw_id = atoi(row[15]);
				g_tStation_Info[g_nStation_Num].node_sd_yw_id = atoi(row[16]);
				sprintf_s(g_tStation_Info[g_nStation_Num].notes,"%s",row[17]);

				g_nStation_Num ++;

			}
			mysql_free_result(res) ;
		}

		if (g_nStation_Num >= MAX_STATION_NUM)
		{
			MessageBox("系统最大支持的站点数为4000个，超过部分无法进行展示","热点视频监视");
		}
	}
	catch(...)
	{

	}
}

BOOL CDlgPageServer::ShowControl(int nFormNum)
{
	SetWindowPos(&wndTop,0,MENUHIGHT,0,0,SWP_NOSIZE);

	m_btnShowHide.SetIndex(1);
	m_btnShowHide.Invalidate(TRUE);

	CWnd *pWnd = NULL;

	pWnd = GetDlgItem(IDC_BUTTON_CHOOSEYT);
	if (pWnd != NULL)
	{
		pWnd->ShowWindow(SW_SHOW);
	}

	pWnd = GetDlgItem(IDC_BUTTON_CHOOSEVIEW);
	if (pWnd != NULL)
	{
		pWnd->ShowWindow(SW_SHOW);
	}

	pWnd = GetDlgItem(IDC_BUTTON_PS_DEVICELIST);
	if (pWnd != NULL)
	{
		pWnd->ShowWindow(SW_SHOW);
	}

	pWnd = GetDlgItem(IDC_BUTTON_PS_CAMERALIST);
	if (pWnd != NULL)
	{
		pWnd->ShowWindow(SW_SHOW);
	}

	pWnd = GetDlgItem(IDC_BUTTON_PS_CYCLE_LIST);
	if (pWnd != NULL)
	{
		pWnd->ShowWindow(SW_SHOW);
	}

	pWnd = GetDlgItem(IDC_BUTTON_PS_FORM1);
	if (pWnd != NULL)
	{
		pWnd->ShowWindow(SW_SHOW);
	}

	pWnd = GetDlgItem(IDC_BUTTON_PS_FORM4);
	if (pWnd != NULL)
	{
		pWnd->ShowWindow(SW_SHOW);
	}

	pWnd = GetDlgItem(IDC_BUTTON_PS_FORM6);
	if (pWnd != NULL)
	{
		pWnd->ShowWindow(SW_SHOW);
	}

	pWnd = GetDlgItem(IDC_BUTTON_PS_FORM9);
	if (pWnd != NULL)
	{
		pWnd->ShowWindow(SW_SHOW);
	}

	pWnd = GetDlgItem(IDC_BUTTON_PS_FORM16);
	if (pWnd != NULL)
	{
		pWnd->ShowWindow(SW_SHOW);
	}

	g_pMainDlg->PostMessage(OM_CONTROLBUTTON, IDC_BUTTON_CHOOSEYT);
	g_pMainDlg->PostMessage(OM_CONTROLBUTTON, IDC_BUTTON_PS_CAMERALIST);

	Invalidate(TRUE);

	switch(nFormNum)
	{
	case 1:
		{
			if (g_pMainDlg != NULL)
				g_pMainDlg->PostMessage(OM_CONTROLBUTTON, IDC_BUTTON_PS_FORM1);
		}
		break;
	case 4:
		{
			if (g_pMainDlg != NULL)
				g_pMainDlg->PostMessage(OM_CONTROLBUTTON, IDC_BUTTON_PS_FORM4);
		}
		break;
	case 6:
		{
			if (g_pMainDlg != NULL)
				g_pMainDlg->PostMessage(OM_CONTROLBUTTON, IDC_BUTTON_PS_FORM6);
		}
		break;
	case 9:
		{
			if (g_pMainDlg != NULL)
				g_pMainDlg->PostMessage(OM_CONTROLBUTTON, IDC_BUTTON_PS_FORM9);
		}
		break;
	case  16:
		{
			if (g_pMainDlg != NULL)
				g_pMainDlg->PostMessage(OM_CONTROLBUTTON, IDC_BUTTON_PS_FORM16);
		}
		break;
	default:
		break;
	}

	return TRUE;
}

BOOL CDlgPageServer::HideControl(int nFormNum)
{
	SetWindowPos(&wndTop,-(CONTROLWIDTH-20),MENUHIGHT,0,0,SWP_NOSIZE);

	m_btnShowHide.SetIndex(0);
	m_btnShowHide.Invalidate(TRUE);

	CWnd *pWnd = NULL;

	pWnd = GetDlgItem(IDC_BUTTON_CHOOSEYT);
	if (pWnd != NULL)
	{
		pWnd->ShowWindow(SW_HIDE);
	}

	pWnd = GetDlgItem(IDC_BUTTON_CHOOSEVIEW);
	if (pWnd != NULL)
	{
		pWnd->ShowWindow(SW_HIDE);
	}

	pWnd = GetDlgItem(IDC_BUTTON_PS_DEVICELIST);
	if (pWnd != NULL)
	{
		pWnd->ShowWindow(SW_HIDE);
	}

	pWnd = GetDlgItem(IDC_BUTTON_PS_CAMERALIST);
	if (pWnd != NULL)
	{
		pWnd->ShowWindow(SW_HIDE);
	}

	pWnd = GetDlgItem(IDC_BUTTON_PS_CYCLE_LIST);
	if (pWnd != NULL)
	{
		pWnd->ShowWindow(SW_HIDE);
	}

	pWnd = GetDlgItem(IDC_BUTTON_PS_FORM1);
	if (pWnd != NULL)
	{
		pWnd->ShowWindow(SW_HIDE);
	}

	pWnd = GetDlgItem(IDC_BUTTON_PS_FORM4);
	if (pWnd != NULL)
	{
		pWnd->ShowWindow(SW_HIDE);
	}

	pWnd = GetDlgItem(IDC_BUTTON_PS_FORM6);
	if (pWnd != NULL)
	{
		pWnd->ShowWindow(SW_HIDE);
	}

	pWnd = GetDlgItem(IDC_BUTTON_PS_FORM9);
	if (pWnd != NULL)
	{
		pWnd->ShowWindow(SW_HIDE);
	}

	pWnd = GetDlgItem(IDC_BUTTON_PS_FORM16);
	if (pWnd != NULL)
	{
		pWnd->ShowWindow(SW_HIDE);
	}

	// 列表区域
	pWnd = GetDlgItem(IDC_TREE_SERVER);
	if (pWnd != NULL)
	{
		pWnd->ShowWindow(SW_HIDE);
	}

	pWnd = GetDlgItem(IDC_TREE_PRESET);
	if (pWnd != NULL)
	{
		pWnd->ShowWindow(SW_HIDE);
	}

	pWnd = GetDlgItem(IDC_TREE_TUNECYCLE);
	if (pWnd != NULL)
	{
		pWnd->ShowWindow(SW_HIDE);
	}

	pWnd = GetDlgItem(IDC_LIST_PS_RELATION);
	if (pWnd != NULL)
	{
		pWnd->ShowWindow(SW_HIDE);
	}

	pWnd = GetDlgItem(IDC_BUTTON_RELATION_SHOW);
	if (pWnd != NULL)
	{
		pWnd->ShowWindow(SW_HIDE);
	}

	// 云镜控制区域
	pWnd = GetDlgItem(IDC_BUTTON_CHOOSEYT);
	if (pWnd != NULL)
	{
		pWnd->ShowWindow(SW_HIDE);
	}

	pWnd = GetDlgItem(IDC_BUTTON_CHOOSEVIEW);
	if (pWnd != NULL)
	{
		pWnd->ShowWindow(SW_HIDE);
	}

	pWnd = GetDlgItem(IDC_YT_ZOOMIN);
	if (pWnd != NULL)
	{
		pWnd->ShowWindow(SW_HIDE);
	}

	pWnd = GetDlgItem(IDC_YT_ZOOMOUT);
	if (pWnd != NULL)
	{
		pWnd->ShowWindow(SW_HIDE);
	}

	pWnd = GetDlgItem(IDC_YT_LEFT);
	if (pWnd != NULL)
	{
		pWnd->ShowWindow(SW_HIDE);
	}

	pWnd = GetDlgItem(IDC_YT_UP);
	if (pWnd != NULL)
	{
		pWnd->ShowWindow(SW_HIDE);
	}

	pWnd = GetDlgItem(IDC_YT_DOWN);
	if (pWnd != NULL)
	{
		pWnd->ShowWindow(SW_HIDE);
	}

	pWnd = GetDlgItem(IDC_YT_RIGHT);
	if (pWnd != NULL)
	{
		pWnd->ShowWindow(SW_HIDE);
	}

	pWnd = GetDlgItem(IDC_YT_SAVE);
	if (pWnd != NULL)
	{
		pWnd->ShowWindow(SW_HIDE);
	}

	pWnd = GetDlgItem(IDC_YT_CALL);
	if (pWnd != NULL)
	{
		pWnd->ShowWindow(SW_HIDE);
	}

	pWnd = GetDlgItem(IDC_YT_DIAPHRAGM_LARGE);
	if (pWnd != NULL)
	{
		pWnd->ShowWindow(SW_HIDE);
	}

	pWnd = GetDlgItem(IDC_YT_DIAPHRAGM_SMALL);
	if (pWnd != NULL)
	{
		pWnd->ShowWindow(SW_HIDE);
	}

	pWnd = GetDlgItem(IDC_YT_RESURVE1);
	if (pWnd != NULL)
	{
		pWnd->ShowWindow(SW_HIDE);
	}

	pWnd = GetDlgItem(IDC_YT_RESURVE2);
	if (pWnd != NULL)
	{
		pWnd->ShowWindow(SW_HIDE);
	}

	pWnd = GetDlgItem(IDC_COMBO_SPEED);
	if (pWnd != NULL)
	{
		pWnd->ShowWindow(SW_HIDE);
	}

	pWnd = GetDlgItem(IDC_STATIC_SPEED);
	if (pWnd != NULL)
	{
		pWnd->ShowWindow(SW_HIDE);
	}

	pWnd = GetDlgItem(IDC_YT_LOCK);
	if (pWnd != NULL)
	{
		pWnd->ShowWindow(SW_HIDE);
	}

	Invalidate(TRUE);

	switch(nFormNum)
	{
	case 1:
		{
			if (g_pMainDlg != NULL)
				g_pMainDlg->PostMessage(OM_CONTROLBUTTON, IDC_BUTTON_PS_FORM1);
		}
		break;
	case 4:
		{
			if (g_pMainDlg != NULL)
				g_pMainDlg->PostMessage(OM_CONTROLBUTTON, IDC_BUTTON_PS_FORM4);
		}
		break;
	case 6:
		{
			if (g_pMainDlg != NULL)
				g_pMainDlg->PostMessage(OM_CONTROLBUTTON, IDC_BUTTON_PS_FORM6);
		}
		break;
	case 9:
		{
			if (g_pMainDlg != NULL)
				g_pMainDlg->PostMessage(OM_CONTROLBUTTON, IDC_BUTTON_PS_FORM9);
		}
		break;
	case  16:
		{
			if (g_pMainDlg != NULL)
				g_pMainDlg->PostMessage(OM_CONTROLBUTTON, IDC_BUTTON_PS_FORM16);
		}
		break;
	default:
		break;
	}

	return TRUE;
}

//在摄像头树形列表中加载指定站点下面的摄像头
BOOL CDlgPageServer::CameraTreelistAddCameraByStastion(int nStationId)
{
	int i = 0;
	char szStationNum[64] = {0};
	HTREEITEM hStationTreeItem = NULL;

	try
	{
		if (nStationId <= 0)
			return FALSE;

		for (i = 0;;i++)
		{
			if (i >= g_nStation_Num)
				return FALSE;

			if(g_tStation_Info[i].station_id == nStationId)
				break;
		}

		strcpy_s(szStationNum,sizeof(szStationNum),g_tStation_Info[i].station_code_videoplant);

		hStationTreeItem = SearchCameraTreeStationHandleTreeItem(NULL,szStationNum,NULL);
		if (hStationTreeItem == NULL)
			return FALSE;

		return OnTreeServerStationFlashInfo(hStationTreeItem);
	}
	catch(...)
	{

	}
	return FALSE;
}

BOOL CDlgPageServer::CameraTreelistAddCameraByStastion(char *szStationName)
{
	int i = 0;
	int nStationId = 0;
	char szStationNum[64] = {0};
	HTREEITEM hStationTreeItem = NULL;

	try
	{
		if (szStationName == NULL||strlen(szStationName) == 0)
			return FALSE;

		for (i = 0;;i++)
		{
			if (i >= g_nStation_Num)
				return FALSE;

			if(strcmp(g_tStation_Info[i].station_name_videoplant,szStationName) == 0)
				break;
		}

		nStationId = g_tStation_Info[i].station_id;
		strcpy_s(szStationNum,sizeof(szStationNum),g_tStation_Info[i].station_code_videoplant);

		hStationTreeItem = SearchCameraTreeStationHandleTreeItem(NULL,szStationNum,NULL);
		if (hStationTreeItem == NULL)
			return FALSE;

		return OnTreeServerStationFlashInfo(hStationTreeItem);

	}
	catch(...)
	{

	}

	return FALSE;
}

BOOL CDlgPageServer::PresetTreelistAddPresetByStastion(char *szStationName)
{
	int i = 0;
	int nStationId = 0;
	char szStationNum[64] = {0};
	char sql_buf[1024] = {0};

	MYSQL_RES    * res = NULL ;
	MYSQL_ROW	row ;

	_T_NODE_PRESET_INFO	*pPresetNodeInfo = NULL;
	HTREEITEM hStationTreeItem = NULL;
	HTREEITEM hInsertItem = NULL;
	HTREEITEM hSelectItem = NULL;

	try
	{
		if (szStationName == NULL||strlen(szStationName) == 0)
			return FALSE;

		for (i = 0;;i++)
		{
			if (i >= g_nStation_Num)
				return FALSE;

			if(strcmp(g_tStation_Info[i].station_name_videoplant,szStationName) == 0)
				break;
		}

		nStationId = g_tStation_Info[i].station_id;
		strcpy_s(szStationNum,sizeof(szStationNum),g_tStation_Info[i].station_code_videoplant);

		hStationTreeItem = SearchPresetTreeStationHandleTreeItem(NULL,szStationNum,NULL);
		if (hStationTreeItem == NULL)
			return FALSE;

		// 判断节点是何种类型节点
		pPresetNodeInfo = (_T_NODE_PRESET_INFO *)(m_trPreset.GetItemData(hStationTreeItem));

		if (pPresetNodeInfo == NULL||pPresetNodeInfo->node_type != 2)
			return FALSE;

		if (m_trPreset.ItemHasChildren(hStationTreeItem) == FALSE)
		{
			hInsertItem = m_trPreset.InsertItem("区域", 7, 7, hStationTreeItem);
			SetPresetNodeInfo(hInsertItem, "区域", "00000", pPresetNodeInfo->node_id, NULL, 0,
				1,
				0,
				0,
				0,
				0,
				6,
				pPresetNodeInfo->node_station,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0
				);

			hInsertItem = m_trPreset.InsertItem("设备", 7, 7, hStationTreeItem);
			SetPresetNodeInfo(hInsertItem, "设备", "00000", pPresetNodeInfo->node_id, NULL, 0,
				1,
				0,
				0,
				0,
				0,
				7,
				pPresetNodeInfo->node_station,
				0,
				0,
				0,
				0,
				0,
				0,
				0,
				0
				);
		}

		hSelectItem = m_trPreset.GetChildItem(hStationTreeItem);

		while(hSelectItem != NULL)
		{
			pPresetNodeInfo = (_T_NODE_PRESET_INFO*)m_trPreset.GetItemData(hSelectItem);
			if (pPresetNodeInfo != NULL&&pPresetNodeInfo->node_type == 7)
				break;

			hSelectItem = m_trPreset.GetNextSiblingItem(hSelectItem);
		}

		hInsertItem = NULL;
		if (hSelectItem != NULL&&pPresetNodeInfo->node_type == 7)//设备组
		{
			if (m_trPreset.ItemHasChildren(hSelectItem))
				return TRUE;

			int nCount = 0;
			int nCameraStatus = 1;
			int nStreamless = 0;

			// 表示该节点为变电站的设备组，获取数据库中的信息，并将预置位信息插入到数据表中
			sprintf_s(sql_buf, "SELECT t1.camera_id, t1.camera_code, t1.camera_name, t1.camera_type, t1.camera_preset_id, t1.preset_name,t1.from_x_1,t1.from_y_1,t1.to_x_1,t1.to_y_1,t1.from_x_2,t1.from_y_2,t1.to_x_2,t1.to_y_2,t2.camera_status,t2.streamless "
				" FROM ob_d5000_camerapreset_station_%d AS t1,ob_d5000_camera_station_%d AS t2 WHERE t1.camera_code=t2.camera_code ORDER BY t1.preset_name ASC",m_pPresetNodeInfo->node_id,m_pPresetNodeInfo->node_id);

			if (!mysql_query(g_mySqlData, sql_buf))
			{
				res = mysql_store_result(g_mySqlData);

				while ( row = mysql_fetch_row( res ) )
				{
					nCameraStatus = atoi(row[14]);
					nStreamless = atoi(row[15]);

					if (nCameraStatus == 0)
					{
						hInsertItem = m_trPreset.InsertItem(row[5], 4, 4, hSelectItem);
					}
					else if (nCameraStatus == 1)
					{
						if (nStreamless == 1)
						{
							hInsertItem = m_trPreset.InsertItem(row[5], 4, 4, hSelectItem);
						}
						else
						{
							hInsertItem = m_trPreset.InsertItem(row[5], 5, 5, hSelectItem);
						}
					}
					else if (nCameraStatus == 2)
					{
						if (nStreamless == 1)
						{
							hInsertItem = m_trPreset.InsertItem(row[5], 11, 11, hSelectItem);
						}
						else
						{
							hInsertItem = m_trPreset.InsertItem(row[5], 9, 9, hSelectItem);
						}
					}
					else if (nCameraStatus == 3)
					{
						hInsertItem = m_trPreset.InsertItem(row[5], 11, 11, hSelectItem);
					}
					else
					{
						hInsertItem = m_trPreset.InsertItem(row[5], 5, 5, hSelectItem);
					}

					if (hInsertItem == NULL)
						continue;

					SetPresetNodeInfo(hInsertItem, row[2], row[1], pPresetNodeInfo->node_id, row[5], atoi(row[4]),
						nCameraStatus,
						nStreamless,
						0,
						0,
						atoi(row[3]),
						5,
						pPresetNodeInfo->node_station,
						atoi(row[6]),
						atoi(row[7]),
						atoi(row[8]),
						atoi(row[9]),
						atoi(row[10]),
						atoi(row[11]),
						atoi(row[12]),
						atoi(row[13])
						);

					nCount++;
				}
				mysql_free_result( res ) ;
				res = NULL;
			}

			//设置摄像头DVR信息
			SetTreePresetStationCameraDvrInfo(hSelectItem);
			return TRUE;
		}
		return FALSE;
	}
	catch(...)
	{

	}

	return FALSE;
}

int CDlgPageServer::CheckStationCurrentStatus(int nStationId)
{
	int nStationStatus = 0;
	int nCount = 0;

	char sql_buf[512] = {0};

	MYSQL_RES    * res = NULL ;
	MYSQL_ROW	row ;

	try
	{
		sprintf_s(sql_buf, "SELECT COUNT(camera_id) FROM ob_d5000_camera_station_%d where camera_status<>0",nStationId);

		if (!mysql_query(g_mySqlData, sql_buf))
		{
			res = mysql_store_result(g_mySqlData);

			while(row = mysql_fetch_row( res ))
			{
				nCount = atoi(row[0]);
			}

			mysql_free_result( res ) ;
		}

		nStationStatus = (nCount > 0)?1:0;
	}
	catch(...)
	{

	}

	return nStationStatus;
}

int CDlgPageServer::CheckStationCurrentStatus2(int nStationId)
{
	int nStationStatus = 1;

	try
	{
		for (int i = 0;i < g_nStation_Num;i++)
		{
			if (g_tStation_Info[i].station_id == nStationId)
			{
				if (g_tStation_Info[i].station_video_status == 0)
					nStationStatus = 0;
			}
		}
		return nStationStatus;
	}
	catch(...)
	{

	}
	return nStationStatus;
}

BOOL CDlgPageServer::ReadAndSetRelationListInfo(char *szLinkageTime)
{

	try
	{
		if (szLinkageTime == NULL)
			return FALSE;

		m_RealRelationList.DeleteAllItems();

		char sql_buf[255] = {0};
		bool result=false;

		MYSQL_RES * res = NULL ;
		MYSQL_ROW	row ;

		char szYear[8] = {0};
		char szMonth[4] = {0};
		int nYear = 2013;
		int nMonth = 1;

		int nLinkageType = 0;
		char szLinkageType[64] = {0};
		char szStationName[256] = {0};
		char szDeviceName[256] = {0};
		char szSubType[64] = {0};
		char szScreenId[64] = {0};
		char szLinkageRealTime[64] = {0};
		char szContent[256] = {0};
		char szValue1[64] = {0};
		char szValue2[64] = {0};
		char szLinkageStationName[256] = {0};
		char szLinkageNum[16] = {0};

		szYear[0] = szLinkageTime[0];
		szYear[1] = szLinkageTime[1];
		szYear[2] = szLinkageTime[2];
		szYear[3] = szLinkageTime[3];
		szYear[4] = 0;

		nYear = atoi(szYear);

		szMonth[0] = szLinkageTime[5];
		szMonth[1] = szLinkageTime[6];

		nMonth = atoi(szMonth);

		//读联动信息
		sprintf_s(sql_buf, "SELECT linkage_type, station_name, device_name, sub_type, screen_id, linkage_time,"
			"content, value1,value2,linkage_station_name,linkage_num FROM ct_linkage_history_log_%s_%04d "
			"where linkage_time >= '%s' ORDER BY id",g_userpower.username,nYear,szLinkageTime);

		if (!mysql_query(g_mySqlData, sql_buf))
		{
			res = mysql_store_result(g_mySqlData);

			while ( row = mysql_fetch_row( res ) )
			{
				nLinkageType = atoi(row[0]);

				switch(nLinkageType)
				{
				case VIDEO_LINKAGE_INFO_TYPE:
					{
						sprintf_s(szLinkageType,"手动联动");
					}
					break;

				case VIDEO_ALARM_LINKAGE_INFO_TYPE:
					{
						sprintf_s(szLinkageType,"告警联动");
					}
					break;

				case VIDEO_STATE_LINKAGE_INFO_TYPE:
					{
						sprintf_s(szLinkageType,"状态联动");
					}
					break;

				default:
					{
						sprintf_s(szLinkageType,"未知");
					}
					break;
				}

				sprintf_s(szStationName, "%s", row[1]);
				sprintf_s(szDeviceName, "%s", row[2]);
				sprintf_s(szSubType,"%s",row[3]);
				sprintf_s(szScreenId,"%s",row[4]);
				sprintf_s(szLinkageRealTime,"%s",row[5]);
				sprintf_s(szContent,"%s",row[6]);
				sprintf_s(szValue1, "%s", row[7]);
				sprintf_s(szValue2,"%s",row[8]);
				sprintf_s(szLinkageStationName,"%s",row[9]);
				sprintf_s(szLinkageNum,"%s",row[10]);

				m_RealRelationList.InsertItem(0,szLinkageType);
				m_RealRelationList.SetItemText(0,1,szDeviceName);
				m_RealRelationList.SetItemText(0,2,szStationName);
				m_RealRelationList.SetItemText(0,3,szLinkageRealTime);
				m_RealRelationList.SetItemText(0,4,szSubType);
				m_RealRelationList.SetItemText(0,5,szScreenId);
				m_RealRelationList.SetItemText(0,6,szContent);
				m_RealRelationList.SetItemText(0,7,szValue1);
				m_RealRelationList.SetItemText(0,8,szValue2);
				m_RealRelationList.SetItemText(0,9,szLinkageStationName);
				m_RealRelationList.SetItemText(0,10,szLinkageNum);
			}
			mysql_free_result( res ) ;
		}

		return TRUE;
	}
	catch(...)
	{

	}
	return FALSE;
}

BOOL CDlgPageServer::ReadAndSetCameraRelationListInfo(int nNodeId,char *szStationName,char *szLinkageTime)
{

	try
	{
		if(nNodeId < 0)
			return FALSE;

		m_RealCameraRelationList.DeleteAllItems();

		char sql_buf[255] = {0};
		bool result=false;

		MYSQL_RES * res = NULL ;
		MYSQL_ROW	row ;

		char szYear[8] = {0};
		char szMonth[4] = {0};
		int nYear = 2013;
		int nMonth = 1;

		char szName[256] = {0};
		char szCode[256] = {0};
		char szDecodeTag[16] = {0};
		char szNodeId[16] = {0};

		szYear[0] = szLinkageTime[0];
		szYear[1] = szLinkageTime[1];
		szYear[2] = szLinkageTime[2];
		szYear[3] = szLinkageTime[3];
		szYear[4] = 0;

		nYear = atoi(szYear);

		szMonth[0] = szLinkageTime[5];
		szMonth[1] = szLinkageTime[6];

		nMonth = atoi(szMonth);

		//读摄像头联动信息
		sprintf_s(sql_buf, "SELECT name, code, decode_tag, node_id "
			"FROM ct_linkage_camera_history_log_%s_%04d "
			"where node_id=%d ORDER BY id",g_userpower.username,nYear,nNodeId);

		if (!mysql_query(g_mySqlData, sql_buf))
		{
			res = mysql_store_result(g_mySqlData);

			while ( row = mysql_fetch_row( res ) )
			{
				sprintf_s(szName, "%s", row[0]);
				sprintf_s(szCode, "%s", row[1]);
				sprintf_s(szDecodeTag,"%s",row[2]);
				sprintf_s(szNodeId,"%s",row[3]);

				m_RealCameraRelationList.InsertItem(0,szName);
				m_RealCameraRelationList.SetItemText(0,1,szCode);
				m_RealCameraRelationList.SetItemText(0,2,szDecodeTag);
				m_RealCameraRelationList.SetItemText(0,3,szStationName);
			}
			mysql_free_result( res ) ;
		}

		return TRUE;
	}
	catch(...)
	{

	}
	return FALSE;
}

int  CDlgPageServer::GetRelationListInfoId(char *szLinkageType,char *szStationName,char *szDeviceName,char *szSubType,char *szLinkageTime)
{
	try
	{
		char sql_buf[255] = {0};
		bool result=false;

		int nResult = -1;

		int nLinkageType = -1;

		MYSQL_RES * res = NULL ;
		MYSQL_ROW	row ;

		char szYear[8] = {0};
		char szMonth[4] = {0};
		int nYear = 2013;
		int nMonth = 1;

		szYear[0] = szLinkageTime[0];
		szYear[1] = szLinkageTime[1];
		szYear[2] = szLinkageTime[2];
		szYear[3] = szLinkageTime[3];
		szYear[4] = 0;

		nYear = atoi(szYear);

		szMonth[0] = szLinkageTime[5];
		szMonth[1] = szLinkageTime[6];

		nMonth = atoi(szMonth);

		if (strcmp(szLinkageType,"手动联动") == 0)
		{
			nLinkageType = VIDEO_LINKAGE_INFO_TYPE;
		}
		else if (strcmp(szLinkageType,"告警联动") == 0)
		{
			nLinkageType = VIDEO_ALARM_LINKAGE_INFO_TYPE;
		}
		else if (strcmp(szLinkageType,"状态联动") == 0)
		{
			nLinkageType = VIDEO_STATE_LINKAGE_INFO_TYPE;
		}
		else
		{
			nLinkageType = VIDEO_LINKAGE_INFO_UNDEFINE_TYPE;
		}

		sprintf_s(sql_buf, "SELECT id FROM ct_linkage_history_log_%s_%04d "
			"where linkage_type=%d AND station_name='%s' AND device_name='%s' AND sub_type='%s' AND linkage_time='%s' ORDER BY id",
			g_userpower.username,nYear,nLinkageType,szStationName,szDeviceName,szSubType,szLinkageTime);

		if (!mysql_query(g_mySqlData, sql_buf))
		{
			res = mysql_store_result(g_mySqlData);

			while ( row = mysql_fetch_row( res ) )
			{
				nResult = atoi(row[0]);
			}

			mysql_free_result( res ) ;
		}

		return nResult;

	}
	catch(...)
	{

	}

	return -1;
}

void CDlgPageServer::InitPresetList()
{
	g_total_preset_setting = 0;

	HTREEITEM	hMainItem = NULL;
	HTREEITEM	hChildItem = NULL, hChildItem1 = NULL, hChildItem2 = NULL;
	HTREEITEM	hCameraItem = NULL;
	HTREEITEM	hStationItem = NULL;
	BOOL b500kVFlag = FALSE;//当为500kV时，不显示下面的市级结点，直接显示变电站。

	int nStationStatus = 0;//站点状态

	int mainid = 0;
	int i = 0, j = 0, k = 0;
	int ns = 0;

	//删除预置位树形列表信息
	DeletePresetTreeList();

	// 添加根节点
	for (i = 0; i < g_TotalSysNum; i ++)
	{
		if (g_SystemInfo[i].localdomain == 1)
		{
			hMainItem = m_trPreset.InsertItem(g_SystemInfo[i].sysname, 1, 1, TVI_ROOT, hMainItem);
			break;
		}
	}

	// 添加变电站中间节点和站点节点
	for (i = 0; i < g_nStationNode_Num; i ++)
	{
		if (g_tStationNode_Info[i].parent_id == g_tStationNode_Info[i].node_id&& g_tStationNode_Info[i].login_type == 0)	//第一级中间节点
		{
			//if (strcmp(g_tStationNode_Info[i].node_name,"220kV") == 0)//不显示220kV
			//	continue;

			//if (strcmp(g_tStationNode_Info[i].node_name,"110kV") == 0)//不显示110kV
			//	continue;

			if (strcmp(g_tStationNode_Info[i].node_name,"500kV") == 0)//不显示下面的市级结点，直接显示变电站
				b500kVFlag = TRUE;
			else
				b500kVFlag = FALSE;

			hChildItem = m_trPreset.InsertItem(g_tStationNode_Info[i].node_name, 7, 7, hMainItem);
			for (ns = 0; ns < g_nStation_Num; ns ++)
			{
				if (g_tStation_Info[ns].node_id == g_tStationNode_Info[i].node_id)
				{
					nStationStatus = CheckStationCurrentStatus2(g_tStation_Info[ns].station_id);
					if (nStationStatus != 0)
						hStationItem = m_trPreset.InsertItem(g_tStation_Info[ns].station_name_videoplant, 8, 8, hChildItem);
					else
						hStationItem = m_trPreset.InsertItem(g_tStation_Info[ns].station_name_videoplant, 10, 10, hChildItem);

					SetPresetNodeInfo(hStationItem,
						g_tStation_Info[ns].station_name_videoplant,
						g_tStation_Info[ns].station_code_videoplant,
						g_tStation_Info[ns].station_id,
						NULL,
						0,
						1,
						0,
						0,
						0,
						0,
						2,
						g_tStation_Info[ns].station_name_videoplant);
				}
			}

			for (j = 0; j < g_nStationNode_Num; j++) //第二级中间节点
			{
				if (g_tStationNode_Info[j].parent_id == g_tStationNode_Info[i].node_id && g_tStationNode_Info[j].parent_id != g_tStationNode_Info[j].node_id)	// 第二季中间节点
				{
					if (b500kVFlag == FALSE)
						hChildItem1 = m_trPreset.InsertItem(g_tStationNode_Info[j].node_name, 7, 7, hChildItem);
					else
						hChildItem1 = hChildItem;

					for (ns = 0; ns < g_nStation_Num; ns ++)
					{
						if (g_tStation_Info[ns].node_id == g_tStationNode_Info[j].node_id)
						{
							nStationStatus = CheckStationCurrentStatus2(g_tStation_Info[ns].station_id);
							if (nStationStatus != 0)
								hStationItem = m_trPreset.InsertItem(g_tStation_Info[ns].station_name_videoplant, 8, 8, hChildItem1);
							else
								hStationItem = m_trPreset.InsertItem(g_tStation_Info[ns].station_name_videoplant, 10, 10, hChildItem1);

							SetPresetNodeInfo(hStationItem,
								g_tStation_Info[ns].station_name_videoplant,
								g_tStation_Info[ns].station_code_videoplant,
								g_tStation_Info[ns].station_id,
								NULL,
								0,
								1,
								0,
								0,
								0,
								0,
								2,
								g_tStation_Info[ns].station_name_videoplant);
						}
					}

					for (k = 0; k < g_nStationNode_Num; k++)//第三级中间节点
					{
						if (g_tStationNode_Info[k].parent_id == g_tStationNode_Info[j].node_id && g_tStationNode_Info[k].parent_id != g_tStationNode_Info[k].node_id)	// 第三季中间节点
						{
							hChildItem2 = m_trPreset.InsertItem(g_tStationNode_Info[k].node_name, 7, 7, hChildItem1);
							for (ns = 0; ns < g_nStation_Num; ns ++)
							{
								if (g_tStation_Info[ns].node_id == g_tStationNode_Info[k].node_id)
								{
									nStationStatus = CheckStationCurrentStatus2(g_tStation_Info[ns].station_id);
									if (nStationStatus != 0)
										hStationItem = m_trPreset.InsertItem(g_tStation_Info[ns].station_name_videoplant, 8, 8, hChildItem2);
									else
										hStationItem = m_trPreset.InsertItem(g_tStation_Info[ns].station_name_videoplant, 10, 10, hChildItem2);

									SetPresetNodeInfo(hStationItem,
										g_tStation_Info[ns].station_name_videoplant,
										g_tStation_Info[ns].station_code_videoplant,
										g_tStation_Info[ns].station_id,
										NULL,
										0,
										1,
										0,
										0,
										0,
										0,
										2,
										g_tStation_Info[ns].station_name_videoplant);
								}
							}
						}
					}
				}
			}
		}
	}

	hMainItem = m_trPreset.GetRootItem();
	if (hMainItem != NULL)
	{
		hChildItem = m_trPreset.GetChildItem(hMainItem);
		if (hChildItem != NULL)
		{
			m_trPreset.SortChildren(hChildItem);//排序
			hChildItem1 = m_trPreset.GetChildItem(hChildItem);
			if (hChildItem1 != NULL)
			{
				m_trPreset.EnsureVisible(hChildItem1);
			}
		}
	}

	hChildItem = m_trPreset.GetNextSiblingItem(hChildItem);
	while(hChildItem != NULL)
	{
		m_trPreset.SortChildren(hChildItem);//排序
		hChildItem1 = m_trPreset.GetChildItem(hChildItem);
		while(hChildItem1 != NULL)
		{
			m_trPreset.SortChildren(hChildItem1);
			hChildItem1 = m_trPreset.GetNextSiblingItem(hChildItem1);
		}
		hChildItem = m_trPreset.GetNextSiblingItem(hChildItem);
	}
}

// 初始化预置位列表树
void CDlgPageServer::InitPresetListByVoltage()
{
	g_total_preset_setting = 0;

	HTREEITEM	hMainItem = NULL;
	HTREEITEM	hChildItem = NULL, hChildItem1 = NULL, hChildItem2 = NULL;
	HTREEITEM	hCameraItem = NULL;
	HTREEITEM	hStationItem = NULL;
	BOOL b500kVFlag = FALSE;//当为500kV时，不显示下面的市级结点，直接显示变电站。

	int nStationStatus = 0;//站点状态

	int i = 0, j = 0, k = 0;
	int ns = 0;
	int v = 0;

	int nNodeId = 0;

	int nNodeStationNum1 = 0,nNodeStationNum2 = 0,nNodeStationNum3 = 0;

	//删除预置位树形列表信息
	DeletePresetTreeList();

	// 添加根节点
	for (i = 0; i < g_TotalSysNum; i ++)
	{
		if (g_SystemInfo[i].localdomain == 1)
		{
			hMainItem = m_trPreset.InsertItem(g_SystemInfo[i].sysname, 1, 1, TVI_ROOT, hMainItem);
			break;
		}
	}

	nNodeId = g_userpower.subgroupno;

	BOOL bPowerFlag = FALSE;

	if (g_userpower.usertype == 10 || g_userpower.usertype == 11)
	{
		bPowerFlag = TRUE;
	}

	// 添加变电站中间节点和站点节点
	for (i = 0; i < g_nStationNode_Num; i ++)
	{
		if (g_tStationNode_Info[i].parent_id == g_tStationNode_Info[i].node_id
			&&g_tStationNode_Info[i].login_type == 2)//电压等级
		{
			//if (strcmp(g_tStationNode_Info[i].node_name,"220kV") == 0)//不显示220kV
			//	continue;

			//if (strcmp(g_tStationNode_Info[i].node_name,"110kV") == 0)//不显示110kV
			//	continue;

			if (strcmp(g_tStationNode_Info[i].node_name,"500kV") == 0)//不显示下面的市级结点，直接显示变电站
				b500kVFlag = TRUE;
			else
				b500kVFlag = FALSE;

			hChildItem = m_trPreset.InsertItem(g_tStationNode_Info[i].node_name, 7, 7, hMainItem);

			nNodeStationNum1 = 0;
			for (ns = 0; ns < g_nStation_Num; ns++)
			{
				if (g_tStation_Info[ns].node_sd_yw_id == g_tStationNode_Info[i].node_id && g_userpower.usertype <= 11)
				{
					nStationStatus = CheckStationCurrentStatus2(g_tStation_Info[ns].station_id);
					if (nStationStatus != 0)
						hStationItem = m_trPreset.InsertItem(g_tStation_Info[ns].station_name_videoplant, 8, 8, hChildItem);
					else
						hStationItem = m_trPreset.InsertItem(g_tStation_Info[ns].station_name_videoplant, 10, 10, hChildItem);

					SetPresetNodeInfo(hStationItem,
						g_tStation_Info[ns].station_name_videoplant,
						g_tStation_Info[ns].station_code_videoplant,
						g_tStation_Info[ns].station_id,
						NULL,
						0,
						1,
						0,
						0,
						0,
						0,
						2,
						g_tStation_Info[ns].station_name_videoplant);

					nNodeStationNum1 ++;
				}
			}

			for (j = 0; j < g_nStationNode_Num; j++) //第二级中间节点，南京市、无锡市、苏州市
			{
				if (g_tStationNode_Info[j].parent_id == g_tStationNode_Info[i].node_id && g_tStationNode_Info[j].parent_id != g_tStationNode_Info[j].node_id)	// 第二级中间节点
				{
					if (b500kVFlag == FALSE)
						hChildItem1 = m_trPreset.InsertItem(g_tStationNode_Info[j].node_name, 7, 7, hChildItem);
					else
						hChildItem1 = hChildItem;

					nNodeStationNum2 = 0;
					for (ns = 0; ns < g_nStation_Num; ns ++)
					{
						if (g_tStation_Info[ns].node_sd_yw_id == g_tStationNode_Info[j].node_id)
						{
							nStationStatus = CheckStationCurrentStatus2(g_tStation_Info[ns].station_id);
							if (nStationStatus != 0)
								hStationItem = m_trPreset.InsertItem(g_tStation_Info[ns].station_name_videoplant, 8, 8, hChildItem1);
							else
								hStationItem = m_trPreset.InsertItem(g_tStation_Info[ns].station_name_videoplant, 10, 10, hChildItem1);

							SetPresetNodeInfo(hStationItem,
								g_tStation_Info[ns].station_name_videoplant,
								g_tStation_Info[ns].station_code_videoplant,
								g_tStation_Info[ns].station_id,
								NULL,
								0,
								1,
								0,
								0,
								0,
								0,
								2,
								g_tStation_Info[ns].station_name_videoplant);

							nNodeStationNum1 ++;
							nNodeStationNum2 ++;
						}
					}

					for (k = 0; k < g_nStationNode_Num; k++)//第三级中间节点
					{
						if (g_tStationNode_Info[k].parent_id == g_tStationNode_Info[j].node_id && g_tStationNode_Info[k].parent_id != g_tStationNode_Info[k].node_id)	// 第三级中间节点
						{
							// 判断运维班是否属于该用户
							if (bPowerFlag == FALSE)
							{
								if (g_pMainDlg != NULL&&g_pMainDlg->CheckUserYwbViewPower2(g_tStationNode_Info[k].node_id) == false)
								{
									continue;
								}
							}

							if (b500kVFlag == FALSE)
								hChildItem2 = m_trPreset.InsertItem(g_tStationNode_Info[k].node_name, 7, 7, hChildItem1);
							else
								hChildItem2 = hChildItem1;

							nNodeStationNum3 = 0;
							for (ns = 0; ns < g_nStation_Num; ns ++)
							{
								if (g_tStation_Info[ns].node_sd_yw_id == g_tStationNode_Info[k].node_id)
								{
									nStationStatus = CheckStationCurrentStatus2(g_tStation_Info[ns].station_id);
									if (nStationStatus != 0)
										hStationItem = m_trPreset.InsertItem(g_tStation_Info[ns].station_name_videoplant, 8, 8, hChildItem2);
									else
										hStationItem = m_trPreset.InsertItem(g_tStation_Info[ns].station_name_videoplant, 10, 10, hChildItem2);

									SetPresetNodeInfo(hStationItem,
										g_tStation_Info[ns].station_name_videoplant,
										g_tStation_Info[ns].station_code_videoplant,
										g_tStation_Info[ns].station_id,
										NULL,
										0,
										1,
										0,
										0,
										0,
										0,
										2,
										g_tStation_Info[ns].station_name_videoplant);

									nNodeStationNum1 ++;
									nNodeStationNum2 ++;
									nNodeStationNum3 ++;
								}
							}

							if (bPowerFlag == FALSE && nNodeStationNum3 == 0)
							{
								DeletePresetTreeListItemAndDataInfo(hChildItem);
							}
						}
					}

					if (bPowerFlag == FALSE && nNodeStationNum2 == 0)
					{
						DeletePresetTreeListItemAndDataInfo(hChildItem1);
					}
				}
			}

			if (bPowerFlag == FALSE && nNodeStationNum1 == 0)
			{
				DeletePresetTreeListItemAndDataInfo(hChildItem);
			}
		}
	}

	hMainItem = m_trPreset.GetRootItem();
	if (hMainItem != NULL)
	{
		hChildItem = m_trPreset.GetChildItem(hMainItem);
		if (hChildItem != NULL)
		{
			m_trPreset.SortChildren(hChildItem);//排序
			hChildItem1 = m_trPreset.GetChildItem(hChildItem);
			if (hChildItem1 != NULL)
			{
				m_trPreset.EnsureVisible(hChildItem1);
			}
		}
	}

	hChildItem = m_trPreset.GetNextSiblingItem(hChildItem);
	while(hChildItem != NULL)
	{
		m_trPreset.SortChildren(hChildItem);//排序
		hChildItem1 = m_trPreset.GetChildItem(hChildItem);
		while(hChildItem1 != NULL)
		{
			m_trPreset.SortChildren(hChildItem1);
			hChildItem1 = m_trPreset.GetNextSiblingItem(hChildItem1);
		}
		hChildItem = m_trPreset.GetNextSiblingItem(hChildItem);
	}
}

// 初始化预置位列表树
void CDlgPageServer::InitPresetListByCity()
{
	g_total_preset_setting = 0;

	HTREEITEM	hMainItem = NULL;
	HTREEITEM	hChildItem = NULL, hChildItem1 = NULL, hChildItem2 = NULL;
	HTREEITEM	hCameraItem = NULL;
	HTREEITEM	hStationItem = NULL;

	int nStationStatus = 0;//站点状态

	int mainid = 0;
	int i = 0, j = 0, k = 0;
	int ns = 0;
	int nNodeId = 0;

	BOOL bPowerFlag = TRUE;

	//市检分部专职判断权限
	if (g_userpower.usertype == 20)
	{
		bPowerFlag = FALSE;
	}

	//删除预置位树形列表信息
	DeletePresetTreeList();

	// 添加根节点
	for (i = 0; i < g_TotalSysNum; i ++)
	{
		if (g_SystemInfo[i].localdomain == 1)
		{
			hMainItem = m_trPreset.InsertItem(g_SystemInfo[i].sysname, 1, 1, TVI_ROOT, hMainItem);
			break;
		}
	}

	nNodeId = g_userpower.subgroupno;

	// 添加变电站中间节点和站点节点
	for (i = 0; i < g_nStationNode_Num; i ++)
	{
		if (((nNodeId == 0&&g_tStationNode_Info[i].parent_id == g_tStationNode_Info[i].node_id)
			||(nNodeId != 0&&g_tStationNode_Info[i].node_id == nNodeId))
			&&g_tStationNode_Info[i].login_type == 1)	//第一级中间节点
		{
			//省检用户不用查看地区一级站点
			if (g_userpower.logintype == 1&&(g_userpower.usertype == 11||g_userpower.usertype == 12))
			{
				if (strcmp(g_tStationNode_Info[i].node_name,"地区") == 0)
					continue;
			}

			hChildItem = m_trPreset.InsertItem(g_tStationNode_Info[i].node_name, 7, 7, hMainItem);
			for (ns = 0; ns < g_nStation_Num; ns ++)
			{
				if (g_tStation_Info[ns].node_yw_id == g_tStationNode_Info[i].node_id)
				{
					nStationStatus = CheckStationCurrentStatus2(g_tStation_Info[ns].station_id);
					if (nStationStatus != 0)
						hStationItem = m_trPreset.InsertItem(g_tStation_Info[ns].station_name_videoplant, 8, 8, hChildItem);
					else
						hStationItem = m_trPreset.InsertItem(g_tStation_Info[ns].station_name_videoplant, 10, 10, hChildItem);

					SetPresetNodeInfo(hStationItem,
						g_tStation_Info[ns].station_name_videoplant,
						g_tStation_Info[ns].station_code_videoplant,
						g_tStation_Info[ns].station_id,
						NULL,
						0,
						1,
						0,
						0,
						0,
						0,
						2,
						g_tStation_Info[ns].station_name_videoplant);
				}
			}

			for (j = 0; j < g_nStationNode_Num; j++) //第二级中间节点
			{
				if (g_tStationNode_Info[j].parent_id == g_tStationNode_Info[i].node_id 
					&& g_tStationNode_Info[j].parent_id != g_tStationNode_Info[j].node_id)	// 第二级中间节点
				{
					//权限判断
					if(bPowerFlag == FALSE)
					{
						if (g_pMainDlg != NULL&&g_pMainDlg->CheckUserYwbViewPower2(g_tStationNode_Info[j].node_id) == FALSE)
						{
							continue;
						}
					}

					hChildItem1 = m_trPreset.InsertItem(g_tStationNode_Info[j].node_name, 7, 7, hChildItem);

					for (ns = 0; ns < g_nStation_Num; ns ++)
					{
						if (g_tStation_Info[ns].node_yw_id == g_tStationNode_Info[j].node_id)
						{
							nStationStatus = CheckStationCurrentStatus2(g_tStation_Info[ns].station_id);
							if (nStationStatus != 0)
								hStationItem = m_trPreset.InsertItem(g_tStation_Info[ns].station_name_videoplant, 8, 8, hChildItem1);
							else
								hStationItem = m_trPreset.InsertItem(g_tStation_Info[ns].station_name_videoplant, 10, 10, hChildItem1);

							SetPresetNodeInfo(hStationItem,
								g_tStation_Info[ns].station_name_videoplant,
								g_tStation_Info[ns].station_code_videoplant,
								g_tStation_Info[ns].station_id,
								NULL,
								0,
								1,
								0,
								0,
								0,
								0,
								2,
								g_tStation_Info[ns].station_name_videoplant);
						}
					}

					for (k = 0; k < g_nStationNode_Num; k++)//第三级中间节点
					{
						if (g_tStationNode_Info[k].parent_id == g_tStationNode_Info[j].node_id 
							&& g_tStationNode_Info[k].parent_id != g_tStationNode_Info[k].node_id)	// 第三级中间节点
						{
							hChildItem2 = m_trPreset.InsertItem(g_tStationNode_Info[k].node_name, 7, 7, hChildItem1);
							for (ns = 0; ns < g_nStation_Num; ns ++)
							{
								if (g_tStation_Info[ns].node_yw_id == g_tStationNode_Info[k].node_id)
								{
									nStationStatus = CheckStationCurrentStatus2(g_tStation_Info[ns].station_id);
									if (nStationStatus != 0)
										hStationItem = m_trPreset.InsertItem(g_tStation_Info[ns].station_name_videoplant, 8, 8, hChildItem2);
									else
										hStationItem = m_trPreset.InsertItem(g_tStation_Info[ns].station_name_videoplant, 10, 10, hChildItem2);

									SetPresetNodeInfo(hStationItem,
										g_tStation_Info[ns].station_name_videoplant,
										g_tStation_Info[ns].station_code_videoplant,
										g_tStation_Info[ns].station_id,
										NULL,
										0,
										1,
										0,
										0,
										0,
										0,
										2,
										g_tStation_Info[ns].station_name_videoplant);
								}
							}
						}
					}
				}
			}
		}
	}

	hMainItem = m_trPreset.GetRootItem();
	if (hMainItem != NULL)
	{
		hChildItem = m_trPreset.GetChildItem(hMainItem);
		if (hChildItem != NULL)
		{
			m_trPreset.SortChildren(hChildItem);//排序
			hChildItem1 = m_trPreset.GetChildItem(hChildItem);
			if (hChildItem1 != NULL)
			{
				m_trPreset.EnsureVisible(hChildItem1);
			}
		}
	}

	hChildItem = m_trPreset.GetNextSiblingItem(hChildItem);
	while(hChildItem != NULL)
	{
		m_trPreset.SortChildren(hChildItem);//排序
		hChildItem1 = m_trPreset.GetChildItem(hChildItem);
		while(hChildItem1 != NULL)
		{
			m_trPreset.SortChildren(hChildItem1);
			hChildItem1 = m_trPreset.GetNextSiblingItem(hChildItem1);
		}
		hChildItem = m_trPreset.GetNextSiblingItem(hChildItem);
	}
}

//删除预置位树
void CDlgPageServer::DeletePresetTreeList()
{
	HTREEITEM	hMainItem = NULL;
	_T_NODE_PRESET_INFO *pNodeInfo = NULL;
	DWORD_PTR pInfo = NULL;

	try
	{
		hMainItem = m_trPreset.GetRootItem();
		while(hMainItem != NULL)
		{
			pInfo = m_trPreset.GetItemData(hMainItem);
			if (pInfo != NULL)
			{
				pNodeInfo = (_T_NODE_PRESET_INFO *)pInfo;
				delete pNodeInfo;
				pInfo = NULL;
				pNodeInfo = NULL;
				m_trPreset.SetItemData(hMainItem,NULL);
			}

			DeletePresetChildTreeListInfo(hMainItem);
			hMainItem = m_trPreset.GetNextSiblingItem(hMainItem);
		}
	}
	catch(...)
	{

	}

	m_trPreset.DeleteAllItems();
}

//删除预置树的子结点信息
void CDlgPageServer::DeletePresetChildTreeListInfo(HTREEITEM hItem)
{
	HTREEITEM	hChildItem = NULL;
	_T_NODE_PRESET_INFO *pNodeInfo = NULL;
	DWORD_PTR pInfo = NULL;

	try
	{
		hChildItem = m_trPreset.GetChildItem(hItem);
		while(hChildItem != NULL)
		{
			pInfo = m_trPreset.GetItemData(hChildItem);
			if (pInfo != NULL)
			{
				pNodeInfo = (_T_NODE_PRESET_INFO *)pInfo;
				delete pNodeInfo;
				pInfo = NULL;
				pNodeInfo = NULL;
				m_trPreset.SetItemData(hChildItem,NULL);
			}

			DeletePresetChildTreeListInfo(hChildItem);
			hChildItem = m_trPreset.GetNextSiblingItem(hChildItem);
		}
	}
	catch(...)
	{

	}
}

//删除列表树
void CDlgPageServer::DeletePresetTreeListItemAndDataInfo(HTREEITEM hItem)
{
	HTREEITEM	hChildItem = NULL;
	_T_NODE_PRESET_INFO *pNodeInfo = NULL;
	DWORD_PTR pInfo = NULL;

	try
	{
		DeletePresetChildTreeListInfo(hItem);

		pInfo = m_trPreset.GetItemData(hItem);
		if (pInfo != NULL)
		{
			pNodeInfo = (_T_NODE_PRESET_INFO *)pInfo;
			delete pNodeInfo;
			pInfo = NULL;
			pNodeInfo = NULL;
			m_trPreset.SetItemData(hItem,NULL);
		}

		m_trPreset.DeleteItem(hItem);
	}
	catch(...)
	{

	}
}

//初始化树列表，该函数只初始化到变电站的结点，摄像头不在这里，当双击变电站结点时，才加载摄像头
void CDlgPageServer::InitCameraTreelist()
{
	try
	{
		int i = 0, j = 0, k = 0;
		int ns = 0;

		//删除摄像头树信息
		DeleteCameraTreeList();

		HTREEITEM	hMainItem = NULL;
		HTREEITEM	hChildItem = NULL, hChildItem1 = NULL, hChildItem2 = NULL;
		HTREEITEM	hStationItem = NULL;
		HTREEITEM	hCameraItem = NULL;
		BOOL b500kVFlag = FALSE;//当为500kV时，不显示下面的市级结点，直接显示变电站。

		int nStationStatus = 0;//变电站状态
		char szStationShowName[256] = {0};
		memset(szStationShowName,0,sizeof(szStationShowName));

		// 添加根节点
		for (i = 0; i < g_TotalSysNum; i ++)
		{
			if (g_SystemInfo[i].localdomain == 1)
			{
				hMainItem = m_trServer.InsertItem(g_SystemInfo[i].sysname, 1, 1, TVI_ROOT, hMainItem);
				break;
			}
		}

		 //添加变电站中间节点和站点节点
		for (i = 0; i < g_nStationNode_Num; i ++)
		{
			if (g_tStationNode_Info[i].parent_id == g_tStationNode_Info[i].node_id&&g_tStationNode_Info[i].login_type == 0)	//第一级中间节点--500kV,220kV,110kV等
			{
				//if (strcmp(g_tStationNode_Info[i].node_name,"220kV") == 0)//不显示220kV
				//	continue;

				//if (strcmp(g_tStationNode_Info[i].node_name,"110kV") == 0)//不显示110kV
				//	continue;

				if (strcmp(g_tStationNode_Info[i].node_name,"500kV") == 0)//不显示下面的市级结点，直接显示变电站
					b500kVFlag = TRUE;
				else
					b500kVFlag = FALSE;

				hChildItem = m_trServer.InsertItem(g_tStationNode_Info[i].node_name, 7, 7, hMainItem);

				for (ns = 0; ns < g_nStation_Num; ns ++)
				{
					if (g_tStation_Info[ns].node_id == g_tStationNode_Info[i].node_id)
					{
						if (strlen(g_tStation_Info[ns].notes) != 0)
							sprintf_s(szStationShowName,sizeof(szStationShowName),"%s(%s)",g_tStation_Info[ns].station_name_videoplant,g_tStation_Info[ns].notes);
						else
							strcpy_s(szStationShowName,sizeof(szStationShowName),g_tStation_Info[ns].station_name_videoplant);

						nStationStatus = CheckStationCurrentStatus2(g_tStation_Info[ns].station_id);
						if (nStationStatus != 0)
							hStationItem = m_trServer.InsertItem(szStationShowName, 8, 8, hChildItem);
						else
							hStationItem = m_trServer.InsertItem(szStationShowName, 10, 10, hChildItem);

						SetCameraNodeInfo(hStationItem,
							g_tStation_Info[ns].station_name_videoplant,
							g_tStation_Info[ns].station_code_videoplant,
							g_tStation_Info[ns].station_id,
							1,
							0,
							0,
							0,
							0,
							2,
							g_tStation_Info[ns].station_name_videoplant);
					}
				}

				for (j = 0; j < g_nStationNode_Num; j ++)//第二级中间节点--南京市、苏州市
				{
					if (g_tStationNode_Info[j].parent_id == g_tStationNode_Info[i].node_id && g_tStationNode_Info[j].parent_id != g_tStationNode_Info[j].node_id)	// 第二季中间节点
					{
						if (b500kVFlag == FALSE)
							hChildItem1 = m_trServer.InsertItem(g_tStationNode_Info[j].node_name, 7, 7, hChildItem);
						else
							hChildItem1 = hChildItem;

						for (ns = 0; ns < g_nStation_Num; ns ++)
						{
							if (g_tStation_Info[ns].node_id == g_tStationNode_Info[j].node_id)
							{
								if (strlen(g_tStation_Info[ns].notes) != 0)
									sprintf_s(szStationShowName,sizeof(szStationShowName),"%s(%s)",g_tStation_Info[ns].station_name_videoplant,g_tStation_Info[ns].notes);
								else
									strcpy_s(szStationShowName,sizeof(szStationShowName),g_tStation_Info[ns].station_name_videoplant);

								nStationStatus = CheckStationCurrentStatus2(g_tStation_Info[ns].station_id);
								if (nStationStatus != 0)
									hStationItem = m_trServer.InsertItem(szStationShowName, 8, 8, hChildItem1);
								else
									hStationItem = m_trServer.InsertItem(szStationShowName, 10, 10, hChildItem1);

								SetCameraNodeInfo(hStationItem,
									g_tStation_Info[ns].station_name_videoplant,
									g_tStation_Info[ns].station_code_videoplant,
									g_tStation_Info[ns].station_id,
									1,
									0,
									0,
									0,
									0,
									2,
									g_tStation_Info[ns].station_name_videoplant);
							}
						}

						for (k = 0; k < g_nStationNode_Num; k++)//第三级中间节点
						{
							if (g_tStationNode_Info[k].parent_id == g_tStationNode_Info[j].node_id && g_tStationNode_Info[k].parent_id != g_tStationNode_Info[k].node_id)	// 第三季中间节点
							{
								hChildItem2 = m_trServer.InsertItem(g_tStationNode_Info[k].node_name, 7, 7, hChildItem1);

								for (ns = 0; ns < g_nStation_Num; ns ++)
								{
									if (g_tStation_Info[ns].node_id == g_tStationNode_Info[k].node_id)
									{
										if (strlen(g_tStation_Info[ns].notes) != 0)
											sprintf_s(szStationShowName,sizeof(szStationShowName),"%s(%s)",g_tStation_Info[ns].station_name_videoplant,g_tStation_Info[ns].notes);
										else
											strcpy_s(szStationShowName,sizeof(szStationShowName),g_tStation_Info[ns].station_name_videoplant);

										nStationStatus = CheckStationCurrentStatus2(g_tStation_Info[ns].station_id);
										if (nStationStatus != 0)
											hStationItem = m_trServer.InsertItem(szStationShowName, 8, 8, hChildItem2);
										else
											hStationItem = m_trServer.InsertItem(szStationShowName, 10, 10, hChildItem2);
										
										SetCameraNodeInfo(hStationItem,
											g_tStation_Info[ns].station_name_videoplant,
											g_tStation_Info[ns].station_code_videoplant,
											g_tStation_Info[ns].station_id,
											1,
											0,
											0,
											0,
											0,
											2,
											g_tStation_Info[ns].station_name_videoplant);
									}
								}
							}
						}
					}
				}
			}
		}

		hMainItem = m_trServer.GetRootItem();
		if (hMainItem != NULL)
		{
			hChildItem = m_trServer.GetChildItem(hMainItem);
			if (hChildItem != NULL)
			{
				m_trServer.SortChildren(hChildItem);//排序
				hChildItem1 = m_trServer.GetChildItem(hChildItem);
				if (hChildItem1 != NULL)
				{
					m_trServer.EnsureVisible(hChildItem1);
				}
			}
		}

		hChildItem = m_trServer.GetNextSiblingItem(hChildItem);
		while(hChildItem != NULL)
		{
			m_trServer.SortChildren(hChildItem);//排序
			hChildItem1 = m_trServer.GetChildItem(hChildItem);
			while(hChildItem1 != NULL)
			{
				m_trServer.SortChildren(hChildItem1);
				hChildItem1 = m_trServer.GetNextSiblingItem(hChildItem1);
			}
			hChildItem = m_trServer.GetNextSiblingItem(hChildItem);
		}
	}
	catch(...)
	{

	}
}

// 初始化列表树--青奥保电
void CDlgPageServer::InitCameraTreelistQABD()
{
	try
	{
		int i = 0, j = 0, k = 0;
		int ns = 0;

		//删除摄像头树信息
		DeleteCameraTreeList();

		HTREEITEM	hMainItem = NULL;
		HTREEITEM	hChildItem = NULL, hChildItem1 = NULL, hChildItem2 = NULL;
		HTREEITEM	hStationItem = NULL;
		HTREEITEM	hCameraItem = NULL;
		BOOL b500kVFlag = FALSE;//当为500kV时，不显示下面的市级结点，直接显示变电站。

		int nStationStatus = 0;//变电站状态
		char szStationShowName[256] = {0};
		memset(szStationShowName,0,sizeof(szStationShowName));

		for (ns = 0; ns < g_nStation_Num; ns ++)
		{
			if (strlen(g_tStation_Info[ns].notes) != 0)
				sprintf_s(szStationShowName,sizeof(szStationShowName),"%s(%s)",g_tStation_Info[ns].station_name_videoplant,g_tStation_Info[ns].notes);
			else
				strcpy_s(szStationShowName,sizeof(szStationShowName),g_tStation_Info[ns].station_name_videoplant);

			nStationStatus = CheckStationCurrentStatus2(g_tStation_Info[ns].station_id);
			if (nStationStatus != 0)
				hStationItem = m_trServer.InsertItem(szStationShowName, 8, 8, hChildItem2);
			else
				hStationItem = m_trServer.InsertItem(szStationShowName, 10, 10, hChildItem2);

			SetCameraNodeInfo(hStationItem,
				g_tStation_Info[ns].station_name_videoplant,
				g_tStation_Info[ns].station_code_videoplant,
				g_tStation_Info[ns].station_id,
				1,
				0,
				0,
				0,
				0,
				2,
				g_tStation_Info[ns].station_name_videoplant);
		}

		//m_trServer.SortChildren(NULL);//排序
	}
	catch(...)
	{

	}
}

// 按电压等级初始化列表树
void CDlgPageServer::InitCameraTreelistByVoltage()
{
	try
	{
		int i = 0, j = 0, k = 0, l = 0;
		int ns = 0;
		int nNodeId = 0;
		int nNodeStationNum1 = 0,nNodeStationNum2 = 0,nNodeStationNum3 = 0;

		//删除摄像头树信息
		DeleteCameraTreeList();

		HTREEITEM	hMainItem = NULL;
		HTREEITEM	hChildItem = NULL, hChildItem1 = NULL, hChildItem2 = NULL;
		HTREEITEM	hStationItem = NULL;
		HTREEITEM	hCameraItem = NULL;
		BOOL b500kVFlag = FALSE;//当为500kV时，不显示下面的市级结点，直接显示变电站。

		int nStationStatus = 0;//变电站状态
		char szStationShowName[256] = {0};
		memset(szStationShowName,0,sizeof(szStationShowName));

		// 添加根节点
		for (i = 0; i < g_TotalSysNum; i ++)
		{
			if (g_SystemInfo[i].localdomain == 1)
			{
				hMainItem = m_trServer.InsertItem(g_SystemInfo[i].sysname, 1, 1, TVI_ROOT, hMainItem);
				break;
			}
		}

		nNodeId = g_userpower.subgroupno;

		BOOL bPowerFlag = FALSE;

		if (g_userpower.usertype == 10 || g_userpower.usertype == 11)
		{
			bPowerFlag = TRUE;
		}

		// 添加变电站中间节点和站点节点
		for (i = 0; i < g_nStationNode_Num; i ++)
		{
			if (g_tStationNode_Info[i].parent_id == g_tStationNode_Info[i].node_id
				&&g_tStationNode_Info[i].login_type == 2)
			{
				//if (strcmp(g_tStationNode_Info[i].node_name,"220kV") == 0)//不显示220kV
				//	continue;

				//if (strcmp(g_tStationNode_Info[i].node_name,"110kV") == 0)//不显示110kV
				//	continue;

				if (strcmp(g_tStationNode_Info[i].node_name,"500kV") == 0)//不显示下面的市级结点，直接显示变电站
					b500kVFlag = TRUE;
				else
					b500kVFlag = FALSE;

				hChildItem = m_trServer.InsertItem(g_tStationNode_Info[i].node_name, 7, 7, hMainItem);

				nNodeStationNum1 = 0;
				for (ns = 0; ns < g_nStation_Num; ns ++)
				{
					if (g_tStation_Info[ns].node_sd_yw_id == g_tStationNode_Info[i].node_id && g_userpower.usertype <= 11)
					{
						// 						if (strlen(g_tStation_Info[ns].notes) != 0)
						// 							sprintf_s(szStationShowName,sizeof(szStationShowName),"%s(%s)",g_tStation_Info[ns].station_name_videoplant,g_tStation_Info[ns].notes);
						// 						else
						strcpy_s(szStationShowName,sizeof(szStationShowName),g_tStation_Info[ns].station_name_videoplant);

						nStationStatus = CheckStationCurrentStatus2(g_tStation_Info[ns].station_id);
						if (nStationStatus != 0)
							hStationItem = m_trServer.InsertItem(szStationShowName, 8, 8, hChildItem);
						else
							hStationItem = m_trServer.InsertItem(szStationShowName, 10, 10, hChildItem);

						SetCameraNodeInfo(hStationItem,
							g_tStation_Info[ns].station_name_videoplant,
							g_tStation_Info[ns].station_code_videoplant,
							g_tStation_Info[ns].station_id,
							1,
							0,
							0,
							0,
							0,
							2,
							g_tStation_Info[ns].station_name_videoplant);

						nNodeStationNum1 ++;
					}
				}

				for (j = 0; j < g_nStationNode_Num; j ++)//第二级中间节点--南京市、苏州市
				{
					if (g_tStationNode_Info[j].parent_id == g_tStationNode_Info[i].node_id && g_tStationNode_Info[j].parent_id != g_tStationNode_Info[j].node_id)	// 第二级中间节点
					{
						if (b500kVFlag == FALSE)
							hChildItem1 = m_trServer.InsertItem(g_tStationNode_Info[j].node_name, 7, 7, hChildItem);
						else
							hChildItem1 = hChildItem;

						nNodeStationNum2 = 0;
						for (ns = 0; ns < g_nStation_Num; ns ++)
						{
							if (g_tStation_Info[ns].node_sd_yw_id == g_tStationNode_Info[j].node_id)
							{
								// 								if (strlen(g_tStation_Info[ns].notes) != 0)
								// 									sprintf_s(szStationShowName,sizeof(szStationShowName),"%s(%s)",g_tStation_Info[ns].station_name_videoplant,g_tStation_Info[ns].notes);
								// 								else
								strcpy_s(szStationShowName,sizeof(szStationShowName),g_tStation_Info[ns].station_name_videoplant);

								nStationStatus = CheckStationCurrentStatus2(g_tStation_Info[ns].station_id);
								if (nStationStatus != 0)
									hStationItem = m_trServer.InsertItem(szStationShowName, 8, 8, hChildItem1);
								else
									hStationItem = m_trServer.InsertItem(szStationShowName, 10, 10, hChildItem1);

								SetCameraNodeInfo(hStationItem,
									g_tStation_Info[ns].station_name_videoplant,
									g_tStation_Info[ns].station_code_videoplant,
									g_tStation_Info[ns].station_id,
									1,
									0,
									0,
									0,
									0,
									2,
									g_tStation_Info[ns].station_name_videoplant);

								nNodeStationNum1 ++;
								nNodeStationNum2 ++;
							}
						}

						for (k = 0; k < g_nStationNode_Num; k++)//第三级中间节点
						{
							if (g_tStationNode_Info[k].parent_id == g_tStationNode_Info[j].node_id && g_tStationNode_Info[k].parent_id != g_tStationNode_Info[k].node_id)	// 第三级中间节点
							{
								// 判断运维班是否属于该用户
								if (bPowerFlag == FALSE)
								{
									if (g_pMainDlg != NULL&&g_pMainDlg->CheckUserYwbViewPower2(g_tStationNode_Info[k].node_id) == false)
									{
										continue;
									}
								}

								hChildItem2 = m_trServer.InsertItem(g_tStationNode_Info[k].node_name, 7, 7, hChildItem1);

								nNodeStationNum3 = 0;
								for (ns = 0; ns < g_nStation_Num; ns ++)
								{
									if (g_tStation_Info[ns].node_sd_yw_id == g_tStationNode_Info[k].node_id)
									{
										// 										if (strlen(g_tStation_Info[ns].notes) != 0)
										// 											sprintf_s(szStationShowName,sizeof(szStationShowName),"%s(%s)",g_tStation_Info[ns].station_name_videoplant,g_tStation_Info[ns].notes);
										// 										else
										strcpy_s(szStationShowName,sizeof(szStationShowName),g_tStation_Info[ns].station_name_videoplant);

										nStationStatus = CheckStationCurrentStatus2(g_tStation_Info[ns].station_id);
										if (nStationStatus != 0)
											hStationItem = m_trServer.InsertItem(szStationShowName, 8, 8, hChildItem2);
										else
											hStationItem = m_trServer.InsertItem(szStationShowName, 10, 10, hChildItem2);

										SetCameraNodeInfo(hStationItem,
											g_tStation_Info[ns].station_name_videoplant,
											g_tStation_Info[ns].station_code_videoplant,
											g_tStation_Info[ns].station_id,
											1,
											0,
											0,
											0,
											0,
											2,
											g_tStation_Info[ns].station_name_videoplant);

										nNodeStationNum1 ++;
										nNodeStationNum2 ++;
										nNodeStationNum3 ++;
									}
								}
								if (bPowerFlag == FALSE && nNodeStationNum3 == 0)
								{
									DeleteCameraTreeListItemAndDataInfo(hChildItem2);
								}
							}
						}

						if (bPowerFlag == FALSE && nNodeStationNum2 == 0)
						{
							DeleteCameraTreeListItemAndDataInfo(hChildItem1);
						}
					}
				}

				if (bPowerFlag == FALSE && nNodeStationNum1 == 0)
				{
					DeleteCameraTreeListItemAndDataInfo(hChildItem);
				}
			}
		}

		hMainItem = m_trServer.GetRootItem();
		if (hMainItem != NULL)
		{
			hChildItem = m_trServer.GetChildItem(hMainItem);
			if (hChildItem != NULL)
			{
				m_trServer.SortChildren(hChildItem);//排序
				hChildItem1 = m_trServer.GetChildItem(hChildItem);
				if (hChildItem1 != NULL)
				{
					m_trServer.EnsureVisible(hChildItem1);
				}
			}
		}

		hChildItem = m_trServer.GetNextSiblingItem(hChildItem);
		while(hChildItem != NULL)
		{
			m_trServer.SortChildren(hChildItem);//排序
			hChildItem1 = m_trServer.GetChildItem(hChildItem);
			while(hChildItem1 != NULL)
			{
				m_trServer.SortChildren(hChildItem1);
				hChildItem1 = m_trServer.GetNextSiblingItem(hChildItem1);
			}
			hChildItem = m_trServer.GetNextSiblingItem(hChildItem);
		}
	}
	catch(...)
	{

	}
}

// 按城市初始化列表树
void CDlgPageServer::InitCameraTreelistByCity()
{
	try
	{
		int i = 0, j = 0, k = 0;
		int ns = 0;
		int nNodeId = 0;

		BOOL bPowerFlag = TRUE;

		//市检分部专职判断权限
		if (g_userpower.usertype == 20)
		{
			bPowerFlag = FALSE;
		}

		//删除摄像头树信息
		DeleteCameraTreeList();

		HTREEITEM	hMainItem = NULL;
		HTREEITEM	hChildItem = NULL, hChildItem1 = NULL, hChildItem2 = NULL;
		HTREEITEM	hStationItem = NULL;
		HTREEITEM	hCameraItem = NULL;

		int nStationStatus = 0;//变电站状态
		char szStationShowName[256] = {0};
		memset(szStationShowName,0,sizeof(szStationShowName));

		// 添加根节点
		for (i = 0; i < g_TotalSysNum; i ++)
		{
			if (g_SystemInfo[i].localdomain == 1)
			{
				hMainItem = m_trServer.InsertItem(g_SystemInfo[i].sysname, 1, 1, TVI_ROOT, hMainItem);
				break;
			}
		}

		nNodeId = g_userpower.subgroupno;

		// 添加变电站中间节点和站点节点
		for (i = 0; i < g_nStationNode_Num; i++)
		{
			if (((nNodeId == 0&&g_tStationNode_Info[i].parent_id == g_tStationNode_Info[i].node_id)
				||(nNodeId != 0&&g_tStationNode_Info[i].node_id == nNodeId))
				&&g_tStationNode_Info[i].login_type == 1)	//第一级中间节点
			{
				//省检用户不用查看地区一级站点
				if (g_userpower.logintype == 1&&(g_userpower.usertype == 11||g_userpower.usertype == 12))
				{
					if (strcmp(g_tStationNode_Info[i].node_name,"地区") == 0)
						continue;
				}

				hChildItem = m_trServer.InsertItem(g_tStationNode_Info[i].node_name, 7, 7, hMainItem);

				for (ns = 0; ns < g_nStation_Num; ns ++)
				{
					if (g_tStation_Info[ns].node_yw_id == g_tStationNode_Info[i].node_id)
					{
						if (strlen(g_tStation_Info[ns].notes) != 0)
							sprintf_s(szStationShowName,sizeof(szStationShowName),"%s(%s)",g_tStation_Info[ns].station_name_videoplant,g_tStation_Info[ns].notes);
						else
							strcpy_s(szStationShowName,sizeof(szStationShowName),g_tStation_Info[ns].station_name_videoplant);

						nStationStatus = CheckStationCurrentStatus2(g_tStation_Info[ns].station_id);
						if (nStationStatus != 0)
							hStationItem = m_trServer.InsertItem(szStationShowName, 8, 8, hChildItem);
						else
							hStationItem = m_trServer.InsertItem(szStationShowName, 10, 10, hChildItem);

						SetCameraNodeInfo(hStationItem,
							g_tStation_Info[ns].station_name_videoplant,
							g_tStation_Info[ns].station_code_videoplant,
							g_tStation_Info[ns].station_id,
							1,
							0,
							0,
							0,
							0,
							2,
							g_tStation_Info[ns].station_name_videoplant);
					}
				}

				for (j = 0; j < g_nStationNode_Num; j ++)//第二级中间节点
				{
					if (g_tStationNode_Info[j].parent_id == g_tStationNode_Info[i].node_id 
						&& g_tStationNode_Info[j].parent_id != g_tStationNode_Info[j].node_id)	// 第二级中间节点
					{
						//权限判断
						if(bPowerFlag == FALSE)
						{
							if (g_pMainDlg != NULL&&g_pMainDlg->CheckUserYwbViewPower2(g_tStationNode_Info[j].node_id) == FALSE)
							{
								continue;
							}
						}

						hChildItem1 = m_trServer.InsertItem(g_tStationNode_Info[j].node_name, 7, 7, hChildItem);

						for (ns = 0; ns < g_nStation_Num; ns++)
						{
							if (g_tStation_Info[ns].node_yw_id == g_tStationNode_Info[j].node_id)
							{
								if (strlen(g_tStation_Info[ns].notes) != 0)
									sprintf_s(szStationShowName,sizeof(szStationShowName),"%s(%s)",g_tStation_Info[ns].station_name_videoplant,g_tStation_Info[ns].notes);
								else
									strcpy_s(szStationShowName,sizeof(szStationShowName),g_tStation_Info[ns].station_name_videoplant);

								nStationStatus = CheckStationCurrentStatus2(g_tStation_Info[ns].station_id);
								if (nStationStatus != 0)
									hStationItem = m_trServer.InsertItem(szStationShowName, 8, 8, hChildItem1);
								else
									hStationItem = m_trServer.InsertItem(szStationShowName, 10, 10, hChildItem1);

								SetCameraNodeInfo(hStationItem,
									g_tStation_Info[ns].station_name_videoplant,
									g_tStation_Info[ns].station_code_videoplant,
									g_tStation_Info[ns].station_id,
									1,
									0,
									0,
									0,
									0,
									2,
									g_tStation_Info[ns].station_name_videoplant);
							}
						}

						for (k = 0; k < g_nStationNode_Num; k++)//第三级中间节点
						{
							if (g_tStationNode_Info[k].parent_id == g_tStationNode_Info[j].node_id 
								&&g_tStationNode_Info[k].parent_id != g_tStationNode_Info[k].node_id)	// 第三级中间节点
							{
								hChildItem2 = m_trServer.InsertItem(g_tStationNode_Info[k].node_name, 7, 7, hChildItem1);

								for (ns = 0; ns < g_nStation_Num; ns ++)
								{
									if (g_tStation_Info[ns].node_yw_id == g_tStationNode_Info[k].node_id)
									{
										if (strlen(g_tStation_Info[ns].notes) != 0)
											sprintf_s(szStationShowName,sizeof(szStationShowName),"%s(%s)",g_tStation_Info[ns].station_name_videoplant,g_tStation_Info[ns].notes);
										else
											strcpy_s(szStationShowName,sizeof(szStationShowName),g_tStation_Info[ns].station_name_videoplant);

										nStationStatus = CheckStationCurrentStatus2(g_tStation_Info[ns].station_id);
										if (nStationStatus != 0)
											hStationItem = m_trServer.InsertItem(szStationShowName, 8, 8, hChildItem2);
										else
											hStationItem = m_trServer.InsertItem(szStationShowName, 10, 10, hChildItem2);

										SetCameraNodeInfo(hStationItem,
											g_tStation_Info[ns].station_name_videoplant,
											g_tStation_Info[ns].station_code_videoplant,
											g_tStation_Info[ns].station_id,
											1,
											0,
											0,
											0,
											0,
											2,
											g_tStation_Info[ns].station_name_videoplant);
									}
								}
							}
						}
					}
				}
			}
		}

		hMainItem = m_trServer.GetRootItem();
		if (hMainItem != NULL)
		{
			hChildItem = m_trServer.GetChildItem(hMainItem);
			if (hChildItem != NULL)
			{
				m_trServer.SortChildren(hChildItem);//排序
				hChildItem1 = m_trServer.GetChildItem(hChildItem);
				if (hChildItem1 != NULL)
				{
					m_trServer.EnsureVisible(hChildItem1);
				}
			}
		}

		hChildItem = m_trServer.GetNextSiblingItem(hChildItem);
		while(hChildItem != NULL)
		{
			m_trServer.SortChildren(hChildItem);//排序
			hChildItem1 = m_trServer.GetChildItem(hChildItem);
			while(hChildItem1 != NULL)
			{
				m_trServer.SortChildren(hChildItem1);
				hChildItem1 = m_trServer.GetNextSiblingItem(hChildItem1);
			}
			hChildItem = m_trServer.GetNextSiblingItem(hChildItem);
		}
	}
	catch(...)
	{

	}
}

//删除摄像头列表树
void CDlgPageServer::DeleteCameraTreeList()
{
	HTREEITEM	hMainItem = NULL;
	_T_NODE_INFO *pNodeInfo = NULL;
	DWORD_PTR pInfo = NULL;

	try
	{
		//人工巡视
		g_VideoPageViewInfo.nStatus = AUTOVIDEO_PAGEVIEW_STATUS_INIT;
		memset(g_VideoPageViewInfo.szFirstStationName,0,sizeof(g_VideoPageViewInfo.szFirstStationName));
		
		//树形列表
		hMainItem = m_trServer.GetRootItem();
		while(hMainItem != NULL)
		{
			pInfo = m_trServer.GetItemData(hMainItem);
			if (pInfo != NULL)
			{
				pNodeInfo = (_T_NODE_INFO *)pInfo;
				delete pNodeInfo;
				pInfo = NULL;
				pNodeInfo = NULL;
				m_trServer.SetItemData(hMainItem,NULL);
			}

			DeleteCameraChildTreeListInfo(hMainItem);
			hMainItem = m_trServer.GetNextSiblingItem(hMainItem);
		}
	}
	catch(...)
	{

	}

	m_trServer.DeleteAllItems();
}

//删除摄像头树的子节点信息
void CDlgPageServer::DeleteCameraChildTreeListInfo(HTREEITEM hItem)
{
	HTREEITEM	hChildItem = NULL;
	_T_NODE_INFO *pNodeInfo = NULL;
	DWORD_PTR pInfo = NULL;

	try
	{
		hChildItem = m_trServer.GetChildItem(hItem);
		while(hChildItem != NULL)
		{
			pInfo = m_trServer.GetItemData(hChildItem);
			if (pInfo != NULL)
			{
				pNodeInfo = (_T_NODE_INFO *)pInfo;
				delete pNodeInfo;
				pInfo = NULL;
				pNodeInfo = NULL;
				m_trServer.SetItemData(hChildItem,NULL);
			}

			DeleteCameraChildTreeListInfo(hChildItem);
			hChildItem = m_trServer.GetNextSiblingItem(hChildItem);
		}
	}
	catch(...)
	{

	}
}

//删除列表树
void CDlgPageServer::DeleteCameraTreeListItemAndDataInfo(HTREEITEM hItem)
{
	HTREEITEM	hChildItem = NULL;
	_T_NODE_INFO *pNodeInfo = NULL;
	DWORD_PTR pInfo = NULL;

	try
	{
		DeleteCameraChildTreeListInfo(hItem);

		pInfo = m_trServer.GetItemData(hItem);
		if (pInfo != NULL)
		{
			pNodeInfo = (_T_NODE_INFO *)pInfo;
			delete pNodeInfo;
			pInfo = NULL;
			pNodeInfo = NULL;
			m_trServer.SetItemData(hItem,NULL);
		}

		m_trServer.DeleteItem(hItem);
	}
	catch(...)
	{

	}
}

// 初始化轮巡列表树
void CDlgPageServer::InitTuneCycleList()
{
	try
	{
		HTREEITEM	hMainItem = NULL;
		HTREEITEM	hChildItem = NULL;
		HTREEITEM	hChildItem2 = NULL;

		int id = 0;
		DWORD num = 0;
		BOOL bFlag = FALSE;

		char szPathName[256] = {0};
		char szTimeInt[32] = {0};
		char szCycleNum[32];
		char szMethod[32] = {0};
		char szMethodName[64] = {0};
		char szCamera[32] = {0};
		char szCameraID[32] = {0};
		char szCameraName[256] = {0};
		char szCameraType[32] = {0};

		_T_TUNECYCLE_NODE_INFO *pNodeInfo = NULL;

		// 删除轮巡树形列表信息
		DeleteTuneCycleTreeList();

		int i = 0;
		for (i = 0; i < g_TotalSysNum; i ++)
		{
			if (g_SystemInfo[i].localdomain == 1)
			{
				hMainItem = m_trTuneCycle.InsertItem(g_SystemInfo[i].sysname, 1, 1, TVI_ROOT, hMainItem);
				break;
			}
		}

		//////////////////////////////////////////////////////////////////////////

		memset(szPathName, 0, sizeof(szPathName));

		if (strlen(g_szAppPath) > 0)
		{
			strcpy_s(szPathName,sizeof(szPathName)-1,g_szAppPath);
		}
		else
		{
			GetCurrentDirectory(sizeof(szPathName)-1, szPathName);
		}

		strcat_s(szPathName, "\\tunecycle.ini");

		for (id = 0;id < 16;id++)
		{
			hChildItem = NULL;
			sprintf(szMethod, "method-%d", id);

			// 名称
			num = GetPrivateProfileString(szMethod,"methodname","轮巡方案",szMethodName,60,szPathName);
			if (num <= 0)
				return;

			// 时间间隔
			num = GetPrivateProfileString(szMethod,"cycletime","60",szTimeInt,30,szPathName);
			if (num <= 0)
				return;

			// 画面数目
			num = GetPrivateProfileString(szMethod,"cyclenum","4",szCycleNum,30,szPathName);
			if (num <= 0)
				return;

			for (int i = 0; i < TOTALCYCLENAMENUM; i++)
			{
				sprintf(szCamera, "cameraid-%d", i);
				num  = GetPrivateProfileString(szMethod, szCamera, "10032003", szCameraID, 30, szPathName);
				sprintf(szCamera, "cameraname-%d", i);
				num  = GetPrivateProfileString(szMethod, szCamera, "10032003", szCameraName, 250, szPathName);
				sprintf(szCamera, "cameratype-%d", i);
				num  = GetPrivateProfileString(szMethod, szCamera, "150", szCameraType, 30, szPathName);

				if (strlen(szCameraID) == 0||strlen(szCameraType) == 0)
					continue;

				if (hChildItem == NULL)
				{
					hChildItem = m_trTuneCycle.InsertItem(szMethodName, 7, 7, hMainItem);
					if (hChildItem == NULL)
						break;

					pNodeInfo = new _T_TUNECYCLE_NODE_INFO;
					if (pNodeInfo == NULL)
						continue;

					memset(pNodeInfo,0,sizeof(_T_TUNECYCLE_NODE_INFO));

					pNodeInfo->node_type = 1;
					sprintf(pNodeInfo->node_num,"%d",id);
					strcpy(pNodeInfo->node_name,szMethodName);
					pNodeInfo->node_decodetag = 0;
					pNodeInfo->nTime = atoi(szTimeInt);
					pNodeInfo->nCycleNum = atoi(szCycleNum);
					m_trTuneCycle.SetItemData(hChildItem,(DWORD_PTR)pNodeInfo);
				}

				hChildItem2 = m_trTuneCycle.InsertItem(szCameraName, 5, 5, hChildItem);
				if (hChildItem2 == NULL)
					continue;

				pNodeInfo = new _T_TUNECYCLE_NODE_INFO;
				if (pNodeInfo == NULL)
					continue;

				memset(pNodeInfo,0,sizeof(_T_TUNECYCLE_NODE_INFO));

				pNodeInfo->node_type = 2;
				strcpy(pNodeInfo->node_num,szCameraID);
				strcpy(pNodeInfo->node_name,szCameraName);
				pNodeInfo->node_decodetag = atoi(szCameraType);

				m_trTuneCycle.SetItemData(hChildItem2,(DWORD_PTR)pNodeInfo);
			}

			//设置轮巡摄像头DVR信息
			SetTreeTuneCycleStationCameraDvrInfo(hChildItem);
		}

		UpdateData(FALSE);
	}
	catch(...)
	{

	}
}

//删除轮巡树
void CDlgPageServer::DeleteTuneCycleTreeList()
{
	HTREEITEM	hMainItem = NULL;
	_T_TUNECYCLE_NODE_INFO *pNodeInfo = NULL;
	DWORD_PTR pInfo = NULL;

	try
	{
		hMainItem = m_trTuneCycle.GetRootItem();
		while(hMainItem != NULL)
		{
			pInfo = m_trTuneCycle.GetItemData(hMainItem);
			if (pInfo != NULL)
			{
				pNodeInfo = (_T_TUNECYCLE_NODE_INFO *)pInfo;
				delete pNodeInfo;
				pInfo = NULL;
				pNodeInfo = NULL;
			}

			DeleteTuneCycleChildTreeListInfo(hMainItem);
			hMainItem = m_trTuneCycle.GetNextSiblingItem(hMainItem);
		}
	}
	catch(...)
	{

	}

	m_trTuneCycle.DeleteAllItems();
}

//删除轮巡树的子结点信息
void CDlgPageServer::DeleteTuneCycleChildTreeListInfo(HTREEITEM hItem)
{
	HTREEITEM	hChildItem = NULL;
	_T_TUNECYCLE_NODE_INFO *pNodeInfo = NULL;
	DWORD_PTR pInfo = NULL;

	try
	{
		hChildItem = m_trTuneCycle.GetChildItem(hItem);
		while(hChildItem != NULL)
		{
			pInfo = m_trTuneCycle.GetItemData(hChildItem);
			if (pInfo != NULL)
			{
				pNodeInfo = (_T_TUNECYCLE_NODE_INFO *)pInfo;
				delete pNodeInfo;
				pInfo = NULL;
				pNodeInfo = NULL;
			}

			DeleteTuneCycleChildTreeListInfo(hChildItem);
			hChildItem = m_trTuneCycle.GetNextSiblingItem(hChildItem);
		}
	}
	catch(...)
	{

	}
}

//删除全部列表树
void CDlgPageServer::DeleteAllTreeList()
{
	try
	{
		DeleteCameraTreeList();
		DeletePresetTreeList();
		DeleteTuneCycleTreeList();
	}
	catch(...)
	{

	}
}

/************************************************************************
* OnTvnBegindragTreeServer
* 开始拖放
*
************************************************************************/
void CDlgPageServer::OnTvnBegindragTreeServer(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);

	try
	{
		CTreeCtrl *pList = (CTreeCtrl *)GetDlgItem(IDC_TREE_SERVER);

		HTREEITEM hItem = pNMTreeView->itemNew.hItem;	// pList->GetSelectedItem();

		m_hSelectItem = hItem;
		m_trServer.SelectItem(hItem);

		if (hItem == NULL)
			return;

		//判断是否末梢节点
		if( NULL != pList->GetChildItem(hItem))	
			return;

		// Create a drag image
		POINT pt;
		int nOffset = -10; //offset in pixels for drag image (positive is up and to the left; neg is down and to the right)
		pt.x = nOffset;
		pt.y = nOffset;

		//判断节点是否是摄像头
		m_pCameraNodeInfo = (_T_NODE_INFO *)(pList->GetItemData(hItem));

		char	    c_nodetype[4]={0x0};
		int		i_nodetype = 0;
		char	    c_devtype[4]={0x0};
		int		i = 0, j = 0;
		int		i_devtype = 0;

		c_nodetype[0] = m_pCameraNodeInfo->node_num[10];
		c_nodetype[1] = m_pCameraNodeInfo->node_num[11];
		c_devtype[0] = m_pCameraNodeInfo->node_num[12];
		c_devtype[1] = m_pCameraNodeInfo->node_num[13];

		i_nodetype = atoi(c_nodetype);
		i_devtype = atoi(c_devtype);

		if (i_nodetype == 3 && i_devtype >=1 && i_devtype <= 15)
		{
			TRACE("选中的设备为摄像头\n");
		}
		else
		{
			return;
		}

		m_pDragImage = pList->CreateDragImage(hItem);//	, &pt);
		ASSERT(m_pDragImage); //make sure it was created
		//We will call delete later (in LButtonUp) to clean this up

		CBitmap bitmap;
		bitmap.LoadBitmap(IDB_BITMAP_DRAG);
		m_pDragImage->Replace(0, &bitmap, &bitmap);

		m_pDragImage->BeginDrag(0, CPoint(nOffset, nOffset - 4));

		m_pDragImage->DragEnter(GetDesktopWindow(), pNMTreeView->action); // ->ptAction);

		// Set dragging flag and others
		m_bDragging = TRUE;	//we are in a drag and drop operation
		//	m_nDropIndex = -1;	//we don't have a drop index yet
		//	m_pDragList = &m_listL; //make note of which list we are dragging from
		//	m_pDropWnd = &m_listL;	//at present the drag list is the drop list

		m_preCh = -1;

		// Capture all mouse messages
		SetCapture();
	}
	catch(...)
	{

	}

	*pResult = 0;
}

//拖放过程中的鼠标移动
void CDlgPageServer::OnMouseMove(UINT nFlags, CPoint point)
{
	try
	{
		if (m_bDragging)
		{
			// Move the drag image
			CPoint pt(point);	//get our current mouse coordinates
			ClientToScreen(&pt); //convert to screen coordinates
			m_pDragImage->DragMove(pt); //move the drag image to those coordinates

			// Unlock window updates (this allows the dragging image to be shown smoothly)
			m_pDragImage->DragShowNolock(false);

			// Get the CWnd pointer of the window that is under the mouse cursor
			CWnd* pDropWnd = WindowFromPoint (pt);
			ASSERT(pDropWnd); //make sure we have a window

			//CVEMCUCtlDlg *pDlg = (CVEMCUCtlDlg *)AfxGetMainWnd();
			if (pDropWnd->IsKindOf(RUNTIME_CLASS(CViewChannel)))
			{
				CViewChannel *pCh = (CViewChannel *)pDropWnd;

				if (m_preCh != pCh->m_iWinID)
				{
					if (m_preCh >= 0)
						m_preCh = pCh->m_iWinID;
				}
			}
			else
			{
				if (m_preCh >= 0)
				{
					m_preCh = -1;
				}
			}
			m_pDragImage->DragShowNolock(true);
		}
	}
	catch(...)
	{

	}

	CPropertyPage::OnMouseMove(nFlags, point);
}

//拖放过程中的鼠标响应
void CDlgPageServer::OnLButtonUp(UINT nFlags, CPoint point)
{
	try
	{
		if (m_bDragging)
		{
			// Release mouse capture, so that other controls can get control/messages
			ReleaseCapture ();

			m_preCh = -1;
			// Note that we are NOT in a drag operation
			m_bDragging = FALSE;

			//结束拖放
			m_pDragImage->DragLeave (GetDesktopWindow ());
			m_pDragImage->EndDrag ();
			delete m_pDragImage; //删除对象
			m_pDragImage = NULL;

			CPoint pt(point); //得到当前鼠标位置
			ClientToScreen (&pt); //转换到屏幕坐标

			for (int i = 0; i < MAXVIEWCH; i ++)
			{
				CRect rc;
				g_DlgVideoView[i].GetWindowRect(rc);

				if (pt.x >= rc.left && pt.x <= rc.right&& pt.y >= rc.top && pt.y <= rc.bottom)
				{
					g_pMainDlg->m_iFocuseWindowID = i;

					if (g_TuneCycle_MehtodInfo.bState == true)
					{
						MessageBox("当前处于轮巡状态","视频预览",MB_ICONWARNING);
						return;
					}

					if (g_VideoPageViewInfo.nStatus == AUTOVIDEO_PAGEVIEW_STATUS_RUNNING)
					{
						MessageBox("当前处于人工自动巡视状态","视频预览",MB_ICONWARNING);
						return;
					}

					//////////////////////////////////////////////////////////////////////////
					VIDEO_SIP_CALL_SERVER_INFO VideoSipCallServerInfo;
					memset(&VideoSipCallServerInfo,0,sizeof(VideoSipCallServerInfo));

					if (m_pCameraNodeInfo == NULL)
						return;

					if (IsCameraVideoLinkByVideoPlatform(g_nClientVideoLinkType,m_pCameraNodeInfo->node_decodetag))//客户端视频连接方式
					{
						VideoSipCallServerInfo.nType = 1;//通过南瑞平台
						VideoSipCallServerInfo.nDecodeTag = m_pCameraNodeInfo->node_decodetag;
					}
					else
					{
						VideoSipCallServerInfo.nType = 2;//直接连接前端设备
						VideoSipCallServerInfo.nDecodeTag = m_pCameraNodeInfo->camera_info.dvr_info.dvr_type;
					}

					VideoSipCallServerInfo.nStatus = m_pCameraNodeInfo->node_status;
					strcpy_s(VideoSipCallServerInfo.szCode,sizeof(VideoSipCallServerInfo.szCode),m_pCameraNodeInfo->node_num);
					VideoSipCallServerInfo.nScreenId = i;
					strcpy_s(VideoSipCallServerInfo.szName,sizeof(VideoSipCallServerInfo.szName),m_pCameraNodeInfo->node_name);
					VideoSipCallServerInfo.hItem = m_hSelectItem;
					strcpy_s(VideoSipCallServerInfo.szStation,sizeof(VideoSipCallServerInfo.szStation),m_pCameraNodeInfo->node_station);
					memcpy(&VideoSipCallServerInfo.server_node_info,m_pCameraNodeInfo,sizeof(VideoSipCallServerInfo.server_node_info));

					g_ThreadVideoOperateNumberInfo.DeviceVideoInNumerAdd();
					g_pMainDlg->ThreadPoolDispatchTask(ThreadMakeCallCameraServer,(void *)&VideoSipCallServerInfo,sizeof(VideoSipCallServerInfo),2);
					//////////////////////////////////////////////////////////////////////////

					if (g_pMainDlg->m_iFocuseWindowID < g_pMainDlg->m_FormsNum)
						g_pMainDlg->DrawFocusChannel(g_pMainDlg->m_iFocuseWindowID, FALSE);

					break;
				}
			}
		}
	}
	catch(...)
	{

	}

	CPropertyPage::OnLButtonUp(nFlags, point);
}

void CDlgPageServer::TreeVisit(HTREEITEM hItem)
{
	if(m_trServer.ItemHasChildren(hItem))
	{
		HTREEITEM   hChildItem = m_trServer.GetChildItem(hItem);
		if (hChildItem != NULL)
		{
			m_hTotalItem[m_TotalItemNum] = hChildItem;
			TRACE("select item:%d, %s\n", m_TotalItemNum, m_trServer.GetItemText(m_hTotalItem[m_TotalItemNum]));
			m_TotalItemNum ++;
		}

		while (hChildItem != NULL)
		{
			TreeVisit(hChildItem); //递归遍历孩子节点
			hChildItem  = m_trServer.GetNextItem(hChildItem, TVGN_NEXT);
			if (hChildItem != NULL)
			{
				m_hTotalItem[m_TotalItemNum] = hChildItem;
				TRACE("select item:%d, %s\n", m_TotalItemNum, m_trServer.GetItemText(m_hTotalItem[m_TotalItemNum]));
				m_TotalItemNum ++;
			}
		}
	}
}

void CDlgPageServer::OnBnClickedButtonPsDevicelist()
{
	try
	{
		if (m_nRelationListShow == 1)
		{
			// 之前显示放大的
			g_pMainDlg->ShowSmallRelationList();
			m_nRelationListShow = 0;
		}

		g_pMainDlg->m_bPageViewInfoFlag = FALSE;
		g_pMainDlg->m_nPageViewInfoHeight = 0;

		// 列表区域
		GetDlgItem(IDC_TREE_SERVER)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_TREE_PRESET)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_TREE_TUNECYCLE)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_LIST_PS_RELATION)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_LIST_PS_CAMERA_RELATION)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_BUTTON_RELATION_SHOW)->ShowWindow(SW_HIDE);

		// 云镜控制区域
		GetDlgItem(IDC_BUTTON_CHOOSEYT)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_BUTTON_CHOOSEVIEW)->ShowWindow(SW_SHOW);

		GetDlgItem(IDC_YT_ZOOMIN)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_YT_ZOOMOUT)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_YT_LEFT)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_YT_UP)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_YT_DOWN)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_YT_RIGHT)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_YT_SAVE)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_YT_CALL)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_YT_DIAPHRAGM_LARGE)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_YT_DIAPHRAGM_SMALL)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_YT_RESURVE1)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_YT_RESURVE2)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_COMBO_SPEED)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_STATIC_SPEED)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_YT_LOCK)->ShowWindow(SW_SHOW);
		//		GetDlgItem(IDC_YT_LOCK)->ShowWindow(SW_HIDE);

		m_DlgPreSet->ShowWindow(SW_HIDE);

		m_DlgVideoParSet->ShowWindow(SW_HIDE);

		m_btnListDevice.SetIndex(0);
		m_btnListDevice.Invalidate(true);
		m_btnListCamera.SetIndex(1);
		m_btnListCamera.Invalidate(true);
		m_btnListRelation.SetIndex(1);
		m_btnListRelation.Invalidate(true);
		m_btnListCycle.SetIndex(1);
		m_btnListCycle.Invalidate(true);

		g_pMainDlg->SetForms(g_pMainDlg->m_FormsNum,TRUE);

		Invalidate(TRUE);
	}
	catch(...)
	{

	}
}

void CDlgPageServer::OnBnClickedButtonPsCameralist()
{
	try
	{
		if (g_pMainDlg == NULL)
			return;

		if (m_nRelationListShow == 1)
		{
			// 之前显示放大的
			g_pMainDlg->ShowSmallRelationList();
			m_nRelationListShow = 0;
		}

		g_pMainDlg->m_bPageViewInfoFlag = TRUE;
		g_pMainDlg->m_nPageViewInfoHeight = PAGEVIEWHEIGHT;

		// 列表区域
		GetDlgItem(IDC_TREE_SERVER)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_TREE_PRESET)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_TREE_TUNECYCLE)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_LIST_PS_RELATION)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_LIST_PS_CAMERA_RELATION)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_BUTTON_RELATION_SHOW)->ShowWindow(SW_HIDE);

		// 云镜控制区域
		GetDlgItem(IDC_BUTTON_CHOOSEYT)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_BUTTON_CHOOSEVIEW)->ShowWindow(SW_SHOW);

		GetDlgItem(IDC_YT_ZOOMIN)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_YT_ZOOMOUT)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_YT_LEFT)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_YT_UP)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_YT_DOWN)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_YT_RIGHT)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_YT_SAVE)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_YT_CALL)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_YT_DIAPHRAGM_LARGE)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_YT_DIAPHRAGM_SMALL)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_YT_RESURVE1)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_YT_RESURVE2)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_COMBO_SPEED)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_STATIC_SPEED)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_YT_LOCK)->ShowWindow(SW_SHOW);
		//		GetDlgItem(IDC_YT_LOCK)->ShowWindow(SW_HIDE);

		if (m_DlgPreSet != NULL)
		{
			m_DlgPreSet->ShowWindow(SW_HIDE);
		}
		
		if(m_DlgVideoParSet != NULL)
		{
			m_DlgVideoParSet->ShowWindow(SW_HIDE);
		}
		
		m_btnListDevice.SetIndex(1);
		m_btnListDevice.Invalidate(true);
		m_btnListCamera.SetIndex(0);
		m_btnListCamera.Invalidate(true);
		m_btnListRelation.SetIndex(1);
		m_btnListRelation.Invalidate(true);
		m_btnListCycle.SetIndex(1);
		m_btnListCycle.Invalidate(true);

		g_pMainDlg->SetForms(g_pMainDlg->m_FormsNum,TRUE);

		Invalidate(TRUE);
	}
	catch(...)
	{

	}
}

void CDlgPageServer::OnBnClickedButtonPsCycleList()
{
	try
	{
		if (g_pMainDlg == NULL)
			return;

		if (m_nRelationListShow == 1)
		{
			// 之前显示放大的
			g_pMainDlg->ShowSmallRelationList();
			m_nRelationListShow = 0;
		}

		g_pMainDlg->m_bPageViewInfoFlag = FALSE;
		g_pMainDlg->m_nPageViewInfoHeight = 0;

		// 列表区域
		GetDlgItem(IDC_TREE_TUNECYCLE)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_TREE_SERVER)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_TREE_PRESET)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_LIST_PS_RELATION)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_LIST_PS_CAMERA_RELATION)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_BUTTON_RELATION_SHOW)->ShowWindow(SW_HIDE);

		// 云镜控制区域
		GetDlgItem(IDC_BUTTON_CHOOSEYT)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_BUTTON_CHOOSEVIEW)->ShowWindow(SW_SHOW);

		GetDlgItem(IDC_YT_ZOOMIN)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_YT_ZOOMOUT)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_YT_LEFT)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_YT_UP)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_YT_DOWN)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_YT_RIGHT)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_YT_SAVE)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_YT_CALL)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_YT_DIAPHRAGM_LARGE)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_YT_DIAPHRAGM_SMALL)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_YT_RESURVE1)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_YT_RESURVE2)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_COMBO_SPEED)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_STATIC_SPEED)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_YT_LOCK)->ShowWindow(SW_SHOW);
		//		GetDlgItem(IDC_YT_LOCK)->ShowWindow(SW_HIDE);

		if (m_DlgPreSet != NULL)
		{
			m_DlgPreSet->ShowWindow(SW_HIDE);
		}
		
		if (m_DlgVideoParSet != NULL)
		{
			m_DlgVideoParSet->ShowWindow(SW_HIDE);
		}
		
		m_btnListDevice.SetIndex(1);
		m_btnListDevice.Invalidate(true);
		m_btnListCamera.SetIndex(1);
		m_btnListCamera.Invalidate(true);
		m_btnListRelation.SetIndex(1);
		m_btnListRelation.Invalidate(true);
		m_btnListCycle.SetIndex(0);
		m_btnListCycle.Invalidate(true);

		g_pMainDlg->SetForms(g_pMainDlg->m_FormsNum,TRUE);

		Invalidate(TRUE);
	}
	catch(...)
	{

	}
}


void CDlgPageServer::OnBnClickedButtonPsRelationlist()
{
	try
	{
		// 列表区域
		GetDlgItem(IDC_TREE_SERVER)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_TREE_PRESET)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_TREE_TUNECYCLE)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_LIST_PS_RELATION)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_LIST_PS_CAMERA_RELATION)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_BUTTON_RELATION_SHOW)->ShowWindow(SW_SHOW);

		// 云镜控制区域
		GetDlgItem(IDC_BUTTON_CHOOSEYT)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_BUTTON_CHOOSEVIEW)->ShowWindow(SW_HIDE);

		GetDlgItem(IDC_YT_ZOOMIN)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_YT_ZOOMOUT)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_YT_LEFT)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_YT_UP)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_YT_DOWN)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_YT_RIGHT)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_YT_SAVE)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_YT_CALL)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_YT_DIAPHRAGM_LARGE)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_YT_DIAPHRAGM_SMALL)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_YT_RESURVE1)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_YT_RESURVE2)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_COMBO_SPEED)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STATIC_SPEED)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_YT_LOCK)->ShowWindow(SW_HIDE);

		if (m_DlgPreSet != NULL)
		{
			m_DlgPreSet->ShowWindow(SW_HIDE);
		}
		
		if (m_DlgVideoParSet != NULL)
		{
			m_DlgVideoParSet->ShowWindow(SW_HIDE);
		}
		
		m_btnListDevice.SetIndex(1);
		m_btnListDevice.Invalidate(true);
		m_btnListCamera.SetIndex(1);
		m_btnListCamera.Invalidate(true);
		m_btnListRelation.SetIndex(0);
		m_btnListRelation.Invalidate(true);
		m_btnListCycle.SetIndex(1);
		m_btnListCycle.Invalidate(true);

		Invalidate(TRUE);
	}
	catch(...)
	{

	}
}

void CDlgPageServer::OnBnClickedButtonRelationShow()
{
	if (g_pMainDlg == NULL)
		return;

	if (m_nRelationListShow == 0)
	{
		// 之前显示缩小的
		g_pMainDlg->ShowLargeRelationList();
		m_nRelationListShow = 1;
	}
	else
	{
		// 之前显示放大的
		g_pMainDlg->ShowSmallRelationList();
		m_nRelationListShow = 0;
	}
}

//右键单击轮巡列表
void CDlgPageServer::OnNMRClickTreeTunecycle(NMHDR *pNMHDR, LRESULT *pResult)
{
	*pResult = 0;

	POINT pt;
	UINT uFlag = 0;

	GetCursorPos(&pt);
	m_trTuneCycle.ScreenToClient(&pt);
	HTREEITEM hItem = m_trTuneCycle.HitTest(pt,&uFlag);
	if (hItem == NULL)
		return;

	if ((uFlag & TVHT_ONITEMICON) == 0 && (uFlag & TVHT_ONITEMLABEL) == 0)
		return;

	m_trTuneCycle.SelectItem(hItem);

	_T_TUNECYCLE_NODE_INFO *pNodeInfo = (_T_TUNECYCLE_NODE_INFO *)m_trTuneCycle.GetItemData(hItem);

	if (pNodeInfo == NULL)
		return;

	if (pNodeInfo->node_type != 1&&pNodeInfo->node_type != 2)
		return;

	HTREEITEM hParentItem = NULL;

	if (pNodeInfo->node_type == 2)
	{
		hParentItem = m_trTuneCycle.GetParentItem(hItem);
		if (hParentItem == NULL)
			return;

		m_trTuneCycle.SelectItem(hParentItem);
	}

	CMenu  popMenu;

	popMenu.LoadMenu(IDR_MENU_TUNE_CYCLE);
	CMenu *pMenu = popMenu.GetSubMenu(0);
	if (pMenu == NULL)
		return;

	if (g_TuneCycle_MehtodInfo.bState == false)
	{
		pMenu->EnableMenuItem(0,MF_BYPOSITION|MF_ENABLED);
		pMenu->EnableMenuItem(1,MF_BYPOSITION|MF_GRAYED|MF_DISABLED);
	}
	else
	{
		pMenu->EnableMenuItem(0,MF_BYPOSITION|MF_GRAYED|MF_DISABLED);
		pMenu->EnableMenuItem(1,MF_BYPOSITION|MF_ENABLED);
	}

	GetCursorPos(&pt);

	pMenu->TrackPopupMenu(TPM_RIGHTBUTTON|TPM_RIGHTALIGN,  pt.x, pt.y, this); 
}

//开始轮巡
void CDlgPageServer::OnMenuStartTunecycle()
{
	try
	{
		if (g_TuneCycle_MehtodInfo.bState == true)
		{
			MessageBox("当前正在轮巡中","视频监视");
			return;
		}

		if (g_VideoPageViewInfo.nStatus == AUTOVIDEO_PAGEVIEW_STATUS_RUNNING)
		{
			MessageBox("当前处于人工自动巡视状态","视频预览",MB_ICONWARNING);
			return;
		}

		HTREEITEM hItem = m_trTuneCycle.GetSelectedItem();
		if (hItem == NULL)
			return;

		_T_TUNECYCLE_NODE_INFO *pNodeInfo  = (_T_TUNECYCLE_NODE_INFO *)m_trTuneCycle.GetItemData(hItem);
		if (pNodeInfo == NULL)
			return;

		if (pNodeInfo->node_type != 1)
			return;

		if(m_trTuneCycle.ItemHasChildren(hItem) == FALSE)
			return;

		int j = 0;
		int nTotalCameraNum = 0;
		HTREEITEM hChildItem = NULL;

		g_TuneCycle_MehtodInfo.MethodID = atoi(pNodeInfo->node_num);
		strcpy_s(g_TuneCycle_MehtodInfo.MethodName,sizeof(g_TuneCycle_MehtodInfo.MethodName),pNodeInfo->node_name);
		g_TuneCycle_MehtodInfo.TimeInt = pNodeInfo->nTime;
		g_TuneCycle_MehtodInfo.ViewNum = pNodeInfo->nCycleNum;

		hChildItem = m_trTuneCycle.GetChildItem(hItem);

		while(hChildItem != NULL)
		{
			pNodeInfo  = (_T_TUNECYCLE_NODE_INFO *)m_trTuneCycle.GetItemData(hChildItem);
			if (pNodeInfo != NULL&&pNodeInfo->node_type == 2)
			{
				g_TuneCycle_MehtodInfo.CurrentID = 0;
				g_TuneCycle_MehtodInfo.PlayFlag[j] = false;
				strcpy(g_TuneCycle_MehtodInfo.CameraNum[j],pNodeInfo->node_num);
				strcpy(g_TuneCycle_MehtodInfo.CameraName[j],pNodeInfo->node_name);
				g_TuneCycle_MehtodInfo.CameraType[j] = pNodeInfo->node_decodetag;
				j++;
				nTotalCameraNum++;
			}

			hChildItem = m_trTuneCycle.GetNextSiblingItem(hChildItem);
		}

		g_TuneCycle_MehtodInfo.TotalCameraNum = nTotalCameraNum;

		g_TuneCycle_MehtodInfo.bState = true;

		UpdateData(FALSE);

		g_pMainDlg->TuneCycleStart();

		g_pMainDlg->PostMessage(OM_CONTROLBUTTON, IDC_MENU_VIEW,VEM_CONTROL_BUTTON_MESSAGE_TYPE_MENU);
	}
	catch(...)
	{

	}

}

//结束轮巡
void CDlgPageServer::OnMenuStopTunecycle()
{
	try
	{
		if (g_TuneCycle_MehtodInfo.bState == false)
		{
			MessageBox("当前无轮巡","视频监视");
			return;
		}

		g_TuneCycle_MehtodInfo.bState = false;

		UpdateData(FALSE);

		g_pMainDlg->TuneCycleEnd();
		g_pMainDlg->PostMessage(OM_CONTROLBUTTON, IDC_MENU_VIEW,VEM_CONTROL_BUTTON_MESSAGE_TYPE_MENU);
	}
	catch(...)
	{

	}

}

void CDlgPageServer::OnDestroy()
{
	CPropertyPage::OnDestroy();

	//////////////////////////////////////////////////////////////////////////
	if (m_DlgPreSet != NULL)
	{
		m_DlgPreSet->DestroyWindow();
		delete m_DlgPreSet;
		m_DlgPreSet = NULL;
	}

	if (m_DlgVideoParSet != NULL)
	{
		m_DlgVideoParSet->DestroyWindow();
		delete m_DlgVideoParSet;
		m_DlgVideoParSet = NULL;
	}
}


//左键单击联动列表
void CDlgPageServer::OnNMClickListPsRelation(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);

	try
	{
		//////////////////////////////////////////////////////////////////////////
		// 在摄像头列表中显示关联摄像头
		POSITION ps =m_RealRelationList.GetFirstSelectedItemPosition();
		int nIndex  = -1;

		char szLinkageType[64] = {0};
		char szStationName[256] = {0};
		char szDeviceName[256] = {0};
		char szSubType[64] = {0};
		char szLinkageTime[64] = {0};

		int nNodeId = -1;
		int nLength = 0;

		if (ps != NULL)
		{
			nIndex = m_RealRelationList.GetNextSelectedItem(ps);
			nLength = m_RealRelationList.GetItemText(nIndex,0,szLinkageType,sizeof(szLinkageType));
			nLength = m_RealRelationList.GetItemText(nIndex,1,szDeviceName,sizeof(szDeviceName));
			nLength = m_RealRelationList.GetItemText(nIndex,2,szStationName,sizeof(szStationName));
			nLength = m_RealRelationList.GetItemText(nIndex,3,szLinkageTime,sizeof(szLinkageTime));
			nLength = m_RealRelationList.GetItemText(nIndex,4,szSubType,sizeof(szSubType));

			nNodeId = GetRelationListInfoId(szLinkageType,szStationName,szDeviceName,szSubType,szLinkageTime);

			if (nNodeId >= 0)
				ReadAndSetCameraRelationListInfo(nNodeId,szStationName,szLinkageTime);
		}
	}
	catch(...)
	{

	}

	*pResult = 0;
}


//左键双击联动列表
void CDlgPageServer::OnNMDblclkListPsRelation(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);

	try
	{
		if (g_TuneCycle_MehtodInfo.bState == true)
		{
			MessageBox("当前处于轮巡状态","视频预览",MB_ICONWARNING);
			return;
		}

		if (g_VideoPageViewInfo.nStatus == AUTOVIDEO_PAGEVIEW_STATUS_RUNNING)
		{
			MessageBox("当前处于人工自动巡视状态","视频预览",MB_ICONWARNING);
			return;
		}

		//////////////////////////////////////////////////////////////////////////
		// 打开所有摄像头视频

		char szName[256] = {0};
		char szCode[64] = {0};
		char szDecodeTag[16] = {0};
		char szStationName[256] = {0};
		char szReason[256] = {0x0};

		int nLength = 0;

		int nScreenId = 0;

		int nCount = m_RealCameraRelationList.GetItemCount();

		for (int nIndex = 0;nIndex < nCount;nIndex++)
		{
			nLength = m_RealCameraRelationList.GetItemText(nIndex,0,szName,sizeof(szName));
			nLength = m_RealCameraRelationList.GetItemText(nIndex,1,szCode,sizeof(szCode));
			nLength = m_RealCameraRelationList.GetItemText(nIndex,2,szDecodeTag,sizeof(szDecodeTag));
			nLength = m_RealCameraRelationList.GetItemText(nIndex,3,szStationName,sizeof(szStationName));

			sprintf(szReason, "{热点区域监视}-{%s}-{%s}",szName, szStationName);
			if (g_pMainDlg != NULL)
			{
				VIDEO_SIP_CALL_SERVER_INFO VideoSipCallServerInfo;
				memset(&VideoSipCallServerInfo,0,sizeof(VideoSipCallServerInfo));

				VideoSipCallServerInfo.nType = 1;
				VideoSipCallServerInfo.nStatus = 1;
				strcpy_s(VideoSipCallServerInfo.szCode,sizeof(VideoSipCallServerInfo.szCode),szCode);
				strcpy_s(VideoSipCallServerInfo.szName,sizeof(VideoSipCallServerInfo.szName),szName);
				VideoSipCallServerInfo.nDecodeTag = atoi(szDecodeTag);
				VideoSipCallServerInfo.nScreenId = nScreenId+nIndex;
				strcpy_s(VideoSipCallServerInfo.szReason,sizeof(VideoSipCallServerInfo.szReason),szReason);

				g_ThreadVideoOperateNumberInfo.DeviceVideoInNumerAdd();
				g_pMainDlg->ThreadPoolDispatchTask(ThreadMakeCallCameraServer,(void *)&VideoSipCallServerInfo,sizeof(VideoSipCallServerInfo),2);
			}
		}

	}
	catch(...)
	{

	}


	*pResult = 0;
}


//左键单击联动摄像头列表
void CDlgPageServer::OnNMDblclkListPsCameraRelation(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);

	try
	{
		if (g_TuneCycle_MehtodInfo.bState == true)
		{
			MessageBox("当前处于轮巡状态","视频预览",MB_ICONWARNING);
			return;
		}

		if (g_VideoPageViewInfo.nStatus == AUTOVIDEO_PAGEVIEW_STATUS_RUNNING)
		{
			MessageBox("当前处于人工自动巡视状态","视频预览",MB_ICONWARNING);
			return;
		}

		//////////////////////////////////////////////////////////////////////////
		// 在摄像头列表中显示关联摄像头
		POSITION ps =m_RealCameraRelationList.GetFirstSelectedItemPosition();
		int nIndex  = -1;

		char szName[256] = {0};
		char szCode[64] = {0};
		char szDecodeTag[16] = {0};
		char szStationName[256] = {0};
		char szReason[256] = {0x0};

		int nLength = 0;

		int nScreenId = 0;

		if (ps != NULL)
		{
			nIndex = m_RealCameraRelationList.GetNextSelectedItem(ps);
			nLength = m_RealCameraRelationList.GetItemText(nIndex,0,szName,sizeof(szName));
			nLength = m_RealCameraRelationList.GetItemText(nIndex,1,szCode,sizeof(szCode));
			nLength = m_RealCameraRelationList.GetItemText(nIndex,2,szDecodeTag,sizeof(szDecodeTag));
			nLength = m_RealCameraRelationList.GetItemText(nIndex,3,szStationName,sizeof(szStationName));

			sprintf(szReason, "{热点区域监视}-{%s}-{%s}",szName, szStationName);
			if (g_pMainDlg != NULL)
			{
				if (g_pMainDlg->m_iFocuseWindowID >= 0&&g_pMainDlg->m_iFocuseWindowID < 4)
					nScreenId = g_pMainDlg->m_iFocuseWindowID;

				VIDEO_SIP_CALL_SERVER_INFO VideoSipCallServerInfo;
				memset(&VideoSipCallServerInfo,0,sizeof(VideoSipCallServerInfo));

				VideoSipCallServerInfo.nType = 1;
				VideoSipCallServerInfo.nStatus = 1;
				strcpy_s(VideoSipCallServerInfo.szCode,sizeof(VideoSipCallServerInfo.szCode),szCode);
				strcpy_s(VideoSipCallServerInfo.szName,sizeof(VideoSipCallServerInfo.szName),szName);
				VideoSipCallServerInfo.nDecodeTag = atoi(szDecodeTag);
				VideoSipCallServerInfo.nScreenId = nScreenId+nIndex;
				strcpy_s(VideoSipCallServerInfo.szReason,sizeof(VideoSipCallServerInfo.szReason),szReason);

				g_ThreadVideoOperateNumberInfo.DeviceVideoInNumerAdd();
				g_pMainDlg->ThreadPoolDispatchTask(ThreadMakeCallCameraServer,(void *)&VideoSipCallServerInfo,sizeof(VideoSipCallServerInfo),2);
			}
		}
	}
	catch(...)
	{

	}

	*pResult = 0;
}

void CDlgPageServer::OnNMDblclkTreeTunecycle(NMHDR *pNMHDR, LRESULT *pResult)
{
	POINT pt;
	UINT uFlag = 0;

	GetCursorPos(&pt);
	m_trTuneCycle.ScreenToClient(&pt);
	HTREEITEM hItem = m_trTuneCycle.HitTest(pt,&uFlag);
	if (hItem == NULL)
		return;

	if ((uFlag & TVHT_ONITEMICON) == 0 && (uFlag & TVHT_ONITEMLABEL) == 0)
		return;

	m_trTuneCycle.SelectItem(hItem);

	_T_TUNECYCLE_NODE_INFO *pNodeInfo = (_T_TUNECYCLE_NODE_INFO *)m_trTuneCycle.GetItemData(hItem);

	if (pNodeInfo == NULL)
		return;

	if (pNodeInfo->node_type != 2)
		return;

	MessageBox("当前的操作不被支持,请使用右键菜单","视频监视");

	*pResult = 0;
}

HTREEITEM CDlgPageServer::SearchCameraTreeCameraHandleTreeItem(HTREEITEM hTreeItem,char *szCameraNum,char *szCameraName,char *szStationName)
{
	try
	{
		if (szCameraNum == NULL||strlen(szCameraNum) == 0)
			return NULL;

		HTREEITEM hResultTreeItem = NULL;
		_T_NODE_INFO *pNodeInfo = NULL;

		if (hTreeItem == NULL)
		{
			hTreeItem = m_trServer.GetRootItem();
			if (hTreeItem == NULL)
				return NULL;

			while(hTreeItem != NULL)
			{
				hResultTreeItem = SearchCameraTreeCameraHandleTreeItem(hTreeItem,szCameraNum,szCameraName,szStationName);
				if (hResultTreeItem != NULL)
					return hResultTreeItem;

				hTreeItem = m_trServer.GetNextSiblingItem(hTreeItem);
			}

			return NULL;
		}
		
		pNodeInfo = (_T_NODE_INFO *)m_trServer.GetItemData(hTreeItem);

		if (pNodeInfo != NULL)
		{
			if(pNodeInfo->node_type == 3&&strcmp(pNodeInfo->node_num,szCameraNum) == 0)
				return hTreeItem;
		}

		HTREEITEM hChildTreeItem = m_trServer.GetChildItem(hTreeItem);

		while(hChildTreeItem != NULL)
		{
			hResultTreeItem = SearchCameraTreeCameraHandleTreeItem(hChildTreeItem,szCameraNum,szCameraName,szStationName);
			if (hResultTreeItem != NULL)
				return hResultTreeItem;

			hChildTreeItem = m_trServer.GetNextSiblingItem(hChildTreeItem);
		}

		return NULL;

	}
	catch(...)
	{

	}

	return NULL;
}

HTREEITEM CDlgPageServer::SearchPresetTreeCameraHandleTreeItem(HTREEITEM hTreeItem,char *szCameraNum,char *szCameraName,char *szStationName)
{
	try
	{
		if (szCameraNum == NULL||strlen(szCameraNum) == 0)
			return NULL;

		if (hTreeItem == NULL)
		{
			hTreeItem = m_trPreset.GetRootItem();
			if (hTreeItem == NULL)
				return NULL;
		}

		_T_NODE_PRESET_INFO *pNodePresetInfo = NULL;
		HTREEITEM hResultTreeItem = NULL;

		pNodePresetInfo = (_T_NODE_PRESET_INFO *)m_trPreset.GetItemData(hTreeItem);

		if (pNodePresetInfo != NULL)
		{
			if((pNodePresetInfo->node_type == 3||pNodePresetInfo->node_type == 5)&&strcmp(pNodePresetInfo->node_num,szCameraNum) == 0)
				return hTreeItem;
		}

		HTREEITEM hChildTreeItem = m_trPreset.GetChildItem(hTreeItem);

		while(hChildTreeItem != NULL)
		{
			hResultTreeItem = SearchPresetTreeCameraHandleTreeItem(hChildTreeItem,szCameraNum,szCameraName,szStationName);
			if (hResultTreeItem != NULL)
				return hResultTreeItem;

			hChildTreeItem = m_trPreset.GetNextSiblingItem(hChildTreeItem);
		}

		return NULL;
	}
	catch(...)
	{

	}
	return NULL;
}

HTREEITEM CDlgPageServer::SearchPresetTreePresetHandleTreeItem(HTREEITEM hTreeItem,char *szCameraNum,char *szPresetName)
{
	try
	{
		if (szCameraNum == NULL||strlen(szCameraNum) == 0||szPresetName == NULL||strlen(szPresetName) == 0)
			return NULL;

		if (hTreeItem == NULL)
		{
			hTreeItem = m_trPreset.GetRootItem();
			if (hTreeItem == NULL)
				return NULL;
		}

		_T_NODE_PRESET_INFO *pNodePresetInfo = NULL;
		HTREEITEM hResultTreeItem = NULL;

		pNodePresetInfo = (_T_NODE_PRESET_INFO *)m_trPreset.GetItemData(hTreeItem);

		if (pNodePresetInfo != NULL)
		{
			if(pNodePresetInfo->node_type == 5&&strcmp(pNodePresetInfo->node_num,szCameraNum) == 0&&strcmp(pNodePresetInfo->preset_name,szPresetName) == 0)
				return hTreeItem;
		}

		HTREEITEM hChildTreeItem = m_trPreset.GetChildItem(hTreeItem);

		while(hChildTreeItem != NULL)
		{
			hResultTreeItem = SearchPresetTreePresetHandleTreeItem(hChildTreeItem,szCameraNum,szPresetName);
			if (hResultTreeItem != NULL)
				return hResultTreeItem;

			hChildTreeItem = m_trPreset.GetNextSiblingItem(hChildTreeItem);
		}

		return NULL;
	}
	catch(...)
	{

	}
	return NULL;
}

HTREEITEM CDlgPageServer::SearchCameraTreeStationHandleTreeItem(HTREEITEM hTreeItem,char *szStationNum,char *szStationName)
{
	try
	{
		if (szStationNum == NULL||strlen(szStationNum) == 0)
			return NULL;

		if (hTreeItem == NULL)
		{
			hTreeItem = m_trServer.GetRootItem();
			if (hTreeItem == NULL)
				return NULL;
		}

		_T_NODE_INFO *pNodeInfo = NULL;
		HTREEITEM hResultTreeItem = NULL;

		pNodeInfo = (_T_NODE_INFO *)m_trServer.GetItemData(hTreeItem);

		if (pNodeInfo != NULL)
		{
			if(pNodeInfo->node_type == 2&&strcmp(pNodeInfo->node_num,szStationNum) == 0)
				return hTreeItem;
		}

		HTREEITEM hChildTreeItem = m_trServer.GetChildItem(hTreeItem);

		while(hChildTreeItem != NULL)
		{
			hResultTreeItem = SearchCameraTreeStationHandleTreeItem(hChildTreeItem,szStationNum,szStationName);
			if (hResultTreeItem != NULL)
				return hResultTreeItem;

			hChildTreeItem = m_trServer.GetNextSiblingItem(hChildTreeItem);
		}

		return NULL;

	}
	catch(...)
	{

	}

	return NULL;

}

HTREEITEM CDlgPageServer::SearchPresetTreeStationHandleTreeItem(HTREEITEM hTreeItem,char *szStationNum,char *szStationName)
{
	try
	{
		if (szStationNum == NULL||strlen(szStationNum) == 0)
			return NULL;

		if (hTreeItem == NULL)
		{
			hTreeItem = m_trPreset.GetRootItem();
			if (hTreeItem == NULL)
				return NULL;
		}

		_T_NODE_PRESET_INFO *pNodePresetInfo = NULL;
		HTREEITEM hResultTreeItem = NULL;

		pNodePresetInfo = (_T_NODE_PRESET_INFO *)m_trPreset.GetItemData(hTreeItem);

		if (pNodePresetInfo != NULL)
		{
			if(pNodePresetInfo->node_type == 2&&strcmp(pNodePresetInfo->node_num,szStationNum) == 0)
				return hTreeItem;
		}

		HTREEITEM hChildTreeItem = m_trPreset.GetChildItem(hTreeItem);

		while(hChildTreeItem != NULL)
		{
			hResultTreeItem = SearchPresetTreeStationHandleTreeItem(hChildTreeItem,szStationNum,szStationName);
			if (hResultTreeItem != NULL)
				return hResultTreeItem;

			hChildTreeItem = m_trPreset.GetNextSiblingItem(hChildTreeItem);
		}

		return NULL;
	}
	catch(...)
	{

	}
	return NULL;
}

//设置摄像头状态
BOOL CDlgPageServer::SetTreeCameraHandleTreeItemAndDatabaseStatus(int nStatus,char *szCameraNum,char *szCameraName,char *szStationName,BOOL bDisplayInfo)
{
	if (szCameraNum == NULL||strlen(szCameraNum) == 0||g_pMainDlg == NULL)
		return FALSE;

	if (nStatus < VM_CAMERA_STATUS_NOTAG_OFFLINE||nStatus > VM_CAMERA_STATUS_TAG_BY_STATUS)
		return FALSE;

	try
	{
		HTREEITEM hTreeItem = NULL;
		HTREEITEM hParentItem = NULL;
		HTREEITEM hParentItem2 = NULL;
		_T_NODE_INFO *pNodeInfo = NULL;
		_T_NODE_INFO *pParentNodeInfo = NULL;
		_T_NODE_PRESET_INFO *pNodePresetInfo = NULL;
		_T_NODE_PRESET_INFO *pParentNodePresetInfo = NULL;

		char sql_buf[1024] = {0};
		char szTime[64] = {0};
		int nStationIndex = 0;

		BOOL bDatabaseUpdateFlag = TRUE;

		SYSTEMTIME sysTm;
		::GetLocalTime(&sysTm);

		sprintf_s(szTime,"%04d-%02d-%02d %02d:%02d:%02d",sysTm.wYear,sysTm.wMonth,sysTm.wDay,sysTm.wHour,sysTm.wMinute,sysTm.wSecond);

		//手动巡视
		hTreeItem = SearchCameraTreeCameraHandleTreeItem(NULL,szCameraNum,szCameraName,szStationName);
		if (hTreeItem != NULL)
		{
			pNodeInfo = (_T_NODE_INFO *)m_trServer.GetItemData(hTreeItem);

			if (pNodeInfo != NULL&&pNodeInfo->node_type == 3&&strcmp(pNodeInfo->node_num,szCameraNum) == 0)//摄像头
			{
				if (g_pMainDlg->CheckUserStationViewPower(hTreeItem,1) == false)
				{
					if (bDisplayInfo != FALSE)
					{
						MessageBox("用户没有对此设备进行\"标注缺陷\"和\"取消标注\"的权限","视频监视");
					}
					return FALSE;
				}

				switch(nStatus)
				{
				case VM_CAMERA_STATUS_NOTAG_OFFLINE:
					{
						if (pNodeInfo->node_status == VM_CAMERA_STATUS_TAG_ONLINE
							||pNodeInfo->node_status == VM_CAMERA_STATUS_NOTAG_ONLINE)
							return FALSE;

						pNodeInfo->node_status = VM_CAMERA_STATUS_NOTAG_OFFLINE;
						m_trServer.SetItemImage(hTreeItem,4,4);
					}
					break;

				case VM_CAMERA_STATUS_NOTAG_ONLINE:
					{
						if (pNodeInfo->node_status == VM_CAMERA_STATUS_NOTAG_OFFLINE
							||pNodeInfo->node_status == VM_CAMERA_STATUS_TAG_OFFLINE)
							return FALSE;

						pNodeInfo->node_status = VM_CAMERA_STATUS_NOTAG_ONLINE;

						if (pNodeInfo->node_streamless == 1)
						{
							m_trServer.SetItemImage(hTreeItem,4,4);
						}
						else
						{
							m_trServer.SetItemImage(hTreeItem,5,5);
						}
					}
					break;

				case VM_CAMERA_STATUS_TAG_ONLINE:
					{
						if (pNodeInfo->node_status == VM_CAMERA_STATUS_NOTAG_OFFLINE
							||pNodeInfo->node_status == VM_CAMERA_STATUS_TAG_OFFLINE)
							return FALSE;

						pNodeInfo->node_status = VM_CAMERA_STATUS_TAG_ONLINE;

						if (pNodeInfo->node_streamless == 1)
						{
							m_trServer.SetItemImage(hTreeItem,11,11);
						}
						else
						{
							m_trServer.SetItemImage(hTreeItem,9,9);
						}
					}
					break;

				case VM_CAMERA_STATUS_TAG_OFFLINE:
					{
						if (pNodeInfo->node_status == VM_CAMERA_STATUS_TAG_ONLINE
							||pNodeInfo->node_status == VM_CAMERA_STATUS_NOTAG_ONLINE)
							return FALSE;

						pNodeInfo->node_status = VM_CAMERA_STATUS_TAG_OFFLINE;
						m_trServer.SetItemImage(hTreeItem,11,11);
					}
					break;

				case VM_CAMERA_STATUS_NO_TAG_BY_STATUS:
					{
						if (pNodeInfo->node_status == VM_CAMERA_STATUS_TAG_ONLINE)
						{
							pNodeInfo->node_status = VM_CAMERA_STATUS_NOTAG_ONLINE;
							if (pNodeInfo->node_streamless == 1)
							{
								m_trServer.SetItemImage(hTreeItem,4,4);
							}
							else
							{
								m_trServer.SetItemImage(hTreeItem,5,5);
							}
						}
						else if (pNodeInfo->node_status == VM_CAMERA_STATUS_TAG_OFFLINE)
						{
							pNodeInfo->node_status = VM_CAMERA_STATUS_NOTAG_OFFLINE;
							m_trServer.SetItemImage(hTreeItem,4,4);
						}
						else
							return FALSE;
					}
					break;

				case VM_CAMERA_STATUS_TAG_BY_STATUS:
					{
						if (pNodeInfo->node_status == VM_CAMERA_STATUS_NOTAG_ONLINE)
						{
							pNodeInfo->node_status = VM_CAMERA_STATUS_TAG_ONLINE;
							if (pNodeInfo->node_streamless == 1)
							{
								m_trServer.SetItemImage(hTreeItem,11,11);
							}
							else
							{
								m_trServer.SetItemImage(hTreeItem,9,9);
							}
						}
						else if (pNodeInfo->node_status == VM_CAMERA_STATUS_NOTAG_OFFLINE)
						{
							pNodeInfo->node_status = VM_CAMERA_STATUS_TAG_OFFLINE;
							m_trServer.SetItemImage(hTreeItem,11,11);
						}
						else
							return FALSE;
					}
					break;

				default:
					return FALSE;
				}

				//更新数据库
				hParentItem = m_trServer.GetParentItem(hTreeItem);
				if (hParentItem != NULL)
				{
					pParentNodeInfo = (_T_NODE_INFO *)m_trServer.GetItemData(hParentItem);

					if (pParentNodeInfo != NULL&&pParentNodeInfo->node_type == 2)//站点
					{
						sprintf(sql_buf,"UPDATE ob_d5000_camera_station_%d SET camera_status=%d,notes='%s' WHERE camera_code='%s' ",
							pParentNodeInfo->node_id,pNodeInfo->node_status,szTime,pNodeInfo->node_num);
						mysql_query(g_mySqlData, sql_buf);

						sprintf(sql_buf,"UPDATE ct_user_station_view_history_%d_%04d%02d SET status=%d,time='%s' WHERE camera_code='%s' AND day=%d ",
							g_userpower.userID,sysTm.wYear,sysTm.wMonth,pNodeInfo->node_status,szTime,pNodeInfo->node_num,sysTm.wDay);
						mysql_query(g_mySqlData, sql_buf);

						bDatabaseUpdateFlag = FALSE;	
					}
				}
			}
		}

		//设备组
		hTreeItem = SearchPresetTreeCameraHandleTreeItem(NULL,szCameraNum,szCameraName,szStationName);
		if (hTreeItem == NULL)
			return TRUE;

		hParentItem = m_trPreset.GetParentItem(hTreeItem);
		if (hParentItem == NULL)
			return TRUE;

		hParentItem2 = hParentItem;

		while(hParentItem2 != NULL)//区域或设备
		{
			hTreeItem = m_trPreset.GetChildItem(hParentItem2);

			while (hTreeItem != NULL)//预置位
			{
				pNodePresetInfo = (_T_NODE_PRESET_INFO *)m_trPreset.GetItemData(hTreeItem);
				if (pNodePresetInfo != NULL)
				{
					if (strcmp(pNodePresetInfo->node_num,szCameraNum) == 0)//判断是否为要查找的结点
					{
						if (g_pMainDlg->CheckUserStationViewPower(hTreeItem,2) == false)
						{
							if (bDisplayInfo != FALSE)
							{
								MessageBox("用户没有对此设备进行\"标注缺陷\"和\"取消标注\"的权限","视频监视");
							}
							return FALSE;
						}

						switch(nStatus)
						{
						case VM_CAMERA_STATUS_NOTAG_OFFLINE:
							{
								if (pNodePresetInfo->node_status == VM_CAMERA_STATUS_NOTAG_ONLINE
									||pNodePresetInfo->node_status == VM_CAMERA_STATUS_TAG_ONLINE)
									return FALSE;

								pNodePresetInfo->node_status = VM_CAMERA_STATUS_NOTAG_OFFLINE;
								m_trPreset.SetItemImage(hTreeItem,4,4);
							}
							break;

						case VM_CAMERA_STATUS_NOTAG_ONLINE:
							{
								if (pNodePresetInfo->node_status == VM_CAMERA_STATUS_NOTAG_OFFLINE
									||pNodePresetInfo->node_status == VM_CAMERA_STATUS_TAG_OFFLINE)
									return FALSE;

								pNodePresetInfo->node_status = VM_CAMERA_STATUS_NOTAG_ONLINE;
								if (pNodePresetInfo->node_streamless == 1)
								{
									m_trPreset.SetItemImage(hTreeItem,4,4);
								}
								else
								{
									m_trPreset.SetItemImage(hTreeItem,5,5);
								}
							}
							break;

						case VM_CAMERA_STATUS_TAG_ONLINE:
							{
								if (pNodePresetInfo->node_status == VM_CAMERA_STATUS_NOTAG_OFFLINE
									||pNodePresetInfo->node_status == VM_CAMERA_STATUS_TAG_OFFLINE)
									return FALSE;

								pNodePresetInfo->node_status = VM_CAMERA_STATUS_TAG_ONLINE;
								if (pNodePresetInfo->node_streamless == 1)
								{
									m_trPreset.SetItemImage(hTreeItem,11,11);
								}
								else
								{
									m_trPreset.SetItemImage(hTreeItem,9,9);
								}
							}
							break;

						case VM_CAMERA_STATUS_TAG_OFFLINE:
							{
								if (pNodePresetInfo->node_status == VM_CAMERA_STATUS_NOTAG_ONLINE
									||pNodePresetInfo->node_status == VM_CAMERA_STATUS_TAG_ONLINE)
									return FALSE;

								pNodePresetInfo->node_status = VM_CAMERA_STATUS_TAG_OFFLINE;
								m_trPreset.SetItemImage(hTreeItem,11,11);
							}
							break;

						case VM_CAMERA_STATUS_NO_TAG_BY_STATUS:
							{
								if (pNodePresetInfo->node_status == VM_CAMERA_STATUS_TAG_ONLINE)
								{
									pNodePresetInfo->node_status = VM_CAMERA_STATUS_NOTAG_ONLINE;
									if (pNodePresetInfo->node_streamless == 1)
									{
										m_trPreset.SetItemImage(hTreeItem,4,4);
									}
									else
									{
										m_trPreset.SetItemImage(hTreeItem,5,5);
									}
								}
								else if (pNodePresetInfo->node_status == VM_CAMERA_STATUS_TAG_OFFLINE)
								{
									pNodePresetInfo->node_status = VM_CAMERA_STATUS_NOTAG_OFFLINE;
									m_trPreset.SetItemImage(hTreeItem,4,4);
								}
								else
									return FALSE;
							}
							break;

						case VM_CAMERA_STATUS_TAG_BY_STATUS:
							{
								if (pNodePresetInfo->node_status == VM_CAMERA_STATUS_NOTAG_ONLINE)
								{
									pNodePresetInfo->node_status = VM_CAMERA_STATUS_TAG_ONLINE;
									if (pNodePresetInfo->node_streamless == 1)
									{
										m_trPreset.SetItemImage(hTreeItem,11,11);
									}
									else
									{
										m_trPreset.SetItemImage(hTreeItem,9,9);
									}
								}
								else if (pNodePresetInfo->node_status == VM_CAMERA_STATUS_NOTAG_OFFLINE)
								{
									pNodePresetInfo->node_status = VM_CAMERA_STATUS_TAG_OFFLINE;
									m_trPreset.SetItemImage(hTreeItem,11,11);
								}
								else
									return FALSE;
							}
							break;

						default:
							return FALSE;
						}

						//更新数据库
						if (bDatabaseUpdateFlag != FALSE)
						{
							hParentItem = m_trPreset.GetParentItem(hTreeItem);
							if (hParentItem != NULL)
							{
								pParentNodePresetInfo = (_T_NODE_PRESET_INFO *)m_trPreset.GetItemData(hParentItem);
								if (pParentNodePresetInfo != NULL&&(pParentNodePresetInfo->node_type == 7||pParentNodePresetInfo->node_type == 6))//设备或区域
								{
									hParentItem = m_trPreset.GetParentItem(hParentItem);
									pParentNodePresetInfo = (_T_NODE_PRESET_INFO *)m_trPreset.GetItemData(hParentItem);
									if (pParentNodePresetInfo != NULL&&pParentNodePresetInfo->node_type == 2)//站点
									{
										sprintf(sql_buf,"UPDATE ob_d5000_camera_station_%d SET camera_status=%d,notes='%s' WHERE camera_code='%s' ",
											pParentNodePresetInfo->node_id,pNodePresetInfo->node_status,szTime,pNodePresetInfo->node_num);
										mysql_query(g_mySqlData, sql_buf);

										sprintf(sql_buf,"UPDATE ct_user_station_view_history_%d_%04d%02d SET status=%d,time='%s' WHERE camera_code='%s' AND day=%d ",
											g_userpower.userID,sysTm.wYear,sysTm.wMonth,pNodePresetInfo->node_status,szTime,pNodePresetInfo->node_num,sysTm.wDay);
										mysql_query(g_mySqlData, sql_buf);

										bDatabaseUpdateFlag = FALSE;	
									}
								}
							}
						}//if (bDatabaseUpdateFlag != FALSE)
					}
				}
				hTreeItem = m_trPreset.GetNextSiblingItem(hTreeItem);
			}
			hParentItem2 = m_trPreset.GetNextSiblingItem(hParentItem2);
		}

		return TRUE;
	}
	catch(...)
	{

	}

	return FALSE;
}

//设置摄像头状态
BOOL CDlgPageServer::AutoSetTreeCameraHandleTreeItemAndDatabaseStatus(int nStatus,char *szCameraNum,char *szCameraName,char *szStationName,BOOL bDisplayInfo)
{
	if (szCameraNum == NULL||strlen(szCameraNum) == 0||g_pMainDlg == NULL)
		return FALSE;

	if (nStatus < VM_CAMERA_STATUS_OFFLINE_BY_STATUS||nStatus > VM_CAMERA_STATUS_ONLINE_BY_STATUS)
		return FALSE;

	try
	{
		HTREEITEM hTreeItem = NULL;
		HTREEITEM hParentItem = NULL;
		HTREEITEM hParentItem2 = NULL;
		_T_NODE_INFO *pNodeInfo = NULL;
		_T_NODE_INFO *pParentNodeInfo = NULL;
		_T_NODE_PRESET_INFO *pNodePresetInfo = NULL;
		_T_NODE_PRESET_INFO *pParentNodePresetInfo = NULL;

		char sql_buf[1024] = {0};
		char szTime[64] = {0};
		int nStationIndex = 0;

		BOOL bDatabaseUpdateFlag = TRUE;
		BOOL bTrServerStationSetStatusOnline = FALSE;
		BOOL bTrServerStationSetStatusOffline = FALSE;
		BOOL bTrPresetStationSetStatusOnline = FALSE;
		BOOL bTrPresetStationSetStatusOffline = FALSE;

		SYSTEMTIME sysTm;
		::GetLocalTime(&sysTm);

		sprintf_s(szTime,"%04d-%02d-%02d %02d:%02d:%02d",sysTm.wYear,sysTm.wMonth,sysTm.wDay,sysTm.wHour,sysTm.wMinute,sysTm.wSecond);

		//手动巡视
		hTreeItem = SearchCameraTreeCameraHandleTreeItem(NULL,szCameraNum,szCameraName,szStationName);
		if (hTreeItem != NULL)
		{
			pNodeInfo = (_T_NODE_INFO *)m_trServer.GetItemData(hTreeItem);

			if (pNodeInfo != NULL&&pNodeInfo->node_type == 3&&strcmp(pNodeInfo->node_num,szCameraNum) == 0)//摄像头
			{
				switch(nStatus)
				{
				case VM_CAMERA_STATUS_OFFLINE_BY_STATUS:
					{
						if (pNodeInfo->node_status == VM_CAMERA_STATUS_NOTAG_OFFLINE
							||pNodeInfo->node_status == VM_CAMERA_STATUS_NOTAG_ONLINE)
						{
							pNodeInfo->node_status = VM_CAMERA_STATUS_NOTAG_OFFLINE;
							m_trServer.SetItemImage(hTreeItem,4,4);
						}
						else if (pNodeInfo->node_status == VM_CAMERA_STATUS_TAG_ONLINE
							||pNodeInfo->node_status == VM_CAMERA_STATUS_TAG_OFFLINE)
						{
							pNodeInfo->node_status = VM_CAMERA_STATUS_TAG_OFFLINE;
							m_trServer.SetItemImage(hTreeItem,11,11);
						}
						else
						{
							return FALSE;
						}
					}
					break;

				case VM_CAMERA_STATUS_ONLINE_BY_STATUS:
					{
						if (pNodeInfo->node_status == VM_CAMERA_STATUS_NOTAG_OFFLINE
							||pNodeInfo->node_status == VM_CAMERA_STATUS_NOTAG_ONLINE)
						{
							pNodeInfo->node_status = VM_CAMERA_STATUS_NOTAG_ONLINE;
							if (pNodeInfo->node_streamless == 1)
							{
								m_trServer.SetItemImage(hTreeItem,4,4);
							}
							else
							{
								m_trServer.SetItemImage(hTreeItem,5,5);
								bTrServerStationSetStatusOnline = TRUE;
							}
						}
						else if (pNodeInfo->node_status == VM_CAMERA_STATUS_TAG_ONLINE
							||pNodeInfo->node_status == VM_CAMERA_STATUS_TAG_OFFLINE)
						{
							pNodeInfo->node_status = VM_CAMERA_STATUS_TAG_ONLINE;
							if (pNodeInfo->node_streamless == 1)
							{
								m_trServer.SetItemImage(hTreeItem,11,11);
							}
							else
							{
								m_trServer.SetItemImage(hTreeItem,9,9);
								bTrServerStationSetStatusOnline = TRUE;
							}
						}
						else
						{
							return FALSE;
						}

						//设置站点在线
						if (bTrServerStationSetStatusOnline != FALSE)
						{
							hParentItem = m_trServer.GetParentItem(hTreeItem);
							if (hParentItem != NULL)
							{
								pParentNodeInfo = (_T_NODE_INFO *)m_trServer.GetItemData(hParentItem);

								if (pParentNodeInfo != NULL&&pParentNodeInfo->node_type == 2)//站点
								{
									pParentNodeInfo->node_status = 1;
									m_trServer.SetItemImage(hParentItem,8,8);
								}
							}
						}

						//设置站点离线
						if (bTrServerStationSetStatusOffline != FALSE)
						{
							hParentItem = m_trServer.GetParentItem(hTreeItem);
							if (hParentItem != NULL)
							{
								pParentNodeInfo = (_T_NODE_INFO *)m_trServer.GetItemData(hParentItem);

								if (pParentNodeInfo != NULL&&pParentNodeInfo->node_type == 2)//站点
								{
									pParentNodeInfo->node_status = 0;
									m_trServer.SetItemImage(hParentItem,10,10);
								}
							}
						}
					}
					break;

				default:
					return FALSE;
				}

				//更新数据库
				hParentItem = m_trServer.GetParentItem(hTreeItem);
				if (hParentItem != NULL)
				{
					pParentNodeInfo = (_T_NODE_INFO *)m_trServer.GetItemData(hParentItem);

					if (pParentNodeInfo != NULL&&pParentNodeInfo->node_type == 2)//站点
					{
						sprintf(sql_buf,"UPDATE ob_d5000_camera_station_%d SET camera_status=%d,notes='%s' WHERE camera_code='%s' ",
							pParentNodeInfo->node_id,pNodeInfo->node_status,szTime,pNodeInfo->node_num);
						mysql_query(g_mySqlData, sql_buf);

						sprintf(sql_buf,"UPDATE ct_user_station_view_history_%d_%04d%02d SET status=%d,time='%s' WHERE camera_code='%s' AND day=%d ",
							g_userpower.userID,sysTm.wYear,sysTm.wMonth,pNodeInfo->node_status,szTime,pNodeInfo->node_num,sysTm.wDay);
						mysql_query(g_mySqlData, sql_buf);

						//更新数据库站点状态
						if (bTrServerStationSetStatusOnline != FALSE)
						{
							sprintf(sql_buf,"UPDATE ob_d5000_station SET station_video_status=1 WHERE station_code_videoplant='%s' ",
								pParentNodeInfo->node_num);
							mysql_query(g_mySqlData, sql_buf);
						}

						if (bTrServerStationSetStatusOffline != FALSE)
						{
							sprintf(sql_buf,"UPDATE ob_d5000_station SET station_video_status=0 WHERE station_code_videoplant='%s' ",
								pParentNodeInfo->node_num);
							mysql_query(g_mySqlData, sql_buf);
						}

						bDatabaseUpdateFlag = FALSE;	
					}
				}
			}
		}

		//设备组
		hTreeItem = SearchPresetTreeCameraHandleTreeItem(NULL,szCameraNum,szCameraName,szStationName);
		if (hTreeItem == NULL)
			return TRUE;

		hParentItem = m_trPreset.GetParentItem(hTreeItem);
		if (hParentItem == NULL)
			return TRUE;

		hParentItem2 = hParentItem;

		while(hParentItem2 != NULL)//区域或设备
		{
			hTreeItem = m_trPreset.GetChildItem(hParentItem2);

			while (hTreeItem != NULL)//预置位
			{
				pNodePresetInfo = (_T_NODE_PRESET_INFO *)m_trPreset.GetItemData(hTreeItem);
				if (pNodePresetInfo != NULL)
				{
					if (strcmp(pNodePresetInfo->node_num,szCameraNum) == 0)//判断是否为要查找的结点
					{
						switch(nStatus)
						{
						case VM_CAMERA_STATUS_OFFLINE_BY_STATUS:
							{
								if (pNodePresetInfo->node_status == VM_CAMERA_STATUS_NOTAG_OFFLINE
									||pNodePresetInfo->node_status == VM_CAMERA_STATUS_NOTAG_ONLINE)
								{
									pNodePresetInfo->node_status = VM_CAMERA_STATUS_NOTAG_OFFLINE;
									m_trPreset.SetItemImage(hTreeItem,4,4);
								}
								else if (pNodePresetInfo->node_status == VM_CAMERA_STATUS_TAG_ONLINE
									||pNodePresetInfo->node_status == VM_CAMERA_STATUS_TAG_OFFLINE)
								{
									pNodePresetInfo->node_status = VM_CAMERA_STATUS_TAG_OFFLINE;
									m_trPreset.SetItemImage(hTreeItem,11,11);
								}
								else
								{
									return FALSE;
								}
							}
							break;

						case VM_CAMERA_STATUS_ONLINE_BY_STATUS:
							{
								if (pNodePresetInfo->node_status == VM_CAMERA_STATUS_NOTAG_OFFLINE
									||pNodePresetInfo->node_status == VM_CAMERA_STATUS_NOTAG_ONLINE)
								{
									pNodePresetInfo->node_status = VM_CAMERA_STATUS_NOTAG_ONLINE;
									if (pNodePresetInfo->node_streamless == 1)
									{
										m_trPreset.SetItemImage(hTreeItem,4,4);
									}
									else
									{
										m_trPreset.SetItemImage(hTreeItem,5,5);
										bTrPresetStationSetStatusOnline = TRUE;
									}
								}
								else if (pNodePresetInfo->node_status == VM_CAMERA_STATUS_TAG_ONLINE
									||pNodePresetInfo->node_status == VM_CAMERA_STATUS_TAG_OFFLINE)
								{
									pNodePresetInfo->node_status = VM_CAMERA_STATUS_TAG_ONLINE;
									if (pNodePresetInfo->node_streamless == 1)
									{
										m_trPreset.SetItemImage(hTreeItem,11,11);
									}
									else
									{
										m_trPreset.SetItemImage(hTreeItem,9,9);
										bTrPresetStationSetStatusOnline = TRUE;
									}
								}
								else
								{
									return FALSE;
								}

								//设置站点在线
								if (bTrPresetStationSetStatusOnline != FALSE)
								{
									hParentItem = m_trPreset.GetParentItem(hTreeItem);
									if (hParentItem != NULL)
									{
										pParentNodePresetInfo = (_T_NODE_PRESET_INFO *)m_trPreset.GetItemData(hParentItem);
										if (pParentNodePresetInfo != NULL&&(pParentNodePresetInfo->node_type == 7||pParentNodePresetInfo->node_type == 6))//设备或区域
										{
											hParentItem = m_trPreset.GetParentItem(hParentItem);
											if (hParentItem != NULL)
											{
												pParentNodePresetInfo = (_T_NODE_PRESET_INFO *)m_trPreset.GetItemData(hParentItem);
												if (pParentNodePresetInfo != NULL&&pParentNodePresetInfo->node_type == 2)//站点
												{
													pParentNodePresetInfo->node_status = 1;
													m_trPreset.SetItemImage(hParentItem,8,8);
												}
											}
										}
									}
								}

								//设置站点离线
								if (bTrPresetStationSetStatusOffline != FALSE)
								{
									hParentItem = m_trPreset.GetParentItem(hTreeItem);
									if (hParentItem != NULL)
									{
										pParentNodePresetInfo = (_T_NODE_PRESET_INFO *)m_trPreset.GetItemData(hParentItem);
										if (pParentNodePresetInfo != NULL&&(pParentNodePresetInfo->node_type == 7||pParentNodePresetInfo->node_type == 6))//设备或区域
										{
											hParentItem = m_trPreset.GetParentItem(hParentItem);
											if (hParentItem != NULL)
											{
												pParentNodePresetInfo = (_T_NODE_PRESET_INFO *)m_trPreset.GetItemData(hParentItem);
												if (pParentNodePresetInfo != NULL&&pParentNodePresetInfo->node_type == 2)//站点
												{
													pParentNodePresetInfo->node_status = 0;
													m_trPreset.SetItemImage(hParentItem,10,10);
												}
											}
										}
									}
								}
							}
							break;

						default:
							return FALSE;
						}

						//更新数据库
						if (bDatabaseUpdateFlag != FALSE)
						{
							hParentItem = m_trPreset.GetParentItem(hTreeItem);
							if (hParentItem != NULL)
							{
								pParentNodePresetInfo = (_T_NODE_PRESET_INFO *)m_trPreset.GetItemData(hParentItem);
								if (pParentNodePresetInfo != NULL&&(pParentNodePresetInfo->node_type == 7||pParentNodePresetInfo->node_type == 6))//设备或区域
								{
									hParentItem = m_trPreset.GetParentItem(hParentItem);
									pParentNodePresetInfo = (_T_NODE_PRESET_INFO *)m_trPreset.GetItemData(hParentItem);
									if (pParentNodePresetInfo != NULL&&pParentNodePresetInfo->node_type == 2)//站点
									{
										sprintf(sql_buf,"UPDATE ob_d5000_camera_station_%d SET camera_status=%d,notes='%s' WHERE camera_code='%s' ",
											pParentNodePresetInfo->node_id,pNodePresetInfo->node_status,szTime,pNodePresetInfo->node_num);
										mysql_query(g_mySqlData, sql_buf);

										sprintf(sql_buf,"UPDATE ct_user_station_view_history_%d_%04d%02d SET status=%d,time='%s' WHERE camera_code='%s' AND day=%d ",
											g_userpower.userID,sysTm.wYear,sysTm.wMonth,pNodePresetInfo->node_status,szTime,pNodePresetInfo->node_num,sysTm.wDay);
										mysql_query(g_mySqlData, sql_buf);

										//设置数据库站点状态
										if (bTrPresetStationSetStatusOnline != FALSE)
										{
											sprintf(sql_buf,"UPDATE ob_d5000_station SET station_video_status=1 WHERE station_code_videoplant='%s' ",
												pParentNodePresetInfo->node_num);
											mysql_query(g_mySqlData, sql_buf);
										}

										if (bTrPresetStationSetStatusOffline != FALSE)
										{
											sprintf(sql_buf,"UPDATE ob_d5000_station SET station_video_status=0 WHERE station_code_videoplant='%s' ",
												pParentNodePresetInfo->node_num);
											mysql_query(g_mySqlData, sql_buf);
										}

										bDatabaseUpdateFlag = FALSE;	
									}
								}
							}
						}//if (bDatabaseUpdateFlag != FALSE)
					}
				}
				hTreeItem = m_trPreset.GetNextSiblingItem(hTreeItem);
			}
			hParentItem2 = m_trPreset.GetNextSiblingItem(hParentItem2);
		}

		return TRUE;
	}
	catch(...)
	{

	}
	return FALSE;
}

//取消摄像头标注
void CDlgPageServer::OnMenuitemCancelCameraTag()
{
	try
	{
		CWnd *pServerTreeWnd = GetDlgItem(IDC_TREE_SERVER);
		CWnd *pPresetTreeWnd = GetDlgItem(IDC_TREE_PRESET);

		HTREEITEM hItem = NULL;
		_T_NODE_INFO *pNodeInfo = NULL;
		_T_NODE_PRESET_INFO *pNodePresetInfo = NULL;

		char szCameraNum[64] = {0};
		char szCameraName[260] = {0};
		char szStationName[260] = {0};

		int nStatus = -1;

		if(pServerTreeWnd != NULL&&pServerTreeWnd->IsWindowVisible())
		{
			// 查找点中的树结点项目
			hItem = m_trServer.GetSelectedItem();
			if (hItem == NULL)
				return;

			pNodeInfo = (_T_NODE_INFO *)m_trServer.GetItemData(hItem);
			if (pNodeInfo == NULL||pNodeInfo->node_type != 3)
				return;

			if (pNodeInfo->node_status == VM_CAMERA_STATUS_TAG_ONLINE)
				nStatus = VM_CAMERA_STATUS_NOTAG_ONLINE;
			else if (pNodeInfo->node_status == VM_CAMERA_STATUS_TAG_OFFLINE)
				nStatus = VM_CAMERA_STATUS_NOTAG_OFFLINE;
			else
				return;

			strcpy_s(szCameraNum,sizeof(szCameraNum),pNodeInfo->node_num);
			strcpy_s(szCameraName,sizeof(szCameraName),pNodeInfo->node_name);
			strcpy_s(szStationName,sizeof(szStationName),pNodeInfo->node_station);
		}
		else if (pPresetTreeWnd != NULL&&pPresetTreeWnd->IsWindowVisible())
		{
			// 查找点中的树结点项目
			hItem = m_trPreset.GetSelectedItem();
			if (hItem == NULL)
				return;

			pNodePresetInfo = (_T_NODE_PRESET_INFO *)m_trPreset.GetItemData(hItem);
			if (pNodePresetInfo == NULL||(pNodePresetInfo->node_type != 3&&pNodePresetInfo->node_type != 5))
				return;

			if (pNodePresetInfo->node_status == VM_CAMERA_STATUS_TAG_ONLINE)
				nStatus = VM_CAMERA_STATUS_NOTAG_ONLINE;
			else if (pNodePresetInfo->node_status == VM_CAMERA_STATUS_TAG_OFFLINE)
				nStatus = VM_CAMERA_STATUS_NOTAG_OFFLINE;
			else
				return;

			strcpy_s(szCameraNum,sizeof(szCameraNum),pNodePresetInfo->node_num);
			strcpy_s(szCameraName,sizeof(szCameraName),pNodePresetInfo->node_name);
			strcpy_s(szStationName,sizeof(szStationName),pNodePresetInfo->node_station);
		}
		else
			return;

		if (g_pMainDlg != NULL)
		{
			RECT VideoRect;
			int nWinID = 0;
			if(CheckCameraVideoIsOpen(szCameraNum,nWinID) > 0)
			{
				g_VideoTagInfoDlg[nWinID].m_bVideoInfoShowFlag = FALSE;
				g_DlgVideoView[nWinID].m_Pic.GetWindowRect(&VideoRect);
				g_VideoTagInfoDlg[nWinID].SetVideoInfoCurrentIndex(1);
				g_pMainDlg->ModifyVideoTagInfoDlg(nWinID,&VideoRect,FALSE);
			}
		}

		SetTreeCameraHandleTreeItemAndDatabaseStatus(nStatus,szCameraNum,szCameraName,szStationName);
	}
	catch(...)
	{

	}
}

//标注摄像头
void CDlgPageServer::OnMenuitemTagCamera()
{
	try
	{
		CWnd *pServerTreeWnd = GetDlgItem(IDC_TREE_SERVER);
		CWnd *pPresetTreeWnd = GetDlgItem(IDC_TREE_PRESET);

		HTREEITEM hItem = NULL;
		_T_NODE_INFO *pNodeInfo = NULL;
		_T_NODE_PRESET_INFO *pNodePresetInfo = NULL;

		char szCameraNum[64] = {0};
		char szCameraName[260] = {0};
		char szStationName[260] = {0};

		int nStatus = -1;

		if(pServerTreeWnd != NULL&&pServerTreeWnd->IsWindowVisible())
		{
			// 查找点中的树结点项目
			hItem = m_trServer.GetSelectedItem();
			if (hItem == NULL)
				return;

			pNodeInfo = (_T_NODE_INFO *)m_trServer.GetItemData(hItem);
			if (pNodeInfo == NULL||pNodeInfo->node_type != 3)
				return;

			if(pNodeInfo->node_status == VM_CAMERA_STATUS_NOTAG_OFFLINE)
				nStatus = VM_CAMERA_STATUS_TAG_OFFLINE;
			else if (pNodeInfo->node_status == VM_CAMERA_STATUS_NOTAG_ONLINE)
				nStatus = VM_CAMERA_STATUS_TAG_ONLINE;
			else
				return;

			strcpy_s(szCameraNum,sizeof(szCameraNum),pNodeInfo->node_num);
			strcpy_s(szCameraName,sizeof(szCameraName),pNodeInfo->node_name);
			strcpy_s(szStationName,sizeof(szStationName),pNodeInfo->node_station);
		}
		else if (pPresetTreeWnd != NULL&&pPresetTreeWnd->IsWindowVisible())
		{
			// 查找点中的树结点项目
			hItem = m_trPreset.GetSelectedItem();
			if (hItem == NULL)
				return;

			pNodePresetInfo = (_T_NODE_PRESET_INFO *)m_trPreset.GetItemData(hItem);
			if (pNodePresetInfo == NULL||(pNodePresetInfo->node_type != 3&&pNodePresetInfo->node_type != 5))
				return;

			if (pNodePresetInfo->node_status != VM_CAMERA_STATUS_NOTAG_OFFLINE
				&&pNodePresetInfo->node_status != VM_CAMERA_STATUS_NOTAG_ONLINE)
				return;

			if (pNodePresetInfo->node_status == VM_CAMERA_STATUS_NOTAG_OFFLINE)
				nStatus = VM_CAMERA_STATUS_TAG_OFFLINE;
			else if (pNodePresetInfo->node_status == VM_CAMERA_STATUS_NOTAG_ONLINE)
				nStatus = VM_CAMERA_STATUS_TAG_ONLINE;
			else
				return;

			strcpy_s(szCameraNum,sizeof(szCameraNum),pNodePresetInfo->node_num);
			strcpy_s(szCameraName,sizeof(szCameraName),pNodePresetInfo->node_name);
			strcpy_s(szStationName,sizeof(szStationName),pNodePresetInfo->node_station);
		}
		else
			return;

		if (g_pMainDlg != NULL)
		{
			RECT VideoRect;
			int nWinID = 0;

			if (CheckCameraVideoIsOpen(szCameraNum,nWinID) > 0)
			{
				g_DlgVideoView[nWinID].m_Pic.GetWindowRect(&VideoRect);
				g_VideoTagInfoDlg[nWinID].SetVideoInfoCurrentIndex(2);
				g_pMainDlg->ModifyVideoTagInfoDlg(nWinID,&VideoRect,TRUE);
			}
		}

		SetTreeCameraHandleTreeItemAndDatabaseStatus(nStatus,szCameraNum,szCameraName,szStationName);
	}
	catch(...)
	{

	}
}

void CDlgPageServer::OnMenuitemTagClass()
{
	try
	{
		HTREEITEM hTreeItem = m_trServer.GetSelectedItem();
		if (hTreeItem == NULL)
			return;

		_T_NODE_INFO *pNodeInfo = (_T_NODE_INFO *)m_trServer.GetItemData(hTreeItem);
		if (pNodeInfo == NULL||pNodeInfo->node_type != 2)
			return;

		int nIndex = 0;

		for (nIndex = 0;nIndex < g_nStation_Num;nIndex++)
		{
			if(strcmp(g_tStation_Info[nIndex].station_code_videoplant,pNodeInfo->node_num) == 0)
				break;
		}

		if (nIndex >= g_nStation_Num)
			return;

		CDlgTagClass DlgTagClass;

		DlgTagClass.m_hTreeItem = hTreeItem;
		DlgTagClass.m_strStationName = pNodeInfo->node_name;
		DlgTagClass.m_strStationNum = pNodeInfo->node_num;
		DlgTagClass.m_strClass = g_tStation_Info[nIndex].notes;

		if (DlgTagClass.DoModal() != IDOK)
			return;

		char sql_buf[1024] = {0};
		char szStationShowName[256] = {0};

		sprintf_s(sql_buf, sizeof(sql_buf),"UPDATE ob_d5000_station SET notes='%s' "
			"WHERE station_code_videoplant='%s' ",DlgTagClass.m_strClass,DlgTagClass.m_strStationNum);

		if (!mysql_query(g_mySqlData, sql_buf))
		{
			strcpy_s(g_tStation_Info[nIndex].notes,sizeof(g_tStation_Info[nIndex].notes),DlgTagClass.m_strClass);
			if (strlen(g_tStation_Info[nIndex].notes) == 0)
				sprintf_s(szStationShowName,sizeof(szStationShowName),"%s",DlgTagClass.m_strStationName);
			else
				sprintf_s(szStationShowName,sizeof(szStationShowName),"%s(%s)",DlgTagClass.m_strStationName,DlgTagClass.m_strClass);
			m_trServer.SetItemText(hTreeItem,szStationShowName);
		}
	}
	catch(...)
	{

	}
}

void CDlgPageServer::OnNMRClickTreePreset(NMHDR *pNMHDR, LRESULT *pResult)
{
	try
	{
		POINT pt;
		::GetCursorPos(&pt);

		CWnd *pPresetTreeWnd = GetDlgItem(IDC_TREE_PRESET);

		UINT hit_flag;
		HTREEITEM h_item = NULL;

		if(pPresetTreeWnd != NULL&&pPresetTreeWnd->IsWindowVisible())
		{
			m_trPreset.ScreenToClient(&pt);

			// 查找点中的树结点项目
			h_item = m_trPreset.HitTest(pt, &hit_flag);
			if (h_item == NULL)
				return;

			if ((hit_flag & TVHT_ONITEMICON) == 0 && (hit_flag & TVHT_ONITEMLABEL) == 0)
				return;

			m_trPreset.SelectItem(h_item);
		}

		CMenu  popMenu;

		popMenu.LoadMenu(IDR_MENU_PAGESERVER_TREELIST);
		CMenu *pMenu = popMenu.GetSubMenu(0); 

		CPoint posMouse;
		GetCursorPos(&posMouse);

		pMenu->TrackPopupMenu(TPM_RIGHTBUTTON | TPM_RIGHTALIGN,  posMouse.x, posMouse.y, this); 
	}
	catch(...)
	{

	}

	*pResult = 0;
}

//人工手动巡视
void CDlgPageServer::OnMenuitemOpenStationCameraVideo()
{
	try
	{
		CDlgPageViewSelect DlgPageViewSelect;

		CWnd *pServerTreeWnd = GetDlgItem(IDC_TREE_SERVER);

		HTREEITEM hItem = NULL;
		HTREEITEM hSelectItem = NULL;
		_T_NODE_INFO *pNodeInfo = NULL;
		_T_NODE_PRESET_INFO *pNodePresetInfo = NULL;

		if (g_pMainDlg == NULL||g_pMainDlg->m_pDlgPageViewInfo == NULL)
			return;

		if(pServerTreeWnd == NULL||!pServerTreeWnd->IsWindowVisible())
			return;

		hSelectItem = m_trServer.GetSelectedItem();
		if (hSelectItem != NULL)
		{
			pNodeInfo = (_T_NODE_INFO *)m_trServer.GetItemData(hSelectItem);
			if (pNodeInfo != NULL&&pNodeInfo->node_type == 2)
			{
				DlgPageViewSelect.m_strCurrentStation = pNodeInfo->node_station;
			}
		}

		DlgPageViewSelect.m_strPreStation = g_VideoPageViewInfo.szStationName;
		DlgPageViewSelect.m_strFirstStation = g_VideoPageViewInfo.szFirstStationName;

		if (DlgPageViewSelect.DoModal() == IDCANCEL)
			return;

		switch(DlgPageViewSelect.m_nPosType)
		{
		case 0:
			{
				if (strlen(g_VideoPageViewInfo.szStationName) == 0)
					return;

				hItem = m_trServer.GetRootItem();
				if (hItem == NULL)
					return;
		
				hItem = SearchTreeCtrlByInfo(&m_trServer,hItem,g_VideoPageViewInfo.szStationName);
			}
			break;
		case 1:
			{
				hItem = hSelectItem;
			}
			break;
		case 2:
			{
				if (strlen(g_VideoPageViewInfo.szFirstStationName) == 0)
					return;

				hItem = m_trServer.GetRootItem();
				if (hItem == NULL)
					return;

				hItem = SearchTreeCtrlByInfo(&m_trServer,hItem,g_VideoPageViewInfo.szFirstStationName);
			}
			break;
		default:
			{
				hItem = hSelectItem;
			}
			break;
		}

		if (hItem == NULL)
			return;

		if (g_nPageViewOnePageMaxCameraNum == 9)
		{
			if(MessageBox("九画面易发生网络拥塞,造成视频无法打开,是否强制使用九画面?","视频巡视",MB_YESNO) == IDNO)
				return;
		}

		g_pMainDlg->m_pDlgPageViewInfo->OpenFirstPageVideo(hItem);
	}
	catch(...)
	{

	}
}

//人工自动巡视
void CDlgPageServer::OnMenuitemOpenStationCameraVideoAuto()
{
	try
	{
		CDlgPageViewSelect DlgPageViewSelect;

		CWnd *pServerTreeWnd = GetDlgItem(IDC_TREE_SERVER);

		HTREEITEM hItem = NULL;
		HTREEITEM hSelectItem = NULL;
		_T_NODE_INFO *pNodeInfo = NULL;
		_T_NODE_PRESET_INFO *pNodePresetInfo = NULL;

		if (g_pMainDlg == NULL||g_pMainDlg->m_pDlgPageViewInfo == NULL)
			return;

		if(pServerTreeWnd == NULL||!pServerTreeWnd->IsWindowVisible())
			return;

		hSelectItem = m_trServer.GetSelectedItem();
		if (hSelectItem != NULL)
		{
			pNodeInfo = (_T_NODE_INFO *)m_trServer.GetItemData(hSelectItem);
			if (pNodeInfo != NULL&&pNodeInfo->node_type == 2)
			{
				DlgPageViewSelect.m_strCurrentStation = pNodeInfo->node_station;
			}
		}

		DlgPageViewSelect.m_strPreStation = g_VideoPageViewInfo.szStationName;
		DlgPageViewSelect.m_strFirstStation = g_VideoPageViewInfo.szFirstStationName;

		if (DlgPageViewSelect.DoModal() == IDCANCEL)
			return;

		switch(DlgPageViewSelect.m_nPosType)
		{
		case 0:
			{
				if (strlen(g_VideoPageViewInfo.szStationName) == 0)
					return;

				hItem = m_trServer.GetRootItem();
				if (hItem == NULL)
					return;

				hItem = SearchTreeCtrlByInfo(&m_trServer,hItem,g_VideoPageViewInfo.szStationName);
			}
			break;
		case 1:
			{
				hItem = hSelectItem;
			}
			break;
		case 2:
			{
				if (strlen(g_VideoPageViewInfo.szFirstStationName) == 0)
					return;

				hItem = m_trServer.GetRootItem();
				if (hItem == NULL)
					return;

				hItem = SearchTreeCtrlByInfo(&m_trServer,hItem,g_VideoPageViewInfo.szFirstStationName);
			}
			break;
		default:
			{
				hItem = hSelectItem;
			}
			break;
		}

		if (hItem == NULL)
			return;

		if (g_nPageViewOnePageMaxCameraNum == 9)
		{
			if(MessageBox("九画面易发生网络拥塞,造成视频无法打开,是否强制使用九画面?","视频巡视",MB_YESNO) == IDNO)
				return;
		}

		g_pMainDlg->m_pDlgPageViewInfo->OpenFirstPageVideo(hItem);

		g_pMainDlg->m_pDlgPageViewInfo->StartAutoVideoPageView();

	}
	catch(...)
	{

	}
}

BOOL CDlgPageServer::OnTreeServerStationFlashInfo(HTREEITEM hTreeItemStation)
{
	try
	{
		if (hTreeItemStation == NULL)
			return FALSE;

		HTREEITEM hInsertItem = NULL;
		_T_NODE_INFO *pCameraNodeInfo = NULL;

		// 判断节点是何种类型节点
		pCameraNodeInfo = (_T_NODE_INFO *)m_trServer.GetItemData(hTreeItemStation);

		if (pCameraNodeInfo == NULL)
			return FALSE;

		if (pCameraNodeInfo->node_type == 2)
		{
			if (m_trServer.ItemHasChildren(hTreeItemStation))
				return TRUE;

			// 表示该节点为变电站，获取数据库中的信息，并将信息插入到数据表中
			char sql_buf[255] = {0};
			MYSQL_RES * res = NULL;
			MYSQL_ROW	row ;
			int camera_type = -1;
			int decode_tag = 0;
			int camera_status = 0;
			int streamless = 0;

			// 读取当前视频平台数量及信息
			sprintf_s(sql_buf, "SELECT camera_id, camera_code, camera_name, camera_type,camera_decodetag,camera_status,streamless "
				" FROM ob_d5000_camera_station_%d ORDER BY camera_id ASC", pCameraNodeInfo->node_id);

			if (!mysql_query(g_mySqlData, sql_buf))
			{
				res = mysql_store_result(g_mySqlData);
				while ( row = mysql_fetch_row( res ) )
				{
					camera_type = atoi(row[3]);
					if (camera_type <= 0||camera_type > 8)
						continue;

					decode_tag = atoi(row[4]);
					camera_status = atoi(row[5]);
					streamless = atoi(row[6]);

					if (camera_status == 0)
					{
						hInsertItem = m_trServer.InsertItem(row[2], 4, 4, hTreeItemStation);
					}
					else if (camera_status == 1)
					{
						if (streamless == 1)
						{
							hInsertItem = m_trServer.InsertItem(row[2], 4, 4, hTreeItemStation);
						}
						else
						{
							hInsertItem = m_trServer.InsertItem(row[2], 5, 5, hTreeItemStation);
						}
					}
					else if (camera_status == 2)
					{
						if (streamless == 1)
						{
							hInsertItem = m_trServer.InsertItem(row[2], 11, 11, hTreeItemStation);
						}
						else
						{
							hInsertItem = m_trServer.InsertItem(row[2], 9, 9, hTreeItemStation);
						}
					}
					else if (camera_status == 3)
					{
						hInsertItem = m_trServer.InsertItem(row[2], 11, 11, hTreeItemStation);
					}
					else
					{
						hInsertItem = m_trServer.InsertItem(row[2], 5, 5, hTreeItemStation);
					}

					if (hInsertItem == NULL)
						continue;

					SetCameraNodeInfo(hInsertItem, row[2], row[1], pCameraNodeInfo->node_id,
						camera_status,
						streamless,
						0,
						0,
						decode_tag,
						3,
						pCameraNodeInfo->node_station);
				}
				mysql_free_result( res ) ;
			}

			//设置摄像头DVR
			SetTreeServerStationCameraDvrInfo(hTreeItemStation);
		}
	}
	catch(...)
	{

	}
	return FALSE;
}

//设置摄像头的DVR信息
BOOL CDlgPageServer::SetTreeServerCameraDvrInfo(HTREEITEM hTreeItem)
{
	_T_NODE_INFO *pCameraNodeInfo = NULL;

	try
	{
		if (hTreeItem == NULL)
			return FALSE;

		pCameraNodeInfo = (_T_NODE_INFO *)m_trServer.GetItemData(hTreeItem);
		if (pCameraNodeInfo == NULL||pCameraNodeInfo->node_type != 3)
			return FALSE;

		_T_CAMERA_INFO * pCameraDvrInfo = &pCameraNodeInfo->camera_info;
		_T_DVR_INFO * pDvrInfo = &pCameraDvrInfo->dvr_info;

		char szDvrType[32] = {0};

		//表示该节点为摄像头信息,获取数据库中的摄像头DVR信息
		char sql_buf[1024] = {0};

		MYSQL_RES * res = NULL;
		MYSQL_ROW	row;

		// 读取摄像头DVR信息
		sprintf_s(sql_buf, "SELECT t1.id,t1.dvr_type,t1.dvr_id,t1.dvr_ip,t1.link_port,t2.channel,t1.adminuser,t1.adminpasswd,t1.station_group,t1.station_name,t1.station_seq  "
			" FROM video_dvr AS t1,video_camera AS t2 WHERE t2.camera_id='%s' AND t2.dvr_id=t1.dvr_id ORDER BY t1.id ASC", pCameraNodeInfo->node_num);

		if (!mysql_query(g_mySqlData, sql_buf))
		{
			res = mysql_store_result(g_mySqlData);
			while (row = mysql_fetch_row(res))
			{
				pDvrInfo->dvr_id = atoi(row[0]);
				strcpy_s(szDvrType,sizeof(szDvrType),row[1]);
				pDvrInfo->dvr_type = AppGetCameraDvrType(szDvrType);
				strcpy_s(pDvrInfo->dvr_num,sizeof(pDvrInfo->dvr_num),row[2]);
				strcpy_s(pDvrInfo->dvr_ip,sizeof(pDvrInfo->dvr_ip),row[3]);
				pDvrInfo->dvr_port = atoi(row[4]);
				pCameraDvrInfo->channel = atoi(row[5]);
				strcpy_s(pDvrInfo->dvr_user,sizeof(pDvrInfo->dvr_user),row[6]);
				strcpy_s(pDvrInfo->dvr_password,sizeof(pDvrInfo->dvr_password),row[7]);
				strcpy_s(pDvrInfo->station_num,sizeof(pDvrInfo->station_num),row[8]);
				strcpy_s(pDvrInfo->station_name,sizeof(pDvrInfo->station_name),row[9]);
				pDvrInfo->station_id = atoi(row[10]);
			}
			mysql_free_result(res);
		}

		return TRUE;
	}
	catch(...)
	{

	}
	return FALSE;
}

BOOL CDlgPageServer::SetTreePresetCameraDvrInfo(HTREEITEM hTreeItem)
{
	_T_NODE_PRESET_INFO *pPresetNodeInfo = NULL;

	try
	{
		if (hTreeItem == NULL)
			return FALSE;

		pPresetNodeInfo = (_T_NODE_PRESET_INFO *)m_trPreset.GetItemData(hTreeItem);
		if (pPresetNodeInfo == NULL||(pPresetNodeInfo->node_type != 3&&pPresetNodeInfo->node_type != 5))
			return FALSE;

		_T_CAMERA_INFO * pCameraDvrInfo = &pPresetNodeInfo->camera_info;
		_T_DVR_INFO * pDvrInfo = &pCameraDvrInfo->dvr_info;

		char szDvrType[32] = {0};

		//表示该节点为摄像头信息,获取数据库中的摄像头DVR信息
		char sql_buf[1024] = {0};

		MYSQL_RES * res = NULL;
		MYSQL_ROW row;

		// 读取摄像头DVR信息
		sprintf_s(sql_buf, "SELECT t1.id,t1.dvr_type,t1.dvr_id,t1.dvr_ip,t1.link_port,t2.channel,t1.adminuser,t1.adminpasswd,t1.station_group,t1.station_name,t1.station_seq  "
			" FROM video_dvr AS t1,video_camera AS t2 WHERE t2.camera_id='%s' AND t2.dvr_id=t1.dvr_id ORDER BY t1.id ASC", pPresetNodeInfo->node_num);

		if (!mysql_query(g_mySqlData, sql_buf))
		{
			res = mysql_store_result(g_mySqlData);
			while (row = mysql_fetch_row(res))
			{
				pDvrInfo->dvr_id = atoi(row[0]);
				strcpy_s(szDvrType,sizeof(szDvrType),row[1]);
				pDvrInfo->dvr_type = AppGetCameraDvrType(szDvrType);
				strcpy_s(pDvrInfo->dvr_num,sizeof(pDvrInfo->dvr_num),row[2]);
				strcpy_s(pDvrInfo->dvr_ip,sizeof(pDvrInfo->dvr_ip),row[3]);
				pDvrInfo->dvr_port = atoi(row[4]);
				pCameraDvrInfo->channel = atoi(row[5]);
				strcpy_s(pDvrInfo->dvr_user,sizeof(pDvrInfo->dvr_user),row[6]);
				strcpy_s(pDvrInfo->dvr_password,sizeof(pDvrInfo->dvr_password),row[7]);
				strcpy_s(pDvrInfo->station_num,sizeof(pDvrInfo->station_num),row[8]);
				strcpy_s(pDvrInfo->station_name,sizeof(pDvrInfo->station_name),row[9]);
				pDvrInfo->station_id = atoi(row[10]);
			}
			mysql_free_result(res);
		}

		return TRUE;
	}
	catch(...)
	{

	}
	return FALSE;
}

//设置站点摄像头DVR信息
BOOL CDlgPageServer::SetTreeServerStationCameraDvrInfo(HTREEITEM hTreeItem)
{
	_T_NODE_INFO *pCameraNodeInfo = NULL;
	HTREEITEM hChildTreeItem = NULL;
	int i = 0;

	try
	{
		if (hTreeItem == NULL)
			return FALSE;

		pCameraNodeInfo = (_T_NODE_INFO *)m_trServer.GetItemData(hTreeItem);
		if (pCameraNodeInfo == NULL||pCameraNodeInfo->node_type != 2)
			return FALSE;

		_T_CAMERA_INFO * pCameraDvrInfo = NULL;
		_T_DVR_INFO * pDvrInfo = NULL;

		char szDvrType[32] = {0};

		g_nCameraDvrInfoCount = 0;
		memset(g_CameraDvrInfo,0,sizeof(g_CameraDvrInfo));

		//表示该节点为摄像头信息,获取数据库中的摄像头DVR信息
		char sql_buf[1024] = {0};

		MYSQL_RES * res = NULL;
		MYSQL_ROW	row;

		// 读取摄像头DVR信息
		sprintf_s(sql_buf, "SELECT t1.id,t1.dvr_type,t1.dvr_id,t1.dvr_ip,t1.link_port,t2.channel,t1.adminuser,t1.adminpasswd,t1.station_group,t1.station_name,t1.station_seq,t2.camera_id  "
			" FROM video_dvr AS t1,video_camera AS t2 WHERE t1.station_group='%s' AND t2.dvr_id=t1.dvr_id ORDER BY t1.id ASC", pCameraNodeInfo->node_num);

		if (!mysql_query(g_mySqlData, sql_buf))
		{
			res = mysql_store_result(g_mySqlData);
			while (row = mysql_fetch_row(res))
			{
				if (g_nCameraDvrInfoCount >= 512)
				{
					break;
				}

				pCameraDvrInfo = &g_CameraDvrInfo[g_nCameraDvrInfoCount];
				pDvrInfo = &pCameraDvrInfo->dvr_info;

				pDvrInfo->dvr_id = atoi(row[0]);
				strcpy_s(szDvrType,sizeof(szDvrType),row[1]);
				pDvrInfo->dvr_type = AppGetCameraDvrType(szDvrType);
				strcpy_s(pDvrInfo->dvr_num,sizeof(pDvrInfo->dvr_num),row[2]);
				strcpy_s(pDvrInfo->dvr_ip,sizeof(pDvrInfo->dvr_ip),row[3]);
				pDvrInfo->dvr_port = atoi(row[4]);
				pCameraDvrInfo->channel = atoi(row[5]);
				strcpy_s(pDvrInfo->dvr_user,sizeof(pDvrInfo->dvr_user),row[6]);
				strcpy_s(pDvrInfo->dvr_password,sizeof(pDvrInfo->dvr_password),row[7]);
				strcpy_s(pDvrInfo->station_num,sizeof(pDvrInfo->station_num),row[8]);
				strcpy_s(pDvrInfo->station_name,sizeof(pDvrInfo->station_name),row[9]);
				pDvrInfo->station_id = atoi(row[10]);
				strcpy_s(pCameraDvrInfo->camera_num,sizeof(pCameraDvrInfo->camera_num),row[11]);

				g_nCameraDvrInfoCount++;
			}
			mysql_free_result(res);
		}

		//设置摄像头DVR信息
		if (m_trServer.ItemHasChildren(hTreeItem) != FALSE)
		{
			hChildTreeItem = m_trServer.GetChildItem(hTreeItem);
			while(hChildTreeItem != NULL)
			{
				pCameraNodeInfo = (_T_NODE_INFO *)m_trServer.GetItemData(hChildTreeItem);
				if (pCameraNodeInfo != NULL&&pCameraNodeInfo->node_type == 3)
				{
					for (i = 0;i < g_nCameraDvrInfoCount;i++)
					{
						if (strcmp(pCameraNodeInfo->node_num,g_CameraDvrInfo[i].camera_num) == 0)
						{
							memcpy(&pCameraNodeInfo->camera_info,&g_CameraDvrInfo[i],sizeof(g_CameraDvrInfo[i]));
						}
					}
				}
				hChildTreeItem = m_trServer.GetNextSiblingItem(hChildTreeItem);
			}
		}

		return TRUE;
	}
	catch(...)
	{

	}
	return FALSE;
}

BOOL CDlgPageServer::SetTreePresetStationCameraDvrInfo(HTREEITEM hTreeItem)
{
	_T_NODE_PRESET_INFO *pPresetNodeInfo = NULL;
	_T_NODE_PRESET_INFO *pParentPresetNodeInfo = NULL;
	HTREEITEM hChildTreeItem = NULL;
	HTREEITEM hParentTreeItem = NULL;
	int i = 0;

	try
	{
		if (hTreeItem == NULL)
			return FALSE;

		pPresetNodeInfo = (_T_NODE_PRESET_INFO *)m_trPreset.GetItemData(hTreeItem);
		if (pPresetNodeInfo == NULL||(pPresetNodeInfo->node_type != 6&&pPresetNodeInfo->node_type != 7))
			return FALSE;

		hParentTreeItem = m_trPreset.GetParentItem(hTreeItem);
		if (hParentTreeItem == NULL)
			return FALSE;

		pParentPresetNodeInfo = (_T_NODE_PRESET_INFO *)m_trPreset.GetItemData(hParentTreeItem);
		if (pParentPresetNodeInfo == NULL||pParentPresetNodeInfo->node_type != 2)
			return FALSE;

		_T_CAMERA_INFO * pCameraDvrInfo = NULL;
		_T_DVR_INFO * pDvrInfo = NULL;

		char szDvrType[32] = {0};

		g_nCameraDvrInfoCount = 0;
		memset(g_CameraDvrInfo,0,sizeof(g_CameraDvrInfo));

		//表示该节点为摄像头信息,获取数据库中的摄像头DVR信息
		char sql_buf[1024] = {0};

		MYSQL_RES * res = NULL;
		MYSQL_ROW	row;

		// 读取摄像头DVR信息
		sprintf_s(sql_buf, "SELECT t1.id,t1.dvr_type,t1.dvr_id,t1.dvr_ip,t1.link_port,t2.channel,t1.adminuser,t1.adminpasswd,t1.station_group,t1.station_name,t1.station_seq,t2.camera_id  "
			" FROM video_dvr AS t1,video_camera AS t2 WHERE t1.station_group='%s' AND t2.dvr_id=t1.dvr_id ORDER BY t1.id ASC", pParentPresetNodeInfo->node_num);

		if (!mysql_query(g_mySqlData, sql_buf))
		{
			res = mysql_store_result(g_mySqlData);
			while (row = mysql_fetch_row(res))
			{
				if (g_nCameraDvrInfoCount >= 512)
				{
					break;
				}

				pCameraDvrInfo = &g_CameraDvrInfo[g_nCameraDvrInfoCount];
				pDvrInfo = &pCameraDvrInfo->dvr_info;

				pDvrInfo->dvr_id = atoi(row[0]);
				strcpy_s(szDvrType,sizeof(szDvrType),row[1]);
				pDvrInfo->dvr_type = AppGetCameraDvrType(szDvrType);
				strcpy_s(pDvrInfo->dvr_num,sizeof(pDvrInfo->dvr_num),row[2]);
				strcpy_s(pDvrInfo->dvr_ip,sizeof(pDvrInfo->dvr_ip),row[3]);
				pDvrInfo->dvr_port = atoi(row[4]);
				pCameraDvrInfo->channel = atoi(row[5]);
				strcpy_s(pDvrInfo->dvr_user,sizeof(pDvrInfo->dvr_user),row[6]);
				strcpy_s(pDvrInfo->dvr_password,sizeof(pDvrInfo->dvr_password),row[7]);
				strcpy_s(pDvrInfo->station_num,sizeof(pDvrInfo->station_num),row[8]);
				strcpy_s(pDvrInfo->station_name,sizeof(pDvrInfo->station_name),row[9]);
				pDvrInfo->station_id = atoi(row[10]);
				strcpy_s(pCameraDvrInfo->camera_num,sizeof(pCameraDvrInfo->camera_num),row[11]);

				g_nCameraDvrInfoCount++;
			}
			mysql_free_result(res);
		}

		//设置摄像头DVR信息
		if (m_trPreset.ItemHasChildren(hTreeItem) != FALSE)
		{
			hChildTreeItem = m_trPreset.GetChildItem(hTreeItem);
			while(hChildTreeItem != NULL)
			{
				pPresetNodeInfo = (_T_NODE_PRESET_INFO *)m_trPreset.GetItemData(hChildTreeItem);
				if (pPresetNodeInfo != NULL&&(pPresetNodeInfo->node_type == 3||pPresetNodeInfo->node_type == 5))
				{
					for (i = 0;i < g_nCameraDvrInfoCount;i++)
					{
						if (strcmp(pPresetNodeInfo->node_num,g_CameraDvrInfo[i].camera_num) == 0)
						{
							memcpy(&pPresetNodeInfo->camera_info,&g_CameraDvrInfo[i],sizeof(g_CameraDvrInfo[i]));
						}
					}
				}
				hChildTreeItem = m_trPreset.GetNextSiblingItem(hChildTreeItem);
			}
		}

		return TRUE;
	}
	catch(...)
	{

	}
	return FALSE;
}

//设置自动轮巡摄像头DVR信息
BOOL CDlgPageServer::SetTreeTuneCycleStationCameraDvrInfo(HTREEITEM hTreeItem)
{
	_T_TUNECYCLE_NODE_INFO *pTuneCycleNodeInfo = NULL;
	HTREEITEM hChildTreeItem = NULL;
	int i = 0;
	int nStationId = -1;

	try
	{
		if (hTreeItem == NULL)
			return FALSE;

		pTuneCycleNodeInfo = (_T_TUNECYCLE_NODE_INFO *)m_trTuneCycle.GetItemData(hTreeItem);
		if (pTuneCycleNodeInfo == NULL||pTuneCycleNodeInfo->node_type != 1)
			return FALSE;

		//设置摄像头DVR信息
		if (m_trTuneCycle.ItemHasChildren(hTreeItem) != FALSE)
		{
			hChildTreeItem = m_trTuneCycle.GetChildItem(hTreeItem);
			while(hChildTreeItem != NULL)
			{
				pTuneCycleNodeInfo = (_T_TUNECYCLE_NODE_INFO *)m_trTuneCycle.GetItemData(hChildTreeItem);
				if (pTuneCycleNodeInfo != NULL&&pTuneCycleNodeInfo->node_type == 2)
				{
					if (strlen(pTuneCycleNodeInfo->camera_info.camera_num) == 0)
					{
						nStationId = GetTreeTuneCycleStationIdByCameraNum(pTuneCycleNodeInfo->node_num);
						if (nStationId > 0)
						{
							CameraTreelistAddCameraByStastion(nStationId);//在摄像头列表中添加信息
							SetTreeTuneCycleStationCameraDvrInfo(hTreeItem,nStationId);//在轮巡列表中添加信息
						}
					}
				}
				hChildTreeItem = m_trTuneCycle.GetNextSiblingItem(hChildTreeItem);
			}
		}
		return TRUE;
	}
	catch(...)
	{

	}
	return FALSE;
}

int CDlgPageServer::GetTreeTuneCycleStationIdByCameraNum(char *szCameraNum)
{
	int nResult = -1;
	MYSQL_RES * res = NULL;
	MYSQL_ROW row;
	char sql_buf[1024] = {0};

	try
	{
		if (szCameraNum == NULL||strlen(szCameraNum) == 0)
			return nResult;

		memset(sql_buf,0,sizeof(sql_buf));

		// 读取站点信息
		sprintf_s(sql_buf, "SELECT t1.station_seq FROM video_dvr AS t1,video_camera AS t2 WHERE t1.dvr_id=t2.dvr_id AND t2.camera_id='%s' ",szCameraNum);

		if (!mysql_query(g_mySqlData, sql_buf))
		{
			res = mysql_store_result(g_mySqlData);
			while (row = mysql_fetch_row(res))
			{
				nResult = atoi(row[0]);
			}
			mysql_free_result(res);
		}

		return nResult;
	}
	catch(...)
	{

	}

	return -1;
}

BOOL CDlgPageServer::SetTreeTuneCycleStationCameraDvrInfo(HTREEITEM hTreeItem,int nStationId)
{
	_T_TUNECYCLE_NODE_INFO *pTuneCycleNodeInfo = NULL;
	HTREEITEM hChildTreeItem = NULL;
	int i = 0;

	try
	{
		if (hTreeItem == NULL||nStationId <= 0)
			return FALSE;

		pTuneCycleNodeInfo = (_T_TUNECYCLE_NODE_INFO *)m_trTuneCycle.GetItemData(hTreeItem);
		if (pTuneCycleNodeInfo == NULL||pTuneCycleNodeInfo->node_type != 1)
			return FALSE;

		_T_CAMERA_INFO * pCameraDvrInfo = NULL;
		_T_DVR_INFO * pDvrInfo = NULL;

		char szDvrType[32] = {0};

		g_nCameraDvrInfoCount = 0;
		memset(g_CameraDvrInfo,0,sizeof(g_CameraDvrInfo));

		//表示该节点为摄像头信息,获取数据库中的摄像头DVR信息
		char sql_buf[1024] = {0};

		MYSQL_RES * res = NULL;
		MYSQL_ROW row;

		// 读取摄像头DVR信息
		sprintf_s(sql_buf, "SELECT t1.id,t1.dvr_type,t1.dvr_id,t1.dvr_ip,t1.link_port,t2.channel,t1.adminuser,t1.adminpasswd,t1.station_group,t1.station_name,t1.station_seq,t2.camera_id  "
			" FROM video_dvr AS t1,video_camera AS t2 WHERE t1.station_group='%s' AND t2.dvr_id=t1.dvr_id ORDER BY t1.id ASC", nStationId);

		if (!mysql_query(g_mySqlData, sql_buf))
		{
			res = mysql_store_result(g_mySqlData);
			while (row = mysql_fetch_row(res))
			{
				if (g_nCameraDvrInfoCount >= 512)
				{
					break;
				}

				pCameraDvrInfo = &g_CameraDvrInfo[g_nCameraDvrInfoCount];
				pDvrInfo = &pCameraDvrInfo->dvr_info;

				pDvrInfo->dvr_id = atoi(row[0]);
				strcpy_s(szDvrType,sizeof(szDvrType),row[1]);
				pDvrInfo->dvr_type = AppGetCameraDvrType(szDvrType);
				strcpy_s(pDvrInfo->dvr_num,sizeof(pDvrInfo->dvr_num),row[2]);
				strcpy_s(pDvrInfo->dvr_ip,sizeof(pDvrInfo->dvr_ip),row[3]);
				pDvrInfo->dvr_port = atoi(row[4]);
				pCameraDvrInfo->channel = atoi(row[5]);
				strcpy_s(pDvrInfo->dvr_user,sizeof(pDvrInfo->dvr_user),row[6]);
				strcpy_s(pDvrInfo->dvr_password,sizeof(pDvrInfo->dvr_password),row[7]);
				strcpy_s(pDvrInfo->station_num,sizeof(pDvrInfo->station_num),row[8]);
				strcpy_s(pDvrInfo->station_name,sizeof(pDvrInfo->station_name),row[9]);
				pDvrInfo->station_id = atoi(row[10]);
				strcpy_s(pCameraDvrInfo->camera_num,sizeof(pCameraDvrInfo->camera_num),row[11]);

				g_nCameraDvrInfoCount++;
			}
			mysql_free_result(res);
		}

		//设置摄像头DVR信息
		if (m_trTuneCycle.ItemHasChildren(hTreeItem) != FALSE)
		{
			hChildTreeItem = m_trTuneCycle.GetChildItem(hTreeItem);
			while(hChildTreeItem != NULL)
			{
				pTuneCycleNodeInfo = (_T_TUNECYCLE_NODE_INFO *)m_trTuneCycle.GetItemData(hChildTreeItem);
				if (pTuneCycleNodeInfo != NULL&&pTuneCycleNodeInfo->node_type == 2)
				{
					for (i = 0;i < g_nCameraDvrInfoCount;i++)
					{
						if (strcmp(pTuneCycleNodeInfo->node_num,g_CameraDvrInfo[i].camera_num) == 0)
						{
							memcpy(&pTuneCycleNodeInfo->camera_info,&g_CameraDvrInfo[i],sizeof(g_CameraDvrInfo[i]));
						}
					}
				}
				hChildTreeItem = m_trTuneCycle.GetNextSiblingItem(hChildTreeItem);
			}
		}
		return TRUE;
	}
	catch(...)
	{

	}
	return FALSE;
}

//记录用户操作日志
BOOL CDlgPageServer::WriteUserClientVideoOperateInfo(char *szCameraNum,char *szCameraName,char *szStationName,int nOperateType,int nOperateResult)
{
	try
	{
		if (g_pMainDlg == NULL)
			return FALSE;

		CLIENT_VIDEO_INFO ClientVideoInfo;
		memset(&ClientVideoInfo,0,sizeof(ClientVideoInfo));

		ClientVideoInfo.operate_type = nOperateType;
		ClientVideoInfo.operate_result = nOperateResult;

		strcpy_s(ClientVideoInfo.user_name,sizeof(ClientVideoInfo.user_name),g_user_config_info.szUserName);
		strcpy_s(ClientVideoInfo.camera_name,sizeof(ClientVideoInfo.camera_name),szCameraName);
		strcpy_s(ClientVideoInfo.camera_code,sizeof(ClientVideoInfo.camera_code),szCameraNum);
		strcpy_s(ClientVideoInfo.station_name,sizeof(ClientVideoInfo.station_name),szStationName);

		int nIndex = -1;

		nIndex = g_pMainDlg->GetStationIndexByStationName(ClientVideoInfo.station_name);
		if (nIndex < 0)
		{
			nIndex = g_pMainDlg->GetStationIndexByD5000StationName(ClientVideoInfo.station_name);
		}

		if (nIndex >= 0&&nIndex < MAX_STATION_NUM)
		{
			strcpy_s(ClientVideoInfo.station_code,sizeof(ClientVideoInfo.station_code),g_tStation_Info[nIndex].station_code_videoplant);
			ClientVideoInfo.station_id = g_tStation_Info[nIndex].station_id;

			nIndex = g_pMainDlg->GetStationNodeInfoByNodeId(g_tStation_Info[nIndex].node_id);

			if (nIndex > 0)
			{
				strcpy_s(ClientVideoInfo.node_name,sizeof(ClientVideoInfo.node_name),g_tStationNode_Info[nIndex].node_name);
			}
		}

		if (strlen(ClientVideoInfo.camera_name) != 0&&strlen(ClientVideoInfo.camera_code) > 10
			&&strlen(ClientVideoInfo.node_name) != 0&&strlen(ClientVideoInfo.station_name) != 0)
		{
			g_ClientVideoInfo.WriteClientVideoInfo(&ClientVideoInfo);
		}

		return TRUE;
	}
	catch(...)
	{

	}

	return FALSE;
}

//搜索摄像机树形列表
HTREEITEM CDlgPageServer::SearchTreeServerByInfo(char *szInfo)
{
	HTREEITEM hTreeItem = NULL;
	HTREEITEM hRootItem = NULL;

	if (szInfo == NULL||strlen(szInfo) == 0)
		return NULL;

	hTreeItem = m_trServer.GetSelectedItem();
	if (hTreeItem == NULL)
	{
		hTreeItem = m_trServer.GetRootItem();
		if (hTreeItem == NULL)
			return NULL;
	}

	hTreeItem = SearchTreeCtrlByInfo(&m_trServer,hTreeItem,szInfo);

	if (hTreeItem == NULL)
	{
		hRootItem = m_trServer.GetRootItem();
		if (hRootItem == NULL)
			return NULL;

		m_trServer.SelectItem(hRootItem);
	}

	return hTreeItem;
}

//搜索预置位树形列表
HTREEITEM CDlgPageServer::SearchTreePresetByInfo(char *szInfo)
{
	HTREEITEM hTreeItem = NULL;
	HTREEITEM hRootItem = NULL;

	if (szInfo == NULL||strlen(szInfo) == 0)
		return NULL;

	hTreeItem = m_trPreset.GetSelectedItem();
	if (hTreeItem == NULL)
	{
		hTreeItem = m_trPreset.GetRootItem();
		if (hTreeItem == NULL)
			return NULL;
	}

	hTreeItem = SearchTreeCtrlByInfo(&m_trPreset,hTreeItem,szInfo);

	if (hTreeItem == NULL)
	{
		hRootItem = m_trPreset.GetRootItem();
		if (hRootItem == NULL)
			return NULL;

		m_trPreset.SelectItem(hRootItem);
	}

	return hTreeItem;
}

//搜索自动轮巡树形列表
HTREEITEM CDlgPageServer::SearchTreeTunecycleByInfo(char *szInfo)
{
	HTREEITEM hTreeItem = NULL;
	HTREEITEM hRootItem = NULL;

	if (szInfo == NULL||strlen(szInfo) == 0)
		return NULL;

	hTreeItem = m_trTuneCycle.GetSelectedItem();
	if (hTreeItem == NULL)
	{
		hTreeItem = m_trTuneCycle.GetRootItem();
		if (hTreeItem == NULL)
			return NULL;
	}

	hTreeItem = SearchTreeCtrlByInfo(&m_trTuneCycle,hTreeItem,szInfo);

	if (hTreeItem == NULL)
	{
		hRootItem = m_trTuneCycle.GetRootItem();
		if (hRootItem == NULL)
			return NULL;

		m_trTuneCycle.SelectItem(hRootItem);
	}

	return hTreeItem;
}

//搜索树形列表
HTREEITEM CDlgPageServer::SearchTreeCtrlByInfo(CTreeCtrl *pTreeCtrl,HTREEITEM hTreeItem,char *szInfo)
{
	try
	{
		HTREEITEM hResultItem = NULL;
		HTREEITEM hSiblingItem = NULL;
		HTREEITEM hParentItem = NULL;
		HTREEITEM hParentSiblingItem = NULL;
		CString strInfo;

		if (pTreeCtrl == NULL||hTreeItem == NULL||szInfo == NULL||strlen(szInfo) == 0)
			return NULL;

		hResultItem = SearchTreeCtrlChildItemByInfo(pTreeCtrl,hTreeItem,szInfo);
		if (hResultItem != NULL)
			return hResultItem;

		hResultItem = SearchTreeCtrlSiblingItemByInfo(pTreeCtrl,hTreeItem,szInfo);
		if (hResultItem != NULL)
			return hResultItem;

		hParentItem = pTreeCtrl->GetParentItem(hTreeItem);
		while(hParentItem != NULL)
		{
			hResultItem = SearchTreeCtrlSiblingItemByInfo(pTreeCtrl,hParentItem,szInfo);
			if (hResultItem != NULL)
				return hResultItem;

			hParentItem = pTreeCtrl->GetParentItem(hParentItem);
		}

		return NULL;
	}
	catch (...)
	{

	}

	return NULL;
}

//搜索树形列表下面结点
HTREEITEM CDlgPageServer::SearchTreeCtrlChildItemByInfo(CTreeCtrl *pTreeCtrl,HTREEITEM hTreeItem,char *szInfo)
{
	try
	{
		HTREEITEM hResultItem = NULL;
		HTREEITEM hChildItem = NULL;
		CString strInfo;

		if (pTreeCtrl == NULL||hTreeItem == NULL||szInfo == NULL||strlen(szInfo) == 0)
			return NULL;

		hChildItem = pTreeCtrl->GetChildItem(hTreeItem);
		while(hChildItem != NULL)
		{
			strInfo = pTreeCtrl->GetItemText(hChildItem);
			if (strstr(strInfo,szInfo) != NULL )
				return hChildItem;

			hResultItem = SearchTreeCtrlChildItemByInfo(pTreeCtrl,hChildItem,szInfo);
			if (hResultItem != NULL)
				return hResultItem;

			hChildItem = pTreeCtrl->GetNextSiblingItem(hChildItem);
		}

		return NULL;
	}
	catch (...)
	{

	}

	return NULL;
}

//搜索树形列表下面结点
HTREEITEM CDlgPageServer::SearchTreeCtrlSiblingItemByInfo(CTreeCtrl *pTreeCtrl,HTREEITEM hTreeItem,char *szInfo)
{
	try
	{
		HTREEITEM hResultItem = NULL;
		HTREEITEM hSiblingItem = NULL;
		CString strInfo;

		if (pTreeCtrl == NULL||hTreeItem == NULL||szInfo == NULL||strlen(szInfo) == 0)
			return NULL;

		hSiblingItem = pTreeCtrl->GetNextSiblingItem(hTreeItem);

		while(hSiblingItem != NULL)
		{
			strInfo = pTreeCtrl->GetItemText(hSiblingItem);
			if (strstr(strInfo,szInfo) != NULL )
				return hSiblingItem;

			hResultItem = SearchTreeCtrlChildItemByInfo(pTreeCtrl,hSiblingItem,szInfo);
			if (hResultItem != NULL)
				return hResultItem;

			hSiblingItem = pTreeCtrl->GetNextSiblingItem(hSiblingItem);
		}

		return NULL;
	}
	catch (...)
	{

	}

	return NULL;
}

void CDlgPageServer::OnBnClickedButtonSearch()
{
	char szInfo[256] = {0};
	memset(szInfo,0,sizeof(szInfo));
	HTREEITEM hTreeItem = NULL;

	try
	{
		UpdateData(TRUE);

		CWnd *pWnd = NULL;

		//摄像机列表
		pWnd = GetDlgItem(IDC_TREE_SERVER);
		if (pWnd != NULL)
		{
			if(pWnd->IsWindowVisible() != FALSE)
			{
				strcpy_s(szInfo,sizeof(szInfo)-1,m_strSearch);
				if (strlen(szInfo) != 0)
				{
					hTreeItem = SearchTreeServerByInfo(szInfo);
					if (hTreeItem != NULL)
					{
						m_trServer.SelectItem(hTreeItem);
						m_trServer.EnsureVisible(hTreeItem);
					}
				}
			}
		}

		//预置位列表
		pWnd = GetDlgItem(IDC_TREE_PRESET);
		if (pWnd != NULL)
		{
			if(pWnd->IsWindowVisible() != FALSE)
			{
				strcpy_s(szInfo,sizeof(szInfo)-1,m_strSearch);
				if (strlen(szInfo) != 0)
				{
					hTreeItem = SearchTreePresetByInfo(szInfo);
					if (hTreeItem != NULL)
					{
						m_trPreset.SelectItem(hTreeItem);
						m_trPreset.EnsureVisible(hTreeItem);
					}
				}
			}
		}

		//自动轮巡列表
		pWnd = GetDlgItem(IDC_TREE_TUNECYCLE);
		if (pWnd != NULL)
		{
			if(pWnd->IsWindowVisible() != FALSE)
			{
				strcpy_s(szInfo,sizeof(szInfo)-1,m_strSearch);
				if (strlen(szInfo) != 0)
				{
					hTreeItem = SearchTreeTunecycleByInfo(szInfo);
					if (hTreeItem != NULL)
					{
						m_trTuneCycle.SelectItem(hTreeItem);
						m_trTuneCycle.EnsureVisible(hTreeItem);
					}
				}
			}
		}

	}
	catch(...)
	{

	}
}

void CDlgPageServer::OnTimer(UINT_PTR nIDEvent)
{
	switch (nIDEvent)
	{
	case 1:
		{
			KillTimer(nIDEvent);

			// 树状列表初始化
			ReadDBSystemInfo();

			// 初始化树形列表
			InitTreeList();
		}
		break;
	}

	CPropertyPage::OnTimer(nIDEvent);
}

void CDlgPageServer::OnMenuitemViewCameraStatus()
{
	try
	{
		CWnd *pServerTreeWnd = GetDlgItem(IDC_TREE_SERVER);
		CWnd *pPresetTreeWnd = GetDlgItem(IDC_TREE_PRESET);

		HTREEITEM hItem = NULL;
		_T_NODE_INFO *pNodeInfo = NULL;
		_T_NODE_PRESET_INFO *pNodePresetInfo = NULL;

		char szCameraNum[64] = {0};
		char szCameraName[260] = {0};
		char szStationName[260] = {0};

		int nStatus = -1;

		if(pServerTreeWnd != NULL&&pServerTreeWnd->IsWindowVisible())
		{
			// 查找点中的树结点项目
			hItem = m_trServer.GetSelectedItem();
			if (hItem == NULL)
				return;

			pNodeInfo = (_T_NODE_INFO *)m_trServer.GetItemData(hItem);
			if (pNodeInfo == NULL||pNodeInfo->node_type != 3)
				return;

			CDlgCameraStatus DlgCameraStatus(pNodeInfo->node_num);//摄像机状态值窗口

			if(DlgCameraStatus.DoModal() == IDOK)
			{

			}
		}
		else if (pPresetTreeWnd != NULL&&pPresetTreeWnd->IsWindowVisible())
		{
			// 查找点中的树结点项目
			hItem = m_trPreset.GetSelectedItem();
			if (hItem == NULL)
				return;

			pNodePresetInfo = (_T_NODE_PRESET_INFO *)m_trPreset.GetItemData(hItem);
			if (pNodePresetInfo == NULL||(pNodePresetInfo->node_type != 3&&pNodePresetInfo->node_type != 5))
				return;

			CDlgCameraStatus DlgCameraStatus(pNodeInfo->node_num);//摄像机状态值窗口

			if(DlgCameraStatus.DoModal() == IDOK)
			{

			}
		}
		else
			return;
	}
	catch(...)
	{

	}
}

int CDlgPageServer::UpdateCameraAnFangFlag(char* szCameraCode, int nFlag)
{
	char sql_buf[1024] = {0};
	sprintf(sql_buf,"UPDATE video_camera set anfang_flag=%d where camera_id='%s'",nFlag,szCameraCode);
	if (mysql_query(g_mySqlData, sql_buf))
	{	
		return FALSE;
	}
	return TRUE;
	
}

void CDlgPageServer::OnMenuitemSetAnfangCamera()
{
	// TODO: 在此添加命令处理程序代码
	try
	{
		CWnd *pServerTreeWnd = GetDlgItem(IDC_TREE_SERVER);
		CWnd *pPresetTreeWnd = GetDlgItem(IDC_TREE_PRESET);

		HTREEITEM hItem = NULL;
		_T_NODE_INFO *pNodeInfo = NULL;
		_T_NODE_PRESET_INFO *pNodePresetInfo = NULL;

		char szCameraNum[64] = {0};
		char szCameraName[260] = {0};
		char szStationName[260] = {0};

		int nStatus = -1;

		if(pServerTreeWnd != NULL&&pServerTreeWnd->IsWindowVisible())
		{
			// 查找点中的树结点项目
			hItem = m_trServer.GetSelectedItem();
			if (hItem == NULL)
				return;

			pNodeInfo = (_T_NODE_INFO *)m_trServer.GetItemData(hItem);
			if (pNodeInfo == NULL||pNodeInfo->node_type != 3)
				return;

			//更新摄像头安防标识
			if (UpdateCameraAnFangFlag(pNodeInfo->node_num,1))
			{
				//设置项目文本
				char szItemText[80] = {0};
				sprintf_s(szItemText,80,"[安防]%s",pNodeInfo->node_name);		
				m_trServer.SetItemText(hItem,szItemText);
				//m_trServer.SetItemImage(hItem,12,12);
			}

		}
		else if (pPresetTreeWnd != NULL&&pPresetTreeWnd->IsWindowVisible())
		{
			// 查找点中的树结点项目
			hItem = m_trPreset.GetSelectedItem();
			if (hItem == NULL)
				return;

			pNodePresetInfo = (_T_NODE_PRESET_INFO *)m_trPreset.GetItemData(hItem);
			if (pNodePresetInfo == NULL||(pNodePresetInfo->node_type != 3&&pNodePresetInfo->node_type != 5))
				return;
			
			//更新摄像头安防标识
			if (UpdateCameraAnFangFlag(pNodePresetInfo->node_num,1))
			{
				//设置项目文本
				char szItemText[80] = {0};
				sprintf_s(szItemText,80,"[安防]%s",pNodePresetInfo->node_name);		
				m_trServer.SetItemText(hItem,szItemText);
				//设置项目图标
				//m_trServer.SetItemImage(hItem,12,12);
			}
		}
		else
			return;
	}
	catch(...)
	{

	}

}

void CDlgPageServer::OnMenuitemCancelAnfangCamera()
{
	// TODO: 在此添加命令处理程序代码
	try
	{
		CWnd *pServerTreeWnd = GetDlgItem(IDC_TREE_SERVER);
		CWnd *pPresetTreeWnd = GetDlgItem(IDC_TREE_PRESET);

		HTREEITEM hItem = NULL;
		_T_NODE_INFO *pNodeInfo = NULL;
		_T_NODE_PRESET_INFO *pNodePresetInfo = NULL;

		char szCameraNum[64] = {0};
		char szCameraName[260] = {0};
		char szStationName[260] = {0};

		int nStatus = -1;

		if(pServerTreeWnd != NULL&&pServerTreeWnd->IsWindowVisible())
		{
			// 查找点中的树结点项目
			hItem = m_trServer.GetSelectedItem();
			if (hItem == NULL)
				return;

			pNodeInfo = (_T_NODE_INFO *)m_trServer.GetItemData(hItem);
			if (pNodeInfo == NULL||pNodeInfo->node_type != 3)
				return;

			//更新摄像头安防标识
			if(UpdateCameraAnFangFlag(pNodeInfo->node_num,0))
			{
				//设置项目文本，非安防
				char szItemText[80] = {0};
				sprintf_s(szItemText,80,"%s",pNodeInfo->node_name);		
				m_trServer.SetItemText(hItem,szItemText);
				//设置节点图标
				//m_trServer.SetItemImage(hItem,5,5);
			}

		}
		else if (pPresetTreeWnd != NULL&&pPresetTreeWnd->IsWindowVisible())
		{
			// 查找点中的树结点项目
			hItem = m_trPreset.GetSelectedItem();
			if (hItem == NULL)
				return;

			pNodePresetInfo = (_T_NODE_PRESET_INFO *)m_trPreset.GetItemData(hItem);
			if (pNodePresetInfo == NULL||(pNodePresetInfo->node_type != 3&&pNodePresetInfo->node_type != 5))
				return;

			//更新摄像头安防标识
			if(UpdateCameraAnFangFlag(pNodePresetInfo->node_num,0))
			{
				//设置项目文本，非安防
				char szItemText[80] = {0};
				sprintf_s(szItemText,80,"%s",pNodePresetInfo->node_name);		
				m_trServer.SetItemText(hItem,szItemText);
				//设置节点图标
				//m_trServer.SetItemImage(hItem,5,5);
			}
		}
		else
			return;
	}
	catch(...)
	{

	}
}

void CDlgPageServer::OnMenuitemOnlyShowAnfangCamera()
{
	// TODO: 在此添加命令处理程序代码
	DeleteCameraTreeList();
	InitCameraTreelist();
	m_bOnlyShowAnFang = TRUE;
}

void CDlgPageServer::OnMenuitemShowAllCamera()
{
	// TODO: 在此添加命令处理程序代码
	DeleteCameraTreeList();
	InitCameraTreelist();
	m_bOnlyShowAnFang = FALSE;
}
