//  Copyright (c) 2015-2015 The restful Authors. All rights reserved.
//  Created on: 2015/11/24 Author: tianyiheng

#ifndef __LOG_TRACE_H__
#define __LOG_TRACE_H__

#include <stdio.h>      
#include <stdlib.h>     
#include <memory.h>     
#include <string.h>     
#include<unistd.h>      
                        
#include <sys/types.h>  
#include <sys/socket.h> 
#include <netinet/in.h> 
#include <arpa/inet.h>  
#include <sys/un.h>     

#include <string>
#include <iostream>
using namespace std;

#define UNIX_LOGGER_SOCK_FILE_PATH "/var/www/tmp/logcorefile"

namespace log_trace{

enum LogTraceType{
  TRACE_API_LOG = 1,
  TRACE_WEB_LOG = 2
};

enum LoggerStatus{
  LOGGER_UNINIT = 0,
  LOGGER_NORMAL = 1,
  LOGGER_EXCEPTION = 2
};

class LogTrace{
  public:
    LogTrace();
    void Init(std::string sockfile);
    ~LogTrace();
    bool LogMessage(std::string msg);
    bool LogMsg(const char *data, size_t data_len);
  private:
    int sock_fd_;
    LoggerStatus status_;
};

}


#endif

