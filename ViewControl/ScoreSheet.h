#pragma once


// CScoreSheet

class CScoreSheet : public CWnd
{
	DECLARE_DYNAMIC(CScoreSheet)

public:
	CScoreSheet();
	virtual ~CScoreSheet();

	void updateScore(ULONG type);

protected:
	afx_msg void OnPaint();
	afx_msg void OnDestroy();

	DECLARE_MESSAGE_MAP()

};


