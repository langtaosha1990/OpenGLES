//
//  FileWrapper.m
//  opengl3_demo
//
//  Created by Gpf 郭 on 2022/10/10.
//


#import <Foundation/Foundation.h>

const char *GetBundleFileName( const char *fileName )
{
    NSString* fileNameNS = [NSString stringWithUTF8String:fileName];
    NSString* baseName = [fileNameNS stringByDeletingPathExtension];
    NSString* extension = [fileNameNS pathExtension];
    NSString *path = [[NSBundle mainBundle] pathForResource: baseName ofType: extension ];
    fileName = [path cStringUsingEncoding:1];
    
    return fileName;
}
