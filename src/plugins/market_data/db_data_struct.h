//  Copyright (c) 2015-2015 The KID Authors. All rights reserved.
//  Created on: 2017年1月24日 Author: zjc

#ifndef MARKET_DATA_DB_DATA_STRUCT_H_
#define MARKET_DATA_DB_DATA_STRUCT_H_

#include <time.h>

#include <vector>
#include <string>
#include <sstream>

namespace stock_db {

struct CodeRealtimeInfo {
  std::string code;
  std::string name;
  std::string date;
  double price_change;
  double change_percent;
  double price;
  double open;
  double high;
  double close;
  double low;
  double settlement;
  int type;
  time_t market_time;
  double volume;
  double amount;
  CodeRealtimeInfo()
      : price_change(0.0),
        change_percent(0.0),
        price(0.0),
        open(0.0),
        high(0.0),
        close(0.0),
        low(0.0),
        settlement(0.0),
        type(0),
        market_time(0.0),
        volume(0.0),
        amount(0.0) {
  }

        std::string Serialized() const {
                std::ostringstream oss;
                oss << "'" << code << "'" << ","
                << "'" << name << "'" << ","
                << change_percent << ","
                << price << ","
                << open << ","
                << high << ","
                << low << ","
                << settlement << ","
                << type << ","
                << market_time << ","
                << volume << ","
                << amount;
                return oss.str();
        }

};
typedef std::vector<CodeRealtimeInfo> CodeInfoArray;
}

#endif /* MARKET_DATA_DB_DATA_STRUCT_H_ */
