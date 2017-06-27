//  Copyright (c) 2015-2015 The KID Authors. All rights reserved.
//  Created on: 2015骞�9鏈�17鏃� Author: kerry


#include "redis_controller.h"
#include "logic/logic_comm.h"
#include <algorithm>

namespace stock_price {

void RedisController::Release() {
  redis_engine_->Release();
  delete redis_engine_;
}

void RedisController::InitParam(std::list<base::ConnAddr>& addrlist){
 LOG_MSG2("%s", "Init params");
  addrlist_ = addrlist;
  base::ConnAddr &addr = addrlist.front();
  LOG_MSG2("%s %d", addr.host().c_str(), addr.port());
  redis_engine_ = base_storage::DictionaryStorageEngine::Create(base_storage::IMPL_RADIES);
  if (redis_engine_ == NULL) {
    LOG_MSG2("message %s", "redis_engine_ Create error");
    return;
  }

    LOG_MSG2("%s", "redis_engine_ create ok");
  bool r = redis_engine_->Connections(addrlist_);
  if (!r) {
    LOG_MSG2("message %s", "redis_engine_ connect error");
    return;
  }
    LOG_MSG2("message %s", "redis_engine_ connect ok");
}

bool RedisController::GetKey(const std::string& key, std::string& value) {
  base_storage::DictionaryStorageEngine* redis_engine = GetRedisEngine();
  if(NULL == redis_engine) {
      return false;
  }
  char* val = NULL;
  size_t val_len = 0;
  bool ret = redis_engine->GetValue(key.c_str(), key.size(), &val, &val_len);
  if (ret) value.assign(val, val_len);
  free(val);
  val = NULL;
  return ret;
}

bool RedisController::GetAllHashFields(const std::string& hkey, StringArray& res) {
  base_storage::DictionaryStorageEngine* redis_engine = GetRedisEngine();
  if(NULL == redis_engine) {
      return false;
  }
  std::list<std::string> field_list;
  if (!redis_engine->GetHashKeys(hkey.c_str(), hkey.size(), field_list)) {
    return false;
  }
  std::copy(field_list.begin(), field_list.end(), std::back_inserter(res));
  return true;
}

bool RedisController::AddHashElement(std::string& hash_name, std::string& key, std::string& value){
    base_storage::DictionaryStorageEngine* redis_engine = GetRedisEngine();
    if( NULL == redis_engine ){
        return false;
    }
    return redis_engine->SetHashElement(hash_name.c_str(), key.c_str(),
                                        key.size(), value.c_str(), value.size());
}

bool RedisController::GetHashElement(const std::string& hash_name,
                                     const std::string& key, std::string& value){
    bool r = false;
    base_storage::DictionaryStorageEngine* redis_engine = GetRedisEngine();
    if( NULL == redis_engine ){
        return false;
    }
    char* str_result = NULL;
    size_t result_len = 0;
    r = redis_engine->GetHashElement(hash_name.c_str(), key.c_str(),
                                     key.size(), &str_result, &result_len);
    if( !r ){
        return false;
    }
    value = std::string(str_result, result_len);
    free(str_result);
    return true;
}

bool RedisController::AddValueInZSet(const char* hash_name, const char* key,
                                     const size_t key_len, const double score){
    base_storage::DictionaryStorageEngine* redis_engine = GetRedisEngine();
    if( NULL == redis_engine ){
        return false;
    }
    return redis_engine->AddRedisZsetElement(hash_name, key, key_len, score);
}

int RedisController::AddRedisZsetMap(const char* hash_name, const size_t hash_len,
                                     std::map<std::string, double>& map){
    base_storage::DictionaryStorageEngine* redis_engine = GetRedisEngine();
    if( NULL == redis_engine ){
        return false;
    }
    return redis_engine->AddRedisZSetMap(hash_name, hash_len, map);
}

bool RedisController::WriteData(const int32 type, base_logic::Value* value) {
    return true;
}

bool RedisController::ReadData(const int32 type, base_logic::Value* value,
        void (*storage_get)(void*, base_logic::Value*)) {
    switch (type) {
      case 0:
        ReadHashData(value, storage_get);
        break;
      case 1:
        ReadKeyValueData(value, storage_get);
        break;
      default:
        break;
    }
    return true;
}


bool RedisController::ReadKeyValueData(base_logic::Value* value,
        void (*storage_get)(void*, base_logic::Value*)) {
    bool r = false;
    std::string key;
    base_logic::DictionaryValue* dict = (base_logic::DictionaryValue*)(value);
    r = dict->GetString(L"rediskey", &key);
    if (!r)
        return r;
    base_dic::AutoDicCommEngine auto_engine;
    base_storage::DictionaryStorageEngine* redis_engine =
            auto_engine.GetDicEngine();
    //  r = redis_engine->GetAllHash()
    return true;
}

bool RedisController::ReadHashData(base_logic::Value* value,
        void (*storage_get)(void*, base_logic::Value*)) {
    bool r = false;
    std::list<std::string> list;
    base_logic::DictionaryValue* dict = (base_logic::DictionaryValue*)(value);
    std::string redis_map;
    r = dict->GetString(L"redismap", &redis_map);
    if (!r)
        return r;
    base_logic::ListValue* list_value = new base_logic::ListValue();
    base_dic::AutoDicCommEngine auto_engine;
    base_storage::DictionaryStorageEngine* redis_engine =
            auto_engine.GetDicEngine();
    r = redis_engine->GetHashValues(redis_map.c_str(),
            redis_map.length(), list);
    if (!r)
        return false;
    while (list.size() > 0) {
        std::string info = list.front();
        list.pop_front();
        list_value->Append(base_logic::Value::CreateStringValue(info));
    }
    dict->GetList(L"resultvalue", &list_value);
    return true;
}

}  // namespace base_logic
