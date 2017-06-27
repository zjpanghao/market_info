#include "realinfo_minute.h"
#include "realinfo_util.h"
#include "base/logic/logic_comm.h"
namespace stock_price {

bool RealMinuteInfo::CheckNewDayRecord(const struct tm &stamp) {
  MinuteMap::reverse_iterator it = minute_map_.rbegin();
  if (it == minute_map_.rend())
    return true;
  time_t ts = it->first;
  struct tm last_record;
  localtime_r(&ts, &last_record);

  return last_record.tm_mday != stamp.tm_mday;
}

void RealMinuteInfo::GetData(std::list<RealtimeCodeInfo> *code_info) {
  MinuteMap::iterator it = minute_map_.begin();
  while (it != minute_map_.end()) {
    code_info->push_back(it->second);
    it++;
  }
}

bool RealInfoStockMinute::AddStockRecord(const std::string &date,
                                         RealtimeCodeInfo &info) {
  std::string stock_code = info.code;
  struct tm stamp;
  if (!RealInfoUtil::GetTimestamp(date, &stamp)) {
    return false;
  }
  time_t ts = mktime(&stamp);
  info.timestamp = ts;
  latest_mp_[stock_code] = info;
  if (stock_code == "000002") {
    LOG_MSG2("ADD0 STOCK %d  high %.2f price %.2f", ts, info.high, info.price);
  }
  if (!RealInfoUtil::IsKlineData(stamp)) {
    return true;
  }
  StockMinuteMap::iterator it = stock_minute_mp_.find(stock_code);
  if (it == stock_minute_mp_.end()) {
    RealMinuteInfo minute_info;
    minute_info.UpdateRecord(ts, info);
    stock_minute_mp_[stock_code] = minute_info;
  } else {
    RealMinuteInfo &minute_info = it->second;
    if (minute_info.CheckNewDayRecord(stamp)) {
      minute_info.DoNewDayWork();
    }
    minute_info.UpdateRecord(ts, info);
  }
  return true;
}

void RealInfoStockMinute::GetDayData(const std::string &code,
                                     std::list<RealtimeCodeInfo> *code_info) {
  StockMinuteMap::iterator it = stock_minute_mp_.find(code);
  if (it == stock_minute_mp_.end()) {
    return;
  }
  RealMinuteInfo &minute_info = it->second;
  minute_info.GetData(code_info);
}

bool RealInfoStockMinute::GetLatestAllInfo(
    std::map<std::string, RealtimeCodeInfo> *all_stock_mp) {
  *all_stock_mp = latest_mp_;
  return true;
}

bool RealInfoStockMinute::GetLatestInfo(const std::string &code,
                                        RealtimeCodeInfo *info) {
  StockLatestMap::iterator it = latest_mp_.find(code);
  if (it == latest_mp_.end()) {
    return false;
  }
  *info = it->second;
  return true;
}

}  // namespace stock_price
