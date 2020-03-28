
#include "stdafx.h"
#include "Shaders.h"
#include "Exceptions.h"

#include <fstream>

using namespace std;

//------------------------------------
CShader::CShader():
_vs(0), _fs(0), _prog(0),
_vertCompiled(0), _fragCompiled(0),
_linked(0), 
_vs_source(0),
_fs_source(0)
//------------------------------------
{
	GLint major, minor;
    const char* verstr = (const char*)glGetString(GL_VERSION);
    if( (verstr == 0) || (sscanf_s( verstr, "%d.%d", &major, &minor ) != 2) )
    {
        major = minor = 0;
		cerr << "Invalid GL_VERSION format." << endl;
    }
    if (major < 2)
	{
		if (!(GLEW_ARB_vertex_shader && GLEW_ARB_fragment_shader))
			throw Exception("Support for OpenGL ARB Extension or OpenGL 2.0 is required.");
		#define  _GREY_
	}
	else
	{
		#define _ORANGE_
	}
}

//------------------------------------
// returns true if an OpenGL error occurred, false otherwise.
//------------------------------------
bool CShader::PrintOpenGLError(const char *fileName, int line)
//------------------------------------
{
    GLenum glErr;
    bool result = false;

    glErr = glGetError();
	cout << "\\begin{glErrors - block}" << endl;
    while (glErr != GL_NO_ERROR)
    {
		cout << "glError in file " << fileName 
			 << " at line " << line << ": " << endl 
			 << gluErrorString(glErr) << endl;
        result = true;
        glErr = glGetError();
    }
	cout << "\\end{glErrors - block}" << endl << endl;
    return result;
}

//------------------------------------
void CShader::PrintVertexShaderInfoLog()
//------------------------------------
{
    int infologLength = 0;
    int charsWritten  = 0;
    GLchar *infoLog = 0;

	// check for OpenGL errors
	// PrintOpenGLError(__FILE__, __LINE__);

	#ifdef _ORANGE_
		glGetShaderiv(_vs, GL_INFO_LOG_LENGTH, &infologLength);
	#elif _GREY_
		glGetObjectParameterivARB(_vs, GL_OBJECT_INFO_LOG_LENGTH_ARB, &infologLength);
	#endif

    if (infologLength > 0)
    {
        infoLog = new GLchar[infologLength];
        if (!infoLog)
			throw Exception("CShader::PrintVertexShaderInfoLog - Could not allocate InfoLog buffer.");

		#ifdef _ORANGE_
			glGetShaderInfoLog(_vs, infologLength, &charsWritten, infoLog);
		#elif _GREY_
			glGetInfoLogARB(_vs, infologLength, &charsWritten, infoLog);
		#endif

		cout << "------------" << endl;
		cout << "\\begin{Vertex Shader InfoLog}" << endl << "id = " << _vs << ", name = " << _fs_name.c_str() << endl;
		cout <<  infoLog << endl;
		cout << "\\end{Vertex Shader InfoLog}" << endl;
		cout << "------------" << endl << endl;
		delete[] infoLog;
    }
	// check for OpenGL errors
	// PrintOpenGLError(__FILE__, __LINE__);
}

//------------------------------------
void CShader::PrintFragmentShaderInfoLog()
//------------------------------------
{
    int infologLength = 0;
    int charsWritten  = 0;
    GLchar *infoLog = 0;

	// check for OpenGL errors
	// PrintOpenGLError(__FILE__, __LINE__);
	#ifdef _ORANGE_
		glGetShaderiv(_fs, GL_INFO_LOG_LENGTH, &infologLength);
	#elif _GREY_
		glGetObjectParameterivARB(_fs, GL_OBJECT_INFO_LOG_LENGTH_ARB, &infologLength);
	#endif

    if (infologLength > 0)
    {
        infoLog = new GLchar[infologLength];
        if (!infoLog)
			throw Exception("CShader::PrintFragmentShaderInfoLog - Could not allocate InfoLog buffer.");

		#ifdef _ORANGE_
			glGetShaderInfoLog(_fs, infologLength, &charsWritten, infoLog);
		#define _GREY_
			glGetInfoLogARB(_fs, infologLength, &charsWritten, infoLog);
		#endif

		cout << "------------" << endl;
		cout << "\\begin{Fragment Shader InfoLog}" << endl<< "id = " << _fs << ", name = "  << _fs_name.c_str() << endl;
		cout <<  infoLog << endl;
		cout << "\\end{Vertex Shader InfoLog}" << endl;
		cout << "------------" << endl << endl;

		delete[] infoLog;
    }
	// check for OpenGL errors
	// PrintOpenGLError(__FILE__, __LINE__);
}

//------------------------------------
void CShader::PrintProgramInfoLog()
//------------------------------------
{
    int infologLength = 0;
    int charsWritten  = 0;
    GLchar *infoLog = 0;

	// check for OpenGL errors
	// PrintOpenGLError(__FILE__, __LINE__);
	#ifdef _ORANGE_
		glGetProgramiv(_prog, GL_INFO_LOG_LENGTH, &infologLength);
	#elif _GREY_
		glGetObjectParameterivARB(_prog, GL_OBJECT_INFO_LOG_LENGTH_ARB, &infologLength);
	#endif

    if (infologLength > 0)
    {
        infoLog = new GLchar[infologLength];
        if (!infoLog)
			throw Exception("CShader::PrintProgramInfoLog - Could not allocate InfoLog buffer.");

		#ifdef _ORANGE_
			glGetProgramInfoLog(_prog, infologLength, &charsWritten, infoLog);
		#elif _GREY_
			glGetInfoLogARB(_prog, infologLength, &charsWritten, infoLog);
		#endif
		cout << "------------" << endl;
		cout << "\\begin{Program InfoLog}" << endl<< "id = " << _prog << endl;
		cout <<  infoLog << endl;
		cout << "\\end{Program InfoLog}" << endl;
		cout << "------------" << endl << endl;
		delete[] infoLog;
    }
	// check for OpenGL errors
	// PrintOpenGLError(__FILE__, __LINE__);
}

//------------------------------------
void CShader::PrintValidateInfoLog()
//------------------------------------
{
	int status = false;

	// check for OpenGL errors
	// PrintOpenGLError(__FILE__, __LINE__);
	#ifdef _ORANGE_
		glGetProgramiv(_prog, GL_VALIDATE_STATUS, &status);
	#elif _GREY_
		glGetObjectParameterivARB(_prog, GL_OBJECT_VALIDATE_STATUS_ARB, &status);
	#endif

	cout << "------------" << endl;
	cout << "\\begin{Program Validate InfoLog}" << endl<< "id = " << _prog << endl;
	cout <<  (status? "Validated." : "Not validated.") << endl;
	cout << "\\end{Program Validate InfoLog}" << endl;
	cout << "------------" << endl << endl;

	// check for OpenGL errors
	// PrintOpenGLError(__FILE__, __LINE__);}
}

//------------------------------------
GLint CShader::GetUniformVariableLocation(const GLchar *name)
//------------------------------------
{
	GLint loc;
	#ifdef _ORANGE_
		loc = glGetUniformLocation(_prog, name);
	#elif _GREY_
		loc = glGetUniformLocationARB(_prog, name);
	#endif
	if (loc == -1)
	{
		std::string reason = "No such uniform named: ";
		reason += name;
		cout << reason << endl;
		// check for OpenGL errors
		// PrintOpenGLError(__FILE__, __LINE__);
	}
	return loc;
}

//------------------------------------
int CShader::InstallShaders(const std::wstring vertexShaderFile, const std::wstring fragmentShaderFile)
//------------------------------------
{
	//====================================
	// loading source codes into the shader objects
	//====================================

		//------------------------------------
		_vs_name = vertexShaderFile;
		_fs_name = fragmentShaderFile;
		//------------------------------------

		//------------------------------------
		std::fstream fvs, ffs;
		fvs.open(vertexShaderFile.c_str(), ios_base::in);
		if(!fvs.is_open())
			return 0;
		ffs.open(fragmentShaderFile.c_str(), ios_base::in);
		if(!ffs.is_open())
		{
			fvs.close();
			return 0;
		}
		std::string svs = "", sfs = "", aux;
		//------------------------------------

		//------------------------------------
		cout << "-----------------------" << endl;
		cout << "Source of vertex shader" << endl;
		cout << "-----------------------" << endl;
		while (!fvs.eof())
		{
			getline(fvs, aux, '\n');
			svs += aux + '\n';
			cout << aux << endl;
		}
		fvs.close();
		_source.push_back(svs);
		//------------------------------------

		//------------------------------------
		cout << "-------------------------" << endl;
		cout << "Source of fragment shader" << endl;
		cout << "-------------------------" << endl;
		while (!ffs.eof())
		{
			getline(ffs, aux, '\n');
			sfs += aux + '\n';
			cout << aux << endl;
		}
		ffs.close();
		_source.push_back(sfs);
		//------------------------------------

	//====================================

	//====================================
	// creating to empty shader objects
	//====================================

		#ifdef _ORANGE_
			_vs = glCreateShader(GL_VERTEX_SHADER);
			_fs = glCreateShader(GL_FRAGMENT_SHADER);
		#elif _GREY_
			_vs = glCreateShaderObjectARB(GL_VERTEX_SHADER_ARB);
			_fs = glCreateShaderObjectARB(GL_FRAGMENT_SHADER_ARB);
		#endif

	//====================================

	//====================================
	// setting the source codes for the shaders
	//====================================

		//------------------------------------
		const GLchar *pVS = &_source[0][0],
					 *pFS = &_source[1][0];
		//------------------------------------

		//------------------------------------
		#ifdef _ORANGE_
			glShaderSource(_vs, 1, &pVS, NULL);
			glShaderSource(_fs, 1, &pFS, NULL);
		#elif _GREY_
			glShaderSourceARB(_vs, 1, &pVS, NULL);
			glShaderSourceARB(_fs, 1, &pFS, NULL);
		#endif
		//------------------------------------

	//====================================

	//====================================
	// compiling the vertex shader
	//====================================

		// check for OpenGL errors
		// PrintOpenGLError(__FILE__, __LINE__);

		//------------------------------------
		#ifdef _ORANGE_
			glCompileShader(_vs);
			glGetShaderiv(_vs, GL_COMPILE_STATUS, &_vertCompiled);
		#elif _GREY_
			glCompileShaderARB(_vs);
			glGetObjectParameterivARB(_vs, GL_OBJECT_COMPILE_STATUS_ARB, &_vertCompiled);
		#endif
		//------------------------------------

		//------------------------------------
		// printing vertex shader info log
		//------------------------------------
		PrintVertexShaderInfoLog();
		//------------------------------------

	//====================================

	//====================================
	// compiling the fragment shader
	//====================================

		// check for OpenGL errors
		// PrintOpenGLError(__FILE__, __LINE__);

		//------------------------------------
		#ifdef _ORANGE_
			glCompileShader(_fs);
			glGetShaderiv(_fs, GL_COMPILE_STATUS, &_fragCompiled);
		#elif _GREY_
			glCompileShaderARB(_fs);
			glGetObjectParameterivARB(_fs, GL_OBJECT_COMPILE_STATUS_ARB, &_fragCompiled);
		#endif
		//------------------------------------

		//------------------------------------
		// printing fragment shader info log
		//------------------------------------
		PrintFragmentShaderInfoLog();
		//------------------------------------

	//====================================

	//====================================

		if (!_vertCompiled || !_fragCompiled)
			return 0;

	//====================================

	//====================================
	// creating the program object
	//====================================

		//------------------------------------
		#ifdef _ORANGE_
			_prog = glCreateProgram();
		#elif _GREY_
			_prog = glCreateProgramObjectARB();
		#endif
		//------------------------------------

	//====================================

	//====================================
	// attaching the vertex and fragment shaders to the program object
	//====================================

		//------------------------------------
		#ifdef _ORANGE_
			glAttachShader(_prog, _vs);
			glAttachShader(_prog, _fs);
		#elif _GREY_
			glAttachObjectARB(_prog, _vs);
			glAttachObjectARB(_prog, _fs);
		#endif
		//------------------------------------

	//====================================

	//====================================
	// linking the program
	//====================================

		// check for OpenGL errors
		// PrintOpenGLError(__FILE__, __LINE__);

		//------------------------------------
		#ifdef _ORANGE_
			glLinkProgram(_prog);
			glGetProgramiv(_prog, GL_LINK_STATUS, &_linked);
		#elif _GREY_
			glLinkProgramARB(_prog);
			glGetObjectParameterivARB(_prog, GL_OBJECT_LINK_STATUS_ARB, &_linked);
		#endif
		//------------------------------------

		//------------------------------------
		// printing program info log
		//------------------------------------
		PrintProgramInfoLog();
		//------------------------------------

		//------------------------------------
		if (!_linked)
			return 0;
		//------------------------------------

	//====================================

	//====================================
	// flag shaders for deletion
	//====================================

		//------------------------------------
		#ifdef _ORANGE_
			glDeleteShader(_vs);
			glDeleteShader(_fs);
		#elif _GREY_
			glDeleteObjectARB(_vs);
			glDeleteObjectARB(_fs);
		#endif
		//------------------------------------

	//====================================

	return 1;
}

//------------------------------------
void CShader::Apply()
//------------------------------------
{
	if (_vertCompiled && _fragCompiled && _linked)
	{
		#ifdef _ORANGE_
			glUseProgram(_prog);
			glValidateProgram(_prog);
		#elif _GREY_
			glUseProgramObjectARB(_prog);
			glValidateProgramARB(_prog);
		#endif
		PrintValidateInfoLog();
	}
}

//------------------------------------
bool CShader::SetUniformVariable1i(
	const GLchar *name,
	int   parameter)
//------------------------------------
{
	if (!_prog) return false;
	GLint location = GetUniformVariableLocation(name);
	if (location == -1)
		return false;
	#ifdef _ORANGE_
		glUniform1i(location, parameter);
	#elif _GREY_
		glUniform1iARB(location, parameter);
	#endif
	return true;
}


//------------------------------------
bool CShader::SetUniformVariable2f(
	const GLchar *name, 
	float parameter1,
	float parameter2)
//------------------------------------
{
	if (_prog)
	{
		GLint location = GetUniformVariableLocation(name);
		if (location == -1)
			return false;
		#ifdef _ORANGE_
			glUniform2f(location, parameter1, parameter2);
		#elif _GREY_
			glUniform2fARB(location, parameter1, parameter2);
		#endif
		return true;
	}
	return false;
}

//------------------------------------
bool CShader::SetUniformVariable3f(
	const GLchar *name, 
	float parameter1,
	float parameter2,
	float parameter3)
//------------------------------------
{
	if (_prog)
	{
		GLint location = GetUniformVariableLocation(name);
		if (location == -1)
			return false;
		#ifdef _ORANGE_
			glUniform3f(location, parameter1, parameter2, parameter3);
		#elif _GREY_
			glUniform3fARB(location, parameter1, parameter2, parameter3);
		#endif
		return true;
	}
	return false;
}

//------------------------------------
void CShader::Enable()
//------------------------------------
{
	if (_vertCompiled && _fragCompiled && _linked)
	{
		#ifdef _ORANGE_
			glUseProgram(_prog);
			glValidateProgram(_prog);
		#elif _GREY_
			glUseProgramObjectARB(_prog);
			glValidateProgramARB(_prog);
		#endif
		//PrintValidateInfoLog();
	}
}

//------------------------------------
void CShader::Disable()
//------------------------------------
{
	#ifdef _ORANGE_
		glUseProgram(0);
	#elif _GREY_
		glUseProgramObjectARB(0);
	#endif
}

//------------------------------------
CShader::~CShader()
//------------------------------------
{
	// all the attached shader objects
	// will be automatically detached, and,
	// because they are flagged for deletion,
	// they will be automatically deleted at
	// that time as well
	if (_prog)
	{
		#ifdef _ORANGE_
			glDeleteProgram(_prog);
		#elif _GREY_
			glDeleteObjectARB(_prog);
		#endif
	}
	_source.clear();
}
