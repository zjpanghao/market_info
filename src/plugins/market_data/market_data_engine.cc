//  Copyright (c) 2015-2015 The KID Authors. All rights reserved.
//  Created on: 2016/10/13 Author: zjc

#include "market_data_engine.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include <sstream>
#include <algorithm>

#include "logic/logic_comm.h"
#include "stock_util.h"
#include "stock_price.h"
#include "realinfo_util.h"

#ifndef speedtest__
#define speedtest__(data)   for (long blockTime = NULL; \
  (blockTime == NULL ? (blockTime = clock()) != NULL : false); \
  LOG_MSG2(data "%.6lfs", (double) (clock() - blockTime) / CLOCKS_PER_SEC))
#endif
namespace stock_price {

//static FILE* hFile = NULL;
//static const char filename[] = "err_file.txt";

const char DataEngine::kShIndex[] = "000001";
const char DataEngine::kShSz300[] = "000300";
const char DataEngine::kSzComponentIndex[] = "399001";
const char DataEngine::kGEMIndex[] = "399006";

DataEngine::DataEngine()
    : type_(SH),
      redis_(NULL) {
}

DataEngine::~DataEngine() {
}

void DataEngine::Init(Type type, const std::string& hkey_prefix,
                      RedisController* redis, bool only_fetch_latest) {
  type_ = type;
  hkey_prefix_ = hkey_prefix;
  redis_ = redis;
  only_fetch_latest_ = only_fetch_latest;
}

bool DataEngine::Parse(const std::string& info, StringArray2& res) {
  if (info.empty()) {
    return false;
  }
  std::stringstream ss(info);
  std::string line;
  while (getline(ss, line, ';')) {
    StringArray v;
//    LOG_DEBUG2("line: %s", line.c_str());
    ParseLine(line, v);
    res.push_back(v);
  }
  return true;
}

bool DataEngine::ParseLine(const std::string& line, StringArray& res) {
  std::stringstream ss(line);
  std::string field;
  RealtimeCodeInfo code;
  res.clear();
  while (getline(ss, field, ',')) {
//    LOG_DEBUG2("field: %s", field.c_str());
    res.push_back(field);
  }
  return true;
}

bool DataEngine::FilterNewData(const StringArray& all_times,
                               StringArray& new_times) {
  new_times.clear();
  std::string last_key = "";
  std::set<std::string>::reverse_iterator it = fetched_keys_.rbegin();
  if (it != fetched_keys_.rend()) {
    last_key = *it;
  }

  for (size_t i = all_times.size() - 1; i > 0; i--) {
    if (all_times[i] == last_key) {
      break;
    }
    new_times.push_back(all_times[i]);
    fetched_keys_.insert(all_times[i]);
  }
  reverse(new_times.begin(), new_times.end());
  return !new_times.empty();
}

void DataEngine::ProcessIndex(RealtimeCodeInfo& info) {
  if (SH == type_) {
    if (kShIndex == info.code) {
      info.code = "sh" + info.code;
    } else if (kShSz300 == info.code) {
      info.code = "hs300";
    }
  } else if (SZ == type_) {
    if (kSzComponentIndex == info.code || kGEMIndex == info.code) {
      info.code = "sh" + info.code;
    }
  }
}

bool DataEngine::FetchRawData(const std::string& date,
                              const std::string& market_time,
                              CodeInfoArray& res) {
  std::string raw_data;
  std::string hkey = hkey_prefix_ + date;
  if (!redis_->GetHashElement(hkey, market_time, raw_data)) {
    LOG_ERROR2("fetch data error, hkeys: %s, market_time: %s", hkey.c_str(),
               market_time.c_str());
    return false;
  }

  StringArray2 fields;
  Parse(raw_data, fields);

  res.clear();
  res.reserve(fields.size());

  for (size_t i = 0; i < fields.size(); ++i) {
    RealtimeCodeInfo code;
    if (code.Deserialize(fields[i])) {
      ProcessIndex(code);
      res.push_back(code);
    }
  }
  return !res.empty();
}

void DataEngine::GetDistinctMinMaxKeys(const StringArray &origin,
                                       StringArray *distinct) {
  StringArray::const_reverse_iterator it = origin.rbegin();
  std::string old;
  while (it != origin.rend()) {
    if (it == origin.rbegin()) {
      distinct->push_back(*it);
      old = it->substr(0, 4);
    } else {
      std::string tmp = it->substr(0, 4);
      if (tmp != old) {
        old = tmp;
        distinct->push_back(*it);
      }
    }
    it++;
  }
  reverse(distinct->begin(), distinct->end());

}

bool DataEngine::FetchDayData(const std::string &trade_date,
                              std::map<std::string, CodeInfoArray> *day_map) {
  trade_date_ = trade_date;
  StringArray all_times;
  all_times.reserve(2000);
  std::string hkey = hkey_prefix_ + trade_date_;
  if (!redis_->GetAllHashFields(hkey, all_times)) {
    LOG_MSG2("get market time error: %s", hkey.c_str());
    return false;
  }

  sort(all_times.begin(), all_times.end());
  LOG_MSG2("The key size %d", all_times.size());
  StringArray day_keys;
  day_keys.reserve(2000);
  StringArray time_list;
  time_list.reserve(2000);
  if (!FilterNewData(all_times, day_keys)) {
    return false;
  }
  CodeInfoArray data;
  GetDistinctMinMaxKeys(day_keys, &time_list);
  for (size_t i = 0; i < time_list.size(); ++i) {
    //  read last 10 records
    if (only_fetch_latest_ && (i + 10) < time_list.size()) {
      continue;
    }
    if (!FetchRawData(trade_date_, time_list[i], data) || data.empty()) {
      continue;
    }
    (*day_map)[time_list[i]] = data;
  }
  return true;
}

bool DataEngine::Update(std::string date,
                        std::map<std::string, CodeInfoArray> *day_map) {
  StringArray all_times;
  if (trade_date_ != date) {
    trade_date_ = date;
    fetched_keys_.clear();
    LOG_MSG2("NewUpdate %s", date.c_str());
  }
  std::string hkey = hkey_prefix_ + trade_date_;
  if (!redis_->GetAllHashFields(hkey, all_times)) {
    LOG_MSG2("get market time error: %s", hkey.c_str());
    return false;
  }
  sort(all_times.begin(), all_times.end());
  StringArray new_times;
  StringArray day_keys;
  if (!FilterNewData(all_times, day_keys)) {
    return false;
  }

  CodeInfoArray data;

  GetDistinctMinMaxKeys(day_keys, &new_times);
  for (size_t i = 0; i < new_times.size(); ++i) {
    if (!RealInfoUtil::IsWorkTime(new_times[i])) {
      continue;
    }
    if (!FetchRawData(trade_date_, new_times[i], data) || data.empty()) {
      continue;
    }
    (*day_map)[new_times[i]] = data;
  }
  return true;
}

} /* namespace stock_price */
