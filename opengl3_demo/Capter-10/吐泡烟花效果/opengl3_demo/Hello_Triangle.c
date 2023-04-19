//
//  Hello_Triangle.c
//  opengl3_demo
//
//  Created by Gpf 郭 on 2022/10/8.
//

#include "Hello_Triangle.h"
#include "FileWrapper.h"

#define NUM_PARTICLES   1000
#define PARTICLE_SIZE   7

#define ATTRIBUTE_LIFETIME_LOCATION       0
#define ATTRIBUTE_STARTPOSITION_LOCATION  1
#define ATTRIBUTE_ENDPOSITION_LOCATION    2

typedef struct {
    GLuint program;
    
    GLint timeLoc;
    GLint colorLoc;
    GLint centerPositionLoc;
    GLint samplerLoc;
    
    GLuint textureId;
    
    float particleData[NUM_PARTICLES * PARTICLE_SIZE];
    
    float time;
} UserData;


GLuint LoadTexture(void *ioContext, char *fileName)
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
    glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
    glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
    
    free(buffer);
    return texId;
}


int Init(ESContext *esContext)
{
    UserData * userData = esContext->userData;
    int i;
    
    const char * vShader = GetShaderFileStr("VShader.glsl");
    const char * fShader = GetShaderFileStr("FShader.glsl");
    
    userData->program = esLoadProgram(vShader, fShader);
    
    userData->timeLoc = glGetUniformLocation(userData->program, "u_time");
    userData->centerPositionLoc = glGetUniformLocation(userData->program, "u_centerPosition");
    userData->colorLoc = glGetUniformLocation(userData->program, "u_color");
    userData->samplerLoc = glGetUniformLocation(userData->program, "s_texture");
    
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    
    srand(0);
    
    float scale = esContext->width / (esContext->height * 1.0);
    esLogMessage("%d--%d", esContext->width, esContext->height);
    for (int i = 0; i < NUM_PARTICLES; i++) {
        float *particleData = &userData->particleData[i * PARTICLE_SIZE];
        
        (*particleData++) = ((float)(rand() % 10000) / 10000.0f);
        // 起始位置
        (*particleData++) = ((float)(rand() % 10000) / 5000.0f) - 1.0f;
        (*particleData++) = (((float)(rand() % 10000) / 5000.0f) - 1.0f) * scale;
        (*particleData++) = ((float)(rand() % 10000) / 5000.0f) - 1.0f;
        // 结束位置
        (*particleData++) = ((float)(rand() % 10000) / 40000.0f) - 0.125f;
        (*particleData++) = (((float)(rand() % 10000) / 40000.0f) - 0.125f) * scale;
        (*particleData++) = ((float)(rand() % 10000) / 40000.0f) - 0.125f;
    }
    
    userData->time = 1.0f;
    userData->textureId = LoadTexture(esContext->platformData, "smoke.tga");
    
    if (userData->textureId <= 0) {
        return GL_FALSE;
    }
    return GL_TRUE;
}

void Update(ESContext *esContext, float deltaTime)
{
    UserData * userData = esContext->userData;
    userData->time += deltaTime;
    
    glUseProgram(userData->program);
    
    if (userData->time >= 1.0f) {
        float centerPos[3];
        float color[4];
        
        userData->time = 0.0f;
        
        centerPos[0] = ((float)(rand() % 10000) / 10000.0f) - 0.5f;
        centerPos[1] = ((float)(rand() % 10000) / 10000.0f) - 0.5f;
        centerPos[2] = ((float)(rand() % 10000) / 10000.0f) - 0.5f;
        
        glUniform3fv(userData->centerPositionLoc, 1, &centerPos[0]);
        
        color[0] = ((float)(rand() % 10000) / 20000.0f) + 0.5f;
        color[1] = ((float)(rand() % 10000) / 20000.0f) + 0.5f;
        color[2] = ((float)(rand() % 10000) / 20000.0f) + 0.5f;
        color[3] = 0.5;
        
        glUniform4fv(userData->colorLoc, 1, &color[0]);
    }
    glUniform1f(userData->timeLoc, userData->time);
    
    
    
    
}

void Draw(ESContext *esContext)
{
    UserData * userData = esContext->userData;
    glViewport(0, 0, esContext->width, esContext->height);
    
    glClear(GL_COLOR_BUFFER_BIT);
    
    glUseProgram(userData->program);
    
    glVertexAttribPointer(ATTRIBUTE_LIFETIME_LOCATION, 1, GL_FLOAT, GL_FALSE, PARTICLE_SIZE * sizeof(GLfloat), userData->particleData);
    
    glVertexAttribPointer(ATTRIBUTE_ENDPOSITION_LOCATION, 3, GL_FLOAT, GL_FALSE, PARTICLE_SIZE * sizeof(GLfloat), &userData->particleData[1]);
    
    glVertexAttribPointer (ATTRIBUTE_STARTPOSITION_LOCATION, 3, GL_FLOAT,
                            GL_FALSE, PARTICLE_SIZE * sizeof(GLfloat),
                            &userData->particleData[4]);
    glEnableVertexAttribArray(ATTRIBUTE_LIFETIME_LOCATION);
    glEnableVertexAttribArray(ATTRIBUTE_ENDPOSITION_LOCATION);
    glEnableVertexAttribArray(ATTRIBUTE_STARTPOSITION_LOCATION);
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, userData->textureId);
    
    glUniform1i(userData->samplerLoc, 0);
    
    glDrawArrays(GL_POINTS, 0, NUM_PARTICLES);
}



void ShutDown(ESContext *esContext)
{
    UserData *userData = esContext->userData;
    
    glDeleteTextures (1, &userData->textureId);

    glDeleteProgram (userData->program);
}


int esMain ( ESContext *esContext )
{
    esContext->userData = malloc(sizeof(UserData));
    
    esCreateWindow(esContext, "粒子系统", esContext->width, esContext->height, ES_WINDOW_RGB);
    if (!Init(esContext)) {
        return GL_FALSE;
    }
    
    esRegisterDrawFunc(esContext, Draw);
    esRegisterUpdateFunc(esContext, Update);
    esRegisterShutdownFunc(esContext, ShutDown);
    
    return GL_TRUE;
}

