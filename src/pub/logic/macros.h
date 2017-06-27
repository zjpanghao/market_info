//  Copyright (c) 2015-2015 The KID Authors. All rights reserved.
//  Created on: 2017/1/12 Author: zjc

#ifndef SRC_PUB_LOGIC_MACROS_H_
#define SRC_PUB_LOGIC_MACROS_H_

#define REFCOUNT_DECLARE(TypeNmae)            \
  TypeNmae(const TypeNmae& other);            \
  TypeNmae& operator=(const TypeNmae& other); \
  ~TypeNmae()

#define REFCOUNT_DEFINE(TypeNmae)             \
  TypeNmae::TypeNmae(const TypeNmae& other)   \
      : data_(other.data_) {                  \
    data_->AddRef();                          \
  }                                           \
                                              \
  TypeNmae& TypeNmae::operator=(              \
      const TypeNmae& other) {                \
    if (&other == this) {                     \
      return *this;                           \
    }                                         \
    data_->Release();                         \
    data_ = other.data_;                      \
    data_->AddRef();                          \
    return *this;                             \
  }                                           \
                                              \
  TypeNmae::~TypeNmae() {                     \
    data_->Release();                         \
  }

#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
  TypeName(const TypeName&);               \
  void operator=(const TypeName&)

// 过户费
#define IS_SH_CODE(code)  ('6' == code[0])
#define TRANSFER_FEE(num) ((int(num)+999)/1000)

// 佣金
#define COMMISSION_RATE  0.003
#define ROUND_COMMISSION(commision) ((((int)((double)(commision) + 4.999))/5)*5)

// 印花税
#define STAMP_DUTY_RATE 0.001

#endif /* SRC_PUB_LOGIC_MACROS_H_ */
