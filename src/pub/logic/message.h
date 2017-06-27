//  Copyright (c) 2015-2015 The KID Authors. All rights reserved.
//  Created on: 2017/1/9 Author: zjc

#ifndef SRC_PUB_LOGIC_USER_MESSAGE_H_
#define SRC_PUB_LOGIC_USER_MESSAGE_H_

#include <string>
#include <sstream>
#include <vector>

#include "basic/basictypes.h"
#include "user_defined_types.h"

namespace base_logic {
class Value;
class ListValue;
class DictionaryValue;
}

using base_logic::Value;
using base_logic::ListValue;
using base_logic::DictionaryValue;

namespace strade_user {

typedef uint32 UserId;
typedef uint32 GroupId;

struct ReqHead {
  uint16 type;
  uint16 opcode;
  UserId user_id;
  std::string token;

  ReqHead()
      : type(0),
        opcode(0),
        user_id(0) {}
  virtual ~ReqHead() {}
  bool StartDeserialize(DictionaryValue& dict);
  void StartDump(std::ostringstream& oss);
  virtual bool Deserialize(DictionaryValue& dict);
  virtual void Dump(std::ostringstream& oss);
};

struct Status {
  enum State {
    SUCCESS,
    FAILED,
    ERROR_MSG,
    UNKNOWN_OPCODE,
    USER_NOT_EXIST,
    INVALID_TOKEN,
    GROUP_NAME_ALREADAY_EXIST,
    MYSQL_ERROR,
    GROUP_NOT_EXIST,
    STOCK_NOT_IN_GROUP,
    STOCK_NOT_EXIST,
    CAPITAL_NOT_ENOUGH,
    NO_HOLDING_STOCK,
    ORDER_NOT_EXIST
  };

  Status() : state(SUCCESS) { }
  Status(State s)
      : state(s) { }
  State state;
  std::string to_string();
  bool Serialize(DictionaryValue& dict);
};

struct ResHead {
  virtual ~ResHead() { }
  Status status;
  bool StartSerialize(DictionaryValue& dict);
  virtual bool Serialize(DictionaryValue& dict);
};

///////////////////////////////////////////////////////////////////////////////
// create stock group
struct CreateGroupReq : ReqHead {
  const static uint32 ID = 101;

  std::string group_name;
  StockCodeList code_list;
  bool Deserialize(DictionaryValue& dict);
  void Dump(std::ostringstream& oss);
};

struct CreateGroupRes : ResHead {
  GroupId group_id;
  bool Serialize(DictionaryValue& dict);
};

///////////////////////////////////////////////////////////////////////////////
// add stock
struct AddStockReq : ReqHead {
  const static uint32 ID = 102;

  GroupId group_id;
  StockCodeList code_list;
  bool Deserialize(DictionaryValue& dict);
  void Dump(std::ostringstream& oss);
};

// del stock
struct DelStockReq : ReqHead {
  const static uint32 ID = 103;

  GroupId group_id;
  StockCodeList code_list;

  bool Deserialize(DictionaryValue& dict);
  void Dump(std::ostringstream& oss);
};

// query group
struct QueryGroupReq : ReqHead {
  const static uint32 ID = 104;

  bool Deserialize(DictionaryValue& dict);
  void Dump(std::ostringstream& oss);
};

struct QueryGroupsRes : ResHead {
  struct GroupInfo {
    GroupId id;
    std::string name;
    bool Serialize(DictionaryValue& dict);
  };
  typedef std::vector<GroupInfo> GroupList;
  GroupList group_list;

  bool Serialize(DictionaryValue& dict);
};

// query stock
struct QueryStocksReq : ReqHead {
  const static uint32 ID = 105;

  GroupId group_id;
  bool Deserialize(DictionaryValue& dict);
  void Dump(std::ostringstream& oss);
};

struct QueryStocksRes : ResHead {
  struct StockInfo {
    std::string code;
    std::string name;
    uint32 visit_heat;
    double price;
    double change;
    uint64 volume;          // 单位手
    std::string industry;   // 行业
    bool Serialize(DictionaryValue& dict);
  };
  typedef std::vector<StockInfo> StockList;

  StockList stock_list;

  bool Serialize(DictionaryValue& dict);
};

// query holding stocks
struct QueryHoldingStocksReq : ReqHead {
  const static uint32 ID = 106;

  bool Deserialize(DictionaryValue& dict);
  void Dump(std::ostringstream& oss);

};

struct QueryHoldingStocksRes : ResHead {
  struct StockInfo {
    std::string code;
    uint32 holding;
    uint32 available;
    double cost;
    double price;
    double market_value;
    double profit;
    double profit_ratio;
    double position;        // 仓位
    bool Serialize(DictionaryValue& dict);
  };
  typedef std::vector<StockInfo> StockList;
  StockList stock_list;

  bool Serialize(DictionaryValue& dict);
};

// query today orders
struct QueryTodayOrdersReq : ReqHead {
  const static uint32 ID = 107;

  bool Deserialize(DictionaryValue& dict);
  void Dump(std::ostringstream& oss);
};

struct QueryTodayOrdersRes : ResHead {
  struct OrderInfo {
    uint32 id;
    std::string code;
    std::string name;
    OrderOperation op;
    double order_price;
    uint32 order_nums;
    time_t order_time;
    OrderStatus status;
    bool Serialize(DictionaryValue& dict);
  };
  typedef std::vector<OrderInfo> OrderList;
  OrderList order_list;

  bool Serialize(DictionaryValue& dict);
};

// query today finished orders
struct QueryTodayFinishedOrdersReq : ReqHead {
  const static uint32 ID = 108;

  bool Deserialize(DictionaryValue& dict);
  void Dump(std::ostringstream& oss);
};

struct QueryTodayFinishedOrdersRes : ResHead {
  struct OrderInfo {
    std::string code;
    OrderOperation op;
    double order_price;
    uint32 order_nums;
    double amount;
    time_t order_time;
    bool Serialize(DictionaryValue& dict);
  };
  typedef std::vector<OrderInfo> OrderList;
  OrderList order_list;

  bool Serialize(DictionaryValue& dict);
};

// query history finished orders
struct QueryHistoryFinishedOrdersReq : ReqHead {
  const static uint32 ID = 109;

  std::string begin_time;
  std::string end_time;

  bool Deserialize(DictionaryValue& dict);
  void Dump(std::ostringstream& oss);
};

struct QueryHistoryFinishedOrdersRes : ResHead {
  struct OrderInfo {
    std::string code;
    OrderOperation op;
    double order_price;
    double order_nums;
    double amount;
    time_t order_time;
    bool Serialize(DictionaryValue& dict);
  };
  typedef std::vector<OrderInfo> OrderList;
  OrderList order_list;

  bool Serialize(DictionaryValue& dict);
};

// query statement
struct QueryStatementReq : ReqHead {
  const static uint32 ID = 110;
  std::string begin_time;
  std::string end_time;

  bool Deserialize(DictionaryValue& dict);
  void Dump(std::ostringstream& oss);
};

struct QueryStatementRes : ResHead {
  struct StatementRecord {
    std::string code;
    OrderOperation op;
    double order_price;
    uint32 order_nums;
    double commission;
    double stamp_duty;
    double transfer_fee;
    double amount;
    double available_capital;
    bool Serialize(DictionaryValue& dict);
  };
  typedef std::vector<StatementRecord> StatementRecordList;
  StatementRecordList statement_list;

  bool Serialize(DictionaryValue& dict);
};

///////////////////////////////////////////////////////////////////////////////

struct SubmitOrderReq : ReqHead {
  const static uint32 ID = 111;

  GroupId group_id;
  std::string code;
  double order_price;
  double expected_price;    // 止损或止盈价格
  uint32 order_nums;
  OrderOperation op;

  bool Deserialize(DictionaryValue& dict);
  void Dump(std::ostringstream& oss);
};

struct SubmitOrderRes : ResHead {
  OrderId order_id;

  bool Serialize(DictionaryValue& dict);
};

///////////////////////////////////////////////////////////////////////////////
struct GroupStockHoldingReq : ReqHead {
  const static uint32 ID = 112;
  GroupId group_id;

  bool Deserialize(DictionaryValue& dict);
  void Dump(std::ostringstream& oss);
};

struct GroupStockHoldingRes : ResHead {
  struct StockInfo {
    std::string code;
    std::string name;
    uint32 holding;
    bool Serialize(DictionaryValue& dict);
  };
  typedef std::vector<StockInfo> StockList;

  StockList stock_list;
  bool Serialize(DictionaryValue& dict);
};

///////////////////////////////////////////////////////////////////////////////
struct AvailableStockCountReq : ReqHead {
  const static uint32 ID = 113;

  std::string code;

  bool Deserialize(DictionaryValue& dict);
  void Dump(std::ostringstream& oss);
};

struct AvailableStockCountRes : ResHead {
  std::string code;
  std::string name;
  uint32 count;
  bool Serialize(DictionaryValue& dict);
};

///////////////////////////////////////////////////////////////////////////////
struct CancelOrderReq : ReqHead {
  const static uint32 ID = 114;

  OrderId order_id;
  bool Deserialize(DictionaryValue& dict);
  void Dump(std::ostringstream& oss);
};

struct ProfitAndLossOrderNumReq : ReqHead {
  const static uint32 ID = 115;

  bool Deserialize(DictionaryValue& dict);
  void Dump(std::ostringstream& oss);
};

struct ProfitAndLossOrderNumRes : ResHead {
  uint32 profit_num;
  uint32 loss_num;
  bool Serialize(DictionaryValue& dict);
};

}

#endif /* SRC_PUB_LOGIC_USER_MESSAGE_H_ */
