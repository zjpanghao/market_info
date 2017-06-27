/*=============================================================================
#    Copyright (c) 2015
#    ShanghaiKunyan.  All rights reserved
#
#    Filename     : /home/tianyiheng_test/logrunner/northsea/restful/pub/fcgimodule/fcgimodule.h
#    Author       : tianyiheng
#    Eail        : tianyiheng@kunyan-inc.com
#    Date         : 2016-03-17 11:18
#    Description  : 
=============================================================================*/

#ifndef __FCGI_MODULE_H__
#define __FCGI_MODULE_H__

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <iostream>
#include <sstream>
// fcgi include
#if defined (FCGI_STD)
#include <fcgi_stdio.h>
#elif defined(FCGI_PLUS)
#include <fcgio.h>
#include <fcgi_config.h>
#endif

// library public
#include "config/config.h"
#include "comm/core_interface.h"
#include "log/mig_log.h"
#include "net/operator_code.h"

// log trace
#include "logtrace/log_trace.h"

// fcgi project module
#define RESTFUL_CORE_SOCK_FILE "/var/www/tmp/twookongcorefile122"
namespace fcgi_module {

enum HttpRequestType {
  HTTP_GET = 1,
  HTTP_POST = 2,
  HTTP_PUT = 3,
  HTTP_DELETE = 4
};

class FcgiModule {
 public:
  FcgiModule();
  ~FcgiModule();
  bool Init(std::string core_sock_file,
            int api_type,
            int opcode,
            int log_type = log_trace::TRACE_API_LOG,
            int send_by_size = 1);

  void Run();
  void Close();

 protected:
  void StdRun();
  void PlusRun();
  // std
  bool GetRequestMethod(const char* query);
  bool PostRequestMethod(const std::string& content);
  bool PutRequestMethod(const std::string& content);
  bool DeleteRequestMethod(const std::string& content);

  // plus
  bool GetRequestMethod(FCGX_Request* request);
  bool PostRequestMethod(FCGX_Request* request);
  bool PutRequestMethod(FCGX_Request* request);
  bool DeleteRequestMethod(FCGX_Request* request);

 protected:
  FCGX_Request request_;
  int api_type_;
  log_trace::LogTrace api_logger_;
  int log_type_;
  int operate_code_;
  int send_by_size_;
};

}; // namespace fcgi_module end  

#endif

