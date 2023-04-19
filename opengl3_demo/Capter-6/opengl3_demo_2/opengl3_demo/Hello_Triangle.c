//
//  Hello_Triangle.c
//  opengl3_demo
//
//  Created by Gpf 郭 on 2022/10/8.
//

#include "Hello_Triangle.h"
typedef struct {
    GLuint programObject;
} UserData;

#define VERTEX_POS_SIZE    3
#define VERTEX_COLOR_SIZE   4

// 配置初始化的东西：着色器/顶点/颜色
int Init(ESContext * esContext) {
    UserData *userData = esContext->userData;
    char vShaderChar[] =
    "#version 300 es                           \n"
    "layout(location = 0) in vec4 vPosition;   \n"
    "layout(location = 1) in vec4 inColor;     \n"
    "out vec4 v_color;                         \n"
    "void main()                               \n"
    "{                                         \n"
    "   gl_Position = vPosition;               \n"
    "   v_color = inColor;                     \n"
    "}                                         \n";
    
    char fShaderChar[] =
    "#version 300 es                            \n"
    "precision mediump float;                   \n"
    "out vec4 fragColor;                        \n"
    "in vec4 v_color;                           \n"
    "void main()                                \n"
    "{                                          \n"
    "   fragColor = v_color;                    \n"
    "}                                          \n";
    GLuint programObject;
    
    programObject = esLoadProgram(vShaderChar, fShaderChar);
    
    if (programObject == 0) {
        // 初始化失败
        return 0;
    }
    
    userData->programObject = programObject;
    glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
    return GL_TRUE;
}

void Draw(ESContext * esContext)
{
    UserData *userData = esContext->userData;
    GLfloat vVertices[] = {
        0.0f, 0.5f, 0.0f,
        -0.5f, -0.5f, 0.0f,
        0.5f, -0.5f, 0.0f
    };
    
    GLfloat cVertices[] = {
        1.0f, 0.0f, 0.0f, 1.0f,
        0.0f, 1.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 1.0f, 1.0f
    };
    
    
    glViewport(0, 0, esContext->width, esContext->height);
    
    glClear(GL_COLOR_BUFFER_BIT);
    
    glUseProgram(userData->programObject);
    
    GLint location = glGetAttribLocation(userData->programObject, "vPosition");
    glVertexAttribPointer(location, VERTEX_POS_SIZE, GL_FLOAT, GL_FALSE, 0, &vVertices);
    
    GLint color = glGetAttribLocation(userData->programObject, "inColor");
    glVertexAttribPointer(color, VERTEX_COLOR_SIZE, GL_FLOAT, GL_FALSE, 0, &cVertices);
    
    glEnableVertexAttribArray(color);
    glEnableVertexAttribArray(location);
    glDrawArrays(GL_TRIANGLES, 0, 3);
}

void Shutdown(ESContext *esContext)
{
    UserData *userData = esContext->userData;
    glDeleteProgram(userData->programObject);
}

int esMain ( ESContext *esContext )
{
    esContext->userData = malloc(sizeof(UserData));
    
    esCreateWindow(esContext, "彩色三角形", esContext->width, esContext->height, ES_WINDOW_RGB);
    if (!Init(esContext)) {
        return GL_FALSE;
    }
    // 注册绘制和释放方法
    esRegisterDrawFunc(esContext, Draw);
    esRegisterShutdownFunc(esContext, Shutdown);
    
    return GL_TRUE;
}

