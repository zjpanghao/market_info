//  Copyright (c) 2015-2015 The KID Authors. All rights reserved.
//  Created on: 2016年10月13日 Author: zjc

#include "realtime_code_info.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include <sstream>

#include "logic/logic_comm.h"

namespace stock_price {

#define TO_STRING(x)  # x

#define SET_ORDER(x)                                        \
  do {                                                      \
    orders[x-1].buy = atof(data[BUY##x].c_str());             \
    orders[x-1].buy_vol = atof(data[BUY##x##VOL].c_str());   \
    orders[x-1].sale = atof(data[SAL##x].c_str());            \
    orders[x-1].sale_vol = atof(data[SAL##x##VOL].c_str());  \
  } while (0)

#define PRINT(x)\
  oss << #x << ':' << x << ',';

#define PRINT_ORDER(x)\
    do {\
      oss << TO_STRING(buy ## x) << ':' << orders[x].buy << ',';\
      oss << TO_STRING(buy_vol ## x) << ':' << orders[x].buy_vol << ',';\
      oss << TO_STRING(sal ## x) << ':' << orders[x].sale << ',';\
      oss << TO_STRING(sal_vol ## x) << ':' << orders[x].sale_vol << ',';\
    } while (0)

double RealtimeCodeInfo::MIN_PRICE = 1.00;
double RealtimeCodeInfo::MIN_PRICE_RISE = -11.00;
double RealtimeCodeInfo::MAX_PRICE_RISE = 45.00;

bool RealtimeCodeInfo::Valid() {
  if (price < MIN_PRICE) {
    return false;
  }
  if (high < MIN_PRICE) {
    return false;
  }
  if (low < MIN_PRICE) {
    return false;
  }
  if (close < MIN_PRICE) {
    return false;
  }

  double change_percent = 0.0;
  if (0.0 != close) {
    change_percent = price / close - 1.0;
    change_percent *= 100;
  }

  if (!(change_percent >= MIN_PRICE_RISE
      && change_percent <= MAX_PRICE_RISE)) {
    return false;
  }

  return true;
}

bool RealtimeCodeInfo::Delist() {
//  if (price >= 0.1) {
//    return false;
//  }
//  if (open >= 0.1) {
//    return false;
//  }
//  if (high >= 0.1) {
//    return false;
//  }
//  if (low >= 0.1) {
//    return false;
//  }
//  if (amount >= 0.1) {
//    return false;
//  }
//
//  for (size_t i = 0; i < 10; ++i) {
//    if (orders[i].buy >= 0.1)
//      return false;
//
//    if (orders[i].sale >= 0.1)
//      return false;
//  }

  if (open < 0.1) {
    return true;
  }

  return false;
}

bool RealtimeCodeInfo::Deserialize(const StrVec& data) {
  if (data.size() < MAX_FIELD) {
    return false;
  }

  code = data[CODE];
  name = data[NAME];
  price = atof(data[PRICE].c_str());
  open = atof(data[OPEN].c_str());
  high = atof(data[HIGH].c_str());
  low = atof(data[LOW].c_str());
  close = atof(data[CLOSE].c_str());
  amount = atof(data[AMOUNT].c_str());
  vol = atof(data[VOL].c_str());
  if (open > 0.01) {
    upper = close * 1.1;
    lower = close * 0.9;
  }

//  orders[0].buy = atof(data[BUY1].c_str());
//  orders[0].buy_vol = atoll(data[BUY1VOL].c_str());
//  orders[0].sale = atof(data[SAL1].c_str());
//  orders[0].sale_vol = atoll(data[SAL1VOL].c_str());
  SET_ORDER(1);
  SET_ORDER(2);
  SET_ORDER(3);
  SET_ORDER(4);
  SET_ORDER(5);

  //tradetime = atoi(data[TRADETIME].c_str());

  return true;
}

std::string RealtimeCodeInfo::Serialized() {
  std::ostringstream oss;
  double change_percent = 0.0;
  if (0.0 != close) {
    change_percent = price / close - 1.0;
    change_percent *= 100;
  }

  PRINT(code);
//  PRINT(name);
  PRINT(price);
  PRINT(close);
  PRINT(change_percent);
  PRINT(open);
  PRINT(high);
  PRINT(low);
  PRINT(amount);
  PRINT(vol);

  PRINT_ORDER(0);
  PRINT_ORDER(1);
  PRINT_ORDER(2);
  PRINT_ORDER(3);
  PRINT_ORDER(4);
  PRINT_ORDER(5);
  PRINT_ORDER(6);
  PRINT_ORDER(7);
  PRINT_ORDER(8);
  PRINT_ORDER(9);

  PRINT(oi);
  PRINT(poi);
  PRINT(settle);
  PRINT(upper);
  PRINT(lower);
  PRINT(tradetime);
  return oss.str();
}

DbCodeInfo RealtimeCodeInfo::CreateDbCodeInfo() {
  DbCodeInfo d;
  d.code = code;
  d.name = name;
  d.price_change = price - open;
  if (close > 0.0) {
    d.change_percent = price / close - 1.0;
    d.change_percent *= 100;
  }
  d.settlement = close;
  if (Delist()) {
    d.change_percent = 0.0;
    return d;
  }

  d.price = price;
  d.open = open;
  d.high = high;
  d.close = close;
  d.low = low;
  d.volume = vol;
  d.amount = amount;
  return d;
}

} /* namespace stock_price */
