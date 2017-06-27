#include <time.h>
#include <list>
#include <map>
#include <string>
#include "realtime_code_info.h"
#include "market_data_engine.h"
namespace stock_price {
typedef std::map<time_t, RealtimeCodeInfo> MinuteMap;
typedef std::map<std::string, RealtimeCodeInfo> StockLatestMap;

class RealMinuteInfo {
 public:
  bool CheckNewDayRecord(const struct tm &stamp);

  void UpdateRecord(time_t ts, const RealtimeCodeInfo &info) {
    minute_map_[ts] = info;
  }

  void DoNewDayWork() {
    ClearRecord();
  }

  void ClearRecord() {
    minute_map_.clear();
  }
  // last day kline
  void GetData(std::list<RealtimeCodeInfo> *code_info);

 private:
  // <ts, info>
  MinuteMap minute_map_;
  DataEngine *sh_engine_;
  DataEngine *sz_engine_;
};

typedef std::map<std::string, RealMinuteInfo> StockMinuteMap;

class RealInfoStockMinute {
 public:
  // save to latest map and minute map
  bool AddStockRecord(const std::string &date, RealtimeCodeInfo &info);
  // get current day kline data
  void GetDayData(const std::string &code,
                  std::list<RealtimeCodeInfo> *code_info);
  // latest data from redis
  bool GetLatestInfo(const std::string &code, RealtimeCodeInfo *info);
  // all stock codes data
  bool GetLatestAllInfo(std::map<std::string, RealtimeCodeInfo> *all_stock_mp);

 private:
  //<stock_code, minute_info>
  StockMinuteMap stock_minute_mp_;
  // <stock_code, info>
  StockLatestMap latest_mp_;
};

}
