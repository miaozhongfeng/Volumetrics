
#include "stdafx.h"
#include "Annotations.h"
#include "materials.h"
#include "labels.h"
#include "Quaternion.h"
#include "..\ViewCtrl.h"

CMaterial
MatLines = CMaterial(
				CColor(0.54/1.5, 0.89/1.5, 0.63/1.5, 0.95/1.5), 
				CColor(0.135*2, 0.2225*2, 0.1575*2, 0.95), 
				CColor(0.316228, 0.316228, 0.316228, 0.95),
				CColor(0.0, 0.0, 0.0, 0.0),
				12.8,
				CColor(0.54/1.5, 0.89/1.5, 0.63/1.5, 0.95/1.5), 
				CColor(0.135*2, 0.2225*2, 0.1575*2, 0.95), 
				CColor(0.316228, 0.316228, 0.316228, 0.95),
				CColor(0.0, 0.0, 0.0, 0.0),
				12.8);

CMaterial
MatDimLines = CMaterial(             
                         CColor(0.8, 0.6, 0.0, 0.5), 
                         CColor(1.0, 0.8, 0.0, 0.5), 
                         CColor(1.0, 1.0, 1.0, 0.5),
                         CColor(0.0, 0.0, 0.0, 0.0), 
                         128.0,
                         CColor(0.8, 0.6, 0.0, 0.5), 
                         CColor(1.0, 0.8, 0.0, 0.5), 
                         CColor(1.0, 1.0, 1.0, 0.5),
                         CColor(0.0, 0.0, 0.0, 0.0), 
                         128.0);

CColor lineColor(0.54/1.5, 0.89/1.5, 0.63/1.5, 0.95/1.5);
CColor dimLineColor(0.80, 0.53, 0.0);//0.8, 0.6, 0.0);
CColor dimLetterColor(0.95, 1.0, 0.38);

AnnotationChild::AnnotationChild(std::string val, size_t id):
    m_dlMesh(0),
    m_value(val),
    m_lengthValue(""),
    m_bDisplayLength(false),
    m_bDisplayValue(true),
    m_id(id)
{
}

AnnotationChild::~AnnotationChild()
{
	deleteDisplayList();
}

void AnnotationChild::deleteDisplayList()
{
	if (m_dlMesh)
	{
		glDeleteLists(m_dlMesh, 1);
		m_dlMesh = 0;
	}
}

void AnnotationChild::render()
{
	if (m_dlMesh)
		glCallList(m_dlMesh);
}
//--------------------------------------------------------
Leader::Leader(std::string val, Vector3 startPos, Vector3 dir, float len, size_t id):
AnnotationChild(val, id),
m_startPos(startPos),
m_direction(dir),
m_length(len)
{
	m_type = AT_LEADER;
	update();
}

void Leader::update()
{
	deleteDisplayList();

    if(!m_bDisplayLength && ! m_bDisplayValue)
        return;

	//Vector3 startPos(m_startPos);
	//Vector3 endPos(m_endPos);

	Vector3 startPos;//(m_startPos);
	Vector3 endPos;//(m_endPos);

	if(GetGlobalView()->getMesh())
	{
		startPos = GetGlobalView()->getMesh()->_worldMatrix.TransformAffine(m_startPos);
		endPos = startPos + m_direction * m_length;//GetGlobalView()->getMesh()->_worldMatrix.TransformAffine(m_endPos);
	}
	else
	{
		startPos = m_startPos;
		endPos = startPos + m_direction * m_length;;
	}

	Vector3 dir(endPos - startPos);
	float fLen = dir.Length();
	dir.Normalize();

	//startPos += dir * 0.05;
	//endPos -= dir * 0.05;
	
	m_dlMesh = glGenLists(1);
	glNewList(m_dlMesh, GL_COMPILE);
    
    glDisable(GL_LIGHTING);

	glPushMatrix();
		glPushMatrix();
			
           // MatLines.Apply();
            glColor3f(lineColor.r, lineColor.g, lineColor.b);

			glLineWidth(1.5);
			glBegin(GL_LINES);
				glNormal3f(1.0, 0.0, 0.0);
				glVertex3f(startPos.x, startPos.y, startPos.z);
				glNormal3f(1.0, 0.0, 0.0);
				glVertex3f(endPos.x, endPos.y, endPos.z);
			glEnd();
		glPopMatrix();

		//MatFBTurquoise.Apply();
        glColor3f(lineColor.r, lineColor.g, lineColor.b);//glColor3f(0.1, 0.18725, 0.1745);

		endPos += dir * 0.01;
        std::string str = "";
        if(m_bDisplayValue)
            str += m_value;
        if(m_bDisplayLength)
        {
            if(m_bDisplayValue)
                str += ":";
            str += m_lengthValue;
        }
		RenderBitmapStringCentered(endPos, GLUT_BITMAP_HELVETICA_18, str.c_str());
		//RenderRotatableString(center, dir, GLUT_STROKE_ROMAN, m_value.c_str());
		//RenderRotatableString(center, dir, GLUT_STROKE_ROMAN, toString(fLen*100));
	glPopMatrix();
    
    glEnable(GL_LIGHTING);
	
    glEndList();
}
//--------------------------------------------------------
DimLine::DimLine(std::string val, Vector3 startPos, Vector3 endPos, float offset):
AnnotationChild(val, -1),
m_startPos(startPos),
m_endPos(endPos),
m_offset(offset)
{
	m_type = AT_DIMLINE;

	if(m_startPos.x > m_endPos.x)
	{
		Vector3 swap = m_startPos;
		m_startPos = m_endPos;
		m_endPos = swap;
	}

	update();
}

std::string toString(Real val, unsigned short precision = 6, 
            unsigned short width = 0, char fill = ' ')
{
	std::ostringstream stream;
	stream.precision(precision);
	stream.width(width);
	stream.fill(fill);
	stream << val;
	return stream.str();
}

void DimLine::update()
{
	deleteDisplayList();

	Vector3 startPos, startPos_;//(m_startPos);
	Vector3 endPos, endPos_;//(m_endPos);

	if(GetGlobalView()->getMesh())
	{
		startPos = GetGlobalView()->getMesh()->_worldMatrix.TransformAffine(m_startPos);
		endPos = GetGlobalView()->getMesh()->_worldMatrix.TransformAffine(m_endPos);
	}
	else
	{
		startPos = m_startPos;
		endPos = m_endPos;
	}

	if(startPos.x > endPos.x)
	{
		Vector3 swap = startPos;
		startPos = endPos;
		endPos = swap;
	}

	startPos_ = startPos;
	endPos_ = endPos;

	// if always horizontal v1
	/*startPos.y = (startPos.y + endPos.y) * 0.5f;
	endPos.y = startPos.y;

	startPos.z = (startPos.z + endPos.z) * 0.5f;
	endPos.z = startPos.z;*/

	// ----- uncomment if horizontal needed
	// if always horizontal v2
	if(m_offset != 0)
	{
		endPos.y = startPos.y;
		endPos.z = startPos.z;
	}

	startPos.y += m_offset;
	endPos.y += m_offset;

	Vector3 _startPos(startPos);
	Vector3 _endPos(endPos);

	// end

	Vector3 dir(endPos - startPos);
	float fLen = dir.Length();
	dir.Normalize();

	startPos += dir * 0.05;
	endPos -= dir * 0.05;
	
	Matrix4 mat;
	// ----- comment if horizontal needed
	if(m_offset == 0)
	{
		Quaternion quat = dir.GetRotationTo(Vector3::UNIT_X);
		Matrix3 rot3x3;
		quat.ToRotationMatrix(rot3x3);
		mat = Matrix4::IDENTITY;
		mat = rot3x3;
		Matrix3 rot3x3inv;
		rot3x3inv = rot3x3.Inverse();

		startPos = startPos * rot3x3inv;
		endPos = endPos * rot3x3inv;
	}
	// ----- 

	dir = endPos - startPos;
	dir.Normalize();
	Vector3 center((endPos + startPos) / 2.f);

	m_dlMesh = glGenLists(1);
	glNewList(m_dlMesh, GL_COMPILE);
	glDisable(GL_LIGHTING);

	glPushMatrix();
		// ----- comment if horizontal needed
		if(m_offset == 0)
			glMultMatrixf(mat);
		// ----- 
		glPushMatrix();
        // Draw line connecting start and end points
			//MatDimLines.Apply();
            glColor3f(dimLineColor.r, dimLineColor.g, dimLineColor.b);

			glLineWidth(2.5);
			glBegin(GL_LINES);
				glNormal3f(1.0, 0.0, 0.0);
				glVertex3f(startPos.x, startPos.y, startPos.z);
				glNormal3f(1.0, 0.0, 0.0);
				glVertex3f(endPos.x, endPos.y, endPos.z);
			glEnd();

         // Draw offset lines
            //MatDimLines.Apply();
            glColor3f(dimLineColor.r, dimLineColor.g, dimLineColor.b);

            glLineWidth(2.0);
            glBegin(GL_LINES);
            glNormal3f(1.0, 0.0, 0.0);
            glVertex3f(_startPos.x, _startPos.y, _startPos.z);
            glNormal3f(1.0, 0.0, 0.0);
            glVertex3f(startPos_.x, startPos_.y, startPos_.z);

            glNormal3f(1.0, 0.0, 0.0);
            glVertex3f(_endPos.x, _endPos.y, _endPos.z);
            glNormal3f(1.0, 0.0, 0.0);
            glVertex3f(endPos_.x, endPos_.y, endPos_.z);
            glEnd();
         glPopMatrix();
         
         // Draw text
         glPushMatrix();
             double viewLength = GetGlobalView()->getFoomFactor();
             viewLength *= 0.5;

             glScalef(viewLength, viewLength, viewLength);

             glLineWidth(1.5);
             //MatDimLines.Apply();
             glColor3f(dimLetterColor.r, dimLetterColor.g, dimLetterColor.b);
             center.y += 0.015*viewLength;
             center /= viewLength;

             RenderRotatableString(center, dir, GLUT_STROKE_ROMAN, m_value.c_str());
             //RenderRotatableString(center, dir, GLUT_STROKE_ROMAN, toString(fLen*100));
         glPopMatrix();

         // Draw arrows
         glEnable(GL_LIGHTING);
         glPushMatrix();

            MatDimLines.Apply();
            //glColor3f(dimLineColor.r, dimLineColor.g, dimLineColor.b);

            glTranslatef(startPos.x, startPos.y, startPos.z);
            glRotatef(-90.0, 0.0, 1.0, 0.0);
            glutSolidCone(0.0100, 0.05, 10, 10); // 0.0125
            glPopMatrix();
            glPushMatrix();
            MatDimLines.Apply();
            //glColor3f(dimLineColor.r, dimLineColor.g, dimLineColor.b);

            glTranslatef(endPos.x, endPos.y, endPos.z);
            glRotatef(90.0, 0.0, 1.0, 0.0);
            glutSolidCone(0.0100, 0.05, 10, 10); // 0.0125

         glPopMatrix();

	glPopMatrix();

	glEndList();
}
//--------------------------------------------------------
AntlerAnnotation::AntlerAnnotation()
{
}

AntlerAnnotation::~AntlerAnnotation()
{
	clear();
}

void AntlerAnnotation::render(bool bLeader, bool bDimLine)
{
	std::vector<AnnotationChild*>::iterator ic = m_childs.begin();
	std::vector<AnnotationChild*>::iterator endc = m_childs.end();

	for(; ic!=endc; ic++)
		if( ((*ic)->getType() == AT_LEADER && bLeader) || ((*ic)->getType() == AT_DIMLINE && bDimLine) )
		(*ic)->render();
}

void AntlerAnnotation::addLeader(std::string value, Vector3 startPos, float len, Vector3 dir)
{
	Leader *leader = new Leader(value, startPos, dir, len, m_childs.size());
	m_childs.push_back(leader);
}

void AntlerAnnotation::addLeader(size_t id, std::string value, std::string lengthValue, Vector3 startPos, float len, Vector3 dir)
{
    Leader *leader = new Leader(value, startPos, dir, len, id);
    leader->setValue(lengthValue);
    m_childs.push_back(leader);
}
void AntlerAnnotation::addDimLine(std::string value, Vector3 startPos, Vector3 endPos, float offset)
{
	DimLine *dimLine = new DimLine(value, startPos, endPos, offset);
	m_childs.push_back(dimLine);
}

void AntlerAnnotation::addDimLine(float value, Vector3 startPos, Vector3 endPos, float offset)
{
	DimLine *dimLine = new DimLine(toString(value), startPos, endPos, offset);
	m_childs.push_back(dimLine);
}

void AntlerAnnotation::showLength(bool bShow, size_t id)
{
    for(size_t i = 0 ; i < m_childs.size(); ++i)
    {
        if(m_childs[i]->getId() == id)
        {
            m_childs[i]->displayLength(bShow);
            m_childs[i]->update();
            break;
        }
    }
}
void AntlerAnnotation::showAnnotations(bool bShow)
{
    for(size_t i = 0 ; i < m_childs.size(); ++i)
    {
        m_childs[i]->displayValue(bShow);
        m_childs[i]->update();
    }
}
void AntlerAnnotation::setLength(std::string lengthVal, size_t id)
{
    for(size_t i = 0 ; i < m_childs.size(); ++i)
    {
        if(m_childs[i]->getId() == id)
        {
            m_childs[i]->setValue(lengthVal);
            m_childs[i]->update();
            break;
        }
    }
}
void AntlerAnnotation::clearLengthInfo()
{
    for(size_t i = 0 ; i < m_childs.size(); ++i)
    {
        m_childs[i]->displayLength(false);
        m_childs[i]->update();
    }
}
void AntlerAnnotation::update()
{
    for(size_t i = 0 ; i < m_childs.size(); ++i)
        m_childs[i]->update();
}