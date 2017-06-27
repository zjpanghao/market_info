#include "realinfo_control.h"
#include "realinfo_day.h"
#include "logic/logic_comm.h"
namespace stock_price {
RealInfoControl *RealInfoControl::instance_ = NULL;
RealInfoControl::RealInfoControl() {
  pthread_rwlock_init(&lock_, NULL);
  minute_info_ = new RealInfoStockMinute();
  RealInfoDay::GetInstance();
}

RealInfoControl* RealInfoControl::GetInstance() {
  if (!instance_)
    instance_ = new RealInfoControl();
  return instance_;
}

void RealInfoControl::GetKLineData(const std::string &code,
                                   std::list<RealtimeCodeInfo> *code_info) {
  pthread_rwlock_rdlock(&lock_);
  minute_info_->GetDayData(code, code_info);
  pthread_rwlock_unlock(&lock_);
}

void RealInfoControl::UpdateData(
    const std::string &date,
    const std::map<std::string, DataEngine::CodeInfoArray> &day_data) {
  pthread_rwlock_wrlock(&lock_);
  std::map<std::string, DataEngine::CodeInfoArray>::const_iterator it = day_data
      .begin();
  while (it != day_data.end()) {
    std::string s = it->first;
    DataEngine::CodeInfoArray::const_iterator vit = it->second.begin();
    while (vit != it->second.end()) {
      RealtimeCodeInfo tmp = *vit;
      std::string full = date + it->first;
      minute_info_->AddStockRecord(full, tmp);
      vit++;
    }
    it++;
  }
  pthread_rwlock_unlock(&lock_);
}

bool RealInfoControl::GetLatestInfo(const std::string &code,
                                    RealtimeCodeInfo *info) {

  pthread_rwlock_wrlock(&lock_);
  bool r = minute_info_->GetLatestInfo(code, info);
  pthread_rwlock_unlock(&lock_);
  return r;
}

bool RealInfoControl::GetLatestAllInfo(
    std::map<std::string, RealtimeCodeInfo> *all_stock_mp) {

  pthread_rwlock_wrlock(&lock_);
  bool r = minute_info_->GetLatestAllInfo(all_stock_mp);
  pthread_rwlock_unlock(&lock_);
  return r;
}

}
