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

import Foundation

class FlattenParam<P: PrecisionType>: OpParam {
  //typealias ParamPrecisionType = P
  required init(opDesc: PMOpDesc, inScope: Scope) throws {
    do {
      input = try FlattenParam.inputX(inputs: opDesc.inputs, from: inScope)
      output = try FlattenParam.outputOut(outputs: opDesc.outputs, from: inScope)
      axis = try FlattenParam.getAttr(key: "axis", attrs: opDesc.attrs)
    } catch let error {
      throw error
    }
  }
  let input: Texture
  var output: Texture
  let axis: Int
}


class FlattenOp<P: PrecisionType>: Operator<FlattenKernel<P>, FlattenParam<P>>, Runable, Creator, InferShaperable{
  
  typealias OpType = FlattenOp<P>

  func inferShape() {
    //        para.output.dim = para.input.dim
  }
  
  func runImpl(device: MTLDevice, buffer: MTLCommandBuffer) throws {
    do {
      try kernel.compute(commandBuffer: buffer, param: para)
    } catch let error {
      throw error
    }
  }
  
  func delogOutput() {
    print(" \(type) output: ")
    let device = para.output.metalTexture!.device
    let outputArray: [Float32] = device.texture2tensor(texture: para.output.metalTexture, dim: para.output.tensorDim.dims, transpose: para.output.transpose)
    print(outputArray.strideArray())
  }
  
}






