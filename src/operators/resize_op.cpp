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

#ifdef RESIZE_OP

#include "operators/resize_op.h"
#include <vector>
namespace paddle_mobile {
namespace operators {

template <typename Dtype, typename T>
void ResizeOp<Dtype, T>::InferShape() const {
  auto out_dims = CalOutputShape(this->param_);
  this->param_.Out()->Resize(out_dims);
}

}  // namespace operators
}  // namespace paddle_mobile

namespace ops = paddle_mobile::operators;
#ifdef PADDLE_MOBILE_CPU
REGISTER_OPERATOR_CPU(resize, ops::ResizeOp);
#endif
#ifdef PADDLE_MOBILE_MALI_GPU
REGISTER_OPERATOR_MALI_GPU(resize, ops::ResizeOp);
#endif
#ifdef PADDLE_MOBILE_X86
REGISTER_OPERATOR_X86(resize, ops::ResizeOp);
#endif

#endif
