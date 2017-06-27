//  Copyright (c) 2015-2015 The george Authors. All rights reserved.
//  Created on: 2016年5月23日 Author: kerry

#ifndef GEORGE_NET_PACKET_PROCESS_H__
#define GEORGE_NET_PACKET_PROCESS_H__

#include "proto_buf.h"
#include "basic/basictypes.h"
#include "logic/base_values.h"

namespace george_logic {

namespace http_packet{

class PacketProcess {
 public:
	PacketProcess();
	virtual ~PacketProcess();
 public:
	bool UnpackPacket(const int socket, const void *msg, const int len, const int8 type,
			void (*packet_process)(const int socket, base_logic::DictionaryValue*,
					george_logic::PacketHead*));

 private:
	base_logic::ValueSerializer*         serializer_;
};

}

namespace json_packet{
class PacketProcess {
 public:
	PacketProcess();
	virtual ~PacketProcess();
 public:
	void PackPacket (const int socket, base_logic::DictionaryValue* value);
 private:
	base_logic::ValueSerializer*         serializer_;
};
}

namespace jsonp_packet {
class PacketProcess {
 public:
	PacketProcess();
	virtual ~PacketProcess();
 public:
	void PackPacket (const int socket, base_logic::DictionaryValue* value);
 private:
	base_logic::ValueSerializer*          serializer_;
};

}
}

#endif
