//
// Created by Harvey on 2017/1/11.
//

#ifndef STRADE_ABSTRACT_DAO_H
#define STRADE_ABSTRACT_DAO_H

#include "logic/base_values.h"
#include "basic/basictypes.h"

#include <list>
#include <mysql/mysql.h>

namespace base_logic {

/*
 * 所有 Mysql 数据库结果集对象基类
 * */
class AbstractDao {
 public:
  AbstractDao();
  virtual ~AbstractDao();
  bool operator()(MYSQL_ROW& row);

 protected:
  bool GetInteger(int32 index, int32& out);
  bool GetInteger(int32 index, uint32& out);
  bool GetInteger(int32& out);

  bool GetReal(int32 index, double& value);
  bool GetReal(double& value);

  bool GetString(int32 index, std::string& out);
  bool GetString(std::string& out);

 private:
  // 获取结果集， GetXXX 的顺序必须与查询的顺序一致
  virtual void Deserialize() = 0;
  std::string PopColumnValue();

 private:
  int curr_index_;
  MYSQL_ROW row_;
};

} /* namespace base_logic */

#endif //STRADE_ABSTRACT_DAO_H
