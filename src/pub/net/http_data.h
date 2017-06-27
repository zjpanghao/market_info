//  Copyright (c) 2015-2015 The http_data.h Authors. All rights reserved.
//  Created on: 2016年1月12日 Author: paco

#ifndef PUB_NET_HTTP_DATA_H_
#define PUB_NET_HTTP_DATA_H_

#include <list>
#include <string>

#include "net/operator_code.h"
#include "net/error_comm.h"
#include "basic/basictypes.h"
#include "basic/basic_util.h"
#include "basic/scoped_ptr.h"
#include "logic/base_values.h"

typedef base_logic::DictionaryValue NetBase;

class RecvPacketBase {
 public:
  explicit RecvPacketBase(NetBase* m);
  inline int32 GetType() const { return this->type_; }
  inline std::string GetJsonp() const { return this->str_jsonp_; }
  inline int32 GetResult() const {return this->error_code_;}
  inline NetBase* Ptr() {return m_;}

  void Reset();

 public:
  int64 type_;
  bool is_jsonp_;
  std::string str_jsonp_;
  NetBase* m_;
  int32 error_code_;
};

class RecvLoginBase:public RecvPacketBase {
public:
  explicit RecvLoginBase(NetBase* m);

protected:
  void Init();
  int64 user_id(){return user_id_;}
private:
  int64 user_id_;
  std::string token_;
};

class SendPacketBase {
public:
  SendPacketBase() {head_.reset(new NetBase());}
  virtual ~SendPacketBase() {}

public:
 void set_msg(const std::string& msg) {head_->SetString("msg", msg);}

 void set_flag(const int32 flag) {head_->SetInteger("flag", flag);}

 void set_status(const int32 status) {head_->SetInteger("status", status);}

 void set_jsonp_callback(const std::string& callbackstr) {
   head_->SetString("jsonpcallback", callbackstr);
 }

public:
 virtual void GetJsonSerialize(std::string* json, int ntype = 0);
 virtual inline NetBase* release() {return head_.release();}

public:
 scoped_ptr<NetBase> head_;
};



#endif // PUB_NET_HTTP_DATA_H_ 
