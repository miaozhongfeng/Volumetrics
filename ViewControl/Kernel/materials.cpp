
#include "stdafx.h"
#include "Materials.h"
#include "Exceptions.h"

using namespace std;

CMaterial::CMaterial(
	CColor	frontAmbient, 
	CColor	frontDiffuse, 
	CColor	frontSpecular, 
	CColor	frontEmissive,
	float	frontShininess,

	CColor	backAmbient, 
	CColor	backDiffuse, 
	CColor	backSpecular, 
	CColor	backEmissive,
	float	backShininess):
//-------------------------------------
	_frontAmbient	(frontAmbient), 
	_frontDiffuse	(frontDiffuse), 
	_frontSpecular	(frontDiffuse), 
	_frontEmissive	(frontEmissive), 
	_frontShininess	(frontShininess), 

	_backAmbient	(backAmbient), 
	_backDiffuse	(backDiffuse), 
	_backSpecular	(backSpecular), 
	_backEmissive	(backEmissive),
	_backShininess	(backShininess)
{

}

//-------------------------------------
void CMaterial::SetAmbientColor(GLenum face, float r, float g, float b, float a)
//-------------------------------------
{
	switch (face)
	{
	case GL_FRONT:

			_frontAmbient.r = r;
			_frontAmbient.g = g;
			_frontAmbient.b = b;
			_frontAmbient.a = a;

		break;

	case GL_BACK:

			_backAmbient.r = r;
			_backAmbient.g = g;
			_backAmbient.b = b;
			_backAmbient.a = a;

		break;

	case GL_FRONT_AND_BACK:

			_frontAmbient.r = r;
			_frontAmbient.g = g;
			_frontAmbient.b = b;
			_frontAmbient.a = a;


			_backAmbient.r = r;
			_backAmbient.g = g;
			_backAmbient.b = b;
			_backAmbient.a = a;

		break;
	}
}

//-------------------------------------
void CMaterial::SetDiffuseColor(GLenum face, float r, float g, float b, float a)
//-------------------------------------
{
	switch (face)
	{
	case GL_FRONT:

			_frontDiffuse.r = r;
			_frontDiffuse.g = g;
			_frontDiffuse.b = b;
			_frontDiffuse.a = a;

		break;

	case GL_BACK:

			_backDiffuse.r = r;
			_backDiffuse.g = g;
			_backDiffuse.b = b;
			_backDiffuse.a = a;

		break;

	case GL_FRONT_AND_BACK:

			_frontDiffuse.r = r;
			_frontDiffuse.g = g;
			_frontDiffuse.b = b;
			_frontDiffuse.a = a;


			_backDiffuse.r = r;
			_backDiffuse.g = g;
			_backDiffuse.b = b;
			_backDiffuse.a = a;

		break;
	}
}

//-------------------------------------
void CMaterial::SetSpecularColor(GLenum face, float r, float g, float b, float a)
//-------------------------------------
{
	switch (face)
	{
	case GL_FRONT:

			_frontSpecular.r = r;
			_frontSpecular.g = g;
			_frontSpecular.b = b;
			_frontSpecular.a = a;

		break;

	case GL_BACK:

			_backSpecular.r = r;
			_backSpecular.g = g;
			_backSpecular.b = b;
			_backSpecular.a = a;

		break;

	case GL_FRONT_AND_BACK:

			_frontSpecular.r = r;
			_frontSpecular.g = g;
			_frontSpecular.b = b;
			_frontSpecular.a = a;


			_backSpecular.r = r;
			_backSpecular.g = g;
			_backSpecular.b = b;
			_backSpecular.a = a;

		break;
	}
}

//-------------------------------------
void CMaterial::SetEmissiveColor(GLenum face, float r, float g, float b, float a)
//-------------------------------------
{
	switch (face)
	{
	case GL_FRONT:

			_frontEmissive.r = r;
			_frontEmissive.g = g;
			_frontEmissive.b = b;
			_frontEmissive.a = a;

			_backEmissive.r = r;
			_backEmissive.g = g;
			_backEmissive.b = b;
			_backEmissive.a = a;

		break;

	case GL_BACK:

			_backEmissive.r = r;
			_backEmissive.g = g;
			_backEmissive.b = b;
			_backEmissive.a = a;

		break;

	case GL_FRONT_AND_BACK:

			_frontEmissive.r = r;
			_frontEmissive.g = g;
			_frontEmissive.b = b;
			_frontEmissive.a = a;


			_backEmissive.r = r;
			_backEmissive.g = g;
			_backEmissive.b = b;
			_backEmissive.a = a;

		break;
	}
}

//-------------------------------------
void CMaterial::SetAmbientColor(GLenum face, CColor c)
//-------------------------------------
{
	switch (face)
	{
	case GL_FRONT:
			_frontAmbient = c;
		break;

	case GL_BACK:
			_backAmbient = c;
		break;

	case GL_FRONT_AND_BACK:
			_frontAmbient = c;
			_backAmbient = c;
		break;
	}
}

//-------------------------------------
void CMaterial::SetDiffuseColor(GLenum face, CColor c)
//-------------------------------------
{
	switch (face)
	{
	case GL_FRONT:
			_frontDiffuse = c;
		break;

	case GL_BACK:
			_backDiffuse = c;
		break;

	case GL_FRONT_AND_BACK:
			_frontDiffuse = c;
			_backDiffuse = c;
		break;
	}
}

//-------------------------------------
void CMaterial::SetSpecularColor(GLenum face, CColor c)
//-------------------------------------
{
	switch (face)
	{
	case GL_FRONT:
			_frontSpecular = c;
		break;

	case GL_BACK:
			_backSpecular = c;
		break;

	case GL_FRONT_AND_BACK:
			_frontSpecular = c;
			_backSpecular = c;
		break;
	}
}

//-------------------------------------
void CMaterial::SetEmissiveColor(GLenum face, CColor c)
//-------------------------------------
{
	switch (face)
	{
	case GL_FRONT:
			_frontEmissive = c;
		break;

	case GL_BACK:
			_backEmissive  = c;
		break;

	case GL_FRONT_AND_BACK:
			_frontEmissive = c;
			_backEmissive = c;
		break;
	}
}


//-------------------------------------
void CMaterial::SetShininess(GLenum face, float shininess)
//-------------------------------------
{
	switch (face)
	{
	case GL_FRONT:
			_frontShininess = shininess;
		break;
	case GL_BACK:
			_backShininess = shininess;
		break;
	case GL_FRONT_AND_BACK:
			_frontShininess = shininess;
			_backShininess = shininess;
		break;
	}
}

//-------------------------------------
void CMaterial::SetTransparency(float alpha)
//-------------------------------------
{
	_frontAmbient.a = _frontDiffuse.a = alpha;
	_backAmbient.a  = _backDiffuse.a  = alpha;
}

//-------------------------------------
void CMaterial::Apply() const
//-------------------------------------
{
	glMaterialfv(GL_FRONT, GL_AMBIENT,	 &_frontAmbient.r);
	glMaterialfv(GL_FRONT, GL_DIFFUSE,	 &_frontDiffuse.r);
	glMaterialfv(GL_FRONT, GL_SPECULAR,  &_frontSpecular.r);
	glMaterialfv(GL_FRONT, GL_EMISSION,  &_frontEmissive.r);
	glMaterialf	(GL_FRONT, GL_SHININESS, _frontShininess);

	glMaterialfv(GL_BACK, GL_AMBIENT,	 &_backAmbient.r);
	glMaterialfv(GL_BACK, GL_DIFFUSE,	 &_backDiffuse.r);
	glMaterialfv(GL_BACK, GL_SPECULAR,	 &_backSpecular.r);
	glMaterialfv(GL_BACK, GL_EMISSION,	 &_backEmissive.r);
	glMaterialf (GL_BACK, GL_SHININESS,	 _backShininess);
}

//-------------------------------------
bool CMaterial::FrontIsTransparent()
//-------------------------------------
{
	if (_frontDiffuse.a < 1.0f || _backDiffuse.a < 1.0f)
		return true;
	else
		return false;
}

/*
//-------------------------------------
ostream& operator << (ostream& lhs, const CMaterial& rhs)
//-------------------------------------
{
	lhs << "\\begin{material}" << endl;

	lhs << rhs._frontAmbient << " & "
		<< rhs._frontDiffuse << " & "
		<< rhs._frontSpecular << " & "
		<< rhs._frontEmissive << " & "
		<< rhs._frontShininess << " \\\\" << endl;

	lhs << rhs._backAmbient << " & "
		<< rhs._backDiffuse << " & "
		<< rhs._backSpecular << " & "
		<< rhs._backEmissive << " & "
		<< rhs._backShininess << " %" << endl;

	lhs << "\\end{material}" << endl;

	return lhs;
}

//-------------------------------------
istream& operator >> (istream& lhs, CMaterial& rhs)
//-------------------------------------
{
	string aux;
	lhs >> aux;
	if (aux != "\\begin{material}")
		throw Exception("operator >> : Wrong material.");

	lhs >> rhs._frontAmbient >> aux;
	if (aux != "&")
		throw Exception("operator >> : Wrong material.");
	lhs >> rhs._frontDiffuse >> aux;
	if (aux != "&")
		throw Exception("operator >> : Wrong material.");
	lhs >> rhs._frontSpecular >> aux;
	if (aux != "&")
		throw Exception("operator >> : Wrong material.");
	lhs >> rhs._frontEmissive >> aux;
	if (aux != "&")
		throw Exception("operator >> : Wrong material.");
	lhs >> rhs._frontShininess >> aux;
	if (aux != "\\\\")
		throw Exception("operator >> : Wrong material.");

	lhs >> rhs._backAmbient >> aux;
	if (aux != "&")
		throw Exception("operator >> : Wrong material.");
	lhs >> rhs._backDiffuse >> aux;
	if (aux != "&")
		throw Exception("operator >> : Wrong material.");
	lhs >> rhs._backSpecular >> aux;
	if (aux != "&")
		throw Exception("operator >> : Wrong material.");
	lhs >> rhs._backEmissive >> aux;
	if (aux != "&")
		throw Exception("operator >> : Wrong material.");
	lhs >> rhs._backShininess >> aux;
	if (aux != "%")
		throw Exception("operator >> : Wrong material.");
	lhs >> aux;
	if (aux != "\\end{material}")
		throw Exception("operator >> : Wrong material.");
	return lhs;
}*/


CMaterial 

MatFBOrange = CMaterial(
				CColor(0.8, 0.6, 0.0, 0.2), 
				CColor(1.0, 0.8, 0.0, 0.3), 
				CColor(1.0, 1.0, 1.0, 0.3),
				CColor(0.0, 0.0, 0.0, 0.0), 
				128.0,
				CColor(0.8, 0.6, 0.0, 0.2), 
				CColor(1.0, 0.8, 0.0, 0.3), 
				CColor(1.0, 1.0, 1.0, 0.3),
				CColor(0.0, 0.0, 0.0, 0.0), 
				128.0),

MatFBBlue = CMaterial(
				CColor(0.0, 0.0, 0.6, 0.2), 
				CColor(0.3, 0.3, 0.5, 0.3), 
				CColor(0.2, 0.2, 0.2, 0.3),
				CColor(0.2, 0.2, 0.2, 0.2),
				10.0,
				CColor(0.0, 0.0, 0.6, 0.2), 
				CColor(0.2, 0.2, 0.3, 0.3), 
				CColor(0.2, 0.2, 0.2, 0.3),
				CColor(0.2, 0.2, 0.2, 0.2),
				10.0),
						  
MatFBBlue2 = CMaterial(
				CColor(0.1, 0.1, 0.5, 0.6), 
				CColor(0.1, 0.1, 1.0, 0.6), 
				CColor(1.0, 1.0, 1.0, 0.8),
				CColor(0.0, 0.0, 0.0, 0.0),
				128.0,
				CColor(0.1, 0.1, 0.5, 0.6), 
				CColor(0.1, 0.1, 1.0, 0.6), 
				CColor(1.0, 1.0, 1.0, 0.8),
				CColor(0.0, 0.0, 0.0, 0.0),
				128.0),

MatFBRed = CMaterial(
				CColor(0.8, 0.0, 0.0, 0.3), 
				CColor(1.0, 0.0, 0.0, 0.4), 
				CColor(1.0, 1.0, 1.0, 0.4),
				CColor(0.0, 0.0, 0.0, 0.0),
				128.0,
				CColor(0.8, 0.0, 0.0, 0.3), 
				CColor(1.0, 0.0, 0.0, 0.4), 
				CColor(1.0, 1.0, 1.0, 0.4),
				CColor(0.0, 0.0, 0.0, 0.0),
				128.0),

MatFBRed2 = CMaterial(
				CColor(0.8, 0.0, 0.0, 0.8), 
				CColor(1.0, 0.0, 0.0, 0.8), 
				CColor(1.0, 1.0, 1.0, 0.9),
				CColor(0.0, 0.0, 0.0, 0.0),
				128.0,
				CColor(0.8, 0.0, 0.0, 0.8), 
				CColor(1.0, 0.0, 0.0, 0.8), 
				CColor(1.0, 1.0, 1.0, 0.9),
				CColor(0.0, 0.0, 0.0, 0.0),
				128.0),

MatFBGreen = CMaterial(
				CColor(0.1, 0.4, 0.1, 0.6), 
				CColor(0.1, 0.8, 0.5, 0.6), 
				CColor(1.0, 1.0, 1.0, 0.8),
				CColor(0.0, 0.0, 0.0, 0.0),
				128.0,
				CColor(0.1, 0.4, 0.1, 0.6), 
				CColor(0.1, 0.8, 0.5, 0.6), 
				CColor(1.0, 1.0, 1.0, 0.8),
				CColor(0.0, 0.0, 0.0, 0.0),
				128.0),

MatFBGreen1 = CMaterial(
				CColor(0.3, 0.4, 0.3, 0.5), 
				CColor(0.3, 0.6, 0.5, 0.0), 
				CColor(0.2, 0.2, 0.2, 0.8),
				CColor(0.0, 0.0, 0.0, 0.0),
				128.0,
				CColor(0.3, 0.4, 0.3, 0.5), 
				CColor(0.3, 0.6, 0.5, 0.0), 
				CColor(0.2, 0.2, 0.2, 0.8),
				CColor(0.0, 0.0, 0.0, 0.0),
				0.0),

MatFBGreen2 = CMaterial(
				CColor(0.3, 0.4, 0.3, 0.5), 
				CColor(0.0, 0.0, 0.0, 0.0), 
				CColor(0.0, 0.0, 0.0, 0.0), 
				CColor(0.0, 0.0, 0.0, 0.0),
				128.0,
				CColor(0.3, 0.4, 0.3, 0.5), 
				CColor(0.0, 0.0, 0.0, 0.0), 
				CColor(0.0, 0.0, 0.0, 0.0), 
				CColor(0.0, 0.0, 0.0, 0.0),
				0.0),

MatFBGrey = CMaterial(
				CColor(0.3, 0.3, 0.3, 1.0), 
				CColor(0.6, 0.6, 0.6, 1.0), 
				CColor(1.0, 1.0, 1.0, 1.0),
				CColor(0.0, 0.0, 0.0, 0.0),
				128.0,
				CColor(0.3, 0.3, 0.3, 1.0), 
				CColor(0.6, 0.6, 0.6, 1.0), 
				CColor(1.0, 1.0, 1.0, 1.0),
				CColor(0.0, 0.0, 0.0, 0.0),
				128.0),

MatFBGrey2 = CMaterial(
				CColor(0.8, 0.8, 0.8, 0.4), 
				CColor(0.9, 0.9, 0.9, 0.4), 
				CColor(1.0, 1.0, 1.0, 0.4),
				CColor(0.0, 0.0, 0.0, 0.0),
				50.0,
				CColor(0.8, 0.8, 0.8, 0.4), 
				CColor(0.9, 0.9, 0.9, 0.4), 
				CColor(1.0, 1.0, 1.0, 0.4),
				CColor(0.0, 0.0, 0.0, 0.0),
				50.0),

MatFBViolet = CMaterial(
				CColor(0.8, 0.0, 0.4, 0.4), 
				CColor(0.9, 0.0, 0.5, 0.6), 
				CColor(1.0, 1.0, 1.0, 0.6),
				CColor(0.0, 0.0, 0.0, 0.0),
				128.0,
				CColor(0.8, 0.0, 0.4, 0.4), 
				CColor(0.9, 0.0, 0.5, 0.6), 
				CColor(1.0, 1.0, 1.0, 0.6),
				CColor(0.0, 0.0, 0.0, 0.0),
				128.0),

MatFBPink = CMaterial(
				CColor(0.95703125, 0.59375, 0.61328125, 0.4), 
				CColor(0.95703125, 0.59375, 0.61328125, 0.6), 
				CColor(1.0, 1.0, 1.0, 0.8),
				CColor(0.0, 0.0, 0.0, 0.0),
				128.0,
				CColor(0.95703125, 0.59375, 0.61328125, 0.4), 
				CColor(0.95703125, 0.59375, 0.61328125, 0.6), 
				CColor(1.0, 1.0, 1.0, 0.8),
				CColor(0.0, 0.0, 0.0, 0.0),
				128.0),				

MatFBWhite = CMaterial(
				CColor(1.0, 1.0, 1.0, 1.0), 
				CColor(1.0, 1.0, 1.0, 1.0), 
				CColor(1.0, 1.0, 1.0, 1.0),
				CColor(0.0, 0.0, 0.0, 0.0),
				128.0,
				CColor(1.0, 1.0, 1.0, 1.0), 
				CColor(1.0, 1.0, 1.0, 1.0), 
				CColor(1.0, 1.0, 1.0, 1.0),
				CColor(0.0, 0.0, 0.0, 0.0),
				128.0),
				
MatFBBrass = CMaterial(
				CColor(0.329412, 0.223529, 0.027451, 0.4), 
				CColor(0.780392, 0.568627, 0.113725, 0.6), 
				CColor(0.992157, 0.941176, 0.807843, 0.8),
				CColor(0.0, 0.0, 0.0, 0.0),
				27.8974,
				CColor(0.329412, 0.223529, 0.027451, 0.4), 
				CColor(0.780392, 0.568627, 0.113725, 0.6), 
				CColor(0.992157, 0.941176, 0.807843, 0.8),
				CColor(0.0, 0.0, 0.0, 0.0),
				27.8974),

MatFBBronze = CMaterial(
				CColor(0.2125, 0.1275, 0.054, 0.4), 
				CColor(0.714, 0.4284, 0.18144, 0.6), 
				CColor(0.393548, 0.271906, 0.166721, 0.8),
				CColor(0.0, 0.0, 0.0, 0.0),
				25.6,
				CColor(0.2125, 0.1275, 0.054, 0.4), 
				CColor(0.714, 0.4284, 0.18144, 0.6), 
				CColor(0.393548, 0.271906, 0.166721, 0.8),
				CColor(0.0, 0.0, 0.0, 0.0),
				25.6),

MatFBPolishedBronze = CMaterial(
				CColor(0.25, 0.148, 0.06475, 0.4), 
				CColor(0.4, 0.2368, 0.1036, 0.6), 
				CColor(0.774597, 0.458561, 0.200621, 0.8),
				CColor(0.0, 0.0, 0.0, 0.0),
				76.8,
				CColor(0.25, 0.148, 0.06475, 0.4), 
				CColor(0.4, 0.2368, 0.1036, 0.6), 
				CColor(0.774597, 0.458561, 0.200621, 0.8),
				CColor(0.0, 0.0, 0.0, 0.0),
				76.8),

MatFBChrome = CMaterial(
				CColor(0.25, 0.25, 0.25, 0.4), 
				CColor(0.4, 0.4, 0.4, 0.6), 
				CColor(0.774597, 0.774597, 0.774597, 0.8),
				CColor(0.0, 0.0, 0.0, 0.0),
				76.8,
				CColor(0.25, 0.25, 0.25, 0.4), 
				CColor(0.4, 0.4, 0.4, 0.6), 
				CColor(0.774597, 0.774597, 0.774597, 0.8),
				CColor(0.0, 0.0, 0.0, 0.0),
				76.8),

MatFBCopper = CMaterial(
				CColor(0.19125, 0.0735, 0.0225, 0.4), 
				CColor(0.7038, 0.27048, 0.0828, 0.6), 
				CColor(0.256777, 0.137622, 0.086014, 0.8),
				CColor(0.0, 0.0, 0.0, 0.0),
				12.8,
				CColor(0.19125, 0.0735, 0.0225, 0.4), 
				CColor(0.7038, 0.27048, 0.0828, 0.6), 
				CColor(0.256777, 0.137622, 0.086014, 0.8),
				CColor(0.0, 0.0, 0.0, 0.0),
				12.8),

MatFBPolishedCopper = CMaterial(
				CColor(0.2295, 0.08825, 0.0275, 0.4), 
				CColor(0.5508, 0.2118, 0.066, 0.6), 
				CColor(0.580594, 0.223257, 0.0695701, 0.8),
				CColor(0.0, 0.0, 0.0, 0.0),
				51.2,
				CColor(0.2295, 0.08825, 0.0275, 0.4), 
				CColor(0.5508, 0.2118, 0.066, 0.6), 
				CColor(0.580594, 0.223257, 0.0695701, 0.8),
				CColor(0.0, 0.0, 0.0, 0.0),
				51.2),

MatFBGold = CMaterial(
				CColor(0.24725, 0.1995, 0.0745, 0.2), 
				CColor(0.75164, 0.60648, 0.22648, 0.3), 
				CColor(0.628281, 0.555802, 0.366065, 0.3),
				CColor(0.0, 0.0, 0.0, 0.0),
				51.2,
				CColor(0.24725, 0.1995, 0.0745, 0.2), 
				CColor(0.75164, 0.60648, 0.22648, 0.3), 
				CColor(0.628281, 0.555802, 0.366065, 0.3),
				CColor(0.0, 0.0, 0.0, 0.0),
				51.2),

MatFBPolishedGold = CMaterial(
				CColor(0.24725, 0.2245, 0.0645, 0.4), 
				CColor(0.34615, 0.3143, 0.0903, 0.6), 
				CColor(0.797357, 0.723991, 0.208006, 0.8),
				CColor(0.0, 0.0, 0.0, 0.0),
				83.2,
				CColor(0.24725, 0.2245, 0.0645, 0.4), 
				CColor(0.34615, 0.3143, 0.0903, 0.6), 
				CColor(0.797357, 0.723991, 0.208006, 0.8),
				CColor(0.0, 0.0, 0.0, 0.0),
				83.2),

MatFBPewter = CMaterial(
				CColor(0.105882, 0.058824, 0.113725, 0.4), 
				CColor(0.427451, 0.470588, 0.541176, 0.6), 
				CColor(0.333333, 0.333333, 0.521569, 0.8),
				CColor(0.0, 0.0, 0.0, 0.0),
				9.84615,
				CColor(0.105882, 0.058824, 0.113725, 0.4), 
				CColor(0.427451, 0.470588, 0.541176, 0.6), 
				CColor(0.333333, 0.333333, 0.521569, 0.8),
				CColor(0.0, 0.0, 0.0, 0.0),
				9.84615),
				
MatFBSilver = CMaterial(
				CColor(0.19225, 0.19225, 0.19225, 0.4), 
				CColor(0.50754, 0.50754, 0.50754, 0.6), 
				CColor(0.508273, 0.508273, 0.508273, 0.8),
				CColor(0.0, 0.0, 0.0, 0.0),
				10.0,
				CColor(0.19225, 0.19225, 0.19225, 0.4), 
				CColor(0.50754, 0.50754, 0.50754, 0.6), 
				CColor(0.508273, 0.508273, 0.508273, 0.8),
				CColor(0.0, 0.0, 0.0, 0.0),
				10.0),

MatFBSilver1 = CMaterial(
				CColor(0.19225, 0.19225, 0.29225, 0.4), 
				CColor(0.30754, 0.30754, 0.30754, 0.6), 
				CColor(0.508273, 0.508273, 0.508273, 0.8),
				CColor(0.0, 0.0, 0.0, 0.0),
				10.0,
				CColor(0.19225, 0.19225, 0.29225, 0.4), 
				CColor(0.30754, 0.30754, 0.30754, 0.6), 
				CColor(0.508273, 0.508273, 0.508273, 0.8),
				CColor(0.0, 0.0, 0.0, 0.0),
				10.0),
				
MatFBPolishedSilver = CMaterial(
				CColor(0.23125, 0.23125, 0.23125, 0.4), 
				CColor(0.2775, 0.2775, 0.2775, 0.6), 
				CColor(0.773911, 0.773911, 0.773911, 0.8),
				CColor(0.0, 0.0, 0.0, 0.0),
				89.6,
				CColor(0.23125, 0.23125, 0.23125, 0.4), 
				CColor(0.2775, 0.2775, 0.2775, 0.6), 
				CColor(0.773911, 0.773911, 0.773911, 0.8),
				CColor(0.0, 0.0, 0.0, 0.0),
				89.6),

MatFBEmerald = CMaterial(
				CColor(0.0215, 0.1745, 0.0215, 0.55), 
				CColor(0.07568, 0.61424, 0.07568, 0.55), 
				CColor(0.633, 0.727811, 0.633, 0.55),
				CColor(0.0, 0.0, 0.0, 0.0),
				76.8,
				CColor(0.0215, 0.1745, 0.0215, 0.55), 
				CColor(0.07568, 0.61424, 0.07568, 0.55), 
				CColor(0.633, 0.727811, 0.633, 0.55),
				CColor(0.0, 0.0, 0.0, 0.0),
				76.8),

MatFBJade = CMaterial(
				CColor(0.135, 0.2225, 0.1575, 0.95), 
				CColor(0.54, 0.89, 0.63, 0.95), 
				CColor(0.316228, 0.316228, 0.316228, 0.95),
				CColor(0.0, 0.0, 0.0, 0.0),
				12.8,
				CColor(0.135, 0.2225, 0.1575, 0.95), 
				CColor(0.54, 0.89, 0.63, 0.95), 
				CColor(0.316228, 0.316228, 0.316228, 0.95),
				CColor(0.0, 0.0, 0.0, 0.0),
				12.8),

MatFBObsidian = CMaterial(
				CColor(0.05375, 0.05, 0.06625, 0.82), 
				CColor(0.18275, 0.17, 0.22525, 0.82), 
				CColor(0.332741, 0.328634, 0.346435, 0.82),
				CColor(0.0, 0.0, 0.0, 0.0),
				38.4,
				CColor(0.05375, 0.05, 0.06625, 0.82), 
				CColor(0.18275, 0.17, 0.22525, 0.82), 
				CColor(0.332741, 0.328634, 0.346435, 0.82),
				CColor(0.0, 0.0, 0.0, 0.0),
				38.4),

MatFBPearl = CMaterial(
				CColor(0.25, 0.20725, 0.20725, 0.922), 
				CColor(1.0, 0.829, 0.829, 0.922), 
				CColor(0.296648, 0.296648, 0.296648, 0.922),
				CColor(0.0, 0.0, 0.0, 0.0),
				11.264,
				CColor(0.25, 0.20725, 0.20725, 0.922), 
				CColor(1.0, 0.829, 0.829, 0.922), 
				CColor(0.296648, 0.296648, 0.296648, 0.922),
				CColor(0.0, 0.0, 0.0, 0.0),
				11.264),

MatFBRuby = CMaterial(
				CColor(0.1745, 0.01175, 0.01175, 0.455), 
				CColor(0.61424, 0.04136, 0.04136, 0.555), 
				CColor(0.727811, 0.626959, 0.626959, 0.555),
				CColor(0.0, 0.0, 0.0, 0.0),
				76.8,
				CColor(0.1745, 0.01175, 0.01175, 0.455), 
				CColor(0.61424, 0.04136, 0.04136, 0.555), 
				CColor(0.727811, 0.626959, 0.626959, 0.555),
				CColor(0.0, 0.0, 0.0, 0.0),
				76.8),
				
MatFBTurquoise = CMaterial(
				CColor(0.1, 0.18725, 0.1745, 0.2), 
				CColor(0.396, 0.74151, 0.69102, 0.3), 
				CColor(0.297254, 0.30829, 0.306678, 0.3),
				CColor(0.0, 0.0, 0.0, 0.0),
				12.8,
				CColor(0.1, 0.18725, 0.1745, 0.2), 
				CColor(0.396, 0.74151, 0.69102, 0.3), 
				CColor(0.297254, 0.30829, 0.306678, 0.3),
				CColor(0.0, 0.0, 0.0, 0.0),
				12.8),

MatFBBlackPlastique = CMaterial(
				CColor(0.0, 0.0, 0.0, 0.4), 
				CColor(0.01, 0.01, 0.01, 0.6), 
				CColor(0.50, 0.50, 0.50, 0.8),
				CColor(0.0, 0.0, 0.0, 0.0),
				32,
				CColor(0.0, 0.0, 0.0, 0.4), 
				CColor(0.01, 0.01, 0.01, 0.6), 
				CColor(0.50, 0.50, 0.50, 0.8),
				CColor(0.0, 0.0, 0.0, 0.0),
				32),

MatFBBlackRubber = CMaterial(
				CColor(0.0, 0.0, 0.0, 0.4), 
				CColor(0.01, 0.01, 0.01, 0.6), 
				CColor(0.50, 0.50, 0.50, 0.8),
				CColor(0.0, 0.0, 0.0, 0.0),
				32,
				CColor(0.02, 0.02, 0.02, 0.4), 
				CColor(0.01, 0.01, 0.01, 0.6), 
				CColor(0.4, 0.4, 0.4, 0.8),
				CColor(0.0, 0.0, 0.0, 0.0),
				10);