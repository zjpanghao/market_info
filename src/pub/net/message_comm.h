//  Copyright (c) 2015-2015 The restful Authors. All rights reserved.
//  Created on: 2015/11/24 Author: jiaoyongqing

#ifndef _HOME_RUNNER_JINDOWIN_PUB_NET_MESSAGE_COMM_HEAD_H_
#define _HOME_RUNNER_JINDOWIN_PUB_NET_MESSAGE_COMM_HEAD_H_

#include <string>

#include "net/http_data.h"
#include "basic/basictypes.h"
#include "logic/base_values.h"
#include "net/error_comm.h"
#include "net/typedef.h"

namespace netcomm_recv {

class RecvMessageHyGn:public RecvPacketBase{
public:
 explicit RecvMessageHyGn(NetBase* m)
 :RecvPacketBase(m) {
   init();
 }

 inline void init() {
   bool r = false;
   r = m_->GetBigInteger(L"user_id", static_cast<int64*>(&user_id_));
   if (false == r) {
     error_code_ = USER_ID_LACK;
   }

   r = m_->GetString(L"token", &token_);
   if (false == r) {
     error_code_ = TOKEN_LACK;
   }

   operate_ = -1;

   r = m_->GetString(L"hy", &hy_);
   if (true == r) {
     std::string out_hy;
     base::BasicUtil::UrlDecode(hy_, out_hy);
     hy_ = out_hy.substr(0, out_hy.length() - 1);

     operate_ = 0;
   }

   r = m_->GetString(L"gn", &gn_);
   if (true == r) {
     std::string out_gn;
     base::BasicUtil::UrlDecode(gn_, out_gn);
     gn_ = out_gn.substr(0, out_gn.length() - 1);

     operate_ = 1;
   }

   if (operate_ == -1) {
     error_code_ = GN_HY_LACK;
   }

 }

 int64 user_id() const { return user_id_; }

 std::string token() const { return token_; }

 std::string hy_or_gn() const {
   if (operate_ == 0)
     return hy_;

   return gn_;
 }

 int64 operate() const { return operate_; }

private:
 int64 user_id_;

 std::string token_;

 std::string hy_;

 std::string gn_;

 int64 operate_;
};

class RecvMessageCode:public RecvPacketBase{
public:
 explicit RecvMessageCode(NetBase* m)
 :RecvPacketBase(m) {
   init();
 }

 inline void init() {
   bool r = false;
   r = m_->GetBigInteger(L"user_id", static_cast<int64*>(&user_id_));
   if (false == r) {
     error_code_ = USER_ID_LACK;
   }

   r = m_->GetString(L"token", &token_);
   if (false == r) {
     error_code_ = TOKEN_LACK;
   }

   r = m_->GetString(L"code", &code_);
   if (false == r) {
     error_code_ = STOCK_CODE_LACK;
   } else {
     std::string out_code;
     base::BasicUtil::UrlDecode(code_, out_code);
     code_ = out_code.substr(0, out_code.length() - 1);
   }
 }

 int64 user_id() const { return user_id_; }

 std::string token() const { return token_; }

 std::string code() const { return code_;}

private:
 int64 user_id_;

 std::string token_;

 std::string code_;
};

}  //  namespace netcomm_recv

namespace netcomm_send {

class SendMessageHyGn:public SendPacketBase {
 public:
  SendMessageHyGn() {
    stock_.reset(new base_logic::ListValue());
    result_.reset(new NetBase());
  }

  NetBase* release() {
    result_->Set("stock_struct", stock_.release());
    head_->Set("result", result_.release());
    this->set_status(1);
    return head_.release();
  }

  inline void set_SubscribeNum(uint64_t subscribe_num) {
    result_->SetBigInteger("subscribe_num", subscribe_num);
  }

  inline void set_IfSubscribe(uint32_t IfSubscribe) {
    result_->SetBigInteger("if_subscribe", IfSubscribe);
  }

  inline void set_restult(base_logic::DictionaryValue* value) {
    stock_->Append(value);
  }

 private:
  scoped_ptr<base_logic::ListValue> stock_;
  scoped_ptr<NetBase> result_;
};

class SendMessageCode:public SendPacketBase {
 public:
  SendMessageCode() {
    result_.reset(new NetBase());
  }

  NetBase* release() {
    head_->Set("result", result_.release());
    this->set_status(1);
    return head_.release();
  }

  inline void set_SubscribeNum(uint64_t subscribe_num) {
    result_->SetBigInteger("subscribe_num", subscribe_num);
  }

  inline void set_IfSubscribe(uint32_t IfSubscribe) {
    result_->SetBigInteger("if_subscribe", IfSubscribe);
  }

  inline void set_vp(double vp) {
    result_->SetReal("visit_percent", vp);
  }
  inline void set_vip(double vip) {
    result_->SetReal("visit_add", vip);
  }

  inline void set_sp(double sp) {
    result_->SetReal("search_percent", sp);
  }
  inline void set_sip(double sip) {
    result_->SetReal("search_add", sip);
  }

  inline void set_fp(double fp) {
    result_->SetReal("follow_percent", fp);
  }
  inline void set_fip(double fip) {
    result_->SetReal("follow_add", fip);
  }

 private:
  scoped_ptr<NetBase> result_;
};

}  //  namespace netcomm_send
#endif  //  _HOME_RUNNER_NORTHSEA_RESTFUL_PUB_NET_STOCK_COMM_HEAD_H_
