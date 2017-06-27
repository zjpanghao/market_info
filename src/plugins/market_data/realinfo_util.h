#include "logic/logic_comm.h"
#include "realinfo_day.h"
namespace stock_price {
class RealInfoUtil {
 public:
  // eg  20170316094011  -> tm  , the return value ignore seconds
  static bool GetTimestamp(std::string full_date_str, struct tm *stamp) {
    if (full_date_str.length() == 0)
      return false;
    long trade_time = atol(full_date_str.c_str());
    int sec = trade_time % 100;
    trade_time /= 100;
    int min = trade_time % 100;
    trade_time /= 100;
    int hour = trade_time % 100;
    trade_time /= 100;
    int day = trade_time % 100;
    trade_time /= 100;
    int mon = trade_time % 100;
    int year = trade_time / 100;
    memset(stamp, 0, sizeof(struct tm));
    if (year < 1900 || mon < 1)
      return false;
    stamp->tm_year = year - 1900;
    stamp->tm_mon = mon - 1;
    stamp->tm_mday = day;
    stamp->tm_hour = hour;
    stamp->tm_min = min;
    stamp->tm_sec = 0;
    mktime(stamp);
    return true;
  }
  // valid time of the data from redis
  static bool IsWorkTime(std::string fetch_key) {
    int key = atoi(fetch_key.c_str());
    key /= 100;
    int min = key % 100;
    key /= 100;
    int hour = key;
    if (hour < 9 || hour > 15)
      return false;
    if (hour >= 12 && hour < 13)
      return false;
    if (hour == 9 && min < 15)
      return false;
    if (hour == 11 && min > 30)
      return false;
    if (hour == 15 && min > 0)
      return false;
    return true;
  }
  // date to stamp eg 20170316  ->  tm
  static bool GetStamp(std::string date, struct tm *stamp) {
    if (date.length() != 8)
      return false;
    int value = atoi(date.c_str());
    int day = value % 100;
    value /= 100;
    int month = value % 100;
    int year = value / 100;
    stamp->tm_year = year - 1900;
    stamp->tm_mon = month - 1;
    stamp->tm_mday = day;
    stamp->tm_hour = 0;
    stamp->tm_min = 0;
    stamp->tm_sec = 0;
    mktime(stamp);
    return true;
  }

  static bool IsWorkDay(std::string date) {
    if (RealInfoDay::GetInstance()->CheckCloseDay(date)) {
      return false;
    }
    struct tm stamp;
    GetStamp(date, &stamp);
    LOG_MSG2("The tm_wday is %d", stamp.tm_wday);
    return stamp.tm_wday < 6 && stamp.tm_wday >= 1;

  }
  // must  check worktime first, kline data not include 9:25-9:30
  static bool IsKlineData(const struct tm &stamp) {
    if (stamp.tm_hour == 9 && stamp.tm_min < 30) {
      return false;
    }
    return true;
  }
  // only reduce redis visiting
  static bool InScanHour(const struct tm &stamp) {
    if (stamp.tm_hour > 8 && stamp.tm_hour < 16) {
      return true;
    }
    return false;
  }
  // timestamp -> date eg  20170316
  static std::string BuildFetchDate(time_t ts) {
    struct tm now;
    localtime_r(&ts, &now);
    char buf[64];
    snprintf(buf, sizeof(buf), "%04d%02d%02d", now.tm_year + 1900,
             now.tm_mon + 1, now.tm_mday);
    return buf;
  }
};

}
