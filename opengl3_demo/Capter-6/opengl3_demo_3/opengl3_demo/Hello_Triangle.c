//
//  Hello_Triangle.c
//  opengl3_demo
//
//  Created by Gpf 郭 on 2022/10/8.
//

#include "Hello_Triangle.h"

typedef struct {
    GLuint program;
    GLuint vboIds[2];
    GLuint offsetLoc;
} UserData;

#define VERTEX_POS_SIZE     3   //顶点长度
#define VERTEX_COLOR_SIZE   4   //颜色长度

#define VERTEX_POS_INDX       0
#define VERTEX_COLOR_INDX     1

int Init(ESContext * esContext) {
    UserData *userData = esContext->userData;
    char vShaderChar[] =
    "#version 300 es                            \n"
    "layout(location = 0) in vec4 a_position;   \n"
    "layout(location = 1) in vec4 a_color;      \n"
    "uniform float u_offset;                    \n"
    "out vec4 v_color;                          \n"
    "void main()                                \n"
    "{                                          \n"
    "   v_color = a_color;                      \n"
    "   gl_Position = a_position;               \n"
    "   gl_Position.x += u_offset;              \n"
    "}                                          \n";
    
    char fShaderChar[] =
    "#version 300 es                            \n"
    "precision mediump float;                   \n"
    "in vec4 v_color;                           \n"
    "out vec4 out_color;                        \n"
    "void main()                                \n"
    "{                                          \n"
    "   out_color = v_color;                    \n"
    "}                                          \n";
    
    GLuint program = esLoadProgram(vShaderChar, fShaderChar);
    userData->offsetLoc = glGetUniformLocation(program, "u_offset");
    if (program == 0) {
        return GL_FALSE;
    }
    
    userData->program = program;
    userData->vboIds[0] = 0;
    userData->vboIds[1] = 0;
    
    glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
    return GL_TRUE;
}

void DrawPrimitiveWithoutVBOs(GLfloat *vertices,
                              GLint vtxStride,
                              GLint numIndices,
                              GLushort *indices)
{
    GLfloat *vtxBuf = vertices;
    
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    
    glEnableVertexAttribArray(VERTEX_POS_INDX);
    glEnableVertexAttribArray(VERTEX_COLOR_INDX);
    
    glVertexAttribPointer(VERTEX_POS_INDX, VERTEX_POS_SIZE, GL_FLOAT, GL_FALSE, vtxStride, vtxBuf);
    
    vtxBuf += VERTEX_POS_SIZE;
    
    glVertexAttribPointer(VERTEX_COLOR_INDX, VERTEX_COLOR_SIZE, GL_FLOAT, GL_FALSE, vtxStride, vtxBuf);
    
    glDrawElements(GL_TRIANGLES, numIndices, GL_UNSIGNED_SHORT, indices);
    
    glDisableVertexAttribArray(VERTEX_POS_INDX);
    glDisableVertexAttribArray(VERTEX_COLOR_INDX);
}

void DrawPrimitiveWithVBOs(ESContext *esContext,
                           GLint numVertices,
                           GLfloat *vtxBuf,
                           GLint vtxStride,
                           GLint numIndices,
                           GLushort *indices)
{
    UserData *userData = esContext->userData;
    GLuint   offset = 0;
    
    if (userData->vboIds[0] == 0 && userData->vboIds[1] == 0) {
        glGenBuffers ( 2, userData->vboIds );

        glBindBuffer ( GL_ARRAY_BUFFER, userData->vboIds[0] );
        glBufferData ( GL_ARRAY_BUFFER, vtxStride * numVertices,
                       vtxBuf, GL_STATIC_DRAW );
        glBindBuffer ( GL_ELEMENT_ARRAY_BUFFER, userData->vboIds[1] );
        glBufferData ( GL_ELEMENT_ARRAY_BUFFER,
                       sizeof ( GLushort ) * numIndices,
                       indices, GL_STATIC_DRAW );
    }
    
    glBindBuffer(GL_ARRAY_BUFFER, userData->vboIds[0]);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, userData->vboIds[1]);
    
    glEnableVertexAttribArray(VERTEX_POS_INDX);
    glEnableVertexAttribArray(VERTEX_COLOR_INDX);
    
    glVertexAttribPointer(VERTEX_POS_INDX, VERTEX_POS_SIZE, GL_FLOAT, GL_FALSE, vtxStride, (const void *)offset);
    
    offset += VERTEX_POS_SIZE * sizeof(GLfloat);
    glVertexAttribPointer(VERTEX_COLOR_INDX, VERTEX_COLOR_SIZE, GL_FLOAT, GL_FALSE, vtxStride, (const void *)offset);
    
    glDrawElements(GL_TRIANGLES, numIndices, GL_UNSIGNED_SHORT, 0);
    
    glDisableVertexAttribArray(VERTEX_POS_INDX);
    glDisableVertexAttribArray(VERTEX_COLOR_INDX);
    
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    
}

void Draw(ESContext *esContext) {
    UserData *userData = esContext->userData;
    
    GLfloat vertices[3 * (VERTEX_POS_SIZE + VERTEX_COLOR_SIZE)] =
    {
        -0.5f,  0.5f, 0.0f,        // v0
        1.0f,  0.0f, 0.0f, 1.0f,  // c0
        -1.0f, -0.5f, 0.0f,        // v1
        0.0f,  1.0f, 0.0f, 1.0f,  // c1
        0.0f, -0.5f, 0.0f,        // v2
        0.0f,  0.0f, 1.0f, 1.0f,  // c2
    };
    
    GLushort indices[3] = {0, 1, 2};
    
    glViewport(0, 0, esContext->width, esContext->height);
    
    glClear(GL_COLOR_BUFFER_BIT);
    glUseProgram(userData->program);
    glUniform1f(userData->offsetLoc, 0.0f);
    
    DrawPrimitiveWithoutVBOs(vertices, sizeof(GLfloat) * (VERTEX_POS_SIZE + VERTEX_COLOR_SIZE), 3, indices);
    

    glUniform1f(userData->offsetLoc, 1.0f);
    DrawPrimitiveWithVBOs(esContext, 3, vertices,
                            sizeof(GLfloat) * (VERTEX_POS_SIZE + VERTEX_COLOR_SIZE),
                            3, indices);
}



void Shutdown(ESContext *esContext) {
    UserData *userData = esContext->userData;
    glDeleteProgram (userData->program);
    glDeleteBuffers (2, userData->vboIds);
}


int esMain ( ESContext *esContext )
{
    esContext->userData = malloc(sizeof(UserData));
    
    esCreateWindow(esContext, "双拼彩色三角形", esContext->width, esContext->height, ES_WINDOW_RGB);
    if(!Init(esContext)) {
        return GL_FALSE;
    }
    
    esRegisterDrawFunc(esContext, Draw);
    esRegisterShutdownFunc(esContext, Shutdown);
    return GL_TRUE;
}

