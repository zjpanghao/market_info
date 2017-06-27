//  Copyright (c) 2015-2015 The KID Authors. All rights reserved.
//  Created on: 2015年9月17日 Author: kerry

#include "storage/data_engine.h"
#include "storage/redis_controller.h"
//#include "storage/mem_controller.h"
#include "storage/data_mysql_engine.h"
//#include "dic/base_dic_redis_auto.h"
//#include "db/base_db_mysql_auto.h"

namespace base_logic {

DataEngine* DataEngine::Create(int32 type) {
	DataEngine* engine = NULL;
    switch (type) {
        case REIDS_TYPE: {
        engine = new RedisController();
        break;
      }
      case MYSQL_TYPE: {
        engine = new DataMYSQLEngine();
        break;
      }
      /*case MEM_TYPE: {
        engine = new MemController();
        break;
      }*/
      default:
        break;
    }
    return engine;
}

void DataEngine::Init(config::FileConfig* config) {
    config_ = config;
}

void DataEngine::Dest() {
}

}  // namespace base_logic



