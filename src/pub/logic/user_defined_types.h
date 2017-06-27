//  Copyright (c) 2015-2015 The KID Authors. All rights reserved.
//  Created on: 2017/1/10 Author: zjc

#ifndef SRC_PUB_LOGIC_USER_DEFINED_TYPES_H_
#define SRC_PUB_LOGIC_USER_DEFINED_TYPES_H_

#include <string>
#include <vector>
#include <set>

#include "basic/basictypes.h"

namespace strade_user {

typedef uint32 UserId;
typedef uint32 GroupId;
typedef uint32 StockPositionId;
typedef uint32 PlatformId;
typedef uint32 UserLevel;
typedef uint32 OrderId;

#define INVALID_USERID      ((UserId)-1)
#define INVALID_GROUPID     ((GroupId)-1)

typedef std::vector<std::string> StockCodeList;
typedef std::set<std::string> StockCodeSet;

enum OrderOperation {
  BUY,
  SELL
};

enum OrderStatus {
  PENDING,
  FINISHED,
  CANCEL      // 撤销
};

template <class T>
class EqualTo {
 public:
  EqualTo(const T& val)
      : val_(val) {
  }
  bool operator()(T val) {
    return val == val_;
  }
 private:
  T val_;
};

}

#endif /* SRC_PUB_LOGIC_USER_DEFINED_TYPES_H_ */
