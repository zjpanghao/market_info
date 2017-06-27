//  Copyright (c) 2015-2015 The restful Authors. All rights reserved.
//  Created on: 2015/11/24 Author: jiaoyongqing

#ifndef _HOME_RUNNER_NORTHSEA_RESTFUL_PUB_TOOLS_TOOLS_H_
#define _HOME_RUNNER_NORTHSEA_RESTFUL_PUB_TOOLS_TOOLS_H_

#include<time.h>

#include<string>
#include <algorithm>
#include <typeinfo>
#include <cctype>
#include <map>
#include <vector>

#include "net/typedef.h"
#include "logic/logic_comm.h"

namespace tools {

//  tea加密
void NumToChar(void *d, size_t l, std::string &token);
size_t CharToNum(void **d, std::string &token);
std::string TeaEncode(const std::string src);
std::string GetToken(int64 user_id, std::string &token);
bool CheckToken(int64 user_id, std::string &token);
bool CheckUserIdAndToken(int64 user_id, std::string &token, int socket);
bool check_id_token(int64 user_id, std::string &token, int socket);

  //  字符串转时间戳
time_t StrToTime(const char *Data);

  //  获取时间戳点钟是几点
int get_time_hour(time_t time_);

  //  得到省份字符串
std::string GetProvinceString(int province);

  //  时间戳转字符串
std::string GetTimeKey(time_t time);

  //  获取今天的0点的时间
time_t TodayStartTime();

  //  获得当前时间戳
time_t CurrentTime();

bool isAllDigit(const std::string& str);

  //  在字符串str区间[start, start + len - 1]中，ch字符第num次出现的位置。
std::string::size_type FindNth(const std::string &str, \
                         std::string::size_type start, \
                                              int len, \
                                              char ch, \
                                              int num);
  //  解密token并检测是否过期
bool CheckToken(int64 user_id, const std::string &token);

  //  得到集合set_two中有，集合set_one中没有的。
std::string DeleteSet(const std::string &set_one, \
                      const std::string &set_two, \
                                  char separator);


  //  判断set_one是否在set_two中，在返回true。
bool IfSetOneIsInSetTwo(const std::string &set_one, \
                        const std::string &set_two, \
                                    char separator);
  //  合并两个集合
std::string MergeSet(const std::string &set_one, \
                     const std::string &set_two, \
                                 char separator);

void MapAdd(std::map<std::string, int64> *map, \
const std::string &key, int64 value);

  //  以separator为分隔符将列表l中的元素连接起来,
  //  并且group_size个元素为一组放到列表out中。
void ListGroup(const ContainerStr &l, \
int group_size, char separator, ContainerStr *const out);

int64 GetMapStrToint64Value(std::map<std::string, int64> *m, std::string &str);

//  以separator为分隔符拆分字符串，拆分后放到列表l中,
//  repeat = true:列表里允许重复，repeat = false:列表里不允许重复
template<class T>
void SeparatorStr(const std::string &instr, \
char separator, T *l, bool repeat = true) {
  std::string str(instr);

  if (str == "") return;

  str = std::string(1, separator) + str;

  if (str[str.length() - 1] != separator) {
    str = str + std::string(1, separator);
  }

  MapStrToInt64 map_repeat;
  map_repeat.clear();
  MapStrToInt64::iterator tmp_it;
  std::string value;
  std::string::size_type start = 0;
  std::string::size_type end = str.find(separator, start + 1);
  while (end != std::string::npos) {
    if (end - start == 1) {
      value = "";
    } else {
      value = str.substr(start + 1, end - (start + 1));
    }

    start = end;
    end = str.find(separator, start + 1);

    if (repeat == false) {
      tmp_it = map_repeat.find(value);
      if (tmp_it == map_repeat.end()) {
        map_repeat[value] = 1;
      } else {
        continue;
      }
    }

    l->push_back(value);
  }
}

template<class M, class L, class R>
bool MapFind(M map, L key, R *value) {
  if (map.find(key) == map.end()) {
    return false;
  } else {
    *value = map[key];
    return true;
  }
}

//  最大堆和最小堆
template <class T, class F>
void HandleHeap(T min_heap[], \
int *const heap_count, const T &temp, int heap_max_num) {
  if (*heap_count < heap_max_num) {
    min_heap[*heap_count] = temp;
    ++(*heap_count);
    std::push_heap(&min_heap[0], &min_heap[*heap_count], F());
  } else {
    if (F()(temp, min_heap[0])) {
      std::pop_heap(&min_heap[0], &min_heap[heap_max_num], F());
      min_heap[heap_max_num - 1] = temp;
      std::push_heap(&min_heap[0], &min_heap[heap_max_num], F());
    }
  }
}

//  分割字符串
////  @param str 要分割的字符串
////  @param patttern 分割符
////  @return 分割结果
std::vector<std::string> Split(std::string str, std::string pattern);

////  格式化时间输出
////  @param time unix时间戳
////  @param format 输出格式
////  @return
std::string TimeFormat(int64 time, const char* format);

// 字符窜替换函数
// 将"12212"这个字符串的所有"12"都替换成"21"
// replace_all_distinct：执行结果为：21221
// replace_all：执行结果为：22211
// 即replace_all_distinct只扫描一次字符窜，发现目标就替换
// replace_all：替换一遍后，发现字符窜中还有目标,
// 会继续替换，知道字符窜中没有目标
void replace_all(std::string *str, \
const std::string &old_value, const std::string &new_value);

void replace_all_distinct(std::string *str, \
const std::string &old_value, const std::string &new_value);

//去除字符窜中的空白字符
void ReplaceBlank(std::string *str);
}  // namespace tools

#endif  //  _HOME_RUNNER_NORTHSEA_RESTFUL_PUB_TOOLS_TOOLS_H_
