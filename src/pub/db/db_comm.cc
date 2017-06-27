// Copyright (c) 2015-2015 The restful Authors. All rights reserved.
// Created on: 2015/11/24 Author: jiaoyongqing

#include "db/db_comm.h"

#include <string>
#include <list>
#include <sstream>
#include "tools/tools.h"
#include "net/typedef.h"
#include "net/error_comm.h"

namespace db {
void DbSql::Init(std::list<base::ConnAddr> *const addrlist) {
  base_db::MysqlDBPool::Init(*addrlist);
}

void DbSql::Dest() {
  base_db::MysqlDBPool::Dest();
}

DbSql::DbSql() {
  error_info_ = 0;
  record_num_ = 0;
  engine_ = base_db::MysqlDBPool::DBConnectionPop();
}

DbSql::~DbSql() {
  base_db::MysqlDBPool::DBConnectionPush(engine_);
}

base_storage::DBStorageEngine* DbSql::GetEntine() {
  error_info_ = 0;
  int num = 0;
  while (num < 5) {
    if (engine_) {
      engine_->FreeRes();
      if (!engine_->CheckConnect()) {
        LOG_DEBUG("lost connection");
        if (!engine_->Connections(base_db::MysqlDBPool::addrlist_)) {
          usleep(100);
          ++num;
        } else {
          break;
        }
      } else {
        break;
      }
    } else {
      LOG_DEBUG2("%s", "engine is NULL");
      return NULL;
    }
  }

  if (num >= 5) {
    LOG_ERROR("GetConnection Error");
    error_info_ = CONNECT_ERROR;
    return NULL;
  }
  return engine_;
}

uint64_t DbSql::GetHyGnSubscribeNum(std::string &hy_gn, int64 operate) {
  engine_ = GetEntine();
  if (NULL == engine_) return 0;

  std::stringstream os;
  if (operate == 0) {
    os << "call proc_GetHySubscribeNum(\'" << hy_gn << "\');";
  } else {
    os << "call proc_GetGnSubscribeNum(\'" << hy_gn << "\');";
  }
  std::string sql = os.str();

  LOG_DEBUG2("sql:%s", sql.c_str());
  bool r = engine_->SQLExec(sql.c_str());
  error_info_ = 0;
  if (!r) {
    LOG_ERROR("exec sql error");
    error_info_ = SQL_EXEC_ERROR;
    return 0;
  }

  int num = engine_->RecordCount();
  if (num <= 0) {
    return 0;
  }

  MYSQL_ROW rows = \
    (*reinterpret_cast<MYSQL_ROW*>((engine_->FetchRows())->proc));

  return atoll(rows[0]);
}


uint64_t DbSql::GetCodeSubscribeNum(std::string &code) {
  engine_ = GetEntine();
  if (NULL == engine_) return 0;

  std::stringstream os;
  os << "call proc_GetCodeSubscribeNum(\'" << code << "\');";
  std::string sql = os.str();

  LOG_DEBUG2("sql:%s", sql.c_str());
  bool r = engine_->SQLExec(sql.c_str());
  error_info_ = 0;
  if (!r) {
    LOG_ERROR("exec sql error");
    error_info_ = SQL_EXEC_ERROR;
    return 0;
  }

  int num = engine_->RecordCount();
  if (num <= 0) {
    return 0;
  }

  MYSQL_ROW rows = \
    (*reinterpret_cast<MYSQL_ROW*>((engine_->FetchRows())->proc));

  return atoll(rows[0]);
}

uint32_t DbSql::HyGnIfSubscribe\
(int64 user_id, std::string &hy_gn, int64 operate) {
  engine_ = GetEntine();
  if (NULL == engine_) return 0;

  std::stringstream os;
  if (operate == 0) {
    os << "call proc_HyIfSubscribe(" << user_id << ",\'" << hy_gn << "\');";
  } else {
    os << "call proc_GnIfSubscribe(" << user_id << ",\'" << hy_gn << "\');";
  }
  std::string sql = os.str();
  bool r = engine_->SQLExec(sql.c_str());
  error_info_ = 0;

  if (!r) {
    LOG_ERROR("exec sql error");
    error_info_ = SQL_EXEC_ERROR;
    return 0;
  }

  int num = engine_->RecordCount();
  if (num <= 0) {
    return 0;
  }

  return 1;
}

uint32_t DbSql::CodeIfSubscribe(int64 user_id, std::string &code) {
  engine_ = GetEntine();
  if (NULL == engine_) return 0;

  std::stringstream os;
  os << "call proc_CodeIfSubscribe(" << user_id << ",\'" << code << "\');";
  std::string sql = os.str();
  bool r = engine_->SQLExec(sql.c_str());
  error_info_ = 0;
  if (!r) {
    LOG_ERROR("exec sql error");
    error_info_ = SQL_EXEC_ERROR;
    return 0;
  }

  int num = engine_->RecordCount();
  if (num <= 0) {
    return 0;
  }

  return 1;
}

std::string DbSql::GetHyGnCode(std::string &hy_gn, int64 operate) {
  engine_ = GetEntine();
  if (NULL == engine_) return 0;

  std::stringstream os;
  if (operate == 0) {
    os << "call proc_GetHyCode(\'" << hy_gn << "\');";
  } else {
    os << "call proc_GetGnCode(\'" << hy_gn << "\');";
  }
  std::string sql = os.str();
  bool r = engine_->SQLExec(sql.c_str());
  error_info_ = 0;
  if (!r) {
    LOG_ERROR("exec sql error");
    error_info_ = SQL_EXEC_ERROR;
    return 0;
  }

  int num = engine_->RecordCount();
  if (num <= 0) {
    return "";
  }

  MYSQL_ROW rows = \
    (*reinterpret_cast<MYSQL_ROW*>((engine_->FetchRows())->proc));

  return rows[0];
}

void DbSql::GetAllNameCode() {
  engine_ = GetEntine();
  if (NULL == engine_) return;

  std::string sql = "call proc_GetAllNameCode()";
  bool r = engine_->SQLExec(sql.c_str());
  error_info_ = 0;
  if (!r) {
    LOG_ERROR("exec sql error");
    error_info_ = SQL_EXEC_ERROR;
    return;
  }
  record_num_ = engine_->RecordCount();
}


void DbSql::GetEvent() {
  engine_ = GetEntine();
  if (NULL == engine_) return;

  std::string sql = "call proc_GetEventS()";

  bool r = engine_->SQLExec(sql.c_str());
  error_info_ = 0;
  if (!r) {
    LOG_ERROR("exec sql error");
    error_info_ = SQL_EXEC_ERROR;
    return;
  }

  record_num_ = engine_->RecordCount();
}


void DbSql::GetHy() {
  engine_ = GetEntine();
  if (NULL == engine_) return;

  std::string sql = "call proc_GetHy()";
  bool r = engine_->SQLExec(sql.c_str());
  error_info_ = 0;
  if (!r) {
    LOG_ERROR("exec sql error");
    error_info_ = SQL_EXEC_ERROR;
    return;
  }
  record_num_ = engine_->RecordCount();
}

void DbSql::GetGn() {
  engine_ = GetEntine();
  if (NULL == engine_)
    return;

  std::string sql = "call proc_GetGn()";
  bool r = engine_->SQLExec(sql.c_str());
  error_info_ = 0;
  if (!r) {
    LOG_ERROR("exec sql error");
    error_info_ = SQL_EXEC_ERROR;
    return;
  }

  record_num_ = engine_->RecordCount();
}

void DbSql::GetUserInfo(int platform_id, const std::string &user_name) {
  engine_ = db::DbSql::GetEntine();
  std::stringstream os;

  os << "call proc_GetUserInfo(\'" << user_name
                                   << "\',"
                                   << platform_id \
                                   << ");";
  std::string sql = os.str();
  bool r = engine_->SQLExec(sql.c_str());
  error_info_ = 0;
  if (!r) {
    LOG_ERROR("exec sql error");
    error_info_ = SQL_EXEC_ERROR;
    return;
  }
  record_num_ = engine_->RecordCount();
  LOG_DEBUG2("\nDbSql::GetUserInfo-------record_num_: %d\n\n", record_num_);
  if (record_num_ > 1) {
    error_info_ = SQL_NUM_MORE_THAN_ONE;
  } else if (record_num_ <= 0) {
    error_info_ = SQL_NO_USER;
  }
  return;
}

std::string DbSql::GetPlatformName(int platform_id) {
  base_storage::DBStorageEngine* engine  = GetEntine();
  std::stringstream os;
  std::string sql;

  error_info_ = 0;
  os.str("");
  os << "call proc_GetPlatFormName(" << platform_id << ");";
  sql = os.str();
  bool r = engine->SQLExec(sql.c_str());
  if (false == r) {
    error_info_ = SQL_EXEC_ERROR;
  }

  int num = engine->RecordCount();
  if (num == 0) return "";

  MYSQL_ROW rows = (*reinterpret_cast<MYSQL_ROW*>((engine->FetchRows())->proc));

  return rows[0];
}

void DbSql::SetUserIfo(int platform_id, std::string user_name, \
                                         std::string password) {
  engine_ = GetEntine();
  std::stringstream os;

  os << "call proc_SetUserIfo(\'" << user_name
                                  << "\',"
                                  << "\'"
                                  << password
                                  << "\',"
                                  << platform_id \
                                  << ");";

  std::string sql = os.str();
  LOG_DEBUG2("sql:%s", sql.c_str());
  bool r = engine_->SQLExec(sql.c_str());
  error_info_ = 0;
  if (!r) {
    LOG_ERROR("exec sql error");
    error_info_ = SQL_EXEC_ERROR;
    return;
  }

  record_num_ = engine_->RecordCount();
  if (record_num_ > 1) {
    error_info_ = SQL_NUM_MORE_THAN_ONE;
  } else if (record_num_ <= 0) {
    error_info_ = SQL_NO_USER;
  }
}

std::string DbSql::GetUserNameInfo(std::string user_name) {
  base_storage::DBStorageEngine* engine  = GetEntine();
  std::stringstream os;
  std::string sql;

  error_info_ = 0;
  os.str("");
  os << "call proc_GetUserNameIfo(\'" << user_name << "\');";
  sql = os.str();
  bool r = engine->SQLExec(sql.c_str());
  if (false == r) {
    error_info_ = SQL_EXEC_ERROR;
  }

  int num = engine->RecordCount();
  if (num == 0)
    return "";

  MYSQL_ROW rows = (*reinterpret_cast<MYSQL_ROW*>((engine->FetchRows())->proc));

  return rows[1];
}

bool DbSql::CheckUseridIfInSql(int user_id) {
  engine_ = GetEntine();
  std::stringstream os;
  std::string sql;

  error_info_ = 0;
  os.str("");
  os << "call proc_CheckUserId(" << user_id << ");";
  sql = os.str();
  bool r = engine_->SQLExec(sql.c_str());
  //  LOG_DEBUG2("\n\DbSql::CheckUseridIfInSql-------r: %d\n\n",r);
  if (false == r) {
    error_info_ = SQL_EXEC_ERROR;
    return false;
  }

  int record_num_ = engine_->RecordCount();
  //  LOG_DEBUG2("\n\DbSql::CheckUseridIfInSql-------num: %d\n\n",record_num_);
  if (record_num_ <= 0)
    return false;
  else
    return true;
}

MYSQL_ROW DbSql::NextRecord() {
  if (record_num_<= 0) return NULL;
  --record_num_;
  MYSQL_ROW rows = \
    (*reinterpret_cast<MYSQL_ROW*>((engine_->FetchRows())->proc));
  return rows;
}

}  // namespace db
