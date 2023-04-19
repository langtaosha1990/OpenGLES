//
//  textureHelper.h
//  opengl3_demo
//
//  Created by Gpf 郭 on 2022/12/2.
//

#ifndef textureHelper_h
#define textureHelper_h
#include <stdio.h>
#include "esUtil.h"


GLboolean GenMipMap2D(GLubyte *src, GLubyte **dst, int srcWidth, int srcHeight, int *dstWidth, int *dstHeight);

GLubyte *GenCheckImage(int width, int height, int checkSize);
#endif /* textureHelper_h */
