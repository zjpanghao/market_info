//  Copyright (c) 2015-2015 The restful Authors. All rights reserved.
//  Created on: 2015/11/24 Author: jiaoyongqing
/*
 *  user_comm_head.h
 *
 *  Created on: 2015/10/22
 *  Author: jiaoyongqing
 */

#ifndef _HOME_RUNNER_NORTHSEA_RESTFUL_PUB_NET_USER_COMM_HEAD_H_
#define _HOME_RUNNER_NORTHSEA_RESTFUL_PUB_NET_USER_COMM_HEAD_H_

#include <string>

#include "net/http_data.h"
#include "basic/basictypes.h"
#include "logic/base_values.h"
#include "net/error_comm.h"
#include "tools/tools.h"
#include "logic/logic_comm.h"
#include "basic/basic_util.h"
#include "db/db_comm.h"
#include "net/typedef.h"

namespace netcomm_recv {
class RecvLogin : public RecvPacketBase {
 public:
	explicit RecvLogin(NetBase* m) :
		RecvPacketBase(m), user_level_(LEVEL_ONE) {
		bool r = false;
		r = m_->GetBigInteger(L"user_type", static_cast<int64*>(&user_type_));
		if (false == r)
			error_code_ = USER_TYPE_LACK;
		if (user_type_ == 0) {
			user_name_ = "kunyan";
			password_ = "123456";
			platform_id_ = 1;
		} else {
			init();
		}
	}

  inline void Reset() {
    init();
  }

  inline void init() {
    bool r = false;

    std::string temp="";

    r = m_->GetBigInteger(L"platform_id", static_cast<int64*>(&platform_id_));
    if (false == r) error_code_ = PLATFORM_ID_LACK;

    r = m_->GetString(L"user_name", &user_name_);
    if (false == r) error_code_ = USER_NAME_LACK;

    r = m_->GetString(L"password", &temp);
    if (false == r) {
      error_code_ = USER_PASSWORD_LACK;
    }

    //r = m_->GetString(L"user_type", &user_type_);
    //if (false == r) error_code_ = USER_NAME_LACK;

    LOG_DEBUG2("-----temp-------%s", temp);
    password_ =temp.substr(0,temp.size()-1);
    LOG_DEBUG2("-----password_-------%s", password_);

  }

  void set_user_id(const int64 user_id) { user_id_ = user_id; }

  void set_platform_id(const int64 platform_id) {platform_id_ = platform_id;}

  void set_user_name(const std::string& user_name) {user_name_ = user_name;}

  void set_password(const std::string& password) {password_ = password;}

  void set_user_level(const int64 user_level) { user_level_ = user_level; }

  int64 platform_id() const { return platform_id_; }
  const std::string& user_name() const { return user_name_; }
  const std::string& password() const { return password_; }
  int user_level() const { return user_level_; }
  int64 user_id() const { return user_id_; }
  int64 user_type() const{ return user_type_;}

  enum UserLevel{ LEVEL_ONE, LEVEL_TWO, LEVEL_THREE };

 private:
  int64 user_id_;
  int64 platform_id_;
  std::string user_name_;
  std::string password_;
  int64 user_level_;
  int64 user_type_;
};

class RecvRegister : public RecvPacketBase {
 public:
  explicit RecvRegister(NetBase* m)
  :RecvPacketBase(m), \
  user_level_(LEVEL_ONE) {
    init();
  }

  inline void Reset() {
    init();
  }

  inline void init() {
    bool r = false;

    std::string temp="";

    r = m_->GetBigInteger(L"platform_id", static_cast<int64*>(&platform_id_));
    if (false == r) error_code_ = PLATFORM_ID_LACK;

    r = m_->GetString(L"user_name", &user_name_);
    if (false == r) error_code_ = USER_NAME_LACK;

    r = m_->GetString(L"password", &temp);
    if (false == r) {
      error_code_ = USER_PASSWORD_LACK;
    }

    password_ =temp.substr(0,temp.size()-1);
  }

  int64 platform_id() const { return platform_id_; }
  const std::string& user_name() const { return user_name_; }
  const std::string& password() const { return password_; }
  int user_level() const { return user_level_; }

  enum UserLevel{ LEVEL_ONE, LEVEL_TWO, LEVEL_THREE };

 private:
  int64 platform_id_;
  std::string user_name_;
  std::string password_;
  int64 user_level_;
};

class RecvVerifyEmail:public RecvPacketBase {
public:
 explicit RecvVerifyEmail(NetBase* m)
 :RecvPacketBase(m){
   init();
 }

 inline void Reset() {
   init();
 }

 inline void init() {
   bool r = false;

   std::string temp="";

   r = m_->GetString(L"email", &email_);
   if (false == r)
     error_code_ = STRUCT_ERROR;
   r = m_->GetString(L"param", &param_);
   if (false == r)
     error_code_ = STRUCT_ERROR;
 }

 const std::string& get_email() const { return email_; }
 const std::string& get_param() const { return param_; }

private:
 std::string email_;
 std::string param_;
};

}  //  namespace netcomm_recv

namespace netcomm_send {
class SendLogin:public SendPacketBase {
 public:
  SendLogin() {
    login_.reset(new NetBase());
    userinfo_login_.reset(new NetBase());
  }

  virtual ~SendLogin() {}

  NetBase* release() {
    if (userinfo_login_->size() > 0) {
      login_->Set("user_info", userinfo_login_.release());
    }
    head_->Set("result", login_.release());
    this->set_flag(0);
    this->set_msg("");
    this->set_status(1);
    return head_.release();
  }

 public:
  inline void set_userinfo_uid(const int32 uid) {
    userinfo_login_->SetBigInteger("user_id", uid);
  }

  inline void set_userinfo_name(const std::string &name) {
    userinfo_login_->SetString("user_name", name);
  }

  inline void set_userinfo_platform(const std::string &platform) {
    userinfo_login_->SetString("platform_name", platform);
  }

  inline void set_userinfo_token(const std::string &token) {
    userinfo_login_->SetString("token", token);
  }

  inline void set_userinfo_level(int64 user_level) {
    userinfo_login_->SetBigInteger("user_level", user_level);
  }

 private:
  scoped_ptr<NetBase> login_;
  scoped_ptr<NetBase> userinfo_login_;
};


class SendRegister:public SendPacketBase {
 public:
	SendRegister() {
    login_.reset(new NetBase());
    userinfo_login_.reset(new NetBase());
  }

  virtual ~SendRegister() {}

  NetBase* release() {
    if (userinfo_login_->size() > 0) {
      login_->Set("user_info", userinfo_login_.release());
    }
    head_->Set("result", login_.release());
    this->set_status(1);
    return head_.release();
  }

 public:
  inline void set_userinfo_name(const std::string &name) {
    userinfo_login_->SetString("user_name", name);
  }

  inline void set_userinfo_password(const std::string &password) {
    userinfo_login_->SetString("password", password);
  }

 private:
  scoped_ptr<NetBase> login_;
  scoped_ptr<NetBase> userinfo_login_;
};








}  //  namespace netcomm_send

#endif  //  _HOME_RUNNER_NORTHSEA_RESTFUL_PUB_NET_USER_COMM_HEAD_H_
