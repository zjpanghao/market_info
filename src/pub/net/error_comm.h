//  Copyright (c) 2015-2015 The restful Authors. All rights reserved.
//  Created on: 2015/11/24 Author: jiaoyongqing

#ifndef _HOME_RUNNER_NORTHSEA_RESTFUL_PUB_NET_ERROR_COMM_H_
#define _HOME_RUNNER_NORTHSEA_RESTFUL_PUB_NET_ERROR_COMM_H_

enum neterrorcode{
  STRUCT_ERROR = -100,                     // 结构错误
  NULL_DATA = -200,                        // 没有数据
  USER_NAME_EXIST = -1,                      //用户名已经存在
  USER_ACCESS_NOT_ENOUGH = -2,               //用户权限不够
  USER_TYPE_LACK =-3,                      //用户类型缺失
  USER_ID_ISNOT_IN_SQL =-4,                //用户id不存在数据库中
  PLATFORM_ID_LACK = -1001,                // 平台id缺失
  USER_NAME_LACK = -1002,                  // 用户名缺失
  USER_PASSWORD_LACK = -1003,              // 用户密码缺失
  USER_PASSWORD_ERROR = -1004,             // 用户密码错误
  TOKEN_LACK = -1005,                      // 用户凭据缺失
  TOKEN_OVER_ERROR = -1006,                // 无效的用户凭据
  USER_ID_LACK = -1007,                    // 用户id缺失

  MESSAGE_LACK = - 1010,                    // 搜索信息缺失

  SQL_DATA_LACK = -1021,                   // 数据库里没有对应的数据
  SQL_NO_USER = -1022,                     // 没有该用户
  SQL_NUM_MORE_THAN_ONE = -1023,           // 查询结果应该小于1
  SQL_EXEC_ERROR = -1024,                  // SQL语句执行错误
  EXEC_REDIS_ERROR = -1025,                // redis执行出错
  CONNECT_ERROR = -1026,                   // mysql连接失败
  REIDS_CONNECT_ERROR = -1027,             // redis连接失败

  HAVE_NO_RIGHT = 1100,                    // 权限不够

  HOT_WORDS_PARAM_NULL = -1100,            // 缺少热词关键字
  SUBSCRIBE_PARAM_NULL = -1101,            // 订阅相关参数都为空
  SUBSCRIBE_DATA_IN_DB     = -1102,        // 相关项已经订阅过
  SEND_MAIL_FAILED         = -1103,        // 发送邮件失败
  QUERY_DB_FAILED = -1104,                 // 查询MySQL数据库失败
  QUERY_REDIS_FAILED = -1105,              // 查询Redis数据库失败
  DECODE_FAILED   = -1106,                 // 解码失败
  TIME_EMAIL_ELEMENT_NULL = -1107,         // 添加定时邮件的基本参数为空
  TIME_EMAIL_QUERY_PARAM_NULL = -1108,     // 定时邮件的查询参数为空
  OPERATOR_CODE_NOT_EXIST     = -1109,     // 相关操作码不存在

  PLATFORM_NAME_LACK = -1111,              // platform_name缺失
  HY_NAME_LACK = -1115,                    // 行业名缺失
  NEW_TYPE_ERROR = -1116,                  // 指定了不匹配的新闻类型

  DB_NOT_EXIST_STOCK = -1130,              // DB中不存在此股票代码
  STOCK_CODE_LACK = -1140,                 // 股票代码缺失
  GN_HY_LACK = -1141,                    // 概念或者行业缺失

  ACTIVE_URL_INVALID = -1142,                        // 激活链接失效
  ACTIVE_URL_ERROR = -1143,                 // 激活链接无效
};

const char*
buddha_strerror(int error_code);

#endif  //  _HOME_RUNNER_NORTHSEA_RESTFUL_PUB_NET_ERROR_COMM_H_
