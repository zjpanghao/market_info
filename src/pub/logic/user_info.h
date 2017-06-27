//  Copyright (c) 2015-2015 The KID Authors. All rights reserved.
//  Created on: 2017/1/10 Author: zjc

#ifndef SRC_PUB_LOGIC_USER_INFO_H_
#define SRC_PUB_LOGIC_USER_INFO_H_

#include <vector>
#include <map>

#include "macros.h"
#include "user_defined_types.h"
#include "stock_group.h"
#include "order_filter.h"
#include "order_info.h"
#include "stock_position.h"
#include "message.h"
#include "thread/base_thread_lock.h"
#include "dao/abstract_dao.h"

namespace strade_user {

class UserInfo;
typedef std::vector<UserInfo> UserList;
typedef std::map<UserId, UserInfo> UserIdMap;

class UserInfo : public base_logic::AbstractDao {
 public:
  static const char kGetAllUserInfoSql[];
  enum {
    ID,
    NAME,
    PASSWORD,
    PLATFORM_ID,
    USER_LEVEL,
    EMAIL,
    PHONE,
    AVAILABLE_CAPITAL,
    FROZEN_CAPITAL
  };
 public:
  UserInfo();
  REFCOUNT_DECLARE(UserInfo);
 public:
  bool Init();
  Status::State CreateGroup(const std::string& name,
                      StockCodeList& code_list,
                      GroupId* id);

  StockGroup* GetGroup(GroupId group_id);

  // not include default group
  GroupStockPositionList GetAllGroupStockPosition();
  GroupStockPosition* GetGroupStockPosition(GroupId group_id, const std::string& code);
  Status::State AddStock(GroupId group_id, StockCodeList& code_list);
  Status::State DelStock(GroupId group_id, StockCodeList& code_list);
  StockGroupList GetAllGroups() const { return data_->stock_group_list_; }
  Status::State GetGroupStock(GroupId group_id, StockCodeList& stocks);
  // include default gruop
  GroupStockPositionList GetHoldingStocks();

  OrderList FindOrders(const OrderFilterList& filters);
  Status::State SubmitOrder(SubmitOrderReq& req);
  void OnOrderDone(OrderInfo* order);
  Status::State OnCancelOrder(OrderId order_id);

 private:
  void Deserialize();
  bool InitStockGroup();
  bool InitStockPosition();
  bool InitOrder();
  bool InitPendingOrder();
  bool InitFinishedOrder();
  void BindOrder();
  Status::State OnBuyOrder(SubmitOrderReq& req, double* frozen);
  Status::State OnSellOrder(SubmitOrderReq& req);
  bool OnBuyOrderDone(OrderInfo* order);
  bool OnSellOrderDone(OrderInfo* order);
  Status::State OnCancelBuyOrder(const OrderInfo* order);
  Status::State OnCancelSellOrder(const OrderInfo* order);
 public:
  UserId id() const { return data_->id_; }
  void set_id(UserId id) { data_->id_ = id; }

  std::string name() const { return data_->name_; }
  void set_name(const std::string& name) { data_->name_ = name; }

  std::string phone() const { return data_->phone_; }
  void set_phone(const std::string& phone) { data_->phone_ = phone; }

  double frozen_capital() const { return data_->frozen_capital_; }

  double available_capital() const { return data_->available_capital_; }
  void set_available_capital(double available_capital) { data_->available_capital_ = available_capital; }

  bool initialized() const { return data_->initialized_; }
 private:
  class Data {
   public:
    Data()
        : refcount_(1),
          id_(0),
          default_gid_(INVALID_GROUPID),
          platform_id_(-1),
          level_(-1),
          available_capital_(0.0),
          frozen_capital_(0.0),
          lock_(NULL),
          initialized_(false) {
      InitThreadrw(&lock_);
    }

    ~Data() {
      DeinitThreadrw(lock_);
    }
   public:
    UserId id_;
    GroupId default_gid_;
    PlatformId platform_id_;
    UserLevel level_;
    std::string name_;
    std::string password_;
    std::string phone_;
    std::string email_;

    bool initialized_;
    double available_capital_;  // 可用资金
    double frozen_capital_;     // 冻结资金

    StockGroupList stock_group_list_;         // 股票组合
    GroupStockPositionList stock_position_list_;   // 当前持仓
    OrderList order_list_;                    // 委托

    threadrw_t *lock_;
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

#endif /* SRC_PUB_LOGIC_USERINFO_H_ */
