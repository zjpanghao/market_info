//  Copyright (c) 2015-2015 The autocrawler Authors. All rights reserved.
//  Created on: 2015年9月17日 Author: kerry

#ifndef KID_REDIS_CONTROLLER_
#define KID_REDIS_CONTROLLER_

#include "storage/data_engine.h"
#include "logic/base_values.h"
#include "storage/data_engine.h"
//#include "net/error_comm.h"
#include "dic/base_dic_redis_auto.h"

#include <list>
#include <string>

namespace base_logic {

class RedisController : public DataEngine {
 public:
  RedisController() {
  }
  virtual ~RedisController() {
  }

 private:
  base_storage::DictionaryStorageEngine* GetRedisEngine() {
    base_dic::AutoDicCommEngine auto_engine;
    base_storage::DictionaryStorageEngine* redis_engine = auto_engine
        .GetDicEngine();
    if (NULL == redis_engine) {
      LOG_ERROR("GetDicEngine redis_engine error");
      return NULL;
    }
    return redis_engine;
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
                void (* storage_get)(void*, base_logic::Value*));

  bool GetKey(const std::string& key, std::string& value);

  typedef std::vector<std::string> StringArray;
  bool GetAllHashFields(const std::string& hkey, StringArray& res);

  bool GetAllHash(const std::string& hname, std::map<std::string, std::string>& hmap);
  bool AddHashElement(std::string& hash_name, std::string& key, std::string& value);

  bool GetHashElement(const std::string& hash_name,
                      const std::string& key, std::string& value);

 private:
  bool ReadHashData(base_logic::Value* value,
                    void (* storage_get)(void*, base_logic::Value*));

  bool ReadKeyValueData(base_logic::Value* value,
                        void (* storage_get)(void*, base_logic::Value*));
};

}  // namespace base_logic

#endif /* PLUGINS_CRAWLERSVC_STORAGE_REDIS_CRAWL_STORAGE_H_ */
