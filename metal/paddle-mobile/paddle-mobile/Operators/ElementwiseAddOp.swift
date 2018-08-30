///* Copyright (c) 2018 PaddlePaddle Authors. All Rights Reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License. */

import Foundation

class ElementwiseAddParam<P: PrecisionType>: OpParam {
  typealias ParamPrecisionType = P
  required init(opDesc: OpDesc, inScope: Scope) throws {
    do {
      inputX = try ElementwiseAddParam.inputX(inputs: opDesc.inputs, from: inScope)
      output = try ElementwiseAddParam.outputOut(outputs: opDesc.outputs, from: inScope)
      axis = try ElementwiseAddParam.getAttr(key: "axis", attrs: opDesc.attrs)
    } catch let error {
      throw error
    }
    do {
      inputY = try ElementwiseAddParam.inputY(inputs: opDesc.paraInputs, from: inScope)
    } catch _ {
      let tensorY: Tensor<P> = try ElementwiseAddParam.inputY(inputs: opDesc.paraInputs, from: inScope)
      let device = inputX.metalTexture!.device
      inputY = Texture.init(device: device, inDim: tensorY.dim)
      let value: [P] = Array(UnsafeBufferPointer(start: tensorY.data.pointer, count: tensorY.dim.numel()))
      inputY.metalTexture = device.tensor2texture(value: value, dim: tensorY.dim.dims)
    }
    
    var offset = axis
    if axis == -1 {
      offset = inputX.tensorDim.cout() - inputY.tensorDim.cout()
    }
    for i in 0..<(inputY.tensorDim.cout()) {
      assert(inputX.tensorDim[offset + i] == inputY.tensorDim[i])
    }
  }
  
  var inputX: Texture<P>
  var inputY: Texture<P>
  var output: Texture<P>
  var axis: Int
}

class ElementwiseAddOp<P: PrecisionType>: Operator<ElementwiseAddKernel<P>, ElementwiseAddParam<P>>, Runable, Creator, InferShaperable{
  typealias OpType = ElementwiseAddOp<P>
  
  func inferShape() {
//    para.output.dim = para.input.dim
  }
  
  func delogOutput() {
    print(" \(type) inputX: ")
    print(para.inputX.metalTexture.toTensor(dim: (n: para.inputX.tensorDim[0], c: para.inputX.tensorDim[1], h: para.inputX.tensorDim[2], w: para.inputX.tensorDim[3])).strideArray())
    print(" \(type) inputY: ")
    print(para.inputY.metalTexture.toTensor(dim: (n: para.inputY.tensorDim[0], c: para.inputY.tensorDim[1], h: para.inputY.tensorDim[2], w: para.inputY.tensorDim[3])).strideArray())
    
    print(" \(type) output: ")
    let originDim = para.output.originDim
    if para.output.transpose == [0, 1, 2, 3] {
      let outputArray = para.output.metalTexture.realNHWC(dim: (n: originDim[0], h: originDim[1], w: originDim[2], c: originDim[3]))
      print(outputArray.strideArray())
    } else if para.output.transpose == [0, 2, 3, 1] {
      print(para.output.metalTexture.toTensor(dim: (n: para.output.tensorDim[0], c: para.output.tensorDim[1], h: para.output.tensorDim[2], w: para.output.tensorDim[3])).strideArray())
    } else {
      print(" not implement")
    }
    
  }
  
  func runImpl(device: MTLDevice, buffer: MTLCommandBuffer) throws {
    do {
      try kernel.compute(commandBuffer: buffer, param: para)
    } catch let error {
      throw error
    }
  }
}






