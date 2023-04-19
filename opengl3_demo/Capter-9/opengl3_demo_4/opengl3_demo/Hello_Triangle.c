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
    
    GLint offsetLoc;
    
    GLuint textureId;
}UserData;

GLubyte *GenCheckImage(int width, int height, int checkSize)
{
    int x, y;
    GLubyte *pixels = malloc(width * height * 3);
    
    if (pixels == NULL) {
        return NULL;
    }
    
    for (y = 0; y < height; y++) {
        for (x = 0; x < width; x++) {
            GLubyte rColor = 0;
            GLubyte bColor = 0;
            
            if ((x / checkSize) % 2 == 0) {
                rColor = 255 * ((y / checkSize) % 2);
                bColor = 255 * (1 - ((y / checkSize) % 2));
            } else {
                bColor = 255 * ((y / checkSize) % 2);
                rColor = 255 * (1 - ((y / checkSize) % 2));
            }
            pixels[(y * width + x) * 3] = rColor;
            pixels[(y * width + x) * 3 + 1] = 0;
            pixels[(y * width + x) * 3 + 2] = bColor;
        }
    }
    return pixels;
}

GLuint CreateTexture2D(void)
{
    GLuint textureId;
    int width, height;
    GLubyte *pixels;
    
//    pixels = GenCheckImage(width, height, 64);
    pixels = GetImageData("dog.jpeg", &width, &height);
    
    if (pixels == NULL) {
        return 0;
    }
    glGenTextures(1, &textureId);
    glBindTexture(GL_TEXTURE_2D, textureId);
    
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0,
                 GL_RGB, GL_UNSIGNED_BYTE, pixels);
    
    glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    return textureId;
}


int Init(ESContext *esContext) {
    UserData *userData = esContext->userData;
    const char *vShaderStr = GetShaderFileStr("LightShader_V.glsl");
    const char * fShaderStr = GetShaderFileStr("LightShader_F.glsl");
    userData->program = esLoadProgram(vShaderStr, fShaderStr);
    
    userData->samplerLoc = glGetUniformLocation(userData->program, "s_texture");
    userData->offsetLoc = glGetUniformLocation(userData->program, "u_offset");
    
    // 获取缓冲区texture的id
    userData->textureId = CreateTexture2D();
    
    glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
    return GL_TRUE;
}


void Draw(ESContext *esContext)
{
    UserData *userData = esContext->userData;
    GLfloat vVertices[] = { -0.3f,  0.3f, 0.0f, 1.0f,  // Position 0
        -1.0f,  -1.0f,              // TexCoord 0
        -0.3f, -0.3f, 0.0f, 1.0f, // Position 1
        -1.0f,  2.0f,              // TexCoord 1
        0.3f, -0.3f, 0.0f, 1.0f, // Position 2
        2.0f,  2.0f,              // TexCoord 2
        0.3f,  0.3f, 0.0f, 1.0f,  // Position 3
        2.0f,  -1.0f               // TexCoord 3
    };
    
    GLushort indices[] = {0, 1, 2, 0, 2, 3};
    glViewport(0, 0, esContext->width, esContext->height);
    
    glClear(GL_COLOR_BUFFER_BIT);
    
    glUseProgram(userData->program);
    
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), &vVertices[0]);
    glEnableVertexAttribArray(0);
    
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), &vVertices[4]);
    glEnableVertexAttribArray(1);
    
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, userData->textureId);
    
    glUniform1i(userData->samplerLoc, 0);
    
    // 重复布局
    glUniform1f(userData->offsetLoc, -0.7f);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, indices);
    
    // 对边缘进行拉伸
    glUniform1f( userData->offsetLoc, 0.0f );
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, indices);
    
    // 镜像布局
    glUniform1f(userData->offsetLoc, 0.7f);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
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
    esCreateWindow(esContext, "纹理贴图", esContext->width, esContext->height, ES_WINDOW_RGB | ES_WINDOW_DEPTH);
    if(!Init(esContext)) {
        return GL_FALSE;
    }
    
    
    
    //
    esRegisterDrawFunc(esContext, Draw);
    esRegisterShutdownFunc(esContext, ShutDown);
    
    return GL_TRUE;
}

