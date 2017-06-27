//  Copyright (c) 2015-2015 The http_data.cc Authors. All rights reserved.
//  Created on: 2016年1月12日 Author: paco

#include "net/http_data.h"

RecvPacketBase::RecvPacketBase(NetBase* m)
  : m_(m)
  , type_(0)
  , error_code_(0) {
  Reset();
}

void RecvPacketBase::Reset() {
  bool r = false;
  r = m_->GetBigInteger(L"type", &type_);
  r = m_->GetString(L"callback", &str_jsonp_);
  if (!r)
    str_jsonp_ = "";
}

RecvLoginBase::RecvLoginBase(NetBase* m):RecvPacketBase(m) {
  Init();
}

void RecvLoginBase::Init() {
  bool r = false;
  do {
    r = m_->GetBigInteger(L"user_id", &user_id_);
    if (!r) {
     error_code_ = USER_ID_LACK;
     break;
    }

    r = m_->GetString(L"token", &token_);
    if (!r) {
     error_code_ = TOKEN_LACK;
     break;
    }
  }while (0);
}



void SendPacketBase::GetJsonSerialize(std::string* json, int ntype) {
  base_logic::ValueSerializer* engine = \
    base_logic::ValueSerializer::Create(ntype, json);

  base_logic::Value* value = (base_logic::Value*)this->release();
  engine->Serialize(*value);
  free(engine);
}
