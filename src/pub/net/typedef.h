//  Copyright (c) 2015-2015 The restful Authors. All rights reserved.
//  Created on: 2015/11/24 Author: jiaoyongqing

#ifndef _HOME_RUNNER_NORTHSEA_RESTFUL_PUB_NET_TYPEDEF_H_
#define _HOME_RUNNER_NORTHSEA_RESTFUL_PUB_NET_TYPEDEF_H_

#include <vector>
#include <map>
#include <string>
#include "basic/basictypes.h"
#include "net/comm_struct.h"

typedef std::map<std::string, std::string> MapStrToStr;
typedef std::map<std::string, int64> MapStrToInt64;


typedef std::vector<std::string> ContainerStr;

typedef std::vector<int64> ContainerInt;


#define REDIS_TIME  53

#define REDIS_VISIT "visit"

#define REDIS_SEARCH "search"

#define REDIS_FOLLOW "follow"

#define REDIS_COUNT "count"

enum Rtype {VISIT = 1, SEARCH, FOLLOW};

#define REDIS_DELAY (3 * 60 * 60)

#endif  //  _HOME_RUNNER_NORTHSEA_RESTFUL_PUB_NET_TYPEDEF_H_
