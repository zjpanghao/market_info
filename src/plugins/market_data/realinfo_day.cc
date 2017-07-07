#include "realinfo_day.h"
#include <stdio.h>
#include <fstream>
#include <streambuf>
#include "logic/logic_comm.h"
#define STOCK_MARKET_CLOSE_FILE   "./market/stock_close_date.txt"
namespace stock_price {
RealInfoDay* RealInfoDay::instance_ = NULL;
RealInfoDay::RealInfoDay() {
  pthread_mutex_init(&mutex_, NULL);
  LoadFromFile();
}

RealInfoDay* RealInfoDay::GetInstance() {
  if (instance_ == NULL) {
    instance_ = new RealInfoDay();
  }
  return instance_;
}

void RealInfoDay::SplitString(const std::string& src, std::set<std::string> *dest, const std::string& flag) {
  std::string::size_type pos1, pos2;
  pos2 = src.find(flag);
  pos1 = 0;
  while(std::string::npos != pos2) {
    dest->insert(src.substr(pos1, pos2 - pos1));
    pos1 = pos2 + flag.size();
    pos2 = src.find(flag, pos1);
  }
  if(pos1 != src.length())
    dest->insert(src.substr(pos1));
}

bool RealInfoDay::LoadFromFile() {
  std::ifstream inputFile(STOCK_MARKET_CLOSE_FILE);
  std::string file_data((std::istreambuf_iterator<char>(inputFile)),
                        std::istreambuf_iterator<char>());
  pthread_mutex_lock(&mutex_);
  close_day_.clear();
  SplitString(file_data, &close_day_, "\n");
  pthread_mutex_unlock(&mutex_);
  LOG_MSG2("The market close file %s", file_data.c_str());
  return true;
}

}
