//  Copyright (c) 2015-2015 The KID Authors. All rights reserved.
//  Created on: 2017/1/10 Author: zjc

#ifndef SRC_PUB_LOGIC_STOCK_POSITION_H_
#define SRC_PUB_LOGIC_STOCK_POSITION_H_

#include <string>
#include <vector>

#include <mysql.h>

#include "macros.h"
#include "user_defined_types.h"
#include "logic/order_info.h"
#include "dao/abstract_dao.h"

namespace strade_user {

class OrderInfo;
class FakeStockPosition;
class GroupStockPosition;

typedef std::vector<FakeStockPosition> FakeStockPositionList;
typedef std::vector<GroupStockPosition> GroupStockPositionList;

class FakeStockPosition : public base_logic::AbstractDao {
 public:
  enum {
    HOLDING_ID,
    USER_ID,
    STOCK,
    NUM,
    GROUP_ID,
    ORDER_ID
  };

  static bool comp(const FakeStockPosition& l, const FakeStockPosition& r) {
    return l.order()->deal_price() < r.order()->deal_price();
  }

  static std::string GetFakeStockPositionSql(UserId user_id,
                                             GroupId group_id,
                                             std::string stock);
 public:
  FakeStockPosition();
  FakeStockPosition(StockPositionId id, OrderInfo* order);
  REFCOUNT_DECLARE(FakeStockPosition);
 public:
  void BindOrder(OrderInfo* order) { data_->order_ = order; }
 private:
  void Deserialize();
public:
  StockPositionId id() const { return data_->id_; }
  OrderId order_id() const { return data_->order_id_; }
  OrderInfo* order() const { return data_->order_; }
  uint32 count() const { return data_->count_; }
  bool initialized() const { return data_->initialized_; }
  int32 sell(int32 n) {
    int32 rc = (data_->count_ -= n);
    if (data_->count_ < 0) data_->count_;
    return rc;
  }
 private:
  class Data {
     public:
      Data()
          : refcount_(1),
            id_(-1),
            order_id_(0),
            count_(0),
            order_(NULL),
            initialized_(false) {
      }

     public:
      StockPositionId id_;
      OrderId order_id_;
      int32 count_;
      OrderInfo* order_;
      bool initialized_;

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


class GroupStockPosition : public base_logic::AbstractDao {
 public:
  static std::string GetGroupStockPositionSql(UserId user_id);
 public:
  GroupStockPosition();
  GroupStockPosition(UserId user_id,
                     GroupId group_id,
                     const std::string& code);
  REFCOUNT_DECLARE(GroupStockPosition);
 public:
  bool InitFakeStockPosition();
  bool AddFakeStockPosition(const FakeStockPosition& p);
  bool Delegate(uint32 n);
  void OnOrderDone(uint32 n, FakeStockPositionList& fp_list);
  void OnOrderCancel(uint32 n);
 private:
  void Deserialize();
 public:
  void set_user_id(UserId id) { data_->user_id_ = id; }
  UserId user_id() const { return data_->user_id_; }

  void set_group_id(GroupId id) { data_->group_id_ = id; }
  GroupId group_id() const { return data_->group_id_; }

  std::string code() const { return data_->code_; }
  uint32 count() const { return data_->count_; }
  uint32 available() const { return data_->available_; }
  double cost() const;
  bool initialized() const { return data_->initialized_; }
 private:
  class Data {
   public:
    Data()
        : refcount_(1),
          user_id_(0),
          group_id_(0),
          count_(0),
          available_(0),
          initialized_(false) {
    }

   public:
    UserId user_id_;
    GroupId group_id_;
    std::string code_;
    uint32 count_;
    uint32 available_;
    bool initialized_;
    FakeStockPositionList fake_stock_position_list_;

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
  friend class UserInfo;
  Data* data_;
};

class StockPosition {
 public:
  StockPosition(const std::string& code, uint32 count);
  REFCOUNT_DECLARE(StockPosition);
 private:
  class Data {
   public:
    Data()
        : refcount_(1),
          count_(0) {
    }

   public:
    std::string code_;
    uint32 count_;
    GroupStockPositionList group_stock_position_list_;

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

} /* namespace strade_user */

#endif /* SRC_PUB_LOGIC_STOCK_POSITION_H_ */
