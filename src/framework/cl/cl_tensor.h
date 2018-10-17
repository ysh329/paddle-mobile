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

#include <memory>
#include <string>
#include <vector>

#include "CL/cl.h"
#include "framework/cl/cl_deleter.h"
#include "framework/cl/cl_engine.h"
#include "framework/tensor_base.h"

namespace paddle_mobile {
namespace framework {

class CLTensor : TensorBase {
 public:
  CLTensor(cl_context context, cl_command_queue command_queue)
      : context_(context), command_queue_(command_queue) {}

  CLTensor() = default;

  /*
   * if init method haven't set context and command_queue, need set
   * */
  void SetContextAndCommandQueue(cl_context context,
                                 cl_command_queue command_queue) {
    context_ = context;
    command_queue_ = command_queue;
  }

  /*! Resize the dimensions of the memory block. */
  inline CLTensor &Resize(const DDim &dims) {
    dims_ = dims;
    return *this;
  }

  template <typename T>
  inline T mutable_with_data(void *data) {
    int64_t size = numel() * sizeof(float);
    holder_.reset(
        new PlaceholderImpl(size, data, typeid(T), context_, command_queue_));
    return reinterpret_cast<T>(
        reinterpret_cast<void *>(reinterpret_cast<uintptr_t>(holder_->ptr())));
  }

  inline void *mutable_data(std::type_index type) {
    if (holder_ != nullptr) {
      holder_->set_type(type);
    }
    PADDLE_MOBILE_ENFORCE(numel() >= 0, "the Tensor's numel must >=0.")
    int64_t size = numel() * SizeOfType(type);
    if (holder_ == nullptr || holder_->size() < size + offset_) {
      holder_.reset(new PlaceholderImpl(size, type, context_, command_queue_));
      offset_ = 0;
    }
    return reinterpret_cast<void *>(
        reinterpret_cast<uintptr_t>(holder_->ptr()) + offset_);
  }

  /**
   * @brief   Return a pointer to mutable memory block.
   * @note    If not exist, then allocation.
   */
  template <typename T>
  inline T *mutable_data() {
    static_assert(std::is_pod<T>::value, "T must be POD");
    return reinterpret_cast<T *>(mutable_data(typeid(T)));
  }

  /**
   * @brief     Return a pointer to mutable memory block.
   *
   * @param[in] dims    The dimensions of the memory block.
   * @param[in] place   The place of the memory block.
   *
   * @note      If not exist, then allocation.
   */
  template <typename T>
  inline T *mutable_data(DDim dims) {
    static_assert(std::is_pod<T>::value, "T must be POD");
    Resize(dims);
    return mutable_data<T>();
  }

 private:
  cl_context context_;
  cl_command_queue command_queue_;

  /*
   *   virtual ~Placeholder() = default;

    virtual void *ptr() const = 0;

    virtual size_t size() const = 0;

    virtual std::type_index type() const = 0;

    virtual void set_type(std::type_index type) = 0;
   * */
  struct PlaceholderImpl : public Placeholder {
    PlaceholderImpl(size_t size, void *input, std::type_index type,
                    cl_context context, cl_command_queue command_queue)
        : ptr_(clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
                              size, reinterpret_cast<void *>(input), NULL)),
          size_(size),
          type_(type),
          command_queue_(command_queue) {}

    PlaceholderImpl(size_t size, std::type_index type, cl_context context,
                    cl_command_queue command_queue)
        : ptr_(clCreateBuffer(context, CL_MEM_READ_WRITE, size, NULL, NULL)),
          size_(size),
          type_(type),
          command_queue_(command_queue) {}

    virtual size_t size() const { return size_; }

    virtual void *ptr() const {
      if (host_ptr_) {
        delete (host_ptr_);
      }
      char *host_ptr = new char[size_];
      clEnqueueReadBuffer(command_queue_, ptr_.get(), CL_TRUE, 0, size_,
                          host_ptr, 0, NULL, NULL);
      return static_cast<void *>(host_ptr);
    }

    virtual std::type_index type() const { return type_; }

    virtual void set_type(std::type_index type) { type_ = type; }

    std::unique_ptr<_cl_mem, CLMemDeleter> ptr_;

    size_t size_;

    /* the current type of memory */
    std::type_index type_;

    cl_command_queue command_queue_;

    ~PlaceholderImpl() {
      if (host_ptr_) {
        delete (host_ptr_);
      }
    }

   private:
    void *host_ptr_;
  };
};

}  // namespace framework
}  // namespace paddle_mobile
