//
// Created by Harvey on 2016/12/29.
//

#include "mysql_engine.h"

namespace base_logic {

MysqlEngine::MysqlEngine(
    base::ConnAddr& read_addr,
    base::ConnAddr& write_addr,
    bool isAsync)
    : read_addr_(read_addr),
      write_addr_(write_addr),
      async_(isAsync) {
  Initialize();
}

MysqlEngine::~MysqlEngine() {
  pthread_join(mysql_thread_, NULL);
}

void MysqlEngine::Initialize() {
  CreateMysqlReadEnginePool(MYSQL_READ_ENGINE_NUM);
  CreateMysqlWriteEnginePool(MYSQL_WRITE_ENGINE_NUM);

  if (async_) {
    int ret = pthread_create(&mysql_thread_,
                             NULL,
                             MysqlThread::Run,
                             &shared_info_);
    if (0 != ret) {
      LOG_ERROR("mysql async thread create error");
      assert(0);
    }
  }
}

bool MysqlEngine::CreateMysqlReadEnginePool(int32 pool_num) {
  shared_info_.read_addr_list_.push_back(read_addr_);
  for (int i = 0; i < pool_num; ++i) {
    bool r = false;
    base_storage::DBStorageEngine* read_engine =
        base_storage::DBStorageEngine::Create(base_storage::IMPL_MYSQL);
    if (read_engine == NULL) {
      LOG_ERROR("create db conntion error");
      assert(0);
    }
    // Create read engine
    r = read_engine->Connections(shared_info_.read_addr_list_);
    if (!r) {
      LOG_ERROR("db conntion error");
      assert(0);
    }
    shared_info_.PushEngine(MYSQL_READ, read_engine);
  }
  return true;
}

bool MysqlEngine::CreateMysqlWriteEnginePool(int32 pool_num) {
  shared_info_.write_addr_list_.push_back(write_addr_);
  // Create write engine
  for (int i = 0; i < pool_num; ++i) {
    bool r = false;
    base_storage::DBStorageEngine* write_engine =
        base_storage::DBStorageEngine::Create(base_storage::IMPL_MYSQL);
    if (write_engine == NULL) {
      LOG_ERROR("create db conntion error");
      assert(0);
    }
    r = write_engine->Connections(shared_info_.write_addr_list_);
    if (!r) {
      LOG_ERROR("db conntion error");
      assert(0);
    }
    shared_info_.PushEngine(MYSQL_WRITE, write_engine);
  }
  return true;
}

bool MysqlEngine::ReadDataRows(int column_num,
                               const std::string& sql,
                               MYSQL_ROWS_VEC& rows_vec) {
  MySqlJobAdapter job(MYSQL_READ, sql, column_num);
  return job.ReadDataRows(&shared_info_, column_num, rows_vec);
}

bool MysqlEngine::ExcuteStorage(int column_num,
                                const std::string& sql,
                                MYSQL_ROWS_VEC& rows_vec) {
  MySqlJobAdapter job(MYSQL_STORAGE, sql, column_num);
  return job.ReadDataRows(&shared_info_, column_num, rows_vec);
}

bool MysqlEngine::WriteData(const std::string& sql) {
  MySqlJobAdapter job(base_logic::MYSQL_WRITE, sql);
  return job.WriteData(&shared_info_);
}

bool MysqlEngine::AddAsyncMysqlJob(int column_num,
                                   const std::string& sql,
                                   MysqlCallback callback,
                                   MYSQL_JOB_TYPE type,
                                   void* param) {
  if (async_) {
    MySqlJobAdapter* job = new MySqlJobAdapter(
        type, sql, column_num, callback, param);
    shared_info_.PushMysqlJob(job);
    return true;
  }
  return false;
}

} /* namespace base_logic */