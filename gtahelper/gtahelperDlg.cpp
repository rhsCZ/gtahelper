#include "pch.h"
#include "framework.h"
#include "gtahelper.h"
#include "gtahelperDlg.h"
#include "afxdialogex.h"
#include "afxwin.h"
#include <Windows.h>
template<class TYPE>
bool RegSetKey(HKEY key, LPSTR keyloc, unsigned long type, REGSAM access, LPSTR name, TYPE indatax);
int RegCrtKey(HKEY key, LPSTR keyloc, REGSAM access);
template<class TYPE>
int RegGetKey(HKEY key, LPSTR keyloc, unsigned long type, REGSAM access, LPSTR name, TYPE outdatax);
#pragma warning(disable:4244)
#pragma warning(disable:6387)
#pragma warning(disable:6011)
#ifdef _DEBUG
#define new DEBUG_NEW
#endif
using namespace std;
int gtahelperDlg::checkelevation(DWORD pid)
{
	DWORD error = 0;
	bool fRet = FALSE;
	HANDLE hToken = NULL;
	HANDLE proc = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
	if (proc == NULL)
	{
		error = GetLastError();
		printf_s("ERROR:OpenProcess:%ul\n", error);
		system("pause");
		return -1;
	}
	if (OpenProcessToken(proc, TOKEN_QUERY, &hToken) != 0) {
		TOKEN_ELEVATION Elevation;
		DWORD cbSize = sizeof(TOKEN_ELEVATION);
		if (GetTokenInformation(hToken, TokenElevation, &Elevation, sizeof(Elevation), &cbSize) != 0)
		{
			fRet = Elevation.TokenIsElevated;
		}
		else
		{
			error = GetLastError();
			printf_s("ERROR:GetTokenInformation:%ul\n", error);
			system("pause");
			CloseHandle(hToken);
			CloseHandle(proc);
			return -1;
		}
	}
	else
	{
		error = GetLastError();
		printf_s("ERROR:OpenProcessToken:%ul\n", error);
		system("pause");
		CloseHandle(proc);
		return -1;
	}
	CloseHandle(hToken);
	CloseHandle(proc);
	return fRet;
}

DWORD gtahelperDlg::GetProcessPIDByName(const char* name)
{
	PROCESSENTRY32 entry;
	entry.dwSize = sizeof(PROCESSENTRY32);

	HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);

	if (Process32First(snapshot, &entry) == TRUE)
	{
		while (Process32Next(snapshot, &entry) == TRUE)
		{

			if (_stricmp(entry.szExeFile, name) == 0)
			{
				CloseHandle(snapshot);
				return entry.th32ProcessID;

			}
		}
		CloseHandle(snapshot);
		return NULL;
	}
	else
	{
		CloseHandle(snapshot);
		return NULL;
	}
}
void gtahelperDlg::createsolo()
{
	DebugSetProcessKillOnExit(0);
	if (!t3)
	{
		DWORD error;
		t3 = true;
		DWORD pid = GetProcessPIDByName("GTA5.exe");
		if (pid == 0)
		{
			::MessageBoxA(NULL, "ERROR: Unable to find GTA5.exe!!", "!!ERROR!!", MB_OK | MB_ICONERROR | MB_TOPMOST);
			goto endfunc;
		}
		int val = checkelevation(pid);
		if (val == 1)
		{
			::MessageBoxA(NULL, "ERROR: Process GTA5.exe runs with admin permission. Please restart app with admin permission!!", "!!ERROR!!", MB_OK | MB_ICONERROR | MB_TOPMOST);
			goto endfunc;
		}
		else if (val == -1)
		{
			::MessageBoxA(NULL, "ERROR: Uknown error!!", "!!ERROR!!", MB_OK | MB_ICONERROR | MB_TOPMOST);
			goto endfunc;
		}
		BOOL ret = DebugActiveProcess(pid);
		if(!ret)
		{
			 error = GetLastError();
		
		if (error != 0)
		{
			::MessageBoxA(NULL, "ERROR: Unable to pause process GTA.exe!!", "!!ERROR!!", MB_OK | MB_ICONERROR | MB_TOPMOST);
			goto endfunc;
		}
		}
		/*error = GetLastError();
		if (error != 0)
		{
			::MessageBoxA(NULL, "ERROR: Unable to set DebugSetProcessKillOnExit(0); !!", "!!ERROR!!", MB_OK | MB_ICONERROR | MB_TOPMOST);
			goto endfunc;
		}*/
		Sleep(10000);
		DebugActiveProcessStop(pid);
		if (!ret)
		{
			error = GetLastError();

			error = GetLastError();
			if (error != 0)
			{
				::MessageBoxA(NULL, "ERROR: Unable to Continue process GTA.exe!!", "!!ERROR!!", MB_OK | MB_ICONERROR | MB_TOPMOST);
				goto endfunc;
			}
		}

		::MessageBoxA(NULL, "Successfully created public solo session", "INFO", MB_OK | MB_ICONINFORMATION | MB_TOPMOST);
	endfunc:
		t3 = false;
	}
}
void gtahelperDlg::loopmouse()
{
	while (t2en)
	{
		input5.mi.dx = -50;
		if (input5.mi.dx == LONG_MIN)
		{
			input5.mi.dx = LONG_MAX;
		}
		SendInput(1, &input5, sizeof(INPUT));
#ifdef DEBUG
		//_cprintf("X = %ld\n", input5.mi.dx);
#endif 
		Sleep(10);
	}
}
void gtahelperDlg::detectkey()
{
	while (t1en)
	{
		if ((GetKeyState(0x11) & 0x8000) && (GetKeyState(0x31) & 0x8000))
		{
			OnBnClickedMovew();
#ifdef DEBUG
			//_cprintf("f4:pressed!\n");	
#endif
		}
		if ((GetKeyState(0x11) & 0x8000) && (GetKeyState(0x32) & 0x8000))
		{
			OnBnClickedMoveafk();
#ifdef DEBUG
			//_cprintf("f5:pressed!\n");
#endif
		}
		if ((GetKeyState(0x11) & 0x8000) && (GetKeyState(0x33) & 0x8000))
		{
			OnBnClickedMovew8();
#ifdef DEBUG
			//_cprintf("f6:pressed!\n");
#endif
		}
		if ((GetKeyState(0x11) & 0x8000) && (GetKeyState(0xC0) & 0x8000))
		{
			OnBnClickedStop();
#ifdef DEBUG
			//_cprintf("f7:pressed!\n");
#endif
		}
		if ((GetKeyState(0x11) & 0x8000) && (GetKeyState(0x34) & 0x8000))
		{
			
			OnBnClickedPubsolo();
#ifdef DEBUG
			//_cprintf("f7:pressed!\n");
#endif
		}
		
			Sleep(20);
	}
}
gtahelperDlg::gtahelperDlg(CWnd* pParent /*=nullptr*/)
	: CDialog(IDD_GTAHELPER_DIALOG, pParent)
{
	m_nidIconData.cbSize = sizeof(NOTIFYICONDATA);
	m_nidIconData.hWnd = 0;
	m_nidIconData.uID = 1;
	m_nidIconData.uCallbackMessage = WM_TRAY_ICON_NOTIFY_MESSAGE;
	m_nidIconData.hIcon = 0;
	m_nidIconData.szTip[0] = 0;
	m_nidIconData.uFlags = NIF_MESSAGE;
	m_bTrayIconVisible = FALSE;
	m_nDefaultMenuItem = 0;
	m_bMinimizeToTray = TRUE;
	m_hIcon = AfxGetApp()->LoadIcon(IDR_ICON);
	input1.type = INPUT_KEYBOARD;
	input1.ki.wVk = 0;
	input1.ki.time = 0;
	input1.ki.wScan = 0x11;
	input1.ki.dwFlags = KEYEVENTF_SCANCODE;
	input2.type = INPUT_KEYBOARD;
	input2.ki.wVk = 0;
	input2.ki.time = 0;
	input2.ki.wScan = 0x1E;
	input2.ki.dwFlags = KEYEVENTF_SCANCODE;
	input3[0].type = INPUT_KEYBOARD;
	input3[0].ki.wVk = 0;
	input3[0].ki.time = 0;
	input3[0].ki.wScan = 0x11;
	input3[0].ki.dwFlags = KEYEVENTF_SCANCODE;
	input3[1].type = INPUT_KEYBOARD;
	input3[1].ki.wVk = 0;
	input3[1].ki.time = 0;
	input3[1].ki.wScan = 0x48;
	input3[1].ki.dwFlags = KEYEVENTF_SCANCODE;
	input4[0].type = INPUT_KEYBOARD;
	input4[0].ki.wVk = 0;
	input4[0].ki.time = 0;
	input4[0].ki.wScan = 0x11;
	input4[0].ki.dwFlags = KEYEVENTF_SCANCODE | KEYEVENTF_KEYUP;
	input4[1].type = INPUT_KEYBOARD;
	input4[1].ki.wVk = 0;
	input4[1].ki.time = 0;
	input4[1].ki.wScan = 0x48;
	input4[1].ki.dwFlags = KEYEVENTF_SCANCODE | KEYEVENTF_KEYUP;
	input4[2].type = INPUT_KEYBOARD;
	input4[2].ki.wVk = 0;
	input4[2].ki.time = 0;
	input4[2].ki.wScan = 0x1E;
	input4[2].ki.dwFlags = KEYEVENTF_SCANCODE | KEYEVENTF_KEYUP;
	input5.type = INPUT_MOUSE;
	input5.mi.mouseData = 0;
	input5.mi.time = 0;
	//input5.mi.dx = LONG_MIN+10;
	input5.mi.dwFlags = MOUSEEVENTF_MOVE | MOUSEEVENTF_MOVE_NOCOALESCE;
	Font.CreateFont(12,                            // Height
		0,                             // Width	
		0,                             // Escapement
		0,                             // Orientation
		FW_NORMAL,                       // Weight
		FALSE,                         // Italic
		FALSE,                          // Underline
		0,                             // StrikeOut
		ANSI_CHARSET,                  // CharSet
		OUT_DEFAULT_PRECIS,            // OutPrecision
		CLIP_DEFAULT_PRECIS,           // ClipPrecision
		DEFAULT_QUALITY,               // Quality
		DEFAULT_PITCH | FF_SWISS,      // PitchAndFamily
		"Arial");
#ifdef DEBUG
	AllocConsole();
#endif
		t1 = thread(&gtahelperDlg::detectkey,this);
}
gtahelperDlg::~gtahelperDlg()
{
	Shell_NotifyIcon(NIM_DELETE,&m_nidIconData);
	t1en = false;
	t2en = false;
	t1.join();
	t2.join();
#ifdef DEBUG
	FreeConsole();
#endif
}
void gtahelperDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}
template<class TYPE>
bool RegSetKey(HKEY key,LPSTR keyloc,unsigned long type, REGSAM access,LPSTR name,TYPE indatax)
{
	unsigned long size = sizeof(type);
	char errorbuf[200];
	HKEY keyval;
	bool onerr = 1;
	int err;
	err = RegOpenKeyExA(key, keyloc, NULL, access, &keyval);
	if (err != ERROR_SUCCESS)
	{
		sprintf_s(errorbuf, "%i\n", err);
		onerr = 0;
		ASSERT(errorbuf);
	} else if(err == ERROR_SUCCESS)
	{ 
		err = RegSetValueExA(keyval, name, NULL, type, (BYTE*)indatax, size);
		if (err != ERROR_SUCCESS)
		{
			sprintf_s(errorbuf, "%i\n", err);
			onerr = 0;
			ASSERT(errorbuf);
		}
	}
	
		CloseHandle(keyval);
	return onerr;
}
int RegCrtKey(HKEY key, LPSTR keyloc, REGSAM access)
{
	HKEY keyval;
	int err;
	char errorbuf[200];
	DWORD dispvalue;
	err = RegCreateKeyExA(key, keyloc, NULL, NULL, REG_OPTION_NON_VOLATILE, access,NULL, &keyval, &dispvalue);
	CloseHandle(keyval);
	if (err == ERROR_SUCCESS)
	{
		if (dispvalue == REG_CREATED_NEW_KEY)
		{
			return 1;
		}
		else
		{
			return 2;
		}
	}
	else
	{
		sprintf_s(errorbuf, "%i\n", err);
		ASSERT(errorbuf);
		return 0;
	}
	//return onerr;
}
template<class TYPE>
int RegGetKey(HKEY key, LPSTR keyloc, unsigned long type, REGSAM access, LPSTR name, TYPE outdatax)
{
	unsigned long size = sizeof(type);
	char errorbuf[200];
	HKEY keyval;
	int onerr = 0;
	int err;
	err = RegOpenKeyExA(key, keyloc, NULL, access, &keyval);
	if (err != ERROR_SUCCESS)
	{
		onerr = false;
	}
	err = RegQueryValueExA(keyval, name, NULL, &type, (BYTE*)outdatax, &size);
	switch (err)
	{
	case ERROR_FILE_NOT_FOUND:
	{
		onerr = 2;
		break;
	} 
	case ERROR_MORE_DATA:
	{
		onerr = 3;
		break;
	}
	case ERROR_SUCCESS:
	{
		onerr = 1;
		break;
	}
	default:
	{
		sprintf_s(errorbuf, "%i\n", err);
		ASSERT(errorbuf);
		onerr = 0;
		break;
	}
	}
		CloseHandle(keyval);
	return onerr;
}
BEGIN_MESSAGE_MAP(gtahelperDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_SYSCOMMAND()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(ID_EXIT, &gtahelperDlg::OnBnClickedExit)
	ON_MESSAGE(WM_TRAY_ICON_NOTIFY_MESSAGE, OnTrayNotify)
	ON_BN_CLICKED(IDC_TRAYEN, &gtahelperDlg::OnBnClickedTrayEn)
	ON_BN_CLICKED(IDC_MINTRAY, &gtahelperDlg::OnBnClickedMinEn)
	ON_COMMAND(ID_MENU_CLOSE, &gtahelperDlg::OnBnClickedExit)
	ON_COMMAND(ID_MENU_OPEN, &gtahelperDlg::OnOpen)
	ON_COMMAND(ID_MENU_MINIMIZE, &gtahelperDlg::OnMinimize)
	ON_BN_CLICKED(IDC_MOVEW, &gtahelperDlg::OnBnClickedMovew)
	ON_BN_CLICKED(IDC_MOVEAFK, &gtahelperDlg::OnBnClickedMoveafk)
	ON_BN_CLICKED(IDC_MOVEW8, &gtahelperDlg::OnBnClickedMovew8)
	ON_BN_CLICKED(IDC_STOP, &gtahelperDlg::OnBnClickedStop)
	ON_BN_CLICKED(IDC_PUBSOLO, &gtahelperDlg::OnBnClickedPubsolo)
	ON_BN_CLICKED(IDC_RESELEV, &gtahelperDlg::OnBnClickedReselev)
END_MESSAGE_MAP()

BOOL gtahelperDlg::OnInitDialog()
{
	int out;
	DWORD indata =  1;
	DWORD outdata = 0;
	
	
	//AfxInitRichEdit2();
	gtahelperDlg::ShowWindow(SW_SHOW);
	gtahelperDlg::RedrawWindow();
	gtahelperDlg::CenterWindow();
	CDialog::OnInitDialog();
	out = RegCrtKey(HKEY_CURRENT_USER, "Software\\gtahelper", KEY_ALL_ACCESS | KEY_WOW64_64KEY);
	if (out == 1)
	{
		RegSetKey(HKEY_CURRENT_USER, "Software\\gtahelper", REG_DWORD, KEY_ALL_ACCESS | KEY_WOW64_64KEY, "TrayEnable", &indata);
		RegSetKey(HKEY_CURRENT_USER, "Software\\gtahelper", REG_DWORD, KEY_ALL_ACCESS | KEY_WOW64_64KEY, "TrayMinimize", &indata);
	}
	else if(out == 0)
	{
		trayenable = 1;
		minimizeen = 1;
		m_bMinimizeToTray = TRUE;
	}
	else if (out == 2)
	{
		out = RegGetKey(HKEY_CURRENT_USER, "Software\\gtahelper", NULL, KEY_ALL_ACCESS | KEY_WOW64_64KEY, "TrayMinimize", &outdata);
		if (out == 2)
		{
			
			RegSetKey(HKEY_CURRENT_USER, "Software\\gtahelper", REG_DWORD, KEY_ALL_ACCESS | KEY_WOW64_64KEY, "TrayMinimize", &indata);
			
			minimizeen = 1;
			m_bMinimizeToTray = TRUE;
		} else if (out == 1)
		{ 
			
			if (outdata == 1)
			{
				minimizeen = 1;
				m_bMinimizeToTray = TRUE;
			}
			else 
			{
				minimizeen = 0;
				m_bMinimizeToTray = FALSE;
			}
		}
		out = RegGetKey(HKEY_CURRENT_USER, "Software\\gtahelper", REG_DWORD, KEY_ALL_ACCESS | KEY_WOW64_64KEY, "TrayEnable", &outdata);
		if (out == 2)
		{

			RegSetKey(HKEY_CURRENT_USER, "Software\\gtahelper", REG_DWORD, KEY_ALL_ACCESS | KEY_WOW64_64KEY, "TrayMinimize", &indata);
			trayenable = 1;
		}
		else if (out == 1)
		{ 
			if (outdata == 1)
			{
				trayenable = 1;
			}
			else
			{
				trayenable = 0;
			}
		}
	}
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	TraySetIcon(m_hIcon);
	TraySetToolTip("gtahelper Tray icon");
	TraySetMenu(ID_MENU_MINIMIZE);
	
	if (gtahelperDlg::IsWindowVisible() != 0)
	{
		checkbox = (CButton*)GetDlgItem(IDC_MINTRAY);
		trayen = (CButton*)GetDlgItem(IDC_TRAYEN);
		reselev = (CButton*)GetDlgItem(IDC_RESELEV);
		
		if (minimizeen)
		{
			CheckDlgButton(IDC_MINTRAY, BST_CHECKED);
		}
		else
		{
			CheckDlgButton(IDC_MINTRAY, BST_UNCHECKED);
		}
		if (trayenable)
		{
			CheckDlgButton(IDC_TRAYEN, BST_CHECKED);
			TrayShow();
		}
		else
		{
			CheckDlgButton(IDC_TRAYEN, BST_UNCHECKED);
			TrayHide();
		}
		if (IsRunAsAdministrator())
		{
			reselev->EnableWindow(0);
			reselev->ShowWindow(SW_HIDE);
		}
	}
	
	return TRUE;
}
void gtahelperDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
	gtahelperDlg::ShowWindow(SW_SHOW);
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR gtahelperDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void gtahelperDlg::OnBnClickedExit()
{
	//gtahelperDlg::OnDestroy();
	//exit(2);
	//PostQuitMessage(2);
	SendMessage(WM_COMMAND, IDOK);
}

void gtahelperDlg::OnBnClickedMinEn()
{
	DWORD indata = 0;
	 int boxcheck = checkbox->GetCheck();
	if (boxcheck == BST_CHECKED)
	{
		indata = 1;
		RegSetKey(HKEY_CURRENT_USER, "Software\\gtahelper", REG_DWORD, KEY_ALL_ACCESS | KEY_WOW64_64KEY, "TrayMinimize", &indata);
		m_bMinimizeToTray = TRUE;
		minimizeen = true;
	}
	else
	{
		indata = 0;
		RegSetKey(HKEY_CURRENT_USER, "Software\\gtahelper", REG_DWORD, KEY_ALL_ACCESS | KEY_WOW64_64KEY, "TrayMinimize", &indata);
		m_bMinimizeToTray = FALSE;
		minimizeen = false;
	}
}
void gtahelperDlg::OnBnClickedTrayEn()
{
	int tren;
	DWORD indata = 0;
	tren = trayen->GetCheck();
	if (tren == BST_CHECKED)
	{
		indata = 1;
		RegSetKey(HKEY_CURRENT_USER, "Software\\gtahelper", REG_DWORD, KEY_ALL_ACCESS | KEY_WOW64_64KEY, "TrayEnable", &indata);
		trayenable = true;
		checkbox->EnableWindow();
		TrayShow();
	}
	else
	{
		indata = 0;
		RegSetKey(HKEY_CURRENT_USER, "Software\\gtahelper", REG_DWORD, KEY_ALL_ACCESS | KEY_WOW64_64KEY, "TrayEnable", &indata);
		trayenable = false;
		m_bMinimizeToTray = FALSE;
		checkbox->EnableWindow(0);
		TrayHide();
	}
}
void gtahelperDlg::OnOpen()
{
	if (this->IsWindowVisible())
	{ 
		this->ShowWindow(SW_NORMAL);
		this->SetFocus();
	}
	else
	{	
		this->ShowWindow(SW_SHOW);
		this->SetFocus();
	}
	
	
}

int gtahelperDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

	m_nidIconData.hWnd = this->m_hWnd;
	m_nidIconData.uID = 1;

	return 0;
}

void gtahelperDlg::OnDestroy()
{
	CDialog::OnDestroy();
	/*if (m_nidIconData.hWnd && m_nidIconData.uID > 0 && TrayIsVisible())
	{
		Shell_NotifyIcon(NIM_DELETE, &m_nidIconData);
	}*/
}

BOOL gtahelperDlg::TrayIsVisible()
{
	return m_bTrayIconVisible;
}

void gtahelperDlg::TraySetIcon(HICON hIcon)
{
	ASSERT(hIcon);

	m_nidIconData.hIcon = hIcon;
	m_nidIconData.uFlags |= NIF_ICON;
}

void gtahelperDlg::TraySetIcon(UINT nResourceID)
{
	ASSERT(nResourceID > 0);
	HICON hIcon = 0;
	hIcon = AfxGetApp()->LoadIcon(nResourceID);
	if (hIcon)
	{
		m_nidIconData.hIcon = hIcon;
		m_nidIconData.uFlags |= NIF_ICON;
	}
	else
	{
		TRACE0("FAILED TO LOAD ICON\n");
	}
}

void gtahelperDlg::TraySetIcon(LPCTSTR lpszResourceName)
{
	HICON hIcon = 0;
	hIcon = AfxGetApp()->LoadIcon(lpszResourceName);
	if (hIcon)
	{
		m_nidIconData.hIcon = hIcon;
		m_nidIconData.uFlags |= NIF_ICON;
	}
	else
	{
		TRACE0("FAILED TO LOAD ICON\n");
	}
}

void gtahelperDlg::TraySetToolTip(LPCTSTR lpszToolTip)
{
	ASSERT(strlen((char*)lpszToolTip) > 0 && strlen((char*)lpszToolTip) < 64);
#if defined(UNICODE) || defined(_UNICODE)
	wcscpy(m_nidIconData.szTip,lpszToolTip);
#else
	strcpy(m_nidIconData.szTip, lpszToolTip);
#endif


	m_nidIconData.uFlags |= NIF_TIP;
}

BOOL gtahelperDlg::TrayShow()
{
	BOOL bSuccess = FALSE;
	if (!m_bTrayIconVisible && trayenable)
	{
		bSuccess = Shell_NotifyIcon(NIM_ADD, &m_nidIconData);
		if (bSuccess)
			m_bTrayIconVisible = TRUE;
	}
	else
	{
		TRACE0("ICON ALREADY VISIBLE");
	}
	return bSuccess;
}

BOOL gtahelperDlg::TrayHide()
{
	BOOL bSuccess = FALSE;
	if (m_bTrayIconVisible)
	{
		bSuccess = Shell_NotifyIcon(NIM_DELETE, &m_nidIconData);
		if (bSuccess)
			m_bTrayIconVisible = FALSE;
	}
	else
	{
		TRACE0("ICON ALREADY HIDDEN");
	}
	bSuccess = TRUE;
	return bSuccess;
}

BOOL gtahelperDlg::TrayUpdate()
{
	BOOL bSuccess = FALSE;
	if (m_bTrayIconVisible)
	{
		bSuccess = Shell_NotifyIcon(NIM_MODIFY, &m_nidIconData);
	}
	else
	{
		TRACE0("ICON NOT VISIBLE");
	}
	return bSuccess;
}


BOOL gtahelperDlg::TraySetMenu(UINT nResourceID, UINT nDefaultPos)
{
	BOOL bSuccess;
	bSuccess = m_mnuTrayMenu.LoadMenu(nResourceID);
	return bSuccess;
}


BOOL gtahelperDlg::TraySetMenu(LPCTSTR lpszMenuName, UINT nDefaultPos)
{
	BOOL bSuccess;
	bSuccess = m_mnuTrayMenu.LoadMenu(lpszMenuName);
	return bSuccess;
}

BOOL gtahelperDlg::TraySetMenu(HMENU hMenu, UINT nDefaultPos)
{
	m_mnuTrayMenu.Attach(hMenu);
	return TRUE;
}

LRESULT gtahelperDlg::OnTrayNotify(WPARAM wParam, LPARAM lParam)
{
	UINT uID;
	UINT uMsg;

	uID = (UINT)wParam;
	uMsg = (UINT)lParam;

	if (uID != 1)
		return false;

	CPoint pt;

	switch (uMsg)
	{
	case WM_MOUSEMOVE:
		GetCursorPos(&pt);
		ClientToScreen(&pt);
		OnTrayMouseMove(pt);
		break;
	case WM_LBUTTONDOWN:
		GetCursorPos(&pt);
		ClientToScreen(&pt);
		OnTrayLButtonDown(pt);
		break;
	case WM_LBUTTONDBLCLK:
		GetCursorPos(&pt);
		ClientToScreen(&pt);
		OnTrayLButtonDblClk(pt);
		break;

	case WM_RBUTTONDOWN:
	case WM_CONTEXTMENU:
		GetCursorPos(&pt);
		//ClientToScreen(&pt);
		OnTrayRButtonDown(pt);
		break;
	case WM_RBUTTONDBLCLK:
		GetCursorPos(&pt);
		ClientToScreen(&pt);
		OnTrayRButtonDblClk(pt);
		break;
	}
	return true;
}
void gtahelperDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if (m_bMinimizeToTray)
	{
		if ((nID & 0xFFF0) == SC_MINIMIZE)
		{
		
			if (TrayShow() || m_bTrayIconVisible)
			{
				this->ShowWindow(SW_HIDE);
			}
		}
		else
		{
			CDialog::OnSysCommand(nID, lParam);
		}
	}
	else
	{ 
		CDialog::OnSysCommand(nID, lParam);
	}
}
void gtahelperDlg::TraySetMinimizeToTray(BOOL bMinimizeToTray)
{
	m_bMinimizeToTray = bMinimizeToTray;
}


void gtahelperDlg::OnTrayRButtonDown(CPoint pt)
{
	m_mnuTrayMenu.GetSubMenu(0)->TrackPopupMenu(TPM_BOTTOMALIGN | TPM_LEFTBUTTON | TPM_RIGHTBUTTON, pt.x, pt.y, this);
	m_mnuTrayMenu.GetSubMenu(0)->SetDefaultItem(m_nDefaultMenuItem, TRUE);
}

void gtahelperDlg::OnTrayLButtonDown(CPoint pt)
{

}

void gtahelperDlg::OnTrayLButtonDblClk(CPoint pt)
{
			this->ShowWindow(SW_SHOW);
			this->SetFocus();
}

void gtahelperDlg::OnTrayRButtonDblClk(CPoint pt)
{

}

void gtahelperDlg::OnTrayMouseMove(CPoint pt)
{
}
void gtahelperDlg::OnMinimize()
{
	if (this->IsWindowVisible())
	{
		if (trayenable && !minimizeen)
		{
			ShowWindow(SW_MINIMIZE);
		}
		else if (trayenable && minimizeen)
		{
			ShowWindow(SW_HIDE);
		}
	}
	else
	{
		ShowWindow(SW_SHOW);
		SetFocus();
	}
}

void gtahelperDlg::OnBnClickedMovew()
{
	SendInput(1, &input1, sizeof(INPUT));
}

void gtahelperDlg::OnBnClickedMoveafk()
{
	SendInput(1, &input2, sizeof(INPUT));
	t2en = true;
	if (!t2.joinable())
	{
		t2 = thread(&gtahelperDlg::loopmouse, this);
	}
}

void gtahelperDlg::OnBnClickedMovew8()
{
	SendInput(2, input3, sizeof(INPUT));
}

void gtahelperDlg::OnBnClickedStop()
{
	SendInput(3, input4, sizeof(INPUT));
	t2en = false;
	if(t2.joinable())
	{ 
	t2.join();
	}
	ZeroMemory(&t2, sizeof(t2));
}


void gtahelperDlg::OnBnClickedPubsolo()
{
	thread t3(&gtahelperDlg::createsolo, this);
	t3.detach();
}


void gtahelperDlg::OnBnClickedReselev()
{
	if (!IsRunAsAdministrator())
	{
		ElevateNow();
		SendMessage(WM_COMMAND, IDOK);
	}
}
