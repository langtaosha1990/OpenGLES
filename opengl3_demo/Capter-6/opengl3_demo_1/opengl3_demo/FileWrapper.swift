//
//  FileWrapper.swift
//  opengl3_demo
//
//  Created by Gpf 郭 on 2022/10/8.
//

import Foundation


@_silgen_name("GetBundleFileName")
func GetBundleFileName( fileName : UnsafePointer<CChar>) -> UnsafePointer<CChar> {
    let myFileName = String(cString: fileName)
    let fileNameNS = NSString.init(utf8String: myFileName)!
    let baseName = fileNameNS.deletingPathExtension
    let exten = fileNameNS.pathExtension
    let path = Bundle.main.path(forResource: baseName, ofType: exten)!
    let charArray = path.cString(using: .utf8)
    let len = charArray?.count
    let pointer = UnsafeMutablePointer<Int8>.allocate(capacity: len!)
    return UnsafePointer(pointer)
}

@_silgen_name("c_swift_test")
func c_swift_test() {
    print("哈利路亚")
}


