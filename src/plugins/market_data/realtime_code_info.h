//  Copyright (c) 2015-2015 The KID Authors. All rights reserved.
//  Created on: 2016年10月13日 Author: zjc

#ifndef SRC_PLUGINS_STOCK_REALTIME_CODE_INFO_H_
#define SRC_PLUGINS_STOCK_REALTIME_CODE_INFO_H_

#include <vector>
#include <string>

#include "basic/basictypes.h"
#include "db_data_struct.h"

namespace stock_price {

typedef stock_db::CodeRealtimeInfo DbCodeInfo;

struct RealtimeCodeInfo {
  enum {
    CODE = 0,
    NAME,
    CLOSE,
    OPEN,
    AMOUNT,
    HIGH,
    LOW,
    PRICE,
    BUY1,
    SAL1,
    VOL,
    BUY1VOL,
    SAL1VOL,
    BUY2,
    BUY2VOL,
    BUY3,
    BUY3VOL,
    BUY4,
    BUY4VOL,
    BUY5,
    BUY5VOL,
    SAL2,
    SAL2VOL,
    SAL3,
    SAL3VOL,
    SAL4,
    SAL4VOL,
    SAL5,
    SAL5VOL,
    BUY6,
    BUY6VOL,
    SAL6,
    SAL6VOL,
    BUY7,
    BUY7VOL,
    SAL7,
    SAL7VOL,
    BUY8,
    BUY8VOL,
    SAL8,
    SAL8VOL,
    BUY9,
    BUY9VOL,
    SAL9,
    SAL9VOL,
    BUY10,
    BUY10VOL,
    SAL10,
    SAL10VOL,
    OI,
    SETTLE,
    POI,
    PSETTLE,
    UPPER,
    LOWER,
    TRADETIME,
    MAX_FIELD
  };

  typedef std::vector<std::string> StrVec;

  std::string code;
  std::string name;
  double price;
  double open;
  double high;
  double low;
  double close;
  double amount;
  double vol;

  struct Order {
    double buy;
    double buy_vol;
    double sale;
    double sale_vol;

    Order()
        : buy(0.0),
          buy_vol(0),
          sale(0.0),
          sale_vol(0) {

    }
  } orders[10];

  int oi;
  int poi;
  double settle;
  double psettle;
  double upper;
  double lower;
  int tradetime;
  time_t timestamp;

  RealtimeCodeInfo()
      : price(0.0),
        open(0.0),
        high(0.0),
        low(0.0),
        close(0.0),
        amount(0.0),
        vol(0),
        oi(0),
        poi(0),
        settle(0.0),
        psettle(0.0),
        upper(0.0),
        lower(0.0),
        tradetime(0),
        timestamp(0) {

  }

  static  double MIN_PRICE; //= 1.00;
  static  double MIN_PRICE_RISE; // = -11.00;
  static  double MAX_PRICE_RISE; // = 45.00;

  bool Deserialize(const StrVec& data);
  std::string Serialized();
  bool Valid();
  bool Delist();
  DbCodeInfo CreateDbCodeInfo();
};

} /* namespace stock_price */

#endif /* SRC_PLUGINS_STOCK_REALTIME_CODE_INFO_H_ */
