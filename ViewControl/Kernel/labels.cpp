
#include "stdafx.h"
#include "Labels.h"
#include "Math.h"
#include "Quaternion.h"

#include <GL/glut.h>


using namespace std;

void RenderRotatableString(Vector3 pos, Vector3 dir, void *font, const string &label) 
{  
	glPushMatrix();

		int wid = 0;
		for(string::const_iterator i = label.begin(); i < label.end(); i++) 
			wid += glutStrokeWidth(font,*i);

		pos -= dir * wid * 0.00015;
		glTranslatef(pos.x, pos.y, pos.z);
		glScalef(0.0003, 0.0003, 0.0003);

		for(string::const_iterator i = label.begin(); i < label.end(); i++) 
			glutStrokeCharacter(font,*i);
	glPopMatrix();
}

void RenderBitmapString2D(float x, float y, void *font, const std::string &label)
{
	glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
		glLoadIdentity();

		glMatrixMode(GL_PROJECTION);
		glPushMatrix();
			glLoadIdentity();
			gluOrtho2D(-1, 1, 1, -1);

			glRasterPos2f(x, y);

			for(string::const_iterator i = label.begin(); i < label.end(); i++) 
				glutBitmapCharacter(font, *i);

		glMatrixMode(GL_PROJECTION);
		glPopMatrix();
	
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
}

void RenderBitmapString(float x, float y, float z, void *font, const string &label) 
{  
	glPushMatrix();
		glRasterPos3f(x, y, z);

		for(string::const_iterator i = label.begin(); i < label.end(); i++) 
			glutBitmapCharacter(font, *i);
	glPopMatrix();
}

void RenderBitmapString(float x, float y, float z, const float *model_view_matrix, void *font, const string &label)
{
	glPushMatrix();
		glLoadIdentity();
		glMultMatrixf(model_view_matrix);
		glRasterPos3f(x, y, z);
		for(string::const_iterator i = label.begin(); i < label.end(); i++) 
			glutBitmapCharacter(font, *i);
	glPopMatrix();
}

void RenderBitmapString(const float v[3], void *font, const string &label)
{
	glPushMatrix();
		glRasterPos3fv(v);
		for(string::const_iterator i = label.begin(); i < label.end(); i++) 
			glutBitmapCharacter(font, *i);
	glPopMatrix();
}

void RenderBitmapString(const float v[3], const float *model_view_matrix, void *font, const string &label)
{
	glPushMatrix();
		glLoadIdentity();
		glMultMatrixf(model_view_matrix);
		glRasterPos3fv(v);
		for(string::const_iterator i = label.begin(); i < label.end(); i++) 
			glutBitmapCharacter(font, *i);
	glPopMatrix();
}

void RenderBitmapString(const Vector3 &v, void *font, const string & label)
{
	glPushMatrix();
		glRasterPos3fv(&v.x);
		for(string::const_iterator i = label.begin(); i < label.end(); i++) 
			glutBitmapCharacter(font, *i);
	glPopMatrix();
}

void RenderBitmapString(const Vector3 &v, const float *model_view_matrix, void *font, const string &label)
{
	glPushMatrix();
		glLoadIdentity();
		glMultMatrixf(model_view_matrix);
		glRasterPos3fv(&v.x);
		for(string::const_iterator i = label.begin(); i < label.end(); i++) 
			glutBitmapCharacter(font, *i);
	glPopMatrix();
}

void RenderBitmapStringCentered(const Vector3 &v, void *font, const string & label)
{
	glPushMatrix();

		int wid = 0;
		for(string::const_iterator i = label.begin(); i < label.end(); i++) 
			wid += glutBitmapWidth(font,*i);

		//std::wostringstream str;
		//str<<"wid: "<<wid;
		//MessageBox(0, str.str().c_str(), L"Some", 0);

		//Vector3 vv(v);
		//glTranslatef(-0.1,0,0);
		//vv.x -= 0.1;
		glRasterPos3fv(&v.x);
		//glTranslatef(-100,0,0);
		//glRasterPos2i( - 1 , 0);

		for(string::const_iterator i = label.begin(); i < label.end(); i++) 
			glutBitmapCharacter(font, *i);

	glPopMatrix();
}
