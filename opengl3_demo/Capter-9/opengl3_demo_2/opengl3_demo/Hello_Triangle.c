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
    
    GLint samplerLoc;
    
    GLuint textureId;
    
    int numIndices;
    // 顶点/法线/绘制数组
    GLfloat *vertices;
    GLfloat *normals;
    GLuint *indices;
    
}UserData;

GLuint CreateSimpleTextureCubemap(void)
{
    GLuint textureId;
    GLubyte cubePixels[6][3] =
    {
        255, 0, 0,
        0, 255, 0,
        0, 0, 255,
        255, 255, 0,
        255, 0, 255,
        255, 255, 255
    };
    
    glGenTextures(1, &textureId);
    
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureId);
    
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, GL_RGB, 1, 1, 0,
                 GL_RGB, GL_UNSIGNED_BYTE, &cubePixels[0]);
    glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, GL_RGB, 1, 1, 0,
                 GL_RGB, GL_UNSIGNED_BYTE, &cubePixels[1]);
    
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, GL_RGB, 1, 1, 0,
                 GL_RGB, GL_UNSIGNED_BYTE, &cubePixels[2]);
    glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, GL_RGB, 1, 1, 0,
                 GL_RGB, GL_UNSIGNED_BYTE, &cubePixels[3]);
    
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, GL_RGB, 1, 1, 0,
                 GL_RGB, GL_UNSIGNED_BYTE, &cubePixels[4]);
    glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, GL_RGB, 1, 1, 0,
                 GL_RGB, GL_UNSIGNED_BYTE, &cubePixels[5]);
    
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    
    return textureId;
}


int Init(ESContext *esContext) {
    UserData *userData = esContext->userData;
    const char *vShaderStr = GetShaderFileStr("LightShader_V.glsl");
    const char * fShaderStr = GetShaderFileStr("LightShader_F.glsl");
    
    userData->program = esLoadProgram(vShaderStr, fShaderStr);
    userData->samplerLoc = glGetUniformLocation(userData->program, "s_texture");
    
    userData->textureId = CreateSimpleTextureCubemap();
    
    userData->numIndices = esGenSphere(20, 0.75f, &userData->vertices, &userData->normals, NULL, &userData->indices);
    
    glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
    return GL_TRUE;
}

void Draw(ESContext *esContext)
{
    UserData *userData = esContext->userData;
    glViewport(0, 0, esContext->width, esContext->height);
    
    glClear(GL_COLOR_BUFFER_BIT);
    
    glCullFace(GL_BACK);
    
    glEnable(GL_CULL_FACE);
    
    glUseProgram(userData->program);
    
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, userData->vertices);
    
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, userData->normals);
    
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, userData->textureId);
    
    glUniform1i(userData->samplerLoc, 0);
    
    glDrawElements(GL_TRIANGLES, userData->numIndices, GL_UNSIGNED_INT, userData->indices);
}

void ShutDown(ESContext *esContext)
{
    UserData *userData = esContext->userData;
    glDeleteTextures(1, &userData->textureId);
    glDeleteProgram(userData->program);
    free(userData->vertices);
    free(userData->normals);
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
    esRegisterShutdownFunc(esContext, ShutDown);
    
    return GL_TRUE;
}

