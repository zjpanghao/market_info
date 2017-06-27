//  Copyright (c) 2015-2015 The restful Authors. All rights reserved.
//  Created on: 2015/11/24 Author: tianyiheng

#include "logic/logic_comm.h"
#include "log_trace.h"

namespace log_trace{

LogTrace::LogTrace()
:status_(LOGGER_UNINIT){
}

LogTrace::~LogTrace(){
  close(sock_fd_);
}

void LogTrace::Init(std::string sockfile){
  //unix 文件套接字
  sock_fd_  = ::socket(AF_UNIX, SOCK_STREAM, 0);                                 
  struct sockaddr_un addr;                            
  addr.sun_family = AF_UNIX;                          
  strcpy((char*)&addr.sun_path, sockfile.c_str());
  // connect                                                                    
  int connect_ret = ::connect(sock_fd_, (struct sockaddr*)&addr, sizeof(addr));
  if (0 > connect_ret){                                                       
    LOG_DEBUG("[connect error!]\n");
    status_ = LOGGER_EXCEPTION;     
  }
  else{
    LOG_DEBUG("LogTrace constructed sucess!!");
    status_ = LOGGER_NORMAL;
  }
}

bool LogTrace::LogMessage(std::string msg){
  if(LOGGER_NORMAL == status_){ 
    int ret = 0;
    int sended_len = 0;
    while(ret = ::send(sock_fd_, msg.c_str() + sended_len, msg.length() - sended_len, 0), ret > 0){
      sended_len += ret;
      if (sended_len >= msg.length()){
        break;
      }
      if (0 > ret){
        status_ = LOGGER_EXCEPTION;  
      }
    }
  }else{
    LOG_DEBUG2("[LogTrace]failed to log message, logger's status is %d, try connect again", (int)status_);
    Init(UNIX_LOGGER_SOCK_FILE_PATH);
    return false;
  }
  return true;
}
 
bool LogTrace::LogMsg(const char *data, size_t data_len){
  if(LOGGER_NORMAL == status_){
    int ret = 0;
    int sended_len = 0;
    while(ret = ::send(sock_fd_, data + sended_len, data_len - sended_len, 0), ret > 0){
      sended_len += ret;
      if (sended_len >= data_len){
        break;
      }
    }
    if (0 > ret){
      status_ = LOGGER_EXCEPTION;
    }
  }else{
    LOG_DEBUG2("[LogTrace]failed to log message, logger's status is %d, try connect again", (int)status_);
    Init(UNIX_LOGGER_SOCK_FILE_PATH);
    return false;
  }
  return true;
}

} //namespace log_trace end
