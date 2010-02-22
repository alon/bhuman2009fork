/**
* @file Shader.cpp
* 
* Definition of opengl glsl shader class
*
* @author <A href="mailto:pachur@informatik.uni-bremen.de">Dennis Pachur</A>
*/

#include <cstring>
#include "Shader.h"
#include <OpenGL/GLHelper.h>
#include <sstream>
#include <fstream>

Shader::Shader(std::string& name_, GLenum& type_, std::string& filename)
: source(0), log(0), id(0), build(GL_FALSE)
{
  // create the shader
  name = name_;
  type = type_;
  file = filename;
}  

Shader::~Shader()
{
  destroy();
}

void Shader::destroy()
{
  if(log)
    delete[] log;
  if(source)
    delete[] source;
  
  if(id != 0 && glIsShader(id))
    glDeleteShader(id);

  source = 0;
  log = 0;
  id = 0;
  build = GL_FALSE;
}

bool Shader::create()
{
  build = GL_FALSE;

  // search for file
  std::stringstream filename,
                    sourcestr;
  filename << "Shader/";
  filename << file;
  std::ifstream infile;
  infile.open(filename.str().c_str() , std::ifstream::in );
  if(infile.is_open())
  {
    // read source code
    GLchar* testline = new GLchar[160];
    unsigned int extraChars = 0;
    while(infile.good())
    {
      infile.getline(testline, 160);
      sourcestr << testline;
      sourcestr << "\n";
      extraChars++;
    }
    delete[] testline;
    infile.close();
    unsigned int newsize = sourcestr.str().size() + extraChars;
    source = new GLchar[newsize];
    strcpy(source, sourcestr.str().c_str());
  }
  else
  {
    GLHelper::getGLH()->glPrintMarkedText(TextObject::WARNING_HINTS, name.c_str(), TextObject::WARNING, 5000,
      "WARNING: Shader \"%s\" has not been found.", name.c_str());
    log = new GLchar[100];
    std::stringstream logmessage;
    logmessage << "file \"";
    logmessage << filename;
    logmessage << "\" is missing\n";
    strcpy(log, logmessage.str().c_str());
    return false;
  }

  // create shader id
  id = glCreateShader(type);
  if( id == 0 )
  {
    GLHelper::getGLH()->glPrintMarkedText(TextObject::WARNING_HINTS, name.c_str(), TextObject::WARNING, 5000,
      "WARNING: Shader \"%s\" got no shader id.", name.c_str());
    log = new GLchar[100];
    strcpy(log, "shader got no id - buy a graphics card...\n");
    return false;
  }
  
  // provide the source
  glShaderSource(id, 1, (const GLchar**)&source, NULL);

  // compile it
  glCompileShader(id);

  // and get the errors
  GLint compiled;
  glGetShaderiv(id, GL_COMPILE_STATUS, &compiled);
  if(!compiled)
  {
    GLint length;
    glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
    log = new GLchar[length];
    glGetShaderInfoLog(id, length, &length, log);
    GLHelper::getGLH()->glPrintMarkedText(TextObject::WARNING_HINTS, name.c_str(), TextObject::WARNING, 5000,
      "WARNING: Compilation errors of Shader \"%s\" have been occured.", name.c_str());
    return false;
  }
  else
    build = GL_TRUE;
  return true;
}

GLuint Shader::getId()
{
  return id;
}

std::string Shader::getName()
{
  return name;
}

GLboolean Shader::usable()
{
  return build;
}

void Shader::printInfo(ShaderInfo info, GLuint posy)
{
  GLuint posx = 260;
  switch(info)
  {
  case SHADER_LOG:
    {
    if(!log)
    {
      glColor3f(GLfloat(0.3), GLfloat(1.0), GLfloat(0.3));
      GLHelper::getGLH()->glPrintf(false, posx, posy, "no log available");
      return;
    }
    glColor3f(GLfloat(1.0), GLfloat(0.3), GLfloat(0.3));
    std::string logstr(log);
    std::string message;
    for(int loc = logstr.find("\n", 0); loc != int(std::string::npos); )
    {
      message = logstr.substr(0, loc);
      GLHelper::getGLH()->glPrintf(false, posx, posy, "%s", message.c_str());
      logstr = logstr.substr(loc+1, logstr.size());
      loc = logstr.find("\n", 0);
      posy -= 20;
    }
    }
    break;
  case SHADER_SOURCE:
    {
      glColor3f(GLfloat(0.7), GLfloat(0.7), GLfloat(0.7));
      if(source)
      {
        std::string sourcestr(source);
        std::string message;
        for(int loc = sourcestr.find("\n", 0); loc != int(std::string::npos); )
        {
          message = sourcestr.substr(0, loc);
          GLHelper::getGLH()->glPrintf(false, posx, posy, "%s", message.c_str());
          sourcestr = sourcestr.substr(loc+1, sourcestr.size());
          loc = sourcestr.find("\n", 0);
          posy -= 20;
        }
      }
      else
        GLHelper::getGLH()->glPrintf(false, posx, posy, "source not loaded");
    }
    break;
  default:
    break;
  }
}
