//  Copyright (c) 2015-2015 The restful Authors. All rights reserved.
//  Created on: 2015/11/24 Author: wukun

#include "net/error_comm.h"

const char* buddha_strerror(int error_code) {
  char* error_string = "";
  switch (error_code) {
    case QUERY_DB_FAILED:

        error_string = "查询MySQL数据库失败";
        break;

    case USER_NAME_LACK:

        error_string = "用户名缺失";
        break;

    case SQL_NUM_MORE_THAN_ONE:

        error_string = "查询结果应该小于1";
        break;

    case SUBSCRIBE_PARAM_NULL:

        error_string = "订阅相关参数都为空";
        break;

    case DECODE_FAILED:

        error_string = "解码失败";
        break;

    case NEW_TYPE_ERROR:

        error_string = "指定了不匹配的新闻类型";
        break;

    case HOT_WORDS_PARAM_NULL:

        error_string = "缺少热词关键字";
        break;

    case SEND_MAIL_FAILED:

        error_string = "发送邮件失败";
        break;

    case SUBSCRIBE_DATA_IN_DB:

        error_string = "相关项已经订阅过";
        break;

    case SQL_EXEC_ERROR:

        error_string = "SQL语句执行错误";
        break;

    case USER_ID_LACK:

        error_string = "用户id缺失";
        break;

    case EXEC_REDIS_ERROR:

        error_string = "redis执行出错";
        break;

    case USER_PASSWORD_LACK:

        error_string = "用户密码缺失";
        break;

    case PLATFORM_ID_LACK:

        error_string = "平台id缺失";
        break;

    case HY_NAME_LACK:

        error_string = "行业名缺失";
        break;

    case QUERY_REDIS_FAILED:

        error_string = "查询Redis数据库失败";
        break;

    case TOKEN_LACK:

        error_string = "用户凭据缺失";
        break;

    case TOKEN_OVER_ERROR:

        error_string = "无效的用户凭据";
        break;

    case NULL_DATA:

        error_string = "没有数据";
        break;

    case DB_NOT_EXIST_STOCK:

        error_string = "DB中不存在此股票代码";
        break;

    case REIDS_CONNECT_ERROR:

        error_string = "redis连接失败";
        break;

    case TIME_EMAIL_QUERY_PARAM_NULL:

        error_string = "定时邮件的查询参数为空";
        break;

    case USER_PASSWORD_ERROR:

        error_string = "用户密码错误";
        break;

    case OPERATOR_CODE_NOT_EXIST:

        error_string = "相关操作码不存在";
        break;

    case PLATFORM_NAME_LACK:

        error_string = "platform_name缺失";
        break;

    case STRUCT_ERROR:

        error_string = "结构错误";
        break;

    case SQL_NO_USER:

        error_string = "没有该用户";
        break;

    case CONNECT_ERROR:

        error_string = "mysql连接失败";
        break;

    case TIME_EMAIL_ELEMENT_NULL:

        error_string = "添加定时邮件的基本参数为空";
        break;

    case SQL_DATA_LACK:

        error_string = "数据库里没有对应的数据";
        break;

    case HAVE_NO_RIGHT:

        error_string = "权限不够";
        break;

    case MESSAGE_LACK:
        error_string = "搜索信息缺失";
        break;

    case USER_NAME_EXIST:
         error_string = "用户名已经存在";
        break;
    case USER_ACCESS_NOT_ENOUGH:
        error_string = "访问权限不够";
        break;
    case USER_TYPE_LACK:
        error_string = "用户类型缺失";
        break;
    case USER_ID_ISNOT_IN_SQL:         
        error_string = "数据库中不存在此user_id";
        break;

    case GN_HY_LACK:
        error_string = "概念或者行业缺失";
        break;
                        
    case STOCK_CODE_LACK:
        error_string = "股票代码缺失";
        break;
    case ACTIVE_URL_INVALID:
        error_string = "激活链接失效";
        break;
    case ACTIVE_URL_ERROR:
        error_string = "激活链接无效";
        break;
    default:
        error_string = "errorcode not define";
        break;
    }
    return error_string;
}
