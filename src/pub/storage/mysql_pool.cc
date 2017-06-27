#include "mysql_pool.h"

namespace base_logic {

std::list<base_storage::DBStorageEngine*>  MYSQL_Pool::db_conn_pool_;
threadrw_t*                                MYSQL_Pool::db_pool_lock_;
std::list<base::ConnAddr>                  MYSQL_Pool::addrlist_;

AutoMYSQLEngine::AutoMYSQLEngine() {
  engine_ = MYSQL_Pool::DBConnectionPop();
}

AutoMYSQLEngine::~AutoMYSQLEngine() {
  MYSQL_Pool::DBConnectionPush(engine_);
}

MYSQL_Pool::MYSQL_Pool() {}

void MYSQL_Pool::Init(std::list<base::ConnAddr>& addrlist,
				  const int32 db_conn_num/* = 5*/) {
	addrlist_ = addrlist;

	bool r =false;
	InitThreadrw(&db_pool_lock_);
	for (int i = 0; i<db_conn_num;i++){
		base_storage::DBStorageEngine* engine  =
				base_storage::DBStorageEngine::Create(base_storage::IMPL_MYSQL);
		if (engine==NULL){
			assert(0);
			LOG_ERROR("create db conntion error");
			continue;
		}

		r = engine->Connections(addrlist_);
		if (!r){
			assert(0);
			LOG_ERROR("db conntion error");
			continue;
		}

		db_conn_pool_.push_back(engine);

	}
}


void MYSQL_Pool::DBConnectionPush(base_storage::DBStorageEngine* engine){
	base_logic::WLockGd lk(db_pool_lock_);
	db_conn_pool_.push_back(engine);
}

base_storage::DBStorageEngine* MYSQL_Pool::DBConnectionPop(){
	if(db_conn_pool_.size()<=0)
		return NULL;
	base_logic::WLockGd lk(db_pool_lock_);
    base_storage::DBStorageEngine* engine = db_conn_pool_.front();
    db_conn_pool_.pop_front();
    return engine;
}


void MYSQL_Pool::Dest(){
	base_logic::WLockGd lk(db_pool_lock_);
	while(db_conn_pool_.size()>0){
		base_storage::DBStorageEngine* engine = db_conn_pool_.front();
		db_conn_pool_.pop_front();
		if(engine){
			engine->Release();
			delete engine;
			engine =NULL;
		}
	}
	DeinitThreadrw(db_pool_lock_);
}


}
