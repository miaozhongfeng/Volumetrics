
#include "stdafx.h"
#include "Lights.h"
#include "Exceptions.h"

//-------------------------------------
CDirectionalLight::CDirectionalLight(
	GLenum			lightIndex,
	CHCoordinate	position, 
	const CColor	&ambientIntensity,
	const CColor	&diffuseIntensity,
	const CColor	&specularIntensity):
//-------------------------------------
	_lightIndex			(lightIndex),
	_position			(position),
	_ambientIntensity	(ambientIntensity),
	_diffuseIntensity	(diffuseIntensity),
	_specularIntensity	(specularIntensity)
{
	glLightfv(lightIndex, GL_POSITION, &_position.x);
	glLightfv(lightIndex, GL_AMBIENT,  &_ambientIntensity.r);
	glLightfv(lightIndex, GL_DIFFUSE,  &_diffuseIntensity.r);
	glLightfv(lightIndex, GL_SPECULAR, &_diffuseIntensity.r);
}

//------------------------------------
void CDirectionalLight::Enable()
//------------------------------------
{
	glEnable(_lightIndex);
}

//------------------------------------
void CDirectionalLight::Disable()
//------------------------------------
{
	glDisable(_lightIndex);
}


//------------------------------------
CPointLight::CPointLight(
	GLenum			lightIndex,
	CHCoordinate	position, 
	const CColor	&ambientIntensity,
	const CColor	&diffuseIntensity,
	const CColor	&specularIntensity,
	float			constantAttenuation,
	float			linearAttenuation,
	float			quadraticAttenuation):
//------------------------------------
	CDirectionalLight			(lightIndex, position, ambientIntensity, diffuseIntensity, specularIntensity),
	_constantAttenuation	(constantAttenuation),
	_linearAttenuation		(linearAttenuation),
	_quadraticAttenuation	(quadraticAttenuation)
{
	if (position.w == 0.0)
		throw Exception("CPointLight::CPointLight - Wrong position.");

	glLightf(_lightIndex, GL_SPOT_CUTOFF, 180.0);
	glLightf(_lightIndex, GL_CONSTANT_ATTENUATION,	_constantAttenuation);
	glLightf(_lightIndex, GL_LINEAR_ATTENUATION,	_linearAttenuation);
	glLightf(_lightIndex, GL_QUADRATIC_ATTENUATION, _quadraticAttenuation);

}

//------------------------------------
CSpotLight::CSpotLight(
	GLenum			lightIndex,
	CHCoordinate	position, 
	const CColor	&ambientIntensity,
	const CColor	&diffuseIntensity,
	const CColor	&specularIntensity,
	float			constantAttenuation,
	float			linearAttenuation,
	float			quadraticAttenuation,
	CHCoordinate	spotDirection,
	float			spotCutoff,
	float			spotExponent):
//------------------------------------
	CPointLight		(lightIndex, position, ambientIntensity, diffuseIntensity, specularIntensity, constantAttenuation, linearAttenuation, quadraticAttenuation),
	_spotDirection	(spotDirection), 
	_spotCutoff		(spotCutoff),
	_spotExponent	(spotExponent)
{
	if (position.w == 0.0)
		throw Exception("CSpotLight::CSpotLight - Wrong position.");

	if (_spotCutoff == 180.0)
		throw Exception("CSpotLight::CSpotLight - Wrong spot cutoff.");

	float spotDir[] = {_spotDirection.x, _spotDirection.y, _spotDirection.z, _spotDirection.w};
	glLightfv(_lightIndex, GL_SPOT_DIRECTION,	spotDir);
	glLightf (_lightIndex, GL_SPOT_CUTOFF,		_spotCutoff);
	glLightf (_lightIndex, GL_SPOT_EXPONENT,	_spotExponent);
}