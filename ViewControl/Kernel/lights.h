#pragma once

#include <GL/glut.h>

#include "Colors.h"
#include "HCoordinates.h"

class CDirectionalLight
{
protected:

	GLenum			_lightIndex;
	CHCoordinate	_position;
	CColor			_ambientIntensity, _diffuseIntensity, _specularIntensity;

public:

	CDirectionalLight(
		GLenum			lightIndex,
		CHCoordinate	position, 
		const CColor	&ambientIntensity,
		const CColor	&diffuseIntensity,
		const CColor	&specularIntensity);

	void Enable();
	void Disable();
};

class CPointLight: public CDirectionalLight
{
protected:

	float _constantAttenuation,
		  _linearAttenuation,
		  _quadraticAttenuation;

public:

	CPointLight(
		GLenum			lightIndex,
		CHCoordinate	position, 
		const CColor	&ambientIntensity,
		const CColor	&diffuseIntensity,
		const CColor	&specularIntensity,
		float			constantAttenuation,
		float			linearAttenuation,
		float			quadraticAttenuation);

};

class CSpotLight: public CPointLight
{

private:

	CHCoordinate	_spotDirection;
	float			_spotCutoff, _spotExponent;

public:

	CSpotLight(
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
		float			spotExponent);
};
