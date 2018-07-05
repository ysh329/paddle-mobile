/* Copyright (c) 2018 PaddlePaddle Authors. All Rights Reserved.
 
 Licensed under the Apache License, Version 2.0 (the "License");
 you may not use this file except in compliance with the License.
 You may obtain a copy of the License at
 
 http://www.apache.org/licenses/LICENSE-2.0
 
 Unless required by applicable law or agreed to in writing, software
 distributed under the License is distributed on an "AS IS" BASIS,
 WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 See the License for the specific language governing permissions and
 limitations under the License. */

import Metal
import Foundation

class InputTexture {
    let mtlTexture: MTLTexture
    let expectDim: Dim
    init(inMTLTexture: MTLTexture, inExpectDim: Dim) {
        mtlTexture = inMTLTexture
        expectDim = inExpectDim
    }
}

extension InputTexture {
    var description: String {
        get{
            return mtlTexture.description
        }
    }
    
    var debugDescription: String {
        get {
            return mtlTexture.debugDescription ?? " MetalTexture "
        }
    }
}

public class Texture: Tensorial {
    var dim: Dim
    let textureDesc: MTLTextureDescriptor
    var metalTexture: MTLTexture
    
    init(device: MTLDevice, inDim: Dim, inLayout: DataLayout = .NHWC) {
        dim = inDim
        layout = inLayout
        let tmpTextureDes = MTLTextureDescriptor.init()
        if inDim.cout() == 1 {
            tmpTextureDes.width = inDim[0]
            tmpTextureDes.textureType = .type1D
        } else if inDim.cout() == 4 {
            tmpTextureDes.height = inDim[1]
            tmpTextureDes.width = inDim[2]
//            print("n : \(inDim[0])")
//            print(inDim[3] * inDim[0])
            tmpTextureDes.depth = 1
            tmpTextureDes.arrayLength = (inDim[3] * inDim[0] + 3)/4
            tmpTextureDes.textureType = .type2DArray
        } else {
            fatalError(" didn't support yet")
        }
        tmpTextureDes.pixelFormat = .r32Float
        tmpTextureDes.storageMode = .shared
        textureDesc = tmpTextureDes
        metalTexture = device.makeTexture(descriptor: tmpTextureDes) ?! " texture nil "
    }
    
//    required public init(inDim: Dim, inLayout: DataLayout = .NHWC, inTexture: MTLTexture) {
//        dim = inDim
//        layout = inLayout
//        metalTexture = inTexture
//        let tmpTextureDes = MTLTextureDescriptor.init()
//        
//        if inDim.cout() == 1 {
//            tmpTextureDes.width = inDim[0]
//            tmpTextureDes.textureType = .type1D
//        } else if inDim.cout() == 2 {
//            tmpTextureDes.height = inDim[0]
//            tmpTextureDes.width = inDim[1]
//            tmpTextureDes.textureType = .type2D
//        } else if inDim.cout() == 3 {
//            fatalError(" not support texture dim 3")
//        } else if inDim.cout() == 4 {
//            tmpTextureDes.height = inDim[1]
//            tmpTextureDes.width = inDim[2]
//            tmpTextureDes.depth = inDim[3] * inDim[1]
//            tmpTextureDes.textureType = .type2DArray
//        }
//        
//        tmpTextureDes.pixelFormat = .r32Float
//        tmpTextureDes.storageMode = .shared
//        textureDesc = tmpTextureDes
//        let device = MTLCreateSystemDefaultDevice()
//        metalTexture = device!.makeTexture(descriptor: tmpTextureDes)!
//    }
    
//    init() {
//        dim = Dim.init(inDim: [])
//        layout = .NCHW
//        let device = MTLCreateSystemDefaultDevice()
//        textureDesc = MTLTextureDescriptor.init()
//        metalTexture = device!.makeTexture(descriptor: textureDesc)!
//    }
    
    private(set) var layout: DataLayout
}

extension Texture {
    public var description: String {
        return debugDescription
    }
    
    public var debugDescription: String{
        var str = ""
        str += "Dim: \(dim) \n value:[ "
        str += " ]"
        return str
    }
    
}
