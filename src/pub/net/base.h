//  Copyright (c) 2015-2015 The restful Authors. All rights reserved.
//  Created on: 2015/11/24 Author: jiaoyongqing

#ifndef _HOME_RUNNER_JINDOWIN_PUB_NET_BASE_H_
#define _HOME_RUNNER_JINDOWIN_PUB_NET_BASE_H_

#include "net/typedef.h"

namespace base {
class Base{
 public:
  Base() : error_info_(0) {}
  virtual ~Base() {}

 public:
  int get_error_info() { return error_info_; }

  bool GetDataHelp(time_t start_time, time_t end_time) {
    bool r = false;
    //  当前时间往前推2天的数据， 两个数据库都没有,
    //  最进两天的数据在redisComm数据库里面。
    time_t current_time = time(NULL);
    current_time = current_time - (current_time % 3600);
    time_t middle_time = current_time - REDIS_TIME * 3600;

    if (end_time < middle_time) {
      r = DataInMySql(start_time, end_time);
    } else if (start_time >= middle_time) {
      r = DataInRedis(start_time, end_time);
    } else {
      r = DataBetweenRedisAndMySql(start_time, end_time, middle_time);
    }
    return r;
  }

  virtual bool GetData() = 0;

  virtual void SendMessage() = 0;

  virtual bool DataInMySql(time_t start_time, time_t end_time) = 0;

  virtual bool DataInRedis(time_t start_time, time_t end_time) = 0;

  virtual bool DataBetweenRedisAndMySql(time_t start_time, \
                                          time_t end_time, \
                                       time_t middle_time) = 0;

 public:
  int error_info_;
};

}  // namespace base

#endif  //  _HOME_RUNNER_JINDOWIN_PUB_NET_BASE_H_
