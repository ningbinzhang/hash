// HashDlg.h : 头文件
//

#ifndef HASH_DLG_H
#define HASH_DLG_H
#pragma once
#include "Hashs.h"
#include "afxcmn.h"
#include "afxwin.h"
#include "WndAnchor.h"


// CHashDlg 对话框
class CHashDlg : public CDialog , public IHashCallback
{
// 构造
public:
	CHashDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_HASH_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;
	CWndAnchor m_wndAnchor;
	CHashsListAsyn m_hashList;
	CEdit m_wndEdOut;
	CString m_strOutTexts;
	CProgressCtrl m_wndPCProcess;
	int m_iNumber;


	CWinThread * m_pThead;
	CCriticalSection m_lock;
	volatile BOOL m_bUpdateText;

	CFile m_fLog;
	CArchive m_arLog;


	BOOL AddFile(LPCTSTR lpFile);


	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnFinish(LPCTSTR lpText);
	virtual BOOL OnProcess(LONG64 lTol, LONG64 lCur);

	afx_msg void OnEnChangeEdit1();
	afx_msg void OnDropFiles(HDROP hDropInfo);
	
	//CRichEditCtrl m_wndRECOut;
	afx_msg void OnBnClickedOk();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnBnClickedBtClear();
	afx_msg void OnUpdateHashCrc32(CCmdUI *pCmdUI);
	afx_msg void OnUpdateHashMd5(CCmdUI *pCmdUI);
	afx_msg void OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu);
	afx_msg void OnBnClickedBtCheckHash();
	afx_msg void OnHashCrc32();
	afx_msg void OnHashMd5();
	afx_msg void OnHashSha1();
	afx_msg void OnHashSha256();
	afx_msg void OnHashSha512();
	afx_msg void OnUpdateHashSha1(CCmdUI *pCmdUI);
	afx_msg void OnUpdateHashSha256(CCmdUI *pCmdUI);
	afx_msg void OnUpdateHashSha512(CCmdUI *pCmdUI);
	afx_msg void OnUpdateHashFilename(CCmdUI *pCmdUI);
	afx_msg void OnUpdateHashFilesize(CCmdUI *pCmdUI);
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedBtCopy();
	afx_msg void OnBnClickedButton3();
	afx_msg void OnBnClickedButton4();
};

#endif