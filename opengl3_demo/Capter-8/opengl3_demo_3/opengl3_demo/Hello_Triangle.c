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
    
    GLuint mvpLoc;
    
    GLfloat *vertices;
    GLuint *indices;
    
    int numIndices;
    
    
    GLfloat angle;
    
    ESMatrix mvpMatrix;
    
    // 纹理
    GLfloat *textures;  // 纹理坐标
    GLint mapLoc0;
    GLuint mapTexId0;
    
    GLint mapLoc1;
    GLuint mapTexId1;
    
    // 光照
    GLuint lightPo; // 光源坐标
    GLfloat *normals; // 法线
    
}UserData;

GLuint LoadTexture (void *ioContext, char *fileName)
{
   int width, height;
   char *buffer = esLoadTGA(ioContext, fileName, &width, &height);
   GLuint texId;

   if (buffer == NULL)
   {
      esLogMessage("Error loading (%s) image.\n", fileName);
      return 0;
   }
   glGenTextures(1, &texId);
   glBindTexture(GL_TEXTURE_2D, texId);

   glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, buffer);
   glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
   glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
   glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
   glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

   free(buffer);
   return texId;
}


int Init(ESContext *esContext) {
    UserData *userData = esContext->userData;
    const char *vShaderStr = GetShaderFileStr("LightShader_V.glsl");
    const char * fShaderStr = GetShaderFileStr("LightShader_F.glsl");
    
    userData->program = esLoadProgram(vShaderStr, fShaderStr);
    userData->mvpLoc = glGetUniformLocation(userData->program, "u_mvpMatrix");
    userData->numIndices = esGenCube(0.5f, &userData->vertices, &userData->normals, &userData->textures, &userData->indices);
    
    userData->mapLoc0 = glGetUniformLocation(userData->program, "map0");
    userData->mapLoc1 = glGetUniformLocation(userData->program, "map1");
    userData->mapTexId0 = LoadTexture(esContext->platformData, "basemap.tga");
    userData->mapTexId1 = LoadTexture(esContext->platformData, "lightmap.tga");
    userData->angle = 45.0f;
    
    glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
    
    return GL_TRUE;
}

void Update(ESContext *esContext, float deltaTime)
{
    UserData *userData = esContext->userData;
    ESMatrix perspective;   // 视图矩阵
    ESMatrix modelview;     // 模型矩阵
    float aspect;
    
    userData->angle += (deltaTime * 40.0f);
    if (userData->angle >= 360.0f) {
        userData->angle -= 360.0f;
    }
    
    aspect = (GLfloat)esContext->width / (GLfloat)esContext->height;
    
    esMatrixLoadIdentity(&perspective);
    //(3)获取透视矩阵
    /*
     参数1：矩阵
     参数2：视角，度数为单位
     参数3：纵横比
     参数4：近平面距离
     参数5：远平面距离
     */
    esPerspective(&perspective, 30.0f, aspect, 3.0f, 10.0f);
    
    // 初始化模型矩阵
    esMatrixLoadIdentity(&modelview);
    // 将模型矩阵沿z轴移动-5.0个单位
    esTranslate(&modelview, 0.0, 0.0, -5.0);
    
    // 设置矩阵modelview绕x/z轴旋转userData->angle度
    esRotate(&modelview, userData->angle, 1.0, 0.0, 1.0);
    
    // 获取矩阵相乘之后的结果
    esMatrixMultiply(&userData->mvpMatrix, &modelview, &perspective);
}

void Draw(ESContext *esContext)
{
    UserData *userData = esContext->userData;
    glViewport(0, 0, esContext->width, esContext->height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    glUseProgram(userData->program);
    
    // 设置读取方式
    //参数1：index,顶点数据的索引
    //参数2：size,每个顶点属性的组件数量，1，2，3，或者4.默认初始值是4.
    //参数3：type,数据中的每个组件的类型，常用的有GL_FLOAT,GL_BYTE,GL_SHORT。默认初始值为GL_FLOAT
    //参数4：normalized,固定点数据值是否应该归一化，或者直接转换为固定值。（GL_FALSE）
    //参数5：stride,连续顶点属性之间的偏移量，默认为0；
    //参数6：指定一个指针，指向数组中的第一个顶点属性的第一个组件。默认为0
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), userData->vertices);
    glEnableVertexAttribArray(0);
    
    //设置纹理相关
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), userData->textures);
    glEnableVertexAttribArray(1);
    
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, userData->mapTexId0);
    glUniform1i(userData->mapLoc0, 0);
    
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, userData->mapTexId1);
    glUniform1i(userData->mapLoc1, 1);
    
    // 设置法线
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), userData->normals);
    glEnableVertexAttribArray(2);
    
    //将模型视图矩阵传递到顶点着色器
    /*
     void glUniformMatrix4fv(GLint location,  GLsizei count,  GLboolean transpose,  const GLfloat *value);
     参数列表：
     location:指要更改的uniform变量的位置
     count:更改矩阵的个数
     transpose:是否要转置矩阵，并将它作为uniform变量的值。必须为GL_FALSE
     value:执行count个元素的指针，用来更新指定uniform变量
     */
    glUniformMatrix4fv(userData->mvpLoc, 1, GL_FALSE, (GLfloat *)&userData->mvpMatrix.m[0][0]);
    
    // 开启剔除效果
    glEnable(GL_CULL_FACE);
    
    // 绘制图元
    glDrawElements(GL_TRIANGLES, userData->numIndices, GL_UNSIGNED_INT, userData->indices);
}

void ShutDown(ESContext *esContext)
{
    UserData *userData = esContext->userData;
    if (userData->vertices != NULL) {
        free(userData->vertices);
    }
    if (userData->indices != NULL) {
        free(userData->indices);
    }
    glDeleteProgram(userData->program);
}

int esMain ( ESContext *esContext )
{
    esContext->userData = malloc(sizeof(UserData));
    
    esCreateWindow(esContext, "旋转的立方体", esContext->width, esContext->height, ES_WINDOW_RGB | ES_WINDOW_DEPTH);
    if(!Init(esContext)) {
        return GL_FALSE;
    }
    
    //
    esRegisterDrawFunc(esContext, Draw);
    esRegisterUpdateFunc(esContext, Update);
    esRegisterShutdownFunc(esContext, ShutDown);
    
    return GL_TRUE;
}

