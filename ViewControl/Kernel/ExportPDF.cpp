
#include "stdafx.h"
#include "ExportPDF.h"
#include "Kernel.h"
#include "..\ViewCtrl.h"
#include "..\CenterLine.h"

#include "cairo/cairo-win32.h"
#include "cairo/cairo-pdf.h"

using namespace std;

#define PAGE_WIDTH		(8.5 * 72)
#define PAGE_HEIGHT		(11 * 72)

#define LEFT_BORDER		(0.5 * 72)
#define RIGHT_BORDER	(0.5 * 72)
#define TOP_BORDER		(0.5 * 72)
#define BOTTOM_BORDER	(0.5 * 72)

#define WORKING_AREA_WIDTH	(PAGE_WIDTH - RIGHT_BORDER - LEFT_BORDER)
#define WORKING_AREA_HEIGHT	(PAGE_HEIGHT - TOP_BORDER - BOTTOM_BORDER)

enum TextStyle
{
	TS_NONE_ = 0,
	TS_BOLD = 1,
	TS_ITALIC = 2,
	TS_CENTERED_X = 4,
	TS_CENTERED_Y = 8,
	TS_CENTERED = TS_CENTERED_X | TS_CENTERED_Y
};

enum CellStyle
{
	CS_LEFT = 1,
	CS_RIGHT = 2,
	CS_VERTICAL = CS_LEFT | CS_RIGHT,

	CS_TOP = 4,
	CS_BOTTOM = 8,
	CS_HORIZONTAL = CS_TOP | CS_BOTTOM,
	
	CS_ALL = 15,

	CS_LEFT_BOLD = 16,
	CS_RIGHT_BOLD = 32,
	CS_VERTICAL_BOLD = CS_LEFT_BOLD | CS_RIGHT_BOLD,

	CS_TOP_BOLD = 64,
	CS_BOTTOM_BOLD = 128,
	CS_HORIZONTAL_BOLD = CS_TOP_BOLD | CS_BOTTOM_BOLD,

	CS_ALL_BOLD = 240
};

inline double writeImage(cairo_t *cr, string name, double x, double y, double width = 0, double height = 0)
{
	// save settings
	cairo_save (cr);

	cairo_surface_t *image;

	image = cairo_image_surface_create_from_png (name.c_str());
	cairo_status_t stat = cairo_surface_status (image);
	if(stat != CAIRO_STATUS_SUCCESS)
	{
		// report error 

		cairo_restore (cr);
		return 0;
	}

	double invw = 1.0 / cairo_image_surface_get_width (image);
	double h = cairo_image_surface_get_height (image);

	if(width == 0)
		width = WORKING_AREA_WIDTH / 72 - x * 2;
	if(height == 0)
		height = width;

	width = width * invw;
	height = height * invw;

	cairo_translate (cr, LEFT_BORDER + x * 72, TOP_BORDER + y * 72);
	cairo_scale (cr, width * 72, height * 72);

	cairo_set_source_surface (cr, image, 0, 0);
	cairo_paint (cr);

	cairo_surface_destroy (image);

	// restore settings
	cairo_restore (cr);
	return h * height;
}

void drawTextLine(cairo_t *cr, double x, double y, string str, double size = 12, int style = TS_CENTERED_Y)
{
	cairo_text_extents_t extents;

	/*cairo_font_slant_t   slant;
	cairo_font_weight_t  weight;
	if(style & TS_BOLD)
		weight = CAIRO_FONT_WEIGHT_BOLD;
	else
		weight = CAIRO_FONT_WEIGHT_NORMAL;

	if(style & TS_ITALIC)
		slant = CAIRO_FONT_SLANT_ITALIC;
	else
		slant = CAIRO_FONT_SLANT_NORMAL;*/

	LOGFONTW fnt;
	/*fnt.lfHeight
	fnt.lfWidth
	fnt.lfOrientation
	fnt.lfEscapement*/

	DWORD dwWeight = (style & TS_BOLD) ? FW_BOLD : FW_REGULAR;

	fnt.lfCharSet = DEFAULT_CHARSET;
	fnt.lfClipPrecision = OUT_DEFAULT_PRECIS;
	fnt.lfItalic = (style & TS_ITALIC) ? 1 : 0;
	fnt.lfOutPrecision = OUT_DEFAULT_PRECIS;
	fnt.lfPitchAndFamily = VARIABLE_PITCH;
	fnt.lfQuality = ANTIALIASED_QUALITY;
	fnt.lfStrikeOut = 0;
	fnt.lfUnderline = 0;
	fnt.lfWeight = dwWeight;

	size -= 2;

	wchar_t* fontNames[3] = { L"Arial", L"Tahoma", L"SansSerif" };
	cairo_font_face_t* cfont = NULL;

	for(int i = 0; i < 3; i++)
	{
		wcscpy_s(fnt.lfFaceName, L"Tahoma");
		cfont = cairo_win32_font_face_create_for_logfontw(&fnt);
		if(cfont)
			break;
	}

	if(!cfont)
		MessageBox(0, L"Error loading font", L"Antler",0);

	cairo_set_font_face(cr, cfont);
	//cairo_select_font_face (cr, "lucida", slant, weight);

	cairo_set_font_size (cr, size);
	cairo_text_extents (cr, str.c_str(), &extents);

	if(style & TS_CENTERED_X)
		x -= (extents.width/2 + extents.x_bearing) / 72;
	if(style & TS_CENTERED_Y)
		y -= (extents.height/2 + extents.y_bearing) / 72;

	cairo_move_to (cr, LEFT_BORDER + x * 72, TOP_BORDER + y * 72);
	cairo_show_text (cr, str.c_str());

	cairo_font_face_destroy(cfont);
}

void drawCell(cairo_t *cr, double x, double y, double width, double height, int style = CS_ALL_BOLD)
{
	x = LEFT_BORDER + x * 72;
	y = LEFT_BORDER + y * 72;

	cairo_set_line_cap (cr, CAIRO_LINE_CAP_ROUND);

	// bold
	cairo_set_line_width (cr, 1.0);
	if(style & CS_LEFT_BOLD)
	{
		cairo_move_to(cr, x, y);
		cairo_rel_line_to(cr, 0, height * 72);
	}
	if(style & CS_RIGHT_BOLD)
	{
		cairo_move_to(cr, x + width * 72, y);
		cairo_rel_line_to(cr, 0, height * 72);
	}
	if(style & CS_TOP_BOLD)
	{
		cairo_move_to(cr, x, y);
		cairo_rel_line_to(cr, width * 72, 0);
	}
	if(style & CS_BOTTOM_BOLD)
	{
		cairo_move_to(cr, x, y + height * 72);
		cairo_rel_line_to(cr, width * 72, 0);
	}
	cairo_stroke (cr);
	
	// thin
	cairo_set_line_width (cr, 0.5);
	if(style & CS_LEFT)
	{
		cairo_move_to(cr, x, y);
		cairo_rel_line_to(cr, 0, height * 72);
	}
	if(style & CS_RIGHT)
	{
		cairo_move_to(cr, x + width * 72, y);
		cairo_rel_line_to(cr, 0, height * 72);
	}
	if(style & CS_TOP)
	{
		cairo_move_to(cr, x, y);
		cairo_rel_line_to(cr, width * 72, 0);
	}
	if(style & CS_BOTTOM)
	{
		cairo_move_to(cr, x, y + height * 72);
		cairo_rel_line_to(cr, width * 72, 0);
	}
	cairo_stroke (cr);
}

void drawTable_PY(cairo_t *cr, double x, double y)
{
	CenterLine* cline = GetGlobalView()->getCenterLine();
	int beamsCount;
	if(cline)
	{
		beamsCount = max(cline->m_sortedTips[0].size(), cline->m_sortedTips[1].size());
		if(beamsCount > 15)
			beamsCount = 15;
	}
	else
		beamsCount = 3;

	string col1[15] = { "Length of Main Beam",
						"Length of First Point (if present)",
                        "Length of Second Point",
                        "Length of Third Point",
                        "Length of Fourth Point",
                        "Length of Fifth Point",
                        "Length of Sixth Point",
                        "Length of Seventh Point",
                        "Length of Eighth Point",
                        "Length of Ninth Point",
                        "Length of Tenth Point",
                        "Length of Eleventh Point",
                        "Length of Twelfth Point",
                        "Length of 13th Point",
						"Length of 14th Point" };


	string col2[15] = {	"F", "G1", "G2", "G3", "G4",  
						"G5", "G6", "G7", "G8", "G9",
						"G10", "G11", "G12", "G13", "G14" };


	string circ[4] = {	"Circumference at Smallest Place Between Burr and First Point",
						"Circumference at Smallest Place Between First and Second Points",
						"Circumference at Smallest Place Between Second and Third Points",
                        "Circumference at Smallest Place Between Third and Fourth Pointst" };

	string circ2[4] = {	"H1", "H2",	"H3", "H4"};

	double dx = x;
	double dy = y;
	double wid = 3.5;
	double height = 0.22;
	double total[3] = {0};
	double score = 0;
	
	// header
	drawCell(cr, dx, dy, wid, height);
	drawTextLine(cr, dx + wid * 0.5, dy + height * 0.5, "Measurement", 12, TS_BOLD | TS_CENTERED);
	dx += wid;
	wid = 1.0;

	drawCell(cr, dx, dy, wid, height, CS_HORIZONTAL_BOLD | CS_RIGHT_BOLD);
	drawTextLine(cr, dx + wid * 0.5, dy + height * 0.5, "Designation", 12, TS_BOLD | TS_CENTERED);
	dx += wid;

	drawCell(cr, dx, dy, wid, height, CS_HORIZONTAL_BOLD | CS_RIGHT_BOLD);
	drawTextLine(cr, dx + wid * 0.5, dy + height * 0.5, "Shared", 12, TS_BOLD | TS_CENTERED);
	dx += wid;

	drawCell(cr, dx, dy, wid, height, CS_HORIZONTAL_BOLD | CS_RIGHT_BOLD);
	drawTextLine(cr, dx + wid * 0.5, dy + height * 0.5, "Right Side", 12, TS_BOLD | TS_CENTERED);
	dx += wid;

	drawCell(cr, dx, dy, wid, height, CS_HORIZONTAL_BOLD | CS_RIGHT_BOLD);
	drawTextLine(cr, dx + wid * 0.5, dy + height * 0.5, "Left Side", 12, TS_BOLD | TS_CENTERED);
	dx += wid;

	// table
	dx = x;
	dy += height + height * 0.5;
	double firstLn = dy;
	height = 0.2;
	wid = 3.5;

	drawCell(cr, dx, dy, wid, height, CS_LEFT_BOLD | CS_RIGHT | CS_BOTTOM | CS_TOP_BOLD);
	drawTextLine(cr, dx + 0.05, dy + height * 0.5, "Widest Inside Spread", 10, TS_CENTERED_Y);
	dy += height;

	// beams
	for(int i = 0; i < beamsCount; i++)
	{
		drawCell(cr, dx, dy, wid, height, CS_LEFT_BOLD | CS_RIGHT | CS_BOTTOM);
		drawTextLine(cr, dx + 0.05, dy + height * 0.5, col1[i], 10, TS_CENTERED_Y);
		dy += height;
	}

	// circumf...
	for(int i = 0; i < 4; i++)
	{
		drawCell(cr, dx, dy, wid, height, CS_LEFT_BOLD | CS_RIGHT | CS_BOTTOM);
		drawTextLine(cr, dx + 0.05, dy + height * 0.5, circ[i], 10, TS_CENTERED_Y);
		dy += height;
	}
	// end of beams

	drawCell(cr, dx, dy, wid, height, CS_LEFT_BOLD | CS_RIGHT | CS_BOTTOM_BOLD);
	drawTextLine(cr, dx + 0.05, dy + height * 0.5, "Branch Volume (not indicated)", 10, TS_CENTERED_Y);
	dy += height;

	// col #2
	dy = firstLn;
	dx += wid;
	wid = 1.0;

	drawCell(cr, dx, dy, wid, height, CS_RIGHT_BOLD | CS_BOTTOM | CS_TOP_BOLD);
	drawTextLine(cr, dx + wid * 0.5, dy + height * 0.5, "D", 10, TS_CENTERED);
	dy += height;

	// beams
	for(int i = 0; i < beamsCount; i++)
	{
		drawCell(cr, dx, dy, wid, height, CS_RIGHT | CS_BOTTOM);
		drawTextLine(cr, dx + wid * 0.5, dy + height * 0.5, col2[i], 10, TS_CENTERED);
		dy += height;
	}

	// circumf...
	for(int i = 0; i < 4; i++)
	{
		drawCell(cr, dx, dy, wid, height, CS_RIGHT | CS_BOTTOM);
		drawTextLine(cr, dx + wid * 0.5, dy + height * 0.5, circ2[i], 10, TS_CENTERED);
		dy += height;
	}
	// end of beams

	drawCell(cr, dx, dy, wid, height, CS_RIGHT | CS_BOTTOM_BOLD);
	drawTextLine(cr, dx + wid * 0.5, dy + height * 0.5, "V", 10, TS_CENTERED);
	dy += height;

	// tot
	dy += height;
	drawCell(cr, dx, dy, wid, height, CS_ALL);
	drawTextLine(cr, dx + wid * 0.5, dy + height * 0.5, "Column Total", 10, TS_CENTERED | TS_BOLD);

	// tot
	dy += height;
	dy += height;
	wid = 1.0*3;
	drawCell(cr, dx, dy, wid, height, CS_ALL);
	drawTextLine(cr, dx + wid * 0.5, dy + height * 0.5, "Final Score = Column 1 + Column 2 + Column 3 =", 10, TS_CENTERED | TS_BOLD);
	wid = 1.0;

	// col #3
	dy = firstLn;
	dx += wid;

	drawCell(cr, dx, dy, wid, height, CS_RIGHT_BOLD | CS_BOTTOM_BOLD | CS_TOP_BOLD);
	if(cline && cline->m_dims.size() > 0)
	{
		std::ostringstream str;
		str.setf(ios::fixed);
		str.precision(2);
		str<<cline->m_dims[0].m_distance;
		total[0] += cline->m_dims[0].m_distance;

		drawTextLine(cr, dx + wid * 0.5, dy + height * 0.5, str.str(), 10, TS_CENTERED);
	}
	dy += height;

	// beams
	for(int i = 0; i < beamsCount; i++)
	{
		drawCell(cr, dx, dy, wid, height, CS_RIGHT_BOLD | CS_BOTTOM);
		dy += height;
	}
	// circumf...
	for(int i = 0; i < 4; i++)
	{
		drawCell(cr, dx, dy, wid, height, CS_RIGHT_BOLD | CS_BOTTOM);
		//drawTextLine(cr, dx + 0.05, dy + height * 0.5, circ2[i], 10, TS_CENTERED_Y);
		dy += height;
	}
	// end of beams

	drawCell(cr, dx, dy, wid, height, CS_RIGHT_BOLD | CS_BOTTOM_BOLD);
	//drawTextLine(cr, dx + wid * 0.5, dy + height * 0.5, "V1", 10, TS_CENTERED);
	dy += height;

	// tot
	dy += height;
	drawCell(cr, dx, dy, wid, height, CS_HORIZONTAL | CS_RIGHT);
	if(total[0] > 0)
	{
		std::ostringstream str;
		str.setf(ios::fixed);
		str.precision(2);
		str<<total[0];
		drawTextLine(cr, dx + wid * 0.5, dy + height * 0.5, str.str(), 10, TS_CENTERED);
	}

	// col #4
	dy = firstLn;
	dx += wid;

	//drawCell(cr, dx, dy, wid, height, CS_RIGHT_BOLD | CS_BOTTOM | CS_TOP_BOLD);
	//drawTextLine(cr, dx + wid * 0.5, dy + height * 0.5, "D", 10, TS_CENTERED);
	dy += height;

	// beams
    for(int i = 0; i < beamsCount; ++i)
    {
        if(i != 0)
            drawCell(cr, dx, dy, wid, height, CS_RIGHT | CS_BOTTOM);
        else
            drawCell(cr, dx, dy, wid, height, CS_TOP_BOLD | CS_RIGHT | CS_BOTTOM);

        if(cline && GetGlobalView()->getTipsMap(1).find(col2[i]) != GetGlobalView()->getTipsMap(1).end())
        {
            std::ostringstream str;
            str.setf(ios::fixed);
            str.precision(2);

            double length = GetGlobalView()->getTipsMap(1)[col2[i]]->m_length;
           
            str << length;
            total[1] += length;

            drawTextLine(cr, dx + wid * 0.5, dy + height * 0.5, str.str(), 10, TS_CENTERED);
        }
        dy += height;
    }
	
	// circumf...
	for(int i = 0; i < 4; i++)
	{
		drawCell(cr, dx, dy, wid, height, CS_RIGHT | CS_BOTTOM);
		//drawTextLine(cr, dx + 0.05, dy + height * 0.5, circ2[i], 10, TS_CENTERED_Y);
		dy += height;
	}
	// end of beams

	drawCell(cr, dx, dy, wid, height, CS_RIGHT | CS_BOTTOM_BOLD);
	if(GetGlobalView()->isBurPlaneInialized(1)) 
	{
		std::ostringstream str;
		str.setf(ios::fixed);
		str.precision(2);
		str<<GetGlobalView()->getBranchVolume(1);
		total[1] += GetGlobalView()->getBranchVolume(1);

		drawTextLine(cr, dx + wid * 0.5, dy + height * 0.5, str.str(), 10, TS_CENTERED);
	}
	dy += height;

	// tot
	dy += height;
	drawCell(cr, dx, dy, wid, height, CS_HORIZONTAL | CS_RIGHT);
	if(total[1] > 0)
	{
		std::ostringstream str;
		str.setf(ios::fixed);
		str.precision(2);
		str<<total[1];
		drawTextLine(cr, dx + wid * 0.5, dy + height * 0.5, str.str(), 10, TS_CENTERED);
	}

	// col #5
	dy = firstLn;
	dx += wid;

	//drawCell(cr, dx, dy, wid, height, CS_RIGHT_BOLD | CS_BOTTOM | CS_TOP_BOLD);
	//drawTextLine(cr, dx + wid * 0.5, dy + height * 0.5, "D", 10, TS_CENTERED);
	dy += height;

	// beams
    for(int i = 0; i < beamsCount; ++i)
    {
        if(i != 0)
            drawCell(cr, dx, dy, wid, height, CS_RIGHT_BOLD | CS_BOTTOM);
        else
            drawCell(cr, dx, dy, wid, height, CS_TOP_BOLD | CS_RIGHT_BOLD | CS_BOTTOM);

        if(cline && GetGlobalView()->getTipsMap(0).find(col2[i]) != GetGlobalView()->getTipsMap(0).end())
        {
            std::ostringstream str;
            str.setf(ios::fixed);
            str.precision(2);

            double length = GetGlobalView()->getTipsMap(0)[col2[i]]->m_length;
            str << length;
            total[2] += length;

            drawTextLine(cr, dx + wid * 0.5, dy + height * 0.5, str.str(), 10, TS_CENTERED);
        }
        dy += height;
    }

	// circumf...
	for(int i = 0; i < 4; i++)
	{
		drawCell(cr, dx, dy, wid, height, CS_RIGHT_BOLD | CS_BOTTOM);
		//drawTextLine(cr, dx + 0.05, dy + height * 0.5, circ2[i], 10, TS_CENTERED_Y);
		dy += height;
	}
	// end of beams

	drawCell(cr, dx, dy, wid, height, CS_RIGHT_BOLD | CS_BOTTOM_BOLD);
	if(GetGlobalView()->isBurPlaneInialized(0)) 
	{
		std::ostringstream str;
		str.setf(ios::fixed);
		str.precision(2);
		str<<GetGlobalView()->getBranchVolume(0);
		total[2] += GetGlobalView()->getBranchVolume(0);

		drawTextLine(cr, dx + wid * 0.5, dy + height * 0.5, str.str(), 10, TS_CENTERED);
	}
	dy += height;

	// tot
	dy += height;
	drawCell(cr, dx, dy, wid, height, CS_HORIZONTAL | CS_RIGHT);
	if(total[2] > 0)
	{
		std::ostringstream str;
		str.setf(ios::fixed);
		str.precision(2);
		str<<total[2];
		drawTextLine(cr, dx + wid * 0.5, dy + height * 0.5, str.str(), 10, TS_CENTERED);
	}

	score = total[0] + total[1] + total[2];
	// tot
	dy += height;
	dy += height;
	drawCell(cr, dx, dy, wid, height, CS_HORIZONTAL | CS_RIGHT);
	if(score > 0)
	{
		std::ostringstream str;
		str.setf(ios::fixed);
		str.precision(2);
		str<<score;
		drawTextLine(cr, dx + wid * 0.5, dy + height * 0.5, str.str(), 10, TS_CENTERED);
	}

}
// ------------------------------------------------------------------------------------------------------
void drawTable_BC(cairo_t *cr, double x, double y)
{
	CenterLine* cline = GetGlobalView()->getCenterLine();
	int beamsCount;
	if(cline)
	{
		beamsCount = max(cline->m_sortedTips[0].size(), cline->m_sortedTips[1].size()) + 1;
		if(beamsCount > 11)
			beamsCount = 11;
	}
	else
		beamsCount = 4;

	string col1[11] = {	"E. Total of Lengths of Abnormal Points",
						"F. Length of Main Beam",
						"G-1. Length of First Point",
                        "G-2. Length of Second Point",
                        "G-3. Length of Third Point",
                        "G-4. Length of Fourth Point, If Present",
                        "G-5. Length of Fifth Point, If Present",
                        "G-6. Length of Sixth Point, If Present",
                        "G-7. Length of Seventh Point, If Present",
                        "G-8. Length of Eighth Point, If Present",
                        "G-9. Length of Ninth Point, If Present"};

	string col2[11] = {	"E", "F", "G1", "G2", "G3", "G4",  
						"G5", "G6", "G7", "G8", "G9" };

	string circ[4] = {	"H-1. Circumference at Smallest Place Between Burr and First Point",
						"H-2. Circumference at Smallest Place Between First and Second Points",
						"H-3. Circumference at Smallest Place Between Second and Third Points",
                        "H-4. Circumference at Smallest Place Between Third and Fourth Pointst" };

	double dx = x;
	double dy = y;
	double wid = 4.1;
	double height = 0.20;
	double total[4] = {0};
	double eTotal[2] = {0};
	double score = 0;

	int iCount = max(GetGlobalView()->getETips(0).size(), GetGlobalView()->getETips(1).size());
	if (iCount < 5)
		iCount = 5;

	// table #1
	dx = x + 5.8;
	dy = y - 1.9 - (iCount - 5) * height;
	wid = 1.7;
	// header #1
	drawCell(cr, dx, dy, wid, height);
	drawTextLine(cr, dx + wid * 0.5, dy + height * 0.5, "Abnormal Points", 10, TS_BOLD | TS_CENTERED);

	// header #2
	dy += height;
	wid = 0.85;

	drawCell(cr, dx, dy, wid, height, CS_VERTICAL_BOLD | CS_BOTTOM_BOLD);
	drawTextLine(cr, dx + wid * 0.5, dy + height * 0.5, "Right Antler", 10, TS_BOLD | TS_CENTERED);
	dx += wid;

	drawCell(cr, dx, dy, wid, height, CS_RIGHT_BOLD | CS_BOTTOM_BOLD);
	drawTextLine(cr, dx + wid * 0.5, dy + height * 0.5, "Left Antler", 10, TS_BOLD | TS_CENTERED);
	dx += wid;

	// col #2
	dy += height;
	dx = x + 5.8;
	double firstLn = dy;

	for(int i = 0; i < iCount; i++)
	{
		if(i < iCount - 1)
			drawCell(cr, dx, dy, wid, height, CS_LEFT_BOLD | CS_BOTTOM | CS_RIGHT);
		else
			drawCell(cr, dx, dy, wid, height, CS_LEFT_BOLD | CS_BOTTOM_BOLD | CS_RIGHT);

		if(i < GetGlobalView()->getETips(1).size())
		{
			std::ostringstream str;
			str.setf(ios::fixed);
			str.precision(2);
			str<<GetGlobalView()->getETips(1)[i]->m_length;
			eTotal[1] += GetGlobalView()->getETips(1)[i]->m_length;

			drawTextLine(cr, dx + wid * 0.5, dy + height * 0.5, str.str(), 10, TS_CENTERED);
		}
		dy += height;
	}

	// col #3
	dy = firstLn;
	dx += wid;

	for(int i = 0; i < iCount; i++)
	{
		if(i < iCount - 1)
			drawCell(cr, dx, dy, wid, height, CS_BOTTOM | CS_RIGHT_BOLD);
		else
			drawCell(cr, dx, dy, wid, height, CS_BOTTOM_BOLD | CS_RIGHT_BOLD);

		if(i < GetGlobalView()->getETips(0).size())
		{
			std::ostringstream str;
			str.setf(ios::fixed);
			str.precision(2);
			str<<GetGlobalView()->getETips(0)[i]->m_length;
			eTotal[0] += GetGlobalView()->getETips(0)[i]->m_length;

			drawTextLine(cr, dx + wid * 0.5, dy + height * 0.5, str.str(), 10, TS_CENTERED);
		}
		//drawTextLine(cr, dx + wid * 0.5, dy + height * 0.5, " ", 10, TS_CENTERED);
		dy += height;
	}
	
	// row #1
	dx = x + 5.8 - 0.85;

	drawCell(cr, dx, dy, wid, height);
	drawTextLine(cr, dx + wid * 0.5, dy + height * 0.5, "SUBTOTALS", 10, TS_BOLD | TS_CENTERED);
	dx += wid;

	drawCell(cr, dx, dy, wid, height, CS_VERTICAL_BOLD | CS_BOTTOM_BOLD);
	if(eTotal[1] > 0)
	{
		std::ostringstream str;
		str.setf(ios::fixed);
		str.precision(2);
		str<<eTotal[1];

		drawTextLine(cr, dx + wid * 0.5, dy + height * 0.5, str.str(), 10, TS_BOLD | TS_CENTERED);
	}
	dx += wid;

	drawCell(cr, dx, dy, wid, height, CS_RIGHT_BOLD | CS_BOTTOM_BOLD);
	if(eTotal[0] > 0)
	{
		std::ostringstream str;
		str.setf(ios::fixed);
		str.precision(2);
		str<<eTotal[0];

		drawTextLine(cr, dx + wid * 0.5, dy + height * 0.5, str.str(), 10, TS_BOLD | TS_CENTERED);
	}
	dx += wid;

	// row #2
	dy += height;
	dx = x + 5.8 - 0.85;

	drawCell(cr, dx, dy, wid, height, CS_VERTICAL_BOLD | CS_BOTTOM_BOLD);
	drawTextLine(cr, dx + wid * 0.5, dy + height * 0.5, "TOTAL TO E", 10, TS_BOLD | TS_CENTERED);
	dx += wid;
	wid = 0.85 * 2;

	drawCell(cr, dx, dy, wid, height, CS_RIGHT_BOLD | CS_BOTTOM_BOLD);
	if(eTotal[0] + eTotal[1] > 0)
	{
		std::ostringstream str;
		str.setf(ios::fixed);
		str.precision(2);
		str<<eTotal[0] + eTotal[1];

		drawTextLine(cr, dx + wid * 0.5, dy + height * 0.5, str.str(), 10, TS_BOLD | TS_CENTERED);
	}
	dx += wid;

	// table #2
	dx = x;
	dy = y;
	wid = 4.1;
	// header #1
	drawCell(cr, dx, dy, wid, height);
	drawTextLine(cr, dx + wid * 0.5, dy + height * 0.5, "Measurement", 10, TS_BOLD | TS_CENTERED);
	dx += wid;
	wid = 0.85;

	drawCell(cr, dx, dy, wid, height, CS_HORIZONTAL_BOLD | CS_RIGHT_BOLD);
	drawTextLine(cr, dx + wid * 0.5, dy + height * 0.5, "COLUMN 1", 10, TS_BOLD | TS_CENTERED);
	dx += wid;

	drawCell(cr, dx, dy, wid, height, CS_HORIZONTAL_BOLD | CS_RIGHT_BOLD);
	drawTextLine(cr, dx + wid * 0.5, dy + height * 0.5, "COLUMN 2", 10, TS_BOLD | TS_CENTERED);
	dx += wid;

	drawCell(cr, dx, dy, wid, height, CS_HORIZONTAL_BOLD | CS_RIGHT_BOLD);
	drawTextLine(cr, dx + wid * 0.5, dy + height * 0.5, "COLUMN 3", 10, TS_BOLD | TS_CENTERED);
	dx += wid;

	drawCell(cr, dx, dy, wid, height, CS_HORIZONTAL_BOLD | CS_RIGHT_BOLD);
	drawTextLine(cr, dx + wid * 0.5, dy + height * 0.5, "COLUMN 4", 10, TS_BOLD | TS_CENTERED);
	dx += wid;

	// row #1
	dx = x;
	dy += height + height * 0.5;
	wid = 1.67;

	drawCell(cr, dx, dy, wid, height, CS_LEFT_BOLD | CS_TOP_BOLD | CS_RIGHT | CS_BOTTOM);
	drawTextLine(cr, dx + 0.05, dy + height * 0.5, "A. No. Points on Right Antler", 10, TS_CENTERED_Y);
	dx += wid;
	wid = 0.48;

	drawCell(cr, dx, dy, wid, height, CS_TOP_BOLD | CS_VERTICAL | CS_BOTTOM);
	if(cline)
	{
		std::ostringstream str;
		str.setf(ios::fixed);
		str.precision(2);
		str<<cline->m_sortedTips[1].size();

		drawTextLine(cr, dx + wid * 0.5, dy + height * 0.5, str.str(), 10, TS_CENTERED);
	}
	dx += wid;
	wid = 1.47;

	drawCell(cr, dx, dy, wid, height, CS_TOP_BOLD | CS_VERTICAL | CS_BOTTOM);
	drawTextLine(cr, dx + 0.05, dy + height * 0.5, "No. Points on Left Antler", 10, TS_CENTERED_Y);
	dx += wid;
	wid = 0.48;

	drawCell(cr, dx, dy, wid, height, CS_TOP_BOLD | CS_RIGHT_BOLD | CS_LEFT | CS_BOTTOM );
	if(cline)
	{
		std::ostringstream str;
		str.setf(ios::fixed);
		str.precision(2);
		str<<cline->m_sortedTips[0].size();

		drawTextLine(cr, dx + wid * 0.5, dy + height * 0.5, str.str(), 10, TS_CENTERED);
	}
	dx += wid;
	wid = 0.85;

	drawCell(cr, dx, dy, wid, height, CS_HORIZONTAL_BOLD | CS_RIGHT_BOLD);
	drawTextLine(cr, dx + wid * 0.5, dy + height * 0.5, "Spread Credit", 10, TS_BOLD | TS_CENTERED);
	dx += wid;

	drawCell(cr, dx, dy, wid, height, CS_HORIZONTAL_BOLD | CS_RIGHT_BOLD);
	drawTextLine(cr, dx + wid * 0.5, dy + height * 0.5, "Right Antler", 10, TS_BOLD | TS_CENTERED);
	dx += wid;

	drawCell(cr, dx, dy, wid, height, CS_HORIZONTAL_BOLD | CS_RIGHT_BOLD);
	drawTextLine(cr, dx + wid * 0.5, dy + height * 0.5, "Left Antler", 10, TS_BOLD | TS_CENTERED);
	dx += wid;

	drawCell(cr, dx, dy, wid, height, CS_HORIZONTAL_BOLD | CS_RIGHT_BOLD);
	drawTextLine(cr, dx + wid * 0.5, dy + height * 0.5, "Difference", 10, TS_BOLD | TS_CENTERED);
	dx += wid;

	// row #2
	dx = x;
	dy += height;
	height = 0.2;
	wid = 1.67;

	drawCell(cr, dx, dy, wid, height, CS_LEFT_BOLD | CS_RIGHT | CS_BOTTOM);
	drawTextLine(cr, dx + 0.05, dy + height * 0.5, "B. Tip to Tip Spread", 10, TS_CENTERED_Y);
	dx += wid;
	wid = 0.48;

	drawCell(cr, dx, dy, wid, height, CS_RIGHT | CS_BOTTOM);
	if(cline && cline->m_dims.size() > 2)
	{
		std::ostringstream str;
		str.setf(ios::fixed);
		str.precision(2);
		str<<cline->m_dims[2].m_distance;

		drawTextLine(cr, dx + wid * 0.5, dy + height * 0.5, str.str(), 10, TS_CENTERED);
	}
	dx += wid;
	wid = 1.47;

	drawCell(cr, dx, dy, wid, height, CS_RIGHT | CS_BOTTOM);
	drawTextLine(cr, dx + 0.05, dy + height * 0.5, "C. Greatest Spread", 10, TS_CENTERED_Y);
	dx += wid;
	wid = 0.48;

	drawCell(cr, dx, dy, wid, height, CS_RIGHT | CS_BOTTOM);
	if(cline && cline->m_dims.size() > 1)
	{
		std::ostringstream str;
		str.setf(ios::fixed);
		str.precision(2);
		str<<cline->m_dims[1].m_distance;

		drawTextLine(cr, dx + wid * 0.5, dy + height * 0.5, str.str(), 10, TS_CENTERED);
	}
	dx += wid;
	wid = 0.85;

	drawCell(cr, dx, dy, wid, height, CS_RIGHT | CS_BOTTOM);
	//drawTextLine(cr, dx + wid * 0.5, dy + height * 0.5, "Val", 10, TS_CENTERED);
	dx += wid;

	drawCell(cr, dx, dy, wid, height, CS_RIGHT | CS_BOTTOM);
	//drawTextLine(cr, dx + wid * 0.5, dy + height * 0.5, "Val", 10, TS_CENTERED);
	dx += wid;

	drawCell(cr, dx, dy, wid, height, CS_RIGHT | CS_BOTTOM);
	//drawTextLine(cr, dx + wid * 0.5, dy + height * 0.5, "Val", 10, TS_CENTERED);
	dx += wid;

	drawCell(cr, dx, dy, wid, height, CS_RIGHT_BOLD | CS_BOTTOM);
	//drawTextLine(cr, dx + wid * 0.5, dy + height * 0.5, "Val", 10, TS_CENTERED);
	dx += wid;

	// table
	dx = x;
	dy += height;
	firstLn = dy;
	height = 0.2;
	wid = 4.1;

	drawCell(cr, dx, dy, wid, height, CS_LEFT_BOLD | CS_RIGHT | CS_BOTTOM);
	drawTextLine(cr, dx + 0.05, dy + height * 0.5, "D. Inside Spread of Main Beams", 10, TS_CENTERED_Y);
	dy += height;

	// beams
	for(int i = 0; i < beamsCount; i++)
	{
		drawCell(cr, dx, dy, wid, height, CS_LEFT_BOLD | CS_RIGHT | CS_BOTTOM);
		drawTextLine(cr, dx + 0.05, dy + height * 0.5, col1[i], 10, TS_CENTERED_Y);
		dy += height;
	}

	// circumf..
	for(int i = 0; i < 4; i++)
	{
		drawCell(cr, dx, dy, wid, height, CS_LEFT_BOLD | CS_RIGHT | CS_BOTTOM);
		drawTextLine(cr, dx + 0.05, dy + height * 0.5, circ[i], 10, TS_CENTERED_Y);
		dy += height;
	}
	// end of beams

	drawCell(cr, dx, dy, wid, height, CS_LEFT_BOLD | CS_RIGHT | CS_BOTTOM_BOLD);
	drawTextLine(cr, dx + 0.05, dy + height * 0.5, "TOTALS", 10, TS_BOLD + TS_CENTERED_Y);
	dy += height;

	// col #2
	dy = firstLn;
	dx += wid;
	wid = 0.85;

	drawCell(cr, dx, dy, wid, height, CS_RIGHT | CS_BOTTOM);
	if(cline && cline->m_dims.size() > 0)
	{
		std::ostringstream str;
		str.setf(ios::fixed);
		str.precision(2);
		str<<cline->m_dims[0].m_distance;
		total[0] += cline->m_dims[0].m_distance;

		drawTextLine(cr, dx + wid * 0.5, dy + height * 0.5, str.str(), 10, TS_CENTERED);
	}
	dy += height;

	// beams
	for(int i = 0; i < beamsCount; i++)
	{
		drawCell(cr, dx, dy, wid, height, CS_RIGHT | CS_BOTTOM);
		dy += height;
	}
	// circumf..
	for(int i = 0; i < 4; i++)
	{
		drawCell(cr, dx, dy, wid, height, CS_RIGHT | CS_BOTTOM);
		dy += height;
	}
	// end of beams

	// tot
	drawCell(cr, dx, dy, wid, height, CS_RIGHT | CS_BOTTOM_BOLD);
	if(total[0] > 0)
	{
		std::ostringstream str;
		str.setf(ios::fixed);
		str.precision(2);
		str<<total[0];
		drawTextLine(cr, dx + wid * 0.5, dy + height * 0.5, str.str(), 10, TS_BOLD | TS_CENTERED);
	}

	// col #3
	dy = firstLn;
	dx += wid;

	drawCell(cr, dx, dy, wid, height, CS_RIGHT | CS_BOTTOM);
	//drawTextLine(cr, dx + wid * 0.5, dy + height * 0.5, "D", 10, TS_CENTERED);
	dy += height;

	// draw E row
	drawCell(cr, dx, dy, wid, height, CS_RIGHT | CS_BOTTOM);
	//drawTextLine(cr, dx + wid * 0.5, dy + height * 0.5, "", 10, TS_CENTERED);
	dy += height;

	// beams
    for(int i = 1; i < beamsCount; ++i)
    {
		drawCell(cr, dx, dy, wid, height, CS_RIGHT | CS_BOTTOM);

        if(cline && GetGlobalView()->getTipsMap(1).find(col2[i]) != GetGlobalView()->getTipsMap(1).end())
        {
            std::ostringstream str;
            str.setf(ios::fixed);
            str.precision(2);

            double length = GetGlobalView()->getTipsMap(1)[col2[i]]->m_length;
           
            str << length;
            total[1] += length;

            drawTextLine(cr, dx + wid * 0.5, dy + height * 0.5, str.str(), 10, TS_CENTERED);
        }
        dy += height;
    }

	// circumf..
	for(int i = 0; i < 4; i++)
	{
		drawCell(cr, dx, dy, wid, height, CS_RIGHT | CS_BOTTOM);
		//drawTextLine(cr, dx + 0.05, dy + height * 0.5, circ[i], 10, TS_CENTERED_Y);
		dy += height;
	}
	// end of beams

	// tot
	drawCell(cr, dx, dy, wid, height, CS_BOTTOM_BOLD | CS_RIGHT);
	if(total[1] > 0)
	{
		std::ostringstream str;
		str.setf(ios::fixed);
		str.precision(2);
		str<<total[1];
		drawTextLine(cr, dx + wid * 0.5, dy + height * 0.5, str.str(), 10, TS_BOLD | TS_CENTERED);
	}	

	// col #4
	dy = firstLn;
	dx += wid;

	drawCell(cr, dx, dy, wid, height, CS_RIGHT | CS_BOTTOM);
	//drawTextLine(cr, dx + wid * 0.5, dy + height * 0.5, "D", 10, TS_CENTERED);
	dy += height;

	// draw E row
	drawCell(cr, dx, dy, wid, height, CS_RIGHT | CS_BOTTOM);
	//drawTextLine(cr, dx + wid * 0.5, dy + height * 0.5, "", 10, TS_CENTERED);
	dy += height;

	// beams
    for(int i = 1; i < beamsCount; ++i)
    {
		drawCell(cr, dx, dy, wid, height, CS_RIGHT | CS_BOTTOM);

        if(cline && GetGlobalView()->getTipsMap(0).find(col2[i]) != GetGlobalView()->getTipsMap(0).end())
        {
            std::ostringstream str;
            str.setf(ios::fixed);
            str.precision(2);

            double length = GetGlobalView()->getTipsMap(0)[col2[i]]->m_length;
            str << length;
            total[2] += length;

            drawTextLine(cr, dx + wid * 0.5, dy + height * 0.5, str.str(), 10, TS_CENTERED);
        }
        dy += height;
    }

	// circumf..
	for(int i = 0; i < 4; i++)
	{
		drawCell(cr, dx, dy, wid, height, CS_RIGHT | CS_BOTTOM);
		//drawTextLine(cr, dx + 0.05, dy + height * 0.5, circ[i], 10, TS_CENTERED_Y);
		dy += height;
	}
	// end of beams

	// tot
	drawCell(cr, dx, dy, wid, height, CS_BOTTOM_BOLD | CS_RIGHT);
	if(total[2] > 0)
	{
		std::ostringstream str;
		str.setf(ios::fixed);
		str.precision(2);
		str<<total[2];
		drawTextLine(cr, dx + wid * 0.5, dy + height * 0.5, str.str(), 10, TS_BOLD | TS_CENTERED);
	}

	// col #5
	dy = firstLn;
	dx += wid;

	drawCell(cr, dx, dy, wid, height, CS_RIGHT_BOLD | CS_BOTTOM);
	//drawTextLine(cr, dx + wid * 0.5, dy + height * 0.5, "D", 10, TS_CENTERED);
	dy += height;

	// draw E row
	drawCell(cr, dx, dy, wid, height, CS_RIGHT_BOLD | CS_BOTTOM);
	if(eTotal[0] > 0 || eTotal[1] > 0)
	{
		std::ostringstream str;
        str.setf(ios::fixed);
        str.precision(2);
		str << abs(eTotal[1] + eTotal[0]);
		total[3] += abs(eTotal[1] + eTotal[0]);

		drawTextLine(cr, dx + wid * 0.5, dy + height * 0.5, str.str(), 10, TS_CENTERED);
	}
	dy += height;

	// beams
    for(int i = 1; i < beamsCount; ++i)
    {
		drawCell(cr, dx, dy, wid, height, CS_RIGHT_BOLD | CS_BOTTOM);
		double len[2] = {0};

        if(cline && GetGlobalView()->getTipsMap(0).find(col2[i]) != GetGlobalView()->getTipsMap(0).end())
			len[0] = GetGlobalView()->getTipsMap(0)[col2[i]]->m_length;
		if(cline && GetGlobalView()->getTipsMap(1).find(col2[i]) != GetGlobalView()->getTipsMap(1).end())
			len[1] = GetGlobalView()->getTipsMap(1)[col2[i]]->m_length;
		{
            std::ostringstream str;
            str.setf(ios::fixed);
            str.precision(2);

            double diff = abs(len[1] - len[0]);
            str << diff;
            total[3] += diff;

			if(diff > 0)
				drawTextLine(cr, dx + wid * 0.5, dy + height * 0.5, str.str(), 10, TS_CENTERED);
        }
        dy += height;
    }

	// circumf..
	for(int i = 0; i < 4; i++)
	{
		drawCell(cr, dx, dy, wid, height, CS_RIGHT_BOLD | CS_BOTTOM);
		//drawTextLine(cr, dx + 0.05, dy + height * 0.5, circ[i], 10, TS_CENTERED_Y);
		dy += height;
	}
	// end of beams

	// tot
	drawCell(cr, dx, dy, wid, height, CS_BOTTOM_BOLD | CS_RIGHT_BOLD);
	if(total[3] > 0)
	{
		std::ostringstream str;
		str.setf(ios::fixed);
		str.precision(2);
		str<<total[3];
		drawTextLine(cr, dx + wid * 0.5, dy + height * 0.5, str.str(), 10, TS_BOLD | TS_CENTERED);
	}

	// table #3
	dy += height * 1.5;
	firstLn = dy;
	dx = x;

	wid = 0.85;
	drawCell(cr, dx, dy, wid, height * 3, CS_LEFT_BOLD | CS_TOP_BOLD | CS_RIGHT | CS_BOTTOM);
	drawTextLine(cr, dx + wid * 0.5, dy + height * 3 * 0.5, "ADD", 10, TS_CENTERED);

	dx += wid;

	drawCell(cr, dx, dy, wid, height, CS_TOP_BOLD | CS_RIGHT | CS_BOTTOM);
	drawTextLine(cr, dx + wid * 0.5, dy + height * 0.5, "Column 1", 10, TS_CENTERED);
	dy += height;

	drawCell(cr, dx, dy, wid, height, CS_RIGHT | CS_BOTTOM);
	drawTextLine(cr, dx + wid * 0.5, dy + height * 0.5, "Column 2", 10, TS_CENTERED);
	dy += height;

	drawCell(cr, dx, dy, wid, height, CS_RIGHT | CS_BOTTOM);
	drawTextLine(cr, dx + wid * 0.5, dy + height * 0.5, "Column 3", 10, TS_CENTERED);
	dy += height;

	dx = x;
	wid = 0.85 * 2;
	drawCell(cr, dx, dy, wid, height,  CS_LEFT_BOLD | CS_RIGHT | CS_BOTTOM);
	drawTextLine(cr, dx + wid * 0.5, dy + height * 0.5, "Subtotal", 10, TS_CENTERED);
	dy += height;

	drawCell(cr, dx, dy, wid, height,  CS_LEFT_BOLD | CS_RIGHT | CS_BOTTOM);
	drawTextLine(cr, dx + wid * 0.5, dy + height * 0.5, "SUBTRACT Column 4", 10, TS_CENTERED);
	dy += height;

	drawCell(cr, dx, dy, wid, height,  CS_LEFT_BOLD | CS_RIGHT | CS_BOTTOM_BOLD);
	drawTextLine(cr, dx + wid * 0.5, dy + height * 0.5, "FINAL SCORE", 10, TS_BOLD | TS_CENTERED);
	dy += height;

	// col #2
	dx += wid;
	dy = firstLn;
	wid = 0.85;

	drawCell(cr, dx, dy, wid, height, CS_TOP_BOLD | CS_RIGHT | CS_BOTTOM);
	if(total[0] > 0)
	{
		std::ostringstream str;
		str.setf(ios::fixed);
		str.precision(2);
		str<<total[0];
		drawTextLine(cr, dx + wid * 0.5, dy + height * 0.5, str.str(), 10, TS_BOLD | TS_CENTERED);
	}
	dy += height;

	drawCell(cr, dx, dy, wid, height, CS_RIGHT | CS_BOTTOM);
	if(total[1] > 0)
	{
		std::ostringstream str;
		str.setf(ios::fixed);
		str.precision(2);
		str<<total[1];
		drawTextLine(cr, dx + wid * 0.5, dy + height * 0.5, str.str(), 10, TS_BOLD | TS_CENTERED);
	}
	dy += height;

	drawCell(cr, dx, dy, wid, height, CS_RIGHT | CS_BOTTOM);
	if(total[2] > 0)
	{
		std::ostringstream str;
		str.setf(ios::fixed);
		str.precision(2);
		str<<total[2];
		drawTextLine(cr, dx + wid * 0.5, dy + height * 0.5, str.str(), 10, TS_BOLD | TS_CENTERED);
	}
	dy += height;

	score = total[0] + total[1] + total[2];
	drawCell(cr, dx, dy, wid, height, CS_RIGHT | CS_BOTTOM);
	if(score > 0)
	{
		std::ostringstream str;
		str.setf(ios::fixed);
		str.precision(2);
		str<<score;
		drawTextLine(cr, dx + wid * 0.5, dy + height * 0.5, str.str(), 10, TS_BOLD | TS_CENTERED);
	}
	dy += height;

	drawCell(cr, dx, dy, wid, height, CS_RIGHT | CS_BOTTOM);
	if(total[3] > 0)
	{
		std::ostringstream str;
		str.setf(ios::fixed);
		str.precision(2);
		str<<total[3];
		drawTextLine(cr, dx + wid * 0.5, dy + height * 0.5, str.str(), 10, TS_BOLD | TS_CENTERED);
	}
	dy += height;

	score -= total[3];
	drawCell(cr, dx, dy, wid, height, CS_RIGHT | CS_BOTTOM_BOLD);
	if(total[3] > 0)
	{
		std::ostringstream str;
		str.setf(ios::fixed);
		str.precision(2);
		str<<score;
		drawTextLine(cr, dx + wid * 0.5, dy + height * 0.5, str.str(), 10, TS_BOLD | TS_CENTERED);
	}
	dy += height;

	// col #3
	dx += wid;
	dy = firstLn;
	wid = 4.95;

	drawCell(cr, dx, dy, wid, height, CS_TOP_BOLD | CS_RIGHT_BOLD | CS_BOTTOM);
	drawTextLine(cr, dx + 0.05, dy + height * 0.5, "Exact Locality Where Killed:", 10, TS_CENTERED_Y);
	dy += height;

	drawCell(cr, dx, dy, wid, height, CS_RIGHT_BOLD | CS_BOTTOM);
	drawTextLine(cr, dx + 0.05, dy + height * 0.5, "Date Killed:", 10, TS_CENTERED_Y);
	drawTextLine(cr, dx + 2 + 0.05, dy + height * 0.5, "Hunter:", 10, TS_CENTERED_Y);
	dy += height;

	drawCell(cr, dx, dy, wid, height, CS_RIGHT_BOLD | CS_BOTTOM);
	drawTextLine(cr, dx + 0.05, dy + height * 0.5, "Trophy Owner:", 10, TS_CENTERED_Y);
	drawTextLine(cr, dx + 2.5 + 0.05, dy + height * 0.5, "Telephone #:", 10, TS_CENTERED_Y);
	dy += height;

	drawCell(cr, dx, dy, wid, height, CS_RIGHT_BOLD | CS_BOTTOM);
	drawTextLine(cr, dx + 0.05, dy + height * 0.5, "Trophy Owner's Address:", 10, TS_CENTERED_Y);
	dy += height;

	drawCell(cr, dx, dy, wid, height, CS_RIGHT_BOLD | CS_BOTTOM);
	drawTextLine(cr, dx + 0.05, dy + height * 0.5, "Trophy Owner's E-mail:", 10, TS_CENTERED_Y);
	drawTextLine(cr, dx + 2.5 + 0.05, dy + height * 0.5, "Guide's Name:", 10, TS_CENTERED_Y);
	dy += height;

	drawCell(cr, dx, dy, wid, height, CS_RIGHT_BOLD | CS_BOTTOM_BOLD);
	drawTextLine(cr, dx + 0.05, dy + height * 0.5, "Remarks: (Mention Any Abnormalities or Unique Qualities)", 10, TS_CENTERED_Y);
	dy += height;
}
// ------------------------------------------------------------------------------------------------------
void drawTable_Safari(cairo_t *cr, double x, double y)
{
	CenterLine* cline = GetGlobalView()->getCenterLine();
	int beamsCount;
	if(cline)
	{
		beamsCount = max(cline->m_sortedTips[0].size(), cline->m_sortedTips[1].size());
		if(beamsCount > 11)
			beamsCount = 11;
	}
	else
		beamsCount = 4;

	string col1[11] = {	"",
						"T-1",
						"T-2",
						"T-3",
                        "T-4",
                        "T-5",
                        "T-6",
                        "T-7",
                        "T-8",
                        "T-9",
                        "T-10" };

	string NT[11] = {	"NT-1",
						"NT-2",
						"NT-3",
                        "NT-4",
						"NT-5",
						"NT-6",
						"NT-7",
						"NT-8",
						"NT-9",
						"NT-10",
						"NT-11"	};

	string col2[11] = {	"F", "G1", "G2", "G3", "G4",  
		"G5", "G6", "G7", "G8", "G9", "G10" };

	string circ[11] = {	"C-1", "C-2", "C-3", "C-4", "C-5" };

	double dx = x;
	double dy = y;
	double wid = 4.1;
	double height = 0.20;
	double score = 0;

	// table #1
	dx = x;
	dy = y;
	wid = 1.7;
	// header #1
	/*drawCell(cr, dx, dy, wid, height);
	drawTextLine(cr, dx + wid * 0.5, dy + height * 0.5, "Abnormal Points", 10, TS_BOLD | TS_CENTERED);*/

	cairo_set_line_cap (cr, CAIRO_LINE_CAP_ROUND);

	// bold
	cairo_set_line_width (cr, 1.0);
		cairo_move_to(cr, LEFT_BORDER + WORKING_AREA_WIDTH / 2, TOP_BORDER);
		cairo_rel_line_to(cr, 0, WORKING_AREA_HEIGHT);
	cairo_stroke (cr);

	drawTextLine(cr, dx, dy, "Hunter __________________________________________________________", 10, TS_NONE_);
	dy += 0.1;
	drawTextLine(cr, dx + 1, dy, "How you want your name to appear in the Record Book", 8, TS_NONE_);
	dy += 0.25;
	drawTextLine(cr, dx, dy, "Membership No. ________________ e-mail ___________________________", 10, TS_NONE_);
	dy += 0.25;
	drawTextLine(cr, dx, dy, "Adress __________________________________________________________", 10, TS_NONE_);
	dy += 0.25;
	drawTextLine(cr, dx, dy, "_________________________________________________________________", 10, TS_NONE_);
	dy += 0.1; 
	drawTextLine(cr, dx, dy, "                City                         State                          Zip                   Country", 8, TS_NONE_);
	dy += 0.25;
	drawTextLine(cr, dx, dy, "Ph. (____) ______________ (____) ______________ (____) ______________", 10, TS_NONE_);
	dy += 0.1; 
	drawTextLine(cr, dx, dy, "                             Home                                  Business                                 Fax", 8, TS_NONE_);
	dy += 0.25;
	//drawTextLine(cr, dx, dy, "_________________________________________________________________", 10, TS_NONE_);
	drawTextLine(cr, dx, dy, "I certify that, to the best of my knowledge, I took this animal without", 10, TS_NONE_);
	dy += 0.15; 
	drawTextLine(cr, dx, dy, "violating the wildlife laws or ethical hunting practices of the country", 10, TS_NONE_);
	dy += 0.15; 
	drawTextLine(cr, dx, dy, "or province in which I hunted. I also certify that, to the best of my", 10, TS_NONE_);
	dy += 0.15; 
	drawTextLine(cr, dx, dy, "knowledge, the laws of my country have not been violated by my taking", 10, TS_NONE_);
	dy += 0.15; 
	drawTextLine(cr, dx, dy, "or importing this animal.", 10, TS_NONE_);
	dy += 0.25; 
	drawTextLine(cr, dx, dy, "Free-ranging", 10, TS_BOLD);
	drawTextLine(cr, dx + 0.75, dy, "  _ Yes  _ No", 10, TS_NONE_);
	dy += 0.25; 
	drawTextLine(cr, dx, dy, "Signature ________________________________________________________", 10, TS_NONE_);
	dy += 0.15;
	drawTextLine(cr, dx, dy, "The acceptance or denial of all entries are at the discretion of Safari", 10, TS_NONE_);
	dy += 0.15;
	drawTextLine(cr, dx, dy, "Club International, its Board and committees. Entries are subject to", 10, TS_NONE_);
	dy += 0.15;
	drawTextLine(cr, dx, dy, "review by the Trophy Records Committee of SCI at any time. All photos", 10, TS_NONE_);
	dy += 0.15;
	drawTextLine(cr, dx, dy, "and entries subitted to SCI become SCI's property.", 10, TS_NONE_);
	dy += 0.15;

	// right side
	dx = WORKING_AREA_WIDTH / 72 / 2 + 0.15;
	dy = 0.1;

	drawTextLine(cr, dx, dy, "Animal", 10, TS_BOLD);
	drawTextLine(cr, dx + 0.45, dy, "___________________________________________________________", 10, TS_NONE_);
	dy += 0.2;
	drawTextLine(cr, dx, dy, "Remeasurement?  _ Yes  _ No  Former Score _____ Record No. __________", 10, TS_NONE_);
	dy += 0.25;
	drawTextLine(cr, dx, dy, "Date Taken_________________________________________________________", 10, TS_NONE_);
	dy += 0.1;
	drawTextLine(cr, dx, dy, "                                          Month                              Day                              Year", 8, TS_NONE_);
	dy += 0.25;
	drawTextLine(cr, dx, dy, "_ Rifle   _ Handgun   _ Muzzleloader   _ Bow   _ Crossbow   _ Picked Up", 10, TS_NONE_);
	dy += 0.25;
	drawTextLine(cr, dx, dy, "Place Taken____________________________   __________________________", 10, TS_NONE_);
	dy += 0.1;
	drawTextLine(cr, dx, dy, "                                            Country                                           State or Province", 8, TS_NONE_);
	dy += 0.25;
	drawTextLine(cr, dx, dy, "Locality ___________________________________________________________", 10, TS_NONE_);
	dy += 0.25;
	drawTextLine(cr, dx, dy, "Guide _________________________ Hunting Co. _________________________", 10, TS_NONE_);

	double total[2] = {0};
	double eTotal[2] = {0};

	dy += 0.25;
	drawTextLine(cr, dx, dy, "I. Length of Main Beam", 10, TS_BOLD);
	//drawTextLine(cr, dx + 1.7, dy, "L _________ /8", 10, TS_NONE_);
	//drawTextLine(cr, dx + 2.6, dy, "R _________ /8", 10, TS_NONE_);
	dy += 0.25;

	drawTextLine(cr, dx, dy, "II. Length of", 10, TS_BOLD);
	dy += 0.15;
	drawTextLine(cr, dx, dy, "      Typical Tines", 10, TS_BOLD);
	dy += 0.3;
	drawTextLine(cr, dx, dy, "(Use back of form for", 10, TS_ITALIC);
	dy += 0.15;
	drawTextLine(cr, dx, dy, "additional tines)", 10, TS_ITALIC);
	dy -= 0.15 + 0.3 + 0.15 + 0.25;

	// beams
    for(int i = 0; i < beamsCount; ++i)
    {
		drawTextLine(cr, dx + 1.4, dy, col1[i], 10, TS_BOLD);
		drawTextLine(cr, dx + 1.8, dy, "L _________ /8", 10, TS_NONE_);
		drawTextLine(cr, dx + 2.7, dy, "R _________ /8", 10, TS_NONE_);

		// col #1
        if(cline && GetGlobalView()->getTipsMap(0).find(col2[i]) != GetGlobalView()->getTipsMap(0).end())
        {
            std::ostringstream str;
            str.setf(ios::fixed);
            str.precision(2);

            double length = GetGlobalView()->getTipsMap(0)[col2[i]]->m_length;
           
            str << length;
            total[0] += length;

			drawTextLine(cr, dx + 2.15, dy, str.str(), 10, TS_CENTERED_X);

        }
		// col #2
        if(cline && GetGlobalView()->getTipsMap(1).find(col2[i]) != GetGlobalView()->getTipsMap(1).end())
        {
            std::ostringstream str;
            str.setf(ios::fixed);
            str.precision(2);

            double length = GetGlobalView()->getTipsMap(1)[col2[i]]->m_length;
           
            str << length;
            total[1] += length;

			drawTextLine(cr, dx + 3.05, dy, str.str(), 10, TS_CENTERED_X);
        }
        dy += 0.25;
    }

	drawTextLine(cr, dx + 1.2, dy, "Subtotal", 10, TS_BOLD);
	drawTextLine(cr, dx + 1.8, dy, "L _________ /8", 10, TS_NONE_);
	drawTextLine(cr, dx + 2.7, dy, "R _________ /8", 10, TS_NONE_);

	if(total[0] > 0)
	{
		std::ostringstream str;
		str.setf(ios::fixed);
		str.precision(2);
		str<<total[0];
		drawTextLine(cr, dx + 2.15, dy, str.str(), 10, TS_CENTERED_X);
	}

	if(total[1] > 0)
	{
		std::ostringstream str;
		str.setf(ios::fixed);
		str.precision(2);
		str<<total[1];
		drawTextLine(cr, dx + 3.05, dy, str.str(), 10, TS_CENTERED_X);
	}
	dy += 0.25;

	// Circumference
	drawTextLine(cr, dx, dy, "III. Circumference of", 10, TS_BOLD);
	dy += 0.15;
	drawTextLine(cr, dx, dy, "      Main Beam", 10, TS_BOLD);
	dy -= 0.15;

    for(int i = 0; i < 4; ++i)
    {
		drawTextLine(cr, dx + 1.4, dy, circ[i], 10, TS_BOLD);
		drawTextLine(cr, dx + 1.8, dy, "L _________ /8", 10, TS_NONE_);
		drawTextLine(cr, dx + 2.7, dy, "R _________ /8", 10, TS_NONE_);
        dy += 0.25;
    }

	drawTextLine(cr, dx + 1.2, dy, "Subtotal", 10, TS_BOLD);
	drawTextLine(cr, dx + 1.8, dy, "L _________ /8", 10, TS_NONE_);
	drawTextLine(cr, dx + 2.7, dy, "R _________ /8", 10, TS_NONE_);
	dy += 0.25;

	score = total[0] + total[1];
	// Inside Span of Main Beams
	drawTextLine(cr, dx, dy, "IV. Inside Span of Main Beams", 10, TS_BOLD);
	drawTextLine(cr, dx + 2.675, dy, "   _________ /8", 10, TS_NONE_);
	if(cline && cline->m_dims.size() > 0)
	{
		std::ostringstream str;
		str.setf(ios::fixed);
		str.precision(2);
		str<<cline->m_dims[0].m_distance;
		score += cline->m_dims[0].m_distance;

		drawTextLine(cr, dx + 3.05, dy, str.str(), 10, TS_CENTERED_X);
	}
	dy += 0.25;

	drawTextLine(cr, dx, dy, "V. Total Score", 10, TS_BOLD);
	drawTextLine(cr, dx + 2.675, dy, "   _________ /8", 10, TS_NONE_);
	if(score > 0)
	{
		std::ostringstream str;
		str.setf(ios::fixed);
		str.precision(2);
		str<<score;

		drawTextLine(cr, dx + 3.05, dy, str.str(), 10, TS_CENTERED_X);
	}
	dy += 0.5;

	int iCount = max(GetGlobalView()->getETips(0).size(), GetGlobalView()->getETips(1).size());
	if (iCount < 4)
		iCount = 4;

	drawCell(cr, dx, dy - 0.15, WORKING_AREA_WIDTH / 72 / 2, 1.30 + (iCount - 4) * 0.25, CS_ALL_BOLD);
	drawCell(cr, dx, dy + 1.15 + (iCount - 4) * 0.25, WORKING_AREA_WIDTH / 72 / 2, 1.15, CS_VERTICAL_BOLD | CS_BOTTOM_BOLD);

	drawTextLine(cr, dx + WORKING_AREA_WIDTH / 72 / 4, dy, "Supplemental Information", 10, TS_BOLD | TS_CENTERED_X);
	dy += 0.25;
	drawTextLine(cr, dx + 0.1, dy, "Length of non-typical", 8, TS_BOLD);
	dy += 0.1;
	drawTextLine(cr, dx + 0.1, dy, "tines, if any.", 8, TS_BOLD);
	dy += 0.1;
	drawTextLine(cr, dx + 0.1, dy, "(Not to be included in", 8, TS_BOLD);
	dy += 0.1;
	drawTextLine(cr, dx + 0.1, dy, "total score)", 8, TS_BOLD);
	dy -= 0.1 * 3;

	for(int i = 0; i < iCount; i++)
	{
		drawTextLine(cr, dx + 1.37, dy, NT[i], 10, TS_BOLD);
		drawTextLine(cr, dx + 1.80, dy, "L _________ /8", 10, TS_NONE_);
		drawTextLine(cr, dx + 2.70, dy, "R _________ /8", 10, TS_NONE_);

		if(i < GetGlobalView()->getETips(0).size())
		{
			std::ostringstream str;
			str.setf(ios::fixed);
			str.precision(2);
			str<<GetGlobalView()->getETips(0)[i]->m_length;

			drawTextLine(cr, dx + 2.15, dy, str.str(), 10, TS_CENTERED_X);
		}

		if(i < GetGlobalView()->getETips(1).size())
		{
			std::ostringstream str;
			str.setf(ios::fixed);
			str.precision(2);
			str<<GetGlobalView()->getETips(1)[i]->m_length;

			drawTextLine(cr, dx + 3.05, dy, str.str(), 10, TS_CENTERED_X);
		}
		dy += 0.25;
	}

	dy += 0.1;

	drawTextLine(cr, dx + 0.1, dy, "Number of Typical Points", 10, TS_NONE_);
	drawTextLine(cr, dx + 1.80, dy, "L _________", 10, TS_NONE_);
	drawTextLine(cr, dx + 2.70, dy, "R _________", 10, TS_NONE_);
	dy += 0.12;
	drawTextLine(cr, dx + 0.1, dy, "(All typical tines plus beam tip)", 8, TS_NONE_);
	dy += 0.18;
	drawTextLine(cr, dx + 0.1, dy, "Number of Non-typical Points", 10, TS_NONE_);
	drawTextLine(cr, dx + 1.80, dy, "L _________", 10, TS_NONE_);
	drawTextLine(cr, dx + 2.70, dy, "R _________", 10, TS_NONE_);
	dy += 0.12;
	drawTextLine(cr, dx + 0.1, dy, "(All non-typical tines)", 8, TS_NONE_);
	dy += 0.23;
	drawTextLine(cr, dx + 0.1, dy, "Total Number of Points", 10, TS_NONE_);
	dy += 0.12;
	drawTextLine(cr, dx + 0.1, dy, "(All tines plus beam tip)", 8, TS_NONE_);
	drawTextLine(cr, dx + 1.80, dy, "L _________", 10, TS_NONE_);
	drawTextLine(cr, dx + 2.70, dy, "R _________", 10, TS_NONE_);
	dy -= 0.12 + 0.18 + 0.12 + 0.23 + 0.12;

	if(GetGlobalView()->getTipsMap(0).size())
	{
		std::ostringstream str;
		str.setf(ios::fixed);
		str.precision(2);
		str<<GetGlobalView()->getTipsMap(0).size();

		drawTextLine(cr, dx + 2.15, dy, str.str(), 10, TS_CENTERED_X);
	}

	if(GetGlobalView()->getTipsMap(1).size())
	{
		std::ostringstream str;
		str.setf(ios::fixed);
		str.precision(2);
		str<<GetGlobalView()->getTipsMap(1).size();

		drawTextLine(cr, dx + 3.05, dy, str.str(), 10, TS_CENTERED_X);
	}
	dy += 0.30;

	if(GetGlobalView()->getETips(0).size())
	{
		std::ostringstream str;
		str.setf(ios::fixed);
		str.precision(2);
		str<<GetGlobalView()->getETips(0).size();

		drawTextLine(cr, dx + 2.15, dy, str.str(), 10, TS_CENTERED_X);
	}

	if(GetGlobalView()->getETips(1).size())
	{
		std::ostringstream str;
		str.setf(ios::fixed);
		str.precision(2);
		str<<GetGlobalView()->getETips(1).size();

		drawTextLine(cr, dx + 3.05, dy, str.str(), 10, TS_CENTERED_X);
	}
	dy += 0.45;

	if(cline)
	{
		std::ostringstream str;
		str.setf(ios::fixed);
		str.precision(2);
		str<<cline->m_sortedTips[0].size();

		drawTextLine(cr, dx + 2.15, dy, str.str(), 10, TS_CENTERED_X);
	}

	if(cline)
	{
		std::ostringstream str;
		str.setf(ios::fixed);
		str.precision(2);
		str<<cline->m_sortedTips[1].size();

		drawTextLine(cr, dx + 3.05, dy, str.str(), 10, TS_CENTERED_X);
	}
	dy += 0.5;
	drawTextLine(cr, dx, dy, "Official Measurer ___________________________________________________", 10, TS_NONE_);
	dy += 0.25;
	drawTextLine(cr, dx, dy, "Measurer No. ________________________ Email_________________________", 10, TS_NONE_);
	dy += 0.25;
	drawTextLine(cr, dx, dy, "Day Measured ______________________________________________________", 10, TS_NONE_);
	dy += 0.1;
	drawTextLine(cr, dx, dy, "                                          Month                              Day                              Year", 8, TS_NONE_);
	dy += 0.25;
	drawTextLine(cr, dx, dy, "Signature of Measurer _______________________________________________", 10, TS_NONE_);
}

void ExportToPDF(const wchar_t* fName, ULONG type)
{
	size_t i;
	char pStr[1024];

	// Conversion
	wcstombs_s(&i, pStr, (size_t)1024, 
			   fName, (size_t)1024 );

	std::wostringstream str;
	//str<<"H: "<<h;
	//MessageBox(0, str.str().c_str(), L"Some", 0)

	cairo_surface_t *surface;
    cairo_t *cr;
	surface = cairo_pdf_surface_create(pStr, PAGE_WIDTH, PAGE_HEIGHT);
	cr = cairo_create (surface);

	double curY = 0;
	double h = 0;

	std::string path = getAppDirA();
	if(type == ST_POPE_AND_YOUNG)
	{
		//h += writeImage(cr, path + "Media/Mule.png", 0, h );
		h += writeImage(cr, path + "Media/Whitetail_P&Y.png", 0, h);

		drawTable_PY(cr, 0, h + 0.4);
	}
	else
	if(type == ST_BOONE_AND_CROCKETT)
	{
		//h += writeImage(cr, path + "Media/Mule.png", 0, h );
		h += writeImage(cr, path + "Media/Whitetail_B&C.png", 0, h);

		drawTable_BC(cr, 0, h + 0.4);
	}
	else
	if(type == ST_SAFARI_WHITETAIL)
	{
		//h += writeImage(cr, path + "Media/Mule.png", 0, h );
		h += writeImage(cr, path + "Media/Whitetail_Safari.png", 0, h, WORKING_AREA_WIDTH / 72 / 2 - 0.15);

		drawTable_Safari(cr, 0, h + 0.2);
	}
	else
	if(type == ST_SAFARI_MULE)
	{
		//h += writeImage(cr, path + "Media/Mule.png", 0, h );
		h += writeImage(cr, path + "Media/Mule_Safari.png", 0, h, WORKING_AREA_WIDTH / 72 / 2 - 0.15);

		drawTable_Safari(cr, 0, h + 0.2);
	}

	// render page to file
	cairo_show_page (cr);

	// free
	cairo_destroy (cr); 
    cairo_surface_destroy (surface); 
}


cairo_surface_t *surface = 0;

void UpdateScore(ULONG type)
{
	if(surface)
	{
		cairo_surface_destroy (surface); 
		surface = 0;
	}
    cairo_t *cr;
	surface = cairo_image_surface_create(CAIRO_FORMAT_RGB24, PAGE_WIDTH, PAGE_HEIGHT);
	cr = cairo_create (surface);

	cairo_set_source_rgb(cr, 1,1,1);
	cairo_paint(cr);
	cairo_set_source_rgb(cr, 0,0,0);

	double curY = 0;
	double h = 0;

	std::string path = getAppDirA();
	if(type == ST_POPE_AND_YOUNG)
	{
		//h += writeImage(cr, path + "Media/Mule.png", 0, h );
		h += writeImage(cr, path + "Media/Whitetail_P&Y.png", 0, h);

		drawTable_PY(cr, 0, h + 0.4);
	}
	else
	if(type == ST_BOONE_AND_CROCKETT)
	{
		//h += writeImage(cr, path + "Media/Mule.png", 0, h );
		h += writeImage(cr, path + "Media/Whitetail_B&C.png", 0, h);

		drawTable_BC(cr, 0, h + 0.4);
	}
	else
	if(type == ST_SAFARI_WHITETAIL)
	{
		//h += writeImage(cr, path + "Media/Mule.png", 0, h );
		h += writeImage(cr, path + "Media/Whitetail_Safari.png", 0, h, WORKING_AREA_WIDTH / 72 / 2 - 0.15);

		drawTable_Safari(cr, 0, h + 0.2);
	}
	else
	if(type == ST_SAFARI_MULE)
	{
		//h += writeImage(cr, path + "Media/Mule.png", 0, h );
		h += writeImage(cr, path + "Media/Mule_Safari.png", 0, h, WORKING_AREA_WIDTH / 72 / 2 - 0.15);

		drawTable_Safari(cr, 0, h + 0.2);
	}

	// render page to file
	cairo_show_page (cr);

	// free
	cairo_destroy (cr); 
}

void PaintScore(HDC hdc)
{
	cairo_surface_t *win_surf;
	cairo_t *win_cr;
	
	win_surf = cairo_win32_surface_create(hdc);
	win_cr = cairo_create (win_surf);

	cairo_set_source_surface(win_cr, surface, 0, 0);
	cairo_paint(win_cr);

	cairo_destroy (win_cr); 
	cairo_surface_destroy (win_surf); 
}

void FreeScore()
{
    cairo_surface_destroy (surface); 
	surface = 0;
}