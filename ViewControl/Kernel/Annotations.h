#ifndef ANNOTATIONS_H
#define ANNOTATIONS_H

#include "Kernel.h"
#include "Vector3.h"
#include "Matrix4.h"

enum AnnotationType
{
	AT_DIMLINE,
	AT_LEADER
};

class AnnotationChild
{
protected:
	GLuint	    m_dlMesh;
	std::string m_value;
    std::string m_lengthValue;
    bool        m_bDisplayLength;
    bool        m_bDisplayValue;
    size_t      m_id;
	AnnotationType	m_type;

public:
	AnnotationChild(std::string, size_t id);
	~AnnotationChild();

	virtual void update() = 0;
	virtual void render();
	void deleteDisplayList();

	const std::string &getString() const
	{
		return m_value;
	}
	void setString(std::string str)
	{
		m_value = str;
	}

    const std::string &getValue() const { return m_lengthValue; }
    void setValue(std::string str) { m_lengthValue = str; }
    void displayLength(bool bDisplay) { m_bDisplayLength = bDisplay; }
    void displayValue(bool bDisplay) { m_bDisplayValue = bDisplay; }

    size_t getId() { return m_id; }
	AnnotationType getType() { return m_type; }
};

class Leader: public AnnotationChild
{
private:
	Vector3 m_startPos;
	Vector3 m_direction;
	float m_length;

public:
	void update();

	Leader(std::string val, Vector3 startPos, Vector3 dir, float len, size_t id);
};

class DimLine: public AnnotationChild
{
private:
	Vector3 m_startPos;
	Vector3 m_endPos;
	float	m_offset;

public:
	void update();

	DimLine(std::string val, Vector3 startPos, Vector3 endPos, float offset = 0);
};

class AntlerAnnotation
{
public:
	std::vector<AnnotationChild*> m_childs;

    void update();
	void render(bool bLeader = true, bool bDimLine = true);
	void addLeader(std::string value, Vector3 startPos, float len = 0.5f, Vector3 dir = Vector3::UNIT_Y);
    void addLeader(size_t id, std::string value, std::string lengthValue, Vector3 startPos, float len = 0.5f, Vector3 dir = Vector3::UNIT_Y);
	void addDimLine(std::string value, Vector3 startPos, Vector3 endPos, float offset = 0);
	void addDimLine(float value, Vector3 startPos, Vector3 endPos, float offset = 0);

    void showAnnotations(bool bShow);
    void showLength(bool bShow, size_t id);
    void setLength(std::string lengthVal, size_t id);
    void clearLengthInfo();

	void removeLast()
	{
		if(!m_childs.empty())
		{
			SAFE_DELETE(*m_childs.rbegin());
			m_childs.pop_back();
		}
	}
	size_t size()
	{
		return m_childs.size();
	}
	void clear()
	{
		std::vector<AnnotationChild*>::iterator ic = m_childs.begin();
		std::vector<AnnotationChild*>::iterator endc = m_childs.end();

		for(; ic!=endc; ic++)
			SAFE_DELETE(*ic);

		m_childs.clear();
	}

	AntlerAnnotation();
	~AntlerAnnotation();
};

#endif
