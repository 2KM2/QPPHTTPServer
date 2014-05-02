// WebServer.cpp : ����Ӧ�ó��������Ϊ��
//

#include "stdafx.h"

#include "MainFrm.h"
#include "WebServer.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CWebServerApp

BEGIN_MESSAGE_MAP(CWebServerApp, CWinApp)
	ON_COMMAND(ID_APP_ABOUT, &CWebServerApp::OnAppAbout)
END_MESSAGE_MAP()


// CWebServerApp ����

CWebServerApp::CWebServerApp()
{
	// TODO: �ڴ˴����ӹ�����룬
	// ��������Ҫ�ĳ�ʼ�������� InitInstance ��
}


// Ψһ��һ�� CWebServerApp ����

CWebServerApp theApp;

// CWebServerApp ��ʼ��

BOOL CWebServerApp::InitInstance()
{
	// ���һ�������� Windows XP �ϵ�Ӧ�ó����嵥ָ��Ҫ
	// ʹ�� ComCtl32.dll �汾 6 ����߰汾�����ÿ��ӻ���ʽ��
	//����Ҫ InitCommonControlsEx()�����򣬽��޷��������ڡ�
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// ��������Ϊ��������Ҫ��Ӧ�ó�����ʹ�õ�
	// �����ؼ��ࡣ
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();

	if(!IOCPNetwork::initWinsockLib(2, 2))
	{
		AfxMessageBox(IDP_SOCKETS_INIT_FAILED);
	}

	// ��Ҫ���������ڣ��˴��뽫�����µĿ�ܴ���
	// ����Ȼ��������ΪӦ�ó���������ڶ���
	CMainFrame* pFrame = new CMainFrame;
	if (!pFrame) return FALSE;
	m_pMainWnd = pFrame;

	// ���������ؿ�ܼ�����Դ
	pFrame->LoadFrame(IDR_MAINFRAME, WS_OVERLAPPEDWINDOW | FWS_ADDTOTITLE, NULL, NULL);

	if ( _tcsicmp(this->m_lpCmdLine, _T("hide")) == 0)
	{
		pFrame->ShowWindow(SW_HIDE);
		
	}
	else
	{
		pFrame->ShowWindow(this->m_nCmdShow);
	}
	
	pFrame->UpdateWindow();

	return TRUE;

}

int CWebServerApp::ExitInstance()
{
	// TODO: �ڴ�����ר�ô����/����û���
	IOCPNetwork::cleanWinsockLib();
	return CWinApp::ExitInstance();
}

// CWebServerApp ��Ϣ��������




// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()

// �������жԻ����Ӧ�ó�������
void CWebServerApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}


// CWebServerApp ��Ϣ��������


#define MY_REGKEY _T("httpserver")
BOOL AutoLaunch(BOOL bRun /* = TRUE */)
{	
	// ��ȡ·��
	TCHAR szFileName[MAX_PATH + 10] = {0};
	szFileName[0] = _T('\"');
	if (0 == GetModuleFileName(NULL, szFileName + 1, MAX_PATH) ) return FALSE;
	_tcscat(szFileName, _T("\" hide")); // ��С������

	BOOL bRet = FALSE;
	HKEY hKey;
	LPCTSTR szKeyPath = _T("Software\\Microsoft\\Windows\\CurrentVersion");		
	long ret = RegOpenKeyEx(HKEY_CURRENT_USER, szKeyPath, 0, KEY_WRITE, &hKey);
	if(ret != ERROR_SUCCESS)
	{
		TRACE("�޷���ȡע���.");
	}
	else
	{
		HKEY hRunKey;
		ret = RegCreateKeyEx(hKey, _T("Run"), 0, NULL, 0, KEY_WRITE, NULL, &hRunKey, NULL);
		if(ERROR_SUCCESS == ret)
		{
			if(bRun)
			{
				bRet = (ERROR_SUCCESS == ::RegSetValueEx(hRunKey, MY_REGKEY, 0, REG_SZ, (BYTE*)szFileName, (_tcslen(szFileName) + 1) * sizeof(TCHAR)));
			}
			else
			{
				ret = RegDeleteValue(hRunKey, MY_REGKEY);
				bRet = (ret == ERROR_SUCCESS);
			}

			RegCloseKey(hRunKey);
		}
		else
		{
			TRACE("�޷�дע���.");
		}
		RegCloseKey(hKey);
	}
	return bRet;
}


BOOL IsAutoLaunch()
{
	BOOL bRet = FALSE;
	HKEY hKey;
	TCHAR szKeyPath[] = _T("Software\\Microsoft\\Windows\\CurrentVersion\\Run");		
	long ret = RegOpenKeyEx(HKEY_CURRENT_USER, szKeyPath, 0, KEY_READ, &hKey);
	if(ret != ERROR_SUCCESS)
	{
		TRACE("�޷���ȡע���.");
	}
	else
	{
		TCHAR szValue[MAX_PATH + 1] = {0};
		DWORD dwType = REG_SZ;
		DWORD dwLen = MAX_PATH * sizeof(TCHAR);

		LONG lRet = ::RegQueryValueEx(hKey, MY_REGKEY, NULL, &dwType, (LPBYTE)szValue, &dwLen);
		if(ERROR_SUCCESS != lRet)
		{
		}
		else
		{
			TCHAR szFileName[MAX_PATH + 10] = {0};
			if (0 == GetModuleFileName(NULL, szFileName + 1, MAX_PATH) )
			{
				TRACE("�޷���ѯ��ȡ��ǰ���̵��ļ���.");
			}
			else
			{
				szFileName[0] = _T('\"');
				_tcscat(szFileName, _T("\" hide"));
				return _tcsicmp(szFileName, szValue) == 0;
			}

		}
		RegCloseKey(hKey);
	}

	return bRet;
}