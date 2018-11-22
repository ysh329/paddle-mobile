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

#ifdef CONCAT_OP

#include "operators/kernel/concat_kernel.h"

namespace paddle_mobile {
namespace operators {

template <>
bool ConcatKernel<GPU_CL, float>::Init(ConcatParam<GPU_CL> *param) {
  if (param->Out()->dims().size() < 4) {
    this->cl_helper_.AddKernel("concatByH", "concat_kernel.cl");
  }
  return true;
}

template <>
void ConcatKernel<GPU_CL, float>::Compute(const ConcatParam<GPU_CL> &param) {
  if (param.Out()->dims().size() < 4) {
    auto kernel = this->cl_helper_.KernelAt(0);
    auto inputs = param.Inputs();
    auto *output_image = param.Out()->GetCLImage();
    int out_W = 0;
    if (param.Out()->dims().size() == 3) {
      out_W = param.Out()->dims()[2];
    } else if (param.Out()->dims().size() == 2) {
      out_W = param.Out()->dims()[1];
    }
    int out_H_Start = 0;
    for (int i = 0; i < inputs.size(); i++) {
      auto input_image = inputs[i]->GetCLImage();
      auto default_work_size = this->cl_helper_.DefaultWorkSize(*inputs[i]);
      cl_int status;
      status = clSetKernelArg(kernel, 0, sizeof(cl_mem), &input_image);
      CL_CHECK_ERRORS(status);
      status = clSetKernelArg(kernel, 1, sizeof(cl_mem), &output_image);
      CL_CHECK_ERRORS(status);
      status = clSetKernelArg(kernel, 2, sizeof(int), &out_W);
      CL_CHECK_ERRORS(status);
      status = clSetKernelArg(kernel, 3, sizeof(int), &out_H_Start);
      CL_CHECK_ERRORS(status);
      status = clEnqueueNDRangeKernel(
          this->cl_helper_.CLCommandQueue(), kernel, default_work_size.size(),
          NULL, default_work_size.data(), NULL, 0, NULL, NULL);
      CL_CHECK_ERRORS(status);
      if (param.Out()->dims().size() == 3) {
        out_H_Start += inputs[i]->dims()[1];
      } else if (param.Out()->dims().size() == 2) {
        out_H_Start += inputs[i]->dims()[0];
      }
    }
  }
}

}  // namespace operators
}  // namespace paddle_mobile

#endif
