//  Copyright (c) 2015-2015 The KID Authors. All rights reserved.
//  Created on: 2016/10/13 Author: zjc

#ifndef SRC_PLUGINS_STOCK_MARKET_DATA_ENGINE_H_
#define SRC_PLUGINS_STOCK_MARKET_DATA_ENGINE_H_

#include <list>
#include <set>
#include <string>
#include <map>
#include <vector>

#include "realtime_code_info.h"

namespace stock_price {
class RedisController;
}

namespace stock_price {

class DataEngine {
 public:
  typedef std::map<std::string, RealtimeCodeInfo*> CodeMap;
  typedef std::map<std::string, RealtimeCodeInfo> CodeMapTemp;
  typedef std::map<int, CodeMap> DataMap;
  typedef std::vector<std::string> StringArray;
  typedef std::vector<StringArray> StringArray2;
  typedef std::vector<int> TimeList;
  typedef std::vector<RealtimeCodeInfo> CodeInfoArray;
  typedef std::map<std::string, CodeInfoArray> CodeKlineMap;
  enum Type {
    SH,
    SZ,
    MAX_TYPE
  };

  enum DbTable {
    TODAY_STOCK,
    TODAY_STOCK_ALL_INFO
  };

 public:
  void Init(Type type, const std::string& hkey_prefix, RedisController* redis, bool only_fetch_latest);
  bool FetchDayData(const std::string &data_str,
                    std::map<std::string, CodeInfoArray> *day_map);
  DataEngine();
  ~DataEngine();

  void GetKLineData(const std::string &code, CodeInfoArray &code_info);
  void UpdataWhenBoot();
  bool Update(std::string date, std::map<std::string, CodeInfoArray> *day_map);

 private:
  // fetch the last data of each minute
  void GetDistinctMinMaxKeys(const StringArray &origin, StringArray *distinct);

  bool FilterNewData(const StringArray& all_times, StringArray& new_times);

  bool FetchRawData(const std::string& date, const std::string& market_time,
                    CodeInfoArray& res);
  bool Parse(const std::string& info, StringArray2& res);
  bool ParseLine(const std::string& line, StringArray& res);

  void ProcessIndex(RealtimeCodeInfo& info);

 private:
  const static char kShIndex[];           // 涓婅瘉鎸囨暟
  const static char kShSz300[];           // 娌繁300
  const static char kSzComponentIndex[];  // 娣辫瘉鎴愭寚
  const static char kGEMIndex[];          // 鍒涗笟鏉挎寚

  const static int CODE_COUNT = 2000;
  const static int BUFFER_DAYS = 1;
  const static int MEM_POOL_SIZE = CODE_COUNT * sizeof(RealtimeCodeInfo);
  const static int START_WORK_TIME = 925;
  const static int END_WORK_TIME = 1510;
  const static int MORNING_START = 93000;
  const static int MORNING_END = 113000;
  const static int AFTERNOON_START = 130000;
  const static int AFTERNOOT_END = 150000;

 private:
  Type type_;
  std::string hkey_prefix_;

  RedisController* redis_;

  std::set<std::string> fetched_keys_;
  std::string trade_date_;
  bool only_fetch_latest_;
};

} /* namespace stock_price */

#endif /* SRC_PLUGINS_STOCK_MARKET_DATA_ENGINE_H_ */
