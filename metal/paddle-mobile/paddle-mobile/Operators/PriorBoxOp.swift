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

class PriorBoxParam<P: PrecisionType>: OpParam {
    typealias ParamPrecisionType = P
    required init(opDesc: OpDesc, inScope: Scope) throws {
        do {
            input = try PriorBoxParam.input(inputs: opDesc.inputs, from: inScope)
            output = try PriorBoxParam.outputBoxes(outputs: opDesc.outputs, from: inScope)
            inputImage = try PriorBoxParam.inputImage(inputs: opDesc.inputs, from: inScope)
            outputVariances = try PriorBoxParam.outputVariances(outputs: opDesc.outputs, from: inScope)
            minSizes = try PriorBoxParam.getAttr(key: "min_sizes", attrs: opDesc.attrs)
            maxSizes = try PriorBoxParam.getAttr(key: "max_sizes", attrs: opDesc.attrs)
            aspectRatios = try PriorBoxParam.getAttr(key: "aspect_ratios", attrs: opDesc.attrs)
            variances = try PriorBoxParam.getAttr(key: "variances", attrs: opDesc.attrs)
            flip = try PriorBoxParam.getAttr(key: "flip", attrs: opDesc.attrs)
            clip = try PriorBoxParam.getAttr(key: "clip", attrs: opDesc.attrs)
            stepW = try PriorBoxParam.getAttr(key: "step_w", attrs: opDesc.attrs)
            stepH = try PriorBoxParam.getAttr(key: "step_h", attrs: opDesc.attrs)
            offset = try PriorBoxParam.getAttr(key: "offset", attrs: opDesc.attrs)
        } catch let error {
            throw error
        }
    }
    
    let minSizes: [Float32]
    let maxSizes: [Float32]
    let aspectRatios: [Float32]
    var newAspectRatios: [Float32]?
    let variances: [Float32]
    let flip: Bool
    let clip: Bool
    var stepW: Float32
    var stepH: Float32
    let offset: Float32
    
    let input: Texture<P>
    let inputImage: Texture<P>
    var output: Texture<P>
    let outputVariances: Texture<P>
}

class PriorBoxOp<P: PrecisionType>: Operator<PriorBoxKernel<P>, PriorBoxParam<P>>, Runable, Creator, InferShaperable{
    
    func inferShape() {
    }
    
    typealias OpType = PriorBoxOp<P>
    func runImpl(device: MTLDevice, buffer: MTLCommandBuffer) throws {
        do {
             try kernel.compute(commandBuffer: buffer, param: para)
        } catch let error {
            throw error
        }
    }
}



