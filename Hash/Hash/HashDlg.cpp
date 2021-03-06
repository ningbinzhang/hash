// HashDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "Hash.h"
#include "HashDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
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


// CHashDlg 对话框


DWORD CALLBACK RicheditInCallback(DWORD_PTR dwCookie, LPBYTE pbBuff, LONG cb, LONG *pcb)
{
	CString *psBuffer = (CString *)dwCookie;
	if (cb > psBuffer->GetLength()) cb = psBuffer->GetLength();

	for (int i=0;i<cb;i++)
	{
		*(pbBuff+i) = psBuffer->GetAt(i);
	}

	*pcb = cb;

	*psBuffer = psBuffer->Mid(cb);

	return 0;
}

DWORD CALLBACK RicheditOutCallback(DWORD_PTR dwCookie, LPBYTE pbBuff, LONG cb, LONG *pcb)
{
	CString sThisWrite;
	sThisWrite.GetBufferSetLength(cb);

	CString *psBuffer = (CString *)dwCookie;

	for (int i=0;i<cb;i++) {
		sThisWrite.SetAt(i,*(pbBuff+i));
	}

	*psBuffer += sThisWrite;

	*pcb = sThisWrite.GetLength();
	sThisWrite.ReleaseBuffer();

	return 0;
}



CHashDlg::CHashDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CHashDlg::IDD, pParent)
	, m_strOutTexts(_T("1.拖拽文件到窗口开始计算HASH值。\r\n2.点击“选择HASH”选择需要计算的HASH值（默认计算CRC32,MD5,SHA1,SHA256,SHA512）。"))
	, m_pThead(NULL)
	, m_iNumber(1)
	, m_bUpdateText(FALSE)
	, m_arLog(&m_fLog, CArchive::store, 1024*1024)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CHashDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_strOutTexts);
	//DDX_Control(pDX, IDC_RICHEDIT21, m_wndRECOut);
	DDX_Control(pDX, IDC_EDIT1, m_wndEdOut);
	DDX_Control(pDX, IDC_PROGRESS1, m_wndPCProcess);
}

BEGIN_MESSAGE_MAP(CHashDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_EN_CHANGE(IDC_EDIT1, &CHashDlg::OnEnChangeEdit1)
	ON_WM_DROPFILES()
	ON_BN_CLICKED(IDOK, &CHashDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CHashDlg::OnBnClickedOk)
	ON_WM_SIZE()
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BT_CLEAR, &CHashDlg::OnBnClickedBtClear)
	ON_UPDATE_COMMAND_UI(ID_HASH_CRC32, &CHashDlg::OnUpdateHashCrc32)
	ON_UPDATE_COMMAND_UI(ID_HASH_MD5, &CHashDlg::OnUpdateHashMd5)
	ON_WM_INITMENUPOPUP()
	ON_BN_CLICKED(IDC_BT_CHECK_HASH, &CHashDlg::OnBnClickedBtCheckHash)
	ON_COMMAND(ID_HASH_CRC32, &CHashDlg::OnHashCrc32)
	ON_COMMAND(ID_HASH_MD5, &CHashDlg::OnHashMd5)
	ON_COMMAND(ID_HASH_SHA1, &CHashDlg::OnHashSha1)
	ON_COMMAND(ID_HASH_SHA256, &CHashDlg::OnHashSha256)
	ON_COMMAND(ID_HASH_SHA512, &CHashDlg::OnHashSha512)
	ON_UPDATE_COMMAND_UI(ID_HASH_SHA1, &CHashDlg::OnUpdateHashSha1)
	ON_UPDATE_COMMAND_UI(ID_HASH_SHA256, &CHashDlg::OnUpdateHashSha256)
	ON_UPDATE_COMMAND_UI(ID_HASH_SHA512, &CHashDlg::OnUpdateHashSha512)
	ON_UPDATE_COMMAND_UI(ID_HASH_FILENAME, &CHashDlg::OnUpdateHashFilename)
	ON_UPDATE_COMMAND_UI(ID_HASH_FILESIZE, &CHashDlg::OnUpdateHashFilesize)
	ON_BN_CLICKED(IDC_BUTTON1, &CHashDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BT_COPY, &CHashDlg::OnBnClickedBtCopy)
	ON_BN_CLICKED(IDC_BUTTON3, &CHashDlg::OnBnClickedButton3)
	ON_BN_CLICKED(IDC_BUTTON4, &CHashDlg::OnBnClickedButton4)
END_MESSAGE_MAP()


// CHashDlg 消息处理程序

BOOL CHashDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	DragAcceptFiles();


	m_hashList.SetCallback(this);

	m_wndAnchor.SetAnchorToSide(m_wndEdOut, *this, ANCHOR_WHERE_LEFT, 5, TRUE);
	m_wndAnchor.SetAnchorToSide(m_wndEdOut, *this, ANCHOR_WHERE_RIGHT, 5, TRUE);
	m_wndAnchor.SetAnchorToSide(m_wndEdOut, *this, ANCHOR_WHERE_TOP, 35, TRUE);
	m_wndAnchor.SetAnchorToSide(m_wndEdOut, *this, ANCHOR_WHERE_BOTTOM, 15, TRUE);

	m_wndAnchor.SetAnchorToSide(m_wndPCProcess, *this, ANCHOR_WHERE_LEFT, 5, TRUE);
	m_wndAnchor.SetAnchorToSide(m_wndPCProcess, *this, ANCHOR_WHERE_RIGHT, 5, TRUE);
	m_wndAnchor.SetAnchorToSide(m_wndPCProcess, m_wndEdOut, ANCHOR_WHERE_BOTTOM, 1, FALSE);
	m_wndAnchor.SetAnchorToSide(m_wndPCProcess, *this, ANCHOR_WHERE_BOTTOM, 5, TRUE);

	m_wndAnchor.Relayout();

	SetTimer(1, 500, NULL);

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CHashDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CHashDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CHashDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CHashDlg::OnEnChangeEdit1()
{
	// TODO:  如果该控件是 RICHEDIT 控件，则它将不会
	// 发送该通知，除非重写 CDialog::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	// TODO:  在此添加控件通知处理程序代码
}

UINT STCalcThread(LPVOID lpvoid)
{
	CHashsListAsyn * pList = (CHashsListAsyn*)lpvoid;
	pList->CalcHashsList();
	return 0;
}

void CHashDlg::OnDropFiles(HDROP hDropInfo)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	
	int iCount = DragQueryFile(hDropInfo,0xFFFFFFFF,NULL,0);
	

	CString strPath;
	DWORD dwSize = 0;
	for(int i=0; i<iCount; i++)
	{
		dwSize = DragQueryFile(hDropInfo, i, NULL, 0);
		DragQueryFile(hDropInfo, i, strPath.GetBuffer(dwSize), dwSize+1);
		strPath.ReleaseBuffer();

		m_hashList.AddFile(strPath);

		

		/*CString strRtf = h.ExportHashTextRtf();

		EDITSTREAM es = {0};
		es.dwCookie = (DWORD_PTR)&strRtf;
		es.dwError = 0;
		es.pfnCallback = RicheditInCallback;

		m_wndRECOut.StreamIn(SF_RTF, es);*/

		/*long lBegin = m_wndRECOut.GetTextLength();

		

		m_wndRECOut.SetSel(-1, -1);
		m_wndRECOut.ReplaceSel(strHash);

		long lEnd = m_wndRECOut.GetTextLength();

		int iPos = strHash.Find(_T("\r\n"));

		CHARFORMAT2 cf;
		ZeroMemory(&cf, sizeof(cf));
		cf.cbSize = sizeof(cf);
		cf.crTextColor = RGB(255, 0, 0);
		cf.dwMask = CFM_COLOR;

		m_wndRECOut.SetSel(lBegin, lBegin+iPos);
		m_wndRECOut.SetSelectionCharFormat(cf);

		long lBegin2 = lBegin + iPos + 2;
		iPos = strHash.Find(_T("："), lBegin2);

		cf.crTextColor = RGB(0, 128, 0);
		m_wndRECOut.SetSel(lBegin2, lBegin+iPos);*/

		

		UpdateData(FALSE);
	}

	DragFinish(hDropInfo);

	AfxBeginThread(STCalcThread, &m_hashList);

	CDialog::OnDropFiles(hDropInfo);
}

void CHashDlg::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	m_arLog.Close();
	m_fLog.Close();
	OnOK();
	/*CString sReadText;

	EDITSTREAM es = {0};
	es.dwCookie = (DWORD_PTR)&sReadText;
	es.dwError = 0;
	es.pfnCallback = RicheditOutCallback;

	m_wndRECOut.StreamOut(SF_RTF, es);*/

}

void CHashDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	// TODO: 在此处添加消息处理程序代码

	m_wndAnchor.Relayout();
}

BOOL CHashDlg::OnFinish( LPCTSTR lpText )
{
	if (lpText == NULL || _tcslen(lpText) == 0)
	{
		m_hashList.Clear();
		m_lock.Lock();
		m_pThead = NULL;
		m_lock.Unlock();
	}else{
		if ((HANDLE)m_fLog != INVALID_HANDLE_VALUE)
		{
			while(*lpText != 0){
				m_arLog << *lpText;
				lpText ++;
			};
		}else{
			m_lock.Lock();
			CString strBegin;
			strBegin.Format(_T("\r\n------------------------------- %d -----------------------------\r\n"), m_iNumber++);
			m_strOutTexts += strBegin;

			m_strOutTexts += lpText;

			//UpdateData(FALSE);
			//m_wndEdOut.LineScroll(m_wndEdOut.GetLineCount());
			m_bUpdateText = TRUE;
			m_lock.Unlock();
		}
		
	}
	

	return TRUE;
}

BOOL CHashDlg::OnProcess( LONG64 lTol, LONG64 lCur )
{
	return TRUE;
}

void CHashDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	
	LONG64 lTol, lCur;
	m_hashList.GetProcess(lTol, lCur);

	if (lTol > 0xfffffff)
	{
		lTol /= 0xfffff;
		lCur /= 0xfffff;
	}

	m_wndPCProcess.SetRange32(0, lTol);
	m_wndPCProcess.SetPos(lCur);

	if (m_bUpdateText)
	{
		m_lock.Lock();
		m_bUpdateText = FALSE;
		UpdateData(FALSE);
		m_wndEdOut.LineScroll(m_wndEdOut.GetLineCount());
		m_lock.Unlock();
	}

	__super::OnTimer(nIDEvent);
}

void CHashDlg::OnBnClickedBtClear()
{
	// TODO: 在此添加控件通知处理程序代码
	m_strOutTexts.Empty();
	UpdateData(FALSE);
}

void CHashDlg::OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu)
{
//	__super::OnInitMenuPopup(pPopupMenu, nIndex, bSysMenu);

	// TODO: 在此处添加消息处理程序代码
	ASSERT(pPopupMenu != NULL);  
	// Check the enabled state of various menu items.  
	  
	CCmdUI state;  
	state.m_pMenu = pPopupMenu;  
	ASSERT(state.m_pOther == NULL);  
	ASSERT(state.m_pParentMenu == NULL);  
	  
	// Determine if menu is popup in top-level menu and set m_pOther to  
	// it if so (m_pParentMenu == NULL indicates that it is secondary popup).  
	HMENU hParentMenu;  
	if (AfxGetThreadState()->m_hTrackingMenu == pPopupMenu->m_hMenu)  
		state.m_pParentMenu = pPopupMenu; // Parent == child for tracking popup.  
	else if ((hParentMenu = ::GetMenu(m_hWnd)) != NULL)  
	{  
		CWnd* pParent = this;  
		// Child windows don't have menus--need to go to the top!  
		if (pParent != NULL &&  
			(hParentMenu = ::GetMenu(pParent->m_hWnd)) != NULL)  
		{  
			int nIndexMax = ::GetMenuItemCount(hParentMenu);  
			for (int nIndex = 0; nIndex < nIndexMax; nIndex++)  
			{  
				if (::GetSubMenu(hParentMenu, nIndex) == pPopupMenu->m_hMenu)  
				{  
					// When popup is found, m_pParentMenu is containing menu.  
					state.m_pParentMenu = CMenu::FromHandle(hParentMenu);  
					break;  
				}  
			}  
		}  
	}  
	  
	state.m_nIndexMax = pPopupMenu->GetMenuItemCount();  
	for (state.m_nIndex = 0; state.m_nIndex < state.m_nIndexMax;  
	state.m_nIndex++)  
	{  
		state.m_nID = pPopupMenu->GetMenuItemID(state.m_nIndex);  
		if (state.m_nID == 0)  
			continue; // Menu separator or invalid cmd - ignore it.  
		  
		ASSERT(state.m_pOther == NULL);  
		ASSERT(state.m_pMenu != NULL);  
		if (state.m_nID == (UINT)-1)  
		{  
			// Possibly a popup menu, route to first item of that popup.  
			state.m_pSubMenu = pPopupMenu->GetSubMenu(state.m_nIndex);  
			if (state.m_pSubMenu == NULL ||  
				(state.m_nID = state.m_pSubMenu->GetMenuItemID(0)) == 0 ||  
				state.m_nID == (UINT)-1)  
			{  
				continue; // First item of popup can't be routed to.  
			}  
			state.DoUpdate(this, TRUE); // Popups are never auto disabled.  
		}  
		else  
		{  
			// Normal menu item.  
			// Auto enable/disable if frame window has m_bAutoMenuEnable  
			// set and command is _not_ a system command.  
			state.m_pSubMenu = NULL;  
			state.DoUpdate(this, FALSE);  
		}  
		  
		// Adjust for menu deletions and additions.  
		UINT nCount = pPopupMenu->GetMenuItemCount();  
		if (nCount < state.m_nIndexMax)  
		{  
			state.m_nIndex -= (state.m_nIndexMax - nCount);  
			while (state.m_nIndex < nCount &&  
				pPopupMenu->GetMenuItemID(state.m_nIndex) == state.m_nID)  
			{  
				state.m_nIndex++;  
			}  
		}  
		state.m_nIndexMax = nCount;  
	}  
}

void CHashDlg::OnUpdateHashCrc32(CCmdUI *pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
	pCmdUI->SetCheck(theApp.GetProfileInt(_T("Hash"), _T("Crc32"), 1));
}

void CHashDlg::OnUpdateHashMd5(CCmdUI *pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
	pCmdUI->SetCheck(theApp.GetProfileInt(_T("Hash"), _T("Md5"), 1));
}

void CHashDlg::OnBnClickedBtCheckHash()
{
	// TODO: 在此添加控件通知处理程序代码
	CMenu m;
	if (m.LoadMenu(IDR_MENU1))
	{
		CMenu * pSub = m.GetSubMenu(0);
		CWnd * pWnd = GetDlgItem(IDC_BT_CHECK_HASH);
		CRect rtButton;
		pWnd->GetWindowRect(rtButton);
		pSub->TrackPopupMenu(0, rtButton.left, rtButton.bottom, this);
	}
}

void CHashDlg::OnHashCrc32()
{
	// TODO: 在此添加命令处理程序代码
	theApp.WriteProfileInt(_T("Hash"), _T("Crc32"), theApp.GetProfileInt(_T("Hash"), _T("Crc32"), 1)?0:1);
}

void CHashDlg::OnHashMd5()
{
	// TODO: 在此添加命令处理程序代码
	theApp.WriteProfileInt(_T("Hash"), _T("Md5"), theApp.GetProfileInt(_T("Hash"), _T("Md5"), 1)?0:1);
}

void CHashDlg::OnHashSha1()
{
	// TODO: 在此添加命令处理程序代码
	theApp.WriteProfileInt(_T("Hash"), _T("Sha1"), theApp.GetProfileInt(_T("Hash"), _T("Sha1"), 1)?0:1);
}

void CHashDlg::OnHashSha256()
{
	// TODO: 在此添加命令处理程序代码
	theApp.WriteProfileInt(_T("Hash"), _T("Sha256"), theApp.GetProfileInt(_T("Hash"), _T("Sha256"), 1)?0:1);
}

void CHashDlg::OnHashSha512()
{
	// TODO: 在此添加命令处理程序代码
	theApp.WriteProfileInt(_T("Hash"), _T("Sha512"), theApp.GetProfileInt(_T("Hash"), _T("Sha512"), 1)?0:1);
}

void CHashDlg::OnUpdateHashSha1(CCmdUI *pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
	pCmdUI->SetCheck(theApp.GetProfileInt(_T("Hash"), _T("Sha1"), 1));
}

void CHashDlg::OnUpdateHashSha256(CCmdUI *pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
	pCmdUI->SetCheck(theApp.GetProfileInt(_T("Hash"), _T("Sha256"), 1));
}

void CHashDlg::OnUpdateHashSha512(CCmdUI *pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
	pCmdUI->SetCheck(theApp.GetProfileInt(_T("Hash"), _T("Sha512"), 1));
}

void CHashDlg::OnUpdateHashFilename(CCmdUI *pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
	pCmdUI->Enable(FALSE);
}

void CHashDlg::OnUpdateHashFilesize(CCmdUI *pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
	pCmdUI->Enable(FALSE);
}

void CHashDlg::OnBnClickedButton1()
{
	// TODO: 在此添加控件通知处理程序代码
	CFileDialog f(TRUE);
	if (f.DoModal() == IDOK)
	{
		m_hashList.AddFile(f.GetPathName());
		AfxBeginThread(STCalcThread, &m_hashList);
	}
}

void CHashDlg::OnBnClickedBtCopy()
{
	// TODO: 在此添加控件通知处理程序代码
	m_wndEdOut.SetSel(0, -1);
	m_wndEdOut.Copy();
}

BOOL CHashDlg::AddFile( LPCTSTR lpFile )
{
	m_hashList.AddFile(lpFile);
	m_lock.Lock();
	if (m_pThead == NULL)
	{
		m_pThead = AfxBeginThread(STCalcThread, &m_hashList);
	}
	m_lock.Unlock();
	return TRUE;
}

CString SelectFolder()
{
	CString strFolderPath;
	TCHAR szFolderPath[MAX_PATH] = {0};
	BROWSEINFO      sInfo;  
	::ZeroMemory(&sInfo, sizeof(BROWSEINFO));  
	sInfo.pidlRoot   = 0;  
	sInfo.lpszTitle   = _T("请选择要扫描的目录");  
	sInfo.ulFlags   = BIF_RETURNONLYFSDIRS|BIF_EDITBOX|BIF_DONTGOBELOWDOMAIN;  
	sInfo.lpfn     = NULL;  

	// 显示文件夹选择对话框  
	LPITEMIDLIST lpidlBrowse = ::SHBrowseForFolder(&sInfo);   
	if (lpidlBrowse != NULL)  
	{  
		// 取得文件夹名  
		if (::SHGetPathFromIDList(lpidlBrowse,szFolderPath))    
		{  
			strFolderPath = szFolderPath;  
		}  
	}  
	if(lpidlBrowse != NULL)  
	{  
		::CoTaskMemFree(lpidlBrowse);  
	}  

	return strFolderPath; 
}

void CHashDlg::OnBnClickedButton3()
{
	// TODO: 在此添加控件通知处理程序代码

	CFileDialog f(FALSE);
	if (f.DoModal() == IDOK)
	{
		if ((HANDLE)m_fLog != INVALID_HANDLE_VALUE)
		{
			m_fLog.Close();
		}
		m_fLog.Open(f.GetPathName(), CFile::modeCreate|CFile::modeReadWrite);
	}
}

void CHashDlg::OnBnClickedButton4()
{
	// TODO: 在此添加控件通知处理程序代码
	ShellExecute(GetSafeHwnd(), NULL, _T("http://www.ningos.com"), NULL, NULL, SW_SHOWNORMAL);
}
