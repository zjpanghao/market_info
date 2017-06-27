//  Copyright (c) 2015-2015 The george Authors. All rights reserved.
//  Created on: 2016年5月18日 Author: kerry


#include "storage/storage.h"
#include "basic/basictypes.h"
#include "thread/base_thread_handler.h"
#include "thread/base_thread_lock.h"
#include "logic/logic_comm.h"

namespace base_logic {

class MYSQL_Pool {
 public:
	MYSQL_Pool();
	virtual ~MYSQL_Pool(){};
 public:
	void Init(std::list<base::ConnAddr>& addrlist, const int32 db_conn_num = 1);
	void Dest();
 public:
	static base_storage::DBStorageEngine* DBConnectionPop(void);
	static void DBConnectionPush(base_storage::DBStorageEngine* engine);
 public:
	static std::list<base_storage::DBStorageEngine*>  db_conn_pool_;
	static threadrw_t*                                db_pool_lock_;
	static std::list<base::ConnAddr>                  addrlist_;
};

class AutoMYSQLEngine {
 public:
	AutoMYSQLEngine();
	virtual ~AutoMYSQLEngine();

	base_storage::DBStorageEngine* GetDBEngine(){
		if(engine_){
			engine_->Release();//释放多余记录集
			if(!engine_->CheckConnect()){//失去连接重新连接
				//重新创建连接
				LOG_DEBUG("lost connection");
				if(!engine_->Connections(MYSQL_Pool::addrlist_))
					return NULL;
			}
			return engine_;
		}
		return engine_;
	}

 private:
	base_storage::DBStorageEngine* engine_;
};

}
