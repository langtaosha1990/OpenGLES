//
//  FileWrapper.m
//  opengl3_demo
//
//  Created by Gpf 郭 on 2022/10/10.
//


#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>

const char *GetBundleFileName(const char *fileName)
{
    NSString* fileNameNS = [NSString stringWithUTF8String:fileName];
    NSString* baseName = [fileNameNS stringByDeletingPathExtension];
    NSString* extension = [fileNameNS pathExtension];
    NSString *path = [[NSBundle mainBundle] pathForResource: baseName ofType: extension ];
    fileName = [path cStringUsingEncoding:1];
    return fileName;
}

const char *GetShaderFileStr( char *fileName)
{
    NSString * file = [NSString stringWithUTF8String:fileName];
    NSString* shader = [[NSBundle mainBundle] pathForResource:file ofType:nil];
    NSString * rel = [NSString stringWithContentsOfFile:shader encoding:NSUTF8StringEncoding error:nil];
    return [rel UTF8String];
}

// 获取UIImage的rgb数组
uint8_t * GetImageData(char *fileName, int *width, int *height)
{
    NSString * name = [NSString stringWithUTF8String:fileName];
    UIImage * image = [UIImage imageNamed:name];
    CGImageRef refImage = [image CGImage];
    CGSize size = image.size;
    *width = (int)(size.width);
    *height = (int)(size.height);
    
    int bitsPerComponent = 8;
    int bytePerPixel = 4;
    
    CGColorSpaceRef colorSpace = CGColorSpaceCreateDeviceRGB();
    
    int pixelCount = size.width * size.height;
    // 获取rgba四通道
    uint8_t *rgba = malloc(pixelCount * bytePerPixel);
    
    CGContextRef context = CGBitmapContextCreate(rgba,
                                                 size.width,
                                                 size.height,
                                                 bitsPerComponent,
                                                 bytePerPixel * size.width,
                                                 colorSpace,
                                                 kCGImageAlphaPremultipliedLast);
    
    CGContextDrawImage(context, CGRectMake(0, 0, size.width, size.height), refImage);
    CGContextRelease(context);
    
    // 通道数少为rgb
    uint8_t *rgb = malloc(pixelCount * 3);
    int m = 0, n = 0;
    for (int i = 0; i < pixelCount; i++) {
        rgb[m++] = rgba[n++];   // r
        rgb[m++] = rgba[n++];   // g
        rgb[m++] = rgba[n++];   // b
        n++;    // 跳过a通道
    }
    free(rgba);
    return rgb;
}
