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

    GLuint fbo;
    
    GLuint colorTexId[4];
    
    // 纹理尺寸
    GLsizei textureWidth;
    GLsizei textureHeight;
}UserData;

int InitFBO(ESContext *esContext)
{
    UserData * userData = esContext->userData;
    int i;
    GLint defaultFramebuffer = 0;
    const GLenum attachments[4] = {
        GL_COLOR_ATTACHMENT0,
        GL_COLOR_ATTACHMENT1,
        GL_COLOR_ATTACHMENT2,
        GL_COLOR_ATTACHMENT3
    };
    
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &defaultFramebuffer);
    
    // 创建fbo
    glGenFramebuffers(1, &userData->fbo);
    // 绑定fbo
    glBindFramebuffer(GL_FRAMEBUFFER, userData->fbo);
    
    userData->textureHeight = userData->textureWidth = 400;
    
    // 申请纹理缓冲区
    glGenTextures(4, &userData->colorTexId[0]);
    for (i = 0; i < 4; i++) {
        // 初始化纹理
        glBindTexture(GL_TEXTURE_2D, userData->colorTexId[i]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,
                     userData->textureWidth, userData->textureHeight,
                     0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
        
        // 设置纹理环绕方式
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        // glFramebufferTexture2D或glFramebufferTexturelayer命令，将相关纹理绑定到FBO上
        glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, attachments[i],
                               GL_TEXTURE_2D, userData->colorTexId[i], 0);
    }
    
    // 为渲染制定颜色附着
    // n 指定bufs中的缓冲区数量
    // 只想一个符号常量数组，这些常量指定了片段颜色或者数据值将要写入的缓冲区，可以通过glGetIntegerv查询颜色覆辙的最大数值
    glDrawBuffers(4, attachments);
    //
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        return GL_FALSE;
    }
    
    glBindFramebuffer(GL_FRAMEBUFFER, defaultFramebuffer);
    return GL_TRUE;
}


int Init(ESContext *esContext) {
    UserData *userData = esContext->userData;
    const char *vShader = GetShaderFileStr("Shader_V.glsl");
    const char *fShader = GetShaderFileStr("Shader_F.glsl");
    userData->program = esLoadProgram(vShader, fShader);
    
    if(!InitFBO(esContext)) {
        return GL_FALSE;
    }
    // 制定清除缓冲区时需要清除到的值
    glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
    return GL_TRUE;
}

void Read(ESContext *esContext)
{
    char *pixel = malloc(sizeof(esContext->width * esContext->height * 32));
    glReadPixels(0, 0, esContext->width, esContext->height, GL_RGBA, GL_FLOAT, &pixel);
    
}

void DrawGeometry(ESContext *esContext)
{
    UserData *userData = esContext->userData;
    GLfloat vVertices[] = { -1.0f,  1.0f, 0.0f,
                            -1.0f, -1.0f, 0.0f,
                             1.0f, -1.0f, 0.0f,
                             1.0f,  1.0f, 0.0f,
                          };
    GLushort indices[] = { 0, 1, 2, 0, 2, 3 };
        
    glViewport(0, 0, esContext->width, esContext->height);
    
    glClear(GL_COLOR_BUFFER_BIT);
    
    glUseProgram(userData->program);
    
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
                          3 * sizeof(GLfloat), vVertices);
    glEnableVertexAttribArray(0);
    
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, indices);
    Read(esContext);
}

void BlitTextures(ESContext *esContext)
{
    
    UserData * userData = esContext->userData;
    
    glBindFramebuffer(GL_READ_FRAMEBUFFER, userData->fbo);
    //
    glReadBuffer(GL_COLOR_ATTACHMENT0);
    glBlitFramebuffer(0, 0, userData->textureWidth, userData->textureHeight,
                      0, 0, esContext->width / 2, esContext->height / 2,
                      GL_COLOR_BUFFER_BIT, GL_LINEAR);
    
    glReadBuffer(GL_COLOR_ATTACHMENT1);
    glBlitFramebuffer(0, 0, userData->textureWidth, userData->textureHeight,
                      esContext->width / 2, 0, esContext->width, esContext->height / 2,
                      GL_COLOR_BUFFER_BIT, GL_LINEAR);
    
    glReadBuffer(GL_COLOR_ATTACHMENT2);
    glBlitFramebuffer(0, 0, userData->textureWidth, userData->textureHeight,
                      0, esContext->height / 2, esContext->width / 2, esContext->height,
                      GL_COLOR_BUFFER_BIT, GL_LINEAR);
    
    glReadBuffer(GL_COLOR_ATTACHMENT3);
    glBlitFramebuffer(0, 0, userData->textureWidth, userData->textureHeight,
                    esContext->width / 2, esContext->height / 2, esContext->width, esContext->height,
                      GL_COLOR_BUFFER_BIT, GL_LINEAR);
    
}


void Draw(ESContext *esContext)
{
    UserData *userData = esContext->userData;
    GLint defaultFramebuffer = 0;
    const GLenum attachments[4] = {
        GL_COLOR_ATTACHMENT0,
        GL_COLOR_ATTACHMENT1,
        GL_COLOR_ATTACHMENT2,
        GL_COLOR_ATTACHMENT3,
    };
    
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &defaultFramebuffer);
    
    glBindFramebuffer(GL_FRAMEBUFFER, userData->fbo);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDrawBuffers(4, attachments);
    
    DrawGeometry(esContext);
    
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, defaultFramebuffer);
    BlitTextures(esContext);
}



void ShutDown(ESContext *esContext)
{
    UserData *userData = esContext->userData;
    glDeleteTextures(4, userData->colorTexId);
    glDeleteFramebuffers(1, &userData->fbo);
    glDeleteProgram(userData->program);
}

int esMain ( ESContext *esContext )
{
    esContext->userData = malloc(sizeof(UserData));
    
    esCreateWindow(esContext, "旋转的立方体", 400, 400, ES_WINDOW_RGB | ES_WINDOW_ALPHA);
    if(!Init(esContext)) {
        return GL_FALSE;
    }
    
    //
    esRegisterDrawFunc(esContext, Draw);
    esRegisterShutdownFunc(esContext, ShutDown);
    
    return GL_TRUE;
}

