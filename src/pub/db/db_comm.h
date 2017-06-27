// Copyright (c) 2015-2015 The restful Authors. All rights reserved.
// Created on: 2015/11/24 Author: jiaoyongqin
#ifndef _HOME_RUNNER_JINDOWIN_PUB_DB_DB_COMM_H_
#define _HOME_RUNNER_JINDOWIN_PUB_DB_DB_COMM_H_

#include <mysql.h>
#include <list>
#include <string>

#include "logic/logic_basic_info.h"
#include "config/config.h"
#include "db/base_db_mysql_auto.h"
#include "storage/storage.h"
#include "net/typedef.h"

namespace db {
class DbSql {
 public:
  DbSql();
  ~DbSql();

 public:
  static void Init(std::list<base::ConnAddr> * const addrlist);
  static void Dest();

 public:
  uint64_t GetCodeSubscribeNum(std::string &code);
  uint64_t GetHyGnSubscribeNum(std::string &hy_gn, int64 operate);

  uint32_t CodeIfSubscribe(int64 user_id, std::string &code);
  uint32_t HyGnIfSubscribe(int64 user_id, std::string &hy_gn, int64 operate);

  std::string GetHyGnCode(std::string &hy_gn, int64 operate);

  void GetAllNameCode();

  void GetHy();

  void GetGn();

  void GetEvent();

  //  得到用户信息
  void GetUserInfo(int platform_id, const std::string &user_name);

  //  得到平台名字
  std::string GetPlatformName(int platform_id);

  //  注册用户密码
  void SetUserIfo(int platform_id, std::string user_name, std::string password);

  //  检查用户名是否已经存在数据库中
  std::string GetUserNameInfo(std::string user_name);

  //  检查user_id是否存在数据库中
  bool CheckUseridIfInSql(int user_id);

  int get_record_num() { return engine_->RecordCount(); }

  MYSQL_ROW NextRecord();
  int get_error_info() { return error_info_; }

 private:
  base_storage::DBStorageEngine* GetEntine();

 private:
  //  记录数据库的状态
  int error_info_;
  base_storage::DBStorageEngine* engine_;
  int record_num_;
};

}  // namespace db

#endif  //  _HOME_RUNNER_JINDOWIN_PUB_DB_DB_COMM_H_
