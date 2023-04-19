//
//  Hello_Triangle.c
//  opengl3_demo
//
//  Created by Gpf 郭 on 2022/10/8.
//

#include "Hello_Triangle.h"
#include <math.h>

#define NUM_INSTANCES   100
#define POSITION_LOC    0
#define COLOR_LOC       1
#define MVP_LOC         2

typedef struct {
    GLuint program;
    
    // VBO
    GLuint positionVBO;
    GLuint colorVBO;
    GLuint mvpVBO;  //
    GLuint indicesIBO;
    
    int numIndices;
    GLfloat angle[NUM_INSTANCES];
} UserData;

int Init(ESContext *esContext)
{
    UserData *userData = esContext->userData;
    
    GLfloat *position;
    GLuint *indices;
    
    const char vShaderStr[] =
    "#version 300 es                            \n"
    "layout(location = 0) in vec4 a_position;   \n"
    "layout(location = 1) in vec4 a_color;      \n"
    "layout(location = 2) in mat4 a_mvpMatrix;  \n"
    "out vec4 v_color;                          \n"
    "void main()                                \n"
    "{                                          \n"
    "   v_color = a_color;                      \n"
    "   gl_Position = a_mvpMatrix * a_position; \n"
    "}                                          \n";
    
    const char fShaderStr[] =
    "#version 300 es                            \n"
    "precision mediump float;                   \n"
    "in vec4 v_color;                           \n"
    "layout(location = 0) out vec4 outColor;    \n"
    "void main()                                \n"
    "{                                          \n"
    "   outColor = v_color;                     \n"
    "}                                          \n"
    ;
    
    userData->program = esLoadProgram(vShaderStr, fShaderStr);
    // 获取顶点数组
    userData->numIndices = esGenCube(0.1f, &position, NULL, NULL, &indices);
    
    glGenBuffers(1, &userData->indicesIBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, userData->indicesIBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * userData->numIndices, indices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    free(indices);
    
    //
    glGenBuffers(1, &userData->positionVBO);
    glBindBuffer(GL_ARRAY_BUFFER, userData->positionVBO);
    glBufferData(GL_ARRAY_BUFFER, 24 * sizeof(GLfloat) * 3, position, GL_STATIC_DRAW);
    free(position);
    
    {
        GLubyte color[NUM_INSTANCES][4];
        int instance;
        
        srandom(0);
        
        // 生成随机色
        for (instance = 0; instance < NUM_INSTANCES; instance++) {
            color[instance][0] = random() % 255;
            color[instance][1] = random() % 255;
            color[instance][2] = random() % 255;
            color[instance][3] = 0;
        }
        
        glGenBuffers(1, &userData->colorVBO);
        glBindBuffer(GL_ARRAY_BUFFER, userData->colorVBO);
        glBufferData(GL_ARRAY_BUFFER, NUM_INSTANCES * 4, color, GL_STATIC_DRAW);
    }
    
    // 设置角度
    {
        int instance;
        for (instance = 0; instance < NUM_INSTANCES; instance++) {
            userData->angle[instance] = (float)(random() % 32768) / 32767.0f * 360.0f;
        }
        glGenBuffers(1, &userData->mvpVBO);
        glBindBuffer(GL_ARRAY_BUFFER, userData->mvpVBO);
        glBufferData(GL_ARRAY_BUFFER, NUM_INSTANCES * sizeof(ESMatrix), NULL, GL_DYNAMIC_DRAW);
    }
    
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
    
    return GL_TRUE;
}

void Draw(ESContext *esContext)
{
    UserData *userData = esContext->userData;
    glViewport(0, 0, esContext->width, esContext->height);
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    glUseProgram(userData->program);
    
    glBindBuffer(GL_ARRAY_BUFFER, userData->positionVBO);
    // 设置位置读取方式
    glVertexAttribPointer(POSITION_LOC, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (const void *)NULL);
    glEnableVertexAttribArray(POSITION_LOC);
    
    // 设置颜色读取方式
    glBindBuffer(GL_ARRAY_BUFFER, userData->colorVBO);
    glVertexAttribPointer(COLOR_LOC, 4, GL_UNSIGNED_BYTE, GL_TRUE, 4 * sizeof(GLubyte), (const void *)NULL);
    glEnableVertexAttribArray(COLOR_LOC);
    glVertexAttribDivisor(COLOR_LOC, 1);
    
    //设置角度读取方式
    glBindBuffer(GL_ARRAY_BUFFER, userData->mvpVBO);
    glVertexAttribPointer(MVP_LOC + 0, 4, GL_FLOAT, GL_FALSE, sizeof(ESMatrix), (const void *)NULL);
    glVertexAttribPointer(MVP_LOC + 1, 4, GL_FLOAT, GL_FALSE, sizeof(ESMatrix), (const void *)(sizeof(GLfloat) * 4));
    glVertexAttribPointer(MVP_LOC + 2, 4, GL_FLOAT, GL_FALSE, sizeof(ESMatrix), (const void *)(sizeof(GLfloat) * 8));
    glVertexAttribPointer(MVP_LOC + 3, 4, GL_FLOAT, GL_FALSE, sizeof(ESMatrix), (const void *)(sizeof(GLfloat) * 12));
    
    // 打开缓冲区
    glEnableVertexAttribArray(MVP_LOC + 0);
    glEnableVertexAttribArray(MVP_LOC + 1);
    glEnableVertexAttribArray(MVP_LOC + 2);
    glEnableVertexAttribArray(MVP_LOC + 3);
    //
    glVertexAttribDivisor(MVP_LOC + 0, 1);
    glVertexAttribDivisor(MVP_LOC + 1, 1);
    glVertexAttribDivisor(MVP_LOC + 2, 1);
    glVertexAttribDivisor(MVP_LOC + 3, 1);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, userData->indicesIBO);
    
    glDrawElementsInstanced(GL_TRIANGLES, userData->numIndices, GL_UNSIGNED_INT, (const void *)NULL, NUM_INSTANCES);
    
}

void Update(ESContext *esContext, float deltaTime)
{
    UserData *userData = esContext->userData;
    ESMatrix *matrixBuf;
    ESMatrix perspective;
    float aspect;
    int instance = 0;
    int numRows;
    int numColumns;
    
    // 获取窗口的宽高比
    aspect = (GLfloat)esContext->width / (GLfloat)esContext->height;
    
    // 初始化矩阵perspective
    esMatrixLoadIdentity(&perspective);
    // 获取初始矩阵
    esPerspective(&perspective, 60.0f, aspect, 1.0f, 20.0f);
    
    glBindBuffer(GL_ARRAY_BUFFER, userData->mvpVBO);
    //命令返回一个指向全部或部分的指针(范围)缓冲区对象的数据存储。可以使用这个指针由应用程序读取或更新缓冲区对象的内容。
    matrixBuf = (ESMatrix *)glMapBufferRange(GL_ARRAY_BUFFER, 0, sizeof(ESMatrix) * NUM_INSTANCES, GL_MAP_WRITE_BIT);
    
    numRows = (int)sqrtf(NUM_INSTANCES);
    numColumns = numRows;
    
    for (instance = 0; instance < NUM_INSTANCES; instance++) {
        ESMatrix modelview;
        float translateX = ((float)(instance % numRows) / (float)numRows) * 2.0f - 1.0f;
        float translateY = ((float)(instance / numColumns) / (float)numColumns) * 2.0f - 1.0f;
        
        esMatrixLoadIdentity(&modelview);
        esTranslate(&modelview, translateX, translateY, -2.0f);
        userData->angle[instance] += (deltaTime * 40.0f);
        
        if (userData->angle[instance] >= 360.0f) {
            userData->angle[instance] -= 360.0f;
        }
        
        esRotate(&modelview, userData->angle[instance], 1.0, 0.0, 1.0);
        
        esMatrixMultiply(&matrixBuf[instance], &modelview, &perspective);
    }
    glUnmapBuffer(GL_ARRAY_BUFFER);
}

void Shutdown(ESContext *esContext)
{
    UserData *userData = esContext->userData;
    glDeleteBuffers (1, &userData->positionVBO);
    glDeleteBuffers (1, &userData->colorVBO);
    glDeleteBuffers (1, &userData->mvpVBO);
    glDeleteBuffers (1, &userData->indicesIBO);

    // Delete program object
    glDeleteProgram (userData->program);
}


int esMain ( ESContext *esContext )
{
    esContext->userData = malloc(sizeof(UserData));
    
    esCreateWindow(esContext, "Instancing", esContext->width, esContext->height, ES_WINDOW_RGB | ES_WINDOW_DEPTH);
    
    if (!Init(esContext)) {
        return GL_FALSE;
    }
    
    // 注册流程
    esRegisterDrawFunc(esContext, Draw);
    esRegisterUpdateFunc(esContext, Update);
    esRegisterShutdownFunc(esContext, Shutdown);
    
    return GL_TRUE;
}

