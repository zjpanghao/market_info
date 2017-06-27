/*=============================================================================
#    Copyright (c) 2015
#    ShanghaiKunyan.  All rights reserved
#
#    Filename     : /home/wukun/work/plugin/subscribe_comm.h
#    Author       : Sunsolo
#    Email        : wukun@kunyan-inc.com
#    Date         : 2015-12-18 15:33
#    Description  : 
=============================================================================*/

#ifndef _NET_NOTICE_COMM_H
#define _NET_NOTICE_COMM_H

#include <string>

#include "net/http_data.h"
#include "basic/basictypes.h"
#include "net/error_comm.h"
#include "logic/base_values.h"
#include "basic/scoped_ptr.h"

namespace netcomm_recv {

class RecvQuery: public RecvPacketBase {
 public:
  explicit RecvQuery(NetBase* m): RecvPacketBase(m) {
    Init();
  }

  void Reset() {
    RecvPacketBase::Reset();
    Init();
  }

  void Init() {
    bool user_id_exist     = false;

    user_id_exist  = m_->GetBigInteger(L"user_id", &user_id_);
    if (user_id_exist == false ){
      error_code_ = USER_ID_LACK;
    }
  }

  const int64 user_id() const { return user_id_; }

 protected:
  int64    user_id_;
};

class RecvSubscribe: public RecvPacketBase {
 public:
  explicit RecvSubscribe(NetBase* m): RecvPacketBase(m) {
    stock_code_.clear();
    section_.clear();
    industry_.clear();

    Init();
  }

  void Reset() {
    RecvPacketBase::Reset();
    Init();
  }

  void Init() {
    bool sock_exist     = false;
    bool section_exist  = false;
    bool industry_exist = false;

    sock_exist     = m_->GetString(L"stock_code", &stock_code_);
    section_exist  = m_->GetString(L"section", &section_);
    industry_exist = m_->GetString(L"industry", &industry_);
    if (sock_exist == false && section_exist == false && \
        industry_exist == false) {
      error_code_ = SUBSCRIBE_PARAM_NULL;
    }
  }

  std::string& stock_code() { return stock_code_; }
  std::string& section() { return section_; }
  std::string& industry() { return industry_; }

 protected:
  std::string    stock_code_;
  std::string    section_;
  std::string    industry_;
};

class RecvTimeEmail: public RecvPacketBase {
 public:
  explicit RecvTimeEmail(NetBase* m): RecvPacketBase(m) {
    stock_code_.clear();
    section_.clear();
    industry_.clear();

    Init();
  }

  void Reset() {
    RecvPacketBase::Reset();
    Init();
  }

  void Init() {
    bool stock_code_exist = false;
    bool section_exist    = false;
    bool industry_exist   = false;
    bool user_id_exist    = false;
    bool start_time_exist = false;
    bool end_time_exist   = false;
    bool time_inval_exist = false;

    stock_code_exist     = m_->GetString(L"stock_code", &stock_code_);
    section_exist        = m_->GetString(L"section", &section_);
    industry_exist       = m_->GetString(L"industry", &industry_);
    user_id_exist        = m_->GetBigInteger(L"user_id", &user_id_);
    start_time_exist     = m_->GetBigInteger(L"start_time", &start_time_);
    end_time_exist       = m_->GetBigInteger(L"end_time", &end_time_);
    time_inval_exist     = m_->GetBigInteger(L"time_inval", &time_inval_);
    if(false == user_id_exist || false == start_time_exist || false == end_time_exist || false == time_inval_exist) {
      error_code_ = TIME_EMAIL_ELEMENT_NULL;
    }else if(stock_code_exist == false && section_exist == false && industry_exist == false) {
      error_code_ = TIME_EMAIL_QUERY_PARAM_NULL;
    }
  }

  int64        user_id() { return user_id_; }
  int64        start_time() { return start_time_; }
  int64        end_time() { return end_time_; }
  int32        time_inval() { return time_inval_;}
  std::string& stock_code() { return stock_code_; }
  std::string& section() { return section_; }
  std::string& industry() { return industry_; }

 protected:
  int64          user_id_;
  int64          start_time_;
  int64          end_time_;
  int64          time_inval_;
  std::string    stock_code_;
  std::string    section_;
  std::string    industry_;
};


class RecvAddSubscribe: public RecvSubscribe {
 public:
  explicit RecvAddSubscribe(NetBase* m): RecvSubscribe(m), user_id_(0) {
    Init();
  }

  void Reset() {
    RecvPacketBase::Reset();
    Init();
  }

  void Init() {
    bool user_id_exist = false;

    user_id_exist = m_->GetBigInteger(L"user_id", &user_id_);
    if (user_id_exist == false) {
      error_code_ = USER_ID_LACK;
    }
  }

  const int64 user_id() const { return user_id_; }

 protected:
  int64 user_id_;
};

}  // namespace netcomm_recv

namespace netcomm_send {
class SendSubscribe: public SendPacketBase {
 public:
  SendSubscribe() {
  }

  virtual ~SendSubscribe() {}

  NetBase* release() {
    head_->Set(L"result", result.release());
    return head_.release();
  }

  inline void set_result(const bool result) {
    this->result.reset(new base_logic::FundamentalValue(result));
  }

 protected:
  scoped_ptr<base_logic::FundamentalValue> result;
};

class SendTimeEmail: public SendPacketBase {
 public:
  SendTimeEmail() {
  }

  virtual ~SendTimeEmail() {}

  NetBase* release() {
    head_->Set(L"result", result.release());
    return head_.release();
  }

  inline void set_result(const bool result) {
    this->result.reset(new base_logic::FundamentalValue(result));
  }

 protected:
  scoped_ptr<base_logic::FundamentalValue> result;
};


class SendSection: public SendPacketBase {
 public:
  SendSection() {
    all_section_count_.reset( new base_logic::ListValue());
  }

  virtual ~SendSection() {}

  NetBase* release() {
    head_->Set(L"section", all_section_count_.release());
    this->set_status(1);
    return head_.release();
  }

  inline void add_section_count(base_logic::DictionaryValue* value) {
    all_section_count_->Append(value);
  }

 protected:
  scoped_ptr<base_logic::ListValue> all_section_count_;
};


class SendIndustry: public SendPacketBase {
 public:
  SendIndustry() {
    all_industry_count_.reset( new base_logic::ListValue());
  }

  virtual ~SendIndustry() {
  }

  NetBase* release() {
    head_->Set(L"industry", all_industry_count_.release());
    this->set_status(1);
    return head_.release();
  }

  inline void add_industry_count(base_logic::DictionaryValue* value) {
    all_industry_count_->Append(value);
  }

 protected:
  scoped_ptr<base_logic::ListValue> all_industry_count_;
};

class SendAllSubscribe: public SendPacketBase {
 public:
  SendAllSubscribe() {
    subscribe_stock_.reset(new base_logic::ListValue());
    subscribe_section_.reset(new base_logic::ListValue());
    subscribe_industry_.reset(new base_logic::ListValue());
  }

  virtual ~SendAllSubscribe() {
  }

  NetBase* release() {
    head_->Set(L"result", result.release());
    head_->Set(L"stock_struct", subscribe_stock_.release());
    head_->Set(L"section", subscribe_section_.release());
    head_->Set(L"industry", subscribe_industry_.release());
    this->set_status(1);
    return head_.release();
  }

  inline void add_stock(base_logic::DictionaryValue* value) {
    subscribe_stock_->Append(value);
  }

  inline void add_section(base_logic::DictionaryValue* value) {
    subscribe_section_->Append(value);
  }

  inline void add_industry(base_logic::DictionaryValue* value) {
    subscribe_industry_->Append(value);
  }

  inline void set_result(const bool result) {
    this->result.reset(new base_logic::FundamentalValue(result));
  }

 protected:
  scoped_ptr<base_logic::FundamentalValue> result;
  scoped_ptr<base_logic::ListValue> subscribe_stock_;
  scoped_ptr<base_logic::ListValue> subscribe_section_;
  scoped_ptr<base_logic::ListValue> subscribe_industry_;
};

class SendStock: public SendPacketBase {
 public:
  SendStock() {
    top_stock_count_.reset( new base_logic::ListValue());
  }

  virtual ~SendStock() {
  }

  NetBase* release() {
    head_->Set(L"stock_struct", top_stock_count_.release());
    this->set_status(1);
    return head_.release();
  }

  inline void add_stock_count(base_logic::DictionaryValue* value) {
    top_stock_count_->Append(value);
  }

 protected:
  scoped_ptr<base_logic::ListValue> top_stock_count_;
};

class SendTopStock: public SendPacketBase {
 public:
  SendTopStock() {
    top_stock_info_.reset( new base_logic::DictionaryValue());
  }

  virtual ~SendTopStock() {
  }

  NetBase* release() {
    head_->Set(L"top_stock", top_stock_info_.release());
    this->set_status(1);
    return head_.release();
  }

  inline void set_stock_name(const std::string& stock_name) {
    top_stock_info_->SetString("stock_name", stock_name);
  }

  inline void set_search_rate(double search_rate) {
    top_stock_info_->SetReal("search_rate", search_rate);
  }

  inline void set_visit_rate(double visit_rate) {
    top_stock_info_->SetReal("visit_rate", visit_rate);
  }

  inline void set_follow_rate(double follow_rate) {
    top_stock_info_->SetReal("follow_rate", follow_rate);
  }

 protected:
  scoped_ptr<base_logic::DictionaryValue> top_stock_info_;
};

class SendTopWord: public SendPacketBase {
 public:
  SendTopWord() {
    top_word_info_.reset( new base_logic::DictionaryValue());
  }

  virtual ~SendTopWord() {
  }

  NetBase* release() {
    head_->Set(L"top_word", top_word_info_.release());
    this->set_status(1);
    return head_.release();
  }

  inline void set_word_name(const std::string& word_name) {
    top_word_info_->SetString("word_name", word_name);
  }

  inline void set_hot_rate(const std::string& hot_rate) {
    top_word_info_->SetString("hot_rate", hot_rate);
  }

 protected:
  scoped_ptr<base_logic::DictionaryValue> top_word_info_;
};
}  // namespace netcomm_send

#endif



