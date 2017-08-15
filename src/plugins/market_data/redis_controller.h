//  Copyright (c) 2015-2015 The autocrawler Authors. All rights reserved.
//  Created on: 2015骞�9鏈�17鏃� Author: kerry

#ifndef MARKET_REDIS_CONTROLLER_
#define MARKET_REDIS_CONTROLLER_


#include <list>
#include <string>
#include "logic/base_values.h"
#include "dic/base_dic_redis_auto.h"

namespace stock_price {

class RedisController  {
 public:
    RedisController() {
    }
    virtual ~RedisController() {
    }

 private:
    base_storage::DictionaryStorageEngine* GetRedisEngine() {
      if (redis_engine_) {
        if (!redis_engine_->CheckConnect()) {
          LOG_MSG2("%s", "redis lost connection");
          redis_engine_->Release();
          if(!redis_engine_->Connections(addrlist_)) {
            LOG_MSG2("%s", "redis CONNECT error");
            return NULL;   
          }
        }
      }
      return redis_engine_;
    }

 public:
    void Release();
    virtual void InitParam(std::list<base::ConnAddr>& addrlist);

 public:
    bool AddValueInZSet(const char* hash_name, const char* key,
                        const size_t key_len, const double score);
    int AddRedisZsetMap(const char* hash_name, const size_t hash_len,
                        std::map<std::string, double>& map);
    bool WriteData(const int32 type, base_logic::Value* value);

    bool ReadData(const int32 type, base_logic::Value* value,
                  void (*storage_get)(void*, base_logic::Value*));

    bool GetKey(const std::string& key, std::string& value);

    typedef std::vector<std::string> StringArray;
    bool GetAllHashFields(const std::string& hkey, StringArray& res);

    bool AddHashElement(std::string& hash_name, std::string& key, std::string& value);

    bool GetHashElement(const std::string& hash_name,
                        const std::string& key, std::string& value);

 private:
    bool ReadHashData(base_logic::Value* value,
                      void (*storage_get)(void*, base_logic::Value*));

    bool ReadKeyValueData(base_logic::Value* value,
                          void (*storage_get)(void*, base_logic::Value*));

 private:
  std::list<base::ConnAddr> addrlist_;
  base_storage::DictionaryStorageEngine* redis_engine_;
};

}  // namespace base_logic

#endif /* PLUGINS_CRAWLERSVC_STORAGE_REDIS_CRAWL_STORAGE_H_ */
