//
//  Hello_Triangle.c
//  opengl3_demo
//
//  Created by Gpf 郭 on 2022/10/8.
//

#include "Hello_Triangle.h"

//
typedef struct {
    GLuint programObject;
} UserData;

GLuint LoadShader(GLenum type, const char *shaderSrc)
{
    GLuint shader;
    GLint compiled;
    
    shader = glCreateShader(type);
    if (shader == 0) {
        return 0;
    }
    // 加载shader源文件
    glShaderSource(shader, 1, &shaderSrc, NULL);
    // 编译shader
    glCompileShader(shader);
    
    // 获取shader编译状态
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
    
    if (!compiled) {
        // 获取编译中出错的问题
        GLint infoLen = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLen);
        
        if (infoLen > 1) {
            char *infoLog = malloc(sizeof(char) * infoLen);
            
            glGetShaderInfoLog(shader, infoLen, NULL, infoLog);
            esLogMessage("Error compiling shader:\n %s\n", infoLog);
            free(infoLog);
        }
        // 标记删除
        glDeleteShader(shader);
        return 0;
    }
    return shader;
}

int Init(ESContext * esContext)
{
    UserData * userData = esContext->userData;
    char vShaderStr[] =
       "#version 300 es                          \n"
       "layout(location = 0) in vec4 vPosition;  \n"
       "void main()                              \n"
       "{                                        \n"
       "   gl_Position = vPosition;              \n"
       "}                                        \n";

    char fShaderStr[] =
       "#version 300 es                              \n"
       "precision mediump float;                     \n"
       "out vec4 fragColor;                          \n"
       "void main()                                  \n"
       "{                                            \n"
       "   fragColor = vec4 ( 1.0, 0.0, 0.0, 1.0 );  \n"
       "}                                            \n";
    
    GLuint vertexShader;
    GLuint fragmentShader;
    GLuint programObject;
    GLint linked;
    
    // 加载顶点着色器和片源着色器
    vertexShader = LoadShader(GL_VERTEX_SHADER, vShaderStr);
    fragmentShader = LoadShader(GL_FRAGMENT_SHADER, fShaderStr);
    
    // 创建链接器
    programObject = glCreateProgram();
    
    if (programObject == 0) {
        return 0;
    }
    
    // 绑定shader
    glAttachShader(programObject, vertexShader);
    glAttachShader(programObject, fragmentShader);
    
    // 链接链接器
    glLinkProgram(programObject);
    
    // 获取链接器状态
    glGetProgramiv(programObject, GL_LINK_STATUS, &linked);
    if (!linked) {  // 如果链接器连接失败
        // 获取日志长度
        GLint infoLen = 0;
        glGetProgramiv(programObject, GL_INFO_LOG_LENGTH, &infoLen);
        
        // 获取输出日志
        if (infoLen > 1) {
            char *infoLog = malloc(sizeof(char) * infoLen);
            glGetProgramInfoLog(programObject, infoLen, NULL, infoLog);
            free(infoLog);
        }
        // 失败时删除程序，并返回失败
        glDeleteProgram(programObject);
        return FALSE;
    }
    
    // 标记删除，当所有链接断开后才会释放
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    
    // 将链接器存储起来
    userData->programObject = programObject;
    glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
    return GL_TRUE;
}

// 绘制三角形
void Draw(ESContext * esContext)
{
    UserData * userData = esContext->userData;
    GLfloat vVertices[] = {
        0.0f, 0.5f, 0.0f,
        -0.5f, -0.5f, 0.0f,
        0.5f, -0.5f, 0.0f
    };
    
    // 设置视口
    glViewport(0, 0, esContext->width, esContext->height);
    
    // 清除缓存区颜色
    glClear(GL_COLOR_BUFFER_BIT);
    
    // 设置连接器
    glUseProgram(userData->programObject);
    // 获取vPosition在链接器中的位置，也可以直接使用layout(location = 0) in vec4 vPosition;中location的值
     GLint location = glGetAttribLocation(userData->programObject, "vPosition");
    
    /**
     glVertexAttribPointer(GLuint indx, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const GLvoid *ptr)
     */
    // 将vvertices数据绑定到顶点着色器
    /**
     glVertexAttribPointer(GLuint indx, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const GLvoid *ptr)
     indx: 指定要修改的顶点属性的索引值，即在着色器中指定的统一变量的编号
     size: 每次读取数据的个数（如position是由3个（x,y,z）组成，而颜色是4个（r,g,b,a）,纹理则是2个.）
     type: 数据类型，可用的符号常量有GL_BYTE, GL_UNSIGNED_BYTE, GL_SHORT,GL_UNSIGNED_SHORT, GL_FIXED, 和 GL_FLOAT，初始值为GL_FLOAT。
     normalized: 指定当被访问时，固定点数据值是否应该被归一化（GL_TRUE）或者直接转换为固定点值（GL_FALSE）
     stride:指定连续顶点属性之间的偏移量。如果为0，那么顶点属性会被理解为：它们是紧密排列在一起的。初始值为0
     ptr:指定一个指针，指向数组中第一个顶点属性的第一个组件。初始值为0
     */
    glVertexAttribPointer(location, 3, GL_FLOAT, GL_FALSE, 0, vVertices);
    //数据在GPU端是否可见，即，着色器能否读取到数据，由是否启用了对应的属性决定，这就是glEnableVertexAttribArray的功能，允许顶点着色器读取GPU（服务器端）数据
    glEnableVertexAttribArray(0);
    
    /**
     glDrawArrays(GLenum mode, GLint first, GLsizei count)
     */
    
    glDrawArrays(GL_TRIANGLES, 0, 3);
}

void Shutdown(ESContext *esContext)
{
    UserData * userData = esContext->userData;
    glDeleteProgram(userData->programObject);
}


int esMain ( ESContext *esContext )
{
    esContext->userData = malloc(sizeof(UserData));
    esCreateWindow(esContext, "Hello Triangle", esContext->width, esContext->height, ES_WINDOW_RGB);
    if (!Init(esContext)) {
        return GL_FALSE;
    }
    
    // 注册释放和绘制方法
    esRegisterShutdownFunc(esContext, Shutdown);
    esRegisterDrawFunc(esContext, Draw);
    
    return GL_TRUE;
}

