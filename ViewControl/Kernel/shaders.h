
#pragma once

#include <string>
#include <vector>
#include <GL/glew.h>

class CShader
{
private:
	GLuint						_vs, _fs, _prog;				
	GLint						_vertCompiled, _fragCompiled;	
	GLint						_linked;
	GLchar						**_vs_source, **_fs_source;
	std::wstring				_vs_name, _fs_name;
	std::vector<std::string>	_source;

public:
	CShader();
	GLint GetUniformVariableLocation(const GLchar *name);
	int InstallShaders(const std::wstring vertexShaderFile, const std::wstring fragmentShaderFile);

	bool PrintOpenGLError(const char *fileName, int line);
	void PrintVertexShaderInfoLog();
	void PrintFragmentShaderInfoLog();
	void PrintProgramInfoLog();
	void PrintValidateInfoLog();

	bool SetUniformVariable1i(
		const GLchar *name,
		int   parameter);
	
	bool CShader::SetUniformVariable1f(
		const GLchar *name,
		float  parameter);

	bool SetUniformVariable2f(
		const GLchar *name, 
		float parameter1,
		float parameter2);

	bool SetUniformVariable3f(
		const GLchar *name, 
		float parameter1,
		float parameter2,
		float parameter3);

	void Apply();

	void Enable();
	void Disable();

	virtual ~CShader();
};
