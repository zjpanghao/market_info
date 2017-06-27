//  Copyright (c) 2015-2015 The comm_struct.h Authors. All rights reserved.
//  Created on: 2016年1月7日 Author: paco

#ifndef PUB_NET_COMM_STRUCT_H_
#define PUB_NET_COMM_STRUCT_H_
const std::string active_url_ = "http://jindowin.iwookong.com/activateaccount.php";
struct PluginInfo {
  char* id;
  char* name;
  char* version;
  char* provider;
};

struct UserInfo {
  int id;
  std::string email;
  std::string passwd;
  int level;
};
#endif /* PUB_NET_COMM_STRUCT_H_ */
