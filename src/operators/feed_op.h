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

#pragma once

#include <string>
#include "framework/operator.h"
#include "operators/kernel/feed_kernel.h"
#include "operators/op_param.h"

namespace paddle_mobile {
namespace operators {
using std::string;
#ifdef PADDLE_MOBILE_CL
template <typename DeviceType, typename T>
class FeedOp
    : public framework::OperatorWithKernel<DeviceType, FeedParam<DeviceType>,
                                           FeedKernel<DeviceType, T>> {
 public:
  FeedOp(const string &type, const VariableNameMap &inputs,
         const VariableNameMap &outputs, const framework::AttributeMap attrs,
         std::shared_ptr<framework::Scope> scope)
      : framework::OperatorWithKernel<DeviceType, FeedParam<DeviceType>,
                                      FeedKernel<DeviceType, T>>(
            type, inputs, outputs, attrs, scope) {}

  void InferShape() const override;

  void RunImpl() override;

 protected:
};
#else
template <typename DeviceType, typename T>
class FeedOp : public framework::OperatorBase<DeviceType> {
 public:
  FeedOp(const string &type, const VariableNameMap &inputs,
         const VariableNameMap &outputs, const framework::AttributeMap attrs,
         std::shared_ptr<framework::Scope> scope)
      : framework::OperatorBase<DeviceType>(type, inputs, outputs, attrs,
                                            scope),
        param_(inputs, outputs, attrs, scope.get()) {}

  void InferShape() const {
    auto out_dims = param_.Out()->dims();
    out_dims[0] = param_.BatchSize();
    param_.Out()->Resize(out_dims);

    //  note : mobile infershape iscalled when executer is created.  so  do not
    //  pass lod here .
    // it is empty
  }

#ifdef PADDLE_MOBILE_FPGA

  void Init() {
    Tensor *output = param_.Out();
    fpga::format_fp16_ofm(output);
  }

  void RunImpl() const {
    auto input =
        reinterpret_cast<Tensor *>(const_cast<LoDTensor *>(param_.InputX()));
    auto input_ptr = input->data<float>();
    fpga::format_image(input);
    Tensor *output = param_.Out();
    auto output_ptr = output->data<float>();

    fpga::BypassArgs args = {fpga::DATA_TYPE_FP32};

    args.input_data_type = fpga::DATA_TYPE_FP32;
    args.output_data_type = fpga::DATA_TYPE_FP16;
    args.input_layout_type = fpga::LAYOUT_CHW;
    args.output_layout_type = fpga::LAYOUT_HWC;
    args.image.address = reinterpret_cast<void *>(input_ptr);
    args.image.channels = (uint32_t)input->dims()[1];
    args.image.height = (uint32_t)input->dims()[2];
    args.image.width = (uint32_t)input->dims()[3];
    args.image.pad_height = 0;
    args.image.pad_width = 0;
    args.output.address = output_ptr;
    args.output.scale_address = output->scale;
    fpga::PerformBypass(args);
  }

#else

  void Init() {}
  void RunImpl() {
                    param_.Out()->ShareDataWith(*param_.InputX());
                    param_.Out()->set_lod(param_.InputX()->lod());
  }

 protected:
  FeedParam<DeviceType> param_;
};

#endif
#endif

}  // namespace operators
}  // namespace paddle_mobile

#ifdef PADDLE_MOBILE_CPU
USE_OP_CPU(feed);
#endif
#ifdef PADDLE_MOBILE_MALI_GPU
USE_OP_MALI_GPU(feed);
#endif
#ifdef PADDLE_MOBILE_FPGA
USE_OP_FPGA(feed);
#endif
#ifdef PADDLE_MOBILE_CL
USE_OP_CL(feed);
#endif
