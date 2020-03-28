// ScoreSheet.cpp : implementation file
//

#include "stdafx.h"
#include "ViewControl.h"
#include "ScoreSheet.h"

#include "Kernel\ExportPDF.h"


// CScoreSheet

IMPLEMENT_DYNAMIC(CScoreSheet, CWnd)

CScoreSheet::CScoreSheet()
{
}

CScoreSheet::~CScoreSheet()
{
}

void CScoreSheet::OnPaint()
{
	PAINTSTRUCT ps;
	CDC* pDC = BeginPaint(&ps);


	if(pDC && pDC->GetSafeHdc())
		PaintScore(pDC->GetSafeHdc());

	EndPaint(&ps);
}

void CScoreSheet::OnDestroy()
{
	FreeScore();
}

void CScoreSheet::updateScore(ULONG type)
{
	UpdateScore(type);
}


BEGIN_MESSAGE_MAP(CScoreSheet, CWnd)
	ON_WM_PAINT()
	ON_WM_DESTROY()
END_MESSAGE_MAP()



// CScoreSheet message handlers


