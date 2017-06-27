#ifndef MARKET_DATA_REALINFO_H
#define MARKET_DATA_REALINFO_H
#include "market_data_engine.h"
#include "realinfo_minute.h"
#include "pthread.h"
namespace stock_price {
class RealInfoControl {
 public:
  RealInfoControl(); 
  void GetKLineData(const std::string &code,
                    std::list<RealtimeCodeInfo> *code_info);
  void UpdateData(
      const std::string &date,
      const std::map<std::string, DataEngine::CodeInfoArray> &day_data);

  bool GetLatestInfo(const std::string &code, RealtimeCodeInfo *info);
  bool GetLatestAllInfo(std::map<std::string, RealtimeCodeInfo> *all_stock_mp);
  static RealInfoControl* GetInstance();

 private:
  static RealInfoControl *instance_;
  pthread_rwlock_t lock_;
  RealInfoStockMinute *minute_info_;
};

}
#endif
