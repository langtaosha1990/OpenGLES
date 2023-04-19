//
//  Hello_Triangle.c
//  opengl3_demo
//
//  Created by Gpf 郭 on 2022/10/8.
//

#include "Hello_Triangle.h"
#include "FileWrapper.h"
#include <math.h>

typedef struct {
    GLuint program;
    
    GLuint samplerLoc;
    GLuint textureId;
}UserData;


GLuint CreateSimpleTexture2D()
{
    GLuint textureId;
    GLubyte pixels[4 * 3] = {
        255, 0, 0,
        0, 255, 0,
        0, 0, 255,
        255, 255, 0
    };
    
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    
    glGenTextures(1, &textureId);
        
    glBindTexture(GL_TEXTURE_2D, textureId);
    
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 2, 2, 0, GL_RGB, GL_UNSIGNED_BYTE, pixels);
    
    //
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    return textureId;
}

int Init(ESContext *esContext) {
    UserData *userData = esContext->userData;
    const char *vShaderStr = GetShaderFileStr("LightShader_V.glsl");
    const char * fShaderStr = GetShaderFileStr("LightShader_F.glsl");
    
    userData->program = esLoadProgram(vShaderStr, fShaderStr);
    
    userData->samplerLoc = glGetUniformLocation(userData->program, "s_texture");
    
    userData->textureId = CreateSimpleTexture2D();
    
    glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
    return GL_TRUE;
}

void Draw(ESContext *esContext)
{
    UserData *userData = esContext->userData;
    GLfloat vVertices[] = { -0.5f,  0.5f, 0.0f,  // Position 0
                            0.0f,  0.0f,        // TexCoord 0
                            -0.5f, -0.5f, 0.0f,  // Position 1
                            0.0f,  1.0f,        // TexCoord 1
                            0.5f, -0.5f, 0.0f,  // Position 2
                            1.0f,  1.0f,        // TexCoord 2
                            0.5f,  0.5f, 0.0f,  // Position 3
                            1.0f,  0.0f         // TexCoord 3
                            };
    
    GLushort indices[] = {0, 1, 2, 0, 2, 3};
    
    glViewport(0, 0, esContext->width, esContext->height);
    
    glClear(GL_COLOR_BUFFER_BIT);
    
    glUseProgram(userData->program);
    
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), vVertices);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), &vVertices[3]);
    
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, userData->textureId);
    
    glUniform1f(userData->samplerLoc, 0);
    
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, indices);
}

void ShutDown(ESContext *esContext)
{
    UserData *userData = esContext->userData;
    glDeleteTextures(1, &userData->textureId);
    glDeleteProgram(userData->program);
}

int esMain ( ESContext *esContext )
{
    esContext->userData = malloc(sizeof(UserData));
    
    esCreateWindow(esContext, "纹理", esContext->width, esContext->height, ES_WINDOW_RGB | ES_WINDOW_DEPTH);
    if(!Init(esContext)) {
        return GL_FALSE;
    }
    
    //
    esRegisterDrawFunc(esContext, Draw);
//    esRegisterUpdateFunc(esContext, Update);
    esRegisterShutdownFunc(esContext, ShutDown);
    
    return GL_TRUE;
}

