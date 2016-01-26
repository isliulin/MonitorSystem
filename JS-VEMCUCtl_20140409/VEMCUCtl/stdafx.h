
// stdafx.h : ��׼ϵͳ�����ļ��İ����ļ���
// ���Ǿ���ʹ�õ��������ĵ�
// �ض�����Ŀ�İ����ļ�

#pragma once

#ifndef _SECURE_ATL
#define _SECURE_ATL 1
#endif

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN            // �� Windows ͷ���ų�����ʹ�õ�����
#endif

#include "targetver.h"

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // ĳЩ CString ���캯��������ʽ��

// �ر� MFC ��ĳЩ�����������ɷ��ĺ��Եľ�����Ϣ������
#define _AFX_ALL_WARNINGS

#include <afxwin.h>         // MFC ��������ͱ�׼���
#include <afxext.h>         // MFC ��չ

#include <afxdisp.h>        // MFC �Զ�����

#pragma comment(linker, "\"/manifestdependency:type='Win32' name='Microsoft.VC90.CRT' version='9.0.21022.8' processorArchitecture='X86' publicKeyToken='1fc8b3b9a1e18e3b' language='*'\"") 

#define TOTALCYCLEMETHODNUM	16
#define TOTALCYCLENAMENUM	100

#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h>           // MFC �� Internet Explorer 4 �����ؼ���֧��
#endif
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>             // MFC �� Windows �����ؼ���֧��
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <afxcontrolbars.h>     // �������Ϳؼ����� MFC ֧��

//���ξ���
#pragma warning(disable:4005 4018 4244 4627 4800 4995 4996)

#include <afxdb.h> 
#include <odbcinst.h>

#include "VMDefine.h"

//ϵͳ��
#pragma comment(lib,"Wsock32.lib")
#pragma comment(lib,"ws2_32.lib")
#pragma comment(lib,"winmm.lib")
#pragma comment(lib,"msimg32.lib")
#pragma comment(lib,"version.lib")

//rtsip��
#include "SipUtil.h"
#pragma comment(lib,"lib\\rtsip.lib")

//rtsp��
#include ".\\include\\rtspClientAPI.h"
#pragma comment(lib,"lib\\RtspLib.lib")

//CommonLib
#include "include\\CommonLib.h"
#pragma comment(lib,"lib\\CommonLib.lib")

// �Զ������
#include "xappany.h"

// ���ݿ�����
#pragma comment(lib,"lib\\libmySQL.lib")

// http ���ֽӿ�
#include ".\\include\\HttpClient.h"

#ifdef DEBUG
#pragma comment(lib,"lib\\HttpClient_d.lib")
#else
#pragma comment(lib,"lib\\HttpClient.lib")
#endif

//����ӿ�
#include ".\\include\\SearchWeatherInfoApi.h"
#pragma comment(lib,"lib\\SearchWeatherInfo.lib")

// ����
#include "include\\SystemLinkageSettingAPI.h"
#ifdef DEBUG
#pragma comment(lib,"lib\\SystemLinkageSetting_d.lib")
#else
#pragma comment(lib,"lib\\SystemLinkageSetting.lib")
#endif

// ϵͳ����ͼ
#include ".\\include\\SysNetTuopuAPI.h"
#pragma comment(lib, "lib\\SysNetTuopu.lib")

// xml����
#include ".\\tinyxml\\xmlApi.h"

// �ļ���־
#include "VMLog.h"

//VideoError��Ϣ
#include "VideoErrorInfo.h"

//ClientVideo��Ϣ
#include "ClientVideoInfo.h"

//CameraStatus��Ϣ
#include "CameraStatusInfo.h"

#include "include\\DevPlayVideoSdk.h"
#pragma comment(lib, "lib\\DevPlayVideoSdk.lib")

//����ϵͳ
#include "include\\ACSSystemAPI.h"
#ifdef DEBUG
#pragma comment(lib,"lib\\ACSSystemD.lib")
#else
#pragma comment(lib,"lib\\ACSSystem.lib")
#endif

//����ϵͳȫ�ֱ����ͽṹ�嶨��
#include "AssSystemDefine.h"
#include "AssDBOperate.h"
#include "AssCommonFunction.h"

//�����������ù���
#include "include\\LinkageSettingAPI.h"
#ifdef DEBUG
#pragma comment(lib,"lib\\LinkageSettingD.lib")
#else
#pragma comment(lib,"lib\\LinkageSetting.lib")
#endif

#include "AssCameraOperate.h"

//���ӵ�ͼ add by wx 20151102
#ifdef  _DEBUG
#pragma comment(lib,"lib\\SDDElecMapD.lib")
#else
#pragma comment(lib,"lib\\SDDElecMap.lib")
#endif
#include "SDDElecMap\DLLInterface.h"

#include "include\\deelx.h"
#include "AssGlobal.h"

#include "include\\MyTreeCtrl.h"
#include "include\\MultiTreeCtrl.h"


#if defined _M_IX86
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_IA64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='ia64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif