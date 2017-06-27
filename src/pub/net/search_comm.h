//  Copyright (c) 2015-2015 The restful Authors. All rights reserved.
//  Created on: 2015/11/24 Author: jiaoyongqing

#ifndef _HOME_RUNNER_JINDOWIN_PUB_NET_SEARCH_COMM_H_
#define _HOME_RUNNER_JINDOWIN_PUB_NET_SEARCH_COMM_H_

#include <string>

#include "net/http_data.h"
#include "basic/basictypes.h"
#include "logic/base_values.h"
#include "net/error_comm.h"
#include "net/typedef.h"

namespace netcomm_recv {
class RecvSearch:public RecvPacketBase{
 public:
  explicit RecvSearch(NetBase* m)
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

    r = m_->GetString(L"message", &message_);
    if (false == r) {
      error_code_ = MESSAGE_LACK;
    } else {
      std::string out_message;
      base::BasicUtil::UrlDecode(message_, out_message);
      message_ = out_message.substr(0, out_message.length() - 1);
    }
  }

  int64 user_id() const { return user_id_; }

  std::string token() const { return token_; }

  std::string message() const { return message_;}

 private:
  int64 user_id_;

  std::string token_;

  std::string message_;
};

}  //  namespace netcomm_recv

namespace netcomm_send {
class SendSearch:public SendPacketBase {
 public:
  SendSearch() {
    name_.reset( new base_logic::ListValue() );
    hy_.reset( new base_logic::ListValue() );
    gn_.reset( new base_logic::ListValue() );
  }

  NetBase* release() {
    head_->Set("stock_struct", name_.release());
    head_->Set("hy", hy_.release());
    head_->Set("gn", gn_.release());
    this->set_status(1);
    return head_.release();
  }

  inline void set_name(base_logic::Value* value) {
    name_->Append(value);
  }

  inline void set_hy(base_logic::Value* value) {
    hy_->Append(value);
  }

  inline void set_gn(base_logic::Value* value) {
    gn_->Append(value);
  }

 private:
  scoped_ptr<base_logic::ListValue> name_;
  scoped_ptr<base_logic::ListValue> hy_;
  scoped_ptr<base_logic::ListValue> gn_;
};

}  //  namespace netcomm_send
#endif  //  _HOME_RUNNER_JINDOWIN_PUB_NET_SEARCH_COMM_H_
