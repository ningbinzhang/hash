#include "StdAfx.h"
#include "WndAnchor.h"



CWndAnchor::CWndAnchor()
{

}

CWndAnchor::~CWndAnchor()
{
	Clear();
}

BOOL CWndAnchor::SetAnchor( HWND hWnd, UINT uWhere, HWND hAnchorWnd, UINT uAnchorWhere, int iOffset )
{
	LPANCHORWNDDATA pAnchor = FindAnchorWnd(hWnd);
	if (pAnchor == NULL)
	{
		pAnchor = new ANCHORWNDDATA;
		memset(pAnchor, 0, sizeof(ANCHORWNDDATA));
		pAnchor->hWnd = hWnd;
		m_listAnchor.push_back(pAnchor);
	}
	ASSERT(pAnchor != NULL);
	LPANCHORWHERE pWhere = NULL;
	switch (uWhere)
	{
	case ANCHOR_WHERE_LEFT:
		ASSERT(CHECK_ANCHOR_WHERE || uWhere & ANCHOR_WHERE_HCENTER_MASK);
		pWhere = &pAnchor->anchorLeft;
		break;
	case ANCHOR_WHERE_RIGHT:
		ASSERT(CHECK_ANCHOR_WHERE || uWhere & ANCHOR_WHERE_HCENTER_MASK);
		pWhere = &pAnchor->anchorRight;
		break;
	case ANCHOR_WHERE_TOP:
		ASSERT(CHECK_ANCHOR_WHERE || uWhere & ANCHOR_WHERE_VCENTER_MASK);
		pWhere = &pAnchor->anchorTop;
		break;
	case ANCHOR_WHERE_BOTTOM:
		ASSERT(CHECK_ANCHOR_WHERE || uWhere & ANCHOR_WHERE_VCENTER_MASK);
		pWhere = &pAnchor->anchorBottom;
		break;
	case ANCHOR_WHERE_HCENTER:
		ASSERT(CHECK_ANCHOR_WHERE || uWhere & ANCHOR_WHERE_HCENTER_MASK);
		pWhere = &pAnchor->anchorHCenter;
		break;
	case ANCHOR_WHERE_VCENTER:
		ASSERT(CHECK_ANCHOR_WHERE || uWhere & ANCHOR_WHERE_VCENTER_MASK);
		pWhere = &pAnchor->anchorVCenter;
		break;
		
	}
	ASSERT(pWhere != NULL);
	pWhere->hAnchorWnd = hAnchorWnd;
	pWhere->uAnchorWhere = uAnchorWhere;
	pWhere->iOffset = iOffset;
	return TRUE;
}

BOOL CWndAnchor::SetAnchor(CWnd & cWnd, UINT uWhere, CWnd & cAnchorWnd, UINT uAnchorWhere, int iOffset)
{
	return SetAnchor(cWnd.GetSafeHwnd(), uWhere, cAnchorWnd.GetSafeHwnd(), uAnchorWhere, iOffset);
}

BOOL CWndAnchor::SetAnchor(CWnd * pWnd, UINT uWhere, CWnd * pAnchorWnd, UINT uAnchorWhere, int iOffset)
{
	return SetAnchor(pWnd->GetSafeHwnd(), uWhere, pAnchorWnd->GetSafeHwnd(), uAnchorWhere, iOffset);
}

BOOL CWndAnchor::SetAnchorToSide(CWnd & cWnd, CWnd & cAnchorWnd, UINT uAnchorWhere, int iOffset, BOOL bInside)
{
	UINT nWhere = 0;
	if (bInside)
	{
		nWhere = uAnchorWhere;

		switch(uAnchorWhere)
		{
		case ANCHOR_WHERE_TOP:
		case ANCHOR_WHERE_LEFT:	
		case ANCHOR_WHERE_HCENTER:
		case ANCHOR_WHERE_VCENTER:
			break;
		case ANCHOR_WHERE_RIGHT:
		case ANCHOR_WHERE_BOTTOM:
			iOffset = -iOffset;
			break;
		default:
			ASSERT(FALSE);
			return FALSE;
			break;
		}
	}else{
		switch(uAnchorWhere)
		{
		case ANCHOR_WHERE_LEFT:
			nWhere = ANCHOR_WHERE_RIGHT;
			iOffset = -iOffset;
			break;
		case ANCHOR_WHERE_RIGHT:
			nWhere = ANCHOR_WHERE_LEFT;
			break;
		case ANCHOR_WHERE_TOP:
			nWhere = ANCHOR_WHERE_BOTTOM;
			iOffset = -iOffset;
			break;
		case ANCHOR_WHERE_BOTTOM:
			nWhere = ANCHOR_WHERE_TOP;
			break;
		case ANCHOR_WHERE_HCENTER:
			nWhere = ANCHOR_WHERE_HCENTER;
			break;
		case ANCHOR_WHERE_VCENTER:
			nWhere = ANCHOR_WHERE_VCENTER;
			break;
		default:
			ASSERT(FALSE);
			return FALSE;
			break;
		}
	}
	
	return SetAnchor(cWnd, nWhere, cAnchorWnd, uAnchorWhere, iOffset);
}

BOOL CWndAnchor::RemoveAnchor(HWND hWnd, UINT uWhere)
{
	LPANCHORWNDDATA pAnchor = FindAnchorWnd(hWnd);
	if (pAnchor != NULL)
	{
		LPANCHORWHERE pWhere = NULL;
		switch (uWhere)
		{
		case ANCHOR_WHERE_LEFT:
			pWhere = &pAnchor->anchorLeft;
			break;
		case ANCHOR_WHERE_RIGHT:
			pWhere = &pAnchor->anchorRight;
			break;
		case ANCHOR_WHERE_TOP:
			pWhere = &pAnchor->anchorTop;
			break;
		case ANCHOR_WHERE_BOTTOM:
			pWhere = &pAnchor->anchorBottom;
			break;
		case ANCHOR_WHERE_HCENTER:
			pWhere = &pAnchor->anchorHCenter;
			break;
		case ANCHOR_WHERE_VCENTER:
			pWhere = &pAnchor->anchorVCenter;
			break;
		}
		ASSERT(pWhere != NULL);
		pWhere->hAnchorWnd = NULL;
		pWhere->uAnchorWhere = 0;
		pWhere->iOffset = 0;
		return TRUE;
	}
	return FALSE;
}

BOOL CWndAnchor::Relayout()
{
	for (ANCHORLIST::iterator iter = m_listAnchor.begin();
		iter != m_listAnchor.end();
		iter ++)
	{
		OnAnchor(*iter);
	}
	return TRUE;
}

BOOL CWndAnchor::Relayout(HWND hWnd)
{
	for (ANCHORLIST::iterator iter = m_listAnchor.begin();
		iter != m_listAnchor.end();
		iter ++)
	{
		if ((*iter)->hWnd == hWnd)
		{
			OnAnchor(*iter);
		}
	}
	return TRUE;
}

void CWndAnchor::Clear()
{
	for (ANCHORLIST::iterator iter = m_listAnchor.begin();
		iter != m_listAnchor.end();
		iter ++)
	{
		if (*iter != NULL)
		{
			delete *iter;
			*iter = NULL;
		}
	}
	m_listAnchor.clear();
}

CWndAnchor::LPANCHORWNDDATA CWndAnchor::FindAnchorWnd( HWND hWnd )
{
	for (ANCHORLIST::iterator iter = m_listAnchor.begin();
		iter != m_listAnchor.end();
		iter ++)
	{
		if (*iter != NULL && (*iter)->hWnd == hWnd)
		{
			return *iter;
		}
	}
	return NULL;
}


int CWndAnchor::tagAnchorWhere::GetAnchorRect(HWND hWnd, CRect & rect)
{
	HWND hParent = GetParent(hWnd);
	if (hParent != hAnchorWnd)
	{
		GetWindowRect(hAnchorWnd, rect);
		CWnd::FromHandle(hParent)->ScreenToClient(rect);
	}else{
		GetClientRect(hAnchorWnd, rect);
	}
	return 0;
}

int CWndAnchor::tagAnchorWhere::GetAnchorPos(HWND hWnd)
{
	CRect rt;
	GetAnchorRect(hWnd, rt);

	int iPos = 0;
	switch(uAnchorWhere)
	{
	case ANCHOR_WHERE_LEFT:
		iPos = rt.left;
		break;
	case ANCHOR_WHERE_RIGHT:
		iPos = rt.right;
		break;
	case ANCHOR_WHERE_TOP:
		iPos = rt.top;
		break;
	case ANCHOR_WHERE_BOTTOM:
		iPos = rt.bottom;
		break;
	case ANCHOR_WHERE_HCENTER:
		iPos = rt.left + rt.Width()/2;
		break;
	case ANCHOR_WHERE_VCENTER:
		iPos = rt.top + rt.Height()/2;
		break;
	default:
		ASSERT(FALSE);
		break;
	}
	return iPos + iOffset;
}

BOOL CWndAnchor::OnAnchor( LPANCHORWNDDATA pAnchor )
{
	BOOL bChangeX = FALSE;
	BOOL bChangeY = FALSE;
	CRect rt, rtOld, rtAnchor;
	pAnchor->GetWindowRect(rtOld);

	if (pAnchor->anchorHCenter.hAnchorWnd != NULL)
	{
		bChangeX = TRUE;
		rt.left = pAnchor->anchorHCenter.GetAnchorPos(pAnchor->hWnd) - rtOld.Width()/2;
		rt.right = rt.left + rtOld.Width();

	}else{
		if (pAnchor->anchorLeft.hAnchorWnd != NULL)
		{
			bChangeX = TRUE;
			rt.left = pAnchor->anchorLeft.GetAnchorPos(pAnchor->hWnd);
			if (pAnchor->anchorRight.hAnchorWnd == NULL)
			{
				rt.right = rt.left + rtOld.Width();
			}
		}
		if (pAnchor->anchorRight.hAnchorWnd != NULL)
		{
			bChangeX = TRUE;
			rt.right = pAnchor->anchorRight.GetAnchorPos(pAnchor->hWnd);
			if (pAnchor->anchorLeft.hAnchorWnd == NULL)
			{
				rt.left = rt.right - rtOld.Width();
			}
		}
	}
	
	if (pAnchor->anchorVCenter.hAnchorWnd != NULL)
	{
		bChangeY = TRUE;
		rt.top = pAnchor->anchorVCenter.GetAnchorPos(pAnchor->hWnd) - rtOld.Height()/2;
		rt.bottom = rt.top + rtOld.Height();
	}else{
		if (pAnchor->anchorTop.hAnchorWnd != NULL)
		{
			bChangeY = TRUE;
			rt.top = pAnchor->anchorTop.GetAnchorPos(pAnchor->hWnd);
			if (pAnchor->anchorBottom.hAnchorWnd == NULL)
			{
				rt.bottom = rt.top + rtOld.Height();
			}
		}
		if (pAnchor->anchorBottom.hAnchorWnd != NULL)
		{
			bChangeY = TRUE;
			rt.bottom = pAnchor->anchorBottom.GetAnchorPos(pAnchor->hWnd);
			if (pAnchor->anchorTop.hAnchorWnd == NULL)
			{
				rt.top = rt.bottom - rtOld.Height();
			}
		}
	}
	
	if (bChangeX || bChangeY)
	{
		if (!bChangeX)
		{
			rt.left = rtOld.left;
			rt.right= rtOld.right;
		}
		if(!bChangeY)
		{
			rt.bottom = rtOld.bottom;
			rt.top = rtOld.top;
		}
		//SetRect(rt);
		SetWindowPos(pAnchor->hWnd, NULL, rt.left, rt.top, rt.Width(), rt.Height(), SWP_NOZORDER);
		return TRUE;
	}
	return FALSE;
}

BOOL CWndAnchor::SetAnchorToAlign(CWnd & cWnd, CWnd & cAnchorWnd, UINT uAnchorWhere, int iOffset)
{
	return SetAnchor(cWnd, uAnchorWhere, cAnchorWnd, uAnchorWhere, iOffset);
}

int CWndAnchor::tagAnchorWndData::GetWindowRect(CRect & rect)
{
	::GetWindowRect(hWnd, rect);
	HWND hParent = GetParent(hWnd);
	if (hParent != NULL)
	{
		CWnd::FromHandle(hParent)->ScreenToClient(rect);
	}
	return 0;
}
