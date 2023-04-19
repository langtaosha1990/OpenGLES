//
//  Hello_Triangle.c
//  opengl3_demo
//
//  Created by Gpf 郭 on 2022/10/8.
//

#include "Hello_Triangle.h"

typedef struct {
    // 链接器
    GLuint programObject;
    
    // 顶点数组ids
    GLuint vboIds[3];
    
} UserData;

#define VERTEX_POS_SIZE       3  // x、y、z    每个顶点所占位数
#define VERTEX_COLOR_SIZE     4  // r、g、b、a  每个颜色所占位数

// 宏定义位置统一变量和颜色统一变量在链接程序中的位置
#define VERTEX_POS_INDX       0
#define VERTEX_COLOR_INDX     1


int Init(ESContext * context)
{
    UserData * userData = context->userData;
    const char vShaderStr[] =
       "#version 300 es                            \n"
       "layout(location = 0) in vec4 a_position;   \n"
       "layout(location = 1) in vec4 a_color;      \n"
       "out vec4 v_color;                          \n"
       "void main()                                \n"
       "{                                          \n"
       "    v_color = a_color;                     \n"
       "    gl_Position = a_position;              \n"
       "}";


    const char fShaderStr[] =
       "#version 300 es            \n"
       "precision mediump float;   \n"
       "in vec4 v_color;           \n"
       "out vec4 o_fragColor;      \n"
       "void main()                \n"
       "{                          \n"
       "    o_fragColor = v_color; \n"
       "}";
    
    GLuint programObject;
    // 创建链接器并关联着色器
    programObject = esLoadProgram(vShaderStr, fShaderStr);
    if (programObject == 0) {
        return GL_FALSE;
    }
    
    userData->programObject = programObject;
    userData->vboIds[0] = 0;
    userData->vboIds[1] = 0;
    userData->vboIds[2] = 0;
    
    glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
    return GL_TRUE;
}

void DrawPrimitiveWithVBOs(ESContext *context,
                           GLint numVertices, GLfloat **vtxBuf,
                           GLint *vtxStrides, GLint numIndices,
                           GLushort *indices)
{
    UserData *userData = context->userData;
    if (userData->vboIds[0] == 0 && userData->vboIds[1] == 0 && userData->vboIds[2] == 0) {
        
        /**
         glGenBuffers(GLsizei n, GLuint *buffers)
         n:申请缓存空间的个数
         buffers:申请空间对应的标记数组
         */
        // 申请空间，并将id放到vboIds
        glGenBuffers(3, userData->vboIds);
        // 将对应的id绑定顶点缓存区.(明确作用)指定ID为userData->vboIds[0]指定缓存的数据类型
        glBindBuffer(GL_ARRAY_BUFFER, userData->vboIds[0]);
        // 将顶点数组的数据copy到顶点缓存区中(GPU显存中)
        glBufferData(GL_ARRAY_BUFFER, vtxStrides[0] * numVertices,
                     vtxBuf[0], GL_STATIC_DRAW);
        
        glBindBuffer ( GL_ARRAY_BUFFER, userData->vboIds[1] );
        glBufferData ( GL_ARRAY_BUFFER, vtxStrides[1] * numVertices,
                       vtxBuf[1], GL_STATIC_DRAW );
        
        glBindBuffer ( GL_ELEMENT_ARRAY_BUFFER, userData->vboIds[2] );
        glBufferData ( GL_ELEMENT_ARRAY_BUFFER,
                       sizeof ( GLushort ) * numIndices,
                       indices, GL_STATIC_DRAW);
    }
    
    glBindBuffer(GL_ARRAY_BUFFER, userData->vboIds[0]);
    glEnableVertexAttribArray(VERTEX_POS_INDX); // 打开position对应的通道
    // glVertexAttrib*命令用于加载index指定的通用顶点属性
    glVertexAttribPointer(VERTEX_POS_INDX, VERTEX_POS_SIZE,
                          GL_FLOAT, GL_FALSE, vtxStrides[0], 0);
    //
    glBindBuffer ( GL_ARRAY_BUFFER, userData->vboIds[1]);
    glEnableVertexAttribArray ( VERTEX_COLOR_INDX );
    glVertexAttribPointer ( VERTEX_COLOR_INDX,
                            VERTEX_COLOR_SIZE,
                            GL_FLOAT, GL_FALSE, vtxStrides[1], 0);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, userData->vboIds[2]);
    glDrawElements(GL_TRIANGLES, numIndices,
                   GL_UNSIGNED_SHORT, 0);
    
    glDisableVertexAttribArray(VERTEX_POS_INDX);
    glDisableVertexAttribArray(VERTEX_COLOR_INDX);
    
//    glBindBuffer(GL_ARRAY_BUFFER, 0);
//    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void Draw(ESContext * context)
{
    UserData * userData = context->userData;
    
    // 生成顶点数组
    GLfloat vertexPos[3 * VERTEX_POS_SIZE] =
    {
        0.0f,  0.5f, 0.0f,        // v0
        -0.5f, -0.5f, 0.0f,        // v1
        0.5f, -0.5f, 0.0f         // v2
    };
    
    // 生成颜色数组
    GLfloat color[4 * VERTEX_COLOR_SIZE] =
    {
        1.0f, 0.0f, 0.0f, 1.0f,   // c0
        0.0f, 1.0f, 0.0f, 1.0f,   // c1
        0.0f, 0.0f, 1.0f, 1.0f    // c2
    };
    
    // 获取需要缓存数据的大小
    GLint vtxStridex[2] =
    {
        VERTEX_POS_SIZE * sizeof(GLfloat),
        VERTEX_COLOR_SIZE * sizeof(GLfloat)
    };
    
    GLushort indices[3] = {0, 1, 2};
    GLfloat *vtxBuf[2] = {vertexPos, color};
    
    glViewport(0, 0, context->width, context->height);
    glClear(GL_COLOR_BUFFER_BIT);
    glUseProgram(userData->programObject);
    DrawPrimitiveWithVBOs(context, 3, vtxBuf,
                          vtxStridex, 3, indices);
    
}

void Shutdown(ESContext *context)
{
    UserData *userData = context->userData;
    glDeleteProgram(userData->programObject);
    glDeleteBuffers(3, userData->vboIds);
}


int esMain ( ESContext *esContext )
{
    esContext->userData = malloc(sizeof(UserData));
    esCreateWindow(esContext, "Triangle", esContext->width, esContext->height, ES_WINDOW_RGB);
    
    if (!Init(esContext)) {
        return GL_FALSE;
    }
    
    esRegisterShutdownFunc(esContext, Shutdown);
    esRegisterDrawFunc(esContext, Draw);
    
    return GL_TRUE;
}

