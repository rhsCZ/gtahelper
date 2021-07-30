#pragma once
#include "afxdialogex.h"
#include "afxwin.h"
#include <thread>
#include <conio.h>
#include <tlhelp32.h>
#include <WinUser.h>
using namespace std;
#pragma warning(disable:6262)
#define bufferSize 10
#define WM_SHOWPAGE WM_APP+2
#define WM_TRAY_ICON_NOTIFY_MESSAGE (WM_USER + 1)
void ElevateNow();
BOOL IsRunAsAdministrator();
class gtahelperDlg : public CDialog
{
private:
	BOOL m_bMinimizeToTray;
	BOOL			m_bTrayIconVisible;
	CMenu			m_mnuTrayMenu;
	UINT			m_nDefaultMenuItem;
	INPUT input1;
	INPUT input2;
	INPUT input3[2];
	INPUT input4[3];
	INPUT input5;
	bool t3 = false;
	bool t1en = true;
	bool t2en = true;
	void detectkey();
	thread t1;
	void loopmouse();
	thread t2;
	afx_msg LRESULT OnTrayNotify(WPARAM wParam, LPARAM lParam);
	CFont Font = {};
	bool trayenable = false;
	bool minimizeen = false;
	CButton* reselev = nullptr;
	CButton* checkbox = nullptr;
	CButton* trayen = nullptr;
	// Construction
public:
	NOTIFYICONDATA	m_nidIconData;
	gtahelperDlg(CWnd* pParent = nullptr);
	~gtahelperDlg();
	void TraySetMinimizeToTray(BOOL bMinimizeToTray = TRUE);
	BOOL TraySetMenu(UINT nResourceID, UINT nDefaultPos = 0);
	BOOL TraySetMenu(HMENU hMenu, UINT nDefaultPos = 0);
	BOOL TraySetMenu(LPCTSTR lpszMenuName, UINT nDefaultPos = 0);
	BOOL TrayUpdate();
	BOOL TrayShow();
	BOOL TrayHide();
	void TraySetToolTip(LPCTSTR lpszToolTip);
	void TraySetIcon(HICON hIcon);
	void TraySetIcon(UINT nResourceID);
	void TraySetIcon(LPCTSTR lpszResourceName);
	BOOL TrayIsVisible();
	virtual void OnTrayLButtonDown(CPoint pt);
	virtual void OnTrayLButtonDblClk(CPoint pt);
	virtual void OnTrayRButtonDown(CPoint pt);
	virtual void OnTrayRButtonDblClk(CPoint pt);
	virtual void OnTrayMouseMove(CPoint pt);
	afx_msg void OnBnClickedExit();
	afx_msg void OnOpen();
	afx_msg void OnMinimize();
	afx_msg void OnBnClickedMinEn();
	afx_msg void OnBnClickedTrayEn();
	afx_msg void OnBnClickedMovew();
	afx_msg void OnBnClickedMoveafk();
	afx_msg void OnBnClickedMovew8();
	afx_msg void OnBnClickedStop();
	afx_msg void OnBnClickedPubsolo();
	afx_msg void OnBnClickedReselev();
	DWORD GetProcessPIDByName(const char* name);
	int checkelevation(DWORD pid);
	void createsolo();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_GTAHELPER_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	
	DECLARE_MESSAGE_MAP()

};
static BOOL IsRunAsAdministrator()
{
	BOOL fIsRunAsAdmin = FALSE;
	DWORD dwError = ERROR_SUCCESS;
	PSID pAdministratorsGroup = NULL;

	SID_IDENTIFIER_AUTHORITY NtAuthority = SECURITY_NT_AUTHORITY;
	if (!AllocateAndInitializeSid(
		&NtAuthority,
		2,
		SECURITY_BUILTIN_DOMAIN_RID,
		DOMAIN_ALIAS_RID_ADMINS,
		0, 0, 0, 0, 0, 0,
		&pAdministratorsGroup))
	{
		dwError = GetLastError();
		goto Cleanup;
	}

	if (!CheckTokenMembership(NULL, pAdministratorsGroup, &fIsRunAsAdmin))
	{
		dwError = GetLastError();
		goto Cleanup;
	}

Cleanup:

	if (pAdministratorsGroup)
	{
		FreeSid(pAdministratorsGroup);
		pAdministratorsGroup = NULL;
	}

	if (ERROR_SUCCESS != dwError)
	{
		throw dwError;
	}

	return fIsRunAsAdmin;
}
static void ElevateNow()
{
	if (!IsRunAsAdministrator())
	{
		char szPath[MAX_PATH];
		if (GetModuleFileName(NULL, szPath, ARRAYSIZE(szPath)))
		{
			SHELLEXECUTEINFO sei = { sizeof(sei) };
			sei.lpVerb = "runas";
			sei.lpFile = szPath;
			sei.hwnd = NULL;
			sei.nShow = SW_NORMAL;
			if (!ShellExecuteEx(&sei))
			{
				DWORD dwError = GetLastError();
				if (dwError == ERROR_CANCELLED)
					CreateThread(0, 0, (LPTHREAD_START_ROUTINE)ElevateNow, 0, 0, 0);
			}
		}
	}
}