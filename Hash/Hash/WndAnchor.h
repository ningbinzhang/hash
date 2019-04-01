#pragma once
#include <list>

using namespace std;

#define ANCHOR_WHERE_LEFT		0x1
#define ANCHOR_WHERE_RIGHT		0x2
#define ANCHOR_WHERE_HCENTER	0x3	//ˮƽ����

#define ANCHOR_WHERE_TOP    0x10
#define ANCHOR_WHERE_BOTTOM 0x20
#define ANCHOR_WHERE_VCENTER 0x30	//��ֱ����


#define CHECK_ANCHOR_WHERE	TRUE	//����λ��

#define ANCHOR_WHERE_HCENTER_MASK 0xF	//ˮƽ����
#define ANCHOR_WHERE_VCENTER_MASK 0xF0	//��ֱ����


class  CWndAnchor
{
public:
	CWndAnchor();
	~CWndAnchor();

public:
	BOOL SetAnchor(HWND hWnd, UINT uWhere, HWND hAnchorWnd, UINT uAnchorWhere, int iOffset);
	BOOL SetAnchor(CWnd & cWnd, UINT uWhere, CWnd & cAnchorWnd, UINT uAnchorWhere, int iOffset);
	BOOL SetAnchor(CWnd * pWnd, UINT uWhere, CWnd * pAnchorWnd, UINT uAnchorWhere, int iOffset);

	//�󶨵�ָ������ĳ����
	BOOL SetAnchorToSide(CWnd & cWnd, CWnd & cAnchorWnd, UINT uAnchorWhere, int iOffset, BOOL bInside = FALSE);

	//����
	BOOL SetAnchorToAlign(CWnd & cWnd, CWnd & cAnchorWnd, UINT uAnchorWhere, int iOffset = 0);
	

	BOOL RemoveAnchor(HWND hWnd, UINT uWhere);

	BOOL Relayout();
	BOOL Relayout(HWND hWnd);

	void Clear();

protected:


	typedef struct tagAnchorWhere{
		HWND hAnchorWnd;
		UINT uAnchorWhere;
		int iOffset;
		int GetAnchorPos(HWND hWnd);
		int GetAnchorRect(HWND hWnd, CRect & rect);
	}ANCHORWHERE, *LPANCHORWHERE;
	typedef struct tagAnchorWndData{
		HWND hWnd;
		ANCHORWHERE anchorLeft;
		ANCHORWHERE anchorTop;
		ANCHORWHERE anchorRight;
		ANCHORWHERE anchorBottom;
		ANCHORWHERE anchorHCenter;	//ˮƽ����
		ANCHORWHERE anchorVCenter;	//��ֱ����
		int GetWindowRect(CRect & rect);
	}ANCHORWNDDATA, *LPANCHORWNDDATA;

	LPANCHORWNDDATA FindAnchorWnd(HWND hWnd);
	BOOL OnAnchor(LPANCHORWNDDATA pAnchor);

	typedef list<LPANCHORWNDDATA> ANCHORLIST;
	ANCHORLIST m_listAnchor;
};

