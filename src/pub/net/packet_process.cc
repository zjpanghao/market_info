//  Copyright (c) 2015-2015 The KID Authors. All rights reserved.
//  Created on: 2016年5月23日 Author: kerry
#include "packet_process.h"
#include "proto_buf.h"
#include "logic/logic_comm.h"
#include "basic/scoped_ptr.h"

namespace george_logic {

namespace http_packet {

PacketProcess::PacketProcess() {
	serializer_ = base_logic::ValueSerializer::Create(base_logic::IMPL_HTTP);
}

PacketProcess::~PacketProcess() {
	if (serializer_){delete serializer_; serializer_ = 	NULL;}
}

bool PacketProcess::UnpackPacket(const int socket, const void *msg, const int len, const int8 type,
		void (*packet_process)(const int socket, base_logic::DictionaryValue*,
				george_logic::PacketHead*)) {

    std::string error_str;
    int error_code = 0;
	std::string packet_stream((const char*)msg,len);
	if (len <= 0 || socket < 0 || msg == NULL )
		return false;
	LOG_MSG2("packet_stream=%s", packet_stream.c_str());
	base_logic::Value* value = serializer_->Deserialize(&packet_stream,
			&error_code, &error_str);

	if (value == NULL)
		return false;

	george_logic::PacketHead* packet = new george_logic::PacketHead;

	base_logic::DictionaryValue* dict = (base_logic::DictionaryValue*)value;
	packet->set_http_head(dict);
	//通过type判断数据什么packet
	if (packet->type() == type)
		packet_process(socket, dict, packet);
	return true;
}


}

namespace json_packet{
PacketProcess::PacketProcess() {
	serializer_ = base_logic::ValueSerializer::Create(base_logic::IMPL_JSON);
}

PacketProcess::~PacketProcess(){
	if(serializer_){delete serializer_; serializer_ = NULL;}
}

void PacketProcess::PackPacket (const int socket, base_logic::DictionaryValue* value) {
	std::string json;
	bool r = serializer_->Serialize(*value, &json);
	if (!r)
		return;
	LOG_MSG2("%s",json.c_str());
	base_logic::LogicComm::SendFull(socket, json.c_str(),json.length());
}

}



namespace jsonp_packet {

PacketProcess::PacketProcess() {
	serializer_ = base_logic::ValueSerializer::Create(base_logic::IMPL_JSONP);
}

PacketProcess::~PacketProcess() {
	if(serializer_){delete serializer_; serializer_ = NULL;}
}

void PacketProcess::PackPacket (const int socket, base_logic::DictionaryValue* value) {
	std::string json;
	bool r = serializer_->Serialize(*value, &json);
	if (!r)
		return;
	LOG_MSG2("%s",json.c_str());
	base_logic::LogicComm::SendFull(socket, json.c_str(),json.length());
}

}


}
