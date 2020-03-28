#ifndef EXPORTPDF_H
#define EXPORTPDF_H

enum ScoresheetType
{
	ST_POPE_AND_YOUNG = 0,
	ST_BOONE_AND_CROCKETT = 1,
	ST_SAFARI_WHITETAIL = 2, 
	ST_SAFARI_MULE = 3
};

void ExportToPDF(const wchar_t* fName, ULONG type = ST_POPE_AND_YOUNG);
void UpdateScore(ULONG type = ST_POPE_AND_YOUNG);
void PaintScore(HDC hdc);
void FreeScore();

#endif
