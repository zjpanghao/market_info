// Copyright (c) 2015-2015 The restful Authors. All rights reserved.
// Created on: 2015/11/24 Author: jiaoyongqing

#include <string>
#include <sstream>
#include <map>
#include <list>

#include "tools/tools.h"
#include "tea/tea.h"
#include "net/typedef.h"
#include "net/error_comm.h"
#include "logic/logic_unit.h"
#include "logic/logic_comm.h"

namespace tools {

  // tea 加密

void NumToChar(void *d, size_t l, std::string &token) {
  std::stringstream os;
  char *p = reinterpret_cast<char *>(d);
  int temp;
  for (int i = 0; i < l; ++i) {
    temp = p[i];
    os << temp << ",";
  }
  token = os.str();
}

size_t CharToNum(void **d, std::string &token) {
  ContainerStr out;
  tools::SeparatorStr<ContainerStr>(token, ',', &out, true);

  *d = reinterpret_cast<void *>(malloc(out.size()));
  char *p = reinterpret_cast<char*>(*d);
  for (int i = 0; i < out.size(); ++i) {
    p[i] = atoi(out[i].c_str());
  }

  return out.size();
}

std::string TeaEncode(const std::string src) {
  LOG_DEBUG2("encode before: %s", src.c_str());
  int src_len = src.length();
  int len = ((src.length() - 1) / 8 + 1) * 8;
  char *in = reinterpret_cast<char*>(malloc(len));
  memset(in, 0, len);
  strncpy(in, src.c_str(), src_len);

  struct tea_data td;
  td.d = reinterpret_cast<void *>(in);
  td.l = len;

  StrEn(&td);
  std::string des;
  NumToChar(td.d, td.l, des);
  free(in);
  LOG_DEBUG2("encode after:%s", des.c_str());
  return des;
}

std::string TeaDecode(const std::string src) {
  struct tea_data td;
  std::string temp_src(src);
  td.l = CharToNum(&td.d, temp_src);
  StrDe(&td);

  std::string temp("");
  for (int i = 0; i < td.l; ++i) {
      temp.append(1, (reinterpret_cast<char*>(td.d))[i]);
  }
  temp.append(1, '\0');
  LOG_DEBUG2("decode after:%s", temp.c_str());
  free(td.d);
  return temp;
}

std::string GetToken(int64 user_id, std::string &token) {
  std::stringstream os;
  std::string cur_token;
  std::string temp;
  os.str("");
  os << user_id;
  os << ",";
  os << time(NULL);
  cur_token = os.str();

  LOG_DEBUG2("\n\norigin token: %s\n\n", cur_token.c_str());

  int len = ((cur_token.length() - 1) / 8 + 1) * 8;
  char *in = reinterpret_cast<char*>(malloc(len));
  memset(in, 0, len);
  strncpy(in, cur_token.c_str(), cur_token.length());

  struct tea_data td;
  td.d = reinterpret_cast<void *>(in);
  td.l = len;

  StrEn(&td);
  NumToChar(td.d, td.l, token);
  free(in);
  return token;
}

bool CheckToken(int64 user_id, std::string &token) {
  struct tea_data td;
  td.l = CharToNum(&td.d, token);
  StrDe(&td);

  std::string origin_token("");
  for (int i = 0; i < td.l; ++i) {
    origin_token.append(1, (reinterpret_cast<char*>(td.d))[i]);
  }
  origin_token.append(1, '\0');

  std::string::size_type separator_pos = origin_token.find(',', 0);
  std::string origin_id = origin_token.substr(0, separator_pos);

  std::stringstream os;
  os.str("");
  os << origin_token.substr(separator_pos + 1, origin_token.length());
  int64 origin_time;
  os >> origin_time;

  os.str("");
  os << user_id;
  std::string current_id = os.str();

  LOG_DEBUG2("\n\norigin token: %s,%d\n\n", origin_id.c_str(), origin_time);

  int64 current_time = time(NULL);

  const int TOKEN_SURVIVE_TIME = 86400;
  if (origin_id == current_id && (current_time - origin_time <= 86400)) {
    return true;
  }
  return false;
}

bool check_id_token(int64 user_id, std::string &token, int socket) {
  int error_code = 0;

  if (CheckToken(user_id, token)) {
    return true;
  } else {
    error_code = HAVE_NO_RIGHT;
    send_error(error_code, socket);
    return false;
  }

  return false;
}

bool CheckUserIdAndToken(int64 user_id, std::string &token, int socket) {
  if (!tools::check_id_token(user_id, token, socket)) {
    return false;
  }

  return true;
}

bool isAllDigit(const std::string& str) {
  int i;
  for (i = 0; i != str.length(); i++) {
    if (!isdigit(str[i])) {
      return false;
    }
  }
  return true;
}

//  时间函数

//获取时间戳点钟是几点
int get_time_hour(time_t time_) {
  const time_t t = time(NULL);
  time_t  s = time_;
  struct tm current_time = {0};
  localtime_r(&s, &current_time);
  return current_time.tm_hour;
}

//  时间戳转字符串时间
std::string GetTimeKey(int64 time) {
  struct tm timeTm;
  int64 s = time;
  localtime_r(&s, &timeTm);
  char s_char[32];
  memset(s_char, '\0', sizeof(s_char));
  snprintf(s_char, sizeof(s_char),
             "%4d-%02d-%02d %02d",
              timeTm.tm_year+1900,
                  timeTm.tm_mon+1,
                   timeTm.tm_mday,
                  timeTm.tm_hour);
  std::string str_time = s_char;
  return str_time;
}

std::string GetProvinceString(int province) {
  switch (province) {
    case 1 :
      return "jsdx:";
    case 2:
      return "shdx:";
    case 3:
      return "zjdx:";
  }
  return "";
}

int64 StrToTime(const char *Data) {
  struct tm* tmp_time = (struct tm*)malloc(sizeof( struct tm ));
  strptime(Data, "%Y-%m-%d %H", tmp_time);
  tmp_time->tm_min = 0;
  tmp_time->tm_sec = 0;
  time_t t = mktime(tmp_time);
  free(tmp_time);

  return t;
}

int64 TodayStartTime() { return time(NULL) - (time(NULL) + 28800) % 86400; }

int64 CurrentTime() { return time(NULL); }

  //  集合的形式为：a,b,c,d,
std::string MergeSet(const std::string &set_one, \
                     const std::string &set_two, \
                                 char separator) {
  if (set_one == "") return set_two;
  if (set_two == "") return set_one;

  std::string ret(set_two);
  if (ret[ret.length() - 1] != separator) {
    ret = ret + std::string(1, separator);
  }

  std::list<std::string> set_one_list;
  SeparatorStr(set_one, ',', &set_one_list);
  std::list<std::string>::iterator it = set_one_list.begin();
  for (; it != set_one_list.end(); ++it) {
    if (set_two.find((*it).c_str()) == std::string::npos) {
      ret += *it;
      ret += std::string(",");
    }
  }

  return ret;
}

void ListGroup(const ContainerStr &l, \
int group_size, char separator, ContainerStr *const out) {
  ContainerStr::const_iterator it = l.begin();
  int i = 0;
  std::string value("");
  for (; it != l.end(); ++it) {
    value += *it;
    value += std::string(1, separator);
    ++i;
    if (i == group_size) {
      out->push_back(value);
      value = "";
      i = 0;
    }
  }

  if (value != "") {
    out->push_back(value);
  }
}

bool IfSetOneIsInSetTwo(const std::string &set_one, \
                        const std::string &set_two, \
                                    char separator) {
  if (set_one == "") return true;
  if (set_two == "") return false;

  std::list<std::string> set_one_list;
  SeparatorStr(set_one, ',', &set_one_list);
  std::list<std::string>::iterator it = set_one_list.begin();
  for (; it != set_one_list.end(); ++it) {
    if (set_two.find((*it).c_str()) == std::string::npos) {
      return false;
    }
  }

  return true;
}

std::string DeleteSet(const std::string &set_one, \
                      const std::string &set_two, \
                                  char separator) {
  if (set_one == "" || set_two == "") return set_two;

  std::string ret("");
  std::list<std::string> set_two_list;
  SeparatorStr(set_two, ',', &set_two_list);
  std::list<std::string>::iterator it = set_two_list.begin();
  for (; it != set_two_list.end(); ++it) {
    if (set_one.find((*it).c_str()) == std::string::npos) {
      ret += *it;
      ret += std::string(",");
    }
  }

  return ret;
}

std::string::size_type FindNth(const std::string &str, \
                         std::string::size_type start, \
                                              int len, \
                                              char ch, \
                                              int num) {
  if (num == 0) return -1;

  std::string::size_type end = str.find(ch, start);
  int count = 0;
  int cur_len = end + 1 - start;
  while (true) {
    if (end == std::string::npos) break;
    if (cur_len > len) break;

    ++count;
    if (cur_len == len) break;
    if (count == num )break;
    start = end + 1;
    end = str.find(ch, start);
    cur_len = end + 1 - start;
  }

  if (count < num) { return -1; }

  return end;
}

void MapAdd(std::map<std::string, \
int64> *map, const std::string &key, int64 value) {
  std::map<std::string, int64>::iterator it;
  it = map->find(key);
  if (it == map->end()) {
    (*map)[key] = value;
  } else {
    (*map)[key] += value;
  }
}

int64 GetMapStrToint64Value(std::map<std::string, int64> *m, std::string &str) {
  int64 ret;
  if (m->find(str) != m->end()) {
    ret = (*m)[str];
  } else {
    ret = 0;
  }
  return ret;
}

std::string TimeFormat(int64 time, const char* format) {
  struct tm timeTm;
  localtime_r(&time, &timeTm);
  char s_char[32];
  memset(s_char, '\0', sizeof(s_char));
  snprintf(s_char, sizeof(s_char),
            format,
                  timeTm.tm_year+1900,
                        timeTm.tm_mon+1,
                              timeTm.tm_mday,
                                    timeTm.tm_hour);
  std::string str_time = s_char;
  return str_time;
}

std::vector<std::string> Split(std::string str, std::string pattern) {
  std::string::size_type pos;
  std::vector<std::string> result;
  str += pattern;
  int size = str.size();
  for (int i = 0; i < size; i++) {
      pos = str.find(pattern, i);
      if (pos < size) {
            std::string s = str.substr(i, pos - i);
            result.push_back(s);
            i = pos + pattern.size() - 1;
          }
    }
  return result;
}

void replace_all(std::string *str, \
const std::string &old_value, const std::string &new_value) {
  while (true) {
    std::string::size_type pos(0);
    if ((pos = str->find(old_value)) != std::string::npos)
      str->replace(pos, old_value.length(), new_value);
    else
      break;
  }
}

void replace_all_distinct(std::string *str, \
const std::string &old_value, const std::string &new_value) {
  for (std::string::size_type pos(0); \
  pos != std::string::npos; pos += new_value.length()) {
    if ((pos = str->find(old_value, pos)) != std::string::npos)
      str->replace(pos, old_value.length(), new_value);
    else
      break;
  }
}

void ReplaceBlank(std::string *str) {
  //  去除空格
  replace_all_distinct(str, " ", "");

  //  去除\t
  replace_all_distinct(str, "\t", "");

  //  去除\n
  replace_all_distinct(str, "\n", "");
}

}  //  namespace tools

