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
    
    return GL_TRUE;
}

void Update(ESContext *esContext, float deltaTime)
{
    UserData *userData = esContext->userData;

}

void Draw(ESContext *esContext)
{
    UserData *userData = esContext->userData;

}

void ShutDown(ESContext *esContext)
{
    UserData *userData = esContext->userData;

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

