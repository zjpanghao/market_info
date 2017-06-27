//  Copyright (c) 2015-2015 The KID Authors. All rights reserved.
//  Created on: 2016/10/13 Author: zjc

#ifndef SRC_PLUGINS_STOCK_STOCK_UTIL_H_
#define SRC_PLUGINS_STOCK_STOCK_UTIL_H_

#include <time.h>

#include <set>
#include <string>
#include <vector>

#include "logic/logic_comm.h"

namespace stock_price {

class StockUtil {
 public:
  const static int MONTH_PER_YEAR = 12;
  static int MONTH_DAY[];

  StockUtil() {
    holiday_in_2016_.insert("2016-06-09");
    holiday_in_2016_.insert("2016-06-10");
    holiday_in_2016_.insert("2016-09-15");
    holiday_in_2016_.insert("2016-09-16");
    holiday_in_2016_.insert("2016-10-01");
    holiday_in_2016_.insert("2016-10-02");
    holiday_in_2016_.insert("2016-10-03");
    holiday_in_2016_.insert("2016-10-04");
    holiday_in_2016_.insert("2016-10-05");
    holiday_in_2016_.insert("2016-10-06");
    holiday_in_2016_.insert("2016-10-07");

  }

  static StockUtil* Instance() {
    if (NULL == instance_)
      instance_ = new StockUtil();
    return instance_;
  }

  int year() {
    time_t now = time(NULL);
    struct tm* t = localtime(&now);
    return t->tm_year + 1900;
  }

  bool leap_year(int year) {
    if ((0 == year%4 && 0 != year%100)
        || 0 == year%400) {
      return true;
    }
    return false;
  }

  int Date() {
    time_t now = time(NULL);
    struct tm* t = localtime(&now);
    int date = t->tm_year + 1900;
    date = date*100 + t->tm_mon+1;
    date = date*100 + t->tm_mday;
    return date;
  }

  int PreDate(int date) {
    --date;
    int day = date % 100;
    date /= 100;
    int mon = date % 100;
    int year = date / 100;
    if (leap_year(year)) {
      MONTH_DAY[2] = 29;
    }

    if (0 == day) {
      --mon;
      day = MONTH_DAY[mon % MONTH_PER_YEAR];
    }
    if (0 == mon) {
      --year;
      mon = 12;
    }

    int pre_date = year;
    pre_date = pre_date*100 + mon;
    pre_date = pre_date*100 + day;
    return pre_date;
  }

  int NextDate(int date) {
    ++date;
    int day = date % 100;
    date /= 100;
    int mon = date % 100;
    int year = date / 100;
    if (leap_year(year)) {
      MONTH_DAY[2] = 29;
    }


    if (day > MONTH_DAY[mon % MONTH_PER_YEAR]) {
      day = 1;
      ++mon;
    }

    if (mon > MONTH_PER_YEAR) {
      mon = 1;
      ++year;
    }

    int next_date = year;
    next_date = next_date*100 + mon;
    next_date = next_date*100 + day;
    return next_date;
  }

  int PreTradeDate(int date) {
    do {
      date = PreDate(date);
    } while (!trade_date(date));
    return date;
  }

  int NextTradeDate(int date) {
    do {
      date = NextDate(date);
    } while (!trade_date(date));
    return date;
  }

  // FORMAT: HHMMSS
  time_t to_timestamp(int mtime) {
    time_t now = time(NULL);
    struct tm* pt = localtime(&now);
    pt->tm_sec = mtime % 100;
    mtime /= 100;
    pt->tm_min = mtime % 100;
    pt->tm_hour = mtime / 100;
    return mktime(pt);
  }

  bool check_double_valid(double d) {
    double d2compare = d + 1;
    char d_str[20];
    char d2_str[20];
    sprintf(d_str, "%f", d);
    sprintf(d2_str, "%f", d2compare);
    return strcmp(d_str, d2_str) != 0;
  }

  std::string get_first_day_of_month() {
    std::string first_day_of_month;
    time_t current_time = time(NULL);
    struct tm* current_tm = localtime(&current_time);
    char str_time[20];
    sprintf(str_time, "%d-%02d-%02d", current_tm->tm_year + 1900,
            current_tm->tm_mon + 1, 1);
    first_day_of_month = str_time;
    return first_day_of_month;
  }

  std::string get_first_day_of_week() {
    std::string first_day_of_week;
    time_t current_time = time(NULL);
    struct tm* current_tm = localtime(&current_time);
    int week = current_tm->tm_wday;
    int diff = 0;
    if (0 == week)
      diff = 7 * 24 * 60 * 60;
    else
      diff = (week - 1) * 24 * 60 * 60;
    long seconds_of_first_day_in_week = current_time - diff;
    struct tm*first_day_in_week_tm = localtime(&seconds_of_first_day_in_week);
    char str_time[20];
    sprintf(str_time, "%d-%02d-%02d", first_day_in_week_tm->tm_year + 1900,
            first_day_in_week_tm->tm_mon + 1, first_day_in_week_tm->tm_mday);
    first_day_of_week = str_time;
    return first_day_of_week;
  }

  int get_current_hour() {
    time_t current_time = time(NULL);
    struct tm* current_tm = localtime(&current_time);
    return current_tm->tm_hour;
  }

  std::string get_current_day_str() {
    time_t current_time = time(NULL);
    struct tm* current_tm = localtime(&current_time);
    char str_today[20];
    sprintf(str_today, "%d-%02d-%02d", current_tm->tm_year + 1900,
            current_tm->tm_mon + 1, current_tm->tm_mday);
    return std::string(str_today);
  }

  bool is_trading_time() {
    time_t current_time = time(NULL);
    struct tm* current_tm = localtime(&current_time);
    //LOG_MSG2("current_tm->tm_wday=%d", current_tm->tm_wday);
    if (0 == current_tm->tm_wday || 6 == current_tm->tm_wday)
      return false;
    char str_hour_time[20];
    sprintf(str_hour_time, "%02d%02d%02d", current_tm->tm_hour,
            current_tm->tm_min, current_tm->tm_sec);
    //LOG_MSG2("str_hour_time=%s", str_hour_time);
    if (strcmp(str_hour_time, "092500") < 0
        || strcmp(str_hour_time, "150000") > 0
        || (strcmp(str_hour_time, "113000") > 0
            && strcmp(str_hour_time, "130000") < 0))
      return false;
    char str_today[20];
    sprintf(str_today, "%d-%02d-%02d", current_tm->tm_year + 1900,
            current_tm->tm_mon + 1, current_tm->tm_mday);
    //LOG_MSG2("str_today=%s", str_today);
    if (holiday_in_2016_.end() != holiday_in_2016_.find(str_today)) {
      //LOG_MSG2("return false%d",1);
      return false;
    }
    //LOG_MSG2("return true%d",1);
    return true;
  }

  bool is_trading_day() {
    time_t current_time = time(NULL);
    return is_trading_day(current_time);
  }

  bool is_trading_day(time_t t) {
    time_t current_time = t;
    struct tm* current_tm = localtime(&current_time);
    if (0 == current_tm->tm_wday || 6 == current_tm->tm_wday)
      return false;
    char str_today[20];
    sprintf(str_today, "%d-%02d-%02d", current_tm->tm_year + 1900,
      current_tm->tm_mon + 1, current_tm->tm_mday);
    if (holiday_in_2016_.end() != holiday_in_2016_.find(str_today)) {
      return false;
    }
    return true;
  }

  bool trade_date(int date) {
    int day = date % 100;
    date /= 100;
    int mon = date % 100;
    int year = date / 100;

    struct tm t;
    memset(&t, 0, sizeof(t));
    t.tm_mday = day;
    t.tm_mon = mon -1;
    t.tm_year = year - 1900;
    time_t s = mktime(&t);

    struct tm* pt = localtime(&s);
    if (0 == pt->tm_wday || 6 == pt->tm_wday)
      return false;
    char str_today[20];
    sprintf(str_today, "%d-%02d-%02d", pt->tm_year + 1900,
            pt->tm_mon + 1, pt->tm_mday);
    if (holiday_in_2016_.end() != holiday_in_2016_.find(str_today)) {
      return false;
    }
    return true;
  }

  static StockUtil* instance_;
  std::set<std::string> holiday_in_2016_;
};

} /* namespace stock_logic */

#endif /* SRC_PLUGINS_STOCK_STOCK_UTIL_H_ */
