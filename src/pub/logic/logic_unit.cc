/*
 * logic_unit.cc
 *
 *  Created on: 2014年12月2日
 *      Author: kerry
 */

#include "logic/logic_unit.h"
#include "basic/radom_in.h"

#include <sstream>

namespace base_logic{

void* SomeUtils::GetLibraryFunction(const std::string& library_name,
                                    const std::string& func_name) {
  void* engine = NULL;
  basic::libhandle handle_lancher = NULL;
  handle_lancher = basic::load_native_library(library_name.c_str());

  if (handle_lancher == NULL) {
    LOG_ERROR2("Cant't load path %s", library_name.c_str());
    return NULL;
  }

  engine = basic::get_function_pointer(handle_lancher, func_name.c_str());

  if (engine == NULL) {
    LOG_ERROR2("Can't find %s", func_name.c_str());
    return NULL;
  }
  return engine;
}

void LogicUnit::SendMessageBySize(const int socket, std::string& json){

  uint32_t len = json.length();
  LOG_DEBUG2("\n%s", json.c_str());
  LOG_DEBUG2("length:%d", len);
  unsigned char *p_to_len = (unsigned char *)(&len);
  std::stringstream os;
  os << p_to_len[0] << p_to_len[1] << p_to_len[2] << p_to_len[3] << json;
  LOG_DEBUG2("length:%d %d %d %d", p_to_len[0], p_to_len[1], p_to_len[2], p_to_len[3]);
  json = os.str();
  //char *buf = new char [4 + len];
  //*(int*)buf = len;
  //memcpy(buf + 4, json.c_str(), json.length());
  int send_length = base_logic::LogicComm::SendFull(socket, json.c_str(), json.length());
  LOG_MSG2("Real send data len %d", send_length);
  // delete [] buf;
}

}
