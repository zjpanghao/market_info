//
// Created by Harvey on 2016/12/29.
//

#ifndef STRADE_SRC_PUB_STORAGE_MYSQL_THREAD_POOL_H
#define STRADE_SRC_PUB_STORAGE_MYSQL_THREAD_POOL_H

#include "config/config.h"
#include "storage/storage.h"
#include "thread/base_thread_lock.h"
#include "logic/logic_comm.h"
#include "logic/base_values.h"
#include "net/comm_head.h"
#include "dao/abstract_dao.h"
#include "basic/scoped_ptr.h"
#include "logic/comm_head.h"

#include <list>
#include <queue>
#include <semaphore.h>
#include <mysql/mysql.h>
#include <sstream>

// /*所有行*/vector<vector<所有列> >
typedef std::vector<std::vector<std::string> > MYSQL_ROWS_VEC;

// 异步回调函数指针
typedef void (* MysqlCallback)(int column_num, MYSQL_ROWS_VEC& rows_vec, void* param);

namespace base_logic {

enum MYSQL_JOB_TYPE {
  MYSQL_READ = 1,
  MYSQL_WRITE = 2,
  MYSQL_STORAGE = 3,
};

// 读数据链接数 >= 2
const int MYSQL_READ_ENGINE_NUM = 3;

// 写连接数 >= 2
const int MYSQL_WRITE_ENGINE_NUM = 2;

namespace {

struct MySqlJobAdapter;

typedef std::list<base_storage::DBStorageEngine*> MYSQL_ENGINE_POOL;
typedef std::queue<MySqlJobAdapter*> MYSQL_TASK_QUEUE;

struct MysqlEngineSharedInfo {
  MysqlEngineSharedInfo() {
    sem_init(&task_num_, 0, 0);                                 // 当前任务数，初始为0
    sem_init(&read_engine_num_, 0, MYSQL_READ_ENGINE_NUM);            // 读连接数
    sem_init(&write_engien_num_, 0, MYSQL_WRITE_ENGINE_NUM);          // 写连接数
    pthread_mutex_init(&mutex_, NULL);
  }

  ~MysqlEngineSharedInfo() {
    pthread_mutex_destroy(&mutex_);
    sem_destroy(&read_engine_num_);
    sem_destroy(&write_engien_num_);
    while (!db_read_pool_.empty()) {
      base_storage::DBStorageEngine* engine = db_read_pool_.front();
      db_read_pool_.pop_front();
      if (engine) {
        engine->Release();
        delete engine;
        engine = NULL;
      }
    }
    while (!db_write_pool_.empty()) {
      base_storage::DBStorageEngine* engine = db_write_pool_.front();
      db_write_pool_.pop_front();
      if (engine) {
        engine->Release();
        delete engine;
        engine = NULL;
      }
    }
  }

  bool PopMysqlJob(MySqlJobAdapter*& job) {
    sem_wait(&task_num_);
    pthread_mutex_lock(&mutex_);
    bool r = false;
    do {
      if (task_queue_.empty()) {
        r = false;
        break;
      }
      job = task_queue_.front();
      task_queue_.pop();
      if(NULL == job) {
        continue;
      }
      r = true;
    } while (0);
    pthread_mutex_unlock(&mutex_);
    return r;
  }

  bool PushMysqlJob(MySqlJobAdapter* job) {
    pthread_mutex_lock(&mutex_);
    task_queue_.push(job);
    sem_post(&task_num_);
    pthread_mutex_unlock(&mutex_);
    return true;
  }

  base_storage::DBStorageEngine* PopEngine(MYSQL_JOB_TYPE engine_type) {
    base_storage::DBStorageEngine* engine = NULL;
    switch (engine_type) {
      case MYSQL_WRITE:
      case MYSQL_STORAGE: {
        sem_wait(&write_engien_num_);
        pthread_mutex_lock(&mutex_);
        engine = db_write_pool_.front();
        db_write_pool_.pop_front();
        CheckConnect(engine, MYSQL_WRITE);
        break;
      }
      case MYSQL_READ: {
        sem_wait(&read_engine_num_);
        pthread_mutex_lock(&mutex_);
        engine = db_read_pool_.front();
        db_read_pool_.pop_front();
        CheckConnect(engine, MYSQL_READ);
        break;
      }
      default: {
        LOG_ERROR("mysql job type illegal");
        return NULL;
      }
    }
    pthread_mutex_unlock(&mutex_);
    return engine;
  }

  void PushEngine(MYSQL_JOB_TYPE engine_type,
                  base_storage::DBStorageEngine* engine) {
    assert(NULL != engine);
    pthread_mutex_lock(&mutex_);
    // 释放结果集
    engine->FreeRes();
    switch (engine_type) {
      case MYSQL_WRITE:
      case MYSQL_STORAGE: {
        db_write_pool_.push_back(engine);
        sem_post(&write_engien_num_);
        break;
      }
      case MYSQL_READ: {
        db_read_pool_.push_back(engine);
        sem_post(&read_engine_num_);
        break;
      }
      default: {
        LOG_ERROR("mysql job type illegal");
        break;
      }
    }
    pthread_mutex_unlock(&mutex_);
  }

  bool CheckConnect(base_storage::DBStorageEngine* engine,
                    MYSQL_JOB_TYPE type) {
    bool r = false;
    r = engine->CheckConnect();
    if (r) {
      return true;
    }
    // reconnect
    int i = 0;
    do {
      if (type == MYSQL_READ) {
        r = engine->Connections(read_addr_list_);
      } else {
        r = engine->Connections(write_addr_list_);
      }
      if (r) {
        LOG_MSG("mysql lost connection, reconnect success");
        break;
      }
      ++i;
      LOG_ERROR2("mysql lost connection try:%d connected failed, 5s retry", i);
      sleep(5);
    } while (true);

    return r;
  }

  sem_t task_num_;
  sem_t read_engine_num_;
  sem_t write_engien_num_;

  pthread_mutex_t mutex_;
  MYSQL_ENGINE_POOL db_read_pool_;
  MYSQL_ENGINE_POOL db_write_pool_;

  MYSQL_TASK_QUEUE task_queue_;
  std::list<base::ConnAddr> read_addr_list_;
  std::list<base::ConnAddr> write_addr_list_;
};

struct MySqlJobAdapter {
  MySqlJobAdapter(MYSQL_JOB_TYPE type,
                  const std::string& sql,
                  int column_num = 0,
                  MysqlCallback callback = NULL,
                  void* param = NULL)
      : type_(type),
        sql_(sql),
        callback_(callback),
        column_num_(column_num),
        param_(param) {
  }

  template<class T>
  bool ReadData(MysqlEngineSharedInfo* shared_info,
                std::vector<T>& result) {
    bool r = false;
    base_storage::DBStorageEngine* engine =
        shared_info->PopEngine(type_);
    if (NULL == engine) {
      LOG_DEBUG2("excute sql=%s error, engine NULL",
                 sql_.c_str());
      return false;
    }
    do {
      r = engine->SQLExec(sql_.c_str());
      if (!r) {
        r = false;
        LOG_ERROR("exec sql error");
        break;
      }
      MYSQL_ROW rows;
      int32 num = engine->RecordCount();
      if (engine->RecordCount() > 0) {
        while ((rows = (*(MYSQL_ROW*) (engine->FetchRows())->proc))) {
          T obj;
          obj(rows);
          result.push_back(obj);
        }
      }
      r = true;
    } while (0);
    // 归还链接
    shared_info->PushEngine(type_, engine);
    return r;
  }

  bool ReadDataRows(MysqlEngineSharedInfo* shared_info,
                    int coulmn_num,
                    MYSQL_ROWS_VEC& rows_vec) {
    bool r = false;
    base_storage::DBStorageEngine* engine =
        shared_info->PopEngine(type_);
    if (NULL == engine) {
      LOG_DEBUG2("excute sql=%s error, engine NULL",
                 sql_.c_str());
      return false;
    }
    do {
      r = engine->SQLExec(sql_.c_str());
      if (!r) {
        r = false;
        LOG_ERROR("exec sql error");
        break;
      }
      MYSQL_ROW rows;
      int32 num = engine->RecordCount();
      if (engine->RecordCount() > 0) {
        while ((rows = (*(MYSQL_ROW*) (engine->FetchRows())->proc))) {
          std::vector<std::string> column_vec;
          for (int i = 0; i < coulmn_num; ++i) {
            std::string temp("");
            if (rows[i]) {
              temp = rows[i];
            }
            column_vec.push_back(temp);
          }
          rows_vec.push_back(column_vec);
        }
      }
      r = true;
    } while (0);
    // 归还链接
    shared_info->PushEngine(type_, engine);
    return r;
  }

  bool WriteData(MysqlEngineSharedInfo* shared_info) {
    bool r = false;
    base_storage::DBStorageEngine* engine =
        shared_info->PopEngine(type_);
    if (NULL == engine) {
      LOG_DEBUG2("excute sql=%s error, engine NULL",
                 sql_.c_str());
      return false;
    }
    do {
      r = engine->SQLExec(sql_.c_str());
      if (!r) {
        r = false;
        LOG_ERROR("exec sql error");
        break;
      }
      r = true;
    } while (0);
    // 归还链接
    shared_info->PushEngine(type_, engine);
    return r;
  }

  std::string sql_;
  MYSQL_JOB_TYPE type_;
  MysqlCallback callback_;
  int column_num_;
  void* param_;
};

struct MysqlThread {
  static void* Run(void* param) {
    LOG_DEBUG("mysql async thread run");
    MysqlEngineSharedInfo* shared_info =
        static_cast<MysqlEngineSharedInfo*>(param);
    bool r = false;
    while (true) {
      MySqlJobAdapter* job = NULL;
      r = shared_info->PopMysqlJob(job);
      if (NULL == job) {
        LOG_DEBUG("mysql async PopMysqlJob error");
        continue;
      }
      scoped_ptr<MySqlJobAdapter> mysql_job(job);
      MYSQL_ROWS_VEC rows_vec;
      r = mysql_job->ReadDataRows(
          shared_info, mysql_job->column_num_, rows_vec);
      if (r && mysql_job->callback_) {
        mysql_job->callback_(
            mysql_job->column_num_,
            rows_vec,
            mysql_job->param_);
      }
    }
    return NULL;
  }
};

} /* namespace */

class MysqlEngine {
 public:
  MysqlEngine(base::ConnAddr& read_addr,
              base::ConnAddr& write_addr,
              bool isAsync = false);
  ~MysqlEngine();

 private:
  void Initialize();
  bool CreateMysqlReadEnginePool(
      int32 pool_num = MYSQL_READ_ENGINE_NUM);
  bool CreateMysqlWriteEnginePool(
      int32 pool_num = MYSQL_WRITE_ENGINE_NUM);

 public:
  // 获取对象
  template<class T>
  bool ReadData(const std::string& sql,
                std::vector<T>& result) {
    MySqlJobAdapter mysql_job(base_logic::MYSQL_READ, sql);
    return mysql_job.ReadData<T>(&shared_info_, result);
  }

  bool ReadDataRows(int column_num,
                    const std::string& sql,
                    MYSQL_ROWS_VEC& rows_vec);

  bool ExcuteStorage(int column_num,
                     const std::string& sql,
                     MYSQL_ROWS_VEC& rows_vec);

  // 更新数据
  bool WriteData(const std::string& sql);

  // 异步添加查询任务
  bool AddAsyncMysqlJob(int column_num,
                        const std::string& sql,
                        MysqlCallback callback,
                        MYSQL_JOB_TYPE type,
                        void* param = NULL);

 private:
  bool async_;
  base::ConnAddr read_addr_;
  base::ConnAddr write_addr_;
  MysqlEngineSharedInfo shared_info_;
  pthread_t mysql_thread_;
};

} /* namespace base_logic */

#endif //STRADE_SRC_PUB_STORAGE_MYSQL_THREAD_POOL_H
