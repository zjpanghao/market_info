//
// Created by Harvey on 2017/1/11.
//

#include "abstract_dao.h"
#include "logic/logic_comm.h"

namespace base_logic {

AbstractDao::AbstractDao()
    : curr_index_(0) {

}

AbstractDao::~AbstractDao() {

}

bool AbstractDao::GetInteger(int32 index, int32& out) {
  curr_index_ = index;
  return GetInteger(out);
}

bool AbstractDao::GetInteger(int32 index, uint32& out) {
  curr_index_ = index;
  int32 tmp;
  if (!GetInteger(tmp)) {
    return false;
  }
  out = (uint32)tmp;
  return true;
}

bool AbstractDao::GetInteger(int32& out) {
  std::string str_temp = PopColumnValue();
  if (!str_temp.empty()) {
    out = atoi(str_temp.c_str());
    return true;
  }
  return false;
}

bool AbstractDao::GetReal(int32 index, double& value) {
  curr_index_ = index;
  return GetReal(value);
}

bool AbstractDao::GetReal(double& out) {
  std::string str_temp = PopColumnValue();
  if (!str_temp.empty()) {
    out = atof(str_temp.c_str());
    return true;
  }
  return false;
}

bool AbstractDao::GetString(int32 index, std::string& out) {
  curr_index_ = index;
  return GetString(out);
}

bool AbstractDao::GetString(std::string& out) {
  out = PopColumnValue();
  if (out.empty()) {
    return false;
  }
  return true;
}

bool AbstractDao::operator()(MYSQL_ROW& row) {
  row_ = row;
  Deserialize();
  return true;
}

std::string AbstractDao::PopColumnValue() {
  if(NULL == row_) {
    LOG_ERROR("PopColumnValue row NULL");
    return "";
  }

  std::string temp("");
  int old_index = curr_index_;
  ++curr_index_;

  if(row_[old_index]) {
    temp = row_[old_index];
  }
//  LOG_DEBUG2("PopColumnValue, index=%d, value=%s",
//             old_index, temp.c_str());
  return temp;
}



} /* namespace base_logic */
