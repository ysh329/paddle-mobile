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

#ifdef FUSION_CONVADD_OP

#include "operators/kernel/conv_add_kernel.h"
#include "operators/kernel/arm/convolution/conv_common.h"
#include "operators/kernel/central-arm-func/conv_arm_func.h"
#include "operators/math/element_wise.h"

namespace paddle_mobile {
namespace operators {

template <>
bool ConvAddKernel<CPU, float>::Init(FusionConvAddParam<CPU> *param) {
  InitBaseConvKernel(param);
  return true;
}

template <>
void ConvAddKernel<CPU, float>::Compute(const FusionConvAddParam<CPU> &param) {
  switch (param.ExecMode()) {
    case ConvParam<CPU>::EXEC_DEPTHWISE3x3S1_FLOAT:
      break;
    case ConvParam<CPU>::EXEC_DEPTHWISE3x3S2_FLOAT:
      math::DepthwiseConv3x3S2<float, float>(*param.Input(), *param.Filter(),
                                             param.Paddings(), param.Output());
      break;
    case ConvParam<CPU>::EXEC_DEPTHWISE5x5_FLOAT:
      DepthwiseConv5x5<float, float>(param);
      break;
    case ConvParam<CPU>::EXEC_WINOGRAD3X3_FLOAT:
      WinogradConv3x3<8, 3>(param);
      break;
    case ConvParam<CPU>::EXEC_GEMM_FLOAT:
      GemmConv<float, float>(param);
      break;
    default:
      PADDLE_MOBILE_THROW_EXCEPTION("Invalid convolution execute mode %d",
                                    param.ExecMode());
  }
  math::AddChannelWise<IDENTITY>(param.Output(), param.Bias(), param.Output());
}

template class ConvAddKernel<CPU, float>;

}  // namespace operators
}  // namespace paddle_mobile

#endif
