/*
 * logic_unit.h
 *
 *  Created on: 2014年12月2日
 *      Author: kerry
 */
#ifndef _BABY_LOGIC_UNIT_H__
#define _BABY_LOGIC_UNIT_H__

#include <string>
#include "lbs/lbs_connector.h"
#include "logic/logic_comm.h"
#include "net/comm_head.h"
#include "basic/native_library.h"
#include <string>

namespace base_logic {

class SomeUtils {
 public:
  static void* GetLibraryFunction(const std::string& library_name,
                                  const std::string& func_name);
};

class LogicUnit {
 public:
  static void SendMessageBySize(const int socket, std::string& json);

};

class FuncTimeCount {
 public:
  FuncTimeCount(std::string func_name) {
    begin_time_ = (int) time(NULL);
    func_name_ = func_name;
    time_used_ = 0;
    func_id_ = 0;
    LOG_MSG2("func %s begin %d!", func_name.c_str(), begin_time_);
  }

  FuncTimeCount(int func_id) {
    func_name_ = "";
    func_id_ = func_id;
    time_used_ = 0;
    begin_time_ = (int) time(NULL);
    LOG_MSG2("func id %d begin %d!", func_id, begin_time_);
  }

  ~FuncTimeCount() {
    int current_time = (int) time(NULL);
    time_used_ = current_time - begin_time_;
    LOG_MSG2("func %s func_id=%d end,time_used_=%d", func_name_.c_str(), func_id_, time_used_);
  }

  std::string func_name_;
  int func_id_;
  int time_used_;
  int begin_time_;
};

}

#define send_message_by_size  base_logic::LogicUnit::SendMessageBySize

#endif

