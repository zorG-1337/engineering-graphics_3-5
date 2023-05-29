#ifndef TECHNIQUE_H
#define	TECHNIQUE_H

#include <list>
#include <GL/glew.h>

class Technique
{
public:

    Technique();

    ~Technique();

    virtual bool Init();

    void Enable();
    
protected:

    bool AddShader(GLenum ShaderType, const char* pShaderText);

    bool Finalize();

    GLint GetUniformLocation(const char* pUniformName);
    
    GLint GetProgramParam(GLint param);
    
    GLuint m_shaderProg;    
    
private:

    typedef std::list<GLuint> ShaderObjList;
    ShaderObjList m_shaderObjList;
};

#define INVALID_UNIFORM_LOCATION 0xFFFFFFFF


#endif	/* TECHNIQUE_H */

