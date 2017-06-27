/*=============================================================================
#    Copyright (c) 2015
#    ShanghaiKunyan.  All rights reserved
#
#    Filename     : /home/tianyiheng_test/logrunner/northsea/restful/pub/fcgimodule/fcgimodule.cc
#    Author       : tianyiheng
#    Email        : tianyiheng@kunyan-inc.com
#    Date         : 2016-03-17 11:56
#    Description  : 
=============================================================================*/

#include "fcgimodule.h"
#include "logic/logic_comm.h"

namespace fcgi_module {

FcgiModule::FcgiModule() {
  api_type_ = 0;
  log_type_ = 0;
  operate_code_ = 0;
}

FcgiModule::~FcgiModule() {

}

bool FcgiModule::Init(std::string core_sock_file,
                      int api_type,
                      int opcode,
                      int log_type,
                      int send_by_size) {
  net::core_connect_ipc(core_sock_file.c_str());
#if defined (FCGI_PLUS)
  FCGX_Init();
  FCGX_InitRequest(&request_, 0, 0);
#endif
  api_type_ = api_type;
  log_type_ = log_type;
  send_by_size_ = send_by_size;
  operate_code_ = opcode;
  api_logger_.Init(UNIX_LOGGER_SOCK_FILE_PATH);

  return true;
}

void FcgiModule::Run() {
#if defined (FCGI_STD)
  StdRun();
#elif defined (FCGI_PLUS)
  PlusRun();
#endif
}

void FcgiModule::StdRun() {
  std::string content;
  const char* query;
#if defined (FCGI_STD)
  while (FCGI_Accept() == 0) {
    char *request_method = getenv("REQUEST_METHOD");
    char *contentLength = getenv("CONTENT_LENGTH");
    if (strcmp(request_method, "POST") == 0) {
      int len = strtol(contentLength, NULL, 10);
      for (int i = 0; i < len; i++) {
        char ch;
        ch = getchar();
        content.append(1, ch);
      }
      PostRequestMethod(content);
      content.clear();
    } else if (strcmp(request_method, "GET") == 0) {
      query = getenv("QUERY_STRING");
      GetRequestMethod(query);
    } else if (strcmp(request_method, "PUT") == 0) {
      std::cin >> content;
      PutRequestMethod(content);
    } else if (strcmp(request_method, "DELETE") == 0) {
      std::cin >> content;
      DeleteRequestMethod(content);
    }
  } // while end
#endif
}

void FcgiModule::PlusRun() {
  std::string content;
  const char* query;
#if defined (FCGI_PLUS)
  while (FCGX_Accept_r(&request_) == 0) {
    fcgi_streambuf cin_fcgi_streambuf(request_.in);
    fcgi_streambuf cout_fcgi_streambuf(request_.out);
    fcgi_streambuf cerr_fcgi_streambuf(request_.err);

#if HAVE_IOSTREAM_WITHASSIGN_STREAMBUF
    std::cin  = &cin_fcgi_streambuf;
    std::cout = &cout_fcgi_streambuf;
    std::cerr = &cerr_fcgi_streambuf;
#else
    std::cin.rdbuf(&cin_fcgi_streambuf);
    std::cout.rdbuf(&cout_fcgi_streambuf);
    std::cerr.rdbuf(&cerr_fcgi_streambuf);
#endif

    char *request_method = FCGX_GetParam(\
        "REQUEST_METHOD", request.envp);
    if (strcmp(request_method, "POST") == 0)
      PostRequestMethod(&request_);
    else if (strcmp(request_method, "GET") == 0)
      GetRequestMethod(&request_);
    else if (strcmp(request_method, "PUT") == 0)
      PutRequestMethod(&request_);
    else if (strcmp(request_method, "DELETE") == 0)
      DeleteRequestMethod(&request_);
  } // while end
#endif
}

void FcgiModule::Close() {
  net::core_close();
}

// std
bool FcgiModule::GetRequestMethod(const char* query) {
  std::string content;
  std::string respone;
  std::stringstream os;
  int flag = 0;
  int code = 0;
  bool ret = false;
  char* addr = getenv("REMOTE_ADDR");
  os << std::string(query)
     << "&remote_addr=" << addr
     << "&operate_code=" << operate_code_
     << "&type=" << api_type_
     << "&log_type=" << log_type_ << "\n";
  content = os.str();
  // log trace 
  api_logger_.LogMsg(content.c_str(), content.length());

  if (send_by_size_ == 0)
    ret = net::core_get(0, content.c_str(), content.length(),
                        respone, flag, code);
  else
    ret = net::core_get_by_size(0, content.c_str(), content.length(), respone, flag, code);
  LOG_DEBUG2("Get responst:%s", respone.c_str());

  if (!respone.empty()) {
    printf("Content-type: text/html\r\n"
               "\r\n"
               "%s", respone.c_str());
  }
  return true;
}

bool FcgiModule::PostRequestMethod(const std::string& content) {
  std::string respone;
  int flag;
  int code;
  std::stringstream os;
  LOG_DEBUG2("fcgi post %s", content.c_str());
  bool r = false;
  // 若CONTENT_TYPE非application/x-www-form-urlencode 直接pass掉
#if defined (CHECK_HEADER)
  if (strcmp(content_type, "application/x-www-form-urlencoded") != 0)
    return false;
#endif
  char* addr = getenv("REMOTE_ADDR");
  os << content
     << "&remote_addr=" << addr
     << "&operate_code=" << operate_code_
     << "&type=" << api_type_
     << "&log_type=" << log_type_ << "\n";
  api_logger_.LogMsg(os.str().c_str(), os.str().length());

  if (send_by_size_ == 0)
    r = net::core_get(0, os.str().c_str(), os.str().length(), respone, flag, code);
  else
    r = net::core_get_by_size(0, os.str().c_str(), os.str().length(), respone, flag, code);

  LOG_DEBUG2("Post responst:%s", respone.c_str());
  if (!respone.empty() && r) {
    printf("Content-type: application/json;charset=utf-8\r\n"
               "\r\n"
               "%s", respone.c_str());
  }
  return true;
}

bool FcgiModule::PutRequestMethod(const std::string& content) {
  return true;
}

bool FcgiModule::DeleteRequestMethod(const std::string& content) {
  return true;
}

// plus
bool FcgiModule::GetRequestMethod(FCGX_Request* request) {
  return true;
}

bool FcgiModule::PostRequestMethod(FCGX_Request* request) {
  char* clenstr = FCGX_GetParam("CONTENT_LENGTH", request->envp);
  if (clenstr) {
    unsigned long clen = strtol(clenstr, &clenstr, 10);
    char* content = new char[clen];
    std::cin.read(content, clen);
    clen = std::cin.gcount();
    if (content) {
      delete[]content;
      content = NULL;
    }
  }
  return true;
}

bool FcgiModule::PutRequestMethod(FCGX_Request* request) {
  return true;
}

bool FcgiModule::DeleteRequestMethod(FCGX_Request* request) {
  return true;
}

} // namespace fcgi_module end
