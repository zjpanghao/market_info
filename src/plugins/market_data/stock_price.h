#ifndef _STOCK_PRICE_H_
#define _STOCK_PRICE_H_

#include "redis_controller.h"
#include "market_data_engine.h"
#include "config/config.h"

#include <list>

#include <pthread.h>
#define DEFAULT_CONFIG_PATH  "/home/runner/depends/lib/market_info/market.xml"
namespace stock_price {
class StockPrice {
 public:
  StockPrice() {
    pthread_mutex_init(&mutex_lock_, NULL);
    init_ = false;
  }

  bool Init(std::list<base::ConnAddr>& addrlist, bool only_fetch_latest);
  
  bool Init(bool only_fetch_latest); 

  void ScanRedis();
  // fetch one day data, if today is not a trading day, or before the trading time
  void UpdataWhenBoot();

  void GetKLineData(const std::string &code,
                    DataEngine::CodeInfoArray& code_info);

  bool GetLatestInfo(const std::string &code, RealtimeCodeInfo *info);
  // all codes info
  bool GetLatestAllInfo(std::map<std::string, RealtimeCodeInfo> *info);
  static void *ScanRedisThd(void *arg);

 private:
  stock_price::RedisController redis_;
  int market_time_;
  DataEngine data_[DataEngine::MAX_TYPE];
  pthread_mutex_t mutex_lock_;
  bool init_;
};

class MutexLockGuard {
 public:
  explicit MutexLockGuard(pthread_mutex_t &mutex)
      : mutex_(mutex) {
    pthread_mutex_lock(&mutex_);
  }

  ~MutexLockGuard() {
    pthread_mutex_unlock(&mutex_);
  }

 private:
  const MutexLockGuard & operator=(const MutexLockGuard &);

  pthread_mutex_t &mutex_;
};

class SingleStockPrice {

 public:
  static StockPrice * GetInstance() {
    if (instance_ == NULL) {
      MutexLockGuard mutex_lock(mutex_);
      if (instance_ == NULL) {
        instance_ = new StockPrice();
        return instance_;
      }
    }

    return instance_;
  }

 private:
  SingleStockPrice() {
  }
  ;
  const SingleStockPrice& operator=(const SingleStockPrice &);

  class ReleaseInstance {
   public:
    ~ReleaseInstance() {
      if (SingleStockPrice::instance_ == NULL)
        delete instance_;
    }
  };

  static ReleaseInstance release_instance;

  static StockPrice *instance_;
  static pthread_mutex_t mutex_;
};

}  // namespace stock_price

#endif
