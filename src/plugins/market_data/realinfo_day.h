#ifndef MARKET_REALINFO_DAY_H
#define MARKET_REALINFO_DAY_H
#include "pthread.h"
#include <set>
#include <string>
#include <iostream>
#include <fstream>

namespace stock_price {
class RealInfoDay {
 public:
  RealInfoDay();

  bool LoadFromFile();

  bool UpdateCloseDay() {
    return LoadFromFile();
  }

  bool CheckCloseDay(std::string date) {
    return close_day_.find(date) != close_day_.end();
  }

  static RealInfoDay* GetInstance();

 private:
  bool ParseDate(std::string content);
  void SplitString(const std::string& src, 
                   std::set<std::string> *dest, 
                   const std::string& flag);
  static RealInfoDay* instance_;
  std::set<std::string> close_day_;
  pthread_mutex_t mutex_;
};
}
#endif
