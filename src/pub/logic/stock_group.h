//  Copyright (c) 2015-2015 The KID Authors. All rights reserved.
//  Created on: 2017/1/10 Author: zjc

#ifndef SRC_PUB_LOGIC_STOCK_GROUP_H_
#define SRC_PUB_LOGIC_STOCK_GROUP_H_

#include <string>
#include <vector>

#include <mysql.h>

#include "macros.h"
#include "user_defined_types.h"
#include "dao/abstract_dao.h"

namespace strade_user {

class StockGroup : public base_logic::AbstractDao {
 public:
  enum Status {
    INVALID,
    VALID,
    DEFAULT
  };

  static std::string GetUserGroupSql(UserId user_id);
  static std::string GetGroupStockSql(GroupId group_id);
  static GroupId CreateGroup(UserId user_id,
                             const std::string& name,
                             Status status = VALID);
 public:
  StockGroup();
  StockGroup(UserId user_id, GroupId id, const std::string& name);
  REFCOUNT_DECLARE(StockGroup);
 public:
  bool Init(const MYSQL_ROW row);
  bool InitStockList();
  bool AddStocks(StockCodeList& stocks);
  bool DelStocks(StockCodeList& stocks);
  StockCodeList stocks() const { return data_->stock_list_; }
  bool exist_stock(const std::string& code) const {
    return data_->stock_set_.count(code);
  }
 private:
  void Deserialize();
 public:
  void set_id(UserId id) { data_->id_ = id; }
  UserId id() const { return data_->id_; }

  void set_name(const std::string& name) { data_->name_ = name; }
  std::string name() const { return data_->name_; }
  Status status() const { return data_->status_; }
  bool initialized() const { return data_->initialized_; }
 public:
  class Data {
   public:
    Data()
        : refcount_(1),
          id_(0),
          user_id_(0),
          status_(VALID),
          initialized_(false) {
    }

   public:
    GroupId id_;
    UserId user_id_;
    std::string name_;
    Status status_;
    bool initialized_;
    StockCodeList stock_list_;
    StockCodeSet stock_set_;

    void AddRef() {
      __sync_fetch_and_add(&refcount_, 1);
    }

    void Release() {
      __sync_fetch_and_sub(&refcount_, 1);
      if (!refcount_)
        delete this;
    }

   private:
    int refcount_;
  };
 private:
  Data* data_;
};

typedef std::vector<StockGroup> StockGroupList;

} /* namespace strade_user */

#endif /* SRC_PUB_LOGIC_STOCK_GROUP_H_ */
