
#pragma once

#include <GL/glut.h>

#include "Colors.h"


class CMaterial
{
	friend std::ostream& operator << (std::ostream& lhs, const CMaterial& rhs);
	friend std::istream& operator >> (std::istream& lhs, CMaterial& rhs);

private:

	CColor	_frontAmbient, _frontDiffuse, _frontSpecular, _frontEmissive;
	float	_frontShininess;

	CColor	_backAmbient, _backDiffuse, _backSpecular, _backEmissive;
	float	_backShininess;

public:

	CMaterial(
		CColor	frontAmbient		= CColor(), 
		CColor	frontDiffuse		= CColor(), 
		CColor	frontSpecular		= CColor(), 
		CColor	frontEmissive		= CColor(),
		float	frontShininess		= 128.0,

		CColor	backAmbient		= CColor(), 
		CColor	backDiffuse		= CColor(), 
		CColor	backSpecular	= CColor(), 
		CColor	backEmissive	= CColor(),
		float	backShininess	= 128.0);

	void SetAmbientColor	(GLenum face, CColor c);
	void SetAmbientColor	(GLenum face, float r, float g, float b, float a = 1.0);

	void SetDiffuseColor	(GLenum face, CColor c);
	void SetDiffuseColor	(GLenum face, float r, float g, float b, float a = 1.0);

	void SetSpecularColor	(GLenum face, CColor c);
	void SetSpecularColor	(GLenum face, float r, float g, float b, float a = 1.0);

	void SetEmissiveColor	(GLenum face, CColor c);
	void SetEmissiveColor	(GLenum face, float r, float g, float b, float a = 1.0);

	void SetShininess		(GLenum face, float shininess);
	void SetTransparency	(float alpha);

	void Apply() const;

	bool FrontIsTransparent();
	bool BackIsTransparent();

};

/*std::ostream& operator << (std::ostream& lhs, const CMaterial &rhs);
std::istream& operator >> (std::istream& lhs, CMaterial &rhs);*/


extern 
CMaterial	MatFBOrange, 
			MatFBBlue, 
			MatFBBlue2, 
			MatFBRed, 
			MatFBRed2, 
			MatFBGreen, 
			MatFBGreen1, 
			MatFBGreen2, 
			MatFBGreen3,
			MatFBGreen4,
			MatFBDarkGrey,
			MatFBGrey, 
			MatFBGrey2,
			MatFBViolet,
			MatFBPink,
			MatFBWhite,
			MatFBBrass,
			MatFBBronze,
			MatFBPolishedBronze,
			MatFBChrome,
			MatFBCopper,
			MatFBPolishedCopper,
			MatFBGold,
			MatFBPolishedGold,
			MatFBPewter,
			MatFBSilver,
			MatFBSilver1,
			MatFBPolishedSilver,
			MatFBEmerald,
			MatFBJade,
			MatFBObsidian,
			MatFBPearl,
			MatFBRuby,
			MatFBTurquoise,
			MatFBBlackPlastique,
			MatFBBlackRubber;
