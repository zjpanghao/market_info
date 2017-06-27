/*=============================================================================
#    Copyright (c) 2015
#    ShanghaiKunyan.  All rights reserved
#
#    Filename     : /home/tyh/project/test_wookong/northsea/restful/pub/sqldbpool/mysql_db_pool.cc
#    Author       : Tianyiheng
#    Email        : tianyiheng@kunyan-inc.com
#    Date         : 2016-06-14 13:49
#    Description  : 
=============================================================================*/
#include <cassert>
#include "mysql_db_pool.h"
#include "storage/storage.h"
#include "basic/basictypes.h"
#include "config/config_info.h"
#include "logic/logic_comm.h"

namespace  plugin_pub_db {

MySqlConnPool::MySqlConnPool() {
  list_mutex_ = PTHREAD_MUTEX_INITIALIZER;
}

MySqlConnPool::~MySqlConnPool() {
    Release();
}

void MySqlConnPool::Lock() {
  pthread_mutex_lock(&list_mutex_);
}

void MySqlConnPool::UnLock() {
  pthread_mutex_unlock(&list_mutex_);
}

bool MySqlConnPool::Init(MySqlConfig const & db_config) {
  Lock();
  std::list<base::ConnAddr> list_addr;
  base::ConnAddr addr_tmp(0, db_config.host(), db_config.port(), \
      db_config.user(), db_config.pswd(), db_config.dbname());
  list_addr.push_back(addr_tmp);
  for (size_t i = 0; i < db_config.pool_size(); ++i) {
    base_storage::DBStorageEngine* engine = \
      base_storage::DBStorageEngine::Create(base_storage::IMPL_MYSQL);
    if (NULL == engine) {
      assert(engine != NULL);
      LOG_ERROR("create db conntion error");
      continue;
    }
    bool r = engine->Connections(list_addr);
    if (!r) {
      LOG_ERROR2("failed to connect db mysql,host:%s, port:%d, user:%s, pswd:%s, db:%s", \
          db_config.host().c_str(), db_config.port(), db_config.user().c_str(), \
          db_config.pswd().c_str(), db_config.dbname().c_str());
      assert(0); 
    }
    engine_list_.push_back(engine); 
  }
  UnLock();
  LOG_DEBUG2("MySqlConnPool.size:%d", engine_list_.size());
  db_config_ = db_config;
  return true;
}

base_storage::DBStorageEngine* MySqlConnPool::PopEngine() {
  base_storage::DBStorageEngine* engine = NULL;
  Lock();
  if (engine_list_.size() > 0) {
    engine = engine_list_.front();
    engine_list_.pop_front();
  }
  UnLock();
  if (NULL != engine) {
    if (!engine->CheckConnect()) {
      // 连接失效，重新连接
      LOG_MSG("mysqlconnect error!");
      engine->Release();

      std::list<base::ConnAddr> list_addr;
      base::ConnAddr addr_tmp(0, db_config_.host(), db_config_.port(), \
          db_config_.user(), db_config_.pswd(), db_config_.dbname());
      list_addr.push_back(addr_tmp);
      if (false == engine->Connections(list_addr)) {
          LOG_ERROR2("failed to connect db mysql,host:%s, port:%d, user:%s, pswd:%s, db:%s", \
              db_config_.host().c_str(), db_config_.port(), db_config_.user().c_str(), \
              db_config_.pswd().c_str(), db_config_.dbname().c_str());
      } 
    } 
  }
  return engine;
}

void MySqlConnPool::PushEngine(base_storage::DBStorageEngine* engine) {
  if (NULL == engine) {
    return;
  }
  engine->FreeRes();
  Lock();
  engine_list_.push_back(engine); 
  UnLock();
}

void MySqlConnPool::Release() {
  Lock();
  std::list<base_storage::DBStorageEngine*>::iterator it = engine_list_.begin();
  for (; it != engine_list_.end(); ++it) {
    base_storage::DBStorageEngine* engine = *it;
    if (NULL != engine) {
      // 断开连接
      engine->Release();
      delete engine, engine = NULL;
      *it = NULL;
    }
  }
  engine_list_.clear();
  UnLock();
}

// -----------------------------------------------[mysql multi pool]--------------------
MySqlMultiPool::MySqlMultiPool() {
    map_mutex_ = PTHREAD_MUTEX_INITIALIZER;
}

MySqlMultiPool::~MySqlMultiPool() {
  Release();
}

void MySqlMultiPool::Lock() {
    pthread_mutex_lock(&map_mutex_);
}

void MySqlMultiPool::UnLock() {
    pthread_mutex_unlock(&map_mutex_);
}

void MySqlMultiPool::InitPool(int pool_id, MySqlConfig const & db_config) {
  Lock();
  std::map<int, MySqlConnPool *>::iterator it = map_mysql_pool_.find(pool_id);
  if (it == map_mysql_pool_.end()) {
    MySqlConnPool *pool_pt = new MySqlConnPool;
    if (NULL != pool_pt) {
      pool_pt->Init(db_config);
      map_mysql_pool_.insert(std::make_pair(pool_id, pool_pt));
    }
  }
  LOG_DEBUG2("MySqlMultiPool.size:%d", map_mysql_pool_.size());
  UnLock();
}

void MySqlMultiPool::Release() {
  Lock();
  std::map<int, MySqlConnPool *>::iterator it = map_mysql_pool_.begin();
  for (; it != map_mysql_pool_.end(); ++it) {
    MySqlConnPool * pool_pt  = it->second;
    if (NULL != pool_pt) {
      pool_pt->Release();
      delete pool_pt, pool_pt = NULL;
    }
  }
  map_mysql_pool_.clear();
  UnLock();
}

base_storage::DBStorageEngine* MySqlMultiPool::GetEngine(int pool_id) {
  base_storage::DBStorageEngine* engine_pt = NULL;
  std::map<int, MySqlConnPool *>::iterator it = map_mysql_pool_.find(pool_id);
  if (it != map_mysql_pool_.end()) {
    MySqlConnPool * pool_pt = it->second;
    if (NULL != pool_pt) {
      engine_pt = pool_pt->PopEngine(); 
    }
  }
  return engine_pt;
}

void MySqlMultiPool::RecycleEngine(int pool_id, base_storage::DBStorageEngine* engine_pt) {
  std::map<int, MySqlConnPool *>::iterator it = map_mysql_pool_.find(pool_id);
  if (it != map_mysql_pool_.end()) {
    MySqlConnPool * pool_pt = it->second;
    if (NULL != pool_pt) {
      engine_pt->FreeRes();
      pool_pt->PushEngine(engine_pt);
    }
  }
}


// -------------------------------------------[mysql pool raii]----------------------------
MultiPoolRaiiMySqlEngine::MultiPoolRaiiMySqlEngine(int pool_id, MySqlMultiPool* multi_pool_pt) {
  engine_pt_ = NULL;
  multi_mysql_pool_pt_ = NULL;
  pool_id_ = 0;
  if (NULL != multi_pool_pt) {
    multi_mysql_pool_pt_ = multi_pool_pt;
    engine_pt_ = multi_mysql_pool_pt_->GetEngine(pool_id);
    pool_id_ = pool_id;
  } 
}

MultiPoolRaiiMySqlEngine::~MultiPoolRaiiMySqlEngine() {
  if (NULL != multi_mysql_pool_pt_) {
    if (NULL != engine_pt_) {
      multi_mysql_pool_pt_->RecycleEngine(pool_id_, engine_pt_); 
    }
  }
}

}  //  namespace plugin_pub_db
