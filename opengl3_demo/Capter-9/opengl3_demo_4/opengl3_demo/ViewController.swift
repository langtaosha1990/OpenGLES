//
//  ViewController.swift
//  opengl3_demo
//
//  Created by Gpf 郭 on 2022/10/8.
//

import UIKit
import UIKit
import OpenGLES
import GLKit

class ViewController: GLKViewController, GLKViewControllerDelegate {
    var context: EAGLContext!
    var _esContext: ESContext!
    override func viewDidLoad() {
        super.viewDidLoad()
        
        context = EAGLContext.init(api: .openGLES3)
        guard let context = context else {
            print("Failed to create es context")
            return
        }

        let newView = self.view as! GLKView
        newView.context = context
        newView.drawableDepthFormat = .format24
        self.delegate = self
        setupGL()
        
    }
    
    func test() {
        
    }
    
    
    deinit {
        tearDownGL()
        if EAGLContext.current() == self.context {
            EAGLContext.setCurrent(nil)
        }
    }
}

extension ViewController {
    
    func glkViewControllerUpdate(_ controller: GLKViewController) {
        update()
    }
    
    func setupGL() {
        EAGLContext.setCurrent(self.context)
        memset(&_esContext, 0, MemoryLayout.size(ofValue: _esContext))
        _esContext.width = GLint(self.view.frame.width);
        _esContext.width = GLint(self.view.frame.height);
        esMain(&_esContext);
    }
    
    func update() {
        if _esContext.updateFunc != nil {
            _esContext.updateFunc(&_esContext, Float(self.timeSinceLastUpdate));
        }
    }
    
    override func glkView(_ view: GLKView, drawIn rect: CGRect) {
        _esContext.width = GLint(view.drawableWidth)
        _esContext.height = GLint(view.drawableHeight)
        
        if (_esContext.drawFunc != nil) {
            _esContext.drawFunc(&_esContext)
        }
    }
    
    func tearDownGL() {
        EAGLContext.setCurrent(self.context)
        if _esContext.shutdownFunc != nil {
            _esContext.shutdownFunc(&_esContext)
        }
    }
    
    override func didReceiveMemoryWarning() {
        super.didReceiveMemoryWarning()
        if self.isViewLoaded && self.view.window == nil {
            self.view = nil
            
            tearDownGL()
            
            if EAGLContext.current() == self.context {
                EAGLContext.setCurrent(nil)
            }
            self.context = nil
        }
    }
}


