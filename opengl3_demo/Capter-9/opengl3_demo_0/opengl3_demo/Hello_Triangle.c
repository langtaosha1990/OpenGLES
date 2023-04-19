//
//  Hello_Triangle.c
//  opengl3_demo
//
//  Created by Gpf 郭 on 2022/10/8.
//

#include "Hello_Triangle.h"
#include "FileWrapper.h"
#include <math.h>
#include "textureHelper.h"

typedef struct {
    GLuint program;
    GLfloat offset;
  
    GLuint samplerLoc;
    GLuint offsetLoc;
    
    GLuint textureId;
}UserData;

// 创建贴图
GLuint CreateMipMappedTexture2D()
{
    GLuint textureId;
    int width = 256, height = 256;
    int level;
    GLubyte *pixels;
    GLubyte *prevImage;
    GLubyte *newImage = NULL;
    
    pixels = GenCheckImage(width, height, 8);
    
    if (pixels == NULL) {
        return 0;
    }
    
    // 将图片id绑定到缓冲区
    glGenTextures(1, &textureId);
    glBindTexture(GL_TEXTURE_2D, textureId);
    
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, pixels);
    
    level = 1;
    prevImage = &pixels[0];
    
    while (width > 1 && height > 1)
    {
        int newWidth, newHeight;
        //
        GenMipMap2D(prevImage, &newImage, width, height, &newWidth, &newHeight);
        
        //
        glTexImage2D(GL_TEXTURE_2D, level, GL_RGB, newWidth, newHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, newImage);
        //
        free(prevImage);
        
        //
        prevImage = newImage;
        level++;
        //
        width = newWidth;
        height = newHeight;
    }
    
    free(newImage);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    return textureId;
}



int Init(ESContext *esContext) {
    UserData *userData = esContext->userData;
    const char * vShaderStr = GetShaderWithFile("LightShader_V.glsl");
    const char *fShaderStr = GetShaderWithFile("LightShader_F.glsl");
    
    userData->program = esLoadProgram(vShaderStr, fShaderStr);
    
    if (userData->program == 0) {
        return GL_FALSE;
    }
    
    userData->samplerLoc = glGetUniformLocation(userData->program, "map");
    userData->offsetLoc = glGetUniformLocation(userData->program, "offset");
    
    userData->textureId = CreateMipMappedTexture2D();
    
    glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
    return GL_TRUE;
}


void Draw(ESContext *esContext)
{
    UserData *userData = esContext->userData;
    GLfloat vVertices[] = { -0.5f,  0.5f, 0.0f, 1.5f,  // Position 0
                             0.0f,  0.0f,              // TexCoord 0
                            -0.5f, -0.5f, 0.0f, 0.75f, // Position 1
                             0.0f,  1.0f,              // TexCoord 1
                             0.5f, -0.5f, 0.0f, 0.75f, // Position 2
                             1.0f,  1.0f,              // TexCoord 2
                             0.5f,  0.5f, 0.0f, 1.5f,  // Position 3
                             1.0f,  0.0f               // TexCoord 3
                          };
    GLushort indices[] = { 0, 1, 2, 0, 2, 3 };
    
    glViewport(0, 0, esContext->width, esContext->height);
    //
    glClear(GL_COLOR_BUFFER_BIT);
    //
    glUseProgram(userData->program);
    
    //设置顶点
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), vVertices);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), &vVertices[4]);
    glEnableVertexAttribArray(1);
    
    // 设置纹理
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, userData->textureId);
    
    glUniform1i(userData->samplerLoc, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glUniform1f(userData->offsetLoc, -0.6f);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, indices);
    
    //
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glUniform1f(userData->offsetLoc, 0.6f);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, indices);
    
}

void ShutDown(ESContext *esContext)
{
    UserData *userData = esContext->userData;

    // Delete texture object
    glDeleteTextures(1, &userData->textureId);

    // Delete program object
    glDeleteProgram(userData->program);
}

int esMain ( ESContext *esContext )
{
    esContext->userData = malloc(sizeof(UserData));
    
    esCreateWindow(esContext, "2D 纹理", 320, 240, ES_WINDOW_RGB);
    
    if (!Init(esContext)) {
        return GL_FALSE;
    }
    esRegisterDrawFunc(esContext, Draw);
    esRegisterShutdownFunc(esContext, ShutDown);
    
    return GL_TRUE;
}

