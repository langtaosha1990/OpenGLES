//
//  textureHelper.c
//  opengl3_demo
//
//  Created by Gpf 郭 on 2022/12/2.
//

#include "textureHelper.h"
#include "esUtil.h"

GLboolean GenMipMap2D(GLubyte *src, GLubyte **dst, int srcWidth, int srcHeight, int *dstWidth, int *dstHeight)
{
    int x, y;
    int texelSize = 3;
    *dstWidth = srcWidth / 2;
    
    if (*dstWidth <= 0) {
        *dstWidth = 1;
    }
    
    *dstHeight = srcHeight / 2;
    
    if (*dstHeight <= 0)
    {
        *dstHeight = 1;
    }
    
    *dst = malloc(sizeof(GLubyte) * texelSize * (*dstWidth) * (*dstHeight));
    
    if (*dst == NULL) {
        return GL_FALSE;
    }
    
    for (y = 0; y < *dstHeight; y++) {
        for (x = 0; x < *dstWidth; x++) {
            int srcIndex[4];
            float r = 0.0f, g = 0.0f, b = 0.0f;
            int sample;
            
            // 计算前一个 2x2 像素网格的偏移量
            //执行盒式过滤器的图像
            srcIndex[0] = (((y * 2) * srcWidth) + (x * 2)) * texelSize;
            srcIndex[1] = (((y * 2) * srcWidth) + (x * 2 + 1)) * texelSize;
            srcIndex[2] = ((((y * 2) + 1) * srcWidth) + (x * 2)) * texelSize;
            srcIndex[3] = ((((y * 2) + 1) * srcWidth) + (x * 2 + 1)) * texelSize;
            
            for (sample = 0; sample < 4; sample++) {
                r += src[srcIndex[sample]];
                g += src[srcIndex[sample] + 1];
                b += src[srcIndex[sample] + 2];
            }
            
            r /= 4.0;
            g /= 4.0;
            b /= 4.0;
            
            // 存储像素
            (*dst)[(y * (*dstWidth) + x) *texelSize] = (GLubyte)(r);
            (*dst)[(y *(*dstWidth) + x) * texelSize + 1] = (GLubyte)(g);
            (*dst)[(y *(*dstWidth) + x) * texelSize + 2] = (GLubyte)(b);
            
        }
    }
    return GL_TRUE;
}

// 创建RGB8图像
GLubyte *GenCheckImage(int width, int height, int checkSize)
{
    int x, y;
    GLubyte *pixels = malloc(width * height * 3);
    if (pixels == NULL)
    {
        return NULL;
    }
    
    // 循环给每个像素赋值rgb颜色
    for (y = 0; y < height; y++) {
        for (x = 0; x < width; x++) {
            GLubyte rColor = 0;
            GLubyte bColor = 0;
            if ((x / checkSize) % 2 == 0)
            {
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
